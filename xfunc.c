/*
 * xfunc.c: X-Fractals / routines to create and manage X-Window display
 *
 * Authored by Parmjit Virk (2017)
 * 
 * Licensed under the MIT license as per the Open Source Initiative 2017.
 * See the LICENSE file for the complete license information,
 * or visit https://opensource.org/licenses/MIT for details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include "Xfractals.h"

/* Define local function prototypes ... */
void drawHotSpot(Display *, int, Window *, GC *, int, int, int, int);

/* 
  Function openDisplay
    -> Open a connection to the local display (:0)
*/
Display * openDisplay(void)
  {
    return (XOpenDisplay((char *)0));
  }

/* 
  Function closeDisplay
    -> Close the connection to the passed display
*/
void closeDisplay(Display *display)
  {
    XCloseDisplay(display);
    return;
  }

/* 
  Function getScreen
    -> Return the value of the local (default) screen
*/
int getScreen(Display *display)
  {
    return (DefaultScreen(display));
  }

/* 
  Function createWindow
    -> Create new window for user input
*/
void createWindow
 (Display *display, int screen, Window *window, char *title)
  {
    /*
      Declare vars ...
       -> <*visual>, pointer to a window visual structure
       -> <wmDeleteWindow>, atom for the window manager
       -> <winAttrib>, structure containing initial window properties
    */

    Visual *visual;
    Atom   wmDeleteWindow;
    XSetWindowAttributes winAttrib;

    /* 
       Get the default visual for this display and screen ... 
         -> DirectColor/TrueColor type for most modern displays
    */

    visual = DefaultVisual(display, screen);

    /* 
      Set window attributes ...
        -> use default (hardware) colormap, black background, no border
        -> listen only for expose, key and button press/release, and 
           window structure change events
    */

    winAttrib.colormap = DefaultColormap(display, screen);
    winAttrib.background_pixel = BlackPixel(display, screen);
    winAttrib.border_pixel = 0;
    winAttrib.event_mask = ExposureMask | ButtonPressMask | 
                           ButtonReleaseMask | KeyPressMask | 
                           StructureNotifyMask;

    /* 
      Create the new window ...
        -> child of root window (i.e. background window)
        -> using WIDTH/HEIGHT constants defined in <Xfractals.h>
        -> using default depth/planes for this display and screen
        -> type InputOutput so that we can draw into window
    */

    *window = XCreateWindow
            (
              display,
              RootWindow(display, screen), 
              0,
              0,
              WIDTH,
              HEIGHT,
              0,
              DisplayPlanes(display, screen),
              InputOutput,
              visual,
              CWBackPixel | CWBorderPixel | CWColormap | CWEventMask,
              &winAttrib
            );

    /* 
      Set new window standard properties ...
        -> Set protocol to allow window manager to terminate window 
           cleanly via "Close" button (WM_DELETE_WINDOW)
    */

    XSetStandardProperties
      (
        display,
        *window,
        title,
        "Hi!",
        None,
        NULL,
        0,
        NULL
      );
    wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, *window, &wmDeleteWindow, 1);
  }

/* 
  Function showWindow
    -> Show window and wait for user input
*/
void showWindow
 (Display *display, int screen, Window *window, GC *gc, 
  unsigned long fractal_points[][HEIGHT][1], int fractal_type, int fractal_color)
  {
    int px1,
        px2,
        py1,
        py2;

    int continueLoop;

    Atom wmDeleteWindow;

    /* <XEvent>, event structure */

    XEvent event; 

    /* <key>, structure to hold keyboard input info */

    KeySym key;

    /* <keyPress>, char array to store entered keystrokes ... */

    char keyPress[255];

    /* Allow window manager to terminate window cleanly via "Close" button */

    wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);

    /* Map window to screen ... */

    XMapWindow(display, *window);

    /* 
      Begin window event loop ...
        -> terminate on 'q', mouse right-click or winmanager close
    */

    continueLoop = 1;
    while(continueLoop == 1)
      {
        /* 
           Grab next event from event queue ...
             -> process based on <event.type> header
        */

        XNextEvent(display, &event);

        switch(event.type)
          {
            case(Expose):
              /* redraw contents of window ... */
              drawFractal(display, window, gc, fractal_points);
            break;

            case(ButtonPress):
              if (event.xbutton.button == Button1)
                {
                  /* store first left-button click coordinates */
                  px1 = event.xbutton.x;
                  py1 = event.xbutton.y;
                }
              else if (event.xbutton.button == Button3)
                {
                  /* terminate if mouse right-click */
                  continueLoop = 0;
                }
            break;

            case(ButtonRelease):
              if (event.xbutton.button == Button1)
                {
                  /* store second left-button click coordinates */
                  px2 = event.xbutton.x;
                  py2 = event.xbutton.y;

                  if ((px1 != px2) && (py1 != py2))
                    {
                      /* Draw hotspot triangle to mark user region selection */
                      drawHotSpot(display, screen, window, gc, px1, py1, px2, py2);
                    }

                  /* create new data and redraw */

                  createFractal(fractal_type, fractal_color, fractal_points, px1, py1, px2, py2);
                  drawFractal(display, window, gc, fractal_points);
                }
            break;

            case(KeyPress):
              /* 
                convert keypress data into a usable string
                  -> store string in <keyPress> array ...
                       -> weird but necessary ...
              */

              XLookupString(&event.xkey, keyPress, 255, &key, 0);

              if (keyPress[0] == 'q')
                {
                  /* terminate if 'q' key pressed */
                  continueLoop = 0;
                }
            break;

            case(ClientMessage):
              if(event.xclient.data.l[0] == wmDeleteWindow)
                {
                  /* terminate if window manager says so! */
                  continueLoop = 0;
                }
            break;
          }
      }

    /* Free the associated graphics context and destroy window */

    XFreeGC(display, *gc);
    XDestroyWindow(display, *window);

    return;
  }

/* 
  Function createGC
    -> Create and bind a graphics context with a window
*/
void createGC
 (Display *display, Window *window, GC *gc)
  {
    *gc = XCreateGC(display, *window, 0, NULL);
    if (*gc == NULL)
      {
        /* Can't do any drawing, so notify and quit ... */

        printf("Could not set graphics context.\n");
        exit(1);
      }

    return;
  }

/* 
  Function drawHotSpot
    -> Draw a rectangle hotspot highlighting user region selection
*/
void drawHotSpot
 (Display *display, int screen, Window *window, GC *gc, int px1, int py1, int px2, int py2)
  {
    int px_min,
        px_max,
        py_min,
        py_max,
        x_width, 
        y_height;

    /* Figure out pixel value order ... */

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

    /* Set rectangle width and height */

    x_width = (px_max - px_min);
    y_height = (py_max - py_min);

    /* Set outline of rectangle to be white and draw ... */

    XSetForeground(display, *gc, WhitePixel(display, screen));
    XDrawRectangle(display, *window, *gc, px_min, py_min, x_width, y_height);

    /* Force processing of all directives in X buffer ...*/

    XFlush(display);

    return;
  }
