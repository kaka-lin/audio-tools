#include <string.h>
#include <stdlib.h>

#include "array.h"

double *array_add(double *a, double *b, int len)
{
    double *c = calloc(len, sizeof(double));
    for (int i = 0; i < len; i++) {
        c[i] = a[i] + b[i];
    }

    return c;
}

double *array_sub(double *a, double *b, int len)
{
    double *c = calloc(len, sizeof(double));
    for (int i = 0; i < len; i++) {
        c[i] = a[i] - b[i];
    }

    return c;
}
