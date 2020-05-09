#include "compressor.h"
#include "descompressor.h"

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

// global variable to store cosine and constant values to later be used in DCT and IDCT
double cosine[8][8], C[8];

// Below, all the functions used to create our BMP compressor

void initCosLUT() {
    for (int i = 0; i < 8; i++) {
        for (int x = 0; x < 8; x++) {

            cosine[i][x] = 0;
            cosine[i][x] = cos(((((2.0 * i) + 1.0) * (x * PI)) / (16)));
        }
        if (i)
            C[i] = 1;
        else
            C[i] = 1 / sqrt(2);
    }
}

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

long int fileSize(FILE *file) {

    long int position = ftell(file); // Saving file pointer current position;

    fseek(file, 0, SEEK_END); // Moving file pointer to the end of file;

    long int size = ftell(file); // Saving final position;

    fseek(file, position, SEEK_SET); // Moving gile pointer to its original position.

    return size;
}

int readBMPFileHeader(FILE *file, BMPFILEHEADER *FH) {

    fread(&FH->bfType, sizeof(unsigned short), 1, file);
    fread(&FH->bfSize, sizeof(unsigned int), 1, file);
    fread(&FH->bfReserved1, sizeof(unsigned short), 1, file);
    fread(&FH->bfReserved2, sizeof(unsigned short), 1, file);
    fread(&FH->bfOffBits, sizeof(unsigned int), 1, file);

    if (FH->bfType != 0x4D42) {
        printf("\nNot a BMP file. Please, enter a valid image.\n");
        return ERROR;
    }
    return SUCCESS;
}

int readBMPInfoHeader(FILE *file, BMPINFOHEADER *IH) {

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
        printf("\nImage isnt valid. Height or Width is incorrect.\nTo use our program, your image must have:\n");
        printf("- Height and width multiple of 8;\n- At least 8x8 pixels;\n- Max size of 1280 x 800 pixels.\n");
        return ERROR;
    }

    return SUCCESS;
}

void moveToBitmapData(FILE *file, BMPFILEHEADER *FH) {
    // fseek(file, FH->bfOffBits, SEEK_SET);
    fseek(file, 54, SEEK_SET);
}

unsigned char **allocMatrix(unsigned char **mat, int n, int m) {

    mat = malloc(n * sizeof(unsigned char *));

    // Allocating enough memory
    for (int i = 0; i < n; i++)
        mat[i] = malloc(m * sizeof(unsigned char));

    // Initializing mat
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            mat[i][j] = 0;
        }
    }

    return mat;
}

int **allocIntMatrix(int **mat, int n, int m) {

    mat = malloc(n * sizeof(int *));

    for (int i = 0; i < n; i++)
        mat[i] = malloc(m * sizeof(int));

    return mat;
}

double **allocDoubleMatrix(double **mat, int n, int m) {

    mat = malloc(n * sizeof(double *));

    for (int i = 0; i < n; i++)
        mat[i] = malloc(m * sizeof(double));

    return mat;
}

float **allocFloatMatrix(float **mat, int n, int m) {

    mat = malloc(n * sizeof(float *));

    // Allocating enough memory
    for (int i = 0; i < n; i++)
        mat[i] = malloc(m * sizeof(float));

    // Initializing mat
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            mat[i][j] = 0;

    return mat;
}

void freeFloatMatrix(float **mat, int rows) {

    for (int i = 0; i < rows; i++)
        free(mat[i]);

    free(mat);
}

void freeDoubleMatrix(double **mat, int rows) {

    for (int i = 0; i < rows; i++)
        free(mat[i]);

    free(mat);
}

void freeMatrix(unsigned char **mat, int rows) {

    for (int i = 0; i < rows; i++)
        free(mat[i]);

    free(mat);
}

void freeIntMatrix(int **mat, int rows) {

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

void levelShift(double **mat, int offBits, int height, int width) {

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            mat[i][j] += offBits;
}

double **dct(double **mat, int height, int width) {

    double aux = 0;
    double **dctCoefs = allocDoubleMatrix(dctCoefs, height, width);

    // First double 'for loop' divides mat into 8x8 pieces and save this slice in mat2
    for (int a = 0; a < ceil(height / 8); a++) {
        for (int b = 0; b < ceil(width / 8); b++) {

            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {

                    aux = 0; // aux variable to store sum values

                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {

                            aux += mat[a * 8 + i][b * 8 + j] * cosine[i][x] * cosine[j][y];
                        }
                    }

                    dctCoefs[a * 8 + x][b * 8 + y] = C[x] * C[y] * 0.25 * aux;
                }
            }
        }
    }

    return dctCoefs;
}

double **quantizationLuminance(double **component, int height, int width) {

    // LuminanceTable is applied in Y component
    double luminanceTable[8][8] = {16, 11, 10, 16, 24, 40, 51, 61,
                                   12, 12, 14, 19, 26, 58, 60, 55,
                                   14, 13, 16, 24, 40, 57, 69, 56,
                                   14, 17, 22, 29, 51, 87, 80, 62,
                                   18, 22, 37, 56, 68, 109, 103, 77,
                                   24, 35, 55, 64, 81, 104, 113, 92,
                                   49, 64, 78, 87, 103, 121, 120, 101,
                                   72, 92, 95, 98, 112, 100, 103, 99};

    for (int a = 0; a < ceil(height / 8); a++) {
        for (int b = 0; b < ceil(width / 8); b++) {
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    component[a * 8 + i][b * 8 + j] = floor(component[a * 8 + i][b * 8 + j] / luminanceTable[i][j]);
        }
    }
    return component;
}

double **quantizationCrominance(double **component, int height, int width) {

    // Crominance is applied in Cb and Cr components
    double crominanceTable[8][8] = {17, 18, 24, 47, 99, 99, 99, 99,
                                    18, 21, 26, 66, 99, 99, 99, 99,
                                    24, 26, 56, 99, 99, 99, 99, 99,
                                    47, 66, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99};

    for (int a = 0; a < ceil(height / 8); a++) {
        for (int b = 0; b < ceil(width / 8); b++) {
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    component[a * 8 + i][b * 8 + j] = floor(component[a * 8 + i][b * 8 + j] / crominanceTable[i][j]);
        }
    }

    return component;
}

void rgbToYcbcr(unsigned char **R, unsigned char **G, unsigned char **B, double **Y, double **Cb, double **Cr, int height, int width) {

    double Kr = 0.299, Kb = 0.114;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            Y[i][j] = (Kr * R[i][j]) + (0.587 * G[i][j]) + (Kb * B[i][j]);
            Cb[i][j] = 0.564 * (B[i][j] - Y[i][j]);
            Cr[i][j] = 0.713 * (R[i][j] - Y[i][j]);
        }
    }
}

void writeHeaders(BMPFILEHEADER *FH, BMPINFOHEADER *IH, FILE *file) {

    // Writing File Header data
    fwrite(&FH->bfType, sizeof(unsigned short), 1, file);
    fwrite(&FH->bfSize, sizeof(unsigned int), 1, file);
    fwrite(&FH->bfReserved1, sizeof(unsigned short), 1, file);
    fwrite(&FH->bfReserved2, sizeof(unsigned short), 1, file);
    fwrite(&FH->bfOffBits, sizeof(unsigned int), 1, file);

    // Writing Info Header data
    fwrite(&IH->biSize, sizeof(unsigned int), 1, file);
    fwrite(&IH->biWidth, sizeof(int), 1, file);
    fwrite(&IH->biHeight, sizeof(int), 1, file);
    fwrite(&IH->biPlanes, sizeof(unsigned short), 1, file);
    fwrite(&IH->biBitCount, sizeof(unsigned short), 1, file);
    fwrite(&IH->biCompression, sizeof(unsigned int), 1, file);
    fwrite(&IH->biSizeImage, sizeof(unsigned int), 1, file);
    fwrite(&IH->biXPelsPerMeter, sizeof(int), 1, file);
    fwrite(&IH->biYPelsPerMeter, sizeof(int), 1, file);
    fwrite(&IH->biClrUsed, sizeof(unsigned int), 1, file);
    fwrite(&IH->biClrImportant, sizeof(unsigned int), 1, file);
}

// void runlength2(unsigned char *vector, FILE *file) {

//     int count = 0;        // This variable will count occurrences of the same value until a different one is found.
//     char binary[9];       // This will stores the binary representation of 'count'.
//     unsigned char buffer; // This will stores the char representation of binary, i.e, count.

//     for (int i = 1; i < 64; i++) { // 'i' starts at position 1 because position 0 has a DC coefficient and run-length must be applied in AC coefficientes

//         // Counting occurrences of current value (while avoiding buffer overflow).
//         count = 1;

//         while (i < 63 && vector[i] == vector[i + 1]) {
//             count++;
//             i++;
//         }

//         // When finds a different value, starts preparation to
//         // write the previous value and its count in file.

//         // Converting count to binary
//         // Thanks to Fifi from StackOverflow for his answer (https://stackoverflow.com/a/58940759/10304974) :)
//         for (int j = 0; j < 8; j++)
//             binary[j] = (count & (int)1 << (8 - j - 1)) ? '1' : '0';
//         binary[8] = '\0';

//         // Transfers binary[] data to 1 byte (thanks to Professor Rudinei Goularte for this)
//         for (int j = 0; j < 8; j++)
//             buffer = (buffer << 1) | (binary[j] == '1');

//         // Writes value and its count in file
//         fwrite(&vector[i], sizeof(vector[i]), 1, file);
//         fwrite(&buffer, sizeof(buffer), 1, file);
//     }
// }

void runlength(double **dctCoefs, FILE *file) {

    int count = 0;              // This variable will count occurrences of the same value until a different one is found.
    char binary[9], binary2[9]; // This will stores the binary representation of 'count'.
    char buffer, buffer2;       // This will stores the char representation of binary, i.e, count.
    int value;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            // Counting occurrences of current value (while avoiding buffer overflow).
            count = 1;

            if (i == j == 0) // run-length must be applied only on DC coefficientes
                j++;

            while (j + 1 < 8 && dctCoefs[i][j] == dctCoefs[i][j + 1]) {
                count++;
                j++;
            }

            value = dctCoefs[i][j];

            /* Converting count to binary */

            // Thanks to Fifi from StackOverflow for his answer (https://stackoverflow.com/a/58940759/10304974) :)
            for (int k = 0; k < 8; k++)
                binary[k] = (count & (int)1 << (8 - k - 1)) ? '1' : '0';
            binary[8] = '\0';

            // Transfers binary[] data to 1 byte (thanks to Professor Rudinei Goularte for this)
            for (int k = 0; k < 8; k++)
                buffer = (buffer << 1) | (binary[k] == '1');

            /* Converting value to binary */

            for (int k = 0; k < 8; k++)
                binary2[k] = (value & (int)1 << (8 - k - 1)) ? '1' : '0';
            binary2[8] = '\0';

            for (int k = 0; k < 8; k++)
                buffer2 = (buffer2 << 1) | (binary2[k] == '1');

            // Writes value and its count in file
            fwrite(&buffer2, sizeof(buffer2), 1, file);
            fwrite(&buffer, sizeof(buffer), 1, file);
        }
    }
}

void printHeader(BMPFILEHEADER *bmpFile, BMPINFOHEADER *bmpInfo) {

    // Writing File Header data in stdout
    printf("bfType: %d\n", bmpFile->bfType);
    printf("bfSize: %d\n", bmpFile->bfSize);
    printf("bfReserved1: %d\n", bmpFile->bfReserved1);
    printf("bfReserved2: %d\n", bmpFile->bfReserved2);
    printf("bfOffBits: %d\n", bmpFile->bfOffBits);

    // Writing Info Header data in stdin
    printf("biSize: %d\n", bmpInfo->biSize);
    printf("biWidth: %d\n", bmpInfo->biWidth);
    printf("biHeight: %d\n", bmpInfo->biHeight);
    printf("biPlanes: %d\n", bmpInfo->biPlanes);
    printf("biBitCount: %d\n", bmpInfo->biBitCount);
    printf("biCompression: %d\n", bmpInfo->biCompression);
    printf("biSizeImage: %d\n", bmpInfo->biSizeImage);
    printf("biXPelsPerMeter: %d\n", bmpInfo->biXPelsPerMeter);
    printf("biYPelsPerMeter: %d\n", bmpInfo->biYPelsPerMeter);
    printf("biClrUsed: %d\n", bmpInfo->biClrUsed);
    printf("biClrImportant: %d\n", bmpInfo->biClrImportant);
}

void printComponent(double **component, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%f ", component[i][j]);
        }
        printf("\n");
    }
}

int compress(long int *auxY, long int *auxCb, double *compressRate) {

    char fileName[51];

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER *)malloc(14);
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER *)malloc(40);

    printf("\nEnter image file name with bmp extension (for example: file.bmp) and max of 50 characters.\n");
    printf("\nATTENTION!!! Please, read this before entering your image file name:\n\n- If your image is in images folder, please, type: images/nameOfImage.bmp\n- If its in root folder, type nameOfImage.bmp\n");
    printf("\nYour image file name: ");
    scanf("%50s", fileName);

    // file = fopen("images/8x8.bmp", "rb"); // Openning image that we want to compress.
    FILE *file = fopen(fileName, "rb"); // Openning image that we want to compress.

    if (file == NULL) { // Checking if there was an error opening the image.
        printf("\nError reading file. Did you enter its correct name?");
        return ERROR;
    }

    printf("\nReading image metadata...\n");

    // Reading the bmp file header and info header so we can read image data without troubles.
    if (!readBMPFileHeader(file, bmpFile) || !readBMPInfoHeader(file, bmpInfo))
        return ERROR;

    // printHeader(bmpFile, bmpInfo); Prints headers content

    printf("Starting compression, please wait...\n");

    // Moving our file pointer to the bitmap data region.
    moveToBitmapData(file, bmpFile);

    // We're going to split the RGB channels into these 3 matrices below:
    unsigned char **R = NULL, **G = NULL, **B = NULL;

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

    rgbToYcbcr(R, G, B, Y, Cb, Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    // Applying level shift to Y, Cb and Cr components in order to increase performance on the next steps
    levelShift(Y, -128, getHeight(bmpInfo), getWidth(bmpInfo));
    levelShift(Cb, -128, getHeight(bmpInfo), getWidth(bmpInfo));
    levelShift(Cr, -128, getHeight(bmpInfo), getWidth(bmpInfo));

    // Opening file to save compressed image.
    FILE *compressed = fopen("compressed.bin", "wb+");

    // Writing header from original image before its compression.
    writeHeaders(bmpFile, bmpInfo, compressed);

    // Making sure that we are writing data at the correct place (after the 54 first bytes).
    moveToBitmapData(compressed, bmpFile);

    // Applying dct on components
    Y = dct(Y, getHeight(bmpInfo), getWidth(bmpInfo));
    Cb = dct(Cb, getHeight(bmpInfo), getWidth(bmpInfo));
    Cr = dct(Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    // Applying quantization on components
    Y = quantizationLuminance(Y, getHeight(bmpInfo), getWidth(bmpInfo));
    Cb = quantizationLuminance(Cb, getHeight(bmpInfo), getWidth(bmpInfo));
    Cr = quantizationLuminance(Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    // Applying run-length on components
    runlength(Y, compressed);
    *auxY = ftell(compressed);

    runlength(Cb, compressed);
    *auxCb = ftell(compressed);

    runlength(Cr, compressed);

    // Calculating compressRate
    *compressRate = (100 - (100 * ((double)fileSize(compressed) / (double)fileSize(file))));

    // Free allocated memory.

    fclose(file);       // closes original image file
    fclose(compressed); // closes compressed image file

    free(bmpFile); // frees bmp file image header
    free(bmpInfo); // frees bmp info image header

    freeMatrix(R, getHeight(bmpInfo));
    freeMatrix(G, getHeight(bmpInfo));
    freeMatrix(B, getHeight(bmpInfo));
    freeDoubleMatrix(Y, getHeight(bmpInfo));
    freeDoubleMatrix(Cb, getHeight(bmpInfo));
    freeDoubleMatrix(Cr, getHeight(bmpInfo));

    return SUCCESS;
}

// Below, functions used to create our descompressor

void YcbcrTorgb(unsigned char **R, unsigned char **G, unsigned char **B, double **Y, double **Cb, double **Cr, int height, int width) {

    double Kr = 0.299, Kb = 0.114;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            R[i][j] = convertion(Y[i][j] + (1.402 * Cr[i][j]));
            printf("value: %lf e valor no R: %d\n", Y[i][j] + (1.402 * Cr[i][j]), R[i][j]);
            G[i][j] = convertion(Y[i][j] - (0.344 * Cb[i][j]) - (0.714 * Cr[i][j]));
            B[i][j] = convertion(Y[i][j] + (1.772 * Cb[i][j]));
        }
    }
}

double **idct(double **mat, int height, int width) {

    double aux = 0;
    double **dctCoefs = allocDoubleMatrix(dctCoefs, height, width);

    // First double 'for loop' divides mat into 8x8 pieces and save this slice in mat2
    for (int a = 0; a < ceil(height / 8); a++) {
        for (int b = 0; b < ceil(width / 8); b++) {

            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {

                    aux = 0; // aux variable to store sum values

                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {

                            aux += C[i] * C[j] * mat[a * 8 + i][b * 8 + j] * cosine[x][i] * cosine[y][j];
                        }
                    }

                    dctCoefs[a * 8 + x][b * 8 + y] = 0.25 * aux;
                }
            }
        }
    }

    return dctCoefs;
}

double **runlengthDescomp(double **mat, FILE *file, int height, int width, long int aux) {

    int stop = 0;         // This variable will be used to avoid buffer overflow.
    int j = 0;            // This variable will control 'column' index of **mat.
    unsigned char buffer; // This will store the char representation of counter.
    unsigned char vector; // This will be used to stores the value.
    int count = 0;

    while (stop != 1 && ftell(file) < aux || !feof(file)) {

        // Read value and its count in file
        fread(&vector, sizeof(vector), 1, file);
        fread(&buffer, sizeof(buffer), 1, file);

        count = buffer;

        // As we stored count as an unsigned char, we need to convert it to integer
        for (int i = 0; i < count; i++) {

            mat[i][j] = vector; // saves value

            if (j < height - 1) // checks buffer overflow
                j++;
            else /*if (i == height - 1)*/ { // checks buffer overflow
                j = 0;
                i++;
            }
            stop = 1; // indicates that the next loop will overflow
        }
    }

    return mat;
}

unsigned char convertion(int num) {
    if (num > 255)
        return 255;
    else if (num < 0)
        return 0;
    else
        return (unsigned char)num;
}

double **quantizationLuminanceDescomp(double **component, int height, int width) {

    // LuminanceTable is applied in Y component
    double luminanceTable[8][8] = {16, 11, 10, 16, 24, 40, 51, 61,
                                   12, 12, 14, 19, 26, 58, 60, 55,
                                   14, 13, 16, 24, 40, 57, 69, 56,
                                   14, 17, 22, 29, 51, 87, 80, 62,
                                   18, 22, 37, 56, 68, 109, 103, 77,
                                   24, 35, 55, 64, 81, 104, 113, 92,
                                   49, 64, 78, 87, 103, 121, 120, 101,
                                   72, 92, 95, 98, 112, 100, 103, 99};

    for (int a = 0; a < ceil(height / 8); a++) {
        for (int b = 0; b < ceil(width / 8); b++) {
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    component[a * 8 + i][b * 8 + j] = component[a * 8 + i][b * 8 + j] * luminanceTable[i][j];
        }
    }
    return component;
}

double **quantizationCrominanceDescomp(double **component, int height, int width) {

    // Crominance is applied in Cb and Cr components
    double crominanceTable[8][8] = {17, 18, 24, 47, 99, 99, 99, 99,
                                    18, 21, 26, 66, 99, 99, 99, 99,
                                    24, 26, 56, 99, 99, 99, 99, 99,
                                    47, 66, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99};

    for (int a = 0; a < ceil(height / 8); a++) {
        for (int b = 0; b < ceil(width / 8); b++) {
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    component[a * 8 + i][b * 8 + j] = floor(component[a * 8 + i][b * 8 + j] / crominanceTable[i][j]);
        }
    }

    return component;
}

int descompressor(long int *auxY, long int *auxCb) {

    FILE *file = NULL;
    char fileName[51];

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER *)malloc(14);
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER *)malloc(40);

    printf("\nEnter image file name with bin extension (for example: file.bin) and max of 50 characters.\n");
    printf("\nATTENTION!!! Please, read this before entering your image file name:\n\n- If your image is in images folder, please, type: images/nameOfImage.bin\n- If its in root folder, type nameOfImage.bin\n");
    printf("\nYour image file name: ");
    scanf("%50s", fileName);

    file = fopen(fileName, "rb"); // Openning image that we want to descompress.

    if (file == NULL) { // Checking if there was an error opening the image.
        printf("\nError reading file. Did you enter its correct name?");
        return ERROR;
    }

    printf("\nReading image metadata...\n");

    // Reading the bmp file header and info header so we can read image data without troubles.
    if (!readBMPFileHeader(file, bmpFile) || !readBMPInfoHeader(file, bmpInfo))
        return ERROR;

    printf("Starting descompression, please wait...\n");

    // Moving our file pointer to the bitmap data region.
    moveToBitmapData(file, bmpFile);

    // Creating and allocating data in variables below to store Y, Cb and Cr components.
    double **Y = NULL, **Cb = NULL, **Cr = NULL;

    Y = allocDoubleMatrix(Y, getHeight(bmpInfo), getWidth(bmpInfo));
    Cb = allocDoubleMatrix(Cb, getHeight(bmpInfo), getWidth(bmpInfo));
    Cr = allocDoubleMatrix(Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    // Starting descompression of run-length data
    Y = runlengthDescomp(Y, file, getHeight(bmpInfo), getWidth(bmpInfo), *auxY);

    for (int i = 0; i < getHeight(bmpInfo); i++) {
        for (int j = 0; j < getWidth(bmpInfo); j++) {
            printf("%f ", Y[i][j]);
        }
        printf("\n");
    }

    // Cb = runlengthDescomp(Cb, file, getHeight(bmpInfo), getWidth(bmpInfo), *auxCb);
    // Cr = runlengthDescomp(Cr, file, getHeight(bmpInfo), getWidth(bmpInfo), EOF);

    // // Starting to undo quantization
    // Y = quantizationLuminanceDescomp(Y);
    // Cb = quantizationCrominanceDescomp(Cb);
    // Cr = quantizationCrominanceDescomp(Cr);

    // // Starting IDCT step
    // Y = idct(Y, getHeight(bmpInfo), getWidth(bmpInfo));
    // Cb = idct(Cb, getHeight(bmpInfo), getWidth(bmpInfo));
    // Cr = idct(Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    // // Applying level shift
    // levelShift(Y, 128, getHeight(bmpInfo), getWidth(bmpInfo));
    // levelShift(Cb, 128, getHeight(bmpInfo), getWidth(bmpInfo));
    // levelShift(Cr, 128, getHeight(bmpInfo), getWidth(bmpInfo));

    // // Converting from YCbCr to RGB
    // unsigned char **R = NULL, **G = NULL, **B = NULL;

    // // Allocating enough memory to store R, G and B channels.
    // R = allocMatrix(R, getHeight(bmpInfo), getWidth(bmpInfo));
    // G = allocMatrix(G, getHeight(bmpInfo), getWidth(bmpInfo));
    // B = allocMatrix(B, getHeight(bmpInfo), getWidth(bmpInfo));

    // YcbcrTorgb(R, G, B, Y, Cb, Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    // // Starting writing in descompressed file
    // FILE *descompressed = fopen("descompressed.bmp", "wb+");

    // // Writing file and info header at the beginning of descompressed file
    // writeHeaders(bmpFile, bmpInfo, descompressed);

    // // Writing B, G and R component to file.
    // // We take care of values lower 0 and greater 255 with convertion() function;
    // for (int i = 0; i < getHeight(bmpInfo); i++) {
    //     for (int j = 0; j < getWidth(bmpInfo); j++) {
    //         fputc(B[i][j], descompressed);
    //         fputc(G[i][j], descompressed);
    //         fputc(R[i][j], descompressed);
    //     }
    // }

    // free(bmpFile);
    // free(bmpInfo);

    // fclose(file);
    // fclose(descompressed);

    // freeDoubleMatrix(Y, getHeight(bmpInfo));
    // freeDoubleMatrix(Cb, getHeight(bmpInfo));
    // freeDoubleMatrix(Cr, getHeight(bmpInfo));

    return 1;
}