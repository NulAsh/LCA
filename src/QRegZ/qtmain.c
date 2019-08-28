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
#include <malloc.h>

#include "quartzsig.h"
#include "quartzver.h"

/* This code shows how to create and verify a signature with the Quartz algorithm, and how
   to import/export private/public keys in a memory buffer. */
   
/* There are some slight differences between this implementation and the specs suppied in
   the official Quartz paper, so do not expect this to be compliant with the original
   version. For example some bits extracted to the SHA-1 digest are different. */   
   
int main()
	{
	const char *message = "TheQuickBrownFoxJumpsOverTheLazyDog";
	const char *rc4_key = "123456789ABCDEF";
	RC4_CTX rc4;
	QUARTZ_PRIVKEY privkey;		/* This has to be initialized and deinitialized     */
	QUARTZ_PUBLICKEY pubkey;	/* This is static and must not be allocated/freed ! */
	QUARTZ_PRIVKEY testprv;		/* These will be used in the import/export section  */
	QUARTZ_PUBLICKEY testpub;	
	unsigned char signature[QUARTZ_SIGNATURE_SIZE];
	unsigned char *prvbuff,*pubbuff;
	int prvbuffsize,pubbuffsize;
	int result;
	int size;
	int i;
	
	/* Random number generator used to create private keys */
	InitRC4(&rc4,rc4_key,(int)strlen(rc4_key));
	
	/* Engine initialization... */
	if( InitQuartzEngine() != 1 )
		{
		printf("Initialization error !\n");
		return 1;
		}

	/* ------------------------------------------------------------- */
	/* Keys initialization and creation                              */
	/* ------------------------------------------------------------- */

	/* Allocating memory for a private key with poly degree of 129 */
	/* Notice: public key needs not to be initialized/deinitialized as the private key. */
	if( init_priv_key(&privkey,POLY129) != 1 )
		{
		printf("Error initializing private key\n");
		return 1;
		}

	printf("Creating private key...\n");
	/* Use ArcFour to generate private key */
	create_priv_key(&rc4,&privkey);
	
	printf("Deriving public key from private...\n");
	/* Unlike RSA, public key can and actually should be derived from private */
	create_public_from_private(&privkey,&pubkey);
	
	/* ------------------------------------------------------------- */
	/* Signature creation phase                                      */
	/* ------------------------------------------------------------- */

	printf("Signing message: %s\n",message);
	/* Message signature is performed here... */
	QuartzSignMessage((unsigned char *)message,(int)strlen(message),&privkey,signature);
	
	/* Just pretty print the signature byte sequence */
	printf("Signature: ");
	for(i=0;i<QUARTZ_SIGNATURE_SIZE;i++)
		printf("%02x ",signature[i]);
		
	printf("\n\n");
	
	/* ------------------------------------------------------------- */
	/* Signature verification phase                                  */
	/* ------------------------------------------------------------- */
	
	/* This signature is expected to be verified... */
	printf("Verifying signature...\n");
	result = QuartzVerifySignature((unsigned char *)message,(int)strlen(message),&pubkey,signature);	
	printf("Test result: %d (expected: 1)\n",result);		
		
	signature[0] ^= 0xff;	// Corrupting signature to force verification failure...	
				
	/* This signature is corrupted and is expected not to be verified... */
	printf("Verifying with an invalid signature...\n");
	result = QuartzVerifySignature((unsigned char *)message,(int)strlen(message),&pubkey,signature);	
	printf("Test result: %d (expected: 0)\n",result);		
	printf("\n");
	
	/* ------------------------------------------------------------- */
	/* Keys import/export example                                    */
	/* ------------------------------------------------------------- */
	
	/* The first time we just query how many bytes to allocate by passing a NULL
	   pointer and a prvbuffsize of 0 */
	prvbuffsize = 0;	
	privkey2buffer(&privkey,NULL,&prvbuffsize);

	printf("Bytes needed to store a private key: %d\n",prvbuffsize);
	
	/* Now that we know how many bytes are needed, just allocate them ! 
	   Notice that prvbuffsize is the size of the buffer, which can be
	   larger than needed. The return value instead is the amount of
	   bytes actually used. */
	prvbuff = (unsigned char *)malloc( prvbuffsize );
	if(!prvbuff)
		{
		printf("Error allocating memory.\n");
		return 1;	
		}

	printf("Copying private key to an allocated buffer for external storage...\n");
	/* Copy the private key into the allocated buffer... */
	size = privkey2buffer(&privkey,prvbuff,&prvbuffsize);
	if(size <= 0)
		{
		printf("Error...\n");
		return 1;
		}

	/* Here you can save the buffer to disk or do whatever else you want...  */
	/* Notice: you may want to encrypt the private key data with a symmetric */
	/* algorithm before saving it to disk.                                   */

	/* ... */

	/* Ok, now let's do the opposite and use the buffer to set a private key... */
	printf("Copying back the buffer to a new private key...\n");
	
	/* Just be sure to initialize the key before filling it with the buffer content,
	   and most important, use the same polydegree as the one with which the buffer
	   was created earlier. */
	if( init_priv_key(&testprv,POLY129) != 1 )
		{
		printf("Error initializing private key\n");
		return 1;
		}
	
	/* Use size and not privbuffsize, because the latter may be larger if not allocated
	   the way we did. */
	if( buffer2privkey(prvbuff,size,&testprv) != 1 )
		{
		printf("Error creating private key from buffer...\n");
		return 1;	
		}
	
	/* And now let's make an example on how to import/export a public key... */
	
	/* Same stuff, query the function with a NULL pointer and 0 size to get the actual
	   public key size (which is always the same and independent of the poly degree */
	pubbuffsize = 0;
	pubkey2buffer(&pubkey,NULL,&pubbuffsize);
	
	/* Just allocate enough space to store export the key in a buffer */
	pubbuff = (unsigned char *)malloc(pubbuffsize);
	if(!pubbuff)
		{
		printf("Cannot allocate memory for public key...\n");
		return 1;
		}
	
	/* Export data to that buffer... */
	size = pubkey2buffer(&pubkey,pubbuff,&pubbuffsize);
	if(size <= 0)
		{
		printf("Error...\n");
		return 1;
		}
	
	/* Again, here you can save the buffer to disk or do whatever else you want... */

	/* ... */

	/* Ok, now let's do the opposite and use the buffer to set a public key... */
	printf("Copying back the buffer to a new public key...\n");
	
	/* Use size and not privbuffsize, because the latter may be larger if not allocated
	   the way we did. */
	if( buffer2pubkey(&testpub,pubbuff,size) != 1 )
		{
		printf("Error creating public key from buffer...\n");
		return 1;	
		}
		
	free(pubbuff);		/* Free up the allocated buffers... */
	free(prvbuff);	
	
	free_priv_key(&testprv);	/* Free resources for private keys */
	free_priv_key(&privkey);	

	DeInitQuartzEngine();		/* Stopping Quartz engine... */
	return 0;	
	}

