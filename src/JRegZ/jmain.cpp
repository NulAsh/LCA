/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include <stdio.h>
#include <time.h>

#include "LicenseCode.h"
#include "JRegZ.h"


int main(int ac, char **av)
{
	// basic instructions on how to use JRegZ algorirthm are
	// given in chapter 4 of LicenseCodesAlgorithms.pdf

	int rr = CLicenseCode::AutoTest();
	if(rr != 1) {
		printf("test aborted\n");
		exit(1);
	}

	printf("Done.\n");
	return 0;
}
