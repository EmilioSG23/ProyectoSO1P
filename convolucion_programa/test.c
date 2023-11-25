#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "pgm.h"
#include "pgm_image.h"
#include "filtros.h"

int core_numbers;

int main (){
    fprintf(stderr, "==== TEST ====\n");
    /* Cantidad de núcleos que presenta un computador */
    core_numbers = sysconf(_SC_NPROCESSORS_ONLN);
    PGMImage* imagen = leer_imagen("images/tank.pgm");
    imagen -> filtro_aplicar = 1;
    imagen -> input = "tank.pgm";
    imagen -> output = "results/test.pgm";

    struct timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME,&time_start);
    int size = (imagen->width)*(imagen->height);

    struct BloqueImagen* bloques = dividir_imagen(imagen,core_numbers);
    for (int i = 0; i < core_numbers; i++){fprintf(stderr,"Bloque %d\n",i);
        aplicar_convolucion(&bloques[i]);}

    /* Unificación de bloques */
    byte* resultado = malloc(sizeof(byte)*size);
    unificar_bloques(bloques,core_numbers,resultado);

    /* Obtención de tiempo de convolución */
    clock_gettime(CLOCK_REALTIME,&time_end);
    double time_executed = (time_end.tv_sec - time_start.tv_sec) + (double) (time_end.tv_nsec - time_start.tv_nsec) / 1000000000L;
    fprintf(stderr,"Tiempo de ejecución de convolución: %lf ms\n",time_executed);

    /* Escritura de bloque */
    WritePGM(imagen->output,resultado,imagen->width,imagen->height);

    /* Liberación de recursos */
    for (int i = 0; i < core_numbers; i++)
        free(bloques[i].result);
    free(bloques);
    free(resultado);
    free(imagen->image);
    free(imagen);
}