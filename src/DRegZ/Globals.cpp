
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// Globals.cpp: implementation of the CGlobals class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>

#include "Globals.h"
#include "osdepn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRandPool RandPool;
static FN_PRINTFUNC PrintFunction = NULL;

void SetPrintFunction(FN_PRINTFUNC pf)
	{
	PrintFunction = pf;
	}

STRING JoinPaths(STRING s1,STRING s2)
	{
	size_t l1 = s1.length();
	while(l1 > 0 && (s1[l1-1] == '\\' || s1[l1-1] == '/'))
		l1--;

	size_t i=0;
	size_t l2 = s2.length();
	while(i < l2 && (s1[i] == '\\' || s1[i] == '/'))
		i++;

	return s1.substr(0,l1) + "/" + s2.substr(i);
	}

int vPrintf(const char *str, va_list ap)
{
	const int count = PRINTSTRINGBUFFSIZE;
	char *s = new char[count+1];

	int len = _vsnprintf(s, count, str, ap);

	s[count] = '\0';

	PrintFunction(s);	

	delete[] s;
	return len;
}

int PrintLog(const char *str, ... )
	{
	if(!PrintFunction) return 0;

	va_list ap;
	va_start(ap, str);
	
	int res = vPrintf(str,ap);
	
	va_end(ap);
	return res;
	}
