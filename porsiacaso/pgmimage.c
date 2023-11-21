/*
Sacado de: https://www.geeksforgeeks.org/how-to-read-a-pgmb-format-image-in-c/
*/
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgmimage.h"

void ignoreComments(FILE* fp){
    int ch;
    char line[100];

    while ((ch = fgetc(fp)) != EOF && isspace(ch));

    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        ignoreComments(fp);
    }
    else
        fseek(fp, -1, SEEK_CUR);
}

bool openPGM(PGMImage* pgm,const char* filename){
    FILE* pgmfile = fopen(filename, "rb");

    if (pgmfile == NULL) {
        printf("File does not exist\n");
        return false;
    }
 
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%s",pgm->pgmType);
 
    if (strcmp(pgm->pgmType, "P5")) {
        fprintf(stderr,"Wrong file type!\n");
        exit(EXIT_FAILURE);
    }
 
    ignoreComments(pgmfile);

    fscanf(pgmfile, "%d %d",&(pgm->width),&(pgm->height));
 
    ignoreComments(pgmfile);

    fscanf(pgmfile, "%d", &(pgm->maxValue));
    ignoreComments(pgmfile);

    pgm->data = malloc(pgm->height * sizeof(unsigned char*));

    if (pgm->pgmType[1] == '5') {
 
        fgetc(pgmfile);
 
        for (int i = 0;
             i < pgm->height; i++) {
            pgm->data[i]
                = malloc(pgm->width
                         * sizeof(unsigned char));
            if (pgm->data[i] == NULL) {
                fprintf(stderr,
                        "malloc failed\n");
                exit(1);
            }
            fread(pgm->data[i],
                  sizeof(unsigned char),
                  pgm->width, pgmfile);
        }
    }
    fclose(pgmfile);
 
    return true;
}

void printImageDetails(PGMImage* pgm)
{
    printf("PGM File type  : %s\n",pgm->pgmType);

    if (!strcmp(pgm->pgmType, "P2"))
        printf("PGM File Format: ASCII\n");
    else if (!strcmp(pgm->pgmType,
                     "P5"))
        printf("PGM File Format: Binary\n");
 
    printf("Width of img   : %d px\n",pgm->width);
    printf("Height of img  : %d px\n", pgm->height);
    printf("Max Gray value : %d\n",pgm->maxValue);
}

/* Escritura mía */
void writeImage(PGMImage* pgm, const char* filename){
    
}