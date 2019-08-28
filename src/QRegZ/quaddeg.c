

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "quaddeg.h"
#include "osdepn.h"

#include "arc4.h"

#define ROWMAXLENGTH 10000

#define ROWMAXTOTSIZE (((ROWMAXLENGTH) * (ROWMAXLENGTH) + (ROWMAXLENGTH) + 2) /2)


/*  1 + n + (n^2 - n) / 2  */


int QuadRowSize(int n)
	{
	if(n<1 || n>=ROWMAXLENGTH) return -1;
	return (n*n + n + 2) / 2;
	}

int QuadIdFromXX(int x1,int x2,int a)
	{
	if(x1 < -1 || x1 >= ROWMAXLENGTH || 
	   x2 < -1 || x2 >= ROWMAXLENGTH || 
	   a <  1 || a >= ROWMAXLENGTH) return -1;

	if(x1 == -1 && x2 == -1) return 0;
	if(x1 == x2) return 1 + x1;

	if( x2 > x1 ) return ((2*a - 3 - x1) * x1 ) / 2 + a + x2;
	else return ((2*a - 3 - x2) * x2 ) / 2 + a + x1;
	}

int QuadSysInit(QUADSYS *q,int size)
	{
	int i;

	if(!q) return -1;

	q->rowlen = QuadRowSize(size);
	if(q->rowlen <= 0) return -2;

	q->size   = size;

	q->sys = (WORD_8 **)malloc(q->size * sizeof(WORD_8 *));
	if(!q->sys) return -2;

	memset(q->sys,0x00,q->size * sizeof(WORD_8 *));

	for(i=0;i<q->size;i++)
		{
		q->sys[i] = (WORD_8 *)malloc(q->rowlen * sizeof(WORD_8));
		if(!q->sys[i]) return -3;
		}

	return 1;
	}

int QuadSysFree(QUADSYS *q)
	{
	if(!q) return 0;

	if(q->sys)
		{
		int i;

		for(i=0;i<q->size;i++)
			{
			if(q->sys[i]) 
				{
				free(q->sys[i]);
				q->sys[i] = NULL;
				}
			}

		free(q->sys);
		}

	q->sys = NULL;
	return 1;
	}

int QuadSysCreate(RC4_CTX *rc4,QUADSYS *q,int flock)
	{
	int i,j,k;

	for(i=0;i<q->size;i++)
		{
		int level = (1 + (i / flock)) * flock;

		q->sys[i][0] = (RC4_GetByte(rc4) & 0x20 ? 1 : 0);

		for(j=0;j<q->size;j++)
			{
			for(k=j;k<q->size;k++)
				{
				int ofs = QuadIdFromXX(j,k,q->size);

				if(j < level && k < level) 
					q->sys[i][ofs] = (RC4_GetByte(rc4) & 0x20 ? 1 : 0);
				else 
					q->sys[i][ofs] = 0;
				}
			}
		}

	return 1;
	}




