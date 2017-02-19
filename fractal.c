/*
 * fractal.c: X-Fractals / routines to calculate point values and draw fractal image
 *
 * Authored by Parmjit Virk (2017)
 * 
 * Licensed under the MIT license as per the Open Source Initiative 2017.
 * See the LICENSE file for the complete license information,
 * or visit https://opensource.org/licenses/MIT for details.
 *
 */

#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include "Xfractals.h"

/* Define max. number of iterations for each fractal point ... */

#define ITER_MAX 155

/* Define local function prototypes ... */
void getNewBounds(int, int, int, int, int, double *, double *, double *, double *);
void calculateMandelbrot(double, double, double *, double *, double, double, double, double);
void calculateJulia(double, double, double *, double *, double, double, double, double);
void calculateSpiral(double, double, double *, double *, double, double, double, double);

/*
  Function createFractal
   -> Generate/store pixel color data for a given fractal and region ...
*/
void createFractal
 (int fractal_type, unsigned long fractal_points[][HEIGHT][1], 
  int px1, int py1, int px2, int py2)
  {
    /* <*Routine>, pointer to a function body  */

    void    (*Routine)(double, double, double *, double *, double, double, double, double),
            calculateMandelbrot(), 
            calculateJulia(),
            calculateLambda();

    double  dist_max,
            real, 
            imag;

    double  xn, 
            xnew,
            yn, 
            ynew,
            x_inc, 
            y_inc, 
            orig1, 
            orig2,
            dist;

    int     px, 
            py, 
            iter_count;

    /* 
       Retain these doubles in memory even after function terminates! 
         -> static vars
    */

    static double xmin,
                  xmax,
                  ymin,
                  ymax;

    /* 
       Triplet of color data, positive values only!  
         -> unsigned data type
    */

    unsigned long index_r,
                  index_g,
                  index_b;

    /* Set appropriate values based on user fractal choice ... */

    switch(fractal_type) 
      {
        case 1:
          /* Assign the function pointer to a function body */
          Routine = &calculateMandelbrot;
          dist_max = 2.0;
          real = 0.0;
          imag = 0.0;
        break;
        case 2:
          Routine = &calculateJulia;
          dist_max = 2.0;
          real = 0.3;
          imag = 0.6;
        break;
        case 3:
          Routine = &calculateLambda;
          dist_max = 4.0;
          real = 0.85;
          imag = 0.6;
        break;
      }       

    /* Determine fractal bounds ... */

    getNewBounds(fractal_type, px1, py1, px2, py2, &xmin, &ymin, &xmax, &ymax);

    /* 
      Generate fractal color data ...
        -> store in <fractal_points> array
    */

    iter_count = 0; 
    dist = 0;
    xn = xmin;
    yn = ymax;
    orig1 = xmin;
    orig2 = ymax;
    x_inc = ((xmax-xmin)/WIDTH);   
    y_inc = ((ymax-ymin)/HEIGHT);

    for (px = 0 ; px < WIDTH ; px++)
      {
        for (py = 0 ; py < HEIGHT ; py++)
          {
            while ((iter_count <= ITER_MAX) && (dist < dist_max))
              {
                /* Call specified fractal routine */
                Routine(xn, yn, &xnew, &ynew, orig1, orig2, real, imag);

                xn = xnew;
                yn = ynew;
                dist = sqrt((xn*xn)+(yn*yn));
                iter_count++;
              }

            if (dist < dist_max)
              {
                /* Set these points to black */
                index_r = 0;
                index_g = 0;
                index_b = 0;
              }
            else
              {
                /* Set these points to some iterated value */
                index_r = iter_count;
                index_g = iter_count;
                index_b = iter_count;
              }

            /* 
              Build a 24-bit long unsigned value from the color triplets ...
                -> required by a TrueColor visual type to render color!
                -> left shift green index by 8 bits, blue index by 16
            */

            fractal_points[px][py][1] = ((index_r) + (index_g << 8) + (index_b << 16));

            iter_count = 0; 
            dist = 0;
            xn = orig1;
            orig2 = (orig2 - y_inc);
            yn = orig2;
          }

        yn = ymax;
        orig2 = ymax;
        orig1 = (orig1 + x_inc);
        xn = orig1;
      }

    return;
  }

/*
  Function drawFractal
   -> Draw fractal into a given window ...
*/
void drawFractal(Display *display, Window *window, GC *gc, unsigned long fractal_points[][HEIGHT][1])
  {
    int x, y;

    for (x = 0 ; x < WIDTH ; x++)
      {
        for (y = 0 ; y < HEIGHT ; y++)
          {
            /* Set foreground color to <fractal_points> value */

            XSetForeground(display, *gc, fractal_points[x][y][1]);

            /* Color pixel at the specified (x,y) coordinate in window */

            XDrawPoint(display, *window, *gc, x, y);
          }
      }

    /* Force processing of all directives in X buffer ...*/

    XFlush(display);

    return;
  }

/*
  Function getNewBounds
   -> Determine fractal bounds based on user input ...
*/
void getNewBounds
 (int type, int px1, int py1, int px2, int py2, 
  double *xmin, double *ymin, double *xmax, double *ymax)
  {
    int px_min,
        px_max,
        py_min,
        py_max;

    double x_diff,
           y_diff;

    /* If first value is -1 (i.e. first viewing), use the defaults */

    if (px1 == -1)
      {
        if (type == 1)
          {
            *xmin = -2.5;
            *xmax = 1.5;
            *ymin = -1.5;
            *ymax = 1.5;
          }
        else if (type == 2)
          {
            *xmin = -0.241001;
            *xmax = 0.222222;
            *ymin = 0.413542;
            *ymax = 0.760960;
          }
        else
          {
            *xmin = -1.5;
            *xmax = 2.5;
            *ymin = -1.5;
            *ymax = 1.5;
          }
      }
    else if ((px1 != px2) && (py1 != py2))
      {
        /* 
          User has made a region selection for zooming
            -> determine new bound values based on hotspot vertecies
        */

        if (px1 < px2)
          {
            px_min = px1;
            px_max = px2;
          }
        else
          {
            px_min = px2;
            px_max = px1;
          }

        if (py1 < py2)
          {
            py_min = py1;
            py_max = py2;
          }
        else
          {
            py_min = py2;
            py_max = py1;
          }

        x_diff = (((*xmax) - (*xmin)) / WIDTH);
        y_diff = (((*ymax) - (*ymin)) / HEIGHT);

        *xmax = ((*xmin) + (px_max * x_diff));
        *xmin = ((*xmin) + (px_min * x_diff));
        *ymin = ((*ymax) - (py_max * y_diff));
        *ymax = ((*ymax) - (py_min * y_diff));
      }
    else if ((px1 == px2) || (py1 == py2))
      {
        /* 
          User has single-clicked on the same point
            -> shift current fractal view to center on this point
            -> no zooming perfomed!
        */

        x_diff = (((*xmax) - (*xmin)) / WIDTH);
        y_diff = (((*ymax) - (*ymin)) / HEIGHT);

        *xmax = (((*xmin) + (px1 * x_diff)) + ((WIDTH / 2) * x_diff));
        *xmin = (((*xmin) + (px1 * x_diff)) - ((WIDTH / 2) * x_diff));
        *ymin = (((*ymax) - (py1 * y_diff)) - ((HEIGHT / 2) * y_diff));
        *ymax = (((*ymax) - (py1 * y_diff)) + ((HEIGHT / 2) * y_diff));
      }

    return;
  }

/* Algorithms for various fractal types ... */

void calculateMandelbrot
(double xn, double yn, double *xnew, double *ynew, 
 double orig1, double orig2, double real, double imag)
  {
    *xnew = ((xn*xn) - (yn*yn) + orig1);
    *ynew = (2 * xn * yn + orig2); 

    return;
  }

void calculateJulia
(double xn, double yn, double *xnew, double *ynew, 
 double orig1, double orig2, double real, double imag)
  { 
    *xnew = ((xn*xn) - (yn*yn) + real);
    *ynew = (2 * xn * yn + imag);

    return;
  }

void calculateLambda
(double xn, double yn, double *xnew, double *ynew, 
 double orig1, double orig2, double real, double imag)
  {
    *xnew = ((real * xn) - (real * xn * xn) + (real * yn * yn) - (imag * yn) + (2 * imag * xn * yn));
    *ynew = ((real * yn) + (imag * xn) - (imag * xn * xn) + (imag * yn * yn) - (2 * real * xn * yn));

    return;
  }
