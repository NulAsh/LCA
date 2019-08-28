/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef POLY_PRINT_H
#define POLY_PRINT_H

#include "ffpoly.h"
#include "expoly.h"

#ifdef __cplusplus
extern "C" {
#endif

void poly_print(FFPOLY *pr,const char *cap);
void expoly_print(EXPOLY *p,const char *cap);
void expoly_twindump(EXPOLY *a,EXPOLY *b);
void ffpoly_dump(FFPOLY *p);

void ffpoly_dumpdata(FFPOLY *fp, int cr);
void expoly_dumpdata(EXPOLY *e);

#ifdef __cplusplus
}
#endif


#endif
