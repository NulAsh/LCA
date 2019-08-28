/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef KARATSUBA_MACROS_H
#define KARATSUBA_MACROS_H

#include "mytypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ffpoly.h"
// #include "expoly.h"

#define KARATSUBA_EXPOLY_CROSS   40

int init_karatsuba();
void free_karatsuba();

extern int karatsuba_cross;

int expoly_mul_karatsuba(FFPOLY *vz,FFPOLY *vx,FFPOLY *vy,int size,FFPOLY *cmod,FFPOLY *mem);
int expoly_fast_mul_karatsuba(FFPOLY *vz,FFPOLY *vx,int xdeg,FFPOLY *vy,int ydeg,int size,FFPOLY *cmod,FFPOLY *mem);

int karatsuba_mul_recursive(WORD32 *vz,WORD32 *vx,WORD32 *vy,int size);
void karatsuba_unroll(WORD32 *vz,WORD32 *vx,WORD32 *vy);

void expoly_recursive_karatsuba_mul(FFPOLY *c,FFPOLY *a,int sa,FFPOLY *b,int sb,FFPOLY *stk,FFPOLY *cmod);


#ifdef _DEBUG
extern int kara_curmem;
extern int kara_topmem;

#endif

#define KARATSUBA_MUL_PLAIN_LOOP(res,id1,id2) do { \
	res = 0; \
	if(id1!=0)  \
		{ \
		while(id2 != 0) \
			{ \
			res ^= (id2 & 0x01 ? id1 : 0); \
			id1<<=1; \
			id2>>=1; \
			} \
		} \
	} while(0)


/*
	res1 ^= (id12 & 0x01 ? id11 : 0); \
	res2 ^= (id22 & 0x01 ? id21 : 0); \
	res3 ^= (id32 & 0x01 ? id31 : 0); \
	id11<<=1; \
	id12>>=1; \
	id21<<=1; \
	id22>>=1; \
	id31<<=1; \
	id32>>=1; \
*/

#define KARATSUBA_MUL_PLAIN_16_x_3(res1,res2,res3,id11,id12,id21,id22,id31,id32) do { \
	static WORD32 ptemp1[2] = { 0,0 }; \
	static WORD32 ptemp2[2] = { 0,0 }; \
	static WORD32 ptemp3[2] = { 0,0 }; \
	ptemp1[1]=id11; \
	ptemp2[1]=id21; \
	ptemp3[1]=id31; \
	res1=res2=res3=0; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	ptemp1[1]<<=1; \
	id12>>=1; \
	ptemp2[1]<<=1; \
	id22>>=1; \
	ptemp3[1]<<=1; \
	id32>>=1; \
	; \
	res1 ^= ptemp1[id12 & 0x01]; \
	res2 ^= ptemp2[id22 & 0x01]; \
	res3 ^= ptemp3[id32 & 0x01]; \
	} while(0)

#define KARATSUBA_MUL_PLAIN_16(res,id1,id2) do { \
	res=0; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	} while(0)

#define KARATSUBA_MUL_PLAIN_8(res,id1,id2) do { \
	res=0; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	id1<<=1; \
	id2>>=1; \
	res ^= (id2 & 0x01 ? id1 : 0); \
	} while(0)

#define KARATSUBA_MUL_PLAIN(_res,_id1,_id2) do { \
	WORD32 _x0,_x1,_x10;\
	WORD32 _y0,_y1,_y10;\
	WORD32 _w0,_w1,_w10,_wall;\
	WORD32 _rlow,_rhigh;\
	_x0=(WORD_8)(_id1>>8);\
	_x1=(WORD_8)_id1;\
	_y0=(WORD_8)(_id2>>8);\
	_y1=(WORD_8)_id2;\
	_x10=_x0^_x1;\
	_y10=_y0^_y1;\
	KARATSUBA_MUL_PLAIN_8(_w0,_x0,_y0);\
	KARATSUBA_MUL_PLAIN_8(_w1,_x1,_y1);\
	KARATSUBA_MUL_PLAIN_8(_w10,_x10,_y10);\
	_wall=_w0^_w1^_w10;\
	_rlow = (WORD16)(_w1 ^ (_wall << 8));\
	_rhigh= (_w0 ^ (_wall >> 8)) << 16;\
	_res=(_rlow | _rhigh); \
	} while(0) 

#define KARATSUBA_MUL_16X16(_vz,_vx,_vy) \
	{ \
	WORD32 _w1,_w0,_w10; \
	WORD32 _x1,_y1,_x0,_y0; \
	WORD32 _x10,_y10; \
	WORD32 _wall; \
	_x1 = (WORD_8)(_vx >> 8); \
	_x0 = (WORD_8)(_vx); \
	_y1 = (WORD_8)(_vy >> 8); \
	_y0 = (WORD_8)(_vy); \
	_x10 = _x1^_x0; \
	_y10 = _y1^_y0; \
	_w0 = K8X8[(_x0  << 8) | _y0 ]; \
	_w1 = K8X8[(_x1  << 8) | _y1 ]; \
	_w10= K8X8[(_x10 << 8) | _y10]; \
	_wall = _w1 ^ _w0 ^ _w10; \
	_vz = (_w1 << 16) ^ _w0 ^ (_wall << 8); \
	} while(0) \

#ifdef __cplusplus
}
#endif

#endif
