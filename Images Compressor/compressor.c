#include "compressor.h"

int main(int argc, char const *argv[])
{
    FILE *file;
    BMPINFOHEADER * bmpInfo;
    BMPFILEHEADER * bmpFile;

    file = fopen("images/1000x624.bmp", "r");

    if(file == NULL) printf("error");
}
