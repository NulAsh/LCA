/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// DRegZ.cpp: implementation of the CDRegZ class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "DRegZ.h"

#include <time.h>

#define DREGZ_VERSION_ID        0x00010001

// DRGZ
#define DREGZ_START_SIGNATURE	0x5A475244

// DEND
#define DREGZ_END_SIGNATURE		0x444E4544

//////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDRegZ::CDRegZ()
{
	kp = NULL;

	SetSize(120);
	SetSecurityLevel(1);
	isok = 0;
}

CDRegZ::~CDRegZ()
{
	Reset();
}

int CDRegZ::SetSecurityLevel(int level)
{
	if(level != 1) return 0;

	isok = 0;
	return 1;
}

bool CDRegZ::SetSize(int size)
{
	if(size < DREGZ_MINCODESIZE || size > DREGZ_MAXCODESIZE) {
		return false;
	}
	
	codesize = size;
	isok = 0;

	return true;
}

int CDRegZ::GetStatus()
{
	return isok != 0 ? 1 : 0;
}

int CDRegZ::GenerateKeyPair()
{
	isok = 0;

	// private key!
	delete kp;
	kp = new CKeyPair();

	int rr = kp->CreateKeyPair(0);
	if(rr <= 0) {
		Reset();
		return -1;
	}

	isok = 1;

	PubKeyReset(kp->GetEqsNo(),kp->GetVarsNo());
	rr = DerivePublicKey();
	if(rr != 1) {
		isok = 0;
		return 0;
	}

	return 1;
}

int CDRegZ::SaveToBuffer(BYTEVECTOR &bv)
{
	if(isok < 1) return -1;

	WORD32 signature = DREGZ_START_SIGNATURE;
	WORD32 version   = DREGZ_VERSION_ID;

	SerializeOut(bv, signature);
	SerializeOut(bv, version);

	SerializeOut(bv, codesize);
	
	// flush out here...
	BYTEVECTOR tb;

	int expsize = kp->GetReqBuffSize(CKeyPair::ALLKEYS);
	tb.resize(expsize);

	int rr = kp->CompressToBuffer(CKeyPair::ALLKEYS,&tb[0],expsize);
	if(rr < 0) return -2;

	if(expsize != (int)tb.size()) return -3;

	SerializeOut(bv, expsize);
	bv.insert(bv.end(),tb.begin(),tb.end());

	SerializeOutPublicKey(bv);
	SerializeOut(bv, DREGZ_END_SIGNATURE);
	return 1;
}

int CDRegZ::LoadFromBuffer(const BYTEVECTOR &bv)
{
	WORD32 signature = DREGZ_START_SIGNATURE;
	WORD32 version   = DREGZ_VERSION_ID;
	WORD32 cmpsig,cmpver,endsign;

	CSerializer ser((unsigned char *)&bv[0],(int)bv.size());

	if(SerializeIn(ser, cmpsig) < 1) return 0;
	if(cmpsig != signature) return -1;
	
	if(SerializeIn(ser, cmpver) < 1) return 0;
	if(cmpver > version) return -2;

	if(SerializeIn(ser, codesize) < 1) return 0;

	// flush in here...
	WORD32 keysize;
	if(SerializeIn(ser, keysize) < 1) return 0;

	delete kp;
	kp = new CKeyPair();

	BYTEVECTOR pk;
	if(ser.ReadBytes(pk,keysize) != (int)keysize) return 0;

	int rr = kp->DecompressBuffer(CKeyPair::ALLKEYS,&pk[0]);
	if(rr != (int)pk.size()) return 0;

	SerializeInPublicKey(ser);

	if(SerializeIn(ser, endsign) < 1) return 0;
	if(endsign != DREGZ_END_SIGNATURE) return -4;

	isok = 1;
	return 1;
}

int CDRegZ::PrivateForward(const BOOLVECTOR &in, BOOLVECTOR &out)
{
	if(isok < 1) return -2;
	if(!kp) return -3;

	int eqs = kp->GetEqsNo();
	int varsno = kp->GetVarsNo();

	if((int)in.size() != GetExpectedForwardInputBitsNo() ||
		varsno != GetExpectedForwardInputBitsNo()) return -4;

	ELEMENT inbuff[MAXMTXSIZE],outbuff[MAXMTXSIZE];

	for(int i=0;i<varsno;i++) {
		inbuff[i] = in[i] ? 1 : 0;
	}

	int rr = kp->fkey.Evaluate(inbuff,varsno,outbuff,eqs);
	if(rr != 1) return -5;

	out.clear();

	for(int j=0;j<eqs;j++) {
		out.push_back( outbuff[j] != 0 );
	}

	return 1;
}

int CDRegZ::PrivateBackward(const BOOLVECTOR &in, BOOLVECTOR &out)
{
	if(isok < 1) return -2;
	if(!kp) return -3;

	int eqs = kp->GetEqsNo();
	int varsno = kp->GetVarsNo();

	if((int)in.size() != eqs) return -4;

	ELEMENT inbuff[MAXMTXSIZE],outbuff[MAXMTXSIZE];

	for(int i=0;i<eqs;i++) {
		inbuff[i] = in[i] ? 1 : 0;
	}

	int rr = kp->Sign(inbuff,eqs,outbuff,varsno);
	if(rr != 1) return 0;

	out.clear();

	for(int j=0;j<varsno-3;j++) {
		out.push_back( outbuff[j] != 0 );
	}

	return 1;
}

// ======================================================================

void CDRegZ::Reset()
{
	delete kp;
	kp = NULL;
}


