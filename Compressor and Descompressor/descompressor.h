#ifndef DESCOMPRESSOR_H
#define DESCOMPRESSOR_H

/*
    Function responsible to receive data to do descompressing of an image.

    PARAMETERS:
        - auxY: aux variable responsible for storing where Y component ends in file;
        - auxCb: aux variable responsible for storing where Cb component ends in file.
*/
int descompressor();

double **idct(double **mat, int height, int width);

void runlengthDescomp(short *vector, FILE *file);

double **quantizationLuminanceDescomp(double **component, int height, int width);

double **quantizationCrominanceDescomp(double **component, int height, int width);

unsigned char convertion(int num);

void YcbcrTorgb(unsigned char **R, unsigned char **G, unsigned char **B, double **Y, double **Cb, double **Cr, int height, int width);

double **undoDivideComponent(double **component, int height, int width, FILE *compressed);

void vectorizationDescomp(double **mat, short *vector);

void zigZagMatrixDescomp(double **arr, short *vector, int n, int m);

#endif