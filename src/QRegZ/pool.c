
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#include "osdepn.h"

#include "arc4.h"
#include "shs.h"

#define KEYSIZE  256
#define POOLSIZE 4096

static char pool[POOLSIZE];
static int pIn     = 0;
static int pOut    = 0;

static void StirPool()
	{
	RC4_CTX rc4;
	int i;

	InitRC4(&rc4,pool,KEYSIZE);

	for(i=0;i<256;i++)	/* Discard first 256 bytes */
		RC4_GetByte(&rc4);

	for(i=0;i<POOLSIZE;i++)
		pool[i] ^= RC4_GetByte(&rc4);

	pIn  = 0;
	pOut = 0;
	}

void UpdatePool(const unsigned char *_pool,int _poolsize)
	{
	int i,t,ofs=0;

	while(_poolsize > (t=(POOLSIZE - pIn)))
		{
		for(i=0;i<t;i++)
			pool[pIn + i] ^= _pool[ofs+i];

		ofs+=t;
		_poolsize-=t;

		pIn += _poolsize;

		StirPool();
		}

	if(_poolsize > 0)
		{
		StirPool();	

		pIn += _poolsize;
		pOut = POOLSIZE;
		}
	}

void GetPoolRnd(unsigned char *block,int size)
	{
	int t;

	while( size > (t = (POOLSIZE - pOut)))
		{
		memcpy(block,&pool[pOut],t);
		size-=t;
		block += t;
		StirPool();
		}
	
	if( size > 0 )
		{
		memcpy(block,&pool[pOut],size);
		pOut+=size;
		}
	}

int InitPool(const char *filename,unsigned char *_pool,int _poolsize)
	{
	FILE *h1;
	WORD32 rnd = GetChronoTime();

	memset(pool,0,POOLSIZE);

	pIn  = 0;
	pOut = 0;

	h1 = fopen(filename,"rb");
	if(h1) 
		{
		if( fread(pool,1,POOLSIZE,h1) != POOLSIZE ) return -1;
		if( fclose(h1) != 0) return -1;
		}
	else if(_pool==NULL || _poolsize <= 0) return 0;

	if(_pool && _poolsize > 0) 
		{
		SHS_INFO sInfo;

		shsInit(&sInfo);
		shsUpdate(&sInfo,_pool,_poolsize);
		shsFinal(&sInfo);

		UpdatePool((const unsigned char *)sInfo.digest,SHS_BLOCKSIZE);
		}

	memcpy(pool,&rnd,sizeof(rnd));

	StirPool();
	return 1;
	}

int DeInitPool(const char *filename)
	{
	FILE *h1 = fopen(filename,"wb");
	if(!h1) return 0;

	if( fwrite(pool,1,POOLSIZE,h1) != POOLSIZE ) return -2;
	if( fclose(h1) != 0 ) return -2;
	return 1;
	}
