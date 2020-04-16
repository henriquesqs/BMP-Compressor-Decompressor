#include "compressor.h"

struct BMPFILEHEADER {
    unsigned short bfType;      /* Magic number for file. Must be in "MB" */
    unsigned int bfSize;        /* Size of file */
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits;     /* Offset to bitmap data */
};

struct BMPINFOHEADER {
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
};

bool validateImage(int height, int width) {

    //if(height > 1280 || height < 8 || width > 800 || width < 8)
    //    return false; // image is not valid
    //if (type != 0x4D42)
    //    return 0; // its not a bmp file
    //return true;

    return (height > 1280 || height < 8 || width > 800 || width < 8) ? ERROR : SUCCESS;
}

int getWidth(BMPINFOHEADER *infoHeader) {
    return infoHeader->biWidth;
}

int getHeight(BMPINFOHEADER *infoHeader) {
    return infoHeader->biHeight;
}

unsigned int imageDataSize(BMPINFOHEADER *infoHeader) {
    return infoHeader->biSizeImage;
}

int imageSize(BMPINFOHEADER *infoHeader) {
    return (infoHeader->biHeight * infoHeader->biWidth);
}

bool readBMPFileHeader(FILE *file, BMPFILEHEADER *FH) {

    fread(&FH->bfType, sizeof(unsigned short), 1, file);
    fread(&FH->bfSize, sizeof(unsigned int), 1, file);
    fread(&FH->bfReserved1, sizeof(unsigned short), 1, file);
    fread(&FH->bfReserved2, sizeof(unsigned short), 1, file);
    fread(&FH->bfOffBits, sizeof(unsigned int), 1, file);

    if (FH->bfType != 0x4D42) {
        printf("not a bmp file\n");
        return ERROR;
    }
    return SUCCESS;
}

bool readBMPInfoHeader(FILE *file, BMPINFOHEADER *IH) {

    fread(&IH->biSize, sizeof(unsigned int), 1, file);
    fread(&IH->biWidth, sizeof(int), 1, file);
    fread(&IH->biHeight, sizeof(int), 1, file);
    fread(&IH->biPlanes, sizeof(unsigned short), 1, file);
    fread(&IH->biBitCount, sizeof(unsigned short), 1, file);
    fread(&IH->biCompression, sizeof(unsigned int), 1, file);
    fread(&IH->biSizeImage, sizeof(unsigned int), 1, file);
    fread(&IH->biXPelsPerMeter, sizeof(int), 1, file);
    fread(&IH->biYPelsPerMeter, sizeof(int), 1, file);
    fread(&IH->biClrUsed, sizeof(unsigned int), 1, file);
    fread(&IH->biClrImportant, sizeof(unsigned int), 1, file);

    if (!validateImage(IH->biHeight, IH->biWidth)) {
        printf("Image isnt valid. Height or Width is incorrect.\n");
        return ERROR;
    }

    return SUCCESS;
}

void moveToBitmapData(FILE *file, BMPFILEHEADER *FH) {
    fseek(file, FH->bfOffBits, SEEK_SET);
}

unsigned char **allocMatrix(unsigned char **mat, int n, int m) {

    mat = malloc(n * sizeof(char *));

    for (int i = 0; i < n; i++)
        mat[i] = malloc(m * sizeof(char));

    return mat;
}

void freeMatrix(unsigned char **mat, int rows) {

    for (int i = 0; i < rows; i++)
        free(mat[i]);

    free(mat);
}

void separateComponents(FILE *file, BMPINFOHEADER *infoHeader, unsigned char **R, unsigned char **G, unsigned char **B) {

    for (int i = 0; i < infoHeader->biHeight; i++) {
        for (int j = 0; j < infoHeader->biWidth; j++) {
            B[i][j] = fgetc(file);
            G[i][j] = fgetc(file);
            R[i][j] = fgetc(file);
        }
    }
}

void dct(unsigned char **dctCoefs, unsigned char **mat) {

    /*  
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        FOR GOD's SAKE WE NEED TO OPTIMIZE THIS PART OF THE CODE
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    */

    // cos(a) = sqrt(1 - sin(a)^2))

    int c1 = 1, c2 = 1;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            dctCoefs[i][j] = 0;

            if (i == 0 && j == 0)
                c1 = c2 = 1 / sqrt(2);

            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {

                    // Thanks to the writer (@author stfwi) of this page no the link below, we found
                    // a fast cosine and sin functions, which uses a LUT (Look Up Table), to use here:
                    // https://www.atwillys.de/content/cc/sine-lookup-for-embedded-in-c/?lang=en

                    // dctCoefs[i][j] = c1 * c2 * mat[x][y] * cos(((2 * x + 1) * i * PI) / 16) * cos(((2 * y + 1) * j * PI) / 16);
                    dctCoefs[i][j] = c1 * c2 * mat[x][y] * cos1(((2 * x + 1) * i * PI) / 16) * cos1(((2 * y + 1) * j * PI) / 16);
                }
            }
        }
    }
}

void divideMatrices(unsigned char **component, unsigned char **dctCoefs, BMPINFOHEADER *infoHeader) {

    unsigned char **mat = NULL;

    mat = malloc(8 * sizeof(char *));

    for (int i = 0; i < 8; i++)
        mat[i] = malloc(8 * sizeof(char));

    for (int i = 0; i < infoHeader->biHeight / 8; i++) {
        for (int j = 0; j < infoHeader->biWidth / 8; j++) {
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    // We are just copying the 8x8 part of component matrix and applying
                    // level shift below, which is said to increase the performance of DCT.
                    mat[k][l] = component[i * 8 + k][j * 8 + l] - 128;
                    dct(dctCoefs, mat);
                }
            }
        }
    }

    for (int i = 0; i < 8; i++)
        free(mat[i]);
    free(mat);
}

int16_t sin1(int16_t angle) {
    int16_t v0, v1;
    if (angle < 0) {
        angle += INT16_MAX;
        angle += 1;
    }
    v0 = (angle >> INTERP_BITS);
    if (v0 & FLIP_BIT) {
        v0 = ~v0;
        v1 = ~angle;
    } else {
        v1 = angle;
    }
    v0 &= TABLE_MASK;
    v1 = sin90[v0] + (int16_t)(((int32_t)(sin90[v0 + 1] - sin90[v0]) * (v1 & INTERP_MASK)) >> INTERP_BITS);
    if ((angle >> INTERP_BITS) & NEGATE_BIT)
        v1 = -v1;
    return v1;
}

int16_t cos1(int16_t angle) {
    if (angle < 0) {
        angle += INT16_MAX;
        angle += 1;
    }
    return sin1(angle - (int16_t)(((int32_t)INT16_MAX * 270) / 360));
}

void quantization(unsigned char **quantCoefs, unsigned char **dctCoefs) {

    for (int i = 0; i < 8; i++) 
        for (int j = 0; j < 8; j++) 
            quantCoefs[i][j] = dctCoefs[i][j] / luminanceTable[i][j];
}