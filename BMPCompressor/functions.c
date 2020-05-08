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

double cosine[8][8]; // global variable to store cosine values to later be used in DCT

// Below, all the functions used to create our BMP compressor

void initCosLUT() {
    for (int i = 0; i < 8; i++) {
        for (int x = 0; x < 8; x++) {

            cosine[i][x] = 0;
            cosine[i][x] = cos(((((2.0 * i) + 1.0) * (x * PI)) / (16)));
        }
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

    long int position = ftell(file);

    fseek(file, 0, SEEK_END);

    long int size = ftell(file);

    fseek(file, position, SEEK_SET);

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
    fseek(file, FH->bfOffBits, SEEK_SET);
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

double **dct(double **dctCoefs, double **mat) {

    float c1 = 0, c2 = 0, aux = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            c1 = c2 = 1; // default value of consts

            if (i == 0)
                c1 = (1 / sqrt(2));

            if (j == 0)
                c2 = (1 / sqrt(2));

            aux = 0; // aux variable to store sum values

            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {

                    aux += mat[x][y] * cosine[x][i] * cosine[y][j];
                }
            }

            dctCoefs[i][j] = c1 * c2 * 0.25 * aux;
        }
    }

    return dctCoefs;
}

double **divideMatrices(int lum, FILE *compressed, double **component, int height, int width) {

    double **mat = allocDoubleMatrix(mat, 8, 8);                // 'mat' will allocate each 8x8 piece of component
    double **dctCoefs = allocDoubleMatrix(dctCoefs, 8, 8);      // 'dctCoefs' will temporally allocate values after dct
    unsigned char *vector = malloc(64 * sizeof(unsigned char)); // 'vector' will be used to store values after vectorization

    // Applying level shift to 'component' in order to increase performance on the next steps
    levelShift(component, -128, height, width);

    // On this next logical block, we are dividing 'component' into 8x8 matrices
    // in order to apply dct, quantization and vectorization into each one of them.

    for (int i = 0; i < height / 8; i++) {
        for (int j = 0; j < width / 8; j++) {

            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {

                    // We are just copying a 8x8 part of 'component' matrix to apply
                    // dct in each 8x8 part in order to increase its performance.
                    mat[k][l] = component[i * 8 + k][j * 8 + l];
                }
            }

            // emptying dctCoefs
            for (int a = 0; a < 8; a++)
                for (int b = 0; b < 8; b++)
                    dctCoefs[a][b] = 0;

            // emptying vector
            for (int a = 0; a < 64; a++)
                vector[a] = 0;

            dct(dctCoefs, mat);

            // idct(dctCoefs, mat);

            if (lum)
                quantizationLuminance(dctCoefs);
            else
                quantizationCrominance(dctCoefs);

            // On this step, we're going to apply vectorization using zig-zag scan. We do this to
            // make easier for us to compress the image by moving all the zero values to the end of the vector.
            // Its told that this step helps to increase run-length encoding performance.

            vectorization(vector, dctCoefs);

            // Applying run-length to compress data on quantified vector. Also, we output the compressed file (compressed.bin)
            runlength(vector, compressed);
        }
    }

    free(vector);
    freeDoubleMatrix(mat, 8);
    freeDoubleMatrix(dctCoefs, 8);

    return component;
}

double **quantizationLuminance(double **component) {

    // LuminanceTable is applied in Y component
    double luminanceTable[8][8] = {16, 11, 10, 16, 24, 40, 51, 61,
                                   12, 12, 14, 19, 26, 58, 60, 55,
                                   14, 13, 16, 24, 40, 57, 69, 56,
                                   14, 17, 22, 29, 51, 87, 80, 62,
                                   18, 22, 37, 56, 68, 109, 103, 77,
                                   24, 35, 55, 64, 81, 104, 113, 92,
                                   49, 64, 78, 87, 103, 121, 120, 101,
                                   72, 92, 95, 98, 112, 100, 103, 99};

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)

            component[i][j] = floor(component[i][j] / luminanceTable[i][j]);

    return component;
}

double **quantizationCrominance(double **component) {

    // Crominance is applied in Cb and Cr components
    double crominanceTable[8][8] = {17, 18, 24, 47, 99, 99, 99, 99,
                                    18, 21, 26, 66, 99, 99, 99, 99,
                                    24, 26, 56, 99, 99, 99, 99, 99,
                                    47, 66, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99,
                                    99, 99, 99, 99, 99, 99, 99, 99};

    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            component[i][j] = floor(component[i][j] / crominanceTable[i][j]);

    return component;
}

void vectorization(unsigned char *vector, double **mat) {

    int dir = -1;         // Every time dir is < 0, go down. Otherwise, go right.
    int steps = 0;        // Variable to avoid buffer overflow.
    int lin = 0, col = 0; // Variables to control lines and col from double**.

    vector[steps++] = mat[lin][col];

    while (steps < 64) {

        if (lin == 0) { // it means we cant go up anymore

            if (dir < 0) {
                col++;
                vector[steps++] = mat[lin][col];
                dir *= -1;
            }

            else {
                while (col > 0) {
                    lin++;
                    col--;
                    vector[steps++] = mat[lin][col];
                }
            }
        }

        else if (col == 0) {

            if (dir > 0) {
                lin++;
                if (lin == 8)
                    break;
                vector[steps++] = mat[lin][col];
                dir *= -1;
            }

            else {
                while (lin > 0 && col < 8) {
                    lin--;
                    col++;
                    vector[steps++] = mat[lin][col];
                }
            }
        }
    }
}

void rgbToYcbcr(unsigned char **R, unsigned char **G, unsigned char **B, double **Y, double **Cb, double **Cr, int height, int width) {

    float Kr = 0.299, Kb = 0.114;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            Y[i][j] = Kr * R[i][j] + (0.587) * G[i][j] + Kb * B[i][j];
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

void runlength(unsigned char *vector, FILE *file) {

    int count = 0;        // This variable will count occurrences of the same value until a different one is found.
    char binary[9];       // This will stores the binary representation of 'count'.
    unsigned char buffer; // This will stores the char representation of binary, i.e, count.

    for (int i = 1; i < 64; i++) { // 'i' starts at position 1 because position 0 has a DC coefficient and run-length must be applied in AC coefficientes

        // Counting occurrences of current value (while avoiding buffer overflow).
        count = 1;
        while (i < 63 && vector[i] == vector[i + 1]) {
            count++;
            i++;
        }

        // When finds a different value, starts preparation to
        // write the previous value and its count in file.

        // Converting count to binary
        // Thanks to Fifi from StackOverflow for his answer (https://stackoverflow.com/a/58940759/10304974) :)
        for (int j = 0; j < 8; j++)
            binary[j] = (count & (int)1 << (8 - j - 1)) ? '1' : '0';
        binary[8] = '\0';

        // Transfers binary[] data to 1 byte (thanks to Professor Rudinei Goularte for this)
        for (int j = 0; j < 8; j++)
            buffer = (buffer << 1) | (binary[j] == '1');

        // Writes value and its count in file
        fwrite(&vector[i], sizeof(vector[i]), 1, file);
        fwrite(&buffer, sizeof(buffer), 1, file);
    }
}

int compress(long int *auxY, long int *auxCb, double *compressRate) {

    FILE *file = NULL;
    char fileName[51];

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER *)malloc(14);
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER *)malloc(40);

    printf("\nEnter image file name with bmp extension (for example: file.bmp) and max of 50 characters.\n");
    printf("\nATTENTION!!! Please, read this before entering your image file name:\n\n- If your image is in images folder, please, type: images/nameOfImage.bmp\n- Otherwise, type nameOfImage.bmp\n");
    printf("\nYour image file name: ");
    scanf("%50s", fileName);

    // file = fopen("images/8x8.bmp", "rb"); // Openning image that we want to compress.
    file = fopen(fileName, "rb"); // Openning image that we want to compress.

    if (file == NULL) { // Checking if there was an error opening the image.
        printf("\nError reading file. Did you enter its correct name?");
        return ERROR;
    }

    printf("\nReading image metadata...\n");

    // Reading the bmp file header and info header so we can read image data without troubles.
    if (!readBMPFileHeader(file, bmpFile) || !readBMPInfoHeader(file, bmpInfo))
        return ERROR;

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

    // Dividing each component into 8x8 matrices in order to use DCT (Discrete Cosine Transform) algorithm,
    // apply quantization and vectorization steps at each 8x8 matrix, due to some researchs proving that this
    // division increases the efficiency of these steps.

    FILE *compressed = fopen("compressed.bin", "wb+"); // File to save compressed image

    // Writing header from image before its compression.
    writeHeaders(bmpFile, bmpInfo, compressed);

    divideMatrices(1, compressed, Y, getHeight(bmpInfo), getWidth(bmpInfo));
    auxY[0] = ftell(compressed);

    divideMatrices(0, compressed, Cb, getHeight(bmpInfo), getWidth(bmpInfo));
    auxCb[0] = ftell(compressed);

    divideMatrices(0, compressed, Cr, getHeight(bmpInfo), getWidth(bmpInfo));

    // Calculating compressRate
    *compressRate = (100 - (100 * ((double)fileSize(compressed) / (double)fileSize(file))));

    // Free allocated memory.
    fclose(file);       // closes original image file
    free(bmpFile);      // frees bmp file image header
    free(bmpInfo);      // frees bmp info image header
    fclose(compressed); // closes compressed image file
    freeMatrix(R, getHeight(bmpInfo));
    freeMatrix(G, getHeight(bmpInfo));
    freeMatrix(B, getHeight(bmpInfo));
    freeDoubleMatrix(Y, getHeight(bmpInfo));
    freeDoubleMatrix(Cb, getHeight(bmpInfo));
    freeDoubleMatrix(Cr, getHeight(bmpInfo));

    return SUCCESS;
}

// Below, functions used to create our descompressor

double **idct(double **dctCoefs, double **mat) {

    float c1 = 0, c2 = 0, aux = 0;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {

            c1 = c2 = 1; // default value of consts

            aux = 0; // aux variable to store sum values

            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {

                    if (i == 0)
                        c1 = (1 / sqrt(2));

                    if (j == 0)
                        c2 = (1 / sqrt(2));

                    aux += c1 * c2 * mat[i][j] * cosine[x][i] * cosine[y][j];
                }
            }

            dctCoefs[x][y] = 0.25 * aux;
        }
    }

    return dctCoefs;
}

float **runlengthDescomp(int height, int width, FILE *file, long int *aux) {

    int counter = 0, times, value, x, y;
    int **component = allocIntMatrix(component, height, width);

    while (counter < aux[0] && !feof(file)) {

        fread(&value, sizeof(int), 1, file);
        fread(&times, sizeof(int), 1, file);

        for (int i = 0; i < times; i++) {
            component[x][y] = value;

            if (y < width - 1)
                y++;
            else if (x < height - 1) {
                y = 0;
                x++;
            } else
                break;
        }

        counter++;
    }

    freeIntMatrix(component, height);
}

int descompressor(BMPINFOHEADER *infoHeader, FILE *compressed, long int *auxY, long int *auxCb) {

    FILE *file = NULL;
    char fileName[51];

    BMPFILEHEADER *bmpFile = (BMPFILEHEADER *)malloc(14);
    BMPINFOHEADER *bmpInfo = (BMPINFOHEADER *)malloc(40);

    printf("\nEnter image file name with bmp extension (for example: file.bmp) and max of 50 characters: ");
    scanf("%50s", fileName);

    file = fopen(fileName, "rb"); // Openning image that we want to descompress.

    if (file == NULL) { // Checking if there was an error opening the image.
        printf("\nError reading file. Did you enter its correct name?");
        return ERROR;
    }

    // Reading the bmp file header and info header so we can read image data without troubles.
    if (!readBMPFileHeader(file, bmpFile) || !readBMPInfoHeader(file, bmpInfo))
        return ERROR;

    // Moving our file pointer to the bitmap data region.
    moveToBitmapData(file, bmpFile);

    runlengthDescomp(getHeight(infoHeader), getWidth(infoHeader), compressed, auxY);
}