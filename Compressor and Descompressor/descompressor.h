#ifndef DESCOMPRESSOR_H
#define DESCOMPRESSOR_H

// Every function has the same header of its equivalent method in compressor.h
// so we are not leaving comments on these, only in methods that do not have its equivalent.

/*
    Function responsible to receive data to do descompressing of an image.

    RETURNS SUCCESS if everything went right and ERROR otherwise.
*/
int descompressor();

/*
    Function responsible for storing the correct values in unsigned char variables
    by converting 'num' to 255 if it's greater than 255 and converting 'num' to 0
    if it's smaller than 0.

    RETURNS converted num.
    
*/
unsigned char convertion(int num);

double **idct(double **mat, int height, int width);

void runlengthDescomp(short *vector, FILE *file);

double **quantizationLuminanceDescomp(double **component, int height, int width);

double **quantizationCrominanceDescomp(double **component, int height, int width);

void YcbcrTorgb(unsigned char **R, unsigned char **G, unsigned char **B, double **Y, double **Cb, double **Cr, int height, int width);

double **undoDivideComponent(double **component, int height, int width, FILE *compressed);

void vectorizationDescomp(double **mat, short *vector);

void zigZagMatrixDescomp(double **arr, short *vector, int n, int m);

#endif