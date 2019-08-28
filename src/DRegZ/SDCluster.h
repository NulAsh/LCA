
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// SDCluster.h: interface for the SDCluster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDCLUSTER_H__AD017C2D_773E_4EC2_BEC6_D5AC8CCC820F__INCLUDED_)
#define AFX_SDCLUSTER_H__AD017C2D_773E_4EC2_BEC6_D5AC8CCC820F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SDVect.h"
#include <vector>

// #define DUMPWHILESOLVING

typedef std::vector<unsigned int> VECUINT;

class SDCluster  
{
public:
	SDCluster();
	SDCluster(int _eqs,int _ndim);
	virtual ~SDCluster();

public:
	int DecompressBuffer(WORD_8 *buff);
	int GetReqBuffSize(void);
	int CompressToBuffer(WORD_8 *buff,int &buffsize);
	int DumpToFile(FILE *h1);
	int Evaluate(ELEMENT *ep, int size, ELEMENT *r,int rsize);
	int TestSolution(ELEMENT *ep, int size);
	int Init(int _eqs,int _ndim);
	int Reduce(ELEMENT *ep, int size, SDCluster &nc);
	void Reset(void);
	int Solve(VECUINT &ls);
	void SetRandom(void);
	int eqs;
	int ndim;
	SDVect **sv;

};

#endif // !defined(AFX_SDCLUSTER_H__AD017C2D_773E_4EC2_BEC6_D5AC8CCC820F__INCLUDED_)
