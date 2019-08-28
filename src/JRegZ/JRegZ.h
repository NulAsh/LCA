/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


// JRegZ.h: interface for the CJRegZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JREGZ_H__1EF5FDF9_7C38_46DD_9234_440E0B5A10B5__INCLUDED_)
#define AFX_JREGZ_H__1EF5FDF9_7C38_46DD_9234_440E0B5A10B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define JREGZ_MINCODESIZE   64
#define JREGZ_MAXCODESIZE  128

#define VINEGAR_BITS         4

#include "LNMatrix.h"
#include "quartzsig.h"
#include "MVTrap.h"
#include "ExPolyHold.h"
#include "rootfind.h"

#include <vector>

typedef std::vector<CExPolyHold> EXPOLYHOLDVECTOR;


class CJRegZ : public CMVTrap  
{
protected:
	FFPOLY cmod;

protected:
	int get_vinegar(ELEMENT *in);
	int get_poly_deg(int size);
	int build_vinegar();
	int compute_beta_coefficient(FFPOLY *beta,FV_COMPONENTS_STRUCT *fc,int v,int i,FFPOLY *cmod);
	int compute_gamma_coefficient(FFPOLY *gamma,FV_COMPONENTS_STRUCT *fc,int v,FFPOLY *cmod);
	void choose_solution(FFPOLY *Z,ROOT_POT *pot);

	int codesize;

	LNMatrix mtx_s;
	LNMatrix mtx_is;
	LNMatrix mtx_t;
	LNMatrix mtx_it;

	EXPOLYHOLDVECTOR vinegar;
	FV_COMPONENTS_STRUCT *fc;
	int isok;

public:
	static int LoopTest(int loops=-1, int pow=7, int size=125);

	int GetN() { return codesize; }
	int GetD() { return fc->deg; }
	int GetPow() { return fc->pow; }

	int PrivateBackward(const BOOLVECTOR &in, BOOLVECTOR &out);
	int PrivateForward(const BOOLVECTOR &in, BOOLVECTOR &out);
	int SetSecurityLevel(int level);
	int LoadFromBuffer(const BYTEVECTOR &bv);
	int SaveToBuffer(BYTEVECTOR &bv);
	int GenerateKeyPair(void);
	bool SetSize(int size);

	CJRegZ();
	virtual ~CJRegZ();

	int GetEngineID() { return LCA_JREGZ_ID; }
	int GetStatus();
	
	int GetDefaultParityBitsNo() const        { return 86; }
	int GetDefaultFeatureBitsNo() const       { return 32; }
	int GetDefaultRedundancyBitsNo() const    { return  7; }
	int GetExpectedForwardInputBitsNo() const { return codesize; }

	std::string GetAlgorithmName() const { return "JRegZ"; }

};

#endif // !defined(AFX_JREGZ_H__1EF5FDF9_7C38_46DD_9234_440E0B5A10B5__INCLUDED_)
