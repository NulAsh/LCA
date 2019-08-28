
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


// FastKey.cpp: implementation of the CFastKey class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>
#include "FastKey.h"
#include "LNPoly.h"

#include "Globals.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const char *CFastKey::scfilename = "lcvec";

CFastKey::CFastKey()
{
	eqs = 0;
	mblk = NULL;
}

CFastKey::~CFastKey()
{
	Reset();
}

void CFastKey::Reset()
{
	delete[] mblk;
	mblk = NULL;
}

int CFastKey::Init(int _ndim,int _eqs)
{
	if(_ndim < 1 || _ndim > 512) return -1;
	if(_eqs  < 16 || _eqs > 512) return -2;

	Reset();

	eqs = _eqs;
	eblocks = (eqs + 31) / 32;
	
	ndim = _ndim;

	vecsize = SDVect::GetVMSize(ndim);
	if(vecsize <= 0) return -3;

	mbsize = vecsize * eblocks;

	mblk = new unsigned int[mbsize];
	if(!mblk) return -4;

	memset(mblk,0,mbsize * sizeof(unsigned int));
	return 1;
}


int CFastKey::PlaceKey(int eq, int idx, ELEMENT v)
{
	if(eq < 0 || eq >= eqs) return -1;

	int eq_d = eq / 32;
	int eq_m = eq % 32;

	unsigned int bmask = (1 << eq_m);

	if(v != 0) mblk[(idx * eblocks) + eq_d] |= bmask;
	else mblk[(idx * eblocks) + eq_d] &= (bmask ^ 0xFFFFFFFF);	

	return 1;
}


void CFastKey::BlockExpand(unsigned int *b,ELEMENT *v,int size)
{
	for(int i=0;i<size;i++)
		{
		unsigned int bmask = (1 << (i & 0x1f) );
		
		if(b[i >> 5] & bmask) v[i] = 0x01;
		else v[i] = 0;
		}

	return;
}

int CFastKey::BlockCompress(ELEMENT *v, int size, unsigned int *b)
{
	memset(b,0x00,((size+31)/32) * sizeof(unsigned int));

	for(int i=0;i<size;i++)
		{
		unsigned int bmask = (1 << (i & 0x1f) );
		
		if(v[i] != 0) b[i >> 5] |= bmask;
		else b[i >> 5] &= (bmask ^ 0xffffffff);
		}

	return 1;
}

int CFastKey::Evaluate(ELEMENT *x, int xsize,ELEMENT *r, int rsize)
	{
	if(xsize != ndim) return -1;
	if(rsize != eqs)  return -2;
	
	unsigned int pot[100];
	int cursor = 0;

	for(int i=0;i<eblocks;i++)
		pot[i] = mblk[cursor++];

	for(int j=0;j<ndim;j++)
		for(int i=0;i<=j;i++)
			{
			unsigned int pr = (x[i] & x[j]) != 0 ? 0xFFFFFFFF : 0x00;
			
			for(int k=0;k<eblocks;k++)
				pot[k] ^= (mblk[cursor++] & pr);
			}

	BlockExpand(pot,r,rsize);
	return 1;
	}

void CFastKey::SetFingerPrint(WORD32 *fp, int size)
{
	if(size != 5) return;
	memcpy(fingerprint,fp,size * sizeof(WORD32));
}

int CFastKey::WriteCStructure(const char *path)
{
	STRING out;
	FILE *h1;
	
	out = JoinPaths(path,scfilename) + ".h";
	
	PrintLog("Writing file %s\n",out.c_str());
	h1 = fopen(out.c_str(),"w");
	if(!h1) return -1;

	unsigned int rc1 = RandPool.GetUInt();
	unsigned int rc2 = RandPool.GetUInt();

	int polycoefs[16];
	int polysize = LNPoly::GetIrredPoly(eqs - KEYIDSIZE, polycoefs,16);

	fprintf(h1,"\n#ifndef LICENSE_CODE_%08X%08X_H\n#define LICENSE_CODE_%08X%08X_H\n\n",
		rc1,rc2,rc1,rc2);

	fprintf(h1,"#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");

	fprintf(h1,"// Created by DRegZ from LCA suite\n");
	fprintf(h1,"// see: http://www.webalice.it/giuliano.bertoletti/lca.html\n\n");

	fprintf(h1,"#define LICENSE_CHARS    25\n");
	fprintf(h1,"#define KEYIDSIZE        %d\n",KEYIDSIZE);
	fprintf(h1,"#define G_KEYSIZE        %d\n",ndim);
	fprintf(h1,"#define G_RESULTSIZE     %d\n",eqs);
	fprintf(h1,"#define POTSIZE          %d\n",eblocks);
	fprintf(h1,"#define POLYSIZE         %d\n",polysize);
	fprintf(h1,"#define REDUNDANCY_BITS  5\n");
	fprintf(h1,"\n");
	fprintf(h1,"#define LICENSE_VECTOR   %d\n",mbsize);
	fprintf(h1,"\n");
	fprintf(h1,"extern const unsigned int lsvec[LICENSE_VECTOR];\n");
	fprintf(h1,"extern const int ipol[POLYSIZE];\n");
	fprintf(h1,"\n");
	fprintf(h1,"#ifdef __cplusplus\n}\n#endif\n");
	fprintf(h1,"\n");
	fprintf(h1,"#endif    /* LICENSE_CODE_%08X%08X_H */\n\n",rc1,rc2);

	fclose(h1);

	out = JoinPaths(path,scfilename) + ".c";

	PrintLog("Writing file %s\n",out.c_str());
	h1 = fopen(out.c_str(),"w");
	if(!h1) return -1;

	fprintf(h1,"\n#include \"%s.h\"\n\n",scfilename);

	fprintf(h1,"// Created by DRegZ from LCA suite\n");
	fprintf(h1,"// see: http://www.webalice.it/giuliano.bertoletti/lca.html\n\n");

	fprintf(h1,"const int ipol[POLYSIZE] = { ");
	
	int i;
	for(i=0;i<polysize;i++)
		{
		fprintf(h1,"%d",polycoefs[i]);
		if(i < polysize - 1) fprintf(h1,",");
		}
	fprintf(h1," };\n\n");

	if(testcodes.size() > 0)
		{
		fprintf(h1,"/*\n   Some valid codes for testing:\n");

		for(STRINGLIST::iterator i=testcodes.begin();i!=testcodes.end();i++)
			fprintf(h1,"   %s\n",(*i).c_str());

		fprintf(h1,"*/\n\n");
		}
	
	fprintf(h1,"/* Key fingerprint: %08X-%08X-%08X-%08X-%08X */\n",
		fingerprint[0],fingerprint[1],fingerprint[2],fingerprint[3],fingerprint[4]);

	fprintf(h1,"const unsigned int lsvec[LICENSE_VECTOR] = {\n");

	const int each = 8;

	for(i=0;i<mbsize;i++)
		{
		int j = i % 8;

		if(j == 0) fprintf(h1,"  ");

		fprintf(h1,"0x%08x",mblk[i]);

		if(i < mbsize - 1) fprintf(h1,",");
		else fprintf(h1,"\n");
	
		if(j == (each-1) && i != (mbsize-1)) fprintf(h1,"\n");
		}

	fprintf(h1,"  };\n\n");

	fclose(h1);
	return 1;
}

