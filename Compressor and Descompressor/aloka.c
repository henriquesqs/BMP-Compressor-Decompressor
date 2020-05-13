#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#define PI 3.14159265358979323846

int main(int argc, char const *argv[]) {

    char c = 0;
    int count = 2;

    c = count;
    printf("c: %d\n", c);

    c = 5;
    count = c;
    printf("count: %d\n", count);

    return 0;
}
