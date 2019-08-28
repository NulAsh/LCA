/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef FINITE_FIELDS_POLYNOMIALS_H
#define FINITE_FIELDS_POLYNOMIALS_H

#include "mytypes.h"

// #define USE_MMX_ASSEMBLY

#ifdef __cplusplus
extern "C" {
#endif

#define MAXDEGREE       256
#define PWORDSIZE        32
#define PWORDBITS         5

#define SCALED_DEGREE  ( (MAXDEGREE) / (PWORDSIZE) )
#define SCALED_WORD_8VEC ( (MAXDEGREE) /  8 )

typedef struct ffpoly {
	WORD32 pv[SCALED_DEGREE];
	int pvdeg;
	} FFPOLY;

int ffpoly_zeroset(FFPOLY *p);
int ffpoly_zeroset_n(FFPOLY *p,int n);

int ffpoly_identity_set(FFPOLY *p);
int ffpoly_set(FFPOLY *p,const int *deg,int degsize);
int ffpoly_setterm(FFPOLY *p,int i);
int ffpoly_getterm(FFPOLY *p,int i);

int ffpoly_set_x(FFPOLY *p);
int ffpoly_set_x_p_1(FFPOLY *p);

int ffpoly_addone(FFPOLY *p);
int ffpoly_addmod(FFPOLY *rs,FFPOLY *p1,FFPOLY *p2,FFPOLY *modulus);
int ffpoly_mulmod(FFPOLY *rs,FFPOLY *p1,FFPOLY *p2,FFPOLY *modulus);
int ffpoly_divide(FFPOLY *qs,FFPOLY *rs,FFPOLY *g, FFPOLY *modulus);
int ffpoly_field_div(FFPOLY *qs,FFPOLY *a,FFPOLY *b,FFPOLY *mod);
int ffpoly_reduce(FFPOLY *rs,FFPOLY *modulus);

int ffpoly_copy(FFPOLY *dest,FFPOLY *src);
int ffpoly_copy_n(FFPOLY *dest,FFPOLY *src,int n);
int ffpoly_cmp(FFPOLY *p1,FFPOLY *p2);

int ffpoly_shiftleft(FFPOLY *rs,FFPOLY *p,int n);
int ffpoly_shiftright(FFPOLY *rs,FFPOLY *p,int n);
int ffpoly_square(FFPOLY *rs,FFPOLY *p,FFPOLY *modulus);

int ffpoly_square(FFPOLY *rs,FFPOLY *p,FFPOLY *modulus);
int ffpoly_powmod(FFPOLY *rs,FFPOLY *p,int n,FFPOLY *modulus);
int ffpoly_inverse(FFPOLY *inv,FFPOLY *x,FFPOLY *mod);

int ffpoly_gcd(FFPOLY *rs,FFPOLY *a,FFPOLY *b);
int ffpoly_subst(FFPOLY *rs,FFPOLY *f,FFPOLY *x,FFPOLY *mod);

int ffpoly_is_zero(FFPOLY *p);
int ffpoly_is_identity(FFPOLY *p);
int ffpoly_is_even(FFPOLY *p);

int ffpoly_fast_update_degree(FFPOLY *rs,int maxdeg);

int ffpoly_test_irreducibility(FFPOLY *fp);

//int ffpoly_factor(FFPOLY **factors,int *factsNo,FFPOLY *f,int extndegree);

#ifdef __cplusplus
}
#endif


#endif
