
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
#include <string.h>

#include "quartzsig.h"
#include "lintra.h"
#include "polyprint.h"
#include "rootfind.h"
#include "qmtx.h"

#include "karatsuba.h"

#include "shs.h"
#include "osdepn.h"

//#define PRINT_DEBUG

static WORD16 vectmap[POLYBITVECT];
static int vectmap_inited = 0;

const int quartz_irr_ppoly[3] = { 103,9,0 };

int InitQuartzEngine(void)
	{
	int res = init_karatsuba();

	return res;
	}

void DeInitQuartzEngine(void)
	{
	free_karatsuba();

	return;
	}

void xorbitvect(WORD_8 *H,const char *bitvector,int inside)
	{
	int i;

	for(i=0;i<inside;i++)
		{
		WORD_8 bit = 0x80 >> (i & 0x07);
		if(bitvector[i] == '1') H[i >> 3] ^= bit;
		}
	}

int vect2int(WORD_8 *v,int len,int *n)
	{
	int i;
	
	if(len < 0 || len>32) return 0;

	*n=0;

	for(i=len-1;i>=0;i--)
		{
		*n<<=1;
		if(v[i] != 0) *n|=1;
		}

	return 1;
	}


/* Generate a random polynomial in F_2 of degree deg using RC4 as an CSPRNG */
int build_randpoly(RC4_CTX *rc4,FFPOLY *p,int deg,int leadcoeff)
	{
	int i;

	if(!p || !rc4 || deg <= 0 || deg >= MAXDEGREE) return 0;

	ffpoly_zeroset(p);
	
	if(leadcoeff != 0) ffpoly_setterm(p,deg);
	
	/* These are deg bits from 0 to deg-1 */
	for(i=0;i<deg;i++)
		{
		int bit = RC4_GetByte(rc4);
		if(bit & 0x01) ffpoly_setterm(p,i);
		}
	
	return 1;
	}

static int compute_beta_coefficient(FFPOLY *beta,FV_COMPONENTS_STRUCT *fc,int v,int i,FFPOLY *cmod)
	{
	int k;

	if(!beta || !fc || v<0 || v>=16 || i<0 || i>fc->pow || !cmod) return 0;

	ffpoly_zeroset(beta);

	/* V = (V_0, V_1, V_2, V_3) which belong to { 0,1 }^4 */
	for(k=0;k<4;k++)
		{
		if(((v << k) & 0x08) != 0)	
			ffpoly_addmod(beta,beta,&fc->b.epsilon[i][k],cmod);
		}

	ffpoly_addmod(beta,beta,&fc->b.ni[i],cmod);
	return 1;
	}

static int compute_gamma_coefficient(FFPOLY *gamma,FV_COMPONENTS_STRUCT *fc,int v,FFPOLY *cmod)
	{
	int k,l;

	if(!gamma || !fc || v<0 || v>=16 || !cmod) return 0;
	
	ffpoly_zeroset(gamma);

	/* V = (V_0, V_1, V_2, V_3) which belong to { 0,1 }^4 */
	for(k=0;k<4;k++)
		{
		int vk = (((v << k) & 0x08)!=0 ? 1 : 0);

		for(l=k+1;l<4 && vk;l++)
			{
			int vl = (((v << l) & 0x08)!=0 ? 1 : 0);
			if(vl) ffpoly_addmod(gamma,gamma,&fc->g.eta[k][l],cmod);
			}
		}

	for(k=0;k<4;k++)
		{
		if(((v << k) & 0x08) != 0)	
			ffpoly_addmod(gamma,gamma,&fc->g.rho[k],cmod);
		}

	ffpoly_addmod(gamma,gamma,&fc->g.tau,cmod);
	return 1;
	}

static void build_fv_components(RC4_CTX *rc4,FV_COMPONENTS_STRUCT *fc)
	{
	int i,j,k,l;

	/* Building alpha_i_j with 0<=(2^i + 2^j)<=129 */

	for(i=0;i<=fc->pow;i++)
		for(j=0;j<=fc->pow;j++)
			{
			if(i < j && ((1<<i) + (1<<j) <= fc->deg))
				build_randpoly(rc4,&fc->a.alpha[i][j],QUARTZ_POLY_DEGREE,0);
			else
				ffpoly_zeroset(&fc->a.alpha[i][j]);
			}

	/* Building the beta function's epsilons */
	for(i=0;i<=fc->pow;i++)
		for(k=0;k<4;k++)
			build_randpoly(rc4,&fc->b.epsilon[i][k],QUARTZ_POLY_DEGREE,0);
		
	/* Building the beta function's nis */
	for(i=0;i<=fc->pow;i++)
		build_randpoly(rc4,&fc->b.ni[i],QUARTZ_POLY_DEGREE,0);

	/* Building the gamma function's etas */
	for(k=0;k<4;k++)
		for(l=0;l<4;l++)
			{
			if(k<l) build_randpoly(rc4,&fc->g.eta[k][l],QUARTZ_POLY_DEGREE,0);
			else ffpoly_zeroset(&fc->g.eta[k][l]);
			}
	
	/* Building the gamma function's rhos */
	for(k=0;k<4;k++)
		build_randpoly(rc4,&fc->g.rho[k],QUARTZ_POLY_DEGREE,0);

	/* Building the gamma function's tau */
	build_randpoly(rc4,&fc->g.tau,QUARTZ_POLY_DEGREE,0);
		
	return;
	}

static int build_all_fvs(FV_STRUCT *fv,FV_COMPONENTS_STRUCT *fc)
	{
	FFPOLY cmod;
	int i,j,v;	

	ffpoly_set(&cmod,quartz_irr_ppoly,3);

	/* The 16 vinegar polys */
	for(v=0;v<16;v++)
		{
		FFPOLY gamma;

		expoly_zeroset(&fv->vinegar[v]);

		/* Setting poly alpha_i_j coefficients of X^(2^i + 2^j) */
		for(i=0;i<=fc->pow;i++)
			{
			for(j=0;j<=fc->pow;j++)
				{
				int deg = (1<<i) + (1<<j);
				if(i >= j || deg > fc->deg) continue;

				expoly_setterm(&fv->vinegar[v],&fc->a.alpha[i][j],deg);
				}
			}

		/* Setting poly beta_i coefficients of X^(2^i) */
		for(i=0;i<=fc->pow;i++)
			{
			FFPOLY beta;
			int deg = (1 << i);
			
			if( compute_beta_coefficient(&beta,fc,v,i,&cmod) != 1 ) 
				return 0;
			ffpoly_addmod(&fv->vinegar[v].ep[deg],&fv->vinegar[v].ep[deg],&beta,&cmod);
			}

		/* Setting poly gamma coefficient of X^0 */
		if( compute_gamma_coefficient(&gamma,fc,v,&cmod) != 1 ) 
			return 0;
		ffpoly_addmod(&fv->vinegar[v].ep[0],&fv->vinegar[v].ep[0],&gamma,&cmod);
		
		ffpoly_identity_set(&fv->vinegar[v].ep[fv->vinegar[v].pvdeg]);
		}

	return 1;
	}

int init_fv_struct(FV_STRUCT *fv)
	{
	if(!fv) return 0;

	for(fv->done=0;fv->done<16;fv->done++)
		{
		if(expoly_init(&fv->vinegar[fv->done],fv->deg+1) != 1) return 0;
		}

	return 1;
	}

void free_fv_struct(FV_STRUCT *fv)
	{
	int i;

	if(!fv) return;

	for(i=0;i<fv->done;i++)
		expoly_free(&fv->vinegar[i]);
	
	return;
	}

int build_fv_functions(RC4_CTX *rc4,FV_STRUCT *fv)
	{
	FV_COMPONENTS_STRUCT fc;

	memset(&fc,0x00,sizeof(&fc));

	fc.deg = fv->deg;
	fc.pow = fv->pow;

	build_fv_components(rc4,&fc);
	if(build_all_fvs(fv,&fc) != 1) return 0;

	return 1;
	}

static void choose_solution(FFPOLY *Z,ROOT_POT *pot)
	{
	FFPOLY *probe = &pot->factors[0];
	int i;
	
	/* Get the lower solution ! */
	for(i=1;i<pot->factsNo;i++)
		{
		if(ffpoly_cmp(probe,&pot->factors[i]) > 0) 
			probe = &pot->factors[i];
		}

	ffpoly_copy(Z,probe);
	}

int privkey_backward(QUARTZ_PRIVKEY *qpk,WORD_8 *out,WORD_8 *in)
	{
	FFPOLY Z,B,cmod;
	ROOT_POT pot;
	EXPOLY fact;
    int v=0;
	
#ifdef MATRIX_PACKING
	WORD32 ftmp0[4];

	QMtx_PackVector(in,QUARTZ_AFFINE_TRANSSIZE,ftmp0);

	ftmp0[0] ^= qpk->ptk[0];
	ftmp0[1] ^= qpk->ptk[1];
	ftmp0[2] ^= qpk->ptk[2];
	ftmp0[3] ^= qpk->ptk[3];

    ffpoly_zeroset(&B);

	QMtxVect4Mul(qpk->ptinv,ftmp0,B.pv);

	v = (B.pv[3] >> 7) & 0x0f;
	B.pv[3] &= 0x7f;

	ffpoly_fast_update_degree(&B,QUARTZ_POLY_DEGREE);

#else
    WORD_8 tmp1[QUARTZ_AFFINE_TRANSSIZE];
    WORD_8 tmp2[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 tmp0[QUARTZ_AFFINE_TRANSSIZE];
	int i;

    for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
        tmp0[i] = in[i] ^ qpk->tk[i];
	
	/* Apply the affine inverse trasnformation t^(-1) */
    vect_mult(qpk->tinv,tmp0,QUARTZ_AFFINE_TRANSSIZE,tmp2);

	/* Apply phi to construct B */
    ffpoly_zeroset(&B);

    for(i=0;i<QUARTZ_POLY_DEGREE;i++)
        if(tmp2[i]) ffpoly_setterm(&B,i);

    /* Collecting the candidate vinegar label to start with... */
    for(i=0,v=0;i<4;i++,v>>=1)
        if(tmp2[QUARTZ_POLY_DEGREE+i] != 0) v|=0x10;

#endif

    ffpoly_set(&cmod,quartz_irr_ppoly,3);

	//ffpoly_dump(&B);
	//printf("Vinegar = %d\n",v);

	/* Ok, let's try to solve Fv(Z) = B now */
	expoly_init(&fact,qpk->fv.deg+1);

	/* Construct and try to solve Fv(Z)-B = 0 */
	expoly_copy(&fact,&qpk->fv.vinegar[v]);
	ffpoly_addmod(&fact.ep[0],&fact.ep[0],&B,&cmod);

	expoly_pot_init(&pot,&cmod,fact.pvdeg);
	expoly_factor(&pot,&fact,&cmod);
	expoly_free(&fact);

	if(pot.factsNo == 0)
		{
		expoly_pot_free(&pot);
		return 0;
		}

	choose_solution(&Z,&pot);
	expoly_pot_free(&pot);
	
	//ffpoly_dump(&Z);

#ifdef MATRIX_PACKING

	Z.pv[3] |= (v << 7);

	Z.pv[0] ^= qpk->psk[0];
	Z.pv[1] ^= qpk->psk[1];
	Z.pv[2] ^= qpk->psk[2];
	Z.pv[3] ^= qpk->psk[3];

	QMtxVect4Mul(qpk->psinv,Z.pv,ftmp0);
	QMtx_UnPackVector(ftmp0,out,QUARTZ_AFFINE_TRANSSIZE);

#else

    for(i=0;i<QUARTZ_POLY_DEGREE;i++)
        tmp1[i] = (ffpoly_getterm(&Z,i) != 0 ? 1 : 0);

	/* Recompute the correct vinegar according to the solution picked up */
	for(i=QUARTZ_POLY_DEGREE;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		tmp1[i] = tmp2[i];

	//print_vector(tmp1,QUARTZ_AFFINE_TRANSSIZE,"S3 = ");

	/* Finally apply the affine trasnformation s^(-1) to get the result */
    for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
        tmp1[i] ^= qpk->sk[i];

   	//print_vector(tmp1,QUARTZ_AFFINE_TRANSSIZE,"S2 = ");

	vect_mult(qpk->sinv,tmp1,QUARTZ_AFFINE_TRANSSIZE,out);

	//print_vector(out,QUARTZ_AFFINE_TRANSSIZE,"S1 = ");
#endif

	//print_vector(out,QUARTZ_AFFINE_TRANSSIZE,"S1 = ");
	return 1;
	}

int privkey_forward(QUARTZ_PRIVKEY *qpk,WORD_8 *out,WORD_8 *in)
    {
    WORD_8 tmp1[QUARTZ_AFFINE_TRANSSIZE];
    WORD_8 tmp2[QUARTZ_AFFINE_TRANSSIZE];
    FFPOLY Z,B,cmod;
    int i,v=0;

    if(!qpk || !qpk->isOk) return 0;

	//print_vector(in,QUARTZ_AFFINE_TRANSSIZE,"S1 = ");

	/* Affine trasformation through the s matrix */
	//print_vector(tmp0,QUARTZ_AFFINE_TRANSSIZE,"S2 = ");
    
	vect_mult(qpk->s,in,QUARTZ_AFFINE_TRANSSIZE,tmp1);

	//print_vector(tmp0,QUARTZ_AFFINE_TRANSSIZE,"S2 = ");

    for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
        tmp1[i] ^= qpk->sk[i];

	//print_vector(tmp1,QUARTZ_AFFINE_TRANSSIZE,"S3 = ");

	/* Applying the phi function to map: { 0,1 }^103 --> F_2[x] / (x^103 + x^9 + 1) */
    ffpoly_zeroset(&Z);

    for(i=0;i<QUARTZ_POLY_DEGREE;i++)
        if(tmp1[i]) ffpoly_setterm(&Z,i);

    /* Collecting the vinegar now... */
    for(i=0,v=0;i<4;i++,v>>=1)
        if(tmp1[QUARTZ_POLY_DEGREE+i] != 0) v|=0x10;

	//poly_print(&Z,"Z=");
	//printf("Vinegar = %d\n",v);

	/* Compute B = Fv(Z) */
    ffpoly_set(&cmod,quartz_irr_ppoly,3);
    expoly_subst(&B,&qpk->fv.vinegar[v],&Z,&cmod);

	//poly_print(&B,"B=");

	/* Now applying phi^(-1) to map back: F_2[x] / (x^103 + x^9 + 1) --> { 0,1 }^103 */
    for(i=0;i<QUARTZ_POLY_DEGREE;i++)
        tmp2[i] = (ffpoly_getterm(&B,i) != 0 ? 1 : 0);

	/* Trasfer the vinegar to output as is... */
	for(i=QUARTZ_POLY_DEGREE;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		tmp2[i] = tmp1[i];

	//print_vector(tmp2,QUARTZ_AFFINE_TRANSSIZE,"S4 = ");

	/* Finally apply the affine trasnformation t to get the result */
    vect_mult(qpk->t,tmp2,QUARTZ_AFFINE_TRANSSIZE,out);

    for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
        out[i] ^= qpk->tk[i];

    return 1;
    }


static int map_xy2id(int x,int y)
	{
	if(x == 0xff && y == 0xff) return 0;

	if(x < 0 || x >= QUARTZ_AFFINE_TRANSSIZE || y<x || y >= QUARTZ_AFFINE_TRANSSIZE) return -1;

	if(x == y) return 1 + x;

	/* 
	
	This forumula is calculated as follow:
	  
	let P(n) = 1 + 2 + ... + n = n*(n-1)/2 the sum of the first n integers and
	a = QUARTZ_AFFINE_TRANSSIZE, then it can be easily proved that:

	id = 1 + a + P(a - 1) - P(a - 1 - x) + y - x - 1

	simplifying the terms above yields:

    id = ((2a - 3 - x) * x ) / 2 + a + y
	*/

	return (((2 * QUARTZ_AFFINE_TRANSSIZE - 3) - x) * x ) / 2 + QUARTZ_AFFINE_TRANSSIZE + y;
	}

static int init_vector_map()
	{
	int i,j,k;

	vectmap[0] = 0xffff;
	
	for(k=1;k<=QUARTZ_AFFINE_TRANSSIZE;k++)
		vectmap[k] = ((k-1)<< 8) | (k-1);
	
	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE-1;i++)
		for(j=i+1;j<QUARTZ_AFFINE_TRANSSIZE;j++)
			{
			if(map_xy2id(i,j) != k) return 0;
			vectmap[k++] = (i << 8) | j;
			}

	if( k != POLYBITVECT) return 0;

	return 1;
	}

static void compute_packed_matrices(QUARTZ_PRIVKEY *qpk)
	{
	int i;

	if(!qpk) return;

	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		{
		QMtx_PackVector(qpk->sinv[i],QUARTZ_AFFINE_TRANSSIZE,&qpk->psinv[4*i]);
		QMtx_PackVector(qpk->tinv[i],QUARTZ_AFFINE_TRANSSIZE,&qpk->ptinv[4*i]);
		}

	QMtx_PackVector(qpk->sk,QUARTZ_AFFINE_TRANSSIZE,qpk->psk);
	QMtx_PackVector(qpk->tk,QUARTZ_AFFINE_TRANSSIZE,qpk->ptk);
	}

/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */

int init_priv_key(QUARTZ_PRIVKEY *qpk,int pow)
	{
	int i;

	if(!qpk) return 0;
	
	qpk->isOk = 0;

	if(pow < D_MINPOLYFIGURE || pow > D_MAXPOLYFIGURE) return 0;

	qpk->fv.pow = pow;
	qpk->fv.deg = (1 << pow) + 1;

	qpk->s    = qpk->t    = NULL;
    qpk->sk   = qpk->tk   = NULL;
	qpk->sinv = qpk->tinv = NULL;

	memset(&qpk->delta.secret,0x00,10);

	if( init_fv_struct(&qpk->fv) != 1 ) return free_priv_key(qpk);
	
	qpk->s = (WORD_8 **)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *) );
	if(!qpk->s) return free_priv_key(qpk);
	memset(qpk->s,0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *));

	qpk->sinv = (WORD_8 **)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *) );
	if(!qpk->sinv) return free_priv_key(qpk);
	memset(qpk->sinv,0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *));

    qpk->sk = (WORD_8 *)malloc( QUARTZ_AFFINE_TRANSSIZE );
    if(!qpk->sk) return free_priv_key(qpk);
    memset(qpk->sk,0x00,QUARTZ_AFFINE_TRANSSIZE);

	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		{
		qpk->s[i] = (WORD_8 *)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8) );
		if(!qpk->s[i]) return free_priv_key(qpk);
		memset(qpk->s[i],0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8));
		}
	
	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		{
		qpk->sinv[i] = (WORD_8 *)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8) );
		if(!qpk->sinv[i]) return free_priv_key(qpk);
		memset(qpk->sinv[i],0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8));
		}
	
	qpk->t = (WORD_8 **)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *) );
	if(!qpk->t) return free_priv_key(qpk);
	memset(qpk->t,0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *));

	qpk->tinv = (WORD_8 **)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *) );
	if(!qpk->tinv) return free_priv_key(qpk);
	memset(qpk->tinv,0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8 *));

    qpk->tk = (WORD_8 *)malloc( QUARTZ_AFFINE_TRANSSIZE );
    if(!qpk->tk) return free_priv_key(qpk);
    memset(qpk->tk,0x00,QUARTZ_AFFINE_TRANSSIZE);

	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		{
		qpk->t[i] = (WORD_8 *)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8) );
		if(!qpk->t[i]) return free_priv_key(qpk);
		memset(qpk->t[i],0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8));
		}

	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		{
		qpk->tinv[i] = (WORD_8 *)malloc( QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8) );
		if(!qpk->tinv[i]) return free_priv_key(qpk);
		memset(qpk->tinv[i],0x00,QUARTZ_AFFINE_TRANSSIZE * sizeof(WORD_8));
		}

	return ( qpk->isOk = 1 );
	}

int free_priv_key(QUARTZ_PRIVKEY *qpk)
	{
	int i;

	if(!qpk) return 0;

	if(qpk->s)
		{
		for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
			if(qpk->s[i]) free(qpk->s[i]);

		free(qpk->s);
		}
	
	if(qpk->t)
		{
		for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
			if(qpk->t[i]) free(qpk->t[i]);

		free(qpk->t);
		}

	if(qpk->sinv)
		{
		for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
			if(qpk->sinv[i]) free(qpk->sinv[i]);

		free(qpk->sinv);
		}

	if(qpk->tinv)
		{
		for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
			if(qpk->tinv[i]) free(qpk->tinv[i]);

		free(qpk->tinv);
		}
	
    if(qpk->sk) free(qpk->sk);
    if(qpk->tk) free(qpk->tk);

	qpk->s  = qpk->t  = NULL;
	qpk->sk = qpk->tk = NULL;

	free_fv_struct(&qpk->fv);

	return 0;
	}

int create_priv_key(RC4_CTX *rc4,QUARTZ_PRIVKEY *qpk)
	{
	int i;

	if(!rc4 || !qpk || !qpk->isOk) return 0;

	/* Just discard the first 256 bytes... */
	for(i=0;i<256;i++)
		RC4_GetByte(rc4);

	create_invertible_matrix(rc4,qpk->s,QUARTZ_AFFINE_TRANSSIZE);
	create_invertible_matrix(rc4,qpk->t,QUARTZ_AFFINE_TRANSSIZE);
	
	invmtx(qpk->s,qpk->sinv,QUARTZ_AFFINE_TRANSSIZE);
	invmtx(qpk->t,qpk->tinv,QUARTZ_AFFINE_TRANSSIZE);

    for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
        {
        qpk->sk[i] = (RC4_GetByte(rc4) & 0x01);
        qpk->tk[i] = (RC4_GetByte(rc4) & 0x01);
        }

	compute_packed_matrices(qpk);

	build_fv_functions(rc4,&qpk->fv);

	for(i=0;i<DELTA_SECRET_LEN;i++)
		qpk->delta.secret[i] = RC4_GetByte(rc4);

	return 1;
	}

int create_public_from_private(QUARTZ_PRIVKEY *qpriv,QUARTZ_PUBLICKEY *qpub)
	{
    WORD_8 in[QUARTZ_AFFINE_TRANSSIZE];
    WORD_8 out[QUARTZ_AFFINE_TRANSSIZE];
	int i,j,k;
	int b1,b2,b3,b4;
	int count;

	if(!qpriv || !qpub || !qpriv->isOk) return 0;
	
	qpub->isOk = 0;

	if(vectmap_inited == 0) 
		{
		vectmap_inited = 1;
		if( init_vector_map() != 1) return 0;
		}

	/* Polynomial interpolation */
	for(i=0;i<QUARTZ_POOL_SIZE;i++)
		QuartzPubKeyReset(qpub,i);

	/* Computing alpha_i */

	memset(in,0x00,QUARTZ_AFFINE_TRANSSIZE);
	privkey_forward(qpriv,out,in);

	count = 0;

	for(i=0;i<QUARTZ_POOL_SIZE;i++)
		if(out[i]) QuartzPubKeySetBit(qpub,i,count);

	/* Calculating beta_i_j */

	count = 1;
	for(j=0;j<QUARTZ_AFFINE_TRANSSIZE;j++)
		{
		memset(in,0x00,QUARTZ_AFFINE_TRANSSIZE);
		in[j] = 1;

		privkey_forward(qpriv,out,in);
			
		for(i=0;i<QUARTZ_POOL_SIZE;i++)
			{
			QuartzPubKeyGetBit(qpub,i,0,&b1);
				
			b2 = b1 ^ out[i];

			if(b2) QuartzPubKeySetBit(qpub,i,count);
			}

		count++;
		}

	/* Calculating gamma_i_j_k */

	for(j=0;j<QUARTZ_AFFINE_TRANSSIZE-1;j++)
		for(k=j+1;k<QUARTZ_AFFINE_TRANSSIZE;k++)
			{
			memset(in,0x00,QUARTZ_AFFINE_TRANSSIZE);
			in[j] = in[k] = 1;

			privkey_forward(qpriv,out,in);
				
			for(i=0;i<QUARTZ_POOL_SIZE;i++)
				{
				QuartzPubKeyGetBit(qpub,i,  0,&b1);
				QuartzPubKeyGetBit(qpub,i,1+j,&b2);
				QuartzPubKeyGetBit(qpub,i,1+k,&b3);
					
				b4 = b1 ^ b2 ^ b3 ^ out[i];

				if(b4) QuartzPubKeySetBit(qpub,i,count);
				}

			count++;
			}

	if( count != POLYBITVECT) return 0;

	qpub->isOk = 1;

	return 1;
	}

int buffer2privkey(WORD_8 *buffer,int buffsize,QUARTZ_PRIVKEY *qpk)
	{
	SHS_INFO sInfo;
	WORD32 exp_signature = PRIVKEY_BODYSIGNATURE;
	WORD32 exp_version = PRIVKEY_EXPECTED_VERSION;
	int curr=0;
	int i,j;

	if(!qpk || !qpk->isOk) return -1;

	qpk->isOk = 0;

	if((curr + (int)(sizeof(exp_signature))) > buffsize) return -2;
	if(memcmp(&buffer[curr],&exp_signature,sizeof(exp_signature)) != 0) return -3;
	curr+=sizeof(exp_signature);

	if((curr + (int)(sizeof(exp_version))) > buffsize) return -2;
	if(memcmp(&buffer[curr],&exp_version,sizeof(exp_version)) != 0) return -5;
	curr+=sizeof(exp_version);

	if((curr + (int)(sizeof(qpk->fv.deg))) > buffsize) return -2;
	if(memcmp(&buffer[curr],&qpk->fv.deg,sizeof(qpk->fv.deg)) != 0) return -6;
	curr+=sizeof(qpk->fv.deg);

	if((curr + (int)(sizeof(qpk->fv.pow))) > buffsize) return -2;
	if(memcmp(&buffer[curr],&qpk->fv.pow,sizeof(qpk->fv.pow)) != 0) return -7;
	curr+=sizeof(qpk->fv.pow);

	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		{
		if((curr + LINESMEM) > buffsize) return -2;
		memset(qpk->s[i],0x00,QUARTZ_AFFINE_TRANSSIZE);
		bitvect2bytevect(&buffer[curr],qpk->s[i],QUARTZ_AFFINE_TRANSSIZE);
		curr+= LINESMEM;

		if((curr + LINESMEM) > buffsize) return -2;
		memset(qpk->t[i],0x00,QUARTZ_AFFINE_TRANSSIZE);
		bitvect2bytevect(&buffer[curr],qpk->t[i],QUARTZ_AFFINE_TRANSSIZE);
		curr+= LINESMEM;

		if((curr + LINESMEM) > buffsize) return -2;
		memset(qpk->sinv[i],0x00,QUARTZ_AFFINE_TRANSSIZE);
		bitvect2bytevect(&buffer[curr],qpk->sinv[i],QUARTZ_AFFINE_TRANSSIZE);
		curr+= LINESMEM;

		if((curr + LINESMEM) > buffsize) return -2;
		memset(qpk->tinv[i],0x00,QUARTZ_AFFINE_TRANSSIZE);
		bitvect2bytevect(&buffer[curr],qpk->tinv[i],QUARTZ_AFFINE_TRANSSIZE);
		curr+= LINESMEM;
		}

	if((curr + LINESMEM) > buffsize) return -2;
	memset(qpk->sk,0x00,QUARTZ_AFFINE_TRANSSIZE);
	bitvect2bytevect(&buffer[curr],qpk->sk,QUARTZ_AFFINE_TRANSSIZE);
	curr+= LINESMEM;

	if((curr + LINESMEM) > buffsize) return -2;
	memset(qpk->tk,0x00,QUARTZ_AFFINE_TRANSSIZE);
	bitvect2bytevect(&buffer[curr],qpk->tk,QUARTZ_AFFINE_TRANSSIZE);
	curr+= LINESMEM;

	if((curr + DELTA_SECRET_LEN) > buffsize) return -2;
	memcpy(qpk->delta.secret,&buffer[curr],DELTA_SECRET_LEN);
	curr+= DELTA_SECRET_LEN;
	
	for(i=0;i<16;i++)
		{
		expoly_zeroset(&qpk->fv.vinegar[i]);

		for(j=0;j<=qpk->fv.deg;j++)
			{
			ffpoly_zeroset(&qpk->fv.vinegar[i].ep[j]);

			if(count_bits(j) <= 2)
				{
				if((curr + (int)(sizeof(qpk->fv.vinegar[i].ep[j].pvdeg))) > buffsize) return -2;
				memcpy(&qpk->fv.vinegar[i].ep[j].pvdeg,&buffer[curr],
					sizeof(qpk->fv.vinegar[i].ep[j].pvdeg));
				curr+=sizeof(qpk->fv.vinegar[i].ep[j].pvdeg);

				if((curr + 16) > buffsize) return -2;
				memcpy(&qpk->fv.vinegar[i].ep[j].pv[0],&buffer[curr],16);
			
				curr+=16;
		
				qpk->fv.vinegar[i].pvdeg = qpk->fv.deg;
				}
			}
		}

	shsInit(&sInfo);
	shsUpdate(&sInfo,buffer,curr);
	shsFinal(&sInfo);

	if((curr + SHS_DIGESTSIZE) > buffsize) return -2;
	if(memcmp(&buffer[curr],sInfo.digest,SHS_DIGESTSIZE) != 0) return -8;
	curr+=SHS_DIGESTSIZE;

	compute_packed_matrices(qpk);

	qpk->isOk = 1;
	return 1;
	}

int privkey2buffer(QUARTZ_PRIVKEY *qpk,WORD_8 *buffer,int *maxlen)
	{
	SHS_INFO sInfo;
	int i,j;
	int curr;
	WORD32 signature = PRIVKEY_BODYSIGNATURE;
	WORD32 version = PRIVKEY_EXPECTED_VERSION;
	int wot;

	if(!qpk || !qpk->isOk) return -1;

	for(wot=0;wot<=1;wot++)
		{
		curr = 0;

		if(wot==1) memset(buffer,0x00,*maxlen);

		if(wot==1) memcpy(&buffer[curr],&signature,sizeof(signature));
		curr+=sizeof(signature);

		if(wot==1) memcpy(&buffer[curr],&version,sizeof(version));
		curr+=sizeof(version);

		if(wot==1) memcpy(&buffer[curr],&qpk->fv.deg,sizeof(qpk->fv.deg));
		curr+=sizeof(qpk->fv.deg);

		if(wot==1) memcpy(&buffer[curr],&qpk->fv.pow,sizeof(qpk->fv.pow));
		curr+=sizeof(qpk->fv.pow);

		for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
			{
			if(wot==1) bytevect2bitvect(qpk->s[i],QUARTZ_AFFINE_TRANSSIZE,&buffer[curr]);
			curr+= LINESMEM;

			if(wot==1) bytevect2bitvect(qpk->t[i],QUARTZ_AFFINE_TRANSSIZE,&buffer[curr]);
			curr+= LINESMEM;

			if(wot==1) bytevect2bitvect(qpk->sinv[i],QUARTZ_AFFINE_TRANSSIZE,&buffer[curr]);
			curr+= LINESMEM;

			if(wot==1) bytevect2bitvect(qpk->tinv[i],QUARTZ_AFFINE_TRANSSIZE,&buffer[curr]);
			curr+= LINESMEM;
			}

		if(wot==1) bytevect2bitvect(qpk->sk,QUARTZ_AFFINE_TRANSSIZE,&buffer[curr]);
		curr+= LINESMEM;

		if(wot==1) bytevect2bitvect(qpk->tk,QUARTZ_AFFINE_TRANSSIZE,&buffer[curr]);
		curr+= LINESMEM;

		if(wot==1) memcpy(&buffer[curr],qpk->delta.secret,DELTA_SECRET_LEN);
		curr+= DELTA_SECRET_LEN;

		for(i=0;i<16;i++)
			{
			for(j=0;j<=qpk->fv.deg;j++)
				{
				if(count_bits(j) <= 2)
					{
					int deg = qpk->fv.vinegar[i].ep[j].pvdeg;
				
					if(wot==1) memcpy(&buffer[curr],&deg,sizeof(deg));
					curr+=sizeof(deg);

					if(wot==1) memcpy(&buffer[curr],&qpk->fv.vinegar[i].ep[j].pv[0],16);
					curr+=16;
					}
				}
			}

		if(wot==1)
			{
			shsInit(&sInfo);
			shsUpdate(&sInfo,buffer,curr);
			shsFinal(&sInfo);

			memcpy(&buffer[curr],sInfo.digest,SHS_DIGESTSIZE);
			}

		curr+=SHS_DIGESTSIZE;

		if(*maxlen < curr || !buffer)
			{
			if(wot == 1) return -1;

			*maxlen = curr;
			return 0;
			}
		}

	return curr;
	}

int pubkey2buffer(QUARTZ_PUBLICKEY *qpub,WORD_8 *buffer,int *maxlen)
	{
	SHS_INFO sInfo;
	WORD32 signature = PUBKEY_BODYSIGNATURE;
	WORD32 version = 0x00010001;
	int curr;
	int wot;
	int i;

	if(!qpub || !qpub->isOk) return 0;

	for(wot=0;wot<=1;wot++)
		{
		curr = 0;

		if(wot==1) memset(buffer,0x00,*maxlen);

		if(wot==1) memcpy(&buffer[curr],&signature,sizeof(signature));
		curr+=sizeof(signature);

		if(wot==1) memcpy(&buffer[curr],&version,sizeof(version));
		curr+=sizeof(version);

		for(i=0;i<QUARTZ_POOL_SIZE;i++)
			{
			if(wot==1) memcpy(&buffer[curr],qpub->p[i],PACKPOLYVECTOR * 4);
			curr+= (PACKPOLYVECTOR * 4);
			}

		if(wot==1)
			{
			shsInit(&sInfo);
			shsUpdate(&sInfo,buffer,curr);
			shsFinal(&sInfo);

			memcpy(&buffer[curr],sInfo.digest,SHS_DIGESTSIZE);
			}

		curr+=SHS_DIGESTSIZE;

		if(*maxlen < curr || !buffer)
			{
			if(wot == 1) return -1;

			*maxlen = curr;
			return 0;
			}
		}

	return curr;
	}

int CheckVariableSizeMessageString(const char *bitvect,const int inside)
	{
	int len,i;
	
	if(!bitvect) return -1;
	if(inside < 0 || inside > QVC_MAXINSIDE) return -2;

	len = (int)strlen(bitvect);
	if(len == 0 || len > QVC_MAXMSGLENGTH) return -3;

	if(inside >= len) return -4;

	for(i=0;i<len;i++)
		if(bitvect[i] != '1' && bitvect[i] != '0') return -5;

	return len;
	}

static int QuartzForwardTransform(QUARTZ_PRIVKEY *qpk,WORD_8 *H,WORD_8 *S)
	{
	SHS_INFO rvInfo;
	WORD_8 Y[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 OT[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 rpool[SHS_DIGESTSIZE + DELTA_SECRET_LEN];
	int vc[QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8];
	int index;
	int done;
	int i,j;
	int count=0;

	memset(S,0x00,QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8);

	for(i=0;i<QUARTZ_ROUNDS;i++)
		{
		bitvect2bytevect(&H[i * 13],Y,QUARTZ_POOL_SIZE);

		for(j=0;j<QUARTZ_POOL_SIZE;j++)
			Y[j] ^= S[j];

		/* Actually we are appending four zeros bit to Y for clarity before concatenating 
		   the secret delta (because 8 divides 104 (13*8 = 104), but not 100). 
		   We would have othewise had to operate SHA at a bit level instead of a byte level. 
		   This would have required a more complex code in exchange of no
		   extra security. 
		   This is however against what is specified in the Quartz paper.
		*/

		bytevect2bitvect(Y,QUARTZ_POOL_SIZE,rpool);
		memcpy(&rpool[13],qpk->delta.secret,10);

		shsInit(&rvInfo);
		shsUpdate(&rvInfo,rpool,23);
		shsFinal(&rvInfo);

		for(j=0;j<128;j++)
			vc[j] = 0;

		done = QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8;

		while( done > 0 )
			{
			bitvect2bytevect((WORD_8 *)rvInfo.digest,&Y[QUARTZ_POOL_SIZE],7);
			
			vect2int(&Y[QUARTZ_POOL_SIZE],7,&index);
			index &= 0x7f;
			
			memset(OT,0x00,QUARTZ_AFFINE_TRANSSIZE);
			if(vc[index] == 0)
				{
				count++;
				if( privkey_backward(qpk,OT,Y) == 1 ) break;

				vc[index]++;
				done--;
				}

			memcpy(rpool,rvInfo.digest,SHS_DIGESTSIZE);

			shsInit(&rvInfo);
			shsUpdate(&rvInfo,rpool,SHS_DIGESTSIZE);
			shsFinal(&rvInfo);
			}
	
		if(done == 0) /* Yup! Cannot sign this message. Prob. 2^(-185) */
			return 0;	

		for(j=0;j<QUARTZ_POOL_SIZE;j++)
			S[j] = OT[j];

		for(j=0;j<7;j++)
			S[QUARTZ_POOL_SIZE + i * 7 + j] = OT[QUARTZ_POOL_SIZE + j];
		}

	return count;
	}

int QuartzSignMessage(const WORD_8 *msg,int msgsize,QUARTZ_PRIVKEY *qpk,WORD_8 *signature)
	{
	WORD_8 H[3 * SHS_DIGESTSIZE];
	WORD_8 S[QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8];
	int r;

	if(!qpk || !qpk->isOk) return 0;
	if( ComputeMessageHash(msg,msgsize,H) != 1 ) return 0;
	
	r = QuartzForwardTransform(qpk,H,S);
	if( r < 1 ) return 0;

	bytevect2bitvect(S,QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8,signature);
	return r;
	}

int QuartzSignVariableCode(const char *bitvect,const int inside,QUARTZ_PRIVKEY *qpk,char *signature)
	{
	WORD_8 H[3 * SHS_DIGESTSIZE];
	WORD_8 S[QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8];
	const int sigsize = QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8;
	int len,outside;
	int i,j;

	if(!qpk || !qpk->isOk) return -1;
	
	len =  CheckVariableSizeMessageString(bitvect,inside);
	if(len < 0) return len;

	outside = len - inside;

	/* 
		Bit vector is:
		[0..inside) | [0..outside)
	*/

	ComputeMessageHash((const unsigned char *)bitvect + inside,outside,H);
	xorbitvect(H,bitvect,inside);

	if( QuartzForwardTransform(qpk,H,S) < 1 ) return 0;

	/* 
		Signature is:
		[0..outside) | [0..sigsize)
	*/

	for(i=0,j=inside;i<outside;i++,j++)
		signature[i] = bitvect[j];

	for(i=outside,j=0;j<sigsize;i++,j++)
		signature[i] = (S[j] != 0 ? '1' : '0');

	signature[i] = '\0';
	return i;
	}

