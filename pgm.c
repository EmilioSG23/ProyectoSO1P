//=======================================================================================
//
// pgm.h
//
// rear & write functions for pgm files
//
// developer: Henry Guennadi Levkin
//
//=======================================================================================

#include <stdio.h>
#include <stdlib.h>

//=======================================================================================
typedef unsigned char byte;

//=======================================================================================
int ReadPGM(char* file_name, byte** ppImg, int* pnWidth, int* pnHeight)
{
 char buf[512];

 FILE* pInpFile = fopen(file_name,"rb");
 if( pInpFile==NULL) return 1;
 
 fgets(buf,510,pInpFile); // "P5"
 fgets(buf,510,pInpFile); // "#comments"

 fgets(buf,510,pInpFile); // "width height"
 sscanf(buf,"%d %d", pnWidth, pnHeight);
 //printf("width=%d height=%d\n", *pnWidth, *pnHeight);

 fgets(buf,510,pInpFile); // "MaxColor"

 *ppImg = malloc((*pnWidth) * (*pnHeight));
 
 fread(*ppImg, 1, (*pnWidth) * (*pnHeight), pInpFile);

 fclose(pInpFile);

 return 0;
}

//=======================================================================================
void WritePGM(char* file_name, byte* pImg, int nWidth, int nHeight)
{
    FILE* img_file= fopen(file_name, "wb");

    fprintf( img_file, "P5\n#\n%d %d\n255\n", nWidth, nHeight );

    fwrite( pImg, 1, nWidth * nHeight, img_file);
    fclose(img_file);
}

//==============================================================================
//
// pgmconv33abs.c
//
// use:
// pgmconv33abs convmatr.txt  file.pgm  result.pgm
//
//==============================================================================

int FrameConv3x3(byte* pInp, byte* pOut, int nW, int nH, double conv[9], double denom)
{
 int i, j;
 int i1m, i1p;
 int j1m, j1p;
 int res;
 int denom2 = (denom+1)/2;
 
//for(i=0; i<9; i++) printf("%d ", conv[i]);
//printf("\n"); 
//printf("%d  %d", nW, nH);
	 
 for(i=1; i < (nH-1); i++) // rows
 {	 
   i1m = i - 1;
   i1p = i + 1;
   if(i1m == (-1)) i1m = 0;
   if(i1p == nH) i1p = (nH-1);
   for(j=1; j<(nW-1); j++)
   {
     j1m = j - 1;
     j1p = j + 1;
     if(j1m == (-1)) j1m = 0;
     if(j1p == nW) j1p = (nW-1);
     
     res  = conv[0]*pInp[j1m + nW*i1m] + conv[1]*pInp[j+nW*i1m] + conv[2]*pInp[j1p+nW*i1m];
     res += conv[3]*pInp[j1m + nW*i  ] + conv[4]*pInp[j+nW*i  ] + conv[5]*pInp[j1p+nW*i  ];
     res += conv[6]*pInp[j1m + nW*i1p] + conv[7]*pInp[j+nW*i1p] + conv[8]*pInp[j1p+nW*i1p];
     res  = (res+denom2) / denom;
	 
	 res = abs(res);
     
	 if(res>255) res=255;
     pOut[j + nW*i] = res;
   }
 }
 return 0;
}