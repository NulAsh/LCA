
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// SDMatrix.cpp: implementation of the SDMatrix class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>

#include "SDMatrix.h"
#include "Remapper.h"
#include "osdepn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SDMatrix::SDMatrix()
{
	clst = NULL;

	hsize = 0;
	vsize = 0;
	mul   = 0;

	mvsize = 0;
	mhsize = 0;

	pick_random_sol = true;
}

SDMatrix::~SDMatrix()
{
	Reset();
}

void SDMatrix::Reset()
	{
	if(clst != NULL)
		{
		for(int i=0;i<mul;i++)
			{
			delete clst[i];
			clst[i] = NULL;
			}

		delete[] clst;
		clst = NULL;

		hsize = 0;
		vsize = 0;
		mul   = 0;

		mvsize = 0;
		mhsize = 0;
		}
	}


int SDMatrix::Init(int _hsize, int _vsize, int _mul)
{
	if(_hsize < 1 || _hsize > 1000 || _vsize < 1 || _vsize > 1000) return 0;

	Reset();

	hsize = _hsize;
	vsize = _vsize;
	mul   = _mul;

	mvsize = 0;
	mhsize = 0;

	clst = new SDCluster *[mul];
	if(!clst) return 0;

	for(int j=0;j<mul;j++)
		{
		int ndim = hsize * (j+1);
		int eqs  = vsize;

		clst[j] = new SDCluster(eqs,ndim);

		mvsize += eqs;
		mhsize =  MAX(mhsize,ndim);
		}

	return 1;
}

int SDMatrix::SetRandom()
{
	if(!clst) return 0;

	for(int i=0;i<mul;i++)
		clst[i]->SetRandom();

	return 1;
}

int SDMatrix::SetKnownVector(ELEMENT *p, int size)
{
	if(size != mvsize) return 0;
	if(!clst) return -1;

	int eq = 0;
	for(int i=0;i<mul;i++)
		for(int j=0;j<clst[i]->eqs;j++)
			clst[i]->sv[j]->SetTerm(-1,-1,p[eq++]);	// Known term!

	return 1;
}

int SDMatrix::GetKnownVector(ELEMENT *p, int size)
{
	if(size != mvsize) return 0;
	if(!clst) return -1;

	int eq = 0;
	for(int i=0;i<mul;i++)
		for(int j=0;j<clst[i]->eqs;j++)
			clst[i]->sv[j]->GetTerm(-1,-1,p[eq++]);	// Known term!

	return 1;
}

int SDMatrix::Solve(SOLLIST &sollst)
{
	sollst.clear();

	ELEMENT *srow = new ELEMENT[mhsize];
	memset(srow,0x00,mhsize * sizeof(ELEMENT));

	sfound = 0;

	_solve(0,sollst,srow,0);

	delete[] srow;

	return sfound > 0 ? 1 : 0;
}

int SDMatrix::SolveEx(SOLLIST &sollst, ELEMENT *kv, int kvsize)
{
	if(kvsize != mvsize) return -1;

	ELEMENT *bkp = new ELEMENT[kvsize];
	memset(bkp,0x00,kvsize);

	int i;
	for(i=0;i<mvsize;i++)
		{
		int px = i % vsize;
		int py = i / vsize;

		clst[py]->sv[px]->GetTerm(-1,-1,bkp[i]);
		clst[py]->sv[px]->SetTerm(-1,-1,bkp[i] ^ kv[i]);
		}

	int rr = Solve(sollst);

	for(i=0;i<mvsize;i++)
		{
		int px = i % vsize;
		int py = i / vsize;

		clst[py]->sv[px]->SetTerm(-1,-1,bkp[i]);
		}

	delete[] bkp;
	return rr;
}

int SDMatrix::_solve(int midx, SOLLIST &sollst, ELEMENT *srow, int sidx)
{
	if(midx == mul)
		{
		VECELEMENT tvec(sidx);

		//std::copy(srow,srow+sidx,tvec);
		for(int i=0;i<sidx;i++)
			tvec[i] = srow[i];

#ifdef DUMPWHILESOLVING
		printf("Sol found: #%d\n",sfound);
#endif
		sollst.push_back(tvec);	
		sfound++;
		return 1;
		}

#ifdef DUMPWHILESOLVING
	printf("Depth: %d/%d\n",midx+1,mul);
#endif

	SDCluster nc;
	int rr;

	rr = clst[midx]->Reduce(srow,sidx,nc);
	if(rr <= 0) return -10;

	VECUINT ls;

	rr = nc.Solve(ls);
	if(rr <= 0) return rr;

	int sz = nc.ndim;

	if(pick_random_sol == false)
		{
		int count = 0;

		for(VECUINT::iterator i=ls.begin();i!=ls.end();i++)
			{
			for(int j=0;j<sz;j++)
				srow[sidx + j] = ((((*i) >> j) & 0x01) ? 0xff : 0x00);

			rr = _solve(midx+1,sollst,srow,sidx+sz);
			if(rr < 0) return rr;

			if(rr > 0) count++;
			}

		if(count == 0) return 0;
		}
	else
		{
		int msz = (int)ls.size();
		int perm[100];

		SDVect::GetRandomPermutation(perm,msz);

		int i;
		for(i=0;i<msz;i++)
			{
#ifdef DUMPWHILESOLVING
			printf("Picking solution #%d\n",perm[i]);
#endif
			for(int j=0;j<sz;j++)
				srow[sidx + j] = (((ls[perm[i]] >> j) & 0x01) ? 0xff : 0x00);

			rr = _solve(midx+1,sollst,srow,sidx+sz);
			if(rr < 0) return rr;
			if(rr > 0) break;
			}

		if(i == msz) return 0;
		}

	return 1;
}

int SDMatrix::TestSolution(ELEMENT *ep, int size)
{
	int count = 0;

	for(int j=0;j<mul;j++)
		{
		int ndim = hsize * (j+1);
		int rr = clst[j]->TestSolution(ep,ndim);
		if(rr <= 0) return rr;

		count+=rr;
		}

	if(count != mul * vsize) return -2;
	return 1;
}

int SDMatrix::TestSolutions(SOLLIST &sols)
{
	int sl = 0;

	for(SOLLIST::iterator i=sols.begin();i!=sols.end();i++)
		{
		int nsz = (int)(*i).size();
		ELEMENT *ep = new ELEMENT[nsz];

		for(int j=0;j<nsz;j++)
			ep[j] = (*i)[j];

		int rr = TestSolution(ep,nsz);

		delete[] ep;

		if( rr <= 0 ) return rr;
		
		sl++;
		}

	return sl;
}

int SDMatrix::Jolt(SDMatrix &out, LNMatrix &i1, LNMatrix &i2,FN_GAUGE_CALLBACK gCallBack)
{
	if(i1.GetDimX() != i1.GetDimY()) return -1;
	if(i1.GetDimX() != mvsize) return -2;

	if(i2.GetDimX() != i2.GetDimY()) return -3;
	if(i2.GetDimX() != mhsize) return -4;

	SDMatrix tmp;
	GAUGE gauge;

	memset(&gauge,0x00,sizeof(GAUGE));

	int i,j;
	int rr;
	
	rr = tmp.Init(mhsize,mvsize,1);
	if(rr != 1) return rr;

	strcpy(gauge.caption,"Expanding key");
	gauge.pass = EXPANDING_KEY;
	gauge.t_gauge = mvsize;
	gauge.gauge   = 0;

	for(i=0;i<mvsize;i++)
		{
		for(j=0;j<mvsize;j++)
			{
			if(i1.GetTerm(i,j) != 0)
				{
				int px = j % vsize;
				int py = j / vsize;

				tmp.clst[0]->sv[i]->PlusEx(clst[py]->sv[px]);
				}
			
			}
		
		if(gCallBack) 
			{
			int rr = gCallBack(&gauge);
			if(rr <= 0) return rr;
			}
	
		gauge.gauge++;
		}

	if(gCallBack) 
		{
		int rr = gCallBack(&gauge);
		if(rr <= 0) return rr;
		}

	CRemapper rmp;
	
	rr = rmp.Init(mhsize);
	if(rr <= 0) return rr;

	rr = rmp.CreateMap(i2);
	if(rr <= 0) return rr;

	rr = out.Init(mhsize,mvsize,1);
	if(rr != 1) return rr;

	strcpy(gauge.caption,"Jolting key");
	gauge.pass = JOLTING_KEY;
	gauge.t_gauge = mvsize;
	gauge.gauge = 0;

	int k;
	for(k=0;k<mvsize;k++)
		{
		SDVect *dst = out.clst[0]->sv[k];
		SDVect *src = tmp.clst[0]->sv[k];

		ELEMENT t;
		src->GetTerm(-1,-1,t);
		dst->SetTerm(-1,-1,t);

		for(int j=0;j<mhsize;j++)
			for(int i=0;i<=j;i++)
				{
				ELEMENT t;
				src->GetTerm(i,j,t);

				if( t != 0 )
					{
					int ofs = SDVect::GetOfsIJ(i,j,mhsize);
					if(ofs <= 0) return -9;

					dst->PlusEx(rmp.rmap[ofs]);
					}
				}

		//printf("Jolting: %d/%d....\r",k,mvsize);
		if(gCallBack) 
			{
			int rr = gCallBack(&gauge);
			if(rr <= 0) return rr;
			}

		gauge.gauge++;
		}

	if(gCallBack) 
		{
		int rr = gCallBack(&gauge);
		if(rr <= 0) return rr;
		}

	//printf("Jolting: %d/%d....\n",k,mvsize);
	return 1;
}

int SDMatrix::Evaluate(ELEMENT *ep, int size, ELEMENT *r, int rsize)
{
	if(rsize != mvsize) return -1;
	
	int cc=0;
	for(int i=0;i<mul;i++)
		{
		if(clst[i]->ndim > size) return -2-i;

		int qsize = MIN(clst[i]->ndim,size);

		if( cc + clst[i]->eqs > rsize) return -9;

		int rr = clst[i]->Evaluate(ep,qsize,r + cc, clst[i]->eqs);
		if(rr <= 0) return rr;

		cc+=clst[i]->eqs;
		}

	return 1;
}


int SDMatrix::PickVectFromList(int id, SOLLIST &sollst, ELEMENT *v, int vsize)
{
	int j=0;
	for(SOLLIST::iterator i=sollst.begin();i!=sollst.end();i++,j++)
		{
		if(j == id)
			{
			if((*i).size() != (unsigned int)vsize) return -1;

			for(int k=0;k<vsize;k++)
				v[k] = (*i)[k];

			return 1;
			}
		}

	return 0;
}


int SDMatrix::DumpToFile(const char *filename)
{
	FILE *h1 = fopen(filename,"w");
	if(!h1) return 0;

	for(int i=0;i<mul;i++)
		clst[i]->DumpToFile(h1);

	fclose(h1);
	return 1;
}

int SDMatrix::CreateFastKey(CFastKey &fk)
{
	if(mul != 1) return 0;

	SDCluster *sd = clst[0];

	fk.Init(sd->ndim,sd->eqs);

	for(int k=0;k<sd->eqs;k++)
		{
		ELEMENT v;
		int idx=0;
		
		sd->sv[k]->GetTerm(-1,-1,v);
		int rr = fk.PlaceKey(k,idx++,v);
		if(rr != 1) return rr;

		for(int j=0;j<sd->ndim;j++)
			for(int i=0;i<=j;i++)
				{
				sd->sv[k]->GetTerm(i,j,v);
				rr = fk.PlaceKey(k,idx++,v);
				if(rr != 1) return rr;
				}
		}

	return 1;
}

int SDMatrix::GetReqBuffSize()
{
	int reqsize = 6;

	for(int i=0;i<mul;i++)
		reqsize += clst[i]->GetReqBuffSize();

	return reqsize;	
}

int SDMatrix::CompressToBuffer(WORD_8 *buff, int &buffsize)
{
	int reqsize = GetReqBuffSize();
	if(buffsize < reqsize)
		{
		buffsize = reqsize;
		return -1;
		}

	*(WORD16 *)(buff+0) = (WORD16)hsize;
	*(WORD16 *)(buff+2) = (WORD16)vsize;
	*(WORD16 *)(buff+4) = (WORD16)mul;

	int ofs = 6;
	reqsize -= ofs;

	for(int i=0;i<mul;i++)
		{
		int rr = clst[i]->CompressToBuffer(buff + ofs, reqsize);
		if(rr < 0) return -2;

		reqsize-=rr;
		ofs+=rr;
		}

	if(reqsize != 0) return -5;
	return ofs;
}

int SDMatrix::DecompressBuffer(WORD_8 *buff)
{
	int _hsize = *(WORD16 *)(buff+0);
	int _vsize = *(WORD16 *)(buff+2);
	int _mul   = *(WORD16 *)(buff+4);

	int rr = Init(_hsize,_vsize,_mul);
	if(rr != 1) return rr;

	int ofs=6;
	for(int i=0;i<mul;i++)
		{
		int rr = clst[i]->DecompressBuffer(buff + ofs);
		if(rr < 0) return rr;
		
		ofs+=rr;
		}

	return ofs;
}
