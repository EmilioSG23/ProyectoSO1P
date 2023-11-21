#include <stdio.h>
#include <stdlib.h>

#include "pgm.h"
#include "filtros.h"

void aplicar_convolucion(struct PGMImage* image){
    byte* resultado = malloc(image->width * image->height);
    switch (image -> filtro_aplicar){
        case 1:
            aplicar_sobel(image, resultado);
            break;
        case 2:
            aplicar_blur(image, resultado);
            break;
        case 3:
            aplicar_sharpen(image, resultado);
            break;
        case 4:
            aplicar_identity(image, resultado);
            break;
        default:
            fprintf(stderr,"Hubo un error al momento de realizar la convolución\n");
            free(resultado);
            break;
    }
    /* Asignación de respuesta */
    image->image = resultado;
}

void aplicar_filtro(struct PGMImage* image, byte* resultado, double conv[9]){
    double nSum = 0;
    for(int i = 0; i<9;i++)
        nSum += conv[i];
    if(nSum == 0) nSum = 1;
    /* Aplicación de la convolución */
    FrameConv3x3(image->image,resultado,image->width,image->height,conv,nSum);
}

void aplicar_sobel (struct PGMImage* image, byte* resultado){
    /* Creación de máscara top sobel */
    double conv[9] = {1, 2, 1,
                0, 0, 0,
                -1,-2,-1};
    /* Aplicación de filtro */
    aplicar_filtro(image,resultado,conv);
}
void aplicar_blur (struct PGMImage* image, byte* resultado){
    /* Creación de máscara blur */
    double conv[9] = {0.0625, 0.125, 0.0625,
                    0.125, 0.25, 0.125,
                    0.0625, 0.125, 0.0625};
    /* Aplicación de filtro */
    aplicar_filtro(image,resultado,conv);
}
void aplicar_sharpen (struct PGMImage* image, byte* resultado){
    /* Creación de máscara sharpen */
    double conv[9] = {0, -1, 0,
                    -1, 5, -1,
                    0,-1, 0};
    /* Aplicación de filtro */
    aplicar_filtro(image,resultado,conv);
}
void aplicar_identity (struct PGMImage* image, byte* resultado){
    /* Creación de máscara identity */
    double conv[9] = {0, 0, 0,
                    0, 1, 0,
                    0, 0, 0};
    /* Aplicación de filtro */
    aplicar_filtro(image,resultado,conv);
}