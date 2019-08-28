
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


// RandPool.cpp: implementation of the CRandPool class.
//
//////////////////////////////////////////////////////////////////////

//#include <windows.h>
#include "RandPool.h"
#include "osdepn.h"

#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "arc4.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRandPool::CRandPool()
	{
	InitRC4(&lRC4,"0123456789ABCDEF",16);

	// Discard first 256 bytes as precaution
	int i;
	for(i=0;i<256;i++)
		(void)RC4_GetByte(&lRC4);
	}

CRandPool::~CRandPool()
	{
	}

void CRandPool::GetBlock(unsigned char *block, int size)
	{
	int i;

	for(i=0;i<size;i++)
		block[i] = RC4_GetByte(&lRC4);
	}

int CRandPool::Randomize()
{
	const int lbsize = 256;
	unsigned char lbuff[lbsize];
	
	int rr = GetRandomData(lbuff,lbsize);
	if(rr != 1) 
		{
		int ll = GetLastError();

		printf("Error initializing... %d [%08x]\n",rr,ll);
		return 0;
		}

	int i;
	for(i=0;i<256;i++)
		lbuff[i] ^= (unsigned char)RC4_GetByte(&lRC4);

	InitRC4(&lRC4,(char *)lbuff,lbsize);

	// Discard first 256 bytes as precaution
	for(i=0;i<256;i++)
		(void)RC4_GetByte(&lRC4);
	
	return 1;
}
