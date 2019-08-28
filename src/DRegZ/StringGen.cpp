
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


// StringGen.cpp: implementation of the CStringGen class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "StringGen.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringGen::CStringGen()
{

}

CStringGen::~CStringGen()
{

}

int CStringGen::Val2Char(int n)
{
	int rv;
	if(n<0 || n>=32) return -1;
	if(n<26) return 'A' + n;
        
	rv = '2' + (n - 26);
	if(rv == '5') rv = '9';  /* Ascii '5' is replaced by '9' */

	return rv;
}

int CStringGen::Char2Val(char c)
{
	/* Those my be confused and are not possible results ! */
	/* If fed back to the decoder they are remapped to letters */
	if(c == '1') c='I';
	if(c == '0') c='O';
	if(c == '5') c='S';
	if(c == '8') c='B';

	/* Code 0-25 for alphabets letters. */
	if(isalpha(c)) return (int)(toupper(c) - 'A');

	/* Codes from 26 to 31 are for the six accepted digits. */

	if(c>='2' && c<'8') return 26 + ( c - '2' );
        
	if( c=='9' ) return 26 + ( '5' - '2' );
	return -1;    
}

int CStringGen::DecorateString(const char *codestr, char *out, int each)
{
	size_t i,j=0;
	size_t len = strlen(codestr);

	for(i=0;i<len;i++)
		{
		if(i>0 && i%each == 0) out[j++]='-';
		out[j++]=codestr[i];
		}

	out[j]='\0';
	return 1;
}


int CStringGen::UndecorateString(const char *deccode, char *undeccode)
{
	size_t len = strlen(deccode);
	size_t j=0;
	size_t i;

	for(i=0;i<len;i++)
		{
		if(deccode[i]=='-') continue;
		else if(Char2Val(deccode[i]) < 0) return 0;

		if(j >= 32) return 0;
		undeccode[j++] = deccode[i];
		}

	undeccode[j]='\0';
	return 1;
}

int CStringGen::Element2CodeStr(ELEMENT *p, int psize, STRING &code)
{
	if(psize != 125) return 0;

	char out[40],out2[40];

	int k,id=0;
	int outp=0;

	for(k=0;k<psize;k++)
		{
		int i=k%5;

		if(i == 0) id = 0;

		if(p[k] != 0) id |= (1 << i);

		if(i == 4) out[outp++] = Val2Char(id);
		}

	out[outp++] = '\0';

	DecorateString(out,out2,5);

	code = out2;
	return 1;
}

int CStringGen::CodeStr2Element(const char *code, ELEMENT *p, int pmaxsize)
{
	char t1[50],t2[500];

	if(strlen(code) > 40) return 0;

	int rr = UndecorateString(code,t1);
	if(rr != 1) return 0;

	size_t len = strlen(t1);
	size_t i,j=0;

	for(i=0;i<len;i++)
		{
		int v = Char2Val(t1[i]);
		if( v < 0 ) return 0;

		int k;

		for(k=0;k<5;k++)
			t2[j++] = (v >> k) & 0x01;
		}

	if(j > (size_t)pmaxsize) return -1;

	for(i=0;i<j;i++)
		p[i] = t2[i];

	return (int)j;
}

