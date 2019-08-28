/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef QUARTZ_FAST_MATRIX_MANIPULATION
#define QUARTZ_FAST_MATRIX_MANIPULATION

#include "mytypes.h"

int QMtx_PackVector(WORD_8 *vector,int len,WORD32 *vect);
int QMtx_UnPackVector(WORD32 *vect,WORD_8 *vector,int len);
void QMtxVect4Mul(WORD32 *mtx,WORD32 *in,WORD32 *out);


#endif
