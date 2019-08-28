
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include <stdio.h>
#include <time.h>

#include "osdepn.h"
#include "mytypes.h"

#ifdef _WIN32

#include <windows.h>

#include "Wincrypt.h"

WORD32 GetChronoTime()
	{
	return GetTickCount();
	}

int GetRandomData(void *data,int size)
	{
	HCRYPTPROV hCryptProv = (HCRYPTPROV)0;
	int rVal = 1;

	if(!CryptAcquireContext(&hCryptProv,NULL,NULL,PROV_RSA_FULL,CRYPT_NEWKEYSET | CRYPT_VERIFYCONTEXT)) 
		return -1;
	
	if(!CryptGenRandom(hCryptProv,size,(BYTE *)data)) rVal = -2;
	CryptReleaseContext(hCryptProv,0);

	return rVal;
	}

#else	/* Should at least work under LINUX */

#include <sys/time.h>
#include "arc4.h"
#include <termios.h>
#include <unistd.h>

WORD32 GetChronoTime()
	{
	static struct timeval _mytime;
    	static struct timezone tz;

	gettimeofday(&_mytime, &tz);
	return (WORD32)(_mytime.tv_sec * 1000 + _mytime.tv_usec / 1000);
	}

#define SMALLBUFF  40
int GetRandomData(void *data,int size)
	{
	unsigned char *b = (unsigned char *)data;
	char buff[SMALLBUFF];
	RC4_CTX rc4;
	FILE *h1;
	int i;

	h1=fopen("/dev/random","rb");
	if(!h1) return 0;

	fread(buff,1,SMALLBUFF,h1);
	fclose(h1);

	InitRC4(&rc4,buff,SMALLBUFF);

	for(i=0;i<size;i++)
		b[i] = RC4_GetByte(&rc4);

	return 1;
	}

int GetLastError()
	{
	return 1;
	}

#define _unlink unlink


#endif
