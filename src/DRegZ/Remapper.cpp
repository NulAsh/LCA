
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// Remapper.cpp: implementation of the CRemapper class.
//
//////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <memory.h>
#include "SDVect.h"
#include "Remapper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemapper::CRemapper()
{
	rmap = NULL;
	Reset();
}

CRemapper::~CRemapper()
{
	Reset();
}

void CRemapper::Reset()
{
	if(rmap)
		{
		for(int i=0;i<rsize;i++)
			delete rmap[i];

		delete[] rmap;
		rmap  = NULL;
		}

	rsize = 0;
	ndim  = 0;
}


int CRemapper::Init(int _ndim)
{
	if(_ndim < 1 || _ndim > 1000) return 0;

	Reset();

	rsize = SDVect::GetVMSize(_ndim);
	if(rsize <=0) return -1;
	
	ndim = _ndim;

	rmap = new SDVect *[rsize];
	if(!rmap) return -2;

	memset(rmap,0x00,rsize * sizeof(SDVect *));

	for(int i=0;i<rsize;i++)
		rmap[i] = new SDVect(ndim);

	return 1;
}

int CRemapper::CreateMap(LNMatrix &lmtx)
	{
	if(lmtx.GetDimX() != lmtx.GetDimY()) return -1;
	int lsize = lmtx.GetDimX();

	if(!rmap)
		{
		int rr = Init(lsize);
		if(rr != 1) return rr;
		}

	for(int j=0;j<lsize;j++)
		{
		for(int i=0;i<=j;i++)
			{
			int pv = SDVect::GetOfsIJ(i,j,lsize);

			if(i == j)
				{
				for(int k=0;k<lsize;k++)
					{
					ELEMENT t = lmtx.GetTerm(i,k);
					rmap[pv]->SetTerm(k,k,t);
					}
				}
			else
				{
				for(int k=0;k<lsize;k++)
					{
					ELEMENT t1 = lmtx.GetTerm(i,k);
					if(t1 == 0) continue;

					for(int m=0;m<lsize;m++)
						{
						ELEMENT t2 = lmtx.GetTerm(j,m);
						if(t2 != 0) rmap[pv]->FlipTerm(k,m);
						}
					}
				}
			}
		}
	
	return 1;	
	}










