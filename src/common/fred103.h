/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef FAST_REDUCTION_MODULO_X103_X9_X1
#define FAST_REDUCTION_MODULO_X103_X9_X1

#include "ffpoly.h"

typedef struct {
	WORD32 mask;
	WORD32 maskpos;
	WORD32 l1,l2;
	WORD32 m1,m2;
	} RED103;

extern const WORD32 L103[256];

void fastest_reduction_mod_103(FFPOLY *poly);
void fastest_reduction_mod_103u(FFPOLY *rs);



#endif
