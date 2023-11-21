#ifndef PGM_H
#define PGM_H

typedef unsigned char byte;

int ReadPGM(char* file_name, byte** ppImg, int* pnWidth, int* pnHeight);
void WritePGM(char* file_name, byte* pImg, int nWidth, int nHeight);
int FrameConv3x3(byte* pInp, byte* pOut, int nW, int nH, double conv[9], double denom);

#endif