#
# Makefile for XFractals project
#
# Authored by Parmjit Virk (2017)
# 
# Licensed under the MIT license as per the Open Source Initiative 2017.
# See the LICENSE file for the complete license information,
# or visit https://opensource.org/licenses/MIT for details.
#

index: xfunc.o fractal.o index.c
	gcc -Wall -o index xfunc.o fractal.o index.c -L/usr/X11R6/lib -lX11 -lm

xfunc.o: xfunc.c
	gcc -Wall -c xfunc.c

fractal.o: fractal.c
	gcc -Wall -c fractal.c

clean:
	(strip index ; rm *.o) 
