/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef POLY_TEST_H
#define POLY_TEST_H

#include "ffpoly.h"

#ifdef __cplusplus
extern "C" {
#endif

int poly_rand(FFPOLY *p,int maxdegree);
int poly_test(const char *pass);

#ifdef __cplusplus
}
#endif


#endif
