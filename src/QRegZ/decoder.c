
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

#include "fileio.h"
#include "decoder.h"

const WORD_8 SB1[32] = {	0x0c,0x1f,0x0a,0x01,0x16,0x1a,0x09,0x19,
							0x0d,0x1e,0x14,0x03,0x02,0x17,0x13,0x0f,
							0x0e,0x12,0x1c,0x0b,0x04,0x07,0x08,0x05,
							0x06,0x10,0x00,0x15,0x1b,0x18,0x1d,0x11 };

const WORD_8 SB2[32] = {	0x09,0x03,0x1b,0x16,0x0e,0x1f,0x15,0x12,
							0x05,0x11,0x00,0x0b,0x07,0x01,0x18,0x06,
							0x0c,0x0a,0x1c,0x1a,0x13,0x14,0x04,0x08,
							0x02,0x1e,0x10,0x0d,0x1d,0x19,0x0f,0x17 };

const WORD_8 SB3[32] = {	0x0a,0x1a,0x1e,0x06,0x04,0x14,0x03,0x1b,
							0x10,0x0f,0x0e,0x15,0x00,0x0c,0x13,0x19,
							0x08,0x02,0x11,0x16,0x09,0x1c,0x1f,0x12,
							0x05,0x18,0x17,0x0b,0x01,0x07,0x0d,0x1d };

const WORD_8 SB4[32] = {	0x06,0x07,0x1e,0x0c,0x12,0x13,0x1b,0x19,
							0x08,0x14,0x0f,0x09,0x11,0x0b,0x0a,0x17,
							0x1a,0x15,0x00,0x04,0x0d,0x05,0x1c,0x1d,
							0x0e,0x03,0x1f,0x01,0x16,0x02,0x18,0x10 };

WORD_8 kg_coremix(WORD_8 a,WORD_8 b,WORD_8 c,WORD_8 d,WORD_8 e)
	{
	WORD_8 r1,r2;
	WORD_8 s;

	a &= 0x1f;
	b &= 0x1f;
	c &= 0x1f;
	d &= 0x1f;
	e &= 0x1f;

	r1 = SB1[(a + SB2[b])&0x1f];
	r2 = SB4[(SB3[c] ^ d)];

	s = (r1 & e) | (r2 & ((~e) & 0x1f));

	return s & 0x1f;	
	}

int kg_mixdown(WORD_8 *vector,int size)
	{
	int i,j;
	
	if(size < 6) return 0;

	for(i=0;i<MIXROUNDS;i++)
		{
		for(j=size-1;j>=0;j--)
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

int Char2Val(char c)
	{
	/* Those my be confused and are not possible results ! */
	/* If fed back to the decoder they are remapped to letters */
	if(c == '1') c='I';
	if(c == '0') c='O';
	if(c == '5') c='S';
	if(c == '8') c='B';

	/* Code 0-25 for alphabets letters. */
	if(isalpha(c)) return (int)(toupper(c) - 'A');

	/* Codes from 26 to 31 are for the six accepted digits. */

	if(c>='2' && c<'8') return 26 + ( c - '2' );
        
	if( c=='9' ) return 26 + ( '5' - '2' );
	return -1;    
	}

void collapse_five_to_eight(WORD_8 *src,WORD_8 *dst)
	{
	WORD_8 bytev[160];
	int i;

	memset(bytev,0x00,160);

	for(i=0;i<32;i++)
		int2vect(src[i],bytev + i*5,5);

	bytevect2bitvect(bytev,160,dst);
	}

int UndecorateQuartzCode(const char *deccode,char *undeccode)
	{
	size_t len = strlen(deccode);
	int j=0;
	size_t i;

	for(i=0;i<len;i++)
		{
		if(deccode[i]=='-') continue;
		else if(Char2Val(deccode[i]) < 0) return 0;

		if(j >= 32) return 0;
		undeccode[j++] = deccode[i];
		}

	undeccode[j]='\0';

	return 1;
	}

int TestSingleCode(QUARTZ_PUBLICKEY *qpub,WORD32 *id,const char *str)
	{
	char message[100];
	WORD_8 signature[QUARTZ_SIGNATURE_SIZE+sizeof(id)];
	char fixup[32];
	int res,i;

	for(i=0;i<32;i++)
		fixup[i] = Char2Val(str[i]);

	kg_mixdown((unsigned char *)fixup,32);

	memset(signature,0x00,QUARTZ_SIGNATURE_SIZE+sizeof(id));
	collapse_five_to_eight((unsigned char *)fixup,signature);

	*id = *(WORD32 *)(signature + QUARTZ_SIGNATURE_SIZE);

	sprintf(message,"%08x",*id);

	res = QuartzVerifySignature((unsigned char *)message,(int)strlen(message),qpub,signature);
	if(res != 1) return 0;
	
	return 1;
	}

