#ifndef FILTROS_H
#define FILTROS_H

#include "pgm.h"

/* Estructura que representa una PGMImage */
typedef struct PGMImage{
    char* filename;
    byte* image;
    int width;
    int height;
    int filtro_aplicar;
}PGMImage;

void aplicar_convolucion(struct PGMImage* image);

void aplicar_filtro(struct PGMImage* image, byte* resultado, double conv[9]);

void aplicar_sobel (struct PGMImage* image, byte* resultado);
void aplicar_blur (struct PGMImage* image, byte* resultado);
void aplicar_sharpen (struct PGMImage* image, byte* resultado);
void aplicar_identity (struct PGMImage* image, byte* resultado);

#endif