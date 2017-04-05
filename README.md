# Computer_Vision
## First program

Write a program that displays continuous changes in color for the xy and the Luv representations. The input to the program is a width and
a height. The output is two images of dimensions width * height that are displayed on the screen.

For the xy image, the pixel at row i and column j should have the color value:
x = j=width; y = i=height; Y = 1

For the Luv image, the pixel at row i and column j should have the color value:
L = 90; u = 512 * j=width / 255; v = 512 * i=height / 255

The main programming effort is writing the routines to convert xyY and Luv pixels to sRGB. The provided
example program proj1a.cpp does everything with the exception of this conversion. It is recommended
that you write your program by changing proj1a.cpp so that it fulfills the requirements.

## Second, third, and fourth program

### Second Program

These programs change the color of the image based on a histogram computed from a window in the image. The window is specified in terms
of the normalized coordinates w1 h1 w2 h2, where the window upper left point is (w1,h1), and its lower right point is (w2,h2). For example,
w1=0,h1=0,w2=1,h2=1 is the entire image, and w1=0.3,h1=0.3,w2=0.7,h2=0.7 is is window in the center of the image. The provided example 
program proj1b.cpp shows how to go over the pixels of this window.

### Third program

Write a program that gets as input a color image, performs histogram equalization in the Luv domain, and writes the scaled image as output. Histogram equalization in Luv is applied to the luminance values, as
computed in the specifieed window. It requires a discretization step, where the real-valued L is discretized into 101 values. As in the
second program, all L values below the smallest L value in the window should be mapped to 0, and all L value above the largest L value
in the window should be mapped to 100.

### Fourth program

This is the same as the second program, except that the scaling is to be performed in the xyY domain. The scaling should stretch only the
luminance (Y) values. You are asked to apply linear scaling that would map the smallest Y value in the specified window and all values
below it to 0, and the largest Y value in the specified window and all values above it to 1.

