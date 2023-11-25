#ifndef FILTROS_H
#define FILTROS_H

#include "pgm.h"
#include "pgm_image.h"

int verificar_filtro(char* filtro);

void aplicar_convolucion(struct BloqueImagen* bloque);

void aplicar_filtro(struct BloqueImagen* bloque, double conv[9]);

void aplicar_sobel (struct BloqueImagen* bloque);
void aplicar_blur (struct BloqueImagen* bloque);
void aplicar_sharpen (struct BloqueImagen* bloque);
void aplicar_identity (struct BloqueImagen* bloque);

void aplicar_convolucion3x3_bloque(struct BloqueImagen* bloque, double conv[9], double denom);

#endif