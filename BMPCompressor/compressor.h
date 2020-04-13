#ifndef COMPRESSOR_H
#define COMPRESSOR_H
#define DEBUG 0
#define MAX 500
#define SUCCESS 1
#define ERROR -999999

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
        - R: matrix to alloc the R component;
        - G: matrix to alloc the G component;
        - B: matrix to alloc the B component.
*/
void allocMatrices(unsigned char **R, unsigned char **G, unsigned char **B, BMPINFOHEADER *infoHeader);

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

#endif