/*
 * index.c: X-Fractals / generate and zoom fractal images within a single X-window
 *
 * Authored by Parmjit Virk (2017)
 * 
 * Licensed under the MIT license as per the Open Source Initiative 2017.
 * See the LICENSE file for the complete license information,
 * or visit https://opensource.org/licenses/MIT for details.
 *
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include "Xfractals.h"

int main(void)
  {
    /* 
       Declare local vars ... 
         -> <*display>, pointer to local display
         -> <screen>, local screen number
         -> <window>, window handle
         -> <gc>, drawable graphics handle
    */

    Display *display;
    int     screen;
    Window  window;
    GC      gc;
    char    *title;

    /* <fractal_points[][]>, holds pixel color info for each point */

    int           fractal_type;
    unsigned long fractal_points[WIDTH][HEIGHT][1];

    /* Get display and screen values */

    display = openDisplay();
    screen = getScreen(display);

    printf("\nFractal Type?\n");
    printf("1) Mandelbrot\n");
    printf("2) Julia\n");
    printf("3) Spiral\n\n");
    printf("Enter the number of your choice: ");
    scanf("\n%d", &fractal_type);

    /*  Set window title ... */

    if (fractal_type != 0)
      {
        if (fractal_type == 1)
          {
            title = "Mandelbrot";
          }
        else if (fractal_type == 2)
          {
            title = "Julia";
          }
        else
          {
            fractal_type = 3;
            title = "Spiral";
          }

        /* Populate color array with appropriate fractal data ... */

        createFractal(fractal_type, fractal_points, -1, 0, 0, 0);

        /* Create new window and graphics context to be used ... */

        createWindow(display, screen, &window, title);
        createGC(display, &window, &gc);

        /* Show new window on screen and wait for user input ... */

        showWindow(display, screen, &window, &gc, fractal_points, fractal_type);
      }

    printf("\n*** End Of Processing *** \n\n"); 

    /* Close display and exit program ... */

    closeDisplay(display);
    return (0);
  }
