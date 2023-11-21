#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct PGMImage {
    char pgmType[3];
    unsigned char** data;
    unsigned int width;
    unsigned int height;
    unsigned int maxValue;
} PGMImage;

void ignoreComments(FILE* fp);
bool openPGM(PGMImage* pgm,const char* filename);
void printImageDetails(PGMImage* pgm);

void writePGM(PGMImage* pgm, const char* filename);

#endif 