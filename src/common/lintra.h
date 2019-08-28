/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef LINTRA_5239890482349082332
#define LINTRA_5239890482349082332

#include "mytypes.h"
#include "arc4.h"

#ifdef __cplusplus
extern "C" {
#endif



void ldt(WORD_8 *inBuff,WORD_8 *outBuff,int **mask,int len);
void print_mtx(WORD_8 **mtx,int size);
void copymtx(WORD_8 **dest,WORD_8 **src,int len);
void trans_mtx(WORD_8 **dest,WORD_8 **src,int len);

void vect_mult(WORD_8 **A,WORD_8 *X,int len,WORD_8 *Y);
void row_mult(WORD_8 **A,WORD_8 *X,int len,WORD_8 *Y);
void mtx_mult(WORD_8 **A,WORD_8 **B,int len,WORD_8 **R);

int  invmtx(WORD_8 **mIn,WORD_8 **mOut,int len);

int PrintBuffer(WORD_8 *buff,int size);
int PrintBitBuffer(WORD_8 *buff,int size);
void print_vector(WORD_8 *vector,int size,char *cap);


int create_invertible_matrix(RC4_CTX *rc4,WORD_8 **mtx,int size);
int create_slide_matrix(WORD_8 *vect,WORD_8 **mtx,int size);
int buffswap(int *buff1,int size,int a,int b,int *buff2);
int buffinit(int *buff1,int *buff2,int size);

#ifdef __cplusplus
}
#endif

#endif
