#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#define DEBUG 0
#define MAX 500
#define SUCCESS 1
#define ERROR -999999
#define PI 3.14159265358979323846

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct BMPFILEHEADER BMPFILEHEADER; // BMP file header structure
typedef struct BMPINFOHEADER BMPINFOHEADER; // BMP file info structure

/*
    Method to initialize cosine look-up table. This is due to the need of
    performance improvements on DCT function.
*/
void initCosLUT();

/*
    Function responsible for return the width of the image
    from its infoHeader struct.

    PARAMETERS:
        - infoHeader: struct with the necessary information.
    
    RETURNS the width of image.
*/
int getWidth(BMPINFOHEADER *infoHeader);

/*
    Function responsible for return the Height of the image
    from its infoHeader struct.

    PARAMETERS:
        - infoHeader: struct with the necessary information.
    
    RETURNS the Height of image.
*/
int getHeight(BMPINFOHEADER *infoHeader);

/* 
    Function to verify if the image is according to project's specification
    A valid image is at least 8x8 px and no greater than 1280x800 px.

    PARAMETERS:
        - height: image's height;
        - width: image's width.

    RETURN: SUCCESS if image is valid and ERROR otherwise.
*/
bool validateImage(int height, int width);

/* 
    Function to read the file header and store in the struct.

    PARAMETERS:
        - file: pointer to an  bitmap image file;
        - fileHeader: pointer to struct that will save the image header.

    RETURNS SUCESS if everything went right. ERROR otherwise.
*/
int readBMPFileHeader(FILE *file, BMPFILEHEADER *fileHeader);

/* 
    Function to read the file information header and store in the struct.

    PARAMETERS:
        - file: pointer to an image file;
        - infoHeader: pointer to a struct that will save the image information.

    RETURNS SUCESS if everything went right. ERROR otherwise.
*/
int readBMPInfoHeader(FILE *file, BMPINFOHEADER *infoHeader);

/*
    Auxiliar method to print headers content.

    PARAMETERS:
        - bmpFile: struct with bmp file header;
        - bmpInfo: struct with bmp info header;
*/
void printHeader(BMPFILEHEADER *bmpFile, BMPINFOHEADER *bmpInfo);
/* 
    Function to move to bitmap image data given a File header and its bfOffBits.

    PARAMETERS:
        - file: pointer to an image file;
        - FH: pointer to a struct that stores the file header.
*/
void moveToBitmapData(FILE *file, BMPFILEHEADER *FH);

/* 
    Function to read the image data from a file pointer.

    PARAMETERS:
        - file: pointer to a bitmap image file;
        - bmpImage: variable to store image data;
        - infoHeader: struct with image header informations.

    RETURNS SUCESS if everything went right. ERROR otherwise.
*/
void readBitMapImage(FILE *file, BMPINFOHEADER *infoHeader, unsigned char **R, unsigned char **G, unsigned char **B);

/*
    Function responsible for alloc unsigned char** matrices.

    PARAMETERS:
        - mat: matrix to alloc;
        - rows: number of rows to alloc;
        - cols: number of columns to alloc.
    
    RETURNS allocated matrix.
*/
unsigned char **allocMatrix(unsigned char **mat, int rows, int cols);

/*
    Function responsible for alloc int matrices.

    PARAMETERS:
        - mat: matrix to alloc;
        - rows: number of rows to alloc;
        - cols: number of columns to alloc.
    
    RETURNS allocated matrix.
*/
int **allocIntMatrix(int **mat, int rows, int cols);

/*
    Function responsible for alloc double matrices.

    PARAMETERS:
        - mat: matrix to alloc;
        - rows: number of rows to alloc;
        - cols: number of columns to alloc.
    
    RETURNS allocated matrix.
*/
double **allocDoubleMatrix(double **mat, int rows, int cols);

/*
    Function responsible for alloc float matrices.

    PARAMETERS:
        - mat: matrix to alloc;
        - rows: number of rows to alloc;
        - cols: number of columns to alloc.
    
    RETURNS allocated matrix.
*/
float **allocFloatMatrix(float **mat, int rows, int cols);

/*
    Function to free a float matrix, given its rows.

    PARAMETERS:
        - mat: matrix to free;
        - rows: number of rows to free.
*/
void freeFloatMatrix(float **mat, int rows);

/*
    Function to free a matrix, given its rows.

    PARAMETERS:
        - mat: matrix to free;
        - rows: number of rows to free.
*/
void freeDoubleMatrix(double **mat, int rows);

/*
    Function to free a matrix, given its rows.

    PARAMETERS:
        - mat: matrix to free;
        - rows: number of rows to free.
*/
void freeMatrix(unsigned char **mat, int rows);

/*
    Function to free a matrix, given its rows.

    PARAMETERS:
        - mat: matrix to free;
        - rows: number of rows to free.
*/
void freeIntMatrix(int **mat, int rows);

/*
    Function responsible for separate the R, G, B componentes.

    PARAMETERS:
        - file: pointer to the image file.
        - infoHeader: struct with the necessary data to alloc the matrices;
        - R: matrix to alloc the R component;
        - G: matrix to alloc the G component;
        - B: matrix to alloc the B component.
*/
void separateComponents(FILE *file, BMPINFOHEADER *infoHeader, unsigned char **R, unsigned char **G, unsigned char **B);

/*
    Function to calculate and return image data size.

    PARAMETERS:
        - infoHeader: struct with image header informations;
    
    RETURNS the size of the data of the image.
*/
unsigned int imageDataSize(BMPINFOHEADER *infoHeader);

/*
    Function to calculate and return image size.

    PARAMETERS:
        - infoHeader: struct with image header informations;
    
    RETURNS the size (width x height) of the image.
*/
int imageSize(BMPINFOHEADER *infoHeader);

/*
    Calculates size of a file;

    PARAMETERS:
        - file: pointer to file to calculates its size.
    
    RETURNS size of file.

*/
long int fileSize(FILE *file);

/*
    Function responsible for dividing our image into 8x8 matrices and
    apply dct, quantization, vectorization and run-length into each one of them.

    PARAMETERS:
        - lum: flag to indicate if its a luminance component or not;
        - compressed: file to store compressed data;
        - component: component we want to divide and apply dct, quantization and run-length;
        - height: height of component;
        - width: width of component;
    
*/
double **divideMatrices(int lum, FILE *compressed, double **component, int height, int width);

// pra testar
void printComponent(double **component, int height, int width);

/*
    Discrete cosine transform (DCT) is responsible for filtering high/low spatial frequencies regions.
    These regions are ready to be compressed without any lose of image quality.

    PARAMETERS:
        - dctCoefs: matrix where we're going to store dct result;
        - mat: matrix with coeffs we're going to apply dct.
*/
double **dct(double **compont, int height, int width);

/*
    Function responsible for apply a level shift in a double matrix.

    PARAMETERS:
        - mat: matrix to apply level shift;
        - offBits: quantity of bits to shift;
        - height: height of mat;
        - width: width of mat.

*/
void levelShift(double **mat, int offBits, int height, int width);

/*
    Function responsible for apply quantization in luminance components. 
    It divides our coefficients matrix (dctCoefs) by a luminance table.

    PARAMETERS:
        - coefs: matrix with dct coefficients.

*/
double **quantizationLuminance(double **component, int height, int width);

/*
    Function responsible for apply quantization in crominance components. 
    It divides our coefficients matrix (dctCoefs) by a crominance table.

    PARAMETERS:
        - coefs: matrix with dct coefficients.

*/
double **quantizationCrominance(double **component, int height, int width);

/*
    This function is responsible for apply vectorization on a matrix using zig-zag scan.
    This will help on run-length codification step as it puts zeros at the ending of vector.

    PARAMETERS:
        - vector: vector to store elements;
        - mat: matrix to apply vectorization.
*/
void vectorization(unsigned char *vector, double **mat);

/*
    Function responsible for converting from RGB channels to YCbCr.

    PARAMETERS:
        - R: matrix of R component;
        - G: matrix of G component;
        - B: matrix of B component;
        - Y: matrix of Y component;
        - Cb: matrix of Cb component;
        - Cr: matrix of Cr component;
        - height: height of components;
        - width: width of components;
*/
void rgbToYcbcr(unsigned char **R, unsigned char **G, unsigned char **B, double **Y, double **Cb, double **Cr, int height, int width);

/*
    Function responsible for applying Run-Length Encondig (RLE), which is a technic to apply digital data compression.

    PARAMETERS:
        - vector: vector with data to compress;
        - file: file to output compressed data.
*/
void runlength(double **dctCoefs, FILE *file, int height, int width);

/*
    Function responsible for writing in 'file' the data inside BMPINFOHEADER and BMPINFOHEADER.

    PARAMETERS:
        FH: struct with bmp file header;
        IH: struct with bmp info header;
        file: pointer to file.
*/
void writeHeaders(BMPFILEHEADER *FH, BMPINFOHEADER *IH, FILE *file);

/*
    Function responsible for calling all the methods to do bmp image compress.

    PAREMETERS:
        - auxY: auxiliar variable to stores where Y component (from YCbCr) will ends in compressed file;
        - auxCb: auxiliar variable to stores where Cb component (from YCbCr) will ends in compressed file;
        - compressRate: pointer to a variable responsible to stores the compression rate.
*/
int compress(long int *auxY, long int *auxCb, double *compressRate);

#endif