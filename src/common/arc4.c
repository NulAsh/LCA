
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include "arc4.h"

void InitRC4(RC4_CTX *rc4,const char *key,int keylen)
	{
	rc4->i=0;
	rc4->j=0;
	
	for(rc4->i=0;rc4->i<256;rc4->i++)
		rc4->S[rc4->i] = rc4->i;

	for(rc4->i=0;rc4->i<256;rc4->i++)
		{
		unsigned char tmp;

		rc4->j = ( rc4->j + rc4->S[rc4->i] + key[rc4->i % keylen] ) & 0xff;

		tmp = rc4->S[rc4->i];
		rc4->S[rc4->i] = rc4->S[rc4->j];
		rc4->S[rc4->j] = tmp;
		}
	
	rc4->i = 0;
	rc4->j = 0;
	}

int RC4_GetByte(RC4_CTX *rc4)
	{
	unsigned char tmp;

	rc4->i = (rc4->i + 1) & 0xff;
	rc4->j = (rc4->S[rc4->i] + rc4->j) & 0xff;

	tmp = rc4->S[rc4->i];
	rc4->S[rc4->i] = rc4->S[rc4->j];
	rc4->S[rc4->j] = tmp;

	return (rc4->S[rc4->i] + rc4->S[rc4->j]) & 0xff;
	}


unsigned int RC4_GetLong(RC4_CTX *rc4)
	{
	unsigned int rv = 0;

	rv |= RC4_GetByte(rc4);
	rv <<=8;
	rv |= RC4_GetByte(rc4);
	rv <<=8;
	rv |= RC4_GetByte(rc4);
	rv <<=8;
	rv |= RC4_GetByte(rc4); 

	return rv;
	}

int GenerateBlock(RC4_CTX *rc4,char *block,const int block_len)
	{
	int i;

	for(i=0;i<block_len;i++)
		block[i] = RC4_GetByte(rc4);
	
	return block_len;
	}
