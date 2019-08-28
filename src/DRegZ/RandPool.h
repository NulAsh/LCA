
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// RandPool.h: interface for the CRandPool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDPOOL_H__177AC67E_4EB6_4907_A798_13B1A749E250__INCLUDED_)
#define AFX_RANDPOOL_H__177AC67E_4EB6_4907_A798_13B1A749E250__INCLUDED_

#if _MSC_VER > 1000
#pragma warning( disable : 4786 4788)
#pragma once
#endif // _MSC_VER > 1000

#include "arc4.h"

#define RNDPOOL_KEYSIZE  256
#define RNDPOOL_POOLSIZE 4096

class CRandPool  
{
private:
	RC4_CTX lRC4;

public:
	CRandPool();
	virtual ~CRandPool();

public:
	int Randomize(void);
	inline int GetByte(void) { return RC4_GetByte(&lRC4); }
	inline unsigned int GetUInt(void) { return RC4_GetLong(&lRC4); };
	void GetBlock(unsigned char *block,int size);
};

#endif // !defined(AFX_RANDPOOL_H__177AC67E_4EB6_4907_A798_13B1A749E250__INCLUDED_)
