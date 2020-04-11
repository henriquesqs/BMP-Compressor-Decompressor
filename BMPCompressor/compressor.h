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
char* readBitMapImage(FILE *file, BMPINFOHEADER *infoHeader);

/*
    Function to separate each channel (RGB) of a BMP file into matrices (R, G and B).

    PARAMETERS:
        - bmpImage: pointer to the image data;
        - infoHeader: struct with image header informations;
        - R, G, B: matrices to storage each channel bits.

*/
void separateComponents(char *bmpImage, BMPINFOHEADER *infoHeader, char** R, char** G, char** B);

/*
    Function to calculate and return image size.

    PARAMETERS:
        - infoHeader: struct with image header informations;
    
    RETURNS the size (width x height) of the image.
*/

unsigned int imageDataSize(BMPINFOHEADER * infoHeader);

int imageSize(BMPINFOHEADER *infoHeader);

#endif