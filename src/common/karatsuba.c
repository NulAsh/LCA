
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#define KARATSUBA_FAST_CALLS
#define USE_TABLE_LOOKUPS

#define USE_SHOUPS_SHORTCUT
#define USE_INLINING

#include <stdio.h>
#include <memory.h>
#include <malloc.h>

#include "karatsuba.h"
#include "osdepn.h"


WORD32 *K8X8 = NULL;

WORD32 karatsuba_mul_plain(WORD32 id1,WORD32 id2)
	{
	WORD32 mul1 = id1;
	WORD32 res = 0;

	if(mul1!=0)
		{
		while(id2 != 0)
			{
			res ^= (id2 & 0x01 ? mul1 : 0);

			mul1<<=1;
			id2>>=1;
			}
		}
	
	return res;	
	}

void build_karatsuba_8x8_table()
	{
	int i,j;

	for(i=0;i<256;i++)
		for(j=0;j<256;j++)
			K8X8[(i<<8) | (j)] = karatsuba_mul_plain(i,j);
	}

int init_karatsuba()
	{
	if(!K8X8)   
		{
		K8X8   = (WORD32 *)malloc( 256 * 256 * sizeof(WORD32));
		if(!K8X8) return 0;
		else build_karatsuba_8x8_table();
		}

	return 1;
	}

void free_karatsuba()
	{
	if(K8X8)   free(K8X8);

	K8X8 = NULL;
	}

void karatsuba_mul1(WORD32 *c, WORD32 a, WORD32 b)
	{
	WORD32 hi, lo;
	WORD32 A[4];

	A[0] = 0;
	A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);	/* 0,a_30,a_29,...,a_0 */
	A[2] = a << 1;								/* a_30,a_29,...,a_1,0 */
	A[3] = A[1] ^ A[2];

	lo = A[b>>(PWORDSIZE-2)];
	hi = lo >> (PWORDSIZE-2); 
	lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];

	// The following code is included from mach_desc.h
	// and handles *any* word size

	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>8)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>6)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>4)&3];\
	hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>2)&3];\

	hi = (hi << 2)|(lo >> (PWORDSIZE-2)); 
	lo = (lo << 2) ^ A[b & 3];

	if (a >> (PWORDSIZE-1)) {
		hi = hi ^ (b >> 1);
		lo = lo ^ (b << (PWORDSIZE-1));
	}

	c[0] = lo;
	c[1] = hi;
	}


int karatsuba_mul_recursive(WORD32 *vz,WORD32 *vx,WORD32 *vy,int size)
	{
	if(size == 1)
		{
#if 0
		WORD32 w1,w0,w10;
		WORD32 wall;
		WORD32 x1,y1,x0,y0;
		WORD32 x10,y10;

		x1 = (WORD16)(vx[0] >> 16);
		x0 = (WORD16)(vx[0]);
		y1 = (WORD16)(vy[0] >> 16);
		y0 = (WORD16)(vy[0]);

		x10 = x1^x0;
		y10 = y1^y0;

#ifdef USE_TABLE_LOOKUPS
		KARATSUBA_MUL_16X16(w0,x0,y0);
		KARATSUBA_MUL_16X16(w1,x1,y1);
		KARATSUBA_MUL_16X16(w10,x10,y10);
#else
		KARATSUBA_MUL_PLAIN_16_x_3(w0,w1,w10,x0,y0,x1,y1,x10,y10);
#endif
		wall = w1 ^ w0 ^ w10;

		vz[0] = w0 ^ (wall << 16);
		vz[1] = w1 ^ (wall >> 16);
#else
		karatsuba_mul1(vz,vx[0],vy[0]);
#endif
		}
	else
		{
		WORD32 w1[4];
		WORD32 w0[4];
		WORD32 w10[4];
		WORD32 wall[4];
		WORD32 x10[2],y10[2];
		int szm = size >> 1;
		int i,j,k;
	
		for(i=0,j=szm;i<szm;i++,j++)
			{
			x10[i] = vx[j] ^ vx[i];
			y10[i] = vy[j] ^ vy[i];
			}

		karatsuba_mul_recursive(w0,vx,vy,szm);
		karatsuba_mul_recursive(w1,vx + szm,vy + szm,szm);
		karatsuba_mul_recursive(w10,x10,y10,szm);
	
		for(i=0;i<size;i++)
			{
			vz[i]      = w0[i];
			vz[i+size] = w1[i];
			wall[i]    = w0[i] ^ w1[i] ^ w10[i];
			}

		for(i=0,j=szm,k=size;i<szm;i++,j++,k++)
			{
			vz[k] ^= wall[j];
			vz[j] ^= wall[i];
			}
		}

	return 1;
	}

static void karatsuba_fold(FFPOLY *T, FFPOLY *b, int sb, int hsa)
	{
	int m = sb - hsa;
	int i;

	for (i = 0; i < m; i++)
		ffpoly_addmod(&T[i],&b[i],&b[hsa+i],NULL);

    ffpoly_copy_n(&T[m],&b[m],hsa-m);
	}

static void karatsuba_add(FFPOLY *T, FFPOLY *b, int sb)
	{
	int i;

	for (i = 0; i < sb; i++)
		ffpoly_addmod(&T[i],&T[i],&b[i],NULL);
	}

static void karatsuba_fix(FFPOLY *c, FFPOLY *b, int sb, int hsa)
	{
	int i;

    ffpoly_copy_n(c,b,hsa);

	for (i = hsa; i < sb; i++)
		ffpoly_addmod(&c[i],&c[i],&b[i],NULL);
	}

static void karatsuba_monoterm_mul(FFPOLY *z,FFPOLY *x,FFPOLY *y0,int size,FFPOLY *cmod)
	{
	int i;

	for(i=0;i<size;i++)
        {
#ifdef KARATSUBA_FAST_CALLS
		z[i].pvdeg = x[i].pvdeg + y0->pvdeg;
        karatsuba_mul_recursive(z[i].pv,x[i].pv,y0->pv,4);
        ffpoly_reduce(&z[i],cmod);
#else
		ffpoly_mulmod(&z[i],&x[i],y0,cmod);
#endif
        }
	}


#define UNROLL(z,x,y,m) { \
	z->pvdeg = x->pvdeg + y->pvdeg; \
	karatsuba_unroll(z->pv,x->pv,y->pv); \
	ffpoly_reduce(z,m); \
	} while(0)

void expoly_recursive_karatsuba_mul(FFPOLY *c,FFPOLY *a,int sa,FFPOLY *b,int sb,FFPOLY *stk,FFPOLY *cmod)
	{
	int hsa;

	if (sa < sb) 
		{
        FFPOLY *tm;
        int t;
		
		t = sa; 
		sa = sb; 
		sb = t;

		tm = a;
		a = b; 
		b = tm;
		}

	if(sb == 1) 
		{  
		if(sa == 1) 
            {
#ifdef KARATSUBA_FAST_CALLS
            c->pvdeg = a->pvdeg + b->pvdeg;

			//karatsuba_mul_recursive(c->pv,a->pv,b->pv,4);
			karatsuba_unroll(c->pv,a->pv,b->pv);
            ffpoly_reduce(c,cmod);
			//UNROLL(c,a,b,cmod);
#else
			ffpoly_mulmod(c,a,b,cmod);
#endif
            }

		else karatsuba_monoterm_mul(c,a,b,sa,cmod); 
	
		return;
		}

	else if(sb == 2 && sa == 2) 
		{
		ffpoly_mulmod(&c[0],&a[0],&b[0],cmod);
		ffpoly_mulmod(&c[2],&a[1],&b[1],cmod);

		ffpoly_addmod(&stk[0],&a[0],&a[1],NULL);
		ffpoly_addmod(&stk[1],&b[0],&b[1],NULL);

		ffpoly_mulmod(&c[1],&stk[0],&stk[1],cmod);

		ffpoly_addmod(&c[1],&c[1],&c[0],NULL);
		ffpoly_addmod(&c[1],&c[1],&c[2],NULL);
		return;
		}

   hsa = (sa + 1) >> 1;

   if(hsa < sb) 
		{
		/* normal case */

		int hsa2 = hsa << 1;

		FFPOLY *T1, *T2, *T3;

		T1 = stk; stk += hsa;
		T2 = stk; stk += hsa;
		T3 = stk; stk += hsa2 - 1;

		/* compute T1 = a_lo + a_hi */

		karatsuba_fold(T1, a, sa, hsa);

		/* compute T2 = b_lo + b_hi */

		karatsuba_fold(T2, b, sb, hsa);

		/* recursively compute T3 = T1 * T2 */

		expoly_recursive_karatsuba_mul(T3, T1, hsa, T2, hsa, stk, cmod);

		/* recursively compute a_hi * b_hi into high part of c */
		/* and subtract from T3 */

		expoly_recursive_karatsuba_mul(c + hsa2, a+hsa, sa-hsa, b+hsa, sb-hsa, stk, cmod);
		karatsuba_add(T3, c + hsa2, sa + sb - hsa2 - 1);

		/* recursively compute a_lo*b_lo into low part of c */
		/* and subtract from T3 */

		expoly_recursive_karatsuba_mul(c, a, hsa, b, hsa, stk, cmod);
		karatsuba_add(T3, c, hsa2 - 1);

		ffpoly_zeroset(&c[hsa2-1]);

		/* finally, add T3 * X^{hsa} to c */

		karatsuba_add(c+hsa,T3,hsa2-1);
		}
	else 
		{
		/* degenerate case */
		FFPOLY *T;

		T = stk; stk += hsa + sb - 1;

		/* recursively compute b*a_hi into high part of c */

		expoly_recursive_karatsuba_mul(c + hsa, a + hsa, sa - hsa, b, sb, stk, cmod);

		/* recursively compute b*a_lo into T */

		expoly_recursive_karatsuba_mul(T, a, hsa, b, sb, stk, cmod);
		karatsuba_fix(c, T, hsa + sb - 1, hsa);
		}
	}

void karatsuba_unroll(WORD32 *vz,WORD32 *vx,WORD32 *vy)
	{
#ifdef USE_SHOUPS_SHORTCUT
	/* Karatsuba step 4: vz,vx,vy (0,0) */
	do	{
		WORD32 w1_rc4[4];
		WORD32 w0_rc4[4];
		WORD32 w10_rc4[4];
		WORD32 wall_rc4[4];
		WORD32 x10_rc4[2],y10_rc4[2];
		
		x10_rc4[0] = vx[0] ^ vx[2];
		y10_rc4[0] = vy[0] ^ vy[2];
		
		x10_rc4[1] = vx[1] ^ vx[3];
		y10_rc4[1] = vy[1] ^ vy[3];
		
		/* Karatsuba step 2: w0_rc4,vx,vy (0,0) */
		do	{
			WORD32 w1_rc2[2];
			WORD32 w0_rc2[2];
			WORD32 w10_rc2[2];
			WORD32 wall_rc2[2];
			WORD32 x10_rc2[1],y10_rc2[1];
			
			x10_rc2[0] = vx[0] ^ vx[1];
			y10_rc2[0] = vy[0] ^ vy[1];
			
			/* Karatsuba step 1: w0_rc2,vx,vy (0,0) */
			if(vx[0]!=0 && vy[0]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w0_rc2[0],vx[0],vy[0]);
#else
				WORD32 a=vx[0],b=vy[0];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w0_rc2[0] = lo;
				w0_rc2[1] = hi;
#endif
				}
			else  /* vx[0]==0 or vy[0]==0 */
				{
				w0_rc2[0] = 0;
				w0_rc2[1] = 0;
				} /* Collecting results back in w0_rc2 */

			/* Karatsuba step 1: w1_rc2,vx,vy (1,1) */
			if(vx[1]!=0 && vy[1]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w1_rc2[0],vx[1],vy[1]);
#else
				WORD32 a=vx[1],b=vy[1];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w1_rc2[0] = lo;
				w1_rc2[1] = hi;
#endif
				}
			else  /* vx[1]==0 or vy[1]==0 */
				{
				w1_rc2[0] = 0;
				w1_rc2[1] = 0;
				} /* Collecting results back in w1_rc2 */

			/* Karatsuba step 1: w10_rc2,x10_rc2,y10_rc2 (0,0) */
			if(x10_rc2[0]!=0 && y10_rc2[0]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w10_rc2[0],x10_rc2[0],y10_rc2[0]);
#else
				WORD32 a=x10_rc2[0],b=y10_rc2[0];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w10_rc2[0] = lo;
				w10_rc2[1] = hi;
#endif
				}
			else  /* x10_rc2[0]==0 or y10_rc2[0]==0 */
				{
				w10_rc2[0] = 0;
				w10_rc2[1] = 0;
				} /* Collecting results back in w10_rc2 */

			
			w0_rc4[0]   = w0_rc2[0];
			w0_rc4[2]   = w1_rc2[0];
			wall_rc2[0] = w0_rc2[0] ^ w1_rc2[0] ^ w10_rc2[0];
			
			w0_rc4[1]   = w0_rc2[1];
			w0_rc4[3]   = w1_rc2[1];
			wall_rc2[1] = w0_rc2[1] ^ w1_rc2[1] ^ w10_rc2[1];
			
			
			w0_rc4[2]   ^= wall_rc2[1];
			w0_rc4[1]   ^= wall_rc2[0];
			
			} while(0);  /* Collecting results back in w0_rc4 */

		/* Karatsuba step 2: w1_rc4,vx,vy (2,2) */
		do	{
			WORD32 w1_rc2[2];
			WORD32 w0_rc2[2];
			WORD32 w10_rc2[2];
			WORD32 wall_rc2[2];
			WORD32 x10_rc2[1],y10_rc2[1];
			
			x10_rc2[0] = vx[2] ^ vx[3];
			y10_rc2[0] = vy[2] ^ vy[3];
			
			/* Karatsuba step 1: w0_rc2,vx,vy (2,2) */
			if(vx[2]!=0 && vy[2]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w0_rc2[0],vx[2],vy[2]);
#else
				WORD32 a=vx[2],b=vy[2];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w0_rc2[0] = lo;
				w0_rc2[1] = hi;
#endif
				}
			else  /* vx[2]==0 or vy[2]==0 */
				{
				w0_rc2[0] = 0;
				w0_rc2[1] = 0;
				} /* Collecting results back in w0_rc2 */

			/* Karatsuba step 1: w1_rc2,vx,vy (3,3) */
			if(vx[3]!=0 && vy[3]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w1_rc2[0],vx[3],vy[3]);
#else
				WORD32 a=vx[3],b=vy[3];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w1_rc2[0] = lo;
				w1_rc2[1] = hi;
#endif
				}
			else  /* vx[3]==0 or vy[3]==0 */
				{
				w1_rc2[0] = 0;
				w1_rc2[1] = 0;
				} /* Collecting results back in w1_rc2 */

			/* Karatsuba step 1: w10_rc2,x10_rc2,y10_rc2 (0,0) */
			if(x10_rc2[0]!=0 && y10_rc2[0]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w10_rc2[0],x10_rc2[0],y10_rc2[0]);
#else
				WORD32 a=x10_rc2[0],b=y10_rc2[0];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w10_rc2[0] = lo;
				w10_rc2[1] = hi;
#endif
				}
			else  /* x10_rc2[0]==0 or y10_rc2[0]==0 */
				{
				w10_rc2[0] = 0;
				w10_rc2[1] = 0;
				} /* Collecting results back in w10_rc2 */

			
			w1_rc4[0]   = w0_rc2[0];
			w1_rc4[2]   = w1_rc2[0];
			wall_rc2[0] = w0_rc2[0] ^ w1_rc2[0] ^ w10_rc2[0];
			
			w1_rc4[1]   = w0_rc2[1];
			w1_rc4[3]   = w1_rc2[1];
			wall_rc2[1] = w0_rc2[1] ^ w1_rc2[1] ^ w10_rc2[1];
			
			
			w1_rc4[2]   ^= wall_rc2[1];
			w1_rc4[1]   ^= wall_rc2[0];
			
			} while(0);  /* Collecting results back in w1_rc4 */

		/* Karatsuba step 2: w10_rc4,x10_rc4,y10_rc4 (0,0) */
		do	{
			WORD32 w1_rc2[2];
			WORD32 w0_rc2[2];
			WORD32 w10_rc2[2];
			WORD32 wall_rc2[2];
			WORD32 x10_rc2[1],y10_rc2[1];
			
			x10_rc2[0] = x10_rc4[0] ^ x10_rc4[1];
			y10_rc2[0] = y10_rc4[0] ^ y10_rc4[1];
			
			/* Karatsuba step 1: w0_rc2,x10_rc4,y10_rc4 (0,0) */
			if(x10_rc4[0]!=0 && y10_rc4[0]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w0_rc2[0],x10_rc4[0],y10_rc4[0]);
#else
				WORD32 a=x10_rc4[0],b=y10_rc4[0];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w0_rc2[0] = lo;
				w0_rc2[1] = hi;
#endif
				}
			else  /* x10_rc4[0]==0 or y10_rc4[0]==0 */
				{
				w0_rc2[0] = 0;
				w0_rc2[1] = 0;
				} /* Collecting results back in w0_rc2 */

			/* Karatsuba step 1: w1_rc2,x10_rc4,y10_rc4 (1,1) */
			if(x10_rc4[1]!=0 && y10_rc4[1]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w1_rc2[0],x10_rc4[1],y10_rc4[1]);
#else
				WORD32 a=x10_rc4[1],b=y10_rc4[1];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w1_rc2[0] = lo;
				w1_rc2[1] = hi;
#endif
				}
			else  /* x10_rc4[1]==0 or y10_rc4[1]==0 */
				{
				w1_rc2[0] = 0;
				w1_rc2[1] = 0;
				} /* Collecting results back in w1_rc2 */

			/* Karatsuba step 1: w10_rc2,x10_rc2,y10_rc2 (0,0) */
			if(x10_rc2[0]!=0 && y10_rc2[0]!=0)
				{
#ifdef USE_INLINING
				karatsuba_mul1(&w10_rc2[0],x10_rc2[0],y10_rc2[0]);
#else
				WORD32 a=x10_rc2[0],b=y10_rc2[0];
				WORD32 hi, lo;
				WORD32 A[4];
				
				A[0] = 0;
				A[1] = a & ((1UL << (PWORDSIZE-1))-1UL);
				A[2] = a << 1;
				A[3] = A[1] ^ A[2];
				
				lo = A[b>>(PWORDSIZE-2)];
				hi = lo >> (PWORDSIZE-2);
				lo = (lo << 2) ^ A[(b >> (PWORDSIZE-4)) & 3];
				
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>26)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>24)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>22)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>20)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>18)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>16)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>14)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>12)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>>10)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 8)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 6)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 4)&3];
				hi=(hi<<2)|(lo>>30); lo=(lo<<2)^A[(b>> 2)&3];
				
				hi = (hi << 2)|(lo >> (PWORDSIZE-2));
				lo = (lo << 2) ^ A[b & 3];
				
				if (a >> (PWORDSIZE-1))
					{
					hi = hi ^ (b >> 1);
					lo = lo ^ (b << (PWORDSIZE-1));
					}
				
				w10_rc2[0] = lo;
				w10_rc2[1] = hi;
#endif
				}
			else  /* x10_rc2[0]==0 or y10_rc2[0]==0 */
				{
				w10_rc2[0] = 0;
				w10_rc2[1] = 0;
				} /* Collecting results back in w10_rc2 */

			
			w10_rc4[0]   = w0_rc2[0];
			w10_rc4[2]   = w1_rc2[0];
			wall_rc2[0] = w0_rc2[0] ^ w1_rc2[0] ^ w10_rc2[0];
			
			w10_rc4[1]   = w0_rc2[1];
			w10_rc4[3]   = w1_rc2[1];
			wall_rc2[1] = w0_rc2[1] ^ w1_rc2[1] ^ w10_rc2[1];
			
			
			w10_rc4[2]   ^= wall_rc2[1];
			w10_rc4[1]   ^= wall_rc2[0];
			
			} while(0);  /* Collecting results back in w10_rc4 */

		
		vz[0]   = w0_rc4[0];
		vz[4]   = w1_rc4[0];
		wall_rc4[0] = w0_rc4[0] ^ w1_rc4[0] ^ w10_rc4[0];
		
		vz[1]   = w0_rc4[1];
		vz[5]   = w1_rc4[1];
		wall_rc4[1] = w0_rc4[1] ^ w1_rc4[1] ^ w10_rc4[1];
		
		vz[2]   = w0_rc4[2];
		vz[6]   = w1_rc4[2];
		wall_rc4[2] = w0_rc4[2] ^ w1_rc4[2] ^ w10_rc4[2];
		
		vz[3]   = w0_rc4[3];
		vz[7]   = w1_rc4[3];
		wall_rc4[3] = w0_rc4[3] ^ w1_rc4[3] ^ w10_rc4[3];
		
		
		vz[4]   ^= wall_rc4[2];
		vz[2]   ^= wall_rc4[0];
		
		vz[5]   ^= wall_rc4[3];
		vz[3]   ^= wall_rc4[1];
		
		} while(0);  /* Collecting results back in vz */
#else
	/* Karatsuba step 4: vz,vx,vy (0,0) */
	do	{
		WORD32 w1_rc4[4];
		WORD32 w0_rc4[4];
		WORD32 w10_rc4[4];
		WORD32 wall_rc4[4];
		WORD32 x10_rc4[2],y10_rc4[2];
		
		x10_rc4[0] = vx[0] ^ vx[2];
		y10_rc4[0] = vy[0] ^ vy[2];
		
		x10_rc4[1] = vx[1] ^ vx[3];
		y10_rc4[1] = vy[1] ^ vy[3];
		
		/* Karatsuba step 2: w0_rc4,vx,vy (0,0) */
		do	{
			WORD32 w1_rc2[2];
			WORD32 w0_rc2[2];
			WORD32 w10_rc2[2];
			WORD32 wall_rc2[2];
			WORD32 x10_rc2[1],y10_rc2[1];
			
			x10_rc2[0] = vx[0] ^ vx[1];
			y10_rc2[0] = vy[0] ^ vy[1];
			
			/* Karatsuba step 1: w0_rc2,vx,vy (0,0) */
			if(vx[0]!=0 && vy[0]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(vx[0]);
				y0 = (WORD16)(vy[0]);

				/* Karatsuba step 0: w0,x0,y0 (0,0) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(vx[0] >> 16);
				y1 = (WORD16)(vy[0] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (0,0) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (0,0) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w0_rc2[0] = w0 ^ (wall << 16);
				w0_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* vx[0]==0 or vy[0]==0 */
				{
				w0_rc2[0] = 0;
				w0_rc2[1] = 0;
				} /* Collecting results back in w0_rc2 */

			/* Karatsuba step 1: w1_rc2,vx,vy (1,1) */
			if(vx[1]!=0 && vy[1]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(vx[1]);
				y0 = (WORD16)(vy[1]);

				/* Karatsuba step 0: w0,x0,y0 (1,1) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(vx[1] >> 16);
				y1 = (WORD16)(vy[1] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (1,1) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (1,1) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w1_rc2[0] = w0 ^ (wall << 16);
				w1_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* vx[1]==0 or vy[1]==0 */
				{
				w1_rc2[0] = 0;
				w1_rc2[1] = 0;
				} /* Collecting results back in w1_rc2 */

			/* Karatsuba step 1: w10_rc2,x10_rc2,y10_rc2 (0,0) */
			if(x10_rc2[0]!=0 && y10_rc2[0]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(x10_rc2[0]);
				y0 = (WORD16)(y10_rc2[0]);

				/* Karatsuba step 0: w0,x0,y0 (0,0) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(x10_rc2[0] >> 16);
				y1 = (WORD16)(y10_rc2[0] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (0,0) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (0,0) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w10_rc2[0] = w0 ^ (wall << 16);
				w10_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* x10_rc2[0]==0 or y10_rc2[0]==0 */
				{
				w10_rc2[0] = 0;
				w10_rc2[1] = 0;
				} /* Collecting results back in w10_rc2 */

			
			w0_rc4[0]   = w0_rc2[0];
			w0_rc4[2]   = w1_rc2[0];
			wall_rc2[0] = w0_rc2[0] ^ w1_rc2[0] ^ w10_rc2[0];
			
			w0_rc4[1]   = w0_rc2[1];
			w0_rc4[3]   = w1_rc2[1];
			wall_rc2[1] = w0_rc2[1] ^ w1_rc2[1] ^ w10_rc2[1];
			
			
			w0_rc4[2]   ^= wall_rc2[1];
			w0_rc4[1]   ^= wall_rc2[0];
			
			} while(0);  /* Collecting results back in w0_rc4 */

		/* Karatsuba step 2: w1_rc4,vx,vy (2,2) */
		do	{
			WORD32 w1_rc2[2];
			WORD32 w0_rc2[2];
			WORD32 w10_rc2[2];
			WORD32 wall_rc2[2];
			WORD32 x10_rc2[1],y10_rc2[1];
			
			x10_rc2[0] = vx[2] ^ vx[3];
			y10_rc2[0] = vy[2] ^ vy[3];
			
			/* Karatsuba step 1: w0_rc2,vx,vy (2,2) */
			if(vx[2]!=0 && vy[2]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(vx[2]);
				y0 = (WORD16)(vy[2]);

				/* Karatsuba step 0: w0,x0,y0 (2,2) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(vx[2] >> 16);
				y1 = (WORD16)(vy[2] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (2,2) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (2,2) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w0_rc2[0] = w0 ^ (wall << 16);
				w0_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* vx[2]==0 or vy[2]==0 */
				{
				w0_rc2[0] = 0;
				w0_rc2[1] = 0;
				} /* Collecting results back in w0_rc2 */

			/* Karatsuba step 1: w1_rc2,vx,vy (3,3) */
			if(vx[3]!=0 && vy[3]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(vx[3]);
				y0 = (WORD16)(vy[3]);

				/* Karatsuba step 0: w0,x0,y0 (3,3) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(vx[3] >> 16);
				y1 = (WORD16)(vy[3] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (3,3) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (3,3) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w1_rc2[0] = w0 ^ (wall << 16);
				w1_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* vx[3]==0 or vy[3]==0 */
				{
				w1_rc2[0] = 0;
				w1_rc2[1] = 0;
				} /* Collecting results back in w1_rc2 */

			/* Karatsuba step 1: w10_rc2,x10_rc2,y10_rc2 (0,0) */
			if(x10_rc2[0]!=0 && y10_rc2[0]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(x10_rc2[0]);
				y0 = (WORD16)(y10_rc2[0]);

				/* Karatsuba step 0: w0,x0,y0 (0,0) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(x10_rc2[0] >> 16);
				y1 = (WORD16)(y10_rc2[0] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (0,0) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (0,0) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w10_rc2[0] = w0 ^ (wall << 16);
				w10_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* x10_rc2[0]==0 or y10_rc2[0]==0 */
				{
				w10_rc2[0] = 0;
				w10_rc2[1] = 0;
				} /* Collecting results back in w10_rc2 */

			
			w1_rc4[0]   = w0_rc2[0];
			w1_rc4[2]   = w1_rc2[0];
			wall_rc2[0] = w0_rc2[0] ^ w1_rc2[0] ^ w10_rc2[0];
			
			w1_rc4[1]   = w0_rc2[1];
			w1_rc4[3]   = w1_rc2[1];
			wall_rc2[1] = w0_rc2[1] ^ w1_rc2[1] ^ w10_rc2[1];
			
			
			w1_rc4[2]   ^= wall_rc2[1];
			w1_rc4[1]   ^= wall_rc2[0];
			
			} while(0);  /* Collecting results back in w1_rc4 */

		/* Karatsuba step 2: w10_rc4,x10_rc4,y10_rc4 (0,0) */
		do	{
			WORD32 w1_rc2[2];
			WORD32 w0_rc2[2];
			WORD32 w10_rc2[2];
			WORD32 wall_rc2[2];
			WORD32 x10_rc2[1],y10_rc2[1];
			
			x10_rc2[0] = x10_rc4[0] ^ x10_rc4[1];
			y10_rc2[0] = y10_rc4[0] ^ y10_rc4[1];
			
			/* Karatsuba step 1: w0_rc2,x10_rc4,y10_rc4 (0,0) */
			if(x10_rc4[0]!=0 && y10_rc4[0]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(x10_rc4[0]);
				y0 = (WORD16)(y10_rc4[0]);

				/* Karatsuba step 0: w0,x0,y0 (0,0) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(x10_rc4[0] >> 16);
				y1 = (WORD16)(y10_rc4[0] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (0,0) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (0,0) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w0_rc2[0] = w0 ^ (wall << 16);
				w0_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* x10_rc4[0]==0 or y10_rc4[0]==0 */
				{
				w0_rc2[0] = 0;
				w0_rc2[1] = 0;
				} /* Collecting results back in w0_rc2 */

			/* Karatsuba step 1: w1_rc2,x10_rc4,y10_rc4 (1,1) */
			if(x10_rc4[1]!=0 && y10_rc4[1]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(x10_rc4[1]);
				y0 = (WORD16)(y10_rc4[1]);

				/* Karatsuba step 0: w0,x0,y0 (1,1) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(x10_rc4[1] >> 16);
				y1 = (WORD16)(y10_rc4[1] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (1,1) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (1,1) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w1_rc2[0] = w0 ^ (wall << 16);
				w1_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* x10_rc4[1]==0 or y10_rc4[1]==0 */
				{
				w1_rc2[0] = 0;
				w1_rc2[1] = 0;
				} /* Collecting results back in w1_rc2 */

			/* Karatsuba step 1: w10_rc2,x10_rc2,y10_rc2 (0,0) */
			if(x10_rc2[0]!=0 && y10_rc2[0]!=0)
				{
				WORD32 w1,w0,w10;
				WORD32 wall;
				WORD32 x1,y1,x0,y0;
				WORD32 x10,y10;
				x0 = (WORD16)(x10_rc2[0]);
				y0 = (WORD16)(y10_rc2[0]);

				/* Karatsuba step 0: w0,x0,y0 (0,0) */
				if(x0!=0 && y0!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x0);
					_y0 = (WORD_8)(y0);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x0) >> 8);
					_y1 = (WORD_8)((y0) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w0 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w0=0; /* x0 == 0 or y0 == 0 */

				x1 = (WORD16)(x10_rc2[0] >> 16);
				y1 = (WORD16)(y10_rc2[0] >> 16);
				/* Karatsuba step 0: w1,x1,y1 (0,0) */
				if(x1!=0 && y1!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x1);
					_y0 = (WORD_8)(y1);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x1) >> 8);
					_y1 = (WORD_8)((y1) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w1 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w1=0; /* x1 == 0 or y1 == 0 */

				x10 = x1^x0;
				y10 = y1^y0;

				/* Karatsuba step 0: w10,x10,y10 (0,0) */
				if(x10!=0 && y10!=0)
					{
					WORD32 _w1,_w0,_w10;
					WORD32 _x1,_y1,_x0,_y0;
					WORD32 _x10,_y10;
					WORD32 _wall;
					_x0 = (WORD_8)(x10);
					_y0 = (WORD_8)(y10);
					_w0 = K8X8[(_x0  << 8) | _y0 ];
					_x1 = (WORD_8)((x10) >> 8);
					_y1 = (WORD_8)((y10) >> 8);
					_w1 = K8X8[(_x1  << 8) | _y1 ];
					_x10 = _x1^_x0;
					_y10 = _y1^_y0;
					_w10= K8X8[(_x10 << 8) | _y10];
					_wall = _w1 ^ _w0 ^ _w10;
					w10 = (_w1 << 16) ^ _w0 ^ (_wall << 8);
					}
				else w10=0; /* x10 == 0 or y10 == 0 */

				wall = w1 ^ w0 ^ w10;
				w10_rc2[0] = w0 ^ (wall << 16);
				w10_rc2[1] = w1 ^ (wall >> 16);
				}
			else  /* x10_rc2[0]==0 or y10_rc2[0]==0 */
				{
				w10_rc2[0] = 0;
				w10_rc2[1] = 0;
				} /* Collecting results back in w10_rc2 */

			
			w10_rc4[0]   = w0_rc2[0];
			w10_rc4[2]   = w1_rc2[0];
			wall_rc2[0] = w0_rc2[0] ^ w1_rc2[0] ^ w10_rc2[0];
			
			w10_rc4[1]   = w0_rc2[1];
			w10_rc4[3]   = w1_rc2[1];
			wall_rc2[1] = w0_rc2[1] ^ w1_rc2[1] ^ w10_rc2[1];
			
			
			w10_rc4[2]   ^= wall_rc2[1];
			w10_rc4[1]   ^= wall_rc2[0];
			
			} while(0);  /* Collecting results back in w10_rc4 */

		
		vz[0]   = w0_rc4[0];
		vz[4]   = w1_rc4[0];
		wall_rc4[0] = w0_rc4[0] ^ w1_rc4[0] ^ w10_rc4[0];
		
		vz[1]   = w0_rc4[1];
		vz[5]   = w1_rc4[1];
		wall_rc4[1] = w0_rc4[1] ^ w1_rc4[1] ^ w10_rc4[1];
		
		vz[2]   = w0_rc4[2];
		vz[6]   = w1_rc4[2];
		wall_rc4[2] = w0_rc4[2] ^ w1_rc4[2] ^ w10_rc4[2];
		
		vz[3]   = w0_rc4[3];
		vz[7]   = w1_rc4[3];
		wall_rc4[3] = w0_rc4[3] ^ w1_rc4[3] ^ w10_rc4[3];
		
		
		vz[4]   ^= wall_rc4[2];
		vz[2]   ^= wall_rc4[0];
		
		vz[5]   ^= wall_rc4[3];
		vz[3]   ^= wall_rc4[1];
	
		} while(0);  /* Collecting results back in vz */
#endif
	}




