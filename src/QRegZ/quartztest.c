
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
#include <string.h>

#include "quartztest.h"
#include "quartzsig.h"
#include "lintra.h"
#include "osdepn.h"

#include "rootfind.h"
#include "polyprint.h"

int test_quarz_signature(RC4_CTX *rc4,QUARTZ_PRIVKEY *qpk,QUARTZ_PUBLICKEY *qpub)
	{
	WORD32 start,end,end2,end3,end4;
	WORD_8 signature[QUARTZ_SIGNATURE_SIZE+1];
	WORD_8 message[TEST_MSG_SIZE];
	char strtest[1024];
	char strsig[1024];
	char bitstr[1024];
	int i,res;
	int inside;
	int outside;

	memset(signature,0x00,QUARTZ_SIGNATURE_SIZE);

	for(i=0;i<TEST_MSG_SIZE;i++)
		message[i] = RC4_GetByte(rc4);

	start = GetChronoTime();

	printf("Signing message...\n");
	PrintBuffer(message,64);

	res = QuartzSignMessage(message,TEST_MSG_SIZE,qpk,signature);

	end = GetChronoTime();
	printf("Signature done... (%3.2f sec.)\n",(end-start)/1000.0);

	if( res < 1 ) 
		{
		printf("...but the signature is invalid !\n");
		return 0;
		}

	printf("Loops: %d, average time per loop %3.2f\n",res,(end-start)/(1000.0 * res));

	PrintBuffer(signature,QUARTZ_SIGNATURE_SIZE);
	
	printf("Siganture verification test...\n");
	
	if( QuartzVerifySignature(message,TEST_MSG_SIZE,qpub,signature) != 1) 
		{
		printf("Verification failed...\n");
		return 0;
		}

	end2 = GetChronoTime();
	printf("Verification done... (%3.2f sec.)\n\n",(end2-end)/1000.0);

	do
		{
		inside  = RC4_GetByte(rc4) % 32;
		outside = RC4_GetByte(rc4);
		} while( outside == 0 );

	for(i=0;i<inside+outside;i++)
		strtest[i] = (RC4_GetByte(rc4) & 0x40 ? '1' : '0');

	strtest[i] = '\0';

	printf("Variable length signature test... (bit inside=%d, outside=%d, len=%d)\n",
		    inside,outside,inside+outside);

	if( QuartzSignVariableCode(strtest,inside,qpk,strsig) <= 0)
		{
		printf("Variable length signature failed !\n");
		return 0;
		}

	end3 = GetChronoTime();
	printf("Signature done in %3.2f secs.\n",(end3-end2) / 1000.0);

	if( QuartzVerifyVariableCode(qpub,strsig,inside,outside,bitstr) != 1 )
		{
		printf("Variable length signature verification failed !\n");
		return 0;
		}

	if(strcmp(strtest,bitstr) != 0)
		{
		int l1,l2;
		int mx,mn;

		printf("Bitstrings mismatch : ");

		l1 = (int)strlen(strtest);
		l2 = (int)strlen(bitstr);

		mx = MAX(l1,l2);
		mn = MIN(l1,l2);

		for(i=0;i<mx;i++)
			{
			if(i < mn)
				{
				if(strtest[i] == bitstr[i]) printf("0");
				else printf("1");
				}
			else if(l1 < mn) printf("-");
			else printf("+");
			}

		printf("\n");
		return 0;
		}

	end4 = GetChronoTime();

	printf("Variable signature test passed (verified in %3.2f secs.)...\n\n",
		   (end4-end3)/1000.0);

	return 1;
	}

#define MAXBUFFSIZE MAX_PRIVKEYLEN

int fv_key_test(RC4_CTX *rc4)
	{
    static int tdone=0,tfail=0;
	QUARTZ_PRIVKEY qpk;
	QUARTZ_PUBLICKEY *qpub;
	WORD_8 in[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 out[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 fin[QUARTZ_AFFINE_TRANSSIZE];
	WORD_8 *buffer;
	int round=1;
	int i,j;
    int done=0,fail=0;

	int powdeg = 7;

    printf("Private key creation test...\n");

	qpub = (QUARTZ_PUBLICKEY *)malloc( sizeof(QUARTZ_PUBLICKEY) );
	if(!qpub) return 0;

	if( init_priv_key(&qpk,powdeg)!=1 ) return 0;

	printf("Private polynomial degree = %d (pow=%d)\n",qpk.fv.deg,qpk.fv.pow);

	if( create_priv_key(rc4,&qpk) != 1 ) return 0;

    printf("Public key derivation (from private) test...\n");
	if( create_public_from_private(&qpk,qpub) != 1 ) return 0;

	buffer = (WORD_8 *)malloc(MAXBUFFSIZE);
	if(buffer)
		{
		int buffsize,maxsize=MAXBUFFSIZE;
		int res;
		
		buffsize = privkey2buffer(&qpk,buffer,&maxsize);
		if(buffsize > 0)
			{
			QUARTZ_PRIVKEY test;
			printf("Testing private key reload...\n");
			
			if( init_priv_key(&test,powdeg)!=1 ) return 0;
		
			res = buffer2privkey(buffer,buffsize,&test);

			free_priv_key(&test);

			if( res != 1 )
				{
				printf("Reload failed %d\n",res);
				return 0;
				}

			printf("Reload completed...\n");
			}

		/* ************************************************************ */

		maxsize = MAXBUFFSIZE;
		buffsize = pubkey2buffer(qpub,buffer,&maxsize);
		if(buffsize > 0)
			{
			QUARTZ_PUBLICKEY *test;
			printf("Testing public key reload...\n");
			
			test = (QUARTZ_PUBLICKEY *)malloc(sizeof(QUARTZ_PUBLICKEY));
			if(!test)
				{
				printf("Public key allocation failed\n");
				return 0;
				}

			res = buffer2pubkey(test,buffer,buffsize);

			free(test);

			if( res != 1 )
				{
				printf("Reload failed %d\n",res);
				return 0;
				}

			printf("Reload completed...\n");
			}

		free(buffer);
		}
	else
		{
		printf("Allocation error !\n");
		return 0;
		}

	/* *********************************************** */

    printf("Signature verification test...\n");
    for(j=0;j<100;j++)
        {
	    for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		    in[i] = RC4_GetByte(rc4) & 0x01;

	    if( pubkey_forward(qpub,out,in) != 1 ) return 0;
	    if( privkey_forward(&qpk,fin,in) != 1 )     return 0;

        //print_vector(in ,107,"Input  :");
	    //print_vector(out,100,"Output1:");
	    //print_vector(fin,100,"Output2:");


        if(memcmp(out,fin,100) == 0) done++;
        else
            {
            fail++;
            }

        if(j%10==9) printf("%d   \r",j+1);
        }

    tdone+=done;
    tfail+=fail;

    printf("Signature ok=%d, failed=%d (total: ok=%d, failed=%d)\n",done,fail,tdone,tfail);
    print_vector(in ,107,"Input  :");
	print_vector(out,100,"Output1:");
	print_vector(fin,100,"Output2:");

	/* *********************************************** */

	printf("\n");

	if( test_quarz_signature(rc4,&qpk,qpub) != 1) return 0;

    if(qpub) free(qpub);

	printf("Testing forward and backward steps...\n");

	while( 1 )
		{
		printf("Attempt: %d      \n",round);
		//print_vector(in,QUARTZ_AFFINE_TRANSSIZE,"Start: ");

		if( privkey_backward(&qpk,out,in) == 1 ) break;

		//printf("Ok wait, I'm trying again...\n");

		for(i=QUARTZ_POLY_DEGREE;i<QUARTZ_AFFINE_TRANSSIZE;i++)
			in[i] = RC4_GetByte(rc4) & 0x01;

		round++;
		}

	if( privkey_forward(&qpk,fin,out) != 1 ) return 0;

	//print_vector(fin,QUARTZ_AFFINE_TRANSSIZE,"End  : ");

	if( free_priv_key(&qpk)!=0 ) return 0;

	if(memcmp(in,fin,QUARTZ_AFFINE_TRANSSIZE) != 0) return 0;

	return 1;
	}

int fv_test(RC4_CTX *rc4)
	{
	ROOT_POT pot;
	FV_STRUCT fv;
	FFPOLY cmod;
	EXPOLY fact;
	FFPOLY B;
	WORD32 start,end;
	int i,j;
    int flag=1;

	/*
	for(i=0;i<100;i++)
		{
		if( fv_key_test(rc4) != 1 ) return 0;
		printf("Done...\n");
		}
	*/
	fv.pow = 7;
	fv.deg = (1 << fv.pow) + 1;

	if( expoly_init(&fact,fv.deg+1) != 1 ) return 0;
	ffpoly_set(&cmod,quartz_irr_ppoly,3);

	poly_print(&cmod,"CMOD=");

	if( init_fv_struct(&fv) != 1) return 0;
	if( build_fv_functions(rc4,&fv) != 1) return 0;

	start = GetChronoTime();
	
	for(i=0;i<1 && flag;i++)
		{
		build_randpoly(rc4,&B,cmod.pvdeg,0);

		if( expoly_copy(&fact,&fv.vinegar[i]) < 1) return 0;

		ffpoly_identity_set(&fact.ep[fv.deg]);
		ffpoly_addmod(&fact.ep[0],&fact.ep[0],&B,&cmod);

		if( expoly_pot_init(&pot,&cmod,fact.pvdeg) != 1) return 0;

		if( expoly_factor(&pot,&fact,&cmod) != 1 ) 
			{
			printf("Error !\n");
			return 0;
			}

		printf("Solutions found = %d\n",pot.factsNo);

        for(j=0;j<pot.factsNo;j++)
            {
            FFPOLY rs;

            if( expoly_subst(&rs,&fact,&pot.factors[j],&cmod) != 1 ) 
                {
                printf("Substitution failed [deg(F)=%d]!\n",pot.factors[j].pvdeg);
                }

            poly_print(&rs,"Fv(Z)-B=");

            if(ffpoly_is_zero(&rs)!=1) flag=0;
            }

		expoly_pot_free(&pot);
		}

	end = GetChronoTime();

	printf("Elapsed sec. %3.3f\n",(end-start) / 1000.0);
   
	free_fv_struct(&fv);
	expoly_free(&fact);

	return flag;
	}
