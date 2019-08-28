
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// KeyRing.cpp: implementation of the CKeyRing class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "KeyRing.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeyRing::CKeyRing()
{

}

CKeyRing::~CKeyRing()
{
	Reset();
}

void CKeyRing::Reset()
{
	unsigned int i;
	for(i=0;i<kt.size();i++)
		delete kt[i].key;

	kt.clear();
}

