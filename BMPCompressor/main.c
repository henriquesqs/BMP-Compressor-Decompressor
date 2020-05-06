#include "compressor.h"
#include "descompressor.h"
#include <time.h>

void menu(int again) {

    if (again)
        printf("\nSorry, invalid option. Do you want to exit? Enter 0.\n\n");

    printf("\nEnter your desired option:\n\n");
    printf("1. Compress an image;\n");
    printf("2. Descompress an image;\n");
    printf("\n0. Exit.\n");
    printf("\nOption: ");
}

int main(int argc, char const *argv[]) {

    int option;                    // variable to store users option.
    clock_t timeBefore, timeAfter; // variables to control compression and descompression running time.
    double duration;               // variable to store duration of compression and descompression functions.
    int error;                     // variable to control functions errors return.

    menu(0);              // showing option menu
    scanf("%d", &option); // waiting for user to choose an option

    while (true) {

        switch (option) {

        case 0: // Option 0: exit program
            break;

        case 1: // Option 1: image compression

            timeBefore = clock(); // saving time before starting image compression

            // If everything went right during compression, calculates its duration.
            if (compress() > 0) {
                timeAfter = clock() - timeBefore;                     // saving time after starting image compression
                duration = (timeAfter - timeBefore) / CLOCKS_PER_SEC; // calculating duration of compression process
                printf("\nImage successfully compressed in %.3lf seconds!\n", duration);
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
            // descompress();
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

    if (error == 0)
        printf("\nThanks for using our services! Have a nice day =)\n");
    else
        printf("\n\nSorry for this. If this error persists, contact us by our github accounts: @henriquesqs or @DennisLemkeGreen\n");

    return SUCCESS;
}
