
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// LNPoly.cpp: implementation of the LNPoly class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>

#include "LNPoly.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const int ipoly[144][16] = { 
 { 0 },
 { 1,0 },
 { 2,1,0 },
 { 3,1,0 },
 { 4,1,0 },
 { 5,2,0 },
 { 6,1,0 },
 { 7,1,0 },
 { 8,4,3,1,0 },
 { 9,1,0 },
 { 10,3,0 },
 { 11,2,0 },
 { 12,3,0 },
 { 13,4,3,1,0 },
 { 14,5,0 },
 { 15,1,0 },
 { 16,5,3,1,0 },
 { 17,3,0 },
 { 18,3,0 },
 { 19,5,2,1,0 },
 { 20,3,0 },
 { 21,2,0 },
 { 22,1,0 },
 { 23,5,0 },
 { 24,4,3,1,0 },
 { 25,3,0 },
 { 26,4,3,1,0 },
 { 27,5,2,1,0 },
 { 28,1,0 },
 { 29,2,0 },
 { 30,1,0 },
 { 31,3,0 },
 { 32,7,3,2,0 },
 { 33,6,3,1,0 },
 { 34,4,3,1,0 },
 { 35,2,0 },
 { 36,5,4,2,0 },
 { 37,5,4,3,2,1,0 },
 { 38,6,5,1,0 },
 { 39,4,0 },
 { 40,5,4,3,0 },
 { 41,3,0 },
 { 42,5,2,1,0 },
 { 43,6,4,3,0 },
 { 44,5,0 },
 { 45,4,3,1,0 },
 { 46,1,0 },
 { 47,5,0 },
 { 48,5,3,2,0 },
 { 49,6,5,4,0 },
 { 50,4,3,2,0 },
 { 51,6,3,1,0 },
 { 52,3,0 },
 { 53,6,2,1,0 },
 { 54,6,5,4,3,2,0 },
 { 55,6,2,1,0 },
 { 56,7,4,2,0 },
 { 57,4,0 },
 { 58,6,5,1,0 },
 { 59,6,5,4,3,1,0 },
 { 60,1,0 },
 { 61,5,2,1,0 },
 { 62,6,5,3,0 },
 { 63,1,0 },
 { 64,4,3,1,0 },
 { 65,4,3,1,0 },
 { 66,3,0 },
 { 67,5,2,1,0 },
 { 68,7,5,1,0 },
 { 69,6,5,2,0 },
 { 70,5,3,1,0 },
 { 71,5,3,1,0 },
 { 72,6,4,3,2,1,0 },
 { 73,4,3,2,0 },
 { 74,6,2,1,0 },
 { 75,6,3,1,0 },
 { 76,5,4,2,0 },
 { 77,6,5,2,0 },
 { 78,6,4,3,2,1,0 },
 { 79,4,3,2,0 },
 { 80,7,5,3,2,1,0 },
 { 81,4,0 },
 { 82,7,6,4,2,1,0 },
 { 83,7,4,2,0 },
 { 84,5,0 },
 { 85,8,2,1,0 },
 { 86,6,5,2,0 },
 { 87,7,5,1,0 },
 { 88,5,4,3,2,1,0 },
 { 89,6,5,3,0 },
 { 90,5,3,2,0 },
 { 91,7,6,5,3,2,0 },
 { 92,6,5,2,0 },
 { 93,2,0 },
 { 94,6,5,1,0 },
 { 95,6,5,4,2,1,0 },
 { 96,6,5,3,2,1,0 },
 { 97,6,0 },
 { 98,7,4,3,0 },
 { 99,6,3,1,0 },
 { 100,6,5,2,0 },
 { 101,7,6,1,0 },
 { 102,6,5,3,0 },
 { 103,7,5,4,3,2,0 },
 { 104,4,3,1,0 },
 { 105,4,0 },
 { 106,6,5,1,0 },
 { 107,7,5,3,2,1,0 },
 { 108,6,4,1,0 },
 { 109,5,4,2,0 },
 { 110,6,4,1,0 },
 { 111,7,4,2,0 },
 { 112,5,4,3,0 },
 { 113,5,3,2,0 },
 { 114,5,3,2,0 },
 { 115,7,5,3,2,1,0 },
 { 116,4,2,1,0 },
 { 117,5,2,1,0 },
 { 118,6,5,2,0 },
 { 119,8,0 },
 { 120,4,3,1,0 },
 { 121,8,5,1,0 },
 { 122,6,2,1,0 },
 { 123,2,0 },
 { 124,6,5,4,3,2,0 },
 { 125,7,5,3,2,1,0 },
 { 126,7,4,2,0 },
 { 127,1,0 },
 { 128,7,2,1,0 },
 { 129,5,0 },
 { 130,3,0 },
 { 131,7,6,5,4,1,0 },
 { 132,6,5,4,2,1,0 },
 { 133,6,5,3,2,1,0 },
 { 134,7,5,1,0 },
 { 135,6,4,3,0 },
 { 136,5,3,2,0 },
 { 137,8,5,4,3,2,0 },
 { 138,8,6,5,3,2,0 },
 { 139,7,5,3,2,1,0 },
 { 140,6,4,1,0 },
 { 141,8,7,5,3,1,0 },
 { 142,7,6,5,4,1,0 },
 { 143,5,3,2,0 },
 };

LNPoly::LNPoly()
{
	
}

LNPoly::~LNPoly()
{

}


int LNPoly::ComputeParity(ELEMENT *in, int isize, ELEMENT *out, int osize)
{
	FFPOLY fp,inv,mod;

	if(isize < 16 || isize >= 144) return 0;
	if(osize < 16 || osize >= 144) return 0;

	ffpoly_zeroset(&fp);

	for(int i=0;i<isize;i++)
		if(in[i]) ffpoly_setterm(&fp,i);

	ffpoly_set(&mod,ipoly[osize],osize);
	ffpoly_inverse(&inv,&fp,&mod);

	for(int j=0;j<osize;j++)
		{
		if(ffpoly_getterm(&inv,j) != 0) out[j] = 1;
		else out[j] = 0;
		}

	return 1;
}

int LNPoly::VerifyParity(ELEMENT *signature, int sigsize, ELEMENT *parity, int psize)
{
	FFPOLY mod,rs;
	FFPOLY f1,f2;
	int i;

	if(sigsize < 16 || sigsize >= 144) return 0;

	ffpoly_zeroset(&f1);

	for(i=0;i<sigsize;i++)
		if(signature[i]) ffpoly_setterm(&f1,i);

	if( ffpoly_is_zero(&f1) ) return 0;

	ffpoly_zeroset(&f2);
	
	for(i=0;i<psize;i++)
		if(parity[i]) ffpoly_setterm(&f2,i);

	if( ffpoly_is_zero(&f2) ) return 0;

	ffpoly_set(&mod,ipoly[psize],psize);
	ffpoly_mulmod(&rs,&f1,&f2,&mod);

	return ffpoly_is_identity(&rs);
}

int LNPoly::Test(int sigsize, int parsize)
{
	ELEMENT s1[200],p1[200];

	if(sigsize < 10 || sigsize >= 200) return -1;
	if(parsize < 10 || parsize >= 144) return -2;

	if(sigsize > parsize) return -3;

	memset(s1,0x00,200 * sizeof(ELEMENT));
	memset(p1,0x00,200 * sizeof(ELEMENT));

	SDVect::RandElementVector(s1,sigsize);
	
	ComputeParity(s1,sigsize,p1,parsize);
	return VerifyParity(s1,sigsize,p1,parsize);
}

int LNPoly::GetIrredPoly(int deg, int *coef,int maxcoefs)
{
	if(deg < 16 || deg >= 144) return -1;
	FFPOLY mod;

	ffpoly_set(&mod,ipoly[deg],deg);

	int count=0;
	for(int i=mod.pvdeg;i>=0;i--)
		if(ffpoly_getterm(&mod,i) != 0)
			{
			if(count < maxcoefs) coef[count++] = i;
			else return -1;
			}

	return count;
}

int LNPoly::GetIrredPoly(int deg, FFPOLY &fp)
{
	if(deg < 16 || deg >= 144) return -1;
	ffpoly_set(&fp,ipoly[deg],deg);
	return 1;
}

