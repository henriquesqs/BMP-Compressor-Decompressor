#include "compressor.h"

int main(int argc, char const *argv[])
{
    FILE *file;

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER*) malloc(sizeof(bmpFile));
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER*) malloc(sizeof(bmpInfo));

    file = fopen("images/1000x624.bmp", "r"); // opens the image that we want to compress

    if(file == NULL) printf("error"); // check if there was an error opening the image

    readBMPFileHeader(file, bmpFile);
    readBMPInfoHeader(file, bmpInfo);

    free(bmpFile);
    free(bmpInfo);
    fclose(file);

    return 1;
}
