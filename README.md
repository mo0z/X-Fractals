Hello All,

This application will generate the fractal boundary map for a number of algorithms, based upon the famous Mandelbrot set for complex numbers:

https://en.wikipedia.org/wiki/Mandelbrot_set

To use this application:

1) Build against the Makefile provided and launch the compiled index binary from a terminal within your linux GUI.

2) Select the fractal type from the list of options to launch a new window with the fractal render.

3) Mouse left-click to center the map on a particular point, or left-click-and-drag a rectangle hotspot to zoom into a given region for more detail.

4) Mouse right-click or use the 'q' key to close the window.

This project was built and tested using the linux Debian 8 (jessie) distro with kernel version 3.16.0 and gcc version 4.9.2.  To compile the project from source requires the development headers for the X11 client-side library, namely 'X11/Xlib.h':

https://packages.debian.org/jessie/libx11-dev

This software is available as per the terms of the MIT License - see the LICENSE file for details.

Happy coding!

Parmjit Virk
pvirk[at]mts.net
