
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

#include "rootfind.h"
#include "expolymod.h"

#include "polyprint.h"
#include "osdepn.h"

#define USE_FAST_SQUARING
//#define PRINTTIME

/*****************************************************************************/
/*                                                                           */
/*  This portion of code implements the root finding algorithm for field     */
/*  with small characteristic described in chapter 4 of the book:            */
/*                                                                           */
/*   Introduction to finite fields and their applications                    */
/*   Rudolf Lidl and Harald Niederreiter                                     */
/*   Cambridge University Press                                              */
/*   ISBN 0-521-46094-8                                                      */
/*                                                                           */
/*****************************************************************************/

static int expoly_pot_depositpoly(ROOT_POT *rp,EXPOLY *hdpoly)
	{
    if(rp->deposit_size >= rp->size) return -1;

    expoly_copy(&rp->deposit[rp->deposit_size],hdpoly);
    return rp->deposit_size++;
	}

static int expoly_deposit_is_empty(ROOT_POT *rp)
	{
	return rp->deposit_size == 0 ? 1 : 0;
	}

static int expoly_pot_addsol(ROOT_POT *rp,FFPOLY *sol,FFPOLY *xterm,FFPOLY *cmod)
	{
    if(rp->factsNo >= rp->size) return 0;

	if(!xterm) 
		ffpoly_copy(&rp->factors[rp->factsNo++],sol);
	else if(ffpoly_is_identity(xterm) || ffpoly_is_zero(sol)) 
		ffpoly_copy(&rp->factors[rp->factsNo++],sol);
	else
		{
		FFPOLY qs,qm,idn;

		ffpoly_identity_set(&idn);

		ffpoly_field_div(&qs,&idn,xterm,cmod);
		ffpoly_mulmod(&qm,&qs,sol,cmod);
		
		ffpoly_copy(&rp->factors[rp->factsNo++],&qm);
		}
    return rp->factsNo;
	}

static int expoly_pot_pickup_poly_get(ROOT_POT *rp,EXPOLY *poly,int id)
    {
    if(id<0 || id>=rp->pickup_size) return 0;

    expoly_copy(poly,&rp->pickup[id]);
    return 1;
    }

static int expoly_pot_pour_deposit_into_pickup(ROOT_POT *rp)
    {
    int i;

    for(i=0;i<rp->deposit_size;i++)
        expoly_copy(&rp->pickup[i],&rp->deposit[i]);

    rp->pickup_size  = rp->deposit_size;
    rp->deposit_size = 0;

    return 1;
    }

static int expoly_checkroot(ROOT_POT *rp,EXPOLY *S,FFPOLY *cmod)
	{
	EXPOLY fgcd;
	EXPOLY fdiv;
	EXPOLY fres;
	EXPOLY probe;
    int id=0;

    /* We are using three buckets, the deposit, the pickup and the solution bucket.
       The first time, the deposit bucket holds only one item,
       which is the one where we derive all the roots.

       Each time, the deposit is poured into the pickup and then
       solutions are checked from there. When a solution is found
       it is put into the solution bucket, otherwise if the degree of
       the poly is greater than one but less than the starting probe, the 
       poly is reput into the pickup for further checks.

       Finally if the poly cannot be decomposed, then we put it into
       the deposit and wait for another run.
    */

	expoly_init(&fdiv,S->pvsize);
	expoly_init(&fres,S->pvsize);
	expoly_init(&fgcd,S->pvsize);

	expoly_init(&probe,S->pvsize);
	if( expoly_pot_pour_deposit_into_pickup(rp) < 0) return 0;

    while( expoly_pot_pickup_poly_get(rp,&probe,id) != 0 )
        {
	    if( expoly_gcd(&fgcd,S,&probe,cmod) == 1 )
            {
	        if(fgcd.pvdeg > 0 && fgcd.pvdeg < probe.pvdeg)
		        {
		        if( expoly_divide(&fdiv,&fres,&probe,&fgcd,cmod) != 1) return 0;

		        if(fgcd.pvdeg == 1) expoly_pot_addsol(rp,&fgcd.ep[0],&fgcd.ep[1],cmod);
		        else if(fgcd.pvdeg > 1) expoly_pot_depositpoly(rp,&fgcd);

			    if(fdiv.pvdeg == 1) expoly_pot_addsol(rp,&fdiv.ep[0],&fdiv.ep[1],cmod);
		        else if(fdiv.pvdeg > 1) expoly_pot_depositpoly(rp,&fdiv);
    	        }
		    else expoly_pot_depositpoly(rp,&probe);
            }
        else expoly_pot_depositpoly(rp,&probe);

		id++;
        }

	expoly_free(&fdiv);
	expoly_free(&fres);
	expoly_free(&fgcd);

	expoly_free(&probe);
	return 1;
	}

/* ----------------------------------------------------------------------------- */

int expoly_pot_init(ROOT_POT *rp,FFPOLY *cmod,int fx_pvdeg)
	{
	int mxtmpdeg = 2 * fx_pvdeg + 1;
	//int extndegree;
	int i;

    rp->deposit      = NULL;
    rp->pickup       = NULL;
	rp->factors      = NULL;
	rp->factsNo      = 0;
	rp->size         = 0;
	rp->deposit_size = 0;
    rp->pickup_size  = 0;

	//extndegree = cmod->pvdeg;

	if(fx_pvdeg < 2) return 0;

	// the maxium amount of monomials and solution is given
	// by the degree of the polynomial.

	rp->factors = (FFPOLY *)malloc( fx_pvdeg * sizeof(FFPOLY) );
	rp->deposit = (EXPOLY *)malloc( fx_pvdeg * sizeof(EXPOLY) );
	rp->pickup  = (EXPOLY *)malloc( fx_pvdeg * sizeof(EXPOLY) );

	if(!rp->factors || !rp->deposit || !rp->pickup) return -1;

	rp->size = fx_pvdeg;

	for(i=0;i<fx_pvdeg;i++)
		{
        if( expoly_init(&rp->deposit[i],mxtmpdeg ) != 1) return 0;
        if( expoly_init(&rp->pickup[i],mxtmpdeg  ) != 1) return 0;
        
        expoly_zeroset(&rp->deposit[i]);
        expoly_zeroset(&rp->pickup[i]);
		}

	ffpoly_zeroset_n(rp->factors,fx_pvdeg);
	return 1;
	}

int expoly_pot_free(ROOT_POT *rp)
	{
    int i;

	if(rp->factors != NULL) free( rp->factors );
	if(rp->deposit != NULL) 
        {
        for(i=0;i<rp->size;i++)
			expoly_free(&rp->deposit[i]);

        free( rp->deposit );
        }

	if(rp->pickup != NULL) 
        {
        for(i=0;i<rp->size;i++)
			expoly_free(&rp->pickup[i]);

        free( rp->pickup );
        }

	rp->factors      = NULL;
	rp->deposit      = NULL;
    rp->pickup       = NULL;
	rp->size         = 0;
	rp->factsNo      = 0;
	rp->pickup_size  = 0;
    rp->deposit_size = 0;

	return 1;
	}

int expoly_factor(ROOT_POT *rp,EXPOLY *fx,FFPOLY *cmod)
	{
	EXPOLY *square = NULL;
	EXPOLY f0,f1;
	EXPOLY f;
	EXPOLY S;
	FFPOLY beta_square;
	FFPOLY beta;
	FFPOLY tmp;
#ifdef USE_FAST_SQUARING
	EXPOLYMOD fmod;
#endif
	int extndegree;
	int done=0;
	int flag = 1;
	int i,j;
	int mxtmpdeg;
    WORD32 start,end;

	if( fx->pvdeg <=1 ) return -101;

	extndegree = cmod->pvdeg;
	mxtmpdeg = 2 * fx->pvdeg + 1;

	if( expoly_init(&f ,mxtmpdeg) != 1) flag = -1;
	if( expoly_init(&f0,mxtmpdeg) != 1) flag = -1;
	if( expoly_init(&f1,mxtmpdeg) != 1) flag = -1;
	if( expoly_init(&S ,mxtmpdeg) != 1) flag = -1;

	expoly_copy(&f,fx);

#ifdef USE_FAST_SQUARING
	if( expolymod_init_modulus(&fmod,&f,cmod) != 1 ) flag = -1;
#endif

	if(flag <= -1) goto EXIT_HERE;

	square = (EXPOLY *)malloc( (extndegree+1) * sizeof(EXPOLY) );
	if(!square)
		{
		flag = -1;
		goto EXIT_HERE;
		}

	for(i=0;i<=extndegree;i++)
		{
		if( expoly_init(&square[i],mxtmpdeg) == 1) done=i+1;
		else break;

		expoly_zeroset(&square[i]);
		}

	if(done!=(extndegree+1)) 
		{
		flag = -1;
		goto EXIT_HERE;
		}

	ffpoly_identity_set(&tmp);
	expoly_setterm(&square[0],&tmp,1);		// Square[0] = x

    start = GetChronoTime();

	/* Compute (Z^q - Z) mod f(X), where q is the length of the
	   vector space defining the subfield. */

	for(i=0;i<extndegree;i++)
		{
#ifdef USE_FAST_SQUARING
		if(1 || cmod->pvdeg != 103)
			{
			if( expoly_square(&square[i+1],&square[i],&f,cmod) != 1 ) return 0;
			} 
		else
			{
			if( expolymod_squaremod(&square[i+1],&square[i],&fmod) != 1 ) return 0;
			}
#else
		if( expoly_square(&square[i+1],&square[i],&f,cmod) != 1 ) return 0;
#endif
		}
	
	end = GetChronoTime();

	if(expoly_cmp(&square[0],&square[extndegree])!=0)
		{
		/* 
		   Evaluate f0 = gcd( Z^q - Z mod f(x), f(x));
		   which contains all the roots of f(x) with multiplicity 1.
		*/
		expoly_addmod(&f1,&square[0],&square[extndegree],&f,cmod);
		expoly_gcd(&f0,&f1,&f,cmod);
		}
	else expoly_copy(&f0,&f);

#ifdef PRINTTIME
    printf("Elapsed = %3.2f\n",(end-start)/1000.0);
#endif

	/*
		if deg(f0) <= 1 then we can stop with 0 or 1 solutions found.
    */

	if(f0.pvdeg <= 0)	// No solutions found !
		goto EXIT_HERE;

	else if(f0.pvdeg == 1)
		{
		// We have only one solution !
		FFPOLY sol;

		ffpoly_field_div(&sol,&f0.ep[0],&f0.ep[1],cmod);
		if( expoly_pot_addsol(rp,&sol,NULL,cmod) != 1) flag = -1;

		goto EXIT_HERE;
		}

	/* 
	   Otherwise we have to keep trying to reduce f0 into smaller
       factors.
	*/

    expoly_pot_depositpoly(rp,&f0);

	/* Sigma computation */
	expoly_addmod(&S,&square[0],&square[1],&f0,cmod);

	/*
        compute:
		f1 = gcd( Tr(x) - c, f0(x) )
    */

	for(i = 2; i < extndegree ; i++)
		expoly_addmod(&S,&S,&square[i],&f0,cmod);

	if(expoly_checkroot(rp,&S,cmod)==0) 
		{
		flag = -2;
		goto EXIT_HERE;
		}

	/* do we have depleted the factors repository so we're done ? */
	if(expoly_deposit_is_empty(rp) != 0) goto EXIT_HERE;

	expoly_addone(&S);

	if(expoly_checkroot(rp,&S,cmod)==0) 
		{
		flag = -3;
		goto EXIT_HERE;
		}

	if( expoly_deposit_is_empty(rp) != 0 ) goto EXIT_HERE;

	/* Beta computation */
	ffpoly_identity_set(&beta);

	/* loop through all elements up to the degree of the
	   irreducible polynomial defining the subfield. */
	for(i = 1; i < extndegree ; i++)
		{
		ffpoly_shiftleft(&beta,&beta,1);	// Multiply by x
		ffpoly_reduce(&beta,cmod);

		expoly_zeroset(&S);
		if( expoly_muladd(&S,&square[0],&beta,cmod) != 1 )
			{
			flag = -10;
			goto EXIT_HERE;
			}

		ffpoly_square(&beta_square,&beta,cmod);

		// S = beta * x^1
		// beta square = S^2

		for(j=1;j<extndegree-1;j++)
			{
			if( expoly_muladd(&S,&square[j],&beta_square,cmod) != 1)
				{
				flag = -11;
				goto EXIT_HERE;
				}
			expoly_reduce(&S,&f0,cmod);

			ffpoly_square(&beta_square,&beta_square,cmod);
			}

		if( expoly_muladd(&S,&square[j],&beta_square,cmod) != 1 )
			{
			flag = -12;
			goto EXIT_HERE;
			}

		expoly_reduce(&S,&f0,cmod);

		// Root check phase 
		if(expoly_checkroot(rp,&S,cmod)==0) 
			{
			flag = -4;
			goto EXIT_HERE;
			}

		if(expoly_deposit_is_empty(rp) != 0) break;

		expoly_addone(&S);

		if(expoly_checkroot(rp,&S,cmod)==0) 
			{
			flag = -5;
			goto EXIT_HERE;
			}

		if(expoly_deposit_is_empty(rp) != 0) break;
		}

EXIT_HERE:

#ifdef USE_FAST_SQUARING
	expolymod_free_modulus(&fmod);
#endif

	expoly_free(&S);
	expoly_free(&f1);
	expoly_free(&f0);
	expoly_free(&f);

	if(square)
		{
		for(i=0;i<done;i++)
			expoly_free(&square[i]);

		free(square);
		}

	return flag;
	}
