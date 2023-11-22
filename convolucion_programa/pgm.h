#ifndef PGM_H
#define PGM_H

typedef unsigned char byte;

int ReadPGM(char* file_name, byte** ppImg, int* pnWidth, int* pnHeight);
void WritePGM(char* file_name, byte* pImg, int nWidth, int nHeight);

#endif