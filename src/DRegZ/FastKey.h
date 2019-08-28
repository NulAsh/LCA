
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// FastKey.h: interface for the CFastKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FASTKEY_H__A7592CF0_B251_4789_8DCC_666CC21EAFF6__INCLUDED_)
#define AFX_FASTKEY_H__A7592CF0_B251_4789_8DCC_666CC21EAFF6__INCLUDED_

#include "SDVect.h"
#include "mytypes.h"	// Added by ClassView

#include <list>
#include <string>

typedef std::string STRING;
typedef std::list<STRING> STRINGLIST;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXMTXSIZE  512

#define ROUNDBITS    5
#define LICIDBITS   31

#define KEYIDSIZE   (LICIDBITS + ROUNDBITS)
#define TOTROUNDS   (1 << ROUNDBITS)

class CFastKey  
{
protected:
	WORD32 fingerprint[5];
	static const char *scfilename;

public:
	void SetFingerPrint(WORD32 *fp,int size);
	int WriteCStructure(const char *path);
	static int BlockCompress(ELEMENT *v,int size,unsigned int *b);
	static void BlockExpand(unsigned int *b,ELEMENT *v,int size);
	int Evaluate(ELEMENT *x, int xsize,ELEMENT *r, int rsize);
	int vecsize;
	int PlaceKey(int eq, int idx, ELEMENT v);
	void Reset(void);
	int Init(int _ndim,int _eqs);
	CFastKey();
	virtual ~CFastKey();

	int mbsize;
	unsigned int * mblk;
	int eblocks;
	int ndim,eqs;

	STRINGLIST testcodes;
};

#endif // !defined(AFX_FASTKEY_H__A7592CF0_B251_4789_8DCC_666CC21EAFF6__INCLUDED_)
