
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

//#define PRINT_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "quartzver.h"
#include "shs.h"

#define MESSAGEBUFFERSIZE   (8*(QVC_MAXMSGLENGTH))

int count_bits(const int g)
	{
	int bits=0;
	int i;

	for(i=0;i<32;i++)
		if(g & (1 << i)) bits++;
	
	return bits;
	}

int bitvect2bytevect(WORD_8 *bitv,WORD_8 *bytev,int vectsize)
	{
	int i;

	for(i=0;i<vectsize;i++)
		bytev[i] = (((bitv[i>>3] << (i&0x07)) & 0x80) != 0 ? 1 : 0);

	return 1;	
	}

int bytevect2bitvect(WORD_8 *bytev,int bytelen,WORD_8 *bitv)
	{
	int i;

	for(i=0;i<bytelen;i++)
		{
		if((i & 0x07)==0) bitv[i>>3] = 0;
		if(bytev[i]!=0) bitv[i>>3] |= (0x80 >> (i&0x07));
		}
	
	return 1;
	}

int int2vect(int n,WORD_8 *v,int len)
	{
	int i;

	if(len < 0 || len>32) return 0;

	for(i=0;i<len;i++)
		{
		v[i] = n & 1;
		n>>=1;
		}

	return 1;
	}

int QuartzPubKeySetBit(QUARTZ_PUBLICKEY *qpub,int idx,int bit)
	{
	int dw,ofs;

	if(idx < 0 || idx>=QUARTZ_POOL_SIZE) return 0;
	if(bit < 0 || bit>=POLYBITVECT) return 0;	
	
	dw  = bit >> 5;
	ofs = bit & 0x1f;

	qpub->p[idx][dw] |= (0x80000000 >> ofs);
	
	return 1;
	}

int QuartzPubKeyClearBit(QUARTZ_PUBLICKEY *qpub,int idx,int bit)
	{
	int dw,ofs;

	if(idx < 0 || idx>=QUARTZ_POOL_SIZE) return 0;
	if(bit < 0 || bit>=POLYBITVECT) return 0;	
	
	dw  = bit >> 5;
	ofs = bit & 0x1f;

	qpub->p[idx][dw] &= (0xffffffff ^ (0x80000000 >> ofs));
	
	return 1;
	}

int QuartzPubKeyGetBit(QUARTZ_PUBLICKEY *qpub,int idx,int bit,int *val)
	{
	int dw,ofs;

	if(idx < 0 || idx>=QUARTZ_POOL_SIZE) return 0;
	if(bit < 0 || bit>=POLYBITVECT) return 0;	
	
	dw  = bit >> 5;
	ofs = bit & 0x1f;

	*val = ((qpub->p[idx][dw] << ofs) & 0x80000000) != 0 ? 1 : 0;
	return 1;
	}


int QuartzPubKeyReset(QUARTZ_PUBLICKEY *qpub,int idx)
	{
	if(idx < 0 || idx>=QUARTZ_POOL_SIZE) return 0;
	
	memset(qpub->p[idx],0x00,PACKPOLYVECTOR * sizeof(WORD32));
	return 1;
	}

int pubkey_forward(QUARTZ_PUBLICKEY *qpub,WORD_8 *out,WORD_8 *in)
	{
	int count;
	int i,j,k;
	int b1;

	memset(out,0x00,QUARTZ_POOL_SIZE);
	memcpy(out+QUARTZ_POOL_SIZE,in+QUARTZ_POOL_SIZE,7);
	
	count = 0;
	for(i=0;i<QUARTZ_POOL_SIZE;i++)
		{
		QuartzPubKeyGetBit(qpub,i,count,&b1);
		if(b1!=0) out[i] ^= 1;
		}
	count++;

	for(j=0;j<QUARTZ_AFFINE_TRANSSIZE;j++)
		{
		for(i=0;i<QUARTZ_POOL_SIZE;i++)
			{
			QuartzPubKeyGetBit(qpub,i,count,&b1);
			if(b1!=0 && in[j]!=0) out[i] ^= 1;
			}

		count++;
		}

	for(k=0;k<QUARTZ_AFFINE_TRANSSIZE-1;k++)
		for(j=k+1;j<QUARTZ_AFFINE_TRANSSIZE;j++)
			{
			for(i=0;i<QUARTZ_POOL_SIZE;i++)
				{
				QuartzPubKeyGetBit(qpub,i,count,&b1);

				if(b1!=0 && in[j]!=0 && in[k]!=0) out[i] ^= 1;
				}

			count++;
			}

   	if( count != POLYBITVECT) return 0;

	return 1;
	}

int ComputeMessageHash(const WORD_8 *msg,int msgsize,WORD_8 *longhash)
	{
	WORD_8 msgbuf[MESSAGEBUFFERSIZE + 2];
	SHS_INFO sInfo;
	int i;

	if(msgsize <= 0 || msgsize >= MESSAGEBUFFERSIZE) return 0;
	
	/* 
	Computing hashes:
		M1 = SHA(M0 | 0x00 )
		M2 = SHA(M0 | 0x01 )
		M3 = SHA(M0 | 0x02 )
    */

	for(i=0;i<3;i++)
		{
		memcpy(msgbuf,msg,msgsize);
		msgbuf[msgsize] = i;

		shsInit(&sInfo);
		shsUpdate(&sInfo,msgbuf,msgsize + 1);
		shsFinal(&sInfo);

		memcpy(&longhash[ i * SHS_DIGESTSIZE ],sInfo.digest,SHS_DIGESTSIZE);
		}

#ifdef PRINT_DEBUG
	printf("Message hash...\n");
	PrintBuffer(longhash,SHS_DIGESTSIZE * 3);
#endif

	return 1;
	}

int buffer2pubkey(QUARTZ_PUBLICKEY *qpub,WORD_8 *buffer,int bufflen)
	{
	SHS_INFO sInfo;
	WORD32 exp_signature = PUBKEY_BODYSIGNATURE;
	WORD32 exp_version   = PUBKEY_EXPECTED_VERSION;
	int curr=0;
	int i;

	if(!qpub) return -1;

	memset(qpub,0x00,sizeof(QUARTZ_PUBLICKEY));

	qpub->isOk = 0;

	if((curr + (int)sizeof(exp_signature)) > bufflen) return -2;
	if(memcmp(&buffer[curr],&exp_signature,sizeof(exp_signature)) != 0) return -3;
	curr+=sizeof(exp_signature);

	if((curr + (int)sizeof(exp_version)) > bufflen) return -2;
	if(memcmp(&buffer[curr],&exp_version,sizeof(exp_version)) != 0) return -4;
	curr+=sizeof(exp_version);

	for(i=0;i<QUARTZ_POOL_SIZE;i++)
		{
		if((curr + (PACKPOLYVECTOR * 4)) > bufflen) return -2;
		memcpy(qpub->p[i],&buffer[curr],PACKPOLYVECTOR * 4);
		curr += (PACKPOLYVECTOR * 4);
		}

	shsInit(&sInfo);
	shsUpdate(&sInfo,buffer,curr);
	shsFinal(&sInfo);

	if((curr + SHS_DIGESTSIZE) > bufflen) return -2;
	if(memcmp(sInfo.digest,&buffer[curr],SHS_DIGESTSIZE) != 0) return -5;
	curr+=SHS_DIGESTSIZE;

	qpub->isOk=1;
	return 1;
	}

int CheckVariableSizeSignatureString(const char *signature,const int inside,const int outside)
	{
	const int sigsize = QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8;
	int len;
	int i;
	
	if(!signature) return -1;
	if(inside < 0 || inside > QVC_MAXINSIDE || outside < 0) return -2;

	len = (int)strlen(signature);

	if(outside + sigsize != len) return -4;
	if(inside + outside > QVC_MAXMSGLENGTH) return -5;

	for(i=0;i<len;i++)
		if(signature[i] != '1' && signature[i] != '0') return -6;

	return len;
	}

static void QuartzInverseTransform(QUARTZ_PUBLICKEY *qpub,WORD_8 *S,WORD_8 *H,WORD_8 *U)
	{
	WORD_8 Y[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 OT[QUARTZ_AFFINE_TRANSSIZE];
	int i,j;

	memcpy(U,S,QUARTZ_POOL_SIZE);

	for(i=QUARTZ_ROUNDS-1;i>=0;i--)
		{
		for(j=0;j<7;j++)
			U[QUARTZ_POOL_SIZE + j] = S[QUARTZ_POOL_SIZE + i * 7 + j];
		
		pubkey_forward(qpub,Y,U);
		bitvect2bytevect(&H[i*13],OT,QUARTZ_POOL_SIZE);

		for(j=0;j<QUARTZ_POOL_SIZE;j++)
			U[j] = Y[j] ^ OT[j];
		}

	return;
	}

/* This is the core Quartz signature verification routine.
   Notice that msg is actually the hash of the real message and has to be of limited size
   (in theory not greater than 1Kbyte, but in practice much shorter (128-256 bits)) 
*/

int QuartzVerifySignature(const WORD_8 *msg,int msgsize,QUARTZ_PUBLICKEY *qpub,WORD_8 *signature)
	{
	WORD_8 S[QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8];
	WORD_8 U[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 H[3 * SHS_DIGESTSIZE];
	int i;

	if(!qpub || !qpub->isOk) return 0;
	if( ComputeMessageHash(msg,msgsize,H) != 1 ) return 0;

	bitvect2bytevect(signature,S,QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8);	

	QuartzInverseTransform(qpub,S,H,U);

	for(i=0;i<QUARTZ_POOL_SIZE;i++)
		if(U[i] != 0) return 0;
	
	return 1;
	}

int QuartzVerifyVariableCode(QUARTZ_PUBLICKEY *qpub,const char *signature,int inside,int outside,char *bitstring)
	{
	WORD_8 S[QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8];
	WORD_8 U[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 H[3 * SHS_DIGESTSIZE];
	const int sigsize = QUARTZ_SIGNATURE_SIZE * QUARTZ_BITSPERWORD_8;
	int len;
	int i;

	if(!qpub || !qpub->isOk) return 0;

	len = CheckVariableSizeSignatureString(signature,inside,outside);
	if(len < 0) return len;

	if( ComputeMessageHash((unsigned char *)signature,outside,H) != 1 ) return 0;

	for(i=0;i<sigsize;i++)
		S[i] = (signature[i+outside] == '1' ? 1 : 0);

	QuartzInverseTransform(qpub,S,H,U);

	for(i=inside;i<QUARTZ_POOL_SIZE;i++)
		if(U[i] != 0) return 0;

	for(i=0;i<inside;i++)
		bitstring[i] = (U[i] != 0 ? '1' : '0');

	for(i=0;i<outside;i++)
		bitstring[i+inside] = signature[i];

	bitstring[inside+outside] = '\0';
	return 1;
	}
