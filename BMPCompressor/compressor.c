#include "compressor.h"

int main(int argc, char const *argv[]) {

    long size = 0;
    FILE *file = NULL;
    unsigned char *bmpImage;


    BMPFILEHEADER *bmpFile = (BMPFILEHEADER *)malloc(14);
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER *)malloc(40);

    file = fopen("images/1000x624.bmp", "rb"); // Openning the image that we want to compress.

    if (file == NULL) { // Checking if there was an error opening the image.
        printf("error reading file");
        return ERROR;
    }

    if (DEBUG) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        rewind(file);
    }

    // Reading the bmp file header and info header so we can read image data without troubles.
    if (!readBMPFileHeader(file, bmpFile) || !readBMPInfoHeader(file, bmpInfo))
        return ERROR;

    // Moving our file pointer to the bitmap data region.
    moveToBitmapData(file, bmpFile);

    // Reading the bitmap image data and the return of this function stores it in bmpImage.
    bmpImage = readBitMapImage(file, bmpInfo);
    
    // Checking for erros on above read
    if(bmpImage == NULL){
        printf("Error reading image data");
        free(bmpFile);
        free(bmpInfo);
        return ERROR;
    }

    if (DEBUG) {
        long location = ftell(file);
        printf("current location %ld and file's final location %ld", location, size);
    }

    char **R, **G, **B; // We're going to split the RGB channels into these 3 matrices.
    unsigned int dataSize = imageDataSize(bmpInfo); // Gets the size of image data.
    
    R = G = B = (char**) malloc(dataSize * sizeof(char*));

    for (int i = 0; i < dataSize; i++) {
        R[i] = (char*) malloc(sizeof(char));
        G[i] = (char*) malloc(sizeof(char));
        B[i] = (char*) malloc(sizeof(char));
    }

    // Separates the bitmap data into its RGB components
    separateComponents(bmpImage, bmpInfo, R, G, B);

    // Free allocated memory
    free(bmpFile);
    free(bmpInfo);
    free(bmpImage);
    fclose(file);

    return SUCCESS;
}