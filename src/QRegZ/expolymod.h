/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef EXTENSION_FIELDS_POLYNOMIALS_FAST_MODULUS_H
#define EXTENSION_FIELDS_POLYNOMIALS_FAST_MODULUS_H

#include "expoly.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct expolymod {
	EXPOLY f;
	EXPOLY f0;
	EXPOLY h0;
	FFPOLY hlc;
	int    hlc_identity;
	FFPOLY cmod;
	// Temporary storage...
	EXPOLY p1,p2;
	} EXPOLYMOD;

int expolymod_init_modulus(EXPOLYMOD *fm,EXPOLY *f,FFPOLY *cmod);
int expolymod_free_modulus(EXPOLYMOD *fm);
int expolymod_squaremod(EXPOLY *dest,EXPOLY *src,EXPOLYMOD *fm);

#ifdef __cplusplus
	}
#endif


#endif
