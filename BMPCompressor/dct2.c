float **divideMatrices(float **component, float **dctCoefs, int height, int width) {

    // for (int i = 0; i < height; i++) {
    //     for (int j = 0; j < width; j++) {
    //         printf("%.f ", component[i][j]);
    //     }
    //     printf("\n");
    // }

    // 'mat' will allocate each 8x8 piece of component
    float sum = 0;
    float **mat = NULL;
    mat = allocFloatMatrix(mat, 8, 8);

    // On this logical block, we are dividing component into 8x8 matrices in order to apply dct into each one of them.
    for (int i = 0; i < ceil(height / 8); i++) {
        for (int j = 0; j < ceil(width / 8); j++) {
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {

                    sum = 0;

                    for (int m = 0; m < 8; m++) {
                        for (int n = 0; n < 8; n++) {
                            sum += component[i * 8 + m][j * 8 + n] - 128 * COS[m][k] * COS[n][l];
                        }
                    }

                    sum *= C[k] * C[l] * 0.25;
                    dctCoefs[i * 8 + k][j * 8 + l] = sum;

                    // We are just copying a 8x8 part of component matrix to apply
                    // dct in each 8x8 part in order to increase its performance.
                    // mat[k][l] = component[i * 8 + k][j * 8 + l];
                }
            }

            // component = dct(dctCoefs, mat);
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%.f ", dctCoefs[i][j]);
        }
        printf("\n");
    }

    freeFloatMatrix(mat, 8);
    return dctCoefs;
}