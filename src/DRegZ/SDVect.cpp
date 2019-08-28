
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// SDVect.cpp: implementation of the SDVect class.
//
//////////////////////////////////////////////////////////////////////

// #include <windows.h>

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

#include "SDVect.h"
#include "arc4.h"

#include "Globals.h"

RC4_CTX gRC4;
bool    gRC4_inited = false;

//////////////////////////////////////////////////////////////////////

//DEL void SDVect::RandInit(const char *pass, int passsize)
//DEL {
//DEL 	InitRC4(&gRC4,pass,passsize);
//DEL 	gRC4_inited = true;
//DEL }

void SDVect::RandElementVector(ELEMENT *v, int size)
{
	RandPool.GetBlock((unsigned char *)v,size * sizeof(ELEMENT));

	for(int i=0;i<size;i++)
		v[i] &= 0x01;
}

void SDVect::GetRandomPermutation(int *perm, int size, int rloops)
{
	for(int i=0;i<size;i++)
		perm[i] = i;

	for(int kx=0;kx<rloops;kx++)
		for(int x1=0;x1<size;x1++)
			{
			int x2 = RandPool.GetUInt() % (x1+1);

			int tmp  = perm[x1];
			perm[x1] = perm[x2];
			perm[x2] = tmp;
			}
	return;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int SDVect::Init(int _n)
{
	Reset();

	vecsize = GetVMSize( _n );
	vec = new ELEMENT[ vecsize ];

	ndim = _n;
	if(vecsize == 0) return 0;

	Clear();
	return 1;
}

void SDVect::Reset()
{
	delete[] vec;
	
	vec = NULL;
	vecsize = 0;
}

SDVect::SDVect()
	{
	vec = NULL;
	vecsize = 0;	
	}

SDVect::SDVect(int _n)
{
	vec = NULL;
	Init(_n);
}

SDVect::~SDVect()
{
	Reset();	
}

int SDVect::PlusEx(SDVect *ad)
{
	if(!ad) return -1;
	if(ad->ndim > ndim) return -2;

	if(ad->ndim == ndim)
		{
		if(vecsize > 8)
			{
			unsigned int *p = (unsigned int *)vec;
			unsigned int *q = (unsigned int *)ad->vec;

			int r1 = vecsize >> 2;
			int r2 = vecsize & 0x03;

			for(int i=0;i<r1;i++)
				p[i] ^= q[i];

			ELEMENT *v1 = vec + (r1 << 2);
			ELEMENT *v2 = ad->vec + (r1 << 2);

			for(int j=0;j<r2;j++)
				v1[j] ^= v2[j];
			}
		else
			{
			for(int i=0;i<vecsize;i++)
				vec[i] ^= ad->vec[i];
			}	
		}
	else
		{
		vec[0] ^= ad->vec[0];

		for(int j=0;j<ad->ndim;j++)
			for(int i=0;i<=j;i++)
				{
				int src = GetOfsIJ(i,j,ad->ndim);
				int dst = GetOfsIJ(i,j,ndim);

				vec[dst] ^= ad->vec[src];
				}
		}

	return 1;
}

void SDVect::Clear()
{
	memset(vec,0x00,vecsize);
}

int SDVect::GetVMSize(int _n)
{
	if( _n < 1 || _n > 1000 ) return 0;
	return 1 + ( _n + _n * _n ) / 2;
}

int SDVect::GetOfsIJ(int i, int j, int a)
{
	if(i < -1 || i >= a || 
	   j < -1 || j >= a) return -1;

	if(i < 0 && j < 0) return 0;

	if(i<=j) return 1 + i + (j * (j+1))/2;
	else return 1 + j + (i * (i+1)) / 2;
}

int SDVect::SetTerm(int x, int y, ELEMENT v)
{
	int ofs = GetOfsIJ(x,y,ndim);
	if(ofs  < 0) return 0;

	vec[ofs] = v;
	return 1;
}


int SDVect::FlipTerm(int x, int y)
{
	int ofs = GetOfsIJ(x,y,ndim);
	if(ofs  < 0) return 0;

	vec[ofs] ^= 0x01;
	return 1;
}


int SDVect::GetTerm(int x, int y, ELEMENT &v)
{
	int ofs = GetOfsIJ(x,y,ndim);
	if(ofs  < 0) 
		{
		v = 0;
		return 0;
		}

	v = vec[ofs];
	return 1;
}

ELEMENT SDVect::evaluate(unsigned int vl, int size)
	{
	if(size != ndim) throw ndim;

	ELEMENT sum = vec[0];

	int tk=1;
	for(int j=0;j<ndim;j++)
		{
		if (((vl >> j) & 0x01)==0) 
			tk+=(j+1);
		else
			{
			for(int i=0;i<=j;i++)
				sum ^= (vec[tk++] & (((vl >> i) & 0x01)!=0 ? 0xff : 0x00));
			}
		}

	return sum;
	}

ELEMENT SDVect::evaluate(ELEMENT *p, int size)
	{
	if(size != ndim) throw ndim;

	ELEMENT sum = vec[0];

	int tk=1;
	for(int j=0;j<ndim;j++)
		{
		if(p[j] == 0) tk+=(j+1);
		else
			{
			for(int i=0;i<=j;i++)
				sum ^= (vec[tk++] & p[i]);
			}
		}

	return sum;
	}


void SDVect::SetRandom()
{
	RandPool.GetBlock((unsigned char *)vec,vecsize);

	for(int i=0;i<vecsize;i++)
		vec[i] &= 0x01;
}

void SDVect::Print()
{
	int terms = 0;

	for(int j=0;j<ndim;j++)
		for(int i=0;i<j;i++)
			{
			int ofs = GetOfsIJ(i,j,ndim);
			if(ofs < 0) throw ndim;

			if(vec[ofs] == 0) continue;
			if(terms > 0) printf("+");
			
			printf("x_%d*x_%d",i,j);

			terms++;
			}

	for(int i=0;i<ndim;i++)
		{
		int ofs = GetOfsIJ(i,i,ndim);
		if(ofs < 0) throw ndim;

		if(vec[ofs] == 0) continue;
		if(terms > 0) printf("+");

		printf("x_%d",i);
		terms++;
		}

	if(vec[0]!=0)
		{
		if(terms > 0) printf("+");
		printf("1");
		}

	printf("\n");
}


int SDVect::partial_evaluate(SDVect *nv,ELEMENT *p, int size)
	{
	int delta = ndim - size;
	
	if(delta <= 0) return -1;

	nv->vec[0] = vec[0];

	int ofs=1;
	for(int j=0;j<ndim;j++)
		{
		for(int i=0;i<=j;i++)
			{
			if( j < size && i < size) 
				{
				nv->vec[0] ^= (vec[ofs] & p[i] & p[j]);
				}
			else if( j >= size && i < size )
				{
				int ofq = GetOfsIJ( j - size, j - size, delta );
				nv->vec[ofq] ^= (vec[ofs] & p[i]);
				}
			else
				{
				int ofq = GetOfsIJ( i - size, j - size, delta );
				nv->vec[ofq] ^= vec[ofs];
				}

			ofs++;
			}
		}

	return 1;
	}



int SDVect::DumpToFile(FILE *h1)
{
	int flag = 0;

	for(int j=ndim-1;j>=0;j--)
		for(int i=j-1;i>=0;i--)
			{
			int ofs = GetOfsIJ(i,j,ndim);

			if(vec[ofs] == 0) continue;

			if(flag) fprintf(h1," + ");

			fprintf(h1,"x_%d*x_%d",i+1,j+1);
			flag=1;
			}

	for(int i=ndim-1;i>=0;i--)
		{
		int ofs = GetOfsIJ(i,i,ndim);

		if(vec[ofs] == 0) continue;

		if(flag) fprintf(h1," + ");

		fprintf(h1,"x_%d",i+1);
		flag=1;
		}

	if(vec[0] != 0)
		{
		if(flag) fprintf(h1," + ");
		fprintf(h1,"1");
		}

	fprintf(h1," = 0\n\n");

	return 1;
}

int SDVect::GetReqBuffSize()
{
	return 2 + (vecsize + 7) / 8;
}

int SDVect::CompressToBuffer(WORD_8 *buff, int &buffsize)
{
	int reqsize = GetReqBuffSize();
	if(buffsize < reqsize)
		{
		buffsize = reqsize;
		return -1;
		}

	memset(buff,0x00,reqsize);
	*(WORD16 *)(buff + 0) = (WORD16)ndim;

	for(int i=0;i<vecsize;i++)
		if(vec[i] != 0) buff[2 + (i >> 3)] |= (1 << (i & 0x07)); 

	return reqsize;
}

int SDVect::DecompressBuffer(WORD_8 *buff)
{
	int _ndim = *(WORD_8 *)buff;

	int rr = Init(_ndim);
	if(rr != 1) return rr;

	for(int i=0;i<vecsize;i++)
		vec[i] = (buff[2 + (i >> 3)] >> (i & 0x07)) & 0x01;
	
	return GetReqBuffSize();
}

