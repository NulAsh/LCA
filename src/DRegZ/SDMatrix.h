
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// SDMatrix.h: interface for the SDMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDMATRIX_H__C2E65E07_58EF_4A99_BB13_64239E24A10B__INCLUDED_)
#define AFX_SDMATRIX_H__C2E65E07_58EF_4A99_BB13_64239E24A10B__INCLUDED_

#include <vector>
#include <list>

#include "SDCluster.h"
#include "LNMatrix.h"
#include "FastKey.h"

#include "callbacks.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef std::vector<ELEMENT> VECELEMENT;
typedef std::list<VECELEMENT> SOLLIST;


class SDMatrix  
{
public:
	int Solve(SOLLIST &sollst);
	int GetKnownVector(ELEMENT *p, int size);
	int SetKnownVector(ELEMENT *p,int size);
	int SetRandom(void);
	int Init(int _hsize, int _vsize,int _mul);
	void Reset(void);
	SDMatrix();
	virtual ~SDMatrix();

protected:
	bool pick_random_sol;
	int mhsize;
	int mvsize;
	int mul;
	int hsize;
	int vsize;
	SDCluster **clst;
private:
	int sfound;
	int _solve(int midx,SOLLIST &sollst, ELEMENT *srow, int sidx);

public:
	int DecompressBuffer(WORD_8 *buff);
	int CompressToBuffer(WORD_8 *buff, int &buffsize);
	int GetReqBuffSize(void);
	int CreateFastKey(CFastKey &fk);
	int DumpToFile(const char *filename);
	static int PickVectFromList(int id,SOLLIST &sollst, ELEMENT *v,int vsize);
	int SolveEx(SOLLIST &sollst,ELEMENT *kv,int kvsize);
	int Evaluate(ELEMENT *ep, int size, ELEMENT *r,int rsize);
	int Jolt(SDMatrix &out, LNMatrix &i1, LNMatrix &i2,FN_GAUGE_CALLBACK gCallBack=NULL);
	int TestSolutions(SOLLIST &sols);
	int TestSolution(ELEMENT *ep, int size);
	void PickRandom(bool _pick_random_sol) { pick_random_sol = _pick_random_sol; };
};

#endif // !defined(AFX_SDMATRIX_H__C2E65E07_58EF_4A99_BB13_64239E24A10B__INCLUDED_)
