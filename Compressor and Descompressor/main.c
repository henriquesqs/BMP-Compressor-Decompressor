/*
    Drive program to run compressor and descompressor of BMP images.

    @authors:
        @henriquesqs
        @DennisLemkeGreen
    
    University of Sao Paulo (USP) - 2020
    Multimedia subject with Professor Rudinei Goularte.
*/

#include "compressor.h"
#include "descompressor.h"
#include <time.h>

// Function responsible for showing options menu.
void menu() {

    printf("\nEnter your desired option:\n\n");
    printf("1. Compress an image;\n");
    printf("2. Descompress an image;\n");
    printf("\n0. Exit.\n");
    printf("\nOption: ");
}

int main(int argc, char const *argv[]) {

    int error = 0;           // Variable to control functions errors return.
    int option = 0;          // Variable to store users option.
    double duration = 0;     // Variable to store duration of compression and descompression functions.
    double compressRate = 0; // Variable to stores compression rate.

    clock_t timeBefore = 0, timeAfter = 0; // Variables to control compression and descompression running time.

    // Initializing cosine look-up table.
    initCosLUT();

    while (true) {

        menu(0); // Showing option menu.

        scanf("%d", &option); // Waiting for user to choose an option.

        switch (option) {

        case 0: // Option 0: exit program.
            break;

        case 1: // Option 1: image compression.

            timeBefore = timeAfter = 0; // Initializing values.

            timeBefore = clock(); // Saving time before starting image compression.

            // If everything went right during compression, calculates its duration.
            if (compress(&compressRate) > 0) {
                timeAfter = clock();                                          // saving time after starting image compression
                duration = (timeAfter - timeBefore) / (double)CLOCKS_PER_SEC; // calculating duration of compression process

                printf("\nImage successfully compressed in %lf seconds with a compress rate of %.lf percent!\n", duration, compressRate);
                printf("The resulting file (compressed.bin) is located at the root of this project.\n");
            }

            // Otherwise, compress() will print an error message to stdout and we will stop the program.
            else {
                error = 1;
                option = 0;
                break;
            }

            break;

        case 2: // Option 2: image descompression.

            timeBefore = timeAfter = 0; // Initializing values

            timeBefore = clock(); // Saving time before starting image compression.

            // If everything went right during descompression, calculates its duration.
            if (descompressor() > 0) {

                timeAfter = clock();
                duration = (timeAfter - timeBefore) / (double)CLOCKS_PER_SEC;

                printf("\nImage successfully descompressed in %lf seconds!\n", duration);
                printf("The resulting file (descompressed.bmp) is located at the root of this project.\n");

            } else {
                error = 1;
                option = 0;
            }
            break;

        default: // Default option: when users enters an invalid option.
            printf("\nSorry, invalid option. Do you want to exit? Enter 0.\n");
            break;
        }

        if (option == 0)
            break;
    }

    if (error)
        printf("\n\nSorry for this...\nIf this error persists, contact us by our github accounts: @henriquesqs or @DennisLemkeGreen\n");
    else
        printf("\nThanks for using our services! Have a nice day =)\n");

    return SUCCESS;
}
