/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


// DRegZ.h: interface for the CDRegZ class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DREGZ_H__1ED5FDF9_9C38_363D_9134_440E075R10B5__INCLUDED_)
#define AFX_DREGZ_H__1ED5FDF9_9C38_363D_9134_440E075R10B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DREGZ_MINCODESIZE  120
#define DREGZ_MAXCODESIZE  120

#include "MVTrap.h"
#include "KeyPair.h"

class CDRegZ : public CMVTrap  
{
protected:
	void Reset();

	CKeyPair *kp;
	int codesize;
	int isok;

public:
	int PrivateBackward(const BOOLVECTOR &in, BOOLVECTOR &out);
	int PrivateForward(const BOOLVECTOR &in, BOOLVECTOR &out);
	int SetSecurityLevel(int level);
	int LoadFromBuffer(const BYTEVECTOR &bv);
	int SaveToBuffer(BYTEVECTOR &bv);
	int GenerateKeyPair(void);
	bool SetSize(int size);

	CDRegZ();
	virtual ~CDRegZ();

	int GetEngineID() { return LCA_DREGZ_ID; }
	int GetStatus();
	
	int GetDefaultParityBitsNo() const        { return  84; }
	int GetDefaultFeatureBitsNo() const       { return  31; }
	int GetDefaultRedundancyBitsNo() const    { return   5; }
	int GetExpectedForwardInputBitsNo() const { return 128; }

	std::string GetAlgorithmName() const { return "DRegZ"; }
};








#endif	// AFX_DREGZ_H__1ED5FDF9_9C38_363D_9134_440E075R10B5__INCLUDED_
