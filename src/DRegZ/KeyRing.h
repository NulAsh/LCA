
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// KeyRing.h: interface for the CKeyRing class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYRING_H__2C4EE0DD_6A05_4A73_A03B_F509205A944A__INCLUDED_)
#define AFX_KEYRING_H__2C4EE0DD_6A05_4A73_A03B_F509205A944A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KeyPair.h"

#include <vector>

class CKeyRing  
{
public:
	class CKeyToken 
		{
		public:
			STRING name;
			STRING fingerprint;
			CKeyPair *key;

		public:
			CKeyToken() { key = NULL; };
		};

	typedef std::vector<CKeyToken> KEYTABLE;

public:
	void Reset(void);
	
	CKeyRing();
	virtual ~CKeyRing();

private:
	KEYTABLE kt;
};

#endif // !defined(AFX_KEYRING_H__2C4EE0DD_6A05_4A73_A03B_F509205A944A__INCLUDED_)
