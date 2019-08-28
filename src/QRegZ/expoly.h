/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef EXTENSION_FIELDS_POLYNOMIALS_H
#define EXTENSION_FIELDS_POLYNOMIALS_H

#include "mytypes.h"
#include "ffpoly.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define USE_FAST_DIVISION

#define EX_MAXDEGREE       ((16384*3)/2)
#define EX_DEFAULTDEGREE   (256)

#define EX_UNINITIALIZED_POLY   -2
#define EX_REVERSE_ALL          -1

#define EX_NEWTON_BUFFER_SIZE   20

typedef struct expoly {
	FFPOLY *ep;
	int pvdeg;
	int pvsize;
	} EXPOLY;

int expoly_init(EXPOLY *p,int _size);
int expoly_free(EXPOLY *p);

int expoly_zeroset(EXPOLY *p);
int expoly_identity_set(EXPOLY *p);
int expoly_setterm(EXPOLY *p,FFPOLY *f,int place);
int expoly_set(EXPOLY *p,int *deg,int degsize);
int expoly_setfrompoly(EXPOLY *p,FFPOLY *f);

int expoly_addone(EXPOLY *p);
int expoly_addmod(EXPOLY *rs,EXPOLY *p1,EXPOLY *p2,EXPOLY *pmod,FFPOLY *cmod);
int expoly_fast_add(EXPOLY *rs,EXPOLY *p1, int cmod_deg);
int expoly_mulmod(EXPOLY *rs,EXPOLY *p1,EXPOLY *p2,EXPOLY *pmod,FFPOLY *cmod);
int expoly_karatsuba_multiply(EXPOLY *out,EXPOLY *p,EXPOLY *q,FFPOLY *cmod);

int expoly_fast_divide(EXPOLY *qs,EXPOLY *rs,EXPOLY *a, EXPOLY *b, FFPOLY *cmod);
int expoly_divide(EXPOLY *qs,EXPOLY *rs,EXPOLY *g, EXPOLY *divisor, FFPOLY *cmod);
int expoly_reduce(EXPOLY *rs,EXPOLY *modulus,FFPOLY *cmod);
int expoly_copy(EXPOLY *dest,EXPOLY *src);
int expoly_copy_rev(EXPOLY *dest,EXPOLY *src,int hi);

int expoly_newton_inverse_truncation(EXPOLY *dest,EXPOLY *src,FFPOLY *cmod,int e);

int expoly_cmp(EXPOLY *p1,EXPOLY *p2);

int expoly_truncate(EXPOLY *dest,EXPOLY *src,int n);

int expoly_shiftleft(EXPOLY *rs,EXPOLY *p,int n);
int expoly_shiftright(EXPOLY *rs,EXPOLY *p,int n);

int expoly_square(EXPOLY *rs,EXPOLY *p,EXPOLY *pmod,FFPOLY *cmod);
int expoly_powmod(EXPOLY *rs,EXPOLY *p,int n,EXPOLY *pmod,FFPOLY *cmod);

int expoly_gcd(EXPOLY *rs,EXPOLY *a,EXPOLY *b,FFPOLY *cmod);

int expoly_muladd(EXPOLY *rs,EXPOLY *p,FFPOLY *coeff,FFPOLY *mod);
int expoly_mulscalar(EXPOLY *rs,EXPOLY *p,FFPOLY *scalar,FFPOLY *mod);

int expoly_subst(FFPOLY *rs,EXPOLY *f,FFPOLY *x,FFPOLY *mod);

int expoly_is_zero(EXPOLY *p);
int expoly_is_identity(EXPOLY *p);

void expoly_fast_recompute_degree(EXPOLY *p,int maxdeg);

//int expoly_factor(EXPOLY **factors,int *factsNo,EXPOLY *f,int extndegree);

#ifdef __cplusplus
}
#endif


#endif
