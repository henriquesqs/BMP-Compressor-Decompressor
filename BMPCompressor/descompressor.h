#ifndef DESCOMPRESSOR_H
#define DESCOMPRESSOR_H

/*
    Function responsible to receive data to do descompress of an image.

    PARAMETERS:
        - auxY: aux variable responsible for storing where Y component ends in file;
        - auxCb: aux variable responsible for storing where Cb component ends in file.
*/
int descompressor();

double **runlengthDescomp(double **mat, FILE *file, int height, int width, long int aux);

double **idct(double **mat, int height, int width);

void YcbcrTorgb(unsigned char **R, unsigned char **G, unsigned char **B, double **Y, double **Cb, double **Cr, int height, int width);

double **quantizationLuminanceDescomp(double **component, int height, int width);

double **quantizationLuminanceDescomp(double **component, int height, int width);

unsigned char convertion(int num);
#endif