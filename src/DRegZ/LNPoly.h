
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// LNPoly.h: interface for the LNPoly class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LNPOLY_H__712D3A7C_425D_41D5_A788_2D6C15266202__INCLUDED_)
#define AFX_LNPOLY_H__712D3A7C_425D_41D5_A788_2D6C15266202__INCLUDED_

#include "SDVect.h"
#include "ffpoly.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef char ELEMENT;

class LNPoly  
{
public:
	static int GetIrredPoly(int deg,int *coef,int maxcoefs);
	static int GetIrredPoly(int deg, FFPOLY &fp);
	static int Test(int sigsize,int parsize);
	static int VerifyParity(ELEMENT *signature, int sigsize, ELEMENT *parity, int psize);
	static int ComputeParity(ELEMENT *in, int isize, ELEMENT *out,int osize);
	LNPoly();
	virtual ~LNPoly();
};

#endif // !defined(AFX_LNPOLY_H__712D3A7C_425D_41D5_A788_2D6C15266202__INCLUDED_)
