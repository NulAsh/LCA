/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef QUARTZ_SIGNATURE_VERIFIER_H
#define QUARTZ_SIGNATURE_VERIFIER_H

#include "mytypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QUARTZ_POLY_DEGREE    103
//#define QUARTZ_POLYEXT_DEGREE 129

#define QVC_MAXINSIDE        36
#define QVC_MAXMSGLENGTH   1024

#define QUARTZ_AFFINE_TRANSSIZE  107
#define QUARTZ_POOL_SIZE         100
#define QUARTZ_SIGNATURE_SIZE     16
#define QUARTZ_ROUNDS              4
#define QUARTZ_BITSPERWORD_8         8

#define QUARTZ_HASH_SKEW         ((QUARTZ_POOL_SIZE + QUARTZ_BITSPERWORD_8 - 1) / QUARTZ_BITSPERWORD_8)

#define PUBKEY_BODYSIGNATURE     0x45424733
#define PUBKEY_EXPECTED_VERSION  0x00010001

#define POLYSIZE(n)   (((n)*(n) + (n) + 2)/2)
#define POLYBITVECT   POLYSIZE(QUARTZ_AFFINE_TRANSSIZE)

#define PACKPOLYVECTOR  ((POLYBITVECT + 31)/32)
#define PACKPOLYWORD_8    (4 * PACKPOLYVECTOR)

typedef struct quartz_pubkey {
	WORD32 p[QUARTZ_POOL_SIZE][PACKPOLYVECTOR];
	int isOk;
	} QUARTZ_PUBLICKEY;

#define EXPECTED_PUBKEYLEN   72428

int int2vect(int n,WORD_8 *v,int len);
int bitvect2bytevect(WORD_8 *bitv,WORD_8 *bytev,int vectsize);
int bytevect2bitvect(WORD_8 *bytev,int bytelen,WORD_8 *bitv);

int pubkey_forward(QUARTZ_PUBLICKEY *qpub,WORD_8 *out,WORD_8 *in);

int QuartzPubKeyGetBit(QUARTZ_PUBLICKEY *qpub,int idx,int bit,int *val);
int QuartzPubKeySetBit(QUARTZ_PUBLICKEY *qpub,int idx,int bit);
int QuartzPubKeyClearBit(QUARTZ_PUBLICKEY *qpub,int idx,int bit);
int QuartzPubKeyReset(QUARTZ_PUBLICKEY *qpub,int idx);
	
int ComputeMessageHash(const WORD_8 *msg,int msgsize,WORD_8 *longhash);	

/* ********************************************************************************** */

int count_bits(const int g);
int buffer2pubkey(QUARTZ_PUBLICKEY *qpub,WORD_8 *buffer,int bufflen);
int QuartzVerifySignature(const WORD_8 *msg,int msgsize,QUARTZ_PUBLICKEY *qpub,WORD_8 *signature);
int QuartzVerifyVariableCode(QUARTZ_PUBLICKEY *qpub,const char *signature,int inside,int outside,char *bitstring);

/* ********************************************************************************** */

#ifdef __cplusplus
	}
#endif


#endif
