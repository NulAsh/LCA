/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef ARC_FOUR_H
#define ARC_FOUR_H

#ifdef __cplusplus
extern "C"
	{
#endif

typedef struct
	{
	unsigned char S[256];
	int i,j;
	} RC4_CTX;

void InitRC4(RC4_CTX *rc4,const char *key,int keylen);
int RC4_GetByte(RC4_CTX *rc4);
unsigned int RC4_GetLong(RC4_CTX *rc4);	

int RC4_GenerateBlock(RC4_CTX *rc4,char *block,const int block_len);


#ifdef __cplusplus
	}
#endif

#endif	// ARC_FOUR_H
