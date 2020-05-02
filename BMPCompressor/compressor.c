#include "compressor.h"

int main(int argc, char const *argv[]) {

    long size = 0;
    FILE *file = NULL;
    unsigned char *bmpImage = NULL;

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER *)malloc(14);
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER *)malloc(40);

    file = fopen("images/8x8.bmp", "rb"); // Openning the image that we want to compress.

    if (file == NULL) { // Checking if there was an error opening the image.
        printf("error reading file");
        return ERROR;
    }

    // Reading the bmp file header and info header so we can read image data without troubles.
    if (!readBMPFileHeader(file, bmpFile) || !readBMPInfoHeader(file, bmpInfo))
        return ERROR;

    // Moving our file pointer to the bitmap data region.
    moveToBitmapData(file, bmpFile);

    unsigned char **R = NULL, **G = NULL, **B = NULL; // We're going to split the RGB channels into these 3 matrices.

    // Allocating enough memory to store R, G and B channels.
    R = allocMatrix(R, getHeight(bmpInfo), getWidth(bmpInfo));
    G = allocMatrix(G, getHeight(bmpInfo), getWidth(bmpInfo));
    B = allocMatrix(B, getHeight(bmpInfo), getWidth(bmpInfo));

    // Separates the bitmap data into its RGB components.
    separateComponents(file, bmpInfo, R, G, B);

    // Now we're going to convert from RGB to YCbCr to increase DCT performance.
    double **Y = NULL, **Cb = NULL, **Cr = NULL;

    Y = allocDoubleMatrix(Y, getHeight(bmpInfo), getWidth(bmpInfo));
    Cb = allocDoubleMatrix(Cb, getHeight(bmpInfo), getWidth(bmpInfo));
    Cr = allocDoubleMatrix(Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    rgbToYcbcr(R, G, B, Y, Cb, Cr);

    // Dividing each component into 8x8 matrices in order to use DCT (Discrete Cosine Transform) algorithm,
    // at each 8x8 matrix, due to some researchs proving that this division increases the efficiency of DCT.
    double **dctCoefs = allocDoubleMatrix(dctCoefs, getHeight(bmpInfo), getWidth(bmpInfo));

    levelShift(dctCoefs, 128); // Applying level shift in order to increase DCT performance.

    Y = divideMatrices(Y, dctCoefs, bmpInfo);
    Cb = divideMatrices(Cb, dctCoefs, bmpInfo);
    Cr = divideMatrices(Cr, dctCoefs, bmpInfo);

    // Starting the quantization step. Here we're going to divide our DCT coefficients by
    // the quantization table so we can perform coefficients quantization.
    double **quantCoefs = allocDoubleMatrix(quantCoefs, getHeight(bmpInfo), getWidth(bmpInfo));

    Y = quantization(quantCoefs, Y);
    Cb = quantization(quantCoefs, Cb);
    Cr = quantization(quantCoefs, Cr);

    // On this step, we're going to apply vectorization using zig-zag scan. We do this to
    // make easier for us to compress the image by moving all the zero values to the end of the vector.
    // Its told that this step helps to increase run-length encoding performance.
    int vectorY[64] = {}, vectorCb[64] = {}, vectorCr[64] = {};

    vectorization(vectorY, Y);
    vectorization(vectorCb, Cb);
    vectorization(vectorCr, Cr);

    // Applying run-length encoding to each component (Y, Cb and Cr).
    FILE *rleFile = fopen("compressed", "wb+");

    runlength(vectorY, rleFile);
    runlength(vectorCb, rleFile);
    runlength(vectorCr, rleFile);

    // Free allocated memory.
    fclose(rleFile);
    fclose(file);
    free(bmpFile);
    free(bmpInfo);
    free(bmpImage);
    freeMatrix(R, getHeight(bmpInfo));
    freeMatrix(G, getHeight(bmpInfo));
    freeMatrix(B, getHeight(bmpInfo));
    freeDoubleMatrix(Y, getHeight(bmpInfo));
    freeDoubleMatrix(Cb, getHeight(bmpInfo));
    freeDoubleMatrix(Cr, getHeight(bmpInfo));
    freeDoubleMatrix(dctCoefs, getHeight(bmpInfo));
    freeDoubleMatrix(quantCoefs, getHeight(bmpInfo));

    return SUCCESS;
}