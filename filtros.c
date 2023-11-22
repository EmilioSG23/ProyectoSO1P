#include <stdio.h>
#include <stdlib.h>

#include "pgm.h"
#include "filtros.h"

void aplicar_convolucion(struct BloqueImagen* bloque){
    bloque -> result = malloc(bloque->width * bloque->height);
    switch (bloque -> image -> filtro_aplicar){
        case 1:
            aplicar_sobel(bloque);
            break;
        case 2:
            aplicar_blur(bloque);
            break;
        case 3:
            aplicar_sharpen(bloque);
            break;
        case 4:
            aplicar_identity(bloque);
            break;
        default:
            fprintf(stderr,"Hubo un error al momento de realizar la convolución\n");
            free(bloque -> result);
            break;
    }
}

void aplicar_filtro(struct BloqueImagen* bloque, double conv[9]){
    double nSum = 0;
    for(int i = 0; i<9;i++)
        nSum += conv[i];
    if(nSum == 0) nSum = 1;
    /* Aplicación de la convolución */
    aplicar_convolucion3x3_bloque(bloque,conv,nSum);
}

/* Filtros para aplicar */
void aplicar_sobel (struct BloqueImagen* bloque){
    /* Creación de máscara top sobel */
    double conv[9] = {1, 2, 1,
                0, 0, 0,
                -1,-2,-1};
    /* Aplicación de filtro */
    aplicar_filtro(bloque,conv);
}
void aplicar_blur (struct BloqueImagen* bloque){
    /* Creación de máscara blur */
    double conv[9] = {0.0625, 0.125, 0.0625,
                    0.125, 0.25, 0.125,
                    0.0625, 0.125, 0.0625};
    /* Aplicación de filtro */
    aplicar_filtro(bloque,conv);
}
void aplicar_sharpen (struct BloqueImagen* bloque){
    /* Creación de máscara sharpen */
    double conv[9] = {0, -1, 0,
                    -1, 5, -1,
                    0,-1, 0};
    /* Aplicación de filtro */
    aplicar_filtro(bloque,conv);
}
void aplicar_identity (struct BloqueImagen* bloque){
    /* Creación de máscara identity */
    double conv[9] = {0, 0, 0,
                    0, 1, 0,
                    0, 0, 0};
    /* Aplicación de filtro */
    aplicar_filtro(bloque,conv);
}

/* Aplicación de convolución 3x3 */
void aplicar_convolucion3x3_bloque(struct BloqueImagen* bloque, double conv[9], double denom) {
    byte* image = bloque -> image -> image;

    int width = bloque -> width;
    int height = bloque -> height;
    int totalSize = (bloque->image->width)*(bloque->image->height);
    int blockSize = width * height;

    //int d_index = -1;   //Posición por defecto a -1
    int d_value = 0;    //Valor por defecto

    int res;int denom2 = (denom+1)/2;

    for(int i = bloque-> begin; i < bloque -> end; i++){
        int index[9] = {i-width-1, i-width, i-width+1,
                        i-1, i, i+1,
                        i+width-1, i+width, i+width+1};
        /* Casos de salida de bordes */
        //Borde izquierdo
        int borde = (i+1)%width;
        if(borde == 0){index[2] = -1; index[5] = -1; index[8] = -1;}
        //Borde derecho
        else if(borde == 1){index[0] = -1; index[3] = -1; index[6] = -1;}

        int inRange = 1;
        for(int j = 0; j < 9; j ++){
            if((index[j] >= totalSize || index[j] <= 0) && (inRange == 1)){
                res = 0;
                inRange = 0;
            }
        }

        if(inRange == 1){
            byte values[9] = {
                image[index[0]],image[index[1]],image[index[2]],
                image[index[3]],image[index[4]],image[index[5]],
                image[index[6]],image[index[7]],image[index[8]]
            };

            res = values[0]*conv[0] + values[1]*conv[1] + values[2]*conv[2]+
                    values[3]*conv[3] + values[4]*conv[4] + values[5]*conv[5]+
                    values[6]*conv[6] + values[7]*conv[7] + values[8]*conv[8];

            res  = (res+denom2) / denom;
            res = abs(res);

            if(res>255) res=255;
        }
        bloque->result[i%blockSize]=res;
    }
}