#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <linux/time.h>

#include "filtros.h"
#include "pgm.h"

int core_numbers;

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

struct BloqueImagen* dividir_imagen(PGMImage* image){
    struct BloqueImagen *bloques = malloc((core_numbers) * sizeof(struct BloqueImagen));
    
    int nWidth = (image->width); int nHeight = (image->height)/core_numbers;
    int size_divided = (image->height * image->width) / core_numbers;
    
    for(int i = 0; i<core_numbers;i++){
        bloques[i].image = image;
        bloques[i].width = nWidth;
        bloques[i].height = nHeight;
        bloques[i].begin = i * nHeight * nWidth;
        bloques[i].end = bloques[i].begin + bloques[i].height * nWidth;
    }

    return bloques;
}

void unificar_bloques(struct BloqueImagen* bloques, byte* resultado){
    for (int i = 0; i < core_numbers; i++) {
        int width = bloques[i].width;
        int height = bloques[i].height;
        int begin = bloques[i].begin;

        for (int j = 0; j < height; j++)
            memcpy(resultado + begin + j * width, bloques[i].result + j * width, sizeof(byte) * width);
    }
}

int main (){
    fprintf(stderr, "==== TEST ====\n");
    /* Cantidad de núcleos que presenta un computador */
    core_numbers = sysconf(_SC_NPROCESSORS_ONLN);
    PGMImage* imagen = leer_imagen("images/tank.pgm");
    imagen -> filtro_aplicar = 1;
    imagen -> filename = "tank.pgm";

    struct timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME,&time_start);
    int size = (imagen->width)*(imagen->height);

    struct BloqueImagen* bloques = dividir_imagen(imagen);
    for (int i = 0; i < core_numbers; i++){fprintf(stderr,"Bloque %d\n",i);
        aplicar_convolucion(&bloques[i]);}

    /* Unificación de bloques */
    byte* resultado = malloc(sizeof(byte)*size);
    unificar_bloques(bloques,resultado);

    /* Obtención de tiempo de convolución */
    clock_gettime(CLOCK_REALTIME,&time_end);
    double time_executed = (time_end.tv_sec - time_start.tv_sec) + (double) (time_end.tv_nsec - time_start.tv_nsec) / 1000000000L;
    fprintf(stderr,"Tiempo de ejecución de convolución: %lf ms\n",time_executed);

    /* Escritura de bloque */
    char new_filename[128]="results/";strcat(new_filename,imagen->filename);
    WritePGM(new_filename,resultado,imagen->width,imagen->height);

    /* Liberación de recursos */
    for (int i = 0; i < core_numbers; i++)
        free(bloques[i].result);
    free(bloques);
    free(resultado);
    free(imagen->image);
    free(imagen);
}