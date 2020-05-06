#ifndef DESCOMPRESSOR_H
#define DESCOMPRESSOR_H

/*
    Function responsible to receive data to do descompress of an image.

    PARAMETERS:
        - infoHeader: struct with image info header;
        - file: pointer to compressed image file;
        - auxY: aux variable responsible for storing where Y component ends in file;
        - auxCb: aux variable responsible for storing where Cb component ends in file;
        - auxCr: aux variable responsible for storing where Cr component ends in file.
*/
int descompressor(BMPINFOHEADER* infoHeader, FILE* file, long int* auxY, long int* auxCb);

float** runlengthDescomp(int height, int width, FILE* file, long int* aux);

#endif