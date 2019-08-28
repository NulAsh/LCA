
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDMatrix.h"
#include "LNMatrix.h"
#include "LNPoly.h"
#include "KeyPair.h"
#include "StringGen.h"
#include "Globals.h"

#include "osdepn.h"
#include "mstub.h"
#include "userio.h"

bool warnprivkey;

int passInformCallBack(char *password,int psize)
	{
	if(warnprivkey == false)
		{
		printf("\n");
		printf("Private key is encrypted. You may enter its password now or use the public key\n");
		printf("to test a license code\n\n");
		}
	
	warnprivkey = true;
	printf("Password: ");

	fflush(stdout);

	int rr = GetUserPassword(password,psize);
	printf("\n");

	if(rr <= 0) return 0;
	return 1;
	}

int DREGZ_CALLTYPE passStubCallBack(char *password,int psize)
	{
	printf("Password: ");
	fflush(stdout);
	
	int rr = GetUserPassword(password,psize);
	printf("\n");

	if(rr <= 0) return 0;
	return 1;
	}

int DREGZ_CALLTYPE gaugeStubCallBack(GAUGE *gauge)
	{
	// printf("%s %d/%d",gauge->caption,gauge->gauge,gauge->t_gauge);

	// if(gauge->gauge == gauge->t_gauge) printf("\n");
	// else printf("\r");
	return 1;
	}


int GetUserPasswordWithConfirmation(char *password,int maxsize)
	{
	if(maxsize <= 0) return -2;

	char *confirm = new char[maxsize];
	if(!confirm) return -2;

	int i,rr;
	for(i=0;i<3;i++)
		{
		printf("Password: ");
		fflush(stdout);
		
		rr = GetUserPassword(password,maxsize);
		printf("\n");

		if(rr <= 0) 
			{
			delete[] confirm;
			return rr;
			}

		printf("Confirm : ");
		fflush(stdout);
		
		rr = GetUserPassword(confirm,maxsize);
		printf("\n");
		
		if(rr <= 0) 
			{
			delete[] confirm;
			return rr;
			}

		if(strcmp(password,confirm)==0) break;

		printf("\n");
		}

	delete[] confirm;
	return i < 3 ? 1 : 0;
	}

int CreateDRegZKeyPair(int ac,char **av)
	{
	if(ac != 1 && ac != 2)
		{
		printf("?Bad args.\n");
		return -1;
		}

	CKeyPair kp;

	int rr;

	rr = kp.CreateKeyPair(0);
	if(rr <= 0)
		{
		printf("Key creation process interrupted\n");
		return -3;
		}

	int rVal = 0;

	STRING filename = av[0];

	STRING privkeyfname= filename + ".prv";
	STRING pubkeyfname = filename + ".pub";

	const int maxpasssize = 1024;
	char password[maxpasssize];
	bool setpassword = true;

	if(ac == 2)
		{
		if(strlen(av[1]) < (unsigned int)maxpasssize) strcpy(password,av[1]);
		else
			{
			printf("Supplied password is too long...\n");
			return -13;
			}
		}
	else
		{
		printf("You may enter a password now in order to protect your private key file,\n");
		printf("or hit ESC to leave it in plain text\n\n");
		
		if( GetUserPasswordWithConfirmation(password,maxpasssize) <= 0 ) setpassword = false;
		printf("\n\n");
		}

	printf("Saving private key: %s\n",privkeyfname.c_str());
	rr = kp.SaveKeyPair(privkeyfname.c_str(),setpassword ? password : NULL);
	if(rr <= 0) 
		{
		printf("Save failed (error=%d)\n",rr);
		rVal = -10;
		}

	printf("Saving public key %s\n",pubkeyfname.c_str());
	rr = kp.SavePublicKey(pubkeyfname.c_str());
	if(rr <= 0) 
		{
		printf("Save failed (error=%d)\n",rr);
		rVal = -11;
		}
	
	return rVal;
	}


int DeriveDRegZPublicKey(int ac,char **av)
	{
	if(ac != 2)
		{
		printf("?Bad args.\n");
		return -1;
		}

	CKeyPair kp;

	int rr;

	printf("Loading key: %s\n",av[0]);
	rr = kp.LoadKey(CKeyPair::ALLKEYS,av[0],NULL,passStubCallBack);	

	if(rr <= 0)
		{
		printf("Load failed.\n");
		return -21;
		}

	rr = kp.DerivePublicKeyFromPrivate();
	if(rr == 0)
		{
		printf("Derivation failed...\n");
		return -22;
		}

	printf("Saving public key %s\n",av[1]);
	rr = kp.SavePublicKey(av[1]);
	if(rr <= 0)
		{
		printf("Save error...\n");
		return -23;
		}

	return 0;
	}

int PrivKey_DRegZ_ChangePassword(int ac,char **av)
	{
	if(ac != 1)
		{
		printf("?Bad args.\n");
		return -1;
		}

	CKeyPair kp;
	int rr;

	printf("Loading key: %s\n",av[0]);
	rr = kp.LoadKey(CKeyPair::ALLKEYS,av[0],NULL,passStubCallBack);	

	if(rr <= 0)
		{
		printf("Load failed.\n");
		return -21;
		}

	const int maxpasssize = 1024;
	char password[maxpasssize];
	bool setpassword = true;

	printf("Enter new password for private key\n");
	
	if( GetUserPasswordWithConfirmation(password,maxpasssize) <= 0 ) setpassword = false;
	printf("\n\n");

	printf("Saving private key: %s\n",av[0]);
	rr = kp.SaveKeyPair(av[0],setpassword ? password : NULL);
	if(rr <= 0) 
		{
		printf("Save failed (error=%d)\n",rr);
		return -22;
		}

	return 0;
	}

int CreateCodes(CKeyPair *kp,int start,int end,const char *filename)
	{
	FILE *h1 = NULL;
	bool append = false;

	WORD32 istart = GetChronoTime();

	if(filename)
		{
		if(filename[0] == '+')
			{
			filename++;
			h1 = fopen(filename,"a");
			append = true;
			}
		else h1 = fopen(filename,"w");

		if(!h1)
			{
			printf("Error: failed to open %s\n",filename);
			return -31;
			}
		}

	if(start != end) 
		printf("Creating codes from %d to %d\n",start,end);
	else 
		printf("Creating code %d\n",start);

	int done = 0;
	unsigned int current;
	for(current = (unsigned int)start;current <= (unsigned int)end; current++)
		{
		STRING code;

		int rr = kp->CodeSign((int)current,code);
		
		if(rr == 0)	// Signature failed...
			{
			printf("%3d: <signature failed>\n",current);
			if(h1) fprintf(h1,"%3d: <signature failed>\n",current);
			}
		else if(rr < 0)
			{
			printf("?Unable to create code %d\n",current);
			return -32;
			}
		else
			{
			printf("%3d: %s\n",current,code.c_str());
			if(h1) fprintf(h1,"%3d: %s\n",current,code.c_str());
			}

		done++;
		}

	printf("\n");

	WORD32 iend = GetChronoTime();

	WORD32 delta = iend - istart;
	if(delta == 0) delta = 1;

	printf("Time elapsed: %3.2f sec. (avg: %3.2f codes/sec.)\n",
		delta / 1000.0, 1000.0 * done / delta);

	if(h1) fclose(h1);
	return 0;
	}

int CreateManyLicenseCodes(int ac,char **av)
	{
	int rr;

	if(ac != 2 && ac != 3)
		{
		printf("?Bar args.\n");
		return -1;
		}

	int start,end;

	rr = sscanf(av[1],"%d-%d",&start,&end);
	if(rr != 2 || start > end || start < 0 || end < 0)
		{
		printf("range error\n");
		return -2;
		}

	CKeyPair kp;

	printf("Loading private key: %s\n",av[0]);
	rr = kp.LoadKey(CKeyPair::ALLKEYS,av[0],NULL,passStubCallBack);
	if(rr <= 0)
		{
		printf("Key load failed...\n");
		return -10;
		}

	return CreateCodes(&kp,start,end, ac == 3 ? av[2] : NULL);
	}

int CreateSingleLicenseCode(int ac,char **av)
	{
	int rr;

	if(ac != 2 && ac != 3)
		{
		printf("?Bar args.\n");
		return -1;
		}

	int start;

	rr = sscanf(av[1],"%d",&start);
	if(rr != 1 || start < 0)
		{
		printf("code id error\n");
		return -2;
		}

	CKeyPair kp;

	printf("Loading private key: %s\n",av[0]);
	rr = kp.LoadKey(CKeyPair::ALLKEYS,av[0],NULL,passStubCallBack);
	if(rr <= 0)
		{
		printf("Key load failed...\n");
		return -10;
		}

	return CreateCodes(&kp,start,start, ac == 3 ? av[2] : NULL);
	}


int VerfityLicenseCode(int ac,char **av)
	{
	int rr;

	if(ac != 2)
		{
		printf("?Bar args.\n");
		return -1;
		}

	CKeyPair kp;

	printf("Loading key: %s\n",av[0]);

	warnprivkey = false;
	rr = kp.LoadKey(CKeyPair::PUBKEY,av[0],NULL,passInformCallBack);
	if(rr <= 0)
		{
		printf("Key load failed...\n");
		return -10;
		}

	printf("Verifying code: %s\n\n",av[1]);

	int licid;
	rr = kp.CodeVerify(av[1],&licid);

	if(rr == 0) 
		{
		printf("Invalid code (rejected)\n");
		}
	else if(rr == 1) printf("Code is valid (accepted with id=%d)\n",licid);
	else printf("Malformed code (rejected)\n");

	return 0;
	}

int OutputSourceCode(int ac,char **av)
	{
	int rr;

	if(ac != 3)
		{
		printf("?Bar args.\n");
		return -1;
		}
	
	int language = -1;

	if(language == -1 && _stricmp(av[1],"c")==0) language = 0;

	if(language == -1)
		{
		printf("Unsupported/unknown output source language\n");
		return -41;
		}

	CKeyPair kp;

	printf("Loading public key %s\n",av[0]);
	rr = kp.LoadKey(CKeyPair::ALLKEYS,av[0],NULL,passStubCallBack);
	if(rr <= 0)
		{
		printf("Key load failed...\n");
		return -10;
		}

	rr = kp.WriteCStructure(av[2],6);
	if(rr <= 0)
		{
		printf("Error outputting source code\n");
		return -98;
		}

	return 0;
	}


