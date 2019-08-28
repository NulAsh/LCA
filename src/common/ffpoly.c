
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#define FASTMOD103
#define KARATSUBA_UNROLL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "ffpoly.h"
#include "karatsuba.h"
#include "fred103.h"

#include "osdepn.h"

int debug_muls;
int debug_adds;

/* This module works with polynomials with coefficients defined in F_2,
   that is polynomials with coefficients which are either 0 or 1. */

/* Special optimizations are done to speed up operations in the finite field
   F_2[x] / { X^103 + X^9 + 1 } which is heavily used by the Quartz signature
   scheme. */
   
/* -------------------------------------------------------------------- */

/* Table lookup used to perform fast ffpoly normalization (degree recomputation) */
WORD_8 idxlup[256] = {
	0x00,0x01,0x02,0x02,0x03,0x03,0x03,0x03,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
	0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,
	0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
	0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
	0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
	0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
	0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
	0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08
	};

/* Table lookup used to perform fast squaring of an ffpoly */
WORD16 FSQR[256] = {
    0x0000,0x0001,0x0004,0x0005,0x0010,0x0011,0x0014,0x0015,
    0x0040,0x0041,0x0044,0x0045,0x0050,0x0051,0x0054,0x0055,
    0x0100,0x0101,0x0104,0x0105,0x0110,0x0111,0x0114,0x0115,
    0x0140,0x0141,0x0144,0x0145,0x0150,0x0151,0x0154,0x0155,
    0x0400,0x0401,0x0404,0x0405,0x0410,0x0411,0x0414,0x0415,
    0x0440,0x0441,0x0444,0x0445,0x0450,0x0451,0x0454,0x0455,
    0x0500,0x0501,0x0504,0x0505,0x0510,0x0511,0x0514,0x0515,
    0x0540,0x0541,0x0544,0x0545,0x0550,0x0551,0x0554,0x0555,
    0x1000,0x1001,0x1004,0x1005,0x1010,0x1011,0x1014,0x1015,
    0x1040,0x1041,0x1044,0x1045,0x1050,0x1051,0x1054,0x1055,
    0x1100,0x1101,0x1104,0x1105,0x1110,0x1111,0x1114,0x1115,
    0x1140,0x1141,0x1144,0x1145,0x1150,0x1151,0x1154,0x1155,
    0x1400,0x1401,0x1404,0x1405,0x1410,0x1411,0x1414,0x1415,
    0x1440,0x1441,0x1444,0x1445,0x1450,0x1451,0x1454,0x1455,
    0x1500,0x1501,0x1504,0x1505,0x1510,0x1511,0x1514,0x1515,
    0x1540,0x1541,0x1544,0x1545,0x1550,0x1551,0x1554,0x1555,
    0x4000,0x4001,0x4004,0x4005,0x4010,0x4011,0x4014,0x4015,
    0x4040,0x4041,0x4044,0x4045,0x4050,0x4051,0x4054,0x4055,
    0x4100,0x4101,0x4104,0x4105,0x4110,0x4111,0x4114,0x4115,
    0x4140,0x4141,0x4144,0x4145,0x4150,0x4151,0x4154,0x4155,
    0x4400,0x4401,0x4404,0x4405,0x4410,0x4411,0x4414,0x4415,
    0x4440,0x4441,0x4444,0x4445,0x4450,0x4451,0x4454,0x4455,
    0x4500,0x4501,0x4504,0x4505,0x4510,0x4511,0x4514,0x4515,
    0x4540,0x4541,0x4544,0x4545,0x4550,0x4551,0x4554,0x4555,
    0x5000,0x5001,0x5004,0x5005,0x5010,0x5011,0x5014,0x5015,
    0x5040,0x5041,0x5044,0x5045,0x5050,0x5051,0x5054,0x5055,
    0x5100,0x5101,0x5104,0x5105,0x5110,0x5111,0x5114,0x5115,
    0x5140,0x5141,0x5144,0x5145,0x5150,0x5151,0x5154,0x5155,
    0x5400,0x5401,0x5404,0x5405,0x5410,0x5411,0x5414,0x5415,
    0x5440,0x5441,0x5444,0x5445,0x5450,0x5451,0x5454,0x5455,
    0x5500,0x5501,0x5504,0x5505,0x5510,0x5511,0x5514,0x5515,
    0x5540,0x5541,0x5544,0x5545,0x5550,0x5551,0x5554,0x5555
    };

#define NTH_PRIME_TABLE_SIZE  54
static int nth_prime[NTH_PRIME_TABLE_SIZE] = {
	2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,
	73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,
	151,157,163,167,173,179,181,191,193,197,199,211,223,227,
	229,233,239,241,251
	};

int ffpoly_fast_update_degree(FFPOLY *rs,int maxdeg)
	{
	int seg,idx;
	unsigned char *uc;
	
	if(!rs || maxdeg >= MAXDEGREE) return 0;

	if(maxdeg < 0)
		{
		rs->pvdeg = -1;
		return 1;
		}
	
	uc = (unsigned char *)rs->pv;

	idx = (maxdeg >> 3) & 0x1f;
	seg = (maxdeg & 0xf8)-1;

	do
		{
		int pf = idxlup[uc[idx]];
		if(pf != 0) 
			{
			rs->pvdeg = pf+seg;
			return 1;
			}
		
		seg-=8;
		idx--;
		} while(idx>=0);

	rs->pvdeg = -1;
	return 1;
	}

static int ffpoly_update_degree(FFPOLY *rs)
	{
	int i;

	if(!rs) return 0;

	rs->pvdeg = MAXDEGREE - 1;

	for(i = SCALED_DEGREE - 1; i >= 0 && rs->pvdeg >= 0; i--, rs->pvdeg-=PWORDSIZE)
		{
		if(rs->pv[i] != 0)
			{
			WORD32 tmp = rs->pv[i];

			while((tmp & 0x80000000) == 0)
				{
				tmp <<=1;
				rs->pvdeg--;
				}
				
			return 1;
			}
		}

	return 1;
	}

/* Assumes no factor is == 0, otherwise the result is still correct but slow to compute */
static int ffpoly_plainmul(FFPOLY *rs,FFPOLY *p1,FFPOLY *p2,FFPOLY *modulus)
	{
	FFPOLY pr,pq;
	int mxdeg;
	int i,j,k;

    /* Precompute the expected final degree... */
    pr.pvdeg = p1->pvdeg + p2->pvdeg;

	if( pr.pvdeg >= MAXDEGREE) return 0;

	//ffpoly_zeroset(&pr);
    memset(&pr.pv,0x00,SCALED_WORD_8VEC);

	mxdeg = p2->pvdeg >> PWORDBITS;

    //ffpoly_copy(&pq,p1);

	for(j = 0;j <= mxdeg; j++)
		{
		int idx = j << PWORDBITS;
        int ij  = p2->pv[j];
        int idk = p2->pvdeg - idx + 1;
        int top = MIN(PWORDSIZE,idk);

		if(p2->pv[j] == 0) continue;

		for(i = 0;i < top; i++)
			{
			/* Just avoid to multiply by 0 */
			if( (ij & 0x01 ) != 0 )
                {
			    if( ffpoly_shiftleft(&pq,p1,i+idx) == 0 ) return 0;

                // Manually add and let's reduce it at the end once for all
                for(k= 0; k < SCALED_DEGREE; k++)
                    pr.pv[k] ^= pq.pv[k];
                }

            ij>>=1;
            //ffpoly_fast_shiftleft(&pq);
			}
		}

    if(modulus) ffpoly_reduce(&pr,modulus);	
	ffpoly_copy(rs,&pr);
	return 1;
	}

/* -------------------------------------------------------------------- */

/* Global functions ******************************** */

/*  computes P(x) = P(x) + 1 */
int ffpoly_addone(FFPOLY *p)
	{
	if(!p) return 0;

	p->pv[0] ^= 0x01;

	if(p->pvdeg > 0) return 1;
	else if(p->pvdeg == 0)
		{
		p->pvdeg = -1;
		return 1;
		}
	else
		{
		p->pvdeg = 0;
		return 1;
		}
	}

/* Compare two polynomials */
int ffpoly_cmp(FFPOLY *p1,FFPOLY *p2)
	{
	int i;

	if(p1->pvdeg < p2->pvdeg) return -1;
	else if(p1->pvdeg > p2->pvdeg) return 1;

	for(i = SCALED_DEGREE - 1; i >= 0; i--)
		{
		if(p1->pv[i] < p2->pv[i]) return -1;
		else if(p1->pv[i] > p2->pv[i]) return 1;
		}

	return 0;
	}

/* returns 1 if P(x) = 0 */
int ffpoly_is_zero(FFPOLY *p)
	{
	if(p->pvdeg < 0) return 1;
	return 0;
	}

/* returns 1 if P(x) = 1 */
int ffpoly_is_identity(FFPOLY *p)
	{
	if(p->pvdeg == 0) return 1;
	return 0;
	}

/* Returns 1 if P(x) is divisible by x (no constant term present) */
int ffpoly_is_even(FFPOLY *p)
	{
	if(p->pvdeg < 0) return 1;

	return (p->pv[0] & 0x01) == 0 ? 1 : 0;
	}

/* copy poly src to dest */
int ffpoly_copy(FFPOLY *dest,FFPOLY *src)
	{
	if(!dest || !src) return 0;

	//memcpy(dest->pv,src->pv,SCALED_WORD_8VEC);
	//dest->pvdeg = src->pvdeg;

	memcpy(dest,src,sizeof(FFPOLY));
	return 1;
	}

int ffpoly_copy_n(FFPOLY *dest,FFPOLY *src,int n)
	{
	if(!dest || !src || n<0) return 0;

	memmove(dest,src,n * sizeof(FFPOLY));
	return 1;
	}

/* Set P(x) = 0 */
int ffpoly_zeroset(FFPOLY *p)
	{
	if(!p) return 0;

	memset(p->pv,0x00,SCALED_WORD_8VEC);
	p->pvdeg=-1;

	return 1;
	}

int ffpoly_zeroset_n(FFPOLY *p,int n)
	{
	int i;
	if(!p || n<0) return 0;

	memset(p->pv,0x00,n * sizeof(FFPOLY));
	
	for(i=0;i<n;i++)
		p[i].pvdeg=-1;

	return 1;
	}

/* Set P(x) = 1 */
int ffpoly_identity_set(FFPOLY *p)
	{
	if(!p) return 0;

	memset(p->pv,0x00,SCALED_WORD_8VEC);;
	
	p->pv[0]++;
	p->pvdeg = 0;
	
	return 1;
	}

/* Set the coefficient a_i=1 in P(x) = a_n x^n + ... + a_1 x + 0,
   but do not change the other coefficients. Repeated calls can
   be used to set all the coefficients. */
int ffpoly_setterm(FFPOLY *p,int i)
	{
	if(!p) return 0;
	if(i<0 || i>=MAXDEGREE) return 0;

	p->pv[i >> PWORDBITS] |= (1 << (i & (PWORDSIZE - 1)));
	
	p->pvdeg = MAX(p->pvdeg,i);
	return 1;
	}

/* Get a_i from poly P(x) */
int ffpoly_getterm(FFPOLY *p,int i)
	{
	if(!p) return -1;
	if(i<0 || i>=MAXDEGREE) return -1;

	return (p->pv[i >> PWORDBITS] & (1 << (i & (PWORDSIZE - 1)))) != 0 ? 1 : 0;
	}

/* Set P(x) = x */
int ffpoly_set_x(FFPOLY *p)
	{
	if(!p) return 0;

	memset(p->pv,0x00,SCALED_WORD_8VEC);

	p->pv[0] = 0x02;
	p->pvdeg = 1;

	return 1;
	}

/* Set P(x) = x+1 */
int ffpoly_set_x_p_1(FFPOLY *p)
	{
	if(!p) return 0;

	memset(p->pv,0x00,SCALED_WORD_8VEC);
	
	p->pv[0] = 0x03;
	p->pvdeg = 1;

	return 1;
	}

/* Set P(x) = x^e_1 + x^e_2 + x^e_3 + ..., 
   where e_1 > e_2 > e_3 > ... e_v.
   v is degsize and e_1...e_v are the elements of deg. */
int ffpoly_set(FFPOLY *p,const int *deg,int degsize)
	{
	int cdeg = MAXDEGREE;
	int i;

	if(!p || !deg) return 0;

	memset(p->pv,0x00,SCALED_WORD_8VEC);
	p->pvdeg = -1;

	for(i=0;i<degsize;i++)
		{
		if(deg[i] < 0 || deg[i] >= cdeg) return 0;
		cdeg = deg[i];

		if(i==0) p->pvdeg = cdeg;

		p->pv[cdeg >> PWORDBITS] |= (1 << (cdeg & (PWORDSIZE-1) ) );
		}

	return 1;
	}

/* Computes RS(x) = P1(x) + P2(x) mod MODULUS(x), if modulus==NULL then
   no reduction is applied. */
int ffpoly_addmod(FFPOLY *rs,FFPOLY *p1,FFPOLY *p2,FFPOLY *modulus)
	{
	int mxdeg;
	int cn;
	int i;

	if(!p1 || !p2 || !rs) return 0;
	
	mxdeg = MAX(p1->pvdeg,p2->pvdeg);
	
	if(mxdeg >=0) cn = (mxdeg >> PWORDBITS);
	else cn=-1;

	if(mxdeg < 128)
		{
		rs->pv[0] = p1->pv[0] ^ p2->pv[0];
		rs->pv[1] = p1->pv[1] ^ p2->pv[1];
		rs->pv[2] = p1->pv[2] ^ p2->pv[2];
		rs->pv[3] = p1->pv[3] ^ p2->pv[3];
		rs->pv[4] = 0;
		rs->pv[5] = 0;
		rs->pv[6] = 0;
		rs->pv[7] = 0;
		}
	else
		{
		for(i = 0; i <= cn ; i++)
			rs->pv[i] = p1->pv[i] ^ p2->pv[i];

		for(i = cn+1; i < SCALED_DEGREE; i++)
			rs->pv[i] = 0;
		}

	if( ffpoly_fast_update_degree(rs,mxdeg) == 0 ) return 0;
	if( modulus && ffpoly_reduce(rs,modulus) == 0 ) return 0;

	return 1;
	}

/* Computes RS(x) = P1(x) * P2(x) mod MODULUS(x), if modulus==NULL,
   no reduction is applied */
int ffpoly_mulmod(FFPOLY *rs,FFPOLY *p1,FFPOLY *p2,FFPOLY *modulus)
	{
	FFPOLY pr;
	int i;

	if(!p1 || !p2 || !rs) return 0;

	if(p1->pvdeg < 0 || p2->pvdeg < 0)
		{
		ffpoly_zeroset(rs);
		return 1;
		}
	
	/* If we are under certain conditions... use plain multiplication... */
	pr.pvdeg = p1->pvdeg + p2->pvdeg;

	if(pr.pvdeg >= 256) printf("Error...\n\n");
	if(pr.pvdeg < 10 || pr.pvdeg > 256 || p1->pvdeg>=128 || p2->pvdeg>=128 || SCALED_DEGREE < 8)
		return ffpoly_plainmul(rs,p1,p2,modulus);

	/* Otherwise switch to Karatsuba's multiplication algorithm... */
	if(rs != p1 && rs != p2)
        {
		FFPOLY ts;
		
        ts.pvdeg = rs->pvdeg = p1->pvdeg + p2->pvdeg;

#ifndef KARATSUBA_UNROLL
        karatsuba_mul_recursive(rs->pv,p1->pv,p2->pv,4);
#else
		karatsuba_unroll(rs->pv,p1->pv,p2->pv);
#endif
		/*
		if(ffpoly_cmp(&ts,rs) != 0)
			{
			printf("Error !!!\n");
			exit(0);
			}
		*/
	    for(i=8;i<SCALED_DEGREE;i++)
		    pr.pv[i] = 0;
		
		if(modulus) ffpoly_reduce(rs,modulus);	
        }
    else
        {
#ifndef KARATSUBA_UNROLL
        karatsuba_mul_recursive(pr.pv,p1->pv,p2->pv,4);
#else
		karatsuba_unroll(pr.pv,p1->pv,p2->pv);
#endif

	    for(i=8;i<SCALED_DEGREE;i++)
		    pr.pv[i] = 0;

        if(modulus) ffpoly_reduce(&pr,modulus);	
	    ffpoly_copy(rs,&pr);
        }

	return 1;
	}



/* Computes RS(x) = RS(x) mod MODULUS(x) */
int ffpoly_reduce(FFPOLY *rs,FFPOLY *modulus)
	{
	int p,d,s;
	int diff;
	int i;

	if(!rs || !modulus) return 0;

	if(rs->pvdeg < modulus->pvdeg || modulus->pvdeg==0) 
		return 1;	/* No need to reduce ! */

	if(rs->pvdeg <= 0) return 0;

	/* Treat apart the special case x^103 + x^9 + 1 used in Quartz */
	if(modulus->pvdeg == 103 && modulus->pv[0] == 513 && 
	   modulus->pv[1] == 0 && modulus->pv[2] == 0 && modulus->pv[3] == 128)
		{
		/* The inner cycle is executed aproximatelly 98 times per call */
#ifndef FASTMOD103
		
		WORD32 l2 = rs->pvdeg - (103 - 9);
		WORD32 l3 = rs->pvdeg - 103;

		while(rs->pvdeg >= 103)
			{
			WORD32 mask = (1 << (rs->pvdeg & (PWORDSIZE-1)));
			WORD32 pos = rs->pvdeg >> PWORDBITS;

			if((rs->pv[pos] & mask) != 0)
				{
				WORD32 m2,p2;
				WORD32 m3,p3;

				rs->pv[pos] ^= mask;
				
				p2 = (l2 >> PWORDBITS);
				p3 = (l3 >> PWORDBITS);

				m2 = 1 << (l2 & (PWORDSIZE-1));
				m3 = 1 << (l3 & (PWORDSIZE-1));
				
				rs->pv[p2] ^= m2;
				rs->pv[p3] ^= m3;
				}

			rs->pvdeg--;
			l2--;
			l3--;
			}
#else
		if(rs->pvdeg < 206)	
			{
			fastest_reduction_mod_103u(rs);
			}
		else 
			{
			fastest_reduction_mod_103(rs);
			}
#endif
		//if( ffpoly_fast_update_degree(rs,102) == 0 ) return 0;
		return 1;
		}

	do
		{
		diff = rs->pvdeg - modulus->pvdeg;
		s    = rs->pvdeg >> PWORDBITS;
		p    = diff >> PWORDBITS;
		d    = diff & (PWORDSIZE - 1);

		if(d == 0)
			{
			for(i=s;i>=p;i--)
				rs->pv[i] ^= modulus->pv[i-p];
			}
		else
			{
			for(i=s;i>=p;i--)
				{
				rs->pv[i] ^= (modulus->pv[i-p]<<d);
				if(i>p) rs->pv[i] ^= (modulus->pv[i-p-1]>>(PWORDSIZE-d));
				}
			}

		if( ffpoly_fast_update_degree(rs,rs->pvdeg-1) == 0 ) return 0;
		} while( rs->pvdeg >= modulus->pvdeg );

	return 1;
	}

/* Compute QS(x) and RS(x) such that: G(x) = QS(x)*DIVISOR(x) + RS(x), with
   0 <= deg(RS(x)) < deg(DIVISOR(x)) and 0 <= deg(QS(x)) < deg(G(x)) */
int ffpoly_divide(FFPOLY *qs,FFPOLY *rs,FFPOLY *g, FFPOLY *divisor)
	{
	int p,d,s;
	int diff;
	int i;

	if(!g || !divisor) return 0;
	if(divisor->pvdeg < 0) return 0;

	ffpoly_zeroset(qs);
	ffpoly_copy(rs,g);

	if(rs->pvdeg < divisor->pvdeg) 
		return 1;	/* No need to reduce ! */

	if(divisor->pvdeg==0)	// I'm dividing by 1
		{
		ffpoly_copy(qs,g);
		ffpoly_zeroset(rs);

		return 1;
		}

	do
		{
		diff = rs->pvdeg - divisor->pvdeg;
		s    = rs->pvdeg >> PWORDBITS;
		p    = diff >> PWORDBITS;
		d    = diff & (PWORDSIZE - 1);

		qs->pv[diff >> PWORDBITS] |= (1 << (diff & (PWORDSIZE - 1)));
		if(qs->pvdeg < diff) qs->pvdeg = diff;

		if(d == 0)
			{
			for(i=s;i>=p;i--)
				rs->pv[i] ^= divisor->pv[i-p];
			}
		else
			{
			for(i=s;i>=p;i--)
				{
				rs->pv[i] ^= (divisor->pv[i-p]<<d);
				if(i>p) rs->pv[i] ^= (divisor->pv[i-p-1]>>(PWORDSIZE-d));
				}
			}

		if( ffpoly_fast_update_degree(rs,rs->pvdeg-1) == 0 ) return 0;
		} while( rs->pvdeg >= divisor->pvdeg );

	return 1;
	}

/* Computes QS(x) such that A(x)*QS(x) mod MOD(x) = B(x),
   clearly 0<=deg(QS(x))<deg(MOD(x)) */
int ffpoly_field_div(FFPOLY *qs,FFPOLY *a,FFPOLY *b,FFPOLY *mod)
	{
	FFPOLY div_a,div_b,div_v;
	int da,db;
	//int i;

	if(!qs || !a || !b || !mod) return 0;

	ffpoly_copy(&div_a,b);
	ffpoly_copy(qs,a);
	ffpoly_copy(&div_b,mod);
	ffpoly_zeroset(&div_v);

	da = div_a.pvdeg;
	db = div_b.pvdeg;

	while(da > 0 && db > 0)
		{
		if(ffpoly_is_even(&div_a))
			{
			ffpoly_shiftright(&div_a,&div_a,1);
			da--;

			if(ffpoly_is_even(qs) == 0) ffpoly_addmod(qs,qs,mod,NULL);
			ffpoly_shiftright(qs,qs,1);
			}
		else
			{
			if(ffpoly_is_even(&div_b))
				{
				ffpoly_shiftright(&div_b,&div_b,1);
				db--;

				if(ffpoly_is_even(&div_v)==0) ffpoly_addmod(&div_v,&div_v,mod,NULL);
				ffpoly_shiftright(&div_v,&div_v,1);
				}
			else
				{
				if(da > db)
					{
					ffpoly_addmod(qs,qs,&div_v,mod);
					if(ffpoly_is_even(qs) == 0) ffpoly_addmod(qs,qs,mod,NULL);
					ffpoly_shiftright(qs,qs,1);

					ffpoly_addmod(&div_a,&div_a,&div_b,NULL);
					ffpoly_shiftright(&div_a,&div_a,1);

					while(ffpoly_getterm(&div_a,da) == 0) 
						da--;
					}
				else
					{
					ffpoly_addmod(&div_v,&div_v,qs,NULL);
					if(ffpoly_is_even(&div_v) == 0) ffpoly_addmod(&div_v,&div_v,mod,NULL);

					ffpoly_shiftright(&div_v,&div_v,1);

					ffpoly_addmod(&div_b,&div_b,&div_a,NULL);
					ffpoly_shiftright(&div_b,&div_b,1);

					while(ffpoly_getterm(&div_b,db) == 0)
						db--;
					}
				}
	
			}


		}

	if(db == 0) ffpoly_copy(qs,&div_v);

	//for(i=(qs->pvdeg >> PWORDBITS)+1;i<SCALED_DEGREE;i++)
	//	qs->pv[i] = 0;

	return 1;	
	}

/* Shift left coefficients of P(x). This is equivalent to
   RS(x) = P(x) * x^n, up to the maximal allowed degree */
int ffpoly_shiftleft(FFPOLY *rs,FFPOLY *p,int n)
	{
	FFPOLY rw;
	int start;
	int mxid;
	int ofs;
	int seg;
	int i;
	int a,b;

	if(!p || n<0) return 0;

	if(n==0) 
		{
		if(p!=rs) ffpoly_copy(rs,p);
		return 1;
		}

	if(p->pvdeg < 0 || n>=MAXDEGREE)
		{
		ffpoly_zeroset(rs);
		return 1;
		}

	ffpoly_zeroset(&rw);

	mxid = p->pvdeg >> PWORDBITS;
	ofs  = (n & (PWORDSIZE-1));

	if(ofs != 0)
		{
		seg  = (n >> PWORDBITS) + 1;

		a = mxid + seg;
		b = SCALED_DEGREE - 1;

		start = MIN( a , b );

		rw.pv[start] |= (p->pv[start-seg+1] << ofs);

		for(i=start;i>=seg;i--)
			{
			rw.pv[i  ] |= (p->pv[i-seg] >> (PWORDSIZE - ofs));
			rw.pv[i-1] |= (p->pv[i-seg] << ofs);
			}
		}
	else
		{
		seg = (n >> PWORDBITS);

		a = mxid + seg;
		b = SCALED_DEGREE - 1;

		start = MIN( a, b );

		for(i=start; i>=seg; i--)
			rw.pv[i] = p->pv[i-seg];
		}

	
	rw.pvdeg = p->pvdeg + n;

	if(rw.pvdeg >= MAXDEGREE) 
		ffpoly_update_degree(&rw);
	
	ffpoly_copy(rs,&rw);

	return 1;
	}

/* Same as above but shifts right. This is equivalent to
   dividing by x^n and discarding the remainder */
int ffpoly_shiftright(FFPOLY *rs,FFPOLY *p,int n)
	{
	FFPOLY rw;
	int end;
	int mxid;
	int ofs;
	int seg;
	int i;
	int a,b;

	if(!p || n<0) return 0;

	if(n==0) 
		{
		if(p!=rs) ffpoly_copy(rs,p);
		return 1;
		}

	if(p->pvdeg < 0 || n>=MAXDEGREE)
		{
		ffpoly_zeroset(rs);
		return 1;
		}

	ffpoly_zeroset(&rw);

	mxid = p->pvdeg >> PWORDBITS;
	ofs  = (n & (PWORDSIZE-1));

	if(ofs != 0)
		{
		seg  = ((n+PWORDSIZE-1) >> PWORDBITS)-1;

		a = SCALED_DEGREE-1;
		b = mxid + seg;

		end = MIN( a , b );

		i=0;
		while(i+seg+1 <= end)
			{
			rw.pv[i] |= (p->pv[i+seg] >> ofs);
			rw.pv[i] |= (p->pv[i+seg+1] << (PWORDSIZE - ofs));

			i++;
			}

		rw.pv[i] |= (p->pv[i+seg] >> ofs);
		}
	else
		{
		seg = (n >> PWORDBITS);

		for(i=0;i<=mxid-seg;i++)
			rw.pv[i] = p->pv[i+seg];
		}

	a = p->pvdeg - n;

	rw.pvdeg = MAX( a , 0 );

	ffpoly_copy(rs,&rw);

	return 1;
	}

/* Computes RS(x) = P(x)^2 mod MODULUS(x). If modulus==NULL, then
   no reduction is applied. */
int ffpoly_square(FFPOLY *rs,FFPOLY *p,FFPOLY *modulus)
	{
	//FFPOLY ws;
    WORD_8 *ptr;
    WORD16 qtr[16];
	//int pt=0;
	//int i;
	//int expdeg;
    //int j;

    if(p->pvdeg >= 128) return 0;
    else if(p->pvdeg <= 0) 
	{
	if(rs != p) ffpoly_copy(rs,p);
	return 1;
	}

    ptr = (WORD_8 *)p->pv;
    //qtr = (WORD16 *)&rs->pv[0];

    qtr[15] = FSQR[ptr[15]];
    qtr[14] = FSQR[ptr[14]];
    qtr[13] = FSQR[ptr[13]];
    qtr[12] = FSQR[ptr[12]];
    qtr[11] = FSQR[ptr[11]];
    qtr[10] = FSQR[ptr[10]];
    qtr[ 9] = FSQR[ptr[ 9]];
    qtr[ 8] = FSQR[ptr[ 8]];
    qtr[ 7] = FSQR[ptr[ 7]];
    qtr[ 6] = FSQR[ptr[ 6]];
    qtr[ 5] = FSQR[ptr[ 5]];
    qtr[ 4] = FSQR[ptr[ 4]];
    qtr[ 3] = FSQR[ptr[ 3]];
    qtr[ 2] = FSQR[ptr[ 2]];
    qtr[ 1] = FSQR[ptr[ 1]];
    qtr[ 0] = FSQR[ptr[ 0]];

    memcpy(rs->pv,qtr,sizeof(rs->pv));

    rs->pvdeg = 2 * p->pvdeg;

    if( modulus && ffpoly_reduce(rs,modulus) == 0 ) return 0;
	return 1;
	}

/* Computes RS(x) = P(x)^n mod MODULUS(x). If modulus==NULL, then
   no reduction is applied. */

int ffpoly_powmod(FFPOLY *rs,FFPOLY *p,int n,FFPOLY *modulus)
	{
	FFPOLY tmp,acc;
	int i;

	ffpoly_identity_set(&tmp);
	ffpoly_copy(&acc,p);

	for(i=n;i>0;i>>=1)
		{
		if(i & 0x01)
			{
			if( ffpoly_mulmod(&tmp,&tmp,&acc,modulus) == 0 ) return 0;
			}

		if( ffpoly_square(&acc,&acc,modulus) == 0) return 0;
		}

	ffpoly_copy(rs,&tmp);

	return 1;
	}

/* Computes the inverse of x modulo an irreducible polynomial mod */
int ffpoly_inverse(FFPOLY *inv,FFPOLY *x,FFPOLY *mod)
	{
	FFPOLY id;

	ffpoly_identity_set(&id);
	if(ffpoly_field_div(inv,&id,x,mod) != 1) return 0;

	return 1;
	}


/* Computes the greatest common divisor of A(x) and B(x).
   By definition RS(x) is the poly of maximal degree such
   that RS(x) divides A(x) and RS(x) divides B(x) with no
   remainder (i.e. A(x) = RS(x) * Q1(x) and B(x) = RS(x) * Q2(x),
   for some Q1(x) and Q2(x).
 */
int ffpoly_gcd(FFPOLY *rs,FFPOLY *a,FFPOLY *b)
	{
	FFPOLY f,q,g,r;
	FFPOLY *fp,*gp,*rp,*tp;
	
	if(!rs || !a || !b) return 0;

	if(a->pvdeg > b->pvdeg) 
		{
		ffpoly_copy(&f,a);
		ffpoly_copy(&g,b);
		}
	else
		{
		ffpoly_copy(&f,b);
		ffpoly_copy(&g,a);
		}

	fp = &f;
	gp = &g;
	rp = &r;

	while( 1 )
		{
		ffpoly_divide(&q,rp,fp,gp);

		if(rp->pvdeg >= gp->pvdeg) 
			return 0;

		if(rp->pvdeg < 0) break;	/* g divide f senza resto */

		//ffpoly_copy(&f,&g);
		//ffpoly_copy(&g,&r);

		tp = fp;
		fp = gp;
		gp = rp;
		rp = tp;
		} 

	ffpoly_copy(rs,gp);
	
	return 1;
	}

/* Computes RS(x) = F(X(x)) mod MODULUS(x) */
int ffpoly_subst(FFPOLY *rs,FFPOLY *f,FFPOLY *x,FFPOLY *mod)
	{
	FFPOLY pr,pq;
	int mxdeg;
	int i,j;

	if(!rs || !f || !x || !mod) return 0;

	if(f->pvdeg <= 0) 
		{
		ffpoly_copy(rs,f);
		return 1;
		}

	ffpoly_zeroset(&pr);

	mxdeg = f->pvdeg >> PWORDBITS;

	for(j = 0;j <= mxdeg; j++)
		{
		int idx = j << PWORDBITS;

		if(f->pv[j] == 0) continue;

		for(i = 0;i < PWORDSIZE && i+idx <= f->pvdeg; i++)
			{
			/* Just avoid to multiply by 0 */
			if( ((f->pv[j] >> i) & 0x01 ) == 0 ) continue;	
		
			if( ffpoly_powmod(&pq,x,i+idx,mod) == 0 ) return 0;
			if( ffpoly_addmod(&pr,&pq,&pr,mod) == 0 ) return 0;
			}
		}
	
	ffpoly_copy(rs,&pr);
	return 1;
	}

static int ffpoly_irred_test_div(FFPOLY *fp,int n)
	{
	FFPOLY xx,x;
	FFPOLY rs;
	int i;
	
	if(!fp) return -1;
	if(n >= MAXDEGREE) return -2;

	ffpoly_set_x(&xx);
	ffpoly_set_x(&x);

	for(i=0;i<n;i++)
		ffpoly_square(&xx,&xx,fp);

	ffpoly_addmod(&xx,&xx,&x,fp);
	ffpoly_gcd(&rs,&xx,fp);

	if(rs.pvdeg > 0) return 0;	// Reducible
	return 1;					// Non defined yet !
	}

int ffpoly_test_irreducibility(FFPOLY *fp)
	{
	FFPOLY x,xx;
	int n,i;
	
	if(!fp) return -1;

	n = fp->pvdeg;
	if(n >= MAXDEGREE/2) return -2;

	ffpoly_set_x(&xx);
	ffpoly_set_x(&x);

	for(i=0;i<n;i++)
		ffpoly_square(&xx,&xx,fp);

	// reducible if x^q^n != x mod fp
	if(ffpoly_cmp(&xx,&x) != 0) return 0;	

	i=0;

	while( i < NTH_PRIME_TABLE_SIZE && nth_prime[i] < fp->pvdeg )
		{
		if( fp->pvdeg % nth_prime[i] == 0 )
			{
			if(ffpoly_irred_test_div(fp,fp->pvdeg / nth_prime[i]) == 0) 
				return 0;
			}

		i++;
		}

	return 1;
	}
















