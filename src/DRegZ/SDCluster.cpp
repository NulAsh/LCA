
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


// SDCluster.cpp: implementation of the SDCluster class.
//
//////////////////////////////////////////////////////////////////////

#include "SDCluster.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SDCluster::SDCluster()
	{
	sv = NULL;
	}

SDCluster::SDCluster(int _eqs,int _ndim)
	{
	sv = NULL;
	if(Init(_eqs,_ndim) != 1) throw _eqs;
	}

int SDCluster::Init(int _eqs, int _ndim)
	{
	Reset();

	eqs = _eqs;
	ndim = _ndim;

	sv = new SDVect *[eqs];
	if(!sv) return 0;

	for(int i=0;i<eqs;i++)
		{
		sv[i] = new SDVect(ndim);
		if(!sv[i]) return 0;
		}

	return 1;
	}

SDCluster::~SDCluster()
{
	Reset();
}

void SDCluster::Reset()
{
	if(!sv) return;

	for(int i=0;i<eqs;i++)
		{
		delete sv[i];
		sv[i] = NULL;
		}

	delete[] sv;

	eqs = 0;
	ndim = 0;
	sv = NULL;
}

void SDCluster::SetRandom()
{
	for(int i=0;i<eqs;i++)
		sv[i]->SetRandom();
}


int SDCluster::Solve(VECUINT &ls)
{
	if(ndim > 32) return -1;

	ls.clear();
	unsigned int ss = 0;
	unsigned int top = (1 << ndim);
	int sols=0;

	do
		{
		int count = 0;

		for(int i=0;i<eqs;i++)
			{
			if(sv[i]->evaluate(ss,ndim) == 0) count++;
			else break;
			}

		if(count == eqs) 
			{
			ls.push_back(ss);
			sols++;
			}

#ifdef DUMPWHILESOLVING
		if((ss & 0xff) == 0) printf("%u\r",ss);
#endif
		ss++;
		} while(ss != top);

#ifdef DUMPWHILESOLVING
	printf("%u --> %d\n",ss,sols);
#endif
	return sols;
}


int SDCluster::Reduce(ELEMENT *ep, int size, SDCluster &nc)
{
	if(ndim <= size) return -1;

	nc.Init(eqs,ndim-size);

	for(int i=0;i<eqs;i++)
		{
		int rr = sv[i]->partial_evaluate(nc.sv[i],ep,size);
		if(rr < 0) return rr;
		}

	return 1;
}


int SDCluster::TestSolution(ELEMENT *ep, int size)
{
	int count = 0;

	for(int i=0;i<eqs;i++)
		{
		int rr = sv[i]->evaluate(ep,size);
		if(rr != 0) return -1;

		count++;
		}

	return count;
}

int SDCluster::Evaluate(ELEMENT *ep, int size, ELEMENT *r, int rsize)
{
	if(size != ndim) return -1;
	if(rsize != eqs) return -2;

	for(int i=0;i<eqs;i++)
		r[i] = sv[i]->evaluate(ep,size);

	return 1;
}

int SDCluster::DumpToFile(FILE *h1)
{
	for(int i=0;i<eqs;i++)
		sv[i]->DumpToFile(h1);

	return 1;
}

int SDCluster::GetReqBuffSize()
	{
	int rq = 0;

	for(int i=0;i<eqs;i++)
		rq += sv[i]->GetReqBuffSize();
		
	return 4 + rq;
	}

int SDCluster::CompressToBuffer(WORD_8 *buff, int &buffsize)
{
	int reqsize = GetReqBuffSize();
	if(buffsize < reqsize)
		{
		buffsize = reqsize;
		return -1;
		}

	*(WORD16 *)(buff+0) = (WORD16) eqs;
	*(WORD16 *)(buff+2) = (WORD16) ndim;

	int ofs = 4;
	reqsize -= ofs;

	for(int i=0;i<eqs;i++)
		{
		int rr = sv[i]->CompressToBuffer(buff + ofs, reqsize);
		if(rr < 0) return -2;

		reqsize-=rr;
		ofs+=rr;
		}

	if(reqsize != 0) return -5;
	return ofs;
}

int SDCluster::DecompressBuffer(WORD_8 *buff)
{
	int _eqs  = *(WORD16 *)(buff+0);
	int _ndim = *(WORD16 *)(buff+2);

	int rr = Init(_eqs,_ndim);
	if(rr != 1) return rr;

	int ofs=4;
	for(int i=0;i<eqs;i++)
		{
		int rr = sv[i]->DecompressBuffer(buff + ofs);
		if(rr < 0) return rr;

		if(sv[i]->GetNDim() != ndim) return -3;
		ofs+=rr;
		}

	return ofs;
}



