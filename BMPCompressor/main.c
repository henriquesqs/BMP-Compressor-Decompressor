#include "compressor.h"
#include "descompressor.h"
#include <time.h>

/*
    Function responsible for showing options menu.

    PARAMETERS:
        - invalid: flag to know if we're showing this menu after an invalid input.
*/
void menu(int invalid) {

    if (invalid)
        printf("\nSorry, invalid option. Do you want to exit? Enter 0.\n\n");

    printf("\nEnter your desired option:\n\n");
    printf("1. Compress an image;\n");
    printf("2. Descompress an image;\n");
    printf("\n0. Exit.\n");
    printf("\nOption: ");
}

int main(int argc, char const *argv[]) {

    int option = 0;      // variable to store users option.
    int error = 0;   // variable to control functions errors return.
    double duration = 0; // variable to store duration of compression and descompression functions.

    double *compressRate = malloc(sizeof(double));  // variable to stores compression rate.
    long int *auxY = malloc(1 * sizeof(long int));  // variable to auxiliate in descompression.
    long int *auxCb = malloc(1 * sizeof(long int)); // variable to auxiliate in descompression.

    clock_t timeBefore = 0, timeAfter = 0; // variables to control compression and descompression running time.

    menu(0);              // showing option menu
    scanf("%d", &option); // waiting for user to choose an option

    while (true) {

        switch (option) {

        case 0: // Option 0: exit program
            break;

        case 1: // Option 1: image compression

            timeBefore = timeAfter = 0; // initializing values

            timeBefore = clock(); // saving time before starting image compression

            // If everything went right during compression, calculates its duration.
            if (compress(auxY, auxCb, compressRate) > 0) {
                timeAfter = clock() - timeBefore;                     // saving time after starting image compression
                duration = (timeAfter - timeBefore) / CLOCKS_PER_SEC; // calculating duration of compression process

                printf("\nImage successfully compressed in %.3lf seconds with a compress rate of %.lf percent!\n", duration, compressRate[0]);
                printf("The resulting file (compressed.bin) is located at the root of this project.\n");
            }

            // Otherwise, compress() will print an error message to stdout and we will stop the program.
            else {
                error = 1;
                option = 0;
                break;
            }


            break;

        case 2: // Option 1: image descompression
           
            timeBefore = timeAfter = 0; // initializing values
           
            // if(descompressor(bmpInfo, compressed, auxY, auxCb, 0) > 0){
            //     timeAfter = clock() - timeBefore;
            //     duration = (timeAfter - timeBefore) / CLOCKS_PER_SEC;

            //     printf("\nImage successfully descompressed in %.3lf seconds!\n", duration);
            //     printf("The resulting file (descompressed.bmp) is located at the root of this project.\n");

            // }
            // else{
            //     error = 1;
            //     option = 0;
            // }
            break;

        default: // Default option: when users enters an invalid option
            menu(1);
            scanf("%d", &option);
            break;
        }

        if (option == 0)
            break;
        else if (option <= 2) {
            menu(0);
            scanf("%d", &option);
        }
    }

    if(error)
        printf("\n\nSorry for this...\nIf this error persists, contact us by our github accounts: @henriquesqs or @DennisLemkeGreen\n");
    else
        printf("\nThanks for using our services! Have a nice day =)\n");

    free(compressRate);
    free(auxY);
    free(auxCb);

    return SUCCESS;
}
