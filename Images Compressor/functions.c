#include "compressor.h"

int buffer[MAX];

typedef struct BMPFILEHEADER {
    unsigned short bfType;      /* Magic number for file */
    unsigned int bfSize;        /* Size of file */
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits;     /* Offset to bitmap data */
} BMPFILEHEADER;
/* bfType deve ser = "MB" */

typedef struct BMPINFOHEADER {
    unsigned int biSize;         /* Size of info header */
    int biWidth;                 /* Width of image */
    int biHeight;                /* Height of image */
    unsigned short biPlanes;     /* Number of color planes */
    unsigned short biBitCount;   /* Number of bits per pixel */
    unsigned int biCompression;  /* Type of compression to use */
    unsigned int biSizeImage;    /* Size of image data */
    int biXPelsPerMeter;         /* X pixels per meter */
    int biYPelsPerMeter;         /* Y pixels per meter */
    unsigned int biClrUsed;      /* Number of colors used */
    unsigned int biClrImportant; /* Number of important colors */
} BMPINFOHEADER;

void readHeader(FILE *F, BMPFILEHEADER *H) {

    fread(&H->bfType, sizeof(unsigned short int), 1, F);
    fread(&H->bfSize, sizeof(unsigned int), 1, F);
    fread(&H->bfReserved1, sizeof(unsigned short int), 1, F);
    fread(&H->bfReserved2, sizeof(unsigned short int), 1, F);
    fread(&H->bfOffBits, sizeof(unsigned int), 1, F);
}

bool validateImage(int height, int width){
    //if(height > 1280 || height < 8 || width > 800 || width < 8)
    //    return false; // image is not valid
    // return true;
    return (height > 1280 || height < 8 || width > 800 || width < 8) ? 1 : 0;
}

void readBMPFileHeader(FILE* file, BMPFILEHEADER *fileHeader){

    fileHeader->bfType = fread(&buffer, 2, 1, file);
    fileHeader->bfSize = fread(&buffer, 4, 1, file);
    fileHeader->bfReserved1 = fread(&buffer, 2, 1, file);
    fileHeader->bfReserved2 = fread(&buffer, 2, 1, file);
    fileHeader->bfOffBits = fread(&buffer, 4, 1, file);

}

void readBMPInfoHeader(FILE* file, BMPINFOHEADER *infoHeader){

    infoHeader->biSize = fread(&buffer, 4, 1, file);
    infoHeader->biWidth = fread(&buffer, 4, 1, file);
    infoHeader->biHeight = fread(&buffer, 4, 1, file);
    infoHeader->biPlanes = fread(&buffer, 2, 1, file);
    infoHeader->biBitCount = fread(&buffer, 2, 1, file);
    infoHeader->biCompression = fread(&buffer, 4, 1, file);
    infoHeader->biSizeImage = fread(&buffer, 4, 1, file);
    infoHeader->biXPelsPerMeter = fread(&buffer, 4, 1, file);
    infoHeader->biYPelsPerMeter = fread(&buffer, 4, 1, file);
    infoHeader->biClrUsed = fread(&buffer, 4, 1, file);
    infoHeader->biClrImportant = fread(&buffer, 4, 1, file);

}