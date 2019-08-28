
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

#include "verystub.h"

int main(int ac,char **av)
	{
	int nRes;

	printf("\n");
	printf("QDecZ V1.04, written by Giuliano Bertoletti\n");
	printf("Copyright (C) 2003 GBE 32241 Software PR. All rights reserved\n");
	printf("Last revision %s - %s\n\n",__DATE__,__TIME__);
	printf("Usage of this program is subject to restrictions\n");
	printf("see the included file LICENSE.TXT for details.\n\n");
	
	if(ac != 3)
		{
		printf("Bad args...\n");
		printf("Usage: %s <public_key> <license_code>\n",av[0]);
		return -1;
		}
	
	nRes = VerfityLicenseCode(ac-1,&av[1]);
	if(nRes < 1) return -2;		// Return -2 if code is rejected because of wrong format

	if(nRes > 1) return 1;	// Return 1 is code is rejected
	return 0;		// Return 0 if code is accepted !
	}
