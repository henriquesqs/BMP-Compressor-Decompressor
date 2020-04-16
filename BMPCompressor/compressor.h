#ifndef COMPRESSOR_H
#define COMPRESSOR_H
#define DEBUG 0
#define MAX 500
#define SUCCESS 1
#define ERROR -999999
#define PI 3.14159265358979323846

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

typedef struct BMPFILEHEADER BMPFILEHEADER; // BMP file header structure
typedef struct BMPINFOHEADER BMPINFOHEADER; // BMP file info structure

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
        - infoHeader: struct with the necessary data to alloc the matrices;
        - mat: matrix to alloc;
    
    RETURNS allocated matrix.
*/
unsigned char **allocMatrix(unsigned char **mat, BMPINFOHEADER *infoHeader);

/*
    Function to free a matrix, given an info header.

    PARAMETERS:
        - mat: matrix to free;
        - infoHeader: struct containg some necessary info to do so.
*/
void freeMatrix(unsigned char **mat, BMPINFOHEADER *infoHeader);

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

void divideMatrices(unsigned char **component, unsigned char **dctCoefs, BMPINFOHEADER *infoHeader);

void dct(unsigned char **dctCoefs, unsigned char **mat);

int16_t sin1(int16_t angle);

int16_t cos1(int16_t angle);

#endif