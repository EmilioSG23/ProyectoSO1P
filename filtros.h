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

struct BloqueImagen{
    PGMImage* image;
    byte* result;
    int width;
    int height;
    int begin;
    int end;
};

void aplicar_convolucion(struct BloqueImagen* bloque);

void aplicar_filtro(struct BloqueImagen* bloque, double conv[9]);

void aplicar_sobel (struct BloqueImagen* bloque);
void aplicar_blur (struct BloqueImagen* bloque);
void aplicar_sharpen (struct BloqueImagen* bloque);
void aplicar_identity (struct BloqueImagen* bloque);

void aplicar_convolucion3x3_bloque(struct BloqueImagen* bloque, double conv[9], double denom);

#endif