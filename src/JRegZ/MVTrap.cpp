/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// MVTrap.cpp: implementation of the CMVTrap class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "MVTrap.h"
#include "osdepn.h"

#define MAX_POTSIZE   8

RC4_CTX CMVTrap::lRc4;
bool CMVTrap::lRc4inited = false;

//////////////////////////////////////////////////////////////////////

void CMVTrap::SeedPrng(const void *_buff, int _buffsize)
{
	SHS_INFO shs;

	shsInit(&shs);
	shsUpdate(&shs,(unsigned char *)_buff,_buffsize);
	shsFinal(&shs);

	InitRC4(&lRc4,(const char *)shs.digest,sizeof(shs.digest));
}

WORD32 CMVTrap::GetRandomWord32()
{
	return RC4_GetLong(&lRc4);
}

int CMVTrap::map_xyp2id(int x,int y, int a)
{
	if(x == 0xff && y == 0xff) return 0;
	if(x < 0 || x >= a || y<x || y >= a) return -1;

	// id = P(a) - P(a - x) + y - x,
	// where P(n) = n*(n+1)/2

	// return (2*y - x*x - x*(1-2*a)) / 2;
	return x + y*(y+1)/2;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMVTrap::CMVTrap()
{
	if(!lRc4inited) {
		const int seedbuffsize = 256;
		unsigned char seedbuff[seedbuffsize];

		GetRandomData(seedbuff,sizeof(seedbuff));
		SeedPrng(seedbuff,sizeof(seedbuff));

		lRc4inited = true;
	}
}

CMVTrap::~CMVTrap()
{

}

int CMVTrap::PubKeyReset(int _eqs, int _vars)
{
	eqs = _eqs;
	vars = _vars;
	psize = (eqs + 31) / 32;

	int vsz = psize * (1 + vars*(vars+1) / 2);

	pubkey.clear();
	for(int i=0;i<vsz;i++) {
		pubkey.push_back( 0 );
	}

	return vsz;
}

int CMVTrap::PubKeySetKnownTerm(int eq, bool bit)
{
	if(eq < 0 || eq >= eqs) return 0;
	
	int idx = eq / 32;
	int ofs = eq % 32;

	int mask = (1 << ofs);
	int umask = mask ^ 0xFFFFFFFF;

	if(bit) pubkey[idx] |= mask;
	else pubkey[idx] &= umask;

	return 1;
}

int CMVTrap::PubKeySetQuadTerm(int eq, int i, int j, bool bit)
{
	if(eq < 0 || eq >= eqs) return 0;
	if(i < 0 || j >= vars || i > j) return 0;

	int id = map_xyp2id(i,j,vars) + 1;

	int idx = psize * id + eq / 32;
	int ofs = eq % 32;

	// printf("%d,%d\n",idx,ofs);

	int mask = (1 << ofs);
	int umask = mask ^ 0xFFFFFFFF;

	if(bit) pubkey[idx] |= mask;
	else pubkey[idx] &= umask;

	return 1;
}

int CMVTrap::DerivePublicKey()
{
	BOOLVECTOR b1,gamma,alpha,k1;
	int i,j,k;

	for(i=0;i<vars;i++) {
		b1.push_back(false);
	}

	PrivateForward(b1,gamma);

	for(i=0;i<eqs;i++) {
		PubKeySetKnownTerm(i,gamma[i]);
	}

	std::vector<BOOLVECTOR> beta;
	for(i=0;i<vars;i++) {
		for(j=0;j<vars;j++) {
			b1[j] = false;
		}

		b1[i] = true;

		PrivateForward(b1,k1);

		for(j=0;j<(int)k1.size();j++) {
			k1[j] = exor(k1[j],gamma[j]);
		}

		beta.push_back(k1);
	}	

	for(j=0;j<vars;j++) {
		for(i=0;i<=j;i++) {
			for(k=0;k<vars;k++) {
				b1[k] = false;
			}

			b1[i] = b1[j] = true;
			PrivateForward(b1,alpha);

			//int id = map_xyp2id(i,j,vars) + 1;
			//printf("%d\n",id);

			for(k=0;k<(int)alpha.size();k++) {
				int rr;
				
				if(i == j) {
					rr = PubKeySetQuadTerm(k,i,j,alpha[k] ^ gamma[k]);
				} else {
					rr = PubKeySetQuadTerm(k,i,j,alpha[k] ^ gamma[k] ^ beta[i][k] ^ beta[j][k]);
				}

				if(rr != 1) return 0;
			}
		}
	}

	/*
	int ptop = psize * 32;

	for(j=0;j<vars;j++) {
		for(i=0;i<=j;i++) {
			for(k=eqs;k<ptop;k++) {
				bool rnd = (RC4_GetByte(&lRc4) & 0x01) != 0;
				int rr = PubKeySetQuadTerm(k,i,j,rnd);
				if(rr != 1) return 0;
			}
		}
	}
	*/
	// printf("\n");

	return 1;
}

int CMVTrap::PublicForward(const BOOLVECTOR &in, BOOLVECTOR &out)
{
	WORD32 pot[MAX_POTSIZE];
	int i,j,k;
	int count = 0;

	for(i=0;i<psize;i++) {
		pot[i] = pubkey[count++];
	}

	for(j=0;j<vars;j++) {
		for(i=0;i<=j;i++) {
			WORD32 mask = (in[i] & in[j]) ? 0xFFFFFFFF : 0x00000000;

			for(k=0;k<psize;k++) {
				pot[k] ^= (pubkey[count++] & mask);
			}
		}
	}

	if(count != (int)pubkey.size()) return 0;

	out.clear();

	for(k=0;k<psize;k++) {
		for(i=0;i<32 && (int)out.size() < eqs;i++) {
			out.push_back( ((pot[k] >> i) & 0x01) != 0 );
		}
	}

	return 1;
}

int CMVTrap::PublicKeyDump()
{
	for(size_t i=0;i<pubkey.size();i++) {
		if(i % 4 == 0) printf("\n");
		printf(" %08x",pubkey[i]);
	}

	printf("\n");
	return 1;
}

void CMVTrap::DumpVector(const BOOLVECTOR &b)
{
	int v=0;
	
	printf("%d: ",b.size());
	size_t i;

	for(i=0;i<b.size();i++) {
		if(i % 32 == 0) v = 0;
		if(b[i]) v |= (1 << (i & 0x1f));

		if((i % 32) == 31) printf(" %08x",v);
	}

	if((i % 32) != 31) printf(" %08x",v);
	printf("\n");
}

// ---------------------------------------------------

void CMVTrap::SerializeOut(BYTEVECTOR &bv, WORD32 n)
{
	bv.push_back( (WORD_8)(n >> 0) );
	bv.push_back( (WORD_8)(n >> 8) );
	bv.push_back( (WORD_8)(n >>16) );
	bv.push_back( (WORD_8)(n >>24) );
}

void CMVTrap::SerializeOut(BYTEVECTOR &bv, int n)
{
	SerializeOut(bv,(WORD32)n);
}

void CMVTrap::SerializeOut(BYTEVECTOR &bv, FFPOLY &fp)
{
	SerializeOut(bv,fp.pvdeg);

	for(int i=0;i<SCALED_DEGREE;i++) {
		SerializeOut(bv,fp.pv[i]);
	}
}

void CMVTrap::SerializeOut(BYTEVECTOR &bv, EXPOLY &e)
{
	SerializeOut(bv,e.pvdeg);
	SerializeOut(bv,e.pvsize);

	for(int i=0;i<=e.pvdeg;i++) {
		SerializeOut(bv,e.ep[i]);
	}
}

void CMVTrap::SerializeOut(BYTEVECTOR &bv, LNMatrix &L)
{
	BYTEVECTOR tmp;
	int tmpsize = L.GetReqBuffSize();

	SerializeOut(bv,tmpsize);

	tmp.resize( tmpsize );

	L.CompressToBuffer(&tmp[0],tmpsize);

	for(size_t i=0;i<tmp.size();i++) {
		bv.push_back(tmp[i]);
	}
}

void CMVTrap::SerializeOutPublicKey(BYTEVECTOR &bv)
{
	SerializeOut(bv,eqs);
	SerializeOut(bv,vars);
	SerializeOut(bv,psize);
	SerializeOut(bv,(WORD32)pubkey.size());

	for(size_t i=0;i<pubkey.size();i++) {
		SerializeOut(bv,pubkey[i]);	
	}
}

void CMVTrap::SerializeOut(BYTEVECTOR &bv, void *buff, size_t size)
{
	const unsigned char *b = (const unsigned char *)buff;

	for(size_t i=0;i<size;i++) {
		bv.push_back(*b++);
	}
}

int CMVTrap::SerializeIn(CSerializer &b, WORD32 &w)
{
	return b.Get(w);
}

int CMVTrap::SerializeIn(CSerializer &b, WORD_8 &c)
{
	return b.Get(c);
}

int CMVTrap::SerializeIn(CSerializer &b, int &i)
{
	return b.Get(*(WORD32 *)&i);
}

int CMVTrap::SerializeIn(CSerializer &b, FFPOLY &fp)
{
	if(SerializeIn(b,fp.pvdeg) < 1) return 0;

	for(int i=0;i<SCALED_DEGREE;i++) {
		if(SerializeIn(b,fp.pv[i]) < 1) return 0;
	}

	return 1;
}

int CMVTrap::SerializeIn(CSerializer &b, EXPOLY &e)
{
	int deg,sz;

	if(SerializeIn(b,deg) < 1) return 0;
	if(SerializeIn(b,sz) < 1) return 0;

	if(expoly_init(&e,sz) < 1) return 0;

	e.pvdeg = deg;

	for(int i=0;i<=e.pvdeg;i++) {
		if(SerializeIn(b,e.ep[i]) < 1) return 0;
	}

	return 1;
}

int CMVTrap::SerializeIn(CSerializer &b, LNMatrix &L)
{
	int tmpsize;

	if(SerializeIn(b,tmpsize) < 1) return 0;

	if(b.Avail() < tmpsize) return 0;

	if(L.DecompressBuffer(b.GetPtr()) < 1) return 0;

	b.Consume(tmpsize);
	return 1;
}

int CMVTrap::SerializeInPublicKey(CSerializer &b)
{
	int sz;

	if(SerializeIn(b,eqs) < 1) return 0;
	if(SerializeIn(b,vars) < 1) return 0;
	if(SerializeIn(b,psize) < 1) return 0;

	if(SerializeIn(b,sz) < 1) return 0;

	pubkey.clear();
	pubkey.resize(sz);

	for(int i=0;i<sz;i++) {
		if(SerializeIn(b,pubkey[i]) < 1) return 0;	
	}

	return 1;
}

