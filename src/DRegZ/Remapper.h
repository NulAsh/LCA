
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// Remapper.h: interface for the CRemapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REMAPPER_H__2492023B_E58C_435B_B42C_83C18E134941__INCLUDED_)
#define AFX_REMAPPER_H__2492023B_E58C_435B_B42C_83C18E134941__INCLUDED_

#include "LNMatrix.h"
#include "SDVect.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRemapper  
{
public:
	int ndim;
	int CreateMap(LNMatrix &lmtx);
	int Init(int _rsize);
	void Reset();
	int rsize;
	SDVect **rmap;
	CRemapper();
	virtual ~CRemapper();

};

#endif // !defined(AFX_REMAPPER_H__2492023B_E58C_435B_B42C_83C18E134941__INCLUDED_)
