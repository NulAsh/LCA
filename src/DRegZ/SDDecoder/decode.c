

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>

#include "decode.h"
#include "lcvec.h"
#include "defs.h"

#define RESSIZE         256
#define LICENSE_CHARS   25
#define COUNTER_BITS    31

int char2val(char c)
	{
	if(c == '1') c='I';
	if(c == '0') c='O';
	if(c == '5') c='S';
	if(c == '8') c='B';

	if(isalpha(c)) return (int)(toupper(c) - 'A');

	if(c>='2' && c<'8') return 26 + ( c - '2' );
        
	if( c=='9' ) return 26 + ( '5' - '2' );
	return -1;    
	}

int undecorate_string(const char *deccode, char *undeccode)
	{
	int len = strlen(deccode);
	int j=0;
	int i;

	for(i=0;i<len;i++)
		{
		if(deccode[i]=='-') continue;
		else if(char2val(deccode[i]) < 0) return 0;

		if(j >= 32) return 0;
		undeccode[j++] = deccode[i];
		}

	undeccode[j]='\0';
	return 1;
	}

int get_license_id(ELEMENT *signature)
	{
	int lic_id = 0;
	int i;

	// the first 31 bits of the signature (in reverse)
	// represent a counter which identify a license
	// (see readme.txt)

	for(i=0;i<COUNTER_BITS;i++) 
		if(signature[i]) lic_id |= (1 << i);

	return lic_id;
	}

int check_blacklist(unsigned int lic_id)
	{
	/* License codes are black-listed here by ID */
	// ...
 
	return 0;
	}

// what am I checking here ?
int verify_parity(ELEMENT *signature, int sigsize, ELEMENT *parity, int psize)
	{
	ELEMENT res[RESSIZE];
	int i,j,mx,delta;

	if(sigsize + psize >= RESSIZE) return -1;

	for(i=0;i<RESSIZE;i++)
		res[i] = 0;

	for(i=0;i<sigsize;i++)
		{
		if(signature[i] == 0) continue;

		for(j=0;j<psize;j++)
			res[i+j] ^= parity[j];
		}

	for(mx=RESSIZE-1;mx >= psize;mx--)
		if(res[mx])
			{
			delta = mx - ipol[0];
			for(i=0;i<POLYSIZE;i++)
				res[ipol[i] + delta] ^= 1;
			}

	for(i=1;i<RESSIZE;i++)
		if(res[i] != 0) return 0;

	if(res[0] != 1) return 0;
	return 1;
	}

int f_evaluate(ELEMENT *x, int xsize,ELEMENT *r, int rsize)
	{
	unsigned int pot[POTSIZE];
	unsigned int pr;
	int cursor = 0;
	int i,j,k;

	for(i=0;i<POTSIZE;i++)
		pot[i] = lsvec[cursor++];

	for(j=0;j<xsize;j++)
		for(i=0;i<=j;i++)
			{
			pr = (x[i] & x[j]) != 0 ? 0xFFFFFFFF : 0x00;
			
			for(k=0;k<POTSIZE;k++)
				pot[k] ^= (lsvec[cursor++] & pr);
			}

	for(i=0;i<rsize;i++)
		{
		unsigned int bmask = (1 << (i & 0x1f) );
		
		if(pot[i >> 5] & bmask) r[i] = 0x01;
		else r[i] = 0;
		}

	return 1;
	}

int decode_license(const char *license, int *lic_id)
	{
	ELEMENT p[150];
	char undec[50];
	int delta;
	int cbits;
	int top,loops;
	int len;
	int i,j;
	int rr;

	if(!license) return -1;
	len = strlen(license);

	if(len >= 30) return 0;

	rr = undecorate_string(license,undec);
	if(rr != 1) return 0;

	len = strlen(undec);
	if(len != 25) return 0;

	cbits = 0;
	for(i=0;i<len;i++)
		{
		int k,v;
		
		v = char2val(undec[i]);
		if( v < 0 ) return 0;

		for(k=0;k<5;k++)
			p[cbits++] = (v >> k) & 0x01;
		}

	if(cbits != 125) return 0;

	delta = G_KEYSIZE - cbits;
	if(delta < 0) return -2;

	top = 1 << delta;

	// codes are 125 bit long, but I need 128 bit vectors, so
	// I'm gonna guess the remaining bits.
	for(loops=0;loops<top;loops++)
		{
		ELEMENT signature[G_RESULTSIZE];
		memset(signature,0x00,G_RESULTSIZE);

		for(j=0;j<delta;j++)
			p[cbits+j] = (loops >> j) & 0x01;

		// takes a 128 bit vector as input and outputs a 120 signature
		rr = f_evaluate(p,G_KEYSIZE,signature,G_RESULTSIZE);
		if(rr != 1) return -3;

		// the signature must meet some criteria to be validated, which one ?
		rr = verify_parity(signature,KEYIDSIZE,signature + KEYIDSIZE,G_RESULTSIZE - KEYIDSIZE);
		if(rr == 1) 
			{
			*lic_id = get_license_id(signature);

			rr = check_blacklist(*lic_id);
			if(rr == 1) return 2;	/* Signature verified but blacklisted */
			return 1; 	/* Signature verified */
			}
		}

	/* Signature not verified */
	return 0;	
	}

