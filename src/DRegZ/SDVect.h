
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// SDVect.h: interface for the SDVect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDVECT_H__5E6A900C_1690_429F_B56B_0AD3B0CC5AE9__INCLUDED_)
#define AFX_SDVECT_H__5E6A900C_1690_429F_B56B_0AD3B0CC5AE9__INCLUDED_

#if _MSC_VER > 1000
#pragma warning( disable : 4786 4788)
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "mytypes.h"


#define SDVECT_DEFAULT_SIZE    16



typedef char ELEMENT;

class SDVect  
{
protected:
	int vecsize;
	int ndim;
	ELEMENT *vec;

public:
	void Reset();
	int GetNDim() { return ndim; }
	int Init(int _n);
	int DecompressBuffer(WORD_8 *buff);
	int GetReqBuffSize(void);
	int CompressToBuffer(WORD_8 *buff,int &buffsize);
	int DumpToFile(FILE *h1);
	int FlipTerm(int x,int y);
	int PlusEx(SDVect *ad);
	void Clear(void);
	static void RandElementVector(ELEMENT *v,int size);
	static void GetRandomPermutation(int *perm,int size,int rloops=100);
	int partial_evaluate(SDVect *nv,ELEMENT *p, int size);
	ELEMENT evaluate(unsigned int vl,int size);
	void Print();
	void SetRandom(void);
	ELEMENT evaluate(ELEMENT *p,int size);
	int GetTerm(int x,int y, ELEMENT &v);
	int SetTerm(int x,int y, ELEMENT v);
	static int GetOfsIJ(int i,int j, int a);
	static int GetVMSize(int _n);
	SDVect(int _n);
	SDVect();
	//SDVect(const SDVect &T);
	virtual ~SDVect();

	
};


#endif // !defined(AFX_SDVECT_H__5E6A900C_1690_429F_B56B_0AD3B0CC5AE9__INCLUDED_)
