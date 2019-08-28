
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

#include "quartzsig.h"
#include "keygen.h"
#include "lintra.h"
#include "fileio.h"
#include "arc4.h"
#include "osdepn.h"


int kg_mixup(WORD_8 *vector,int size)
	{
	int i,j;
	
	if(size < 6) return 0;

	for(i=0;i<MIXROUNDS;i++)
		{
		for(j=0;j<size;j++)
			{
			vector[(j + size - 1) % size] ^= kg_coremix(vector[(j  )     ],
														vector[(j+1)%size],
														vector[(j+2)%size],
														vector[(j+3)%size],
														vector[(j+4)%size]);
			
			
			}
		}

	return 1;
	}
	
int Val2Char(int n)
	{
	int rv;
	if(n<0 || n>=32) return -1;
	if(n<26) return 'A' + n;
        
	rv = '2' + (n - 26);
	if(rv == '5') rv = '9';  /* Ascii '5' is replaced by '9' */

	return rv;
	}

void expand_eight_to_five(WORD_8 *src,WORD_8 *dst)
	{
	WORD_8 bytev[160];
	int dummy,i;

	bitvect2bytevect(src,bytev,160);
	
	for(i=0;i<32;i++)
		{
		vect2int(bytev + i*5,5,&dummy);
		dst[i] = (WORD_8)dummy;
		}
	}

void DecorateQuartzCode(const char *codestr,char *out,int each)
	{
	size_t i,j=0;
	size_t len = strlen(codestr);

	for(i=0;i<len;i++)
		{
		if(i>0 && i%each == 0) out[j++]='-';
		out[j++]=codestr[i];
		}

	out[j]='\0';
	}


int CreateSingleCode(QUARTZ_PRIVKEY *qpk,WORD32 id,char *str)
	{
	char message[100];
	WORD_8 signature[QUARTZ_SIGNATURE_SIZE+sizeof(id)];
	char fixup[32];
	int res,i;

	str[0] = '\0';

	sprintf(message,"%08x",id);

	res = QuartzSignMessage((unsigned char *)message,(int)strlen(message),qpk,signature);
	if(res < 1) return 0;

	memcpy(signature + QUARTZ_SIGNATURE_SIZE,&id,sizeof(id));
	expand_eight_to_five(signature,(unsigned char *)fixup);

	kg_mixup((unsigned char *)fixup,32);

	for(i=0;i<32;i++)
		str[i] = Val2Char(fixup[i]);
		
	str[i] = '\0';
	return 1;	
	}

int OutputCodeToFile(const char *filename,WORD32 id,const char *code)
	{
	FILE *h1;

	if(filename == NULL) return 1;

	h1 = fopen(filename,"a");
	if(!h1) return 0;

	if( fprintf(h1,"%3u: %s\n",id,code) < 0 ) return 0;
	if( fclose(h1) != 0 ) return 0;
	
	return 1;
	}


int LicenseCodeCreation(const char *filename,WORD32 begin,WORD32 end,const char *fout)
	{
	QUARTZ_PRIVKEY qpk;
	QUARTZ_PUBLICKEY *qpub;
	HEADERKEY header;
	char codestr[100];
	char QuartzCode[100];
	char str[_MAX_PATH];
	char msg[200];
	WORD32 id;
	int nRes=0;
	int append=0;
	WORD32 t_start,t_end;
	double avg;

	if(begin > end)
		{
		printf("?Starting code ID must be less or equal than ending code\n");
		return 0;
		}

	if(strlen(filename) > _MAX_PATH - 5)
		{
		printf("?Filename too long\n");
		return 0;
		}

	strcpy(str,filename);
	strcat(str,".prv");

	printf("Probing private key file %s\n",str);
	if( ProbePrivateKeyHeaderFromDisk(str,&header) != 1 )
		{
		// Message already shown
		return 0;
		}

	printf("Private polynomial degree = %d\n",header.deg);

	if(header.encrypted) printf("Key is encrypted...\n");
	printf("\n");

	if( init_priv_key(&qpk,header.pow) != 1 )
		{
		printf("?Error initializing private key\n");
		return 0;
		}

	qpub = (QUARTZ_PUBLICKEY *)malloc( sizeof(QUARTZ_PUBLICKEY) );
	if(!qpub)
		{
		printf("?Error allocating memory for public key\n");
		free_priv_key(&qpk);
		return 0;
		}

	if( ReadPrivateKeyFromDisk(str,&qpk) != 1 ) 
		{
		free_priv_key(&qpk);
		free(qpub);
		return 0;	// Error already shown
		}
	strcpy(str,filename);
	strcat(str,".pub");

	if( ReadPublicKeyFromDisk(str,qpub) != 1 )
		printf("Deriving public key from private because of the above error (wait please...)\n");

	printf("\n");
	if(begin != end) sprintf(msg,"Creating license codes from ID %u to ID %u",begin,end);
	else sprintf(msg,"Creating license code ID %u",begin);

	if(fout && fout[0] == '+')
		{
		if(fout[1]=='\0')
			{
			printf("?Invalid output file string\n");
			free_priv_key(&qpk);
			free(qpub);

			return 0;
			}
		
		append=1;
		fout++;
		}

	if(fout && !append)
		{
		FILE *h1;
		
		printf("Writing codes to output file %s\n",fout);

		h1 = fopen(fout,"w");
		if(!h1)
			{
			printf("?Cannot open output file.\n");
			free_priv_key(&qpk);
			free(qpub);

			return 0;
			}

		if( fprintf(h1,"QuartzReg license code output file\n\n") < 0 )
			{
			printf("Problems writing output file\n");
			free_priv_key(&qpk);
			free(qpub);

			return 0;
			}

		if( fclose(h1) != 0 )
			{
			printf("Problems closing output file\n");
			free_priv_key(&qpk);
			free(qpub);

			return 0;
			}
		}
	
	printf("%s\n\n",msg);

	t_start = GetChronoTime();

	id=begin;
	while( 1 )	// Cannot use for because end may be equal to 2^32 - 1
		{
		WORD32 cmpID;

		nRes = CreateSingleCode(&qpk,id,codestr);
		if(nRes != 1)
			{
			printf("?Code creation failed\n");
			break;
			}

		DecorateQuartzCode(codestr,QuartzCode,8);
		printf("%3u: %s\n",id,QuartzCode);

		nRes = TestSingleCode(qpub,&cmpID,codestr);
		if(nRes != 1 || cmpID != id) 
			{
			printf("?Test failed error. Code: %s is invalid.\n",codestr);
			break;
			}

		nRes = OutputCodeToFile(fout,id,QuartzCode);
		if( nRes != 1 )
			{
			printf("?Unable to write code to disk!\n");
			break;
			}

		if(id == end) break;
		id++;
		}

	t_end = GetChronoTime();

	avg = ((double)(t_end - t_start)) / (end - begin + 1);

	printf("\n");
	printf("%d signature(s) in %3.2f sec(s).\n",end-begin+1,(t_end-t_start)/1000.0);
	printf("Average time per signature: %3.2f sec(s)\n",avg / 1000.0);

	free_priv_key(&qpk);
	free(qpub);
	
	return nRes;	
	}


/* ********************************************************************************** */

int CreateLicenseCode(int ac,char **av)
	{
	WORD32 begin;

	if(ac < 2 || ac > 3)
		{
		printf("?Bad args.\n");
		return 0;
		}

	begin = atoi(av[1]);

	return LicenseCodeCreation(av[0],begin,begin,ac == 3 ? av[2] : NULL);
	}

int CreateManyLicenseCodes(int ac,char **av)
	{
	WORD32 begin,end;

	if(ac < 1 || ac > 3)
		{
		printf("?Bad args.\n");
		return 0;
		}

	begin = atoi(av[1]);
	end   = atoi(av[2]);

	return LicenseCodeCreation(av[0],begin,end,ac==4 ? av[3] : NULL);
	}




