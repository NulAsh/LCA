/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


// ExPolyHold.cpp: implementation of the CExPolyHold class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "ExPolyHold.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExPolyHold::CExPolyHold()
{
	e.ep = NULL;
	e.pvdeg = -1;
	e.pvsize = 0;
}

CExPolyHold::~CExPolyHold()
{
	expoly_free(&e);
}

int CExPolyHold::SetSize(int _size)
{
	expoly_free(&e);
	return expoly_init(&e, _size);
}

CExPolyHold::CExPolyHold(const CExPolyHold &B)
{
	if(&B == this) return;

	e.ep = NULL;
	e.pvdeg = -1;
	e.pvsize = 0;

	expoly_init(&e,B.e.pvsize);
	if(B.e.pvdeg >= 0) {
		expoly_copy(&e,(EXPOLY *)&B.e);
	}
}

CExPolyHold & CExPolyHold::operator=(const CExPolyHold &B)
{
	if(&B != this) {
		expoly_free(&e);
		e.ep = NULL;
	
		expoly_init(&e,B.e.pvsize);

		if(B.e.pvdeg >= 0) {
			expoly_copy(&e,(EXPOLY *)&B.e);
		}
	}
	
	return *this;
}

