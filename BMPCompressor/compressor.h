#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#define DEBUG 0
#define MAX 500
#define SUCCESS 1
#define ERROR -999999
#define PI 3.14159265358979323846

// Some defines to use in the (fast) sin1 cos1 functions
// special thanks to stfwi, writer of the article below
// https://www.atwillys.de/content/cc/sine-lookup-for-embedded-in-c/?lang=en

#define INT16_BITS (8 * sizeof(int16_t))
#ifndef INT16_MAX
#define INT16_MAX ((1 << (INT16_BITS - 1)) - 1)
#endif

#define TABLE_BITS (5)
#define TABLE_SIZE (1 << TABLE_BITS)
#define TABLE_MASK (TABLE_SIZE - 1)

#define LOOKUP_BITS (TABLE_BITS + 2)
#define LOOKUP_MASK ((1 << LOOKUP_BITS) - 1)
#define FLIP_BIT (1 << TABLE_BITS)
#define NEGATE_BIT (1 << (TABLE_BITS + 1))
#define INTERP_BITS (INT16_BITS - 1 - LOOKUP_BITS)
#define INTERP_MASK ((1 << INTERP_BITS) - 1)
// -----------------------------------------------------

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

static int16_t sin90[TABLE_SIZE + 1] = {
    0x0000, 0x0647, 0x0c8b, 0x12c7, 0x18f8, 0x1f19, 0x2527, 0x2b1e,
    0x30fb, 0x36b9, 0x3c56, 0x41cd, 0x471c, 0x4c3f, 0x5133, 0x55f4,
    0x5a81, 0x5ed6, 0x62f1, 0x66ce, 0x6a6c, 0x6dc9, 0x70e1, 0x73b5,
    0x7640, 0x7883, 0x7a7c, 0x7c29, 0x7d89, 0x7e9c, 0x7f61, 0x7fd7,
    0x7fff};

static int luminanceTable[8][8] = {
    16,11,10,16,24,40,51,61,
    12,12,14,19,26,58,60,55,
    14,13,16,24,40,57,69,56,
    14,17,22,29,51,87,80,62,
    18,22,37,56,68,109,103,77,
    24,35,55,64,81,104,113,92,
    49,64,78,87,103,121,120,101,
    72,92,95,98,112,100,103,99};

typedef struct BMPFILEHEADER BMPFILEHEADER; // BMP file header structure
typedef struct BMPINFOHEADER BMPINFOHEADER; // BMP file info structure

/*
    Function responsible to return the width of the image
    from its infoHeader struct.

    PARAMETERS:
        - infoHeader: struct with the necessary information.
    
    RETURNS the width of image.
*/
int getWidth(BMPINFOHEADER *infoHeader);

/*
    Function responsible to return the Height of the image
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
bool readBMPFileHeader(FILE *file, BMPFILEHEADER *fileHeader);

/* 
    Function to read the file information header and store in the struct.

    PARAMETERS:
        - file: pointer to an image file;
        - infoHeader: pointer to a struct that will save the image information.

    RETURNS SUCESS if everything went right. ERROR otherwise.
*/
bool readBMPInfoHeader(FILE *file, BMPINFOHEADER *infoHeader);

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
    Function responsible to alloc the R, G and B matrices.

    PARAMETERS:
        - mat: matrix to alloc;
        - rows: number of rows to alloc;
        - cols: number of columns to alloc.
    
    RETURNS allocated matrix.
*/
unsigned char **allocMatrix(unsigned char **mat, int rows, int cols);

/*
    Function to free a matrix, given an info header.

    PARAMETERS:
        - mat: matrix to free;
        - rows: number of rows to freeze.
*/
void freeMatrix(unsigned char **mat, int rows);

/*
    Function responsible to separate the R, G, B componentes.

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
    Function responsible to do the division of our image into its components
    (R, G and B) which represents its channels.

    PARAMETERS:
        - component: component we want to divide;
        - dctCoefs: matrix we're going to store the future dct coefficients;
        - infoHeader: struct with image infos.
    
*/
void divideMatrices(unsigned char **component, unsigned char **dctCoefs, BMPINFOHEADER *infoHeader);

/*
    Discrete cosine transform (DCT) is responsible for filtering 
    high/low spatial frequencies regions. These regions are ready to be
    compressed without any lose of image quality.

    PARAMETERS:
        - dctCoefs: matrix where we're going to store dct result;
        - mat: matrix with coeffs we're going to apply dct.
*/
void dct(unsigned char **dctCoefs, unsigned char **mat);

/*
    Fast sin function get from:
    https://www.atwillys.de/content/cc/sine-lookup-for-embedded-in-c/?lang=en
*/
int16_t sin1(int16_t angle);

/*
    Fast cos function get from:
    https://www.atwillys.de/content/cc/sine-lookup-for-embedded-in-c/?lang=en
*/
int16_t cos1(int16_t angle);

/*
    Function responsible to apply quantization. It divides our coefficients matrix
    (dctCoefs) by luminance table - a global static matrix.

    PARAMETERS:
        - dctCoefs: matrix with dct coefficients.

    RETURNS a matrix with quantization coefficients.
*/
void quantization(unsigned char **quantCoefs, unsigned char **dctCoefs);

#endif