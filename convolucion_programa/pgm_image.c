#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgm_image.h"

/* Lectura del archivo de imagen, se extrae datos a estructura PGMImage */
PGMImage* leer_imagen (char* filename){
    byte* image;int width;int height;
    if(ReadPGM(filename,&image,&width,&height)==1){
        fprintf(stderr,"Archivo no encontrado\n");
        return NULL;
    }
    PGMImage* pgm_image = malloc(sizeof(PGMImage));
    pgm_image->image=image;pgm_image->width=width;pgm_image->height=height;
    return pgm_image;
}

/* División de imagen por cantidad de procesadores*/
struct BloqueImagen* dividir_imagen(PGMImage* image, int cantidad){
    struct BloqueImagen *bloques = malloc((cantidad) * sizeof(struct BloqueImagen));
    
    int nWidth = (image->width); int nHeight = (image->height)/cantidad;
    int size_divided = (image->height * image->width) / cantidad;
    
    for(int i = 0; i<cantidad;i++){
        bloques[i].image = image;
        bloques[i].width = nWidth;
        bloques[i].height = nHeight;
        bloques[i].begin = i * nHeight * nWidth;
        bloques[i].end = bloques[i].begin + bloques[i].height * nWidth;
    }

    return bloques;
}

/* Une los resultados de las convoluciones de los bloques */
void unificar_bloques(struct BloqueImagen* bloques, int cantidad, byte* resultado){
    for (int i = 0; i < cantidad; i++) {
        int width = bloques[i].width;
        int height = bloques[i].height;
        int begin = bloques[i].begin;

        for (int j = 0; j < height; j++) 
            memcpy(resultado + begin + j * width, bloques[i].result + j * width, sizeof(byte) * width);
    }
    free(bloques);
}