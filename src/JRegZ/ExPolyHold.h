/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// ExPolyHold.h: interface for the CExPolyHold class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPOLYHOLD_H__ABBDE8DE_F7AF_4CA5_9360_E99F6159FF64__INCLUDED_)
#define AFX_EXPOLYHOLD_H__ABBDE8DE_F7AF_4CA5_9360_E99F6159FF64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "expoly.h"

class CExPolyHold  
{
public:
	EXPOLY e;

public:
	int SetSize(int _size);
	CExPolyHold();
	virtual ~CExPolyHold();

	CExPolyHold(const CExPolyHold &B);

	CExPolyHold & operator=(const CExPolyHold &B);
};

#endif // !defined(AFX_EXPOLYHOLD_H__ABBDE8DE_F7AF_4CA5_9360_E99F6159FF64__INCLUDED_)
