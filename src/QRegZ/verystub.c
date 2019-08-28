
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
#include <string.h>

#include "fileio.h"
#include "decoder.h"
#include "verystub.h"

const char *headerkey   = "GBQUARTZ";	/* Please do not modify... :-) */

/* This module is the first stage of the license code verification scheme. 
   Its job is to load the public key from disk and call the routines which
   verify the license code.

   It has some printing capabilities which may be overridden by custom
   error display and handling routines.
*/

int ReadPublicKeyFromDisk(const char *filename,QUARTZ_PUBLICKEY *qpub)
	{
	HEADERKEY header;
	int isOk=1;
	WORD_8 *memblock = NULL;
	unsigned int size;
	int nRes;
	FILE *h1;

	printf("Reading public key file %s\n",filename);
	
	h1 = fopen(filename,"rb");
	if(!h1)
		{
		printf("?Cannot read file.\n");
		return 0;
		}

	if(isOk) isOk=(fread(&header,1,sizeof(header),h1)==sizeof(header));
	if(!isOk)
		{
		printf("?File header not found.\n");
		fclose(h1);
		return 0;
		}

	if(memcmp(header.headstr,headerkey,sizeof(header.headstr))!=0)
		{
		printf("?File signature not found.\n");
		fclose(h1);
		return 0;
		}

	if(header.keytype != ID_PUBKEY)
		{
		printf("?Invalid key type.\n");
		fclose(h1);
		return 0;
		}

	if(header.version != VERSION_KEY)
		{
		printf("?Invalid key version.\n");
		fclose(h1);
		return 0;
		}

	if(header.encrypted)
		{
		printf("?Unsupported encrypted public key.\n");
		fclose(h1);
		return 0;
		}

	if(fread(&size,1,sizeof(size),h1)!=sizeof(size))
		{
		printf("?Corrupted data chunk\n");
		fclose(h1);
		return 0;
		}
	
	if(size > MAXPUBKEYSIZE)
		{
		printf("?Invalid public key size\n");
		fclose(h1);
		return 0;
		}

	memblock = (WORD_8 *)malloc( size );
	if(!memblock)
		{
		printf("?Memory allocation problem\n");
		fclose(h1);
		return 0;
		}

	if(fread(memblock,1,size,h1) != size)
		{
		printf("?Corrupted key data\n");
		fclose(h1);
		free(memblock);
		return 0;
		}

	if( fclose(h1) != 0 )
		{
		printf("?Problems during file closure\n");
		free(memblock);
		return 0;
		}

	nRes = buffer2pubkey(qpub,memblock,(int)size);
	free(memblock);

	switch( nRes )
		{
		case -1:
			printf("?Implementation error\n");
			return 0;
		
		case -2:
			printf("?Public key too short\n");
			return 0;
		
		case -3:
			printf("?Key subsignature not found\n");
			return 0;
		
		case -4:
			printf("?Key subversion mismatch\n");
			return 0;
		
		case -5:
			printf("?Key hash mismatch\n");
			return 0;
		}

	return 1;
	}

int VerfityLicenseCode(int ac,char **av)
	{
	QUARTZ_PUBLICKEY *qpub;
	char QuartzCode[100];
	char str[_MAX_PATH];
	WORD32 id;
	int nRes=0;

	if(ac != 2)
		{
		printf("?Bad args.\n");
		return 0;
		}
	
	if(strlen(av[0]) > _MAX_PATH - 5)
		{
		printf("?Filename too long\n");
		return 0;
		}

	printf("Verifying code: %s\n",av[1]);

	qpub = (QUARTZ_PUBLICKEY *)malloc( sizeof(QUARTZ_PUBLICKEY) );
	if(!qpub)
		{
		printf("?Error allocating memory for public key\n");
		return 0;
		}

	strcpy(str,av[0]);
	strcat(str,".pub");

	if( ReadPublicKeyFromDisk(str,qpub) != 1 ) 
		{
		free(qpub);
		return 0;
		}

	/* Remove dashes '-' from license code */
	if( UndecorateQuartzCode(av[1],QuartzCode) != 1 )
		{
		printf("?Invalid code format\n");
		free(qpub);
		return 0;
		}

	/* Ok, that's where we enter number crunching routines. Probably you may wish
	   to call this directly from your code, taking care of loading the key on your own. */
	nRes = TestSingleCode(qpub,&id,QuartzCode);

	free(qpub);
	printf("\n");

	if(nRes == 1)
		{
		printf("Accepted [ID=%u]\n",id);
		return 1;
		}
	else
		{
		printf("Rejected\n");
		return 2;
		}
	}
