
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

#include "quartzsig.h"
#include "lintra.h"
#include "qmtx.h"

static const WORD32 parity[256] = {
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
	0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0
	};


int QMtx_PackVector(WORD_8 *vector,int len,WORD32 *vect)
	{
	int sz = (len + 31) >> 5;

	while(len >= 32)
		{
		*vect = 0;

		*vect |= ((vector[ 0] & 0x01)      );
		*vect |= ((vector[ 1] & 0x01) <<  1);
		*vect |= ((vector[ 2] & 0x01) <<  2);
		*vect |= ((vector[ 3] & 0x01) <<  3);
		*vect |= ((vector[ 4] & 0x01) <<  4);
		*vect |= ((vector[ 5] & 0x01) <<  5);
		*vect |= ((vector[ 6] & 0x01) <<  6);
		*vect |= ((vector[ 7] & 0x01) <<  7);

		*vect |= ((vector[ 8] & 0x01) <<  8);
		*vect |= ((vector[ 9] & 0x01) <<  9);
		*vect |= ((vector[10] & 0x01) << 10);
		*vect |= ((vector[11] & 0x01) << 11);
		*vect |= ((vector[12] & 0x01) << 12);
		*vect |= ((vector[13] & 0x01) << 13);
		*vect |= ((vector[14] & 0x01) << 14);
		*vect |= ((vector[15] & 0x01) << 15);

		*vect |= ((vector[16] & 0x01) << 16);
		*vect |= ((vector[17] & 0x01) << 17);
		*vect |= ((vector[18] & 0x01) << 18);
		*vect |= ((vector[19] & 0x01) << 19);
		*vect |= ((vector[20] & 0x01) << 20);
		*vect |= ((vector[21] & 0x01) << 21);
		*vect |= ((vector[22] & 0x01) << 22);
		*vect |= ((vector[23] & 0x01) << 23);

		*vect |= ((vector[24] & 0x01) << 24);
		*vect |= ((vector[25] & 0x01) << 25);
		*vect |= ((vector[26] & 0x01) << 26);
		*vect |= ((vector[27] & 0x01) << 27);
		*vect |= ((vector[28] & 0x01) << 28);
		*vect |= ((vector[29] & 0x01) << 29);
		*vect |= ((vector[30] & 0x01) << 30);
		*vect |= ((vector[31] & 0x01) << 31);

		len -= 32;
		vector+=32;
		vect++;
		}

	if(len > 0)
		{
		int i;

		*vect = 0;
		for(i=0;i<len;i++)
			*vect |= ((vector[i] & 0x01) << i);

		}

	return sz;
	}

int QMtx_UnPackVector(WORD32 *vect,WORD_8 *vector,int len)
	{
	while(len > 32)
		{
		vector[ 0] = ((*vect) >>  0) & 0x01;
		vector[ 1] = ((*vect) >>  1) & 0x01;
		vector[ 2] = ((*vect) >>  2) & 0x01;
		vector[ 3] = ((*vect) >>  3) & 0x01;
		vector[ 4] = ((*vect) >>  4) & 0x01;
		vector[ 5] = ((*vect) >>  5) & 0x01;
		vector[ 6] = ((*vect) >>  6) & 0x01;
		vector[ 7] = ((*vect) >>  7) & 0x01;

		vector[ 8] = ((*vect) >>  8) & 0x01;
		vector[ 9] = ((*vect) >>  9) & 0x01;
		vector[10] = ((*vect) >> 10) & 0x01;
		vector[11] = ((*vect) >> 11) & 0x01;
		vector[12] = ((*vect) >> 12) & 0x01;
		vector[13] = ((*vect) >> 13) & 0x01;
		vector[14] = ((*vect) >> 14) & 0x01;
		vector[15] = ((*vect) >> 15) & 0x01;

		vector[16] = ((*vect) >> 16) & 0x01;
		vector[17] = ((*vect) >> 17) & 0x01;
		vector[18] = ((*vect) >> 18) & 0x01;
		vector[19] = ((*vect) >> 19) & 0x01;
		vector[20] = ((*vect) >> 20) & 0x01;
		vector[21] = ((*vect) >> 21) & 0x01;
		vector[22] = ((*vect) >> 22) & 0x01;
		vector[23] = ((*vect) >> 23) & 0x01;

		vector[24] = ((*vect) >> 24) & 0x01;
		vector[25] = ((*vect) >> 25) & 0x01;
		vector[26] = ((*vect) >> 26) & 0x01;
		vector[27] = ((*vect) >> 27) & 0x01;
		vector[28] = ((*vect) >> 28) & 0x01;
		vector[29] = ((*vect) >> 29) & 0x01;
		vector[30] = ((*vect) >> 30) & 0x01;
		vector[31] = ((*vect) >> 31) & 0x01;

		len-=32;
		vector+=32;
		vect++;
		}

	if(len > 0)
		{
		int i;

		for(i=0;i<len;i++)
			vector[i] = (*vect >> i) & 0x01;
		}

	return 1;
	}

void QMtxVect4Mul(WORD32 *mtx,WORD32 *in,WORD32 *out)
	{
	WORD16 sum;
	int i,j=0;
	union 
		{
		WORD_8 b[16];
		WORD32 l[4];
		} tmp;

	memset(out,0x00,16);

	for(i=0;i<QUARTZ_AFFINE_TRANSSIZE;i++)
		{
		sum = 0;

		tmp.l[0] = mtx[0] & in[0];
		tmp.l[1] = mtx[1] & in[1];
		tmp.l[2] = mtx[2] & in[2];
		tmp.l[3] = mtx[3] & in[3];

		sum |= (parity[ tmp.b[ 0] ]      );
		sum |= (parity[ tmp.b[ 1] ] <<  1);
		sum |= (parity[ tmp.b[ 2] ] <<  2);
		sum |= (parity[ tmp.b[ 3] ] <<  3);
		sum |= (parity[ tmp.b[ 4] ] <<  4);
		sum |= (parity[ tmp.b[ 5] ] <<  5);
		sum |= (parity[ tmp.b[ 6] ] <<  6);
		sum |= (parity[ tmp.b[ 7] ] <<  7);

		sum |= (parity[ tmp.b[ 8] ] <<  8);
		sum |= (parity[ tmp.b[ 9] ] <<  9);
		sum |= (parity[ tmp.b[10] ] << 10);
		sum |= (parity[ tmp.b[11] ] << 11);
		sum |= (parity[ tmp.b[12] ] << 12);
		sum |= (parity[ tmp.b[13] ] << 13);
		sum |= (parity[ tmp.b[14] ] << 14);
		sum |= (parity[ tmp.b[15] ] << 15);

		sum = parity[sum >> 8] ^ parity[sum & 0xff];

		mtx += 4;

		*out |= (sum << j);

		j++;
		if(j == 32)
			{
			j=0;
			out++;
			}
		}

	}








