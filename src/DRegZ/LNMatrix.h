
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// LNMatrix.h: interface for the LNMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LNMATRIX_H__4287E4F6_A30C_41C3_836B_8EAD1CCE2629__INCLUDED_)
#define AFX_LNMATRIX_H__4287E4F6_A30C_41C3_836B_8EAD1CCE2629__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lintra.h"

typedef char ELEMENT;

class LNMatrix  
{
public:
	int Print(void);
	int SqMul(LNMatrix &factor, LNMatrix &res);
	int Invert(LNMatrix &mout);
	int CreateInvertible(void);
	int CreateInvertible(RC4_CTX &lRC4);
	int Initialize(int _dimx,int _dimy, bool zeroset=true);
	void Reset();
	LNMatrix();
	LNMatrix(int _dimx,int _dimy);
	virtual ~LNMatrix();

protected:
	WORD_8 ** mtx;
	int dimy;
	int dimx;

public:
	int GetReqBuffSize(void);
	int CompressToBuffer(WORD_8 *buff, int &buffsize);
	int DecompressBuffer(WORD_8 *buff);
	int Transpose(LNMatrix &t);
	int MulLRow(ELEMENT *in, int size, ELEMENT *out);
	int MulLVect(ELEMENT *in, int size, ELEMENT *out);
	const WORD_8 GetTerm(int x,int y);
	int GetDimX() { return dimx; }
	int GetDimY() { return dimy; }
	
};

#endif // !defined(AFX_LNMATRIX_H__4287E4F6_A30C_41C3_836B_8EAD1CCE2629__INCLUDED_)
