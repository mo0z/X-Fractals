/*
 * Xfractals.h: X-Fractals / header file for constants and function prototypes
 *
 * Authored by Parmjit Virk (2017)
 * 
 * Licensed under the MIT license as per the Open Source Initiative 2017.
 * See the LICENSE file for the complete license information,
 * or visit https://opensource.org/licenses/MIT for details.
 *
 */

/* 
  CONSTANTS 
    -> define width and height of new fractal window
*/

#define WIDTH  250
#define HEIGHT 250

/* GENERAL FUNCTION PROTOTYPES */

/* XWindow stuff ... */
Display * openDisplay(void);
void closeDisplay(Display *);
int getScreen(Display *);
void createWindow(Display *, int, Window *, char *);
void showWindow(Display *, int, Window *, GC *, unsigned long [][HEIGHT][1], int);
void createGC(Display *, Window *, GC *);

/* Fractal stuff ... */
void createFractal(int, unsigned long [][HEIGHT][1], int, int, int, int);
void drawFractal(Display *, Window *, GC *, unsigned long [][HEIGHT][1]);
