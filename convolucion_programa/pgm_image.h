#ifndef PGM_IMAGE_H
#define PGM_IMAGE_H

#include "pgm.h"

/* Estructura que representa una PGMImage */
typedef struct PGMImage{
    char* input;
    char* output;
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

PGMImage* leer_imagen (char* filename);
struct BloqueImagen* dividir_imagen(PGMImage* image, int cantidad);
void unificar_bloques(struct BloqueImagen* bloques, int cantidad, byte* resultado);

#endif