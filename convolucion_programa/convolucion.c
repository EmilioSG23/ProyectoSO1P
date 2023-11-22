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
/* Verifica si el nombre del archivo empieza con una extension, sino lo añade */
char* parseExtension(char* filename, char* extension){
    int len_file = strlen(filename);
    int len_ext = strlen(extension);
     
    int endsWith = 0;
    if (len_ext <= len_file) {
        for (int i = 0; i < len_ext; i++) {
            if (filename[i + len_file - len_ext] != extension[i]) {
                endsWith = 0;
                break;
            }
            endsWith = 1;
        }
    }
    if(endsWith == 0)
        filename=strcat(filename,extension);

    return filename;
}

/* Verifica si el nombre del archivo termina con .pgm, sino lo añade */
char* parsePGMExtension (char* filename){
    return parseExtension(filename, ".pgm");
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
        WritePGM(image->output,resultado,image->width,image->height);

        /* Liberación de recursos */
        for (int i = 0; i < core_numbers; i++) {
            free(bloques[i].result);
        }
        free(image->image);
        free(bloques);
        free(resultado);
        free(image);
        
        exit(0);

        return 0;
    }else{/* Proceso padre */
        //waitpid(pid,NULL,0);
        free(image->image);
        free(image);
        return 1;
    }
}

int main(int argc,char **argv){
    if(argc <=1 || argc >= 5){
        fprintf(stderr,"\e[1m\x1b[31m================================ Uso de convolucion ================================\x1b[0m\e[m\n");
        fprintf(stderr, "\e[1mIngresando solo nombre y dirección de imagen a procesar: \e[m\n");
        fprintf(stderr, "\t%s <dirección/nombre_imagen> \n\n",argv[0]);
        fprintf(stderr, "\e[1mIngresando el nombre y dirección de imagen a procesar junto al filtro que quiere utilizar: \e[m\n");
        fprintf(stderr, "\t%s <dirección/nombre_imagen> <filtro>\n\n",argv[0]);
        fprintf(stderr, "\e[1mIngresando nombre y dirección de imagen a procesar y archivo resultante: \e[m\n");
        fprintf(stderr, "\t%s <dirección/nombre_imagen> <dirección/nombre_resultado>\n\n",argv[0]);
        fprintf(stderr, "\e[1mIngresando nombre y dirección de imagen a procesar y archivo resultante junto al filtro que quiere utilizar: \e[m\n");
        fprintf(stderr, "\t%s <dirección/nombre_imagen> <dirección/nombre_resultado>  <filtro>\n\n",argv[0]);
        fprintf(stderr, "\e[1mFiltros disponibles que puede ingresar mediante texto o mediante su respectivo número:\e[m\n");
        fprintf(stderr, "1 -> sobel, 2 -> blur, 3 -> sharpen, 4 -> identity\n");
        return 0;
    }

    char arguments[argc][64];
    for (int i = 1; i < argc; i++){
        strcpy(arguments[i-1],argv[i]);
    }

    char path_entrada[64];strcpy(path_entrada, arguments[0]);
    char* entrada = parsePGMExtension(obtenerFilename(path_entrada));
    char new_filename[SIZE]="results/";strcat(new_filename,entrada);
    char * salida = parsePGMExtension(new_filename);
    int filtro = 2; //Aplica blur por defecto (1-> sobel, 2-> blur, 3-> sharpen, 4-> identity)
    
    if(argc == 3){
        int temp = verificar_filtro(arguments[1]);
        /* Verifica si se ingresó un filtro o nombre de archivo de salida */
        if(temp != -1){
            filtro = temp;
        }else{
            salida = parsePGMExtension(arguments[1]);
        }
    }else if(argc ==4){
        salida = parsePGMExtension(arguments[1]);
        int temp = verificar_filtro(arguments[2]);
        /* Verifica si el fitro ingresado existe y lo usa*/
        if(temp != -1)
            filtro = temp;
        else{
            fprintf(stderr,"Filtro ingresado no válido\n");
            return 0;
        }
    }

    /* Cantidad de núcleos que presenta un computador */
    if((core_numbers = sysconf(_SC_NPROCESSORS_ONLN) -1)<1)
        core_numbers=1;

    PGMImage* pgm_image = leer_imagen(parsePGMExtension(arguments[0]));
    if(pgm_image != NULL){
        pgm_image->filtro_aplicar = filtro;
        pgm_image->input = entrada;
        pgm_image->output = salida;
        procesar_imagen(pgm_image);
    }
}
