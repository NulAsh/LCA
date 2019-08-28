
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include "lintra.h"
#include "arc4.h"

#include <stdio.h>
#include <malloc.h>
#include <math.h> 
#include <string.h>
#include <stdlib.h>


/* ********************************************************************* */
/* Printing modules */
/* ********************************************************************* */

void print_vector(WORD_8 *vector,int size,char *cap)
	{
	WORD_8 acc;
	int i;

	if(cap) printf("%s",cap);

	acc = 0;

	for(i=0;i<size;i++)
		{
		if(vector[i] != 0) acc |= (0x80 >> (i & 0x07));

		if((i & 0x07) == 0x07)
			{
			printf("%02x",acc);
			acc=0;
			}

		if((i & 0x1f) == 0x1f) printf(" ");
		}

	if((i & 0x07) != 0) printf("%02x",acc);

	printf("\n");
	}

void print_adjmtx(WORD_8 **temp,int len)
	{
	int i,j;

	for(j=0;j<len;j++)
		{
		for(i=0;i<2*len;i++)
			{
			if(i==len) printf("- ");
			printf("%d ",temp[j][i]);
			}

		printf("\n");
		}
	}

void print_mtx(WORD_8 **mtx,int size)
	{
	int i,j;

	for(i=0;i<size;i++)
		{
		printf("//  ");

		for(j=0;j<size;j++)
			{
			printf("%d",mtx[i][j]);
			}

		printf("\n");
		}
	}

int PrintBitBuffer(WORD_8 *buff,int size)
	{
	int i;

	for(i=0;i<size;i++)
		{
		printf("%d ",buff[i]!=0);
		if(i%32 == 31) printf("\n");
		}
	printf("\n\n");

	return 1;
	}

int PrintBuffer(WORD_8 *buff,int size)
	{
	int i;

	for(i=0;i<size;i++)
		{
		printf("%02x ",buff[i]);
		if(i%16 == 15) printf("\n");
		}
	printf("\n\n");

	return 1;
	}

/* ********************************************************************* */

void ldt(WORD_8 *inBuff,WORD_8 *outBuff,int **mask,int len)
	{
	int i,j;

	for(i=0;i<len;i++)
		{
		outBuff[i] = 0x00;
		for(j=0;j<len;j++)
			if(mask[i][j]) 
				outBuff[i] ^= inBuff[j];
		}
	}

void copymtx(WORD_8 **dest,WORD_8 **src,int len)
	{
	int i,j;

	for(i=0;i<len;i++)
		for(j=0;j<len;j++)
			dest[i][j] = src[i][j];
	}

void trans_mtx(WORD_8 **dest,WORD_8 **src,int len)
	{
	int i,j;

	for(i=0;i<len;i++)
		for(j=0;j<len;j++)
			dest[i][j] = src[j][i];
	}

void vect_mult(WORD_8 **A,WORD_8 *X,int len,WORD_8 *Y)
	{
	int i,j;

	for(i=0;i<len;i++)
		{
		Y[i] = 0;

		for(j=0;j<len;j++)
			Y[i] ^= (A[i][j] & X[j]);
		}
	}

void row_mult(WORD_8 **A,WORD_8 *X,int len,WORD_8 *Y)
	{
	int i,j;

	for(i=0;i<len;i++)
		{
		Y[i] = 0;

		for(j=0;j<len;j++)
			Y[i] ^= (A[j][i] & X[j] );
		}
	}

void mtx_mult(WORD_8 **A,WORD_8 **B,int len,WORD_8 **R)
	{
	int i,j,k;

	for(i=0;i<len;i++)
		{
		for(j=0;j<len;j++)
			{
			R[i][j] = 0;

			for(k=0;k<len;k++)
				{
				R[i][j] ^= (A[i][k] & B[k][j]);
				}
			}

		}
	}


int invmtx(WORD_8 **mIn,WORD_8 **mOut,int len)
	{
	WORD_8 **temp;
	int i,j;
	int pivot;
	int fail = 0;

	temp = (WORD_8 **)malloc(len * sizeof(WORD_8 *));

	for(j=0;j<len;j++)
		temp[j] = (WORD_8 *)malloc(2 * len);

	for(j=0;j<len;j++)
		{
		for(i=0;i<2*len;i++)
			{
			if(i<len) temp[j][i] = mIn[j][i];
			else temp[j][i] = ((i-len) == j) ? 1 : 0;
			}
		}

	pivot = 0;

	while(pivot < len)
		{
		if(!temp[pivot][pivot])   /* Requires pivoting... */
			{
			int pv=pivot+1;
			int flag = 1;

			while(pv < len && flag)
				{
				if(temp[pv][pivot])
					{
					WORD_8 *t = temp[pivot];
					temp[pivot] = temp[pv];
					temp[pv] = t;
					flag = 0;
					}

				pv++;
				}

			if(flag) 
				{
				fail = 1;
				break;
				}
			}
		
		for(j=0;j<len;j++)
			{
			if(j == pivot) continue;   /* Skip the pivoted column ! */

			if(temp[j][pivot])
				{
				for(i=0;i<2*len;i++)
					temp[j][i] ^= temp[pivot][i];
				}
			}

		pivot++;
		}

	for(j=0;j<len;j++)
		{
		for(i=0;i<len;i++)
			mOut[j][i] = temp[j][i+len];
		}

	for(i=0;i<len;i++)
		free(temp[i]);

	free(temp);

	return !fail;
	}

int create_slide_matrix(WORD_8 *vect,WORD_8 **mtx,int size)
	{
	int i,j;

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			mtx[i][j] = vect[(i+j) % size];

	return 1;
	}

int create_invertible_matrix(RC4_CTX *rc4,WORD_8 **mtx,int size)
	{
	int row1,row2,action;
	int rounds=size * 100;
	int i,j;
	WORD_8 *tmp;

	for(i=0;i<size;i++)
		for(j=0;j<size;j++)
			mtx[i][j] = (i==j ? 1 : 0);

	while(rounds > 0)
		{
		action = RC4_GetByte(rc4) & 0x01;
		do
			{
			row1 = RC4_GetLong(rc4) % size;
			row2 = RC4_GetLong(rc4) % size;
			} while(row1 == row2);

		switch(action)
			{
			case 0:
				tmp = mtx[row1];
				mtx[row1] = mtx[row2];
				mtx[row2] = tmp;
				break;

			case 1:
				for(i=0;i<size;i++)
					mtx[row2][i] ^= mtx[row1][i];
				break;
			}

		rounds--;
		}
	
	return 1;
	}

int buffswap(int *buff1,int size,int a,int b,int *buff2)
	{
	int tmp;
	int c,d;

	if(size<0) return 0;
	if(a<0 || a>=size || b<0 || b>=size) return 0;

	if(a==b) return 2;

	c = buff1[a];
	d = buff1[b];

	tmp = buff2[c];
	buff2[c] = buff2[d];
	buff2[d] = tmp;

	buff1[a] = d;
	buff1[b] = c;
	
	return 1;
	}

int buffinit(int *buff1,int *buff2,int size)
	{
	int i;

	for(i=0;i<size;i++)
		{
		if(buff1) buff1[i] = i;
		if(buff2) buff2[i] = i;
		}
	
	return 1;
	}

