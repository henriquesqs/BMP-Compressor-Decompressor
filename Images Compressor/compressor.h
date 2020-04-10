#ifndef COMPRESSOR_H
#define COMPRESSOR_H
#define MAX 500

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct BMPFILEHEADER BMPFILEHEADER; // BMP file header structure
typedef struct BMPINFOHEADER BMPINFOHEADER; // BMP file info structure

// Function to read an image header.
//
// PARAMETERS:
//  - F: bitmap file that must be read from disk;
//  - H: struct containing header informations.
void readHeader(FILE *F, BMPFILEHEADER *H);

// Function to verify if the image is according to project's specification
// A valid image is at least 8x8 px and no greater than 1280x800 px.
//
// PARAMETERS:
//  - height: image's height;
//  - width: image's width.
//
// RETURN: 1 (true) if image is valid and 0 (false) otherwise.
bool validateImage(int height, int width, short type);

// Function to read the file header and store in the struct.
//
// PARAMETERS:
//  - file: pointer to an image file;
//  - fileHeader: pointer to struct that will save the image header.
void readBMPFileHeader(FILE *file, BMPFILEHEADER *fileHeader);

// Function to read the fil information header and store in the struct.
//
// PARAMETERS
//  - file: pointer to an image file;
//  - infoHeader: pointer to a struct that will save the image information.
void readBMPInfoHeader(FILE *file, BMPINFOHEADER *infoHeader);

#endif