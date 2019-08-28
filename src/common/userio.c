
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
#include "osdepn.h"
#include "userio.h"

#ifdef _WIN32

int GetUserPassword(char *password,int maxsize)
	{
	int nPos = 0;
	memset(password,0x00,maxsize);

	while( 1 )
		{
		char c = _getch();

		if(c == 0x0d || c == 0x0a) break;
		else if(c==0x1b) return 0;	/* Escape... */
		else if(c==0x03) return -1;
		else if(c == 0x08 && nPos>0)	
			{
			/* Delete char, if string is non empty */
			nPos--;
			putchar(c);
			putchar(' ');
			putchar(c);
			}
		else if(nPos < maxsize - 1 && c != 0x08)	
			{
			/* Put char if string length is < maxlen */
			putchar('.');	/* Do not show the char typed */
			password[nPos++] = c;
			}
		}

	password[nPos] = '\0';
	return 1;
	}


#else

#include <unistd.h>

int GetUserPassword(char *password,int maxsize)
	{
	const char *pwd;
	
	memset(password,0x00,maxsize);
	
	pwd = getpass("");
	if(pwd == NULL) return 0;
	
	strncpy(password,pwd,maxsize);
	return 1;
	}


#endif
