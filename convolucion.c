#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include "pgm.h"
#include "filtros.h"

#define SIZE 128 //Tamaño de buffer del scanf para path_file
int core_numbers;
PGMImage* pgm_image;

/* Obtener filename de un path */
char* obtenerFilename(char* path_file){
    char* filename;
    char* token = strtok(path_file,"/");
    while(token != NULL) {
        filename=token;
        token = strtok(NULL, "/");
    }
    return filename;
}

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

/* División de imagen en cantidad de procesadores bloques*/
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

/* Procesamiento del bloque, se activa con un hilo */
void* procesar_bloque(void *datos){
    struct BloqueImagen* bloque = (struct BloqueImagen*) datos;
    /* Convolución */
    aplicar_convolucion(bloque);
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

/* Se procesa la imagen: se divide imagen, se aplica filtros, se unifican */
int procesar_imagen(PGMImage* image){
    int size = (image->width)*(image->height);

    pid_t pid = fork();
    if(pid==-1){fprintf(stderr,"Error en la creación del procesamiento\n");return 0;}
    else if(pid==0){/* Proceso hijo donde se realiza el procesamiento de una imagen */
        struct timespec time_start, time_end;
        clock_gettime(CLOCK_REALTIME,&time_start);

        pthread_t threads[core_numbers];
        struct BloqueImagen* bloques = dividir_imagen(image);

        /* Creación de hilos para procesar cada bloque */
        for(int i = 0; i<core_numbers; i++)
            pthread_create(&threads[i],NULL, procesar_bloque, &bloques[i]);

        /* Esperar a que los hilos terminen */
        for (int i = 0; i<core_numbers;i++){
            pthread_join(threads[i],NULL);}

        /* Unificación de bloques */
        byte* resultado = malloc(sizeof(byte)*size);
        unificar_bloques(bloques,resultado);

        /* Obtención de tiempo de convolución */
        clock_gettime(CLOCK_REALTIME,&time_end);
        double time_executed = (time_end.tv_sec - time_start.tv_sec) + (double) (time_end.tv_nsec - time_start.tv_nsec) / 1000000000L;
        fprintf(stderr,"Tiempo de ejecución de convolución: %lf ms\n",time_executed);

        /* Escritura de bloque */
        char new_filename[SIZE]="results/";strcat(new_filename,image->filename);
        WritePGM(new_filename,resultado,image->width,image->height);

        /* Liberación de recursos */
        for (int i = 0; i < core_numbers; i++) {
            free(bloques[i].result);
        }
        free(image->image);
        free(bloques);
        free(resultado);

        return 0;
    }else{/* Proceso padre */
        waitpid(pid,NULL,0);
        return 1;
    }
}

int main(int argc,char **argv){
    /* Cantidad de núcleos que presenta un computador */
    if((core_numbers = sysconf(_SC_NPROCESSORS_ONLN))<1)
        core_numbers=1;

    int while_status = 1;
    while(while_status == 1){
        int filtro = 0;
        char* path_file = malloc(SIZE);
        /* Ingreso de ruta del archivo */
        fprintf(stderr,"Ingrese ruta o nombre del archivo: ");
        scanf("%s",path_file);
        if(strcmp(path_file,"exit")==0){while_status=0;fprintf(stderr,"Saliendo...\n");return 0;}
        while(filtro>4 || filtro < 1){
            fprintf(stderr,"Ingrese filtro a aplicar (1-> sobel, 2-> blur, 3-> sharpen, 4-> identity): ");
            scanf("%d",&filtro);
        }
        pgm_image = leer_imagen(path_file);
        if(pgm_image != NULL){
            pgm_image->filtro_aplicar = filtro;
            pgm_image->filename = obtenerFilename(path_file);
            while_status = procesar_imagen(pgm_image);
            free(pgm_image);
        }
    }
}