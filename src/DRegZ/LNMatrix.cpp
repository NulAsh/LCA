
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// LNMatrix.cpp: implementation of the LNMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>

#include "RandPool.h"
#include "SDVect.h"
#include "LNMatrix.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


LNMatrix::LNMatrix()
{
	mtx = NULL;
	Reset();
}

LNMatrix::LNMatrix(int _dimx,int _dimy) 
{
	mtx = NULL;
	Initialize(_dimx,_dimy);
}

LNMatrix::~LNMatrix()
{
	Reset();
}

void LNMatrix::Reset()
{
	if(mtx)
		{
		for(int i=0;i<dimy;i++)
			{
			delete[] mtx[i];
			mtx[i] = NULL;
			}

		delete[] mtx;
		mtx = NULL;
		}

	dimx = 0;
	dimy = 0;
}

int LNMatrix::Initialize(int _dimx, int _dimy, bool zeroset)
{
	if(_dimx < 1 || _dimy < 1 || _dimx > 1000 || _dimy > 1000) return 0;

	Reset();

	dimx = _dimx;
	dimy = _dimy;

	mtx = new WORD_8 *[dimy];
	if(!mtx) return -1;

	for(int i=0;i<dimy;i++)
		{
		mtx[i] = new WORD_8[dimx];
		if(zeroset) memset(mtx[i],0x00,dimx);
		}
	
	return 1;
}

int LNMatrix::CreateInvertible(RC4_CTX &lRC4)
{
	if(!mtx) return 0;
	if(dimx != dimy) return -1;

	return create_invertible_matrix(&lRC4,mtx,dimx);
}


int LNMatrix::CreateInvertible()
{
	if(!mtx) return 0;
	if(dimx != dimy) return -1;

	RC4_CTX gRC4;

	const int keysize = 256;
	unsigned char key[keysize];

	RandPool.GetBlock(key,keysize);
	InitRC4(&gRC4,(char *)key,keysize);

	return create_invertible_matrix(&gRC4,mtx,dimx);
}


int LNMatrix::Invert(LNMatrix &mout)
{
	if(!mtx) return 0;
	if(dimx != dimy) return -1;

	if( mout.Initialize(dimx,dimy,false) != 1 ) return -2;
	return invmtx(mtx,mout.mtx,dimx);
}

int LNMatrix::SqMul(LNMatrix &factor, LNMatrix &res)
{
	if(!mtx || !factor.mtx) return 0;
	if(dimx != dimy || factor.dimx != factor.dimy || dimx != factor.dimx) return -1;
	
	res.Initialize(dimx,dimy,false);
	mtx_mult(mtx,factor.mtx,dimx,res.mtx);
	
	return 1;
}

int LNMatrix::Print()
{
	if(!mtx) return 0;
	if(dimx != dimy) return -1;
	print_mtx(mtx,dimx);

	return 1;
}

const WORD_8 LNMatrix::GetTerm(int x, int y)
{
	if(!mtx) return 0xfe;
	
	if(x >= 0 && x <= dimx && y >= 0 && y < dimy) return mtx[x][y];
	else return 0xff;
}

int LNMatrix::MulLVect(ELEMENT *in, int size, ELEMENT *out)
{
	if(size != dimx || size != dimy) return -1;
	vect_mult(mtx,(WORD_8 *)in,size,(WORD_8 *)out);
	
	return 1;
}

int LNMatrix::MulLRow(ELEMENT *in, int size, ELEMENT *out)
{
	if(size != dimx || size != dimy) return -1;
	row_mult(mtx,(WORD_8 *)in,size,(WORD_8 *)out);
	
	return 1;
}

int LNMatrix::Transpose(LNMatrix &t)
{
	if(!mtx) return 0;
	if(dimx != dimy) return -1;
	
	t.Initialize(dimx,dimy,false);

	trans_mtx(t.mtx,mtx,dimx);
	return 1;
}

int LNMatrix::GetReqBuffSize()
{
	return 4 + (dimx * dimy + 7) / 8;	
}

int LNMatrix::DecompressBuffer(WORD_8 *buff)
{
	int _dimx = *(WORD16 *)(buff + 0);	
	int _dimy = *(WORD16 *)(buff + 2);	

	int rr = Initialize(_dimx,_dimy,false);
	if(rr != 1) return rr;

	int count = 0;
	int ofs   = 4;

	for(int i=0;i<dimy;i++)
		for(int j=0;j<dimx;j++)
			{
			mtx[i][j] = ((buff[ofs + (count >> 3)]) & (1 << (count & 0x07))) != 0 ? 1 : 0;
			count++;
			}

	return (count + 7)/8 + ofs;
}

int LNMatrix::CompressToBuffer(WORD_8 *buff, int &buffsize)
{
	int reqsize = GetReqBuffSize();
	if(buffsize < reqsize)
		{
		buffsize = reqsize;
		return -1;
		}

	memset(buff,0x00,reqsize);

	*(WORD16 *)(buff + 0) = (WORD16)dimx;
	*(WORD16 *)(buff + 2) = (WORD16)dimy;

	int count = 0;
	int ofs   = 4;

	for(int i=0;i<dimy;i++)
		for(int j=0;j<dimx;j++)
			{
			if(mtx[i][j] != 0) buff[ofs + (count >> 3)] |= (1 << (count & 0x07));
			count++;
			}

	return reqsize;
}

