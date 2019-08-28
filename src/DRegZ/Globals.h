
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// Globals.h: interface for the CGlobals class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALS_H__0CDE3CB2_1684_4E7A_81C4_59B254BC7F67__INCLUDED_)
#define AFX_GLOBALS_H__0CDE3CB2_1684_4E7A_81C4_59B254BC7F67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
typedef std::string STRING;

#define PRINTSTRINGBUFFSIZE   4096

#include "RandPool.h"

extern CRandPool RandPool;

#define PRINTCALLCONVENTION _cdecl
typedef void (*FN_PRINTFUNC)(const char *s);

STRING JoinPaths(STRING s1,STRING s2);
void   SetPrintFunction(FN_PRINTFUNC pf);
int    PrintLog(const char *str, ... );

#endif // !defined(AFX_GLOBALS_H__0CDE3CB2_1684_4E7A_81C4_59B254BC7F67__INCLUDED_)
