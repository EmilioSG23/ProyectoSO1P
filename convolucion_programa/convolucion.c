#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include "pgm.h"
#include "pgm_image.h"
#include "filtros.h"

#define SIZE 128 //Tamaño de buffer del scanf para path_file
int core_numbers;

/* Obtener filename de un path */
char* obtenerFilename(char* path_file){
    char* copy = malloc(SIZE * sizeof(char*));
    strcpy(copy,path_file);
    char* filename;
    char* token = strtok(copy,"/");
    while(token != NULL) {
        filename=token;
        token = strtok(NULL, "/");
    }
    free(copy);
    return filename;
}
/* Obtener argumentos */
char** obtenerArgumentos(char* line){
    char** argumentos = malloc(4*sizeof(char *));
    int i = 0;
    char* token = strtok(line," ");
    while(token != NULL && i<4) {
        argumentos[i]=token;
        token = strtok(NULL, " ");
        i++;
    }
    return argumentos;
}
/* Obtener cantidad de argumentos */
int cantidadArgumentos(char** argumentos){
    int args_count = 0;
    for(int i = 0; i < 4; i++){
        if(argumentos[i] != NULL)
            args_count++;
    }
    return args_count;
}
/* Verifica si el nombre del archivo empieza con una extension, sino lo añade */
char* parseExtension(char* filename, char* extension){
    char* new_filename = malloc((sizeof(filename)/sizeof(filename[0])*sizeof(char *)));
    strcpy(new_filename, filename);
    int len_file = strlen(filename);
    int len_ext = strlen(extension);

    if(strcmp(filename + (len_file-len_ext), extension)!=0)
        strcat(new_filename,extension);
    return new_filename;
}

/* Verifica si el nombre del archivo termina con .pgm, sino lo añade */
char* parsePGMExtension (char* filename){
    parseExtension(filename, ".pgm");
}

/* Procesamiento del bloque, se activa con un hilo */
void* procesar_bloque(void *datos){
    struct BloqueImagen* bloque = (struct BloqueImagen*) datos;
    aplicar_convolucion(bloque);
}

/* Se procesa la imagen: se divide imagen, se aplica filtros, se unifican */
int procesar_imagen(PGMImage* image){
    pid_t pid = fork();
    if(pid==-1){printf("Error en la creación del procesamiento\n");return 0;}
    else if(pid==0){/* Proceso hijo donde se realiza el procesamiento de una imagen */
        int size = (image->width)*(image->height);
        struct timespec time_start, time_end;
        clock_gettime(CLOCK_REALTIME,&time_start);

        pthread_t threads[core_numbers];
        struct BloqueImagen* bloques = dividir_imagen(image,core_numbers);

        /* Creación de hilos para procesar cada bloque */
        for(int i = 0; i<core_numbers; i++)
            pthread_create(&threads[i],NULL, procesar_bloque, &bloques[i]);

        /* Esperar a que los hilos terminen */
        for (int i = 0; i<core_numbers;i++){
            pthread_join(threads[i],NULL);}

        /* Unificación de bloques */
        byte* resultado = malloc(sizeof(byte)*size);
        unificar_bloques(bloques,core_numbers, resultado);

        /* Obtención de tiempo de convolución */
        clock_gettime(CLOCK_REALTIME,&time_end);
        double time_executed = (time_end.tv_sec - time_start.tv_sec) + (double) (time_end.tv_nsec - time_start.tv_nsec) / 1000000000L;
        printf("Tiempo de ejecución de convolución: %lf ms\n",time_executed);

        /* Escritura de bloque */
        WritePGM(image->output,resultado,image->width,image->height);

        /* Liberación de recursos */
        for (int i = 0; i < core_numbers; i++) {
            free(bloques[i].result);
        }
        free(image->image);free(image->input);free(image->output);free(bloques);free(resultado);free(image);
        return 0;
    }else{/* Proceso padre */
        free(image->image);
        free(image);
        return 1;
    }
}

void printAyuda () {
    printf("\e[1m\x1b[31m================================ Uso de convolucion ================================\x1b[0m\e[m\n");
    printf("\e[1mIngresando solo nombre y dirección de imagen a procesar: \e[m\n");
    printf("\t<dirección/nombre_imagen> \n\n");
    printf("\e[1mIngresando el nombre y dirección de imagen a procesar junto al filtro que quiere utilizar: \e[m\n");
    printf("\t<dirección/nombre_imagen> <filtro>\n\n");
    printf("\e[1mIngresando nombre y dirección de imagen a procesar y archivo resultante: \e[m\n");
    printf("\t<dirección/nombre_imagen> <dirección/nombre_resultado>\n\n");
    printf("\e[1mIngresando nombre y dirección de imagen a procesar y archivo resultante junto al filtro que quiere utilizar: \e[m\n");
    printf("\t<dirección/nombre_imagen> <dirección/nombre_resultado>  <filtro>\n\n");
    printf("\e[1mFiltros disponibles que puede ingresar mediante texto o mediante su respectivo número:\e[m\n");
    printf("1 -> sobel, 2 -> blur, 3 -> sharpen, 4 -> identity\n");
    printf("Ahora, ingrese su comando:\n");
}

int main(int argc,char **argv){
    printf("Ingrese comandos para el procesamiento de imagenes, escriba \e[1mhelp\e[m para ver la guía de uso del programa, escriba \e[1mexit\e[m para salir\n");
    char* linea_consola = (char*) calloc(1,256);
    int status = 1,always=1;
    size_t max = 256;ssize_t l = 0;
    while(status == 1){
        l = getline(&linea_consola, &max, stdin);
        linea_consola[strcspn(linea_consola, "\n")] = 0;
        if(l==1)
            printf("No se ingresó un comando, intente de nuevo.\n");
        
        char** args = obtenerArgumentos(linea_consola);
        int args_count = cantidadArgumentos(args);
        if(strcmp(args[0],"exit")==0){
            printf("Saliendo...\n");status=0;break;}
        if(strcmp(args[0],"help")==0){
            printAyuda();
        }if(args_count>=4){
            printf("Se excedió el número de argumentos permitidos (3), intente otra vez\n");
        }else{
            char* salida;

            int filtro = 2; //Aplica blur por defecto (1-> sobel, 2-> blur, 3-> sharpen, 4-> identity)

            if(args_count == 2){
                int temp = verificar_filtro(args[1]);
                
                if(temp != -1){
                    filtro = temp;
                }else{
                    salida = parsePGMExtension(args[1]);
                }
            }else if(args_count ==3){
                int temp = verificar_filtro(args[2]);
                if(temp != -1)
                    filtro = temp;
                else{
                    printf("Filtro ingresado no válido, intente de nuevo:\n");
                    free(args);break;
                }
                salida = parsePGMExtension(args[1]);
            }
            /* Cantidad de núcleos que presenta un computador */
            if((core_numbers = sysconf(_SC_NPROCESSORS_ONLN) -1)<1)
                core_numbers=1;
            
            /* Procesamiento de imagen */
            PGMImage* pgm_image = leer_imagen(parsePGMExtension(args[0]));
            if(pgm_image != NULL){
                pgm_image->filtro_aplicar = filtro;
                pgm_image->input = parsePGMExtension(obtenerFilename(args[0]));
                if(salida == NULL){
                    char new_filename[SIZE]="results/";strcat(new_filename,pgm_image->input);
                    salida = parsePGMExtension(new_filename);
                }
                pgm_image->output = salida;
                status = procesar_imagen(pgm_image);
            }
        }
        free(args);
    }
    free(linea_consola);
    return 0;
}