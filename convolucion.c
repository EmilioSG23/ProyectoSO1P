#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "pgm.h"
#include "filtros.h"

#define SIZE 128 //Tamaño de buffer del scanf para path_file
int core_numbers;
PGMImage* image;


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
struct PGMImage* dividir_imagen(PGMImage* image){
    struct PGMImage *bloques = malloc((core_numbers) * sizeof(struct PGMImage *));
    int nWidth = (image->width); int nHeight = (image->height)/core_numbers;
    
    for(int i = 0; i<core_numbers;i++){
        bloques[i].width = nWidth;
        bloques[i].height = nHeight;
        bloques[i].filtro_aplicar = image -> filtro_aplicar;
        bloques[i].image = malloc(sizeof(byte) *  nWidth * nHeight);

        /* Copia datos de la imagen al bloque  correspondiente*/
        for (int j = 0; j < nHeight; j++)
            memcpy(bloques[i].image + j * nWidth, 
                    image->image + (i*nHeight + j) * nWidth, 
                    sizeof(byte) * nWidth);
    }
    return bloques;
}

/* Procesamiento del bloque, se activa con un hilo */
void* procesar_bloque(void *datos){
    struct PGMImage* bloque = (struct PGMImage*) datos;
    /* Convolución */
    aplicar_convolucion(bloque);
}


void unificar_bloques(struct PGMImage* bloques, byte* resultado){
    for (int i = 0; i < core_numbers; i++) {
        /* Se copia todo el bloque directamente a resultados */
        memcpy(resultado, bloques[i].image, bloques[i].width * bloques[i].height);
        /* Se mueve puntero al siguiente bloque */
        resultado += bloques[i].width * bloques[i].height;
    }
}

/* Se procesa la imagen: se divide imagen, se aplica filtros, se unifican */
int procesar_imagen(PGMImage* image){
    struct timeval time_start, time_end;
    gettimeofday(&time_start,NULL);
    int size = (image->width)*(image->height);

    pid_t pid = fork();
    if(pid==-1){fprintf(stderr,"Error en la creación del procesamiento\n");return 0;}
    else if(pid==0){/* Proceso hijo donde se realiza el procesamiento de una imagen */
        pthread_t threads[core_numbers];
        struct PGMImage* bloques = dividir_imagen(image);
        free(image->image);
        /* Creación de hilos para procesar cada bloque */
        for(int i = 0; i<core_numbers; i++)
            pthread_create(&threads[i],NULL, procesar_bloque, &bloques[i]);

        /* Esperar a que los hilos terminen */
        for (int i = 0; i<core_numbers;i++)
            pthread_join(threads[i],NULL);

        /* Unificación de bloques */
        byte* resultado = malloc(sizeof(byte)*size);
        unificar_bloques(bloques,resultado);

        /* Obtención de tiempo de convolución */
        gettimeofday(&time_end,NULL);
        int time_executed = time_end.tv_usec - time_start.tv_usec;
        fprintf(stderr,"Tiempo de ejecución de convolución: %d ms\n",time_executed);

        /* Escritura de bloque */
        char new_filename[SIZE]="results/";strcat(new_filename,image->filename);
        WritePGM(new_filename,resultado,image->width,image->height);

        /* Liberación de recursos */
        for (int i = 0; i < core_numbers; i++) {
            free(bloques[i].filename);
            free(bloques[i].image);
        }
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
    core_numbers = sysconf(_SC_NPROCESSORS_ONLN);

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
        image = leer_imagen(path_file);
        if(image != NULL){
            image->filtro_aplicar = filtro;
            image->filename = obtenerFilename(path_file);
            while_status = procesar_imagen(image);
            free(image);
        }
    }
}