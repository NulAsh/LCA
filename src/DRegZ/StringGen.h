
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// StringGen.h: interface for the CStringGen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGGEN_H__286B9AF0_7E01_4AE4_AF01_21BC55E5505F__INCLUDED_)
#define AFX_STRINGGEN_H__286B9AF0_7E01_4AE4_AF01_21BC55E5505F__INCLUDED_

#if _MSC_VER > 1000
#pragma warning( disable : 4786 4788)
#pragma once
#endif // _MSC_VER > 1000

#include "SDVect.h"

#include <algorithm>
#include <string>
typedef std::string STRING;


class CStringGen  
{
public:
	static int UndecorateString(const char *deccode,char *undeccode);
	static int Char2Val(char c);
	static int DecorateString(const char *codestr,char *out,int each);
	static int CodeStr2Element(const char *code, ELEMENT *p, int pmaxsize);
	static int Element2CodeStr(ELEMENT *p, int psize, STRING &code);
	static int Val2Char(int n);
	CStringGen();
	virtual ~CStringGen();

};

#endif // !defined(AFX_STRINGGEN_H__286B9AF0_7E01_4AE4_AF01_21BC55E5505F__INCLUDED_)
