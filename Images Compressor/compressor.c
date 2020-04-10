#include "compressor.h"

int main(int argc, char const *argv[])
{
    FILE *file = NULL;
    long size = 0;

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER*) malloc(sizeof(bmpFile));
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER*) malloc(sizeof(bmpInfo));

    file = fopen("images/1000x624.bmp", "r"); // opens the image that we want to compress
    if(file == NULL){
        printf("error"); // check if there was an error opening the image
        return 0;
    }
    
    // fseek(file, 0, SEEK_END);
    // size = ftell(file);
    // rewind(file);

    readBMPFileHeader(file, bmpFile);
    readBMPInfoHeader(file, bmpInfo);

    // long location = ftell(file);
    // printf("current location %ld and file's final location %ld", location, size);

    return 1;
}