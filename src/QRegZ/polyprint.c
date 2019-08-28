
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

#include "time.h"
#include "polyprint.h"

#include "ffpoly.h"
#include "expoly.h"
#include "osdepn.h"

void poly_sprint(FFPOLY *pr,char *buffer,char c)
	{
	char str[200];
	int i,j=0;

	buffer[0] = '\0';
	
	if(pr->pvdeg < 0) strcat(buffer,"0");

	for(i=pr->pvdeg;i>=0;i--)
		{
		if((pr->pv[i >> PWORDBITS] >> ( i & (PWORDSIZE - 1)) & 0x01) != 0)
			{
			if( j == 1 ) strcat(buffer,"+");
			if( i > 1 ) 
				{
				sprintf(str,"%c^%d",c,i);
				strcat(buffer,str);
				}
			else if(i == 1) 
				{
				char str[2];
				str[0] = c;
				str[1] = '\0';

				strcat(buffer,str);
				}
			else strcat(buffer,"1");
			j = 1;
			}
		}
	
	}

void poly_print(FFPOLY *pr,const char *cap)
	{
	char buffer[4096];

	if(!pr) return;

	if(cap != NULL) printf("%s",cap);

	poly_sprint(pr,buffer,'x');
	printf("%s\n",buffer);
	}

void expoly_print(EXPOLY *p,const char *cap)
	{
	char buffer[4096];
	int j,k=0;
	int zero,one;

	if(!p) return;

	if(cap != NULL) printf("%s",cap);

	for(j=p->pvdeg;j>=0;j--)
		{
		poly_sprint(&p->ep[j],buffer,'t');

		zero = ffpoly_is_zero(&p->ep[j]);
		one  = ffpoly_is_identity(&p->ep[j]);

		if(zero == 0)
			{
			if(k!=0) printf("+");

			if(one == 0)
				{
				if(j>1) printf("(%s)*x^%d",buffer,j);
				else if(j==1) printf("(%s)*x",buffer);
				else printf("(%s)",buffer);
				}
			else
				{
				if(j>1) printf("x^%d",j);
				else if(j==1) printf("x");
				else printf("1");
				}

			k=1;
			}
		}

	if(k==0) printf("0");

	printf("\n");
	}
	
void ffpoly_dump(FFPOLY *p)
	{
	int ln = (p->pvdeg + 31) / 32;
	int i;

	for(i=0;i<=ln;i++)
		printf("%08x ",p->pv[i]);

	printf(" (%d)\n",p->pvdeg);
	}

void expoly_twindump(EXPOLY *a,EXPOLY *b)
	{
	int i;
	int top;

	top = MAX(a->pvdeg,b->pvdeg);

	for(i=0;i<=top;i++)
		{
		if(i<=a->pvdeg)
			{
			printf("%3d[a]: ",i);
			ffpoly_dump(&a->ep[i]);
			}
		if(i<=b->pvdeg)
			{
			printf("%3d[b]: ",i);
			ffpoly_dump(&b->ep[i]);
			}

		printf("--------------------------------------------------\n");
		}
	}

void ffpoly_dumpdata(FFPOLY *fp, int cr)
	{
	int i;

	for(i=0;i<SCALED_DEGREE;i++) 
		{
		if(i > 0) printf(",");
		printf(" 0x%08x",fp->pv[i]);
		}

	if(cr) printf("\n");
	}

void expoly_dumpdata(EXPOLY *e)
{
	int i,count=0;

	for(i=e->pvdeg;i>=0;i--) {
		if(!ffpoly_is_zero(&e->ep[i])) count++;
	}

	printf("DUMPTYPE terms[%d * (SCALED_DEGREE+1)] = {\n",count);

	for(i=e->pvdeg;i>=0;i--) {
		if(ffpoly_is_zero(&e->ep[i])) continue;

		printf("{ %3d,",i);
		ffpoly_dumpdata(&e->ep[i],0);

		printf(" },\n");
	}

	printf("}; // -------------------------------------------------\n");
}





