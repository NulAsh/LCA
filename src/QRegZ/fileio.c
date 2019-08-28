
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "quartzsig.h"
#include "fileio.h"
#include "pool.h"
#include "userio.h"

#define MAXKEYSIZE 256

const char *rndfilename = "random.bin";

static int exists(const char *filename)
	{
	FILE *h1 = fopen(filename,"rb");
	if(!h1) return 0;
	fclose(h1);
	return 1;
	}

static int HandleRandPool(int ac,char **av)
	{
	int nRes;

	if(ac > 1) 
		{
		printf("?Bad args\n");
		return 0;
		}
	
	if(ac == 1) nRes = InitPool(rndfilename,(unsigned char *)av[0],(int)strlen(av[0]));
	else nRes = InitPool(rndfilename,NULL,0);

	if(nRes < 0)
		{
		printf("?Error opening file %s\n",rndfilename);
		return 0;
		}
	else if(nRes == 0)
		{
		printf("?Random pool file %s not found. ",rndfilename);
		printf("Reseed the generator first\n");
		return 0;
		}

	return 1;
	}

static int DeHandleRandPool()
	{
	if( DeInitPool(rndfilename) < 1)
		{
		printf("Warning: cannot update file %s\n",rndfilename);
		return 0;
		}

	return 1;
	}

static int WritePrivateKeyToDisk(const char *filename,QUARTZ_PRIVKEY *qpk,const char *password,int encrypt)
	{
	HEADERKEY header;
	int isOk=1;
	WORD_8 *memblock = NULL;
	int len=0;
	unsigned int size;
	FILE *h1;

	printf("Writing private key file %s\n",filename);
	h1 = fopen(filename,"wb");
	if(!h1)
		{
		printf("?Cannot write output file %s\n",filename);
		return 0;
		}

	privkey2buffer(qpk,NULL,&len);
		
	memblock = (WORD_8 *)malloc( len );
	if(!memblock) 
		{
		printf("?Cannot allocate byte stream buffer for private key\n");
		fclose(h1);
		_unlink(filename);
		return 0;
		}
	
	size = privkey2buffer(qpk,memblock,&len);
	if(size <= 0)
		{
		printf("?Cannot convert private key to byte stream\n");
		fclose(h1);
		_unlink(filename);
		return 0;
		}

	memcpy(header.headstr,headerkey,sizeof(header.headstr));
	header.keytype   = ID_PRIVKEY;
	header.version   = VERSION_KEY;
	header.encrypted = (encrypt ? 1 : 0);
	header.deg       = qpk->fv.deg;
	header.pow       = qpk->fv.pow;

	if(isOk) isOk=(fwrite(&header,1,sizeof(header),h1) == sizeof(header));

	if(header.encrypted)
		{
		ENCRYPTEDBLOCK eb;
		SHS_INFO sInfo;
		RC4_CTX  rc4;
		int i;

		eb.chunk = ID_CRYPTOMATERIAL;

		shsInit(&sInfo);
		shsUpdate(&sInfo,(unsigned char *)password,(int)strlen(password));
		shsFinal(&sInfo);

		InitRC4(&rc4,(char *)sInfo.digest,SHS_BLOCKSIZE);

		shsInit(&sInfo);
		shsUpdate(&sInfo,memblock,size);
		shsFinal(&sInfo);

		memcpy(eb.digest,sInfo.digest,SHS_BLOCKSIZE);

		for(i=0;i<256;i++)
			RC4_GetByte(&rc4);

		for(i=0;i<(int)size;i++)
			memblock[i] ^= RC4_GetByte(&rc4);

		if(isOk) isOk=(fwrite(&eb,1,sizeof(eb),h1) == sizeof(eb));	
		}

	if(isOk) isOk=(fwrite(&size,1,sizeof(size),h1) == sizeof(size));
	if(isOk) isOk=(fwrite(memblock,1,size,h1) == size);

	if(!isOk)
		{
		printf("?I/O error during disk write\n");
		fclose(h1);
		_unlink(filename);
		return 0;
		}
	
	free(memblock);
	if( fclose(h1) != 0 )
		{
		printf("?I/O error during file close\n");
		_unlink(filename);
		return 0;
		}

	return 1;
	}

static int WritePublicKeyToDisk(const char *filename,QUARTZ_PUBLICKEY *qpub)
	{
	HEADERKEY header;
	int isOk=1;
	WORD_8 *memblock = NULL;
	int len=0;
	unsigned int size;
	FILE *h1;

	printf("Writing public key file  %s\n",filename);
	h1 = fopen(filename,"wb");
	if(!h1)
		{
		printf("?Cannot write output file %s\n",filename);
		return 0;
		}

	pubkey2buffer(qpub,NULL,&len);
		
	memblock = (WORD_8 *)malloc( len );
	if(!memblock) 
		{
		printf("?Cannot allocate byte stream buffer for public key\n");
		fclose(h1);
		_unlink(filename);
		return 0;
		}
	
	size = pubkey2buffer(qpub,memblock,&len);
	if(size <= 0)
		{
		printf("?Cannot convert public key to byte stream\n");
		fclose(h1);
		_unlink(filename);
		return 0;
		}

	memcpy(header.headstr,headerkey,sizeof(header.headstr));
	header.keytype   = ID_PUBKEY;
	header.version   = VERSION_KEY;
	header.encrypted = 0;
	header.deg       = 0;	// Do not reveal the degree of the private polynomial
	header.pow       = 0;

	if(isOk) isOk=(fwrite(&header,1,sizeof(header),h1) == sizeof(header));
	if(isOk) isOk=(fwrite(&size,1,sizeof(size),h1) == sizeof(size));
	if(isOk) isOk=(fwrite(memblock,1,size,h1) == size);

	if(!isOk)
		{
		printf("?I/O error during disk write\n");
		fclose(h1);
		_unlink(filename);
		return 0;
		}
	
	free(memblock);
	if( fclose(h1) != 0 )
		{
		printf("?I/O error during file close\n");
		_unlink(filename);
		return 0;
		}

	return 1;
	}

/* ************************************************************************* */
int ReadPrivateKeyHeader(FILE *h1,HEADERKEY *header)
	{
	int isOk=1;

	fseek(h1,0,SEEK_SET);
	
	if(isOk) isOk=(fread(header,1,sizeof(HEADERKEY),h1)==sizeof(HEADERKEY));
	if(!isOk)
		{
		printf("?File header not found.\n");
		fclose(h1);
		return 0;
		}

	if(memcmp(header->headstr,headerkey,sizeof(header->headstr))!=0)
		{
		printf("?File signature not found.\n");
		fclose(h1);
		return 0;
		}

	if(header->keytype != ID_PRIVKEY)
		{
		printf("?Invalid key type.\n");
		fclose(h1);
		return 0;
		}

	if(header->version != VERSION_KEY)
		{
		printf("?Invalid key version.\n");
		fclose(h1);
		return 0;
		}

	return 1;
	}

int ProbePrivateKeyHeaderFromDisk(const char *filename,HEADERKEY *header)
	{
	FILE *h1;
	int nRes;

	h1 = fopen(filename,"rb");
	if(!h1) 
		{
		printf("?Cannot read file...\n");
		return 0;
		}
	
	nRes = ReadPrivateKeyHeader(h1,header);
	if(nRes < 1) return nRes;

	if(header->pow < D_MINPOLYFIGURE || header->pow > D_MAXPOLYFIGURE) return 0;
	if(header->deg != ((WORD32)(1 << header->pow) + 1)) return 0;

	if( fclose(h1) != 0)
		{
		printf("?I/O error during file close\n");
		return 0;
		}

	return 1;
	}

int ReadPrivateKeyFromDisk(const char *filename,QUARTZ_PRIVKEY *qpk)
	{
	HEADERKEY header;
	ENCRYPTEDBLOCK eb;
	WORD_8 *memblock = NULL;
	unsigned int size;
	FILE *h1;
	int nRes;

	printf("Reading private key file %s\n",filename);

	h1 = fopen(filename,"rb");
	if(!h1) 
		{
		printf("?Cannot read file...\n");
		return 0;
		}

	nRes = ReadPrivateKeyHeader(h1,&header);
	if(nRes < 1) return nRes;

	if(header.encrypted != 0)
		{
		if(fread(&eb,1,sizeof(eb),h1) != sizeof(eb))
			{
			printf("?Corrupted encryption chunk\n");
			fclose(h1);
			return 0;
			}

		if(eb.chunk != ID_CRYPTOMATERIAL)
			{
			printf("?Invalid encryption chunk\n");
			fclose(h1);
			return 0;
			}
		}

	if(fread(&size,1,sizeof(size),h1)!=sizeof(size))
		{
		printf("?Corrupted data chunk\n");
		fclose(h1);
		return 0;
		}
	
	if(size > MAXPRIVKEYSIZE)
		{
		printf("?Invalid key size\n");
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
		printf("?I/O error during file close\n");
		return 0;
		}

	if(header.encrypted)
		{
		RC4_CTX rc4;
		SHS_INFO sInfo;
		char password[MAXPASSSIZE];
		int i;

		printf("Password: ");
		fflush(stdout);
		
		nRes = GetUserPassword(password,MAXPASSSIZE);
		printf("\n");
		
		if(nRes < 1)
			{
			printf("?User abort.\n");
			free(memblock);
			return 0;
			}

		shsInit(&sInfo);
		shsUpdate(&sInfo,(unsigned char *)password,(int)strlen(password));
		shsFinal(&sInfo);

		InitRC4(&rc4,(char *)sInfo.digest,SHS_BLOCKSIZE);

		for(i=0;i<256;i++)
			RC4_GetByte(&rc4);

		for(i=0;i<(int)size;i++)
			memblock[i] ^= RC4_GetByte(&rc4);

		shsInit(&sInfo);
		shsUpdate(&sInfo,memblock,size);
		shsFinal(&sInfo);

		if(memcmp(&sInfo.digest,eb.digest,SHS_BLOCKSIZE) != 0)
			{
			printf("?Wrong password supplied.\n");
			free(memblock);
			return 0;
			}
		}

	nRes = buffer2privkey(memblock,(int)size,qpk);
	free(memblock);
	
	switch( nRes )
		{
		case -1:
			printf("?Implementation error\n");
			return 0;
		
		case -2:
			printf("?Private key too short\n");
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


int ReSeed_CSPRNG(int ac,char **av)
	{
	int nRes;

	if(ac < 1)
		{
		printf("?Bad args.\n");
		return 0;
		}

	nRes = HandleRandPool(ac,av);
	if(nRes < 0) return nRes;
	
	nRes = DeHandleRandPool();
	if(nRes == 1) printf("Random number generator reseeded.\n");

	return nRes;
	}

int PromptPasswordAndSavePrivateKey(const char *filename,QUARTZ_PRIVKEY *qpk)
	{
	char password[MAXPASSSIZE];
	char confirm_password[MAXPASSSIZE];
	int nRes;

	printf("\n");
	printf("If you do not wish to encrypt your private key, just hit ESC.\n");

	do
		{
		printf("Password: ");
		fflush(stdout);
		
		nRes = GetUserPassword(password,MAXPASSSIZE);
		printf("\n");

		if(nRes == 1) 
			{
			printf("Confirm : ");
			fflush(stdout);
			nRes = GetUserPassword(confirm_password,MAXPASSSIZE);
			printf("\n");
			}

		if(nRes == 1)
			{
			if(strcmp(password,confirm_password) != 0)
				{
				printf("?Passwords do not match\n");
				nRes = -2;
				}
			}

		if(nRes == -1) 
			{
			printf("?Aborted on user request\n");
			return 0;
			}
		} while(nRes == -2);

	printf("\n");

	nRes = WritePrivateKeyToDisk(filename,qpk,password,nRes);
	return nRes;
	}
	
int CreateQuartzKey(int ac,char **av)
	{
	RC4_CTX rc4;
	QUARTZ_PRIVKEY qpk;
	QUARTZ_PUBLICKEY *qpub;
	unsigned char tmpbuff[MAXKEYSIZE];
	char str[_MAX_PATH];
	int nRes;
	int level;
	int maxdeg;
	
	if(ac < 2) 
		{
		printf("?Bad args\n");
		return 0;
		}

	level = atoi(av[0]);
	if(level < 1 || level > 3) 
		{
		printf("?Bad security level parameter (must be between 1 and 3)\n");
		return 0;
		}

	maxdeg = 6 + level;

	if(strlen(av[1]) > _MAX_PATH - 5)
		{
		printf("?Filename too long.\n");
		return 0;
		}

	nRes = HandleRandPool(0,NULL);
	if(nRes < 1) return nRes;

	GetPoolRnd(tmpbuff,MAXKEYSIZE);
	InitRC4(&rc4,(char *)tmpbuff,MAXKEYSIZE);

	qpub = (QUARTZ_PUBLICKEY *)malloc( sizeof(QUARTZ_PUBLICKEY) );
	if(!qpub)
		{
		printf("?Memory allocation failure\n");
		return 0;
		}

	if( init_priv_key(&qpk,maxdeg)!=1 ) 
		{
		printf("?Memory allocation failure\n");
		free(qpub);
		return 0;
		}

	printf("Creating private key (security level=%d, polydegree=%d)...\n",level,qpk.fv.deg);
	if( create_priv_key(&rc4,&qpk) != 1 )
		{
		printf("?Private key creation failed.\n");
		
		free_priv_key(&qpk);
		free(qpub);
		return 0;
		}

	printf("Deriving public key from private key...\n");
	if( create_public_from_private(&qpk,qpub) != 1 )
		{
		printf("?Public key derivation process failed.\n");
		
		free_priv_key(&qpk);
		free(qpub);
		return 0;
		}

	strcpy(str,av[1]);
	strcat(str,".prv");

	nRes = PromptPasswordAndSavePrivateKey(str,&qpk);
	if(nRes < 1) 
		{
		free_priv_key(&qpk);
		free(qpub);
		return nRes;
		}
		
	strcpy(str,av[1]);
	strcat(str,".pub");
	
	nRes = WritePublicKeyToDisk(str,qpub);
	if(nRes < 1)
		{
		free_priv_key(&qpk);
		free(qpub);
		return nRes;
		}

	// Continue here...
	return DeHandleRandPool();
	}
	
	
int ChangePassword(int ac,char **av)
	{
	QUARTZ_PRIVKEY qpk;
	HEADERKEY header;
	char str[_MAX_PATH];
	char bak[_MAX_PATH];
	int nRes;

	if(ac < 1)
		{
		printf("?Bad args\n");
		return 0;
		}

	if(strlen(av[0]) > _MAX_PATH - 5)
		{
		printf("?Filename too long\n");
		return 0;
		}

	strcpy(str,av[0]);
	strcat(str,".prv");

	strcpy(bak,av[0]);
	strcat(bak,".bak");

	if( ProbePrivateKeyHeaderFromDisk(str,&header) != 1 )
		{
		// Message already shown...
		return 0;
		}

	if( init_priv_key(&qpk,header.pow) != 1 )
		{
		printf("?Error initializing private key\n");
		return 0;
		}

	if( ReadPrivateKeyFromDisk(str,&qpk) != 1) 
		return 0;	// Error already shown

	if(exists(bak)==1)
		{
		if(_unlink(bak) != 0)
			{
			printf("?Cannot delete previous private key backup.\n");
			return 0;
			}
		}

	if(rename(str,bak) != 0)
		{
		printf("?Cannot rename %s to %s\n",str,bak);
		return 0;
		}

	nRes = PromptPasswordAndSavePrivateKey(str,&qpk);
	free_priv_key(&qpk);

	if(nRes == 1) 
		{
		printf("Done\n");
		_unlink(bak);
		}

	return nRes;
	}

int DerivePublicFromPrivate(int ac,char **av)
	{
	QUARTZ_PRIVKEY qpk;
	QUARTZ_PUBLICKEY *qpub;
	HEADERKEY header;
	char str[_MAX_PATH];
	int nRes;

	if(ac < 1)
		{
		printf("?Bad args\n");
		return 0;
		}

	if(strlen(av[0]) > _MAX_PATH - 5)
		{
		printf("?Filename too long\n");
		return 0;
		}

	strcpy(str,av[0]);
	strcat(str,".prv");

	if( ProbePrivateKeyHeaderFromDisk(str,&header) != 1) 
		{
		// Message already shown
		return 0;
		}

	if( init_priv_key(&qpk,header.pow) != 1 )
		{
		printf("?Error initializing private key\n");
		return 0;
		}
	
	if( ReadPrivateKeyFromDisk(str,&qpk) != 1 ) 
		return 0;	// Error already shown

	qpub = (QUARTZ_PUBLICKEY *)malloc(sizeof(QUARTZ_PUBLICKEY));
	if(!qpub)
		{
		printf("?Troubles allocating memory for public key\n");
		free_priv_key(&qpk);
		return 0;
		}

	printf("Deriving public key from private key...\n");
	if( create_public_from_private(&qpk,qpub) != 1 )
		{
		printf("?Implementation error deriving public key\n");
		free(qpub);
		free_priv_key(&qpk);
		return 0;
		}

	strcpy(str,av[0]);
	strcat(str,".pub");

	nRes = WritePublicKeyToDisk(str,qpub);

	free(qpub);
	free_priv_key(&qpk);

	if(nRes == 1) printf("Done.\n");

	return nRes;
	}
