/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef ROOT_FIND_H
#define ROOT_FIND_H

#include "expoly.h"
#include "ffpoly.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct root_pot
	{
	FFPOLY *factors;
	int factsNo;
	int size;

	// ;------------------------------------------------------;

	EXPOLY *deposit;
    EXPOLY *pickup;
	int deposit_size;
    int pickup_size;
	} ROOT_POT;

int expoly_pot_init(ROOT_POT *rp,FFPOLY *cmod,int fx_pvdeg);
int expoly_pot_free(ROOT_POT *rp);

int expoly_factor(ROOT_POT *rp,EXPOLY *fx,FFPOLY *cmod);

#ifdef __cplusplus
}
#endif



#endif
