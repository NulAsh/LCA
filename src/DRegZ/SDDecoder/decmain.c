
/*

  DRegZ code sample. This module needs lcvec.c and lcvec.h which are generated
  by the DRegZ program and are unique for each private key generated.

*/

#include <stdio.h>
#include <stdlib.h>

#include "decode.h"

int main(int ac,char **av)
	{
	int lic_id;
	int result;

	if(ac != 2)
		{
		printf("Usage: %s <license code>\n",av[0]);
		return -1;	
		}	
	
	printf("Verifying code: %s\n",av[1]);
	
	result = decode_license(av[1], &lic_id);

	if(result == 1) 
		{
		printf("Code accepted (id: %d)\n",lic_id);
		return 0;
		}
	else if(result == 0)
		{
		printf("Code rejected\n");
		return 1;
		}
	else if(result == 2)
		{
		printf("Code blacklisted (id: %d)\n",lic_id);
		return 2;
		}
	else
		{ 
		printf("Internal error #%d\n",result);
		return 3;
		}
	}

