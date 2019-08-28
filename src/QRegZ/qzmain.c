
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

#include "ffpoly.h"
#include "polytest.h"
#include "quartzsig.h"
#include "keygen.h"
#include "fileio.h"
#include "verystub.h"


int TestProgram(int ac,char **av)
	{
	if(ac < 1) 
		{
		printf("?Seed missing.\n");
		return 1;
		}
	
	return poly_test(av[0]);
	}

int showhelp(const char *str)
	{
	printf("Usage: %s <command> [par1,par2,...]\n\n",str);
	printf("Available commands:\n\n");
	printf(" -s <randstring>\n");
	printf("    reseed the random pool located in file quartzrnd.bin\n");
	printf("    (if randstring is supplied, then entropy is collected\n");
	printf("    also from it).\n");
	printf(" -c <level> <keyfile>\n");
	printf("    create a keypair named keyfile.prv and keyfile.pub\n");
	printf("    level is a security parameter ranging from 1 (low) to 3 (high).\n");
	printf("    The program prompts the user for a password to protect\n");
	printf("    private key. If the password is the null string, then\n");
	printf("    the private key won't be encrypted.\n");
	printf(" -p <keyfile>\n");
	printf("    change private key password in file keyfile.prv\n");
	printf("    old password and new password must be supplied.\n");
	printf(" -d <keyfile>\n");
	printf("    derive public key from private key named keyfile.prv and\n");
	printf("    writes it to keyfile.pub (only useful if you deleted it,\n");
	printf("    since the -c command creates both private and public keys)\n");
	printf(" -l <keyfile> <num> [[+]fileout]\n");
	printf("    create the license code number num. Range is [0,2^32) using the\n");
	printf("    private key keyfile.prv. Password will be requested if needed.\n");
	printf("    if fileout is specified, it also writes codes on file fileout\n");
	printf("    (if a '+' character is prefixed to the name, codes are appended\n");
	printf("    at the end of the file)\n");
	printf(" -L <keyfile> <a-b> [[+]fileout]\n");
	printf("    same as -l but creates multiple codes from a to b. (0 <= a <= b < 2^32)\n");
	printf(" -v <keyfile> <str>\n");
	printf("    verify license code str using public key file keyfile.pub\n");
	printf(" -t <seed>\n");
	printf("    test program functionalities and performance\n");
	printf(" -h \n");
	printf("    show this help page\n");
	printf("\n");

	return 1;
	}

int main(int ac,char **av)
	{
	char code;
	int rVal=0;

	printf("\n");
	printf("QRegZ V1.06, written by Giuliano Bertoletti\n");
	printf("Copyright (C) 2003-2009 GBE 32241 Software PR. All rights reserved\n");
	printf("Last revision %s - %s\n\n",__DATE__,__TIME__);
	printf("Usage of this program is subject to restrictions\n");
	printf("see the included file LICENSE.TXT for details.\n\n");

	//printf("Outret: %08x --> %08x\n",&a,test_mmx_assembly(&a,&b,&c));

	if( InitQuartzEngine() != 1 )
		{
		printf("?Engine initialization failure...\n");
		return 1;
		}

	if(ac < 2) return showhelp(av[0]);
	else if(av[1][0] != '-') code = 0;
	else if(strlen(av[1]) != 2) code = 0;
	else code = av[1][1];

	switch(code)
		{
		case 's':
			rVal = ReSeed_CSPRNG(ac-2,&av[2]);
			break;
		
		case 'c':
			rVal = CreateQuartzKey(ac-2,&av[2]);
			break;
		
		case 'p':
			rVal = ChangePassword(ac-2,&av[2]);
			break;
		
		case 'd':
			rVal = DerivePublicFromPrivate(ac-2,&av[2]);
			break;
		
		case 'l':
			rVal = CreateLicenseCode(ac-2,&av[2]);
			break;
		
		case 'L':
			rVal = CreateManyLicenseCodes(ac-2,&av[2]);
			break;
		
		case 'v':
			rVal = VerfityLicenseCode(ac-2,&av[2]);
			break;
		
		case 't':
			rVal = TestProgram(ac-2,&av[2]);
			break;

		case 'h':
			rVal = showhelp(av[0]);
			break;
		
		default:
			printf("?Unknown command.\nUse -h for a brief summary of avaialbe commands\n");
			break;
		}

	DeInitQuartzEngine();

	return rVal;
	}







