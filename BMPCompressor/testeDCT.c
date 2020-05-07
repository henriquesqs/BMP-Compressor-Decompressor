#include "compressor.h"

int main(int argc, char const *argv[]) {

    double c1 = 0, c2 = 0;

    float mat[8][8] = {{1, 19, 37, 55, 73, 91, 109, 127},         //512
                        {19, 37, 55, 73, 91, 109, 127, 145},       //656
                        {37, 55, 73, 91, 109, 127, 145, 163},      //800
                        {55, 73, 91, 109, 127, 145, 163, 181},     //944
                        {73, 91, 109, 127, 145, 163, 181, 199},    //1088
                        {91, 109, 127, 145, 163, 181, 199, 217},   //1232
                        {109, 127, 145, 163, 181, 199, 217, 235},  // 1376
                        {127, 145, 163, 181, 199, 217, 235, 253}}; // 1520

    float dctCoefs[8][8];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            dctCoefs[i][j] = 0;
            c1 = c2 = 1;

            if (i == 0)
                c1 = (1 / sqrt(2));

            if (j == 0)
                c2 = (1 / sqrt(2));

            float aux = 0;

            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {

                    aux += mat[x][y] * cos((2 * x + 1) * i * PI / 16) * cos((2 * y + 1) * j * PI / 16);
                }
            }
            dctCoefs[i][j] = (c1 * c2 * 1 / 4 * aux);
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            printf("%.2f ", dctCoefs[i][j]);
        }
        printf("\n");

    }

    return 0;
}
