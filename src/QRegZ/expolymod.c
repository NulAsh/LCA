
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "expolymod.h"
#include "osdepn.h"

#define TEST_FAST_ADDICTION

/* *********************************************************** */

static int expolymod_build_modulus(EXPOLYMOD *fm,EXPOLY *f,FFPOLY *cmod)
	{
	int n = f->pvdeg;

	if( expoly_copy(&fm->f,f) < 1 ) return 0;

	if( expoly_copy_rev(&fm->p1,f,n) < 1 ) return 0;
	// CopyReverse(P1, f, n);

	if( expoly_newton_inverse_truncation(&fm->p2,&fm->p1,cmod,n-1) != 1 ) return 0;
	// InvTrunc(P2, P1, n-1);

	if( expoly_copy_rev(&fm->p1,&fm->p2,n-2) < 1 ) return 0;
	// CopyReverse(P1, P2, n-2);

	if( expoly_truncate(&fm->h0,&fm->p1,n-2) != 1 ) return 0;
	// trunc(F.h0, P1, n-2);

	if( expoly_truncate(&fm->f0,f,n) != 1 ) return 0;
	// trunc(F.f0, f, n);
      
	if( ffpoly_copy(&fm->hlc,&fm->p2.ep[0]) < 1 ) return 0;
	fm->hlc_identity = ffpoly_is_identity(&fm->hlc);
	
	// F.hlc = ConstTerm(P2);

	if( ffpoly_copy(&fm->cmod,cmod) < 1 ) return 0;
	// Added by me...
 	
	return 1;
	}

static int expolymod_reduce(EXPOLY *p,EXPOLYMOD *fm)
	{
	int n = fm->f.pvdeg;

	if( expoly_shiftright(&fm->p1,p,n) != 1 ) return 0;
	if( expoly_mulmod(&fm->p2,&fm->p1,&fm->h0,NULL,&fm->cmod) != 1 ) return 0;
	if( expoly_shiftright(&fm->p2,&fm->p2,n-2) != 1 ) return 0;
	if( fm->hlc_identity != 1 ) 
		{
		if( expoly_mulscalar(&fm->p1,&fm->p1,&fm->hlc,&fm->cmod) != 1 ) return 0;
		}
    
#ifdef TEST_FAST_ADDICTION
	if(fm->cmod.pvdeg == 103) expoly_fast_add(&fm->p2,&fm->p1,fm->cmod.pvdeg);
#else
	if( expoly_addmod(&fm->p2,&fm->p2,&fm->p1,NULL,NULL/*&fm->cmod*/) != 1 ) return 0;
#endif

	if( expoly_mulmod(&fm->p1,&fm->p2,&fm->f0,NULL,&fm->cmod) != 1 ) return 0;
	//if( expoly_karatsuba_multiply(&fm->p1,&fm->p2,&fm->f0,&fm->cmod) != 1 ) return 0;

    if( expoly_truncate(&fm->p1,&fm->p1,n) != 1 ) return 0;
	if( expoly_truncate(p,p,n) !=  1) return 0;
	
#ifdef TEST_FAST_ADDICTION
	expoly_fast_add(p,&fm->p1,fm->cmod.pvdeg);
#else
	if( expoly_addmod(p,p,&fm->p1,NULL,NULL/*&fm->cmod*/) != 1 ) return 0;
#endif
	return 1;
	}


/* *********************************************************** */

int expolymod_init_modulus(EXPOLYMOD *fm,EXPOLY *f,FFPOLY *cmod)
	{
	int err=0;
	int n;

	if(!f || f->pvdeg <=0 || !fm) return 0;

	n = f->pvdeg + 1;

	if( expoly_init(&fm->f ,n) != 1 ) err++;
	if( expoly_init(&fm->f0,n) != 1 ) err++;
	if( expoly_init(&fm->h0,n) != 1 ) err++;
	if( expoly_init(&fm->p1,2*n) != 1 ) err++;
	if( expoly_init(&fm->p2,2*n) != 1 ) err++;

	if(err > 0) return 0;

	return expolymod_build_modulus(fm,f,cmod);
	}


int expolymod_free_modulus(EXPOLYMOD *fm)
	{
	expoly_free(&fm->p2);
	expoly_free(&fm->p1);
	expoly_free(&fm->h0);
	expoly_free(&fm->f0);
	expoly_free(&fm->f);
	
	return 1;
	}


int expolymod_squaremod(EXPOLY *dest,EXPOLY *src,EXPOLYMOD *fm)
	{
	if( expoly_square(dest,src,NULL,&fm->cmod) != 1) return 0;
	if( dest->pvdeg < fm->f.pvdeg ) return 1;
	
    //return expoly_truncate(dest,dest,129);
	return expolymod_reduce(dest,fm);
	}
