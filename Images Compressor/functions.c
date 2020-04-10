#include "compressor.h"

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
    (height > 1280 || height < 8 || width > 800 || width < 8) ? 1 : 0;
}