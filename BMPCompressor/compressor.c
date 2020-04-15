#include "compressor.h"

int main(int argc, char const *argv[]) {

    long size = 0;
    FILE *file = NULL;
    unsigned char *bmpImage = NULL, **dctCoefs = NULL;

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER *)malloc(14);
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER *)malloc(40);

    file = fopen("images/8x8.bmp", "rb"); // Openning the image that we want to compress.

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

    unsigned char **R = NULL, **G = NULL, **B = NULL; // We're going to split the RGB channels into these 3 matrices.

    // Allocating enough memory to allocate the channels.
    R = allocMatrix(R, bmpInfo);
    G = allocMatrix(G, bmpInfo);
    B = allocMatrix(B, bmpInfo);

    // Separates the bitmap data into its RGB components.
    separateComponents(file, bmpInfo, R, G, B);

    if (DEBUG) {
        long location = ftell(file);
        printf("current location %ld and file's final location %ld\n", location, size);
    }

    // Dividing each component into 8x8 matrices in order to use DCT (Discrete cosine transform) algorithm,
    // due to some researchs proving that this division increases the efficiency of DCT.
    dctCoefs = allocMatrix(dctCoefs, bmpInfo);
    divideMatrices(R, dctCoefs, bmpInfo);
    divideMatrices(G, dctCoefs, bmpInfo);
    divideMatrices(B, dctCoefs, bmpInfo);

    // // Free allocated memory.
    fclose(file);
    free(bmpFile);
    free(bmpInfo);
    free(bmpImage);
    freeMatrix(R, bmpInfo);
    freeMatrix(G, bmpInfo);
    freeMatrix(B, bmpInfo);
    freeMatrix(dctCoefs, bmpInfo);

    return SUCCESS;
}