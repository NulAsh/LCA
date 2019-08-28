
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDMatrix.h"
#include "LNMatrix.h"
#include "LNPoly.h"
#include "KeyPair.h"
#include "StringGen.h"
#include "Globals.h"
#include "mstub.h"
#include "osdepn.h"

#include "SHA.h"

#define VSIZE   14
#define HSIZE   16


// #define KEYDUMP

#ifdef AUTOTEST

int ff_main(CKeyPair *kp)
	{
	int tlicid;
	int rr;

	STRING liccode;

	WORD32 start,end;
	start = GetChronoTime();
	rr = kp->CodeSign(kp->clicid,liccode);
	end = GetChronoTime();

	if( rr < 0 ) 
		{
		printf("Signature failed with: %d\n",rr);
		return -1;
		}

	printf("Code : %s\n",liccode.c_str());

	rr = kp->CodeVerify(liccode,&tlicid);
	if( rr <= 0 ) 
		{
		printf("Verification failed with: %d\n",rr);
		return -1;
		}

	if(kp->clicid != tlicid)
		{
		printf("Key ID mismatch ! : %08x - %08x\n",kp->clicid,tlicid);
		return -1;
		}	

	/* Ok, just change one key and see... */
	int ll = liccode.length();
	while( 1 )
		{
		int pq = RandPool.GetUInt() % ll;

		if(liccode[pq] != '-')
			{
			char n;
		
			while( (n = CStringGen::Val2Char(RandPool.GetByte() & 0x1f)) == liccode[pq] )
				;

			liccode[pq] = n;
			break;
			}
		}

	printf("WCode: %s\n",liccode.c_str());

	rr = kp->CodeVerify(liccode,&tlicid);
	if( rr > 0 ) 
		{
		printf("False signature verified: %d\n",rr);
		return -1;
		}

	int dtime = end-start;
	kp->t_time += dtime;
	
	kp->clicid++;
	return 0;
	}

/* ****************** Callbacks ******************* */

char myglobalpass[200];

int DREGZ_CALLTYPE passCallBack(char *password,int psize)
	{
	printf("External callback invoked...\n");
	
	if(strlen(myglobalpass) > (unsigned int)psize) 
		{
		printf("Cannot supply password...\n");
		return 0;
		}

	printf("Supplying password: %s\n",myglobalpass);
	strcpy(password,myglobalpass);

	Sleep(1000);
	return 1;
	}

int DREGZ_CALLTYPE gaugeCallBack(GAUGE *gauge)
	{
	printf("%s %d/%d",gauge->caption,gauge->gauge,gauge->t_gauge);

	if(gauge->gauge == gauge->t_gauge) printf("\n");
	else printf("\r");

	return 1;
	}

/* ************************************************ */

int TestProgram()
	{
	int rr;

	CKeyPair::CHeader h;
	
	rr = h.TestEncryption();
	if(rr != 1) 
		{
		printf("Encryption test failed with: %d\n",rr);
		return 1;
		}
	
	const int keysno = CKeyPair::GetMaxKeySlots();

	CKeyPair::SetGauge(gaugeCallBack);

	CKeyPair **kp;

	kp = new CKeyPair *[keysno];

	int i;
	for(i=0;i<keysno;i++)
		{
		char kname[260];

		sprintf(kname,"MyTestKey_%d.key",i+1);
		sprintf(myglobalpass,"password_%d",i+1);	/* global var to use function callback */

		kp[i] = new CKeyPair();

		rr = kp[i]->LoadKey(CKeyPair::ALLKEYS,kname,NULL,passCallBack);
		if(rr < 0)
			{
			rr = kp[i]->CreateKeyPair(i);
			if(rr <= 0) return 1;

			rr = kp[i]->Test();
			if(rr <= 0)
				{
				printf("Key test failed...\n");
				return 1;
				}

			/*
			printf("Saving key %s\n",kname);
			rr = kp[i]->SaveKeyPair(kname,myglobalpass);
			if(rr <= 0)
				{
				printf("Key saving failed...\n");
				return 1;
				}
			*/
			}
		}

	unsigned int keycount=0;

	while( 1 )
		{
		int key = (int)(keycount % keysno);
		keycount++;

		rr = ff_main(kp[key]);
		if(rr < 0) break;
		
		printf("---- %u ----------------------------------------\n",keycount);
		for(int i=0;i<keysno;i++)
			{
			int sig   = kp[i]->GetSignaturesNo();
			int fails = kp[i]->GetFailuresNo();
			int ttm   = kp[i]->t_time;
			
			double frate = (sig != 0 ? 100.0 * fails / (fails + sig) : 0.0);

			printf("#%d [%d/%d;%d,%d,%d> %d sig., %3.2f sec. %3.2f v/s. f=%d, fr=%3.3f (hi=%d)\n",
				i,kp[i]->GetEqsNo(),kp[i]->GetVarsNo(),
				kp[i]->GetHSize(),kp[i]->GetVSize(),kp[i]->GetMul(),
				sig,ttm / 1000.0, sig != 0 ? ttm / (1000.0 * sig) : 0.0, fails, frate,
				kp[i]->hinosolscore);
			}

		printf("----------------------------------------\n");
		}

	for(i=0;i<keysno;i++)
		delete kp[i];
	
	delete[] kp;

	printf("Failed with: %d\n",rr);
	return 0;
	}


#endif

/* ************************************************************************ */

int showhelp(const char *str)
	{
	printf("Usage: %s <command> [par1,par2,...]\n\n",str);
	printf("Available commands:\n\n");
	printf(" -c <keyfile>\n");
	printf("    create a keypair named keyfile.prv and keyfile.pub\n");
	printf("    The program prompts the user for a password to protect\n");
	printf("    private key. If the password is the null string, then\n");
	printf("    the private key won't be encrypted.\n");
	printf(" -p <keyfile>\n");
	printf("    change private key password in file keyfile.prv\n");
	printf("    old password and new password must be supplied.\n");
	printf(" -d <keyfile.prv> <keyfile.pub> creates keyfile.pub by deriving it from\n");
	printf("    the corresponding private key. Useful if you lose the public key file.\n");
	printf(" -l <keyfile> <num> [[+]fileout]\n");
	printf("    create the license code number num. Range is [0,2^32) using the\n");
	printf("    private key keyfile.prv. Password will be requested if needed.\n");
	printf("    If fileout is specified, it also writes codes on file fileout\n");
	printf("    (if a '+' character is prefixed to the name, codes are appended\n");
	printf("    at the end of the file)\n");
	printf(" -L <keyfile> <a-b> [[+]fileout]\n");
	printf("    same as -l but creates multiple codes from a to b. (0 <= a <= b < 2^32)\n");
	printf(" -v <keyfile> <str>\n");
	printf("    verify license code str using public key file keyfile.pub\n");
#ifdef AUTOTEST
	printf(" -t test program functionalities and performance\n");
#endif
	printf(" -P <keyfile> <lang> <path>\n");
	printf("    outputs decoder source code for the <ketfile> key to the specified path;\n");
	printf("    <keyfile> can be either the private or public key file, <lang> is the language\n");
	printf("    of the source code output.\n");
	printf("    Currently <lang> can be:\n");
	printf("      c : c source language (output will be lcvec.c and lcvec.h)\n");
	printf(" -h \n");
	printf("    show this help page\n");
	printf("\n");

	return 1;
	}

/* This is called by inner routines and the function is set by SetPrintFunction() */
void PrintFN(const char *s)
	{
	printf("%s",s);
	}

int main(int ac,char **av)
	{
	bool entropyOk = false;
	char code;
	int rVal=0;

	printf("\n");
	printf("DRegZ V1.02, written by Giuliano Bertoletti\n");
	printf("Copyright (C) 2005-2009 GBE 32241 Software PR. All rights reserved\n");
	printf("Last revision %s - %s\n\n",__DATE__,__TIME__);

	if(ac < 2) return showhelp(av[0]);
	else if(av[1][0] != '-') code = 0;
	else if(strlen(av[1]) != 2) code = 0;
	else code = av[1][1];

	if(RandPool.Randomize() == 1) entropyOk = true;
	SetPrintFunction(PrintFN);	// Inner routines needs output capabilities

	switch(code)
		{
		case 'c':
			if(!entropyOk) break;
			rVal = CreateDRegZKeyPair(ac-2,&av[2]);
			break;
		
		case 'd':
			rVal = DeriveDRegZPublicKey(ac-2,&av[2]);
			break;

		case 'p':
			if(!entropyOk) break;
			rVal = PrivKey_DRegZ_ChangePassword(ac-2,&av[2]);
			break;
		
		case 'l':
			rVal = CreateSingleLicenseCode(ac-2,&av[2]);
			break;
		
		case 'L':
			rVal = CreateManyLicenseCodes(ac-2,&av[2]);
			break;
		
		case 'v':
			rVal = VerfityLicenseCode(ac-2,&av[2]);
			break;

#ifdef AUTOTEST
		case 't':
			if(!entropyOk) break;
			rVal = TestProgram();
			break;
#endif

		case 'P':
			if(!entropyOk) break;
			rVal = OutputSourceCode(ac-2,&av[2]);
			break;

		case 'h':
			rVal = showhelp(av[0]);
			return 0;
		
		default:
			printf("?Unknown command.\nUse -h for a brief summary of avaialbe commands\n");
			return 0;
		}

	if(entropyOk == false)
		{
		printf("Unable to collect entropy from random number generator...\n");
		return 1;
		}

	return rVal;
	}
