
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
#include "polytest.h"
#include "irredpoly.h"

#include "ffpoly.h"
#include "expoly.h"
#include "expolymod.h"
#include "rootfind.h"
#include "quartzsig.h"

#include "arc4.h"
#include "osdepn.h"

#include "karatsuba.h"
#include "fred103.h"

// #define PRINT_DEBUG

RC4_CTX gRC4;

//static int poly_1[4] = { 253,241,240,235 };
static int poly_1[4] = { 141,123,121,119 };
//static int poly_2[3] = { 7,3,0 };
static int poly_3[3] = { 103,9,0 };
//static int poly_4[3] = { 6,1,0 };

static int poly_5[3] = { 9,4,0 };

//static int poly_6[4] = { 127,109,37,1 };
static int poly_7[5] = { 4,3,2,1,0 };

//static int p1_test[9] = { 21,17,14,13,9,7,6,3,0 };
//static int p2_test[3] = { 14,6,0 };

//static int pv_test[3] = { 3,1,0 };
//static int pw_test[2] = { 2,1 };

//static int p_irred[5] = { 4,3,2,1,0 };

static int p_iri_1[3] = { 43,9,0 };
static int p_iri_2[3] = { 43,3,0 };
static int p_iri_3[5] = { 4,3,2,0 };

static int p_iri_4[3] = { 103,9,1 };
//static int p_iri_5[3] = {  67,9,1 };

static int p_bigirred[5] = { 121, 8, 5, 1, 0 };

//#define TEST_ADDITION
//#define TEST_MULTIPLICATION
//#define TEST_LEFTNRIGHTSHIFTS
//#define TEST_LONGDIVISION
//#define TEST_GCD
//#define TEST_SUBSTITUTION
#define TEST_POLYFACTOR

/*
static int p_uru[42] = { 
	100,96,95,93,90,88,85,83,82,77,75,73,72,71,68,64,
	 63,61,57,45,44,43,40,39,37,33,32,29,27,26,25,24,
	 23,22,21,18,17,12,11,8,4,2 };
*/
#define MAXFACTORS   200

WORD16 irredpolys[125] = {
 0x00b,0x031,0x052,0x034,0x038,0xb11,0x302,0x904,0x508,0x900,0xb10,0x120,0x300,0xb20,0x900,
 0x900,0x720,0x900,0x500,0x300,0x120,0xb10,0x900,0xb10,0x720,0x300,0x500,0x300,0x900,0xd80,
 0xb40,0xb10,0x500,0x530,0xf30,0x360,0x110,0x930,0x900,0x720,0x950,0x120,0xb10,0x300,0x120,
 0xd20,0x170,0xd10,0xb40,0x900,0x740,0xd70,0x740,0x590,0x110,0x360,0xb70,0x300,0x720,0x960,
 0x300,0xb10,0xb10,0x900,0x720,0x3a0,0x560,0xb20,0xb20,0xf50,0xd10,0x740,0xb40,0x530,0x560,
 0xf50,0xd10,0xfa0,0x110,0x7d0,0x590,0x120,0x701,0x560,0x3a0,0xf30,0x960,0xd20,0xde0,0x560,
 0x500,0x360,0x770,0xf60,0x140,0x990,0xb40,0x560,0x3c0,0x960,0xdb0,0xb10,0x110,0x360,0xfa0,
 0x350,0x530,0x350,0x590,0x930,0xd20,0xd20,0xfa0,0x710,0x720,0x560,0x101,0xb10,0x321,0x740,
 0x500,0xd70,0xfa0,0x590,0x300
 };

int poly_rand(FFPOLY *p,int maxdegree)
	{
	int ptest[MAXDEGREE];
	int i,j=0;

	if(maxdegree <= 0 || maxdegree >= MAXDEGREE) return 0;

	ptest[j++] = maxdegree;

	ffpoly_zeroset(p);

	for(i=maxdegree-1,j=0;i>=0;i--)
		{
		if(RC4_GetByte(&gRC4) & 0x80) ptest[j++] = i;
		}
	
	ffpoly_set(p,ptest,j);
	return 1;
	}

int expoly_idrand(EXPOLY *p,int maxdegree)
	{
	FFPOLY tmp;

	if( poly_rand(&tmp,maxdegree) != 1 ) return 0;
	
	if(p->pvsize <= maxdegree) return 0;

	expoly_setfrompoly(p,&tmp);
	p->pvdeg = tmp.pvdeg;

	return 1;
	}

int expoly_rand(EXPOLY *p,int maxdegree,int polydeg)
	{
	int i,cdeg=-1;

	if(!p || p->pvsize<=0) return 0;
	if(p->pvsize <= maxdegree) return 0;

	expoly_zeroset(p);

	do
		{
		for(i=maxdegree;i>=0;i--)
			{
			//if(RC4_GetByte(&gRC4) & 0x20) continue;
			if(cdeg == -1) cdeg = i;

			do
				{
				poly_rand(&p->ep[i],polydeg);
				} while( ffpoly_is_zero(&p->ep[i]) != 0 );
			}

		p->pvdeg = cdeg;
		} while(p->pvdeg == -1);

	return 1;
	}

int expoly_monic_rand(EXPOLY *p,int maxdegree,int polydeg)
	{
	int r;

	r = expoly_rand(p,maxdegree,polydeg);
	if(r!=1) return r;
	
	ffpoly_identity_set(&p->ep[maxdegree]);
	p->pvdeg = maxdegree;
	return 1;
	}



/* *************************************************************** */

#define TEST_MAXDEGREE 255

int poly_test_addition(int maxloops,int printeach)
	{
	FFPOLY p1,p2,modulus;
	FFPOLY r1,r2,r3,r4;
	FFPOLY t1,t2;
	int maxdegree = TEST_MAXDEGREE;
	int loops;

	ffpoly_set(&modulus,poly_3,3);

	printf("Testing addition...\n");
	
	loops=0;
	while( loops < maxloops )
		{
		if( poly_rand(&p1,maxdegree) == 0) return -1;
		if( poly_rand(&p2,maxdegree) == 0) return -2;

		ffpoly_copy(&t1,&p1);
		ffpoly_copy(&t2,&p2);

		if( ffpoly_reduce(&t1,&modulus) == 0) return -3;
		if( ffpoly_reduce(&t2,&modulus) == 0) return -4;

		if( ffpoly_addmod(&r1,&p1,&p2,&modulus) == 0 ) return -5;
		if( ffpoly_addmod(&r2,&p2,&p1,&modulus) == 0 ) return -6;

		if( ffpoly_addmod(&r3,&t1,&t2,&modulus) == 0 ) return -7;
		if( ffpoly_addmod(&r4,&t2,&t1,&modulus) == 0 ) return -8;

		if(ffpoly_cmp(&r1,&r2) != 0 || 
		   ffpoly_cmp(&r3,&r4) != 0 || 
		   ffpoly_cmp(&r1,&r3) != 0)
			{
			poly_print(&p1,"P1 = ");
			poly_print(&p2,"P2 = ");

			poly_print(&r1,"R1 = ");
			poly_print(&r2,"R2 = ");
			poly_print(&r3,"R3 = ");
			poly_print(&r4,"R4 = ");

			printf("Error !\n");
			return -9;
			}

		if((printeach > 0) && (loops % printeach) == (printeach-1))
			printf("%d\r",loops+1);
		loops++;
		}

	return 1;
	}

int poly_test_multiplication(int maxloops,int printeach)
	{
	FFPOLY p1,p2,modulus;
	int maxdegree = TEST_MAXDEGREE;
	int loops;
	FFPOLY r1,r2;
	FFPOLY sq1,sq2;

	ffpoly_set(&modulus,poly_3,3);

	// Testing multiplication
	printf("Testing multiplication...\n");
	
	loops=0;
	while( loops < maxloops )
		{
		if( poly_rand(&p1,maxdegree/7) == 0) return -20;
		if( poly_rand(&p2,maxdegree/7) == 0) return -21;

		if( ffpoly_reduce(&p1,&modulus) == 0) return -22;
		if( ffpoly_reduce(&p2,&modulus) == 0) return -23;

		if( ffpoly_mulmod(&r1,&p1,&p2,&modulus) == 0 ) return -24;
		if( ffpoly_mulmod(&r2,&p2,&p1,&modulus) == 0 ) return -25;

		if( ffpoly_mulmod(&sq1,&p1,&p1,&modulus) == 0 ) return -26;
		if( ffpoly_square(&sq2,&p1,&modulus) == 0 ) return -27;

		if(ffpoly_cmp(&r1,&r2) != 0)
			{
			poly_print(&p1,"P1 = ");
			poly_print(&p2,"P2 = ");

			poly_print(&r1,"R1 = ");
			poly_print(&r2,"R2 = ");

			printf("Error in multiplication!\n");
			return -28;
			}

		if(ffpoly_cmp(&sq1,&sq2) != 0)
			{
			poly_print(&p1 ,"P1 =");
			poly_print(&sq1,"SQ1=");
			poly_print(&sq2,"SQ2=");

			printf("Error in squaring!\n");
			return -28;
			}

		if((printeach > 0) && (loops % printeach) == (printeach-1))
			printf("%d\r",loops+1);
		loops++;
		}

	return 1;
	}

int poly_test_shifts(int maxloops,int printeach)
	{
	int maxdegree = TEST_MAXDEGREE;
	FFPOLY r1,r2;
	FFPOLY p1;
	int loops;
	int i;

	printf("Testing left and right shifts...\n");
	
	loops=0;
	while( loops < maxloops )
		{
		if( poly_rand(&p1,maxdegree/2) == 0) return -20;

		for(i=0;i<MAXDEGREE - p1.pvdeg;i++)
			{
			ffpoly_shiftleft(&r1,&p1,i);
			ffpoly_shiftright(&r2,&r1,i);

			if(ffpoly_cmp(&p1,&r2) != 0)
				{
				poly_print(&p1,"P1 = ");

				poly_print(&r1,"R1 = ");
				poly_print(&r2,"R2 = ");

				printf("Shift count = %d\n",i);

				printf("Error !\n");
				return -26;
				}
			}

		if((printeach > 0) && (loops % printeach) == (printeach-1))
			printf("%d\r",loops+1);
		loops++;
		}

	return 1;
	}
/*
4a1ce908 d1b2d468 6352ef33 0000007e 00000000
6e7cfeb6 68190f40 0e880bf9 00000000
a34b5ae9 44670bf7 71628a82 00000000

4a1ce908 d1b2d468 6352ef33 0000007e 00000000
6e7cfeb6 68190f40 0e880bf9 00000000
a34b5ae9 44670bf7 71628a82 0000000b 00000000
*/
int poly_test_fielddivision(int maxloops,int printeach)
	{
	FFPOLY p1,p2,mod;
	FFPOLY s1,s2,s3;
	int loops = 0;
	
	ffpoly_set(&mod,poly_3,3);

	printf("Testing field division...\n");
	while(loops < maxloops)
		{
		do
			{
			ffpoly_zeroset(&p1);
			ffpoly_zeroset(&p2);
			
			poly_rand(&p1,5 + (RC4_GetByte(&gRC4)%(mod.pvdeg-6)));
			poly_rand(&p2,(RC4_GetByte(&gRC4)%3)+(p1.pvdeg-4));

			ffpoly_reduce(&p1,&mod);
			ffpoly_reduce(&p2,&mod);
			} while(p1.pvdeg == -1 || p2.pvdeg == -1);

		ffpoly_zeroset(&s1);
		ffpoly_field_div(&s1,&p1,&p2,&mod);

		ffpoly_zeroset(&s2);

		ffpoly_mulmod(&s2,&p2,&s1,NULL);
		ffpoly_copy(&s3,&s2);
		ffpoly_reduce(&s2,&mod);

		if(ffpoly_cmp(&p1,&s2)!=0)
			{
			ffpoly_dump(&p1);

			ffpoly_dump(&s1);
			ffpoly_dump(&p2);
	
			ffpoly_dump(&s2);
			ffpoly_dump(&s3);

			poly_print(&p1 ,"P1    = ");
			poly_print(&p2 ,"P2    = ");
			poly_print(&s1 ,"P1/P2 = ");
			poly_print(&s2 ,"S1*P2 = ");
			poly_print(&mod,"Mod   = ");
			
			if(ffpoly_cmp(&p1,&s2)!=0) return 0;
			}

		/*
		poly_print(&p1 ,"P1    = ");
		poly_print(&p2 ,"P2    = ");
		poly_print(&s1 ,"P1/P2 = ");
		poly_print(&s2 ,"S1*P2 = ");
		poly_print(&mod,"Mod   = ");
		*/

		if((printeach > 0) && (loops % printeach) == (printeach-1))
			printf("%d\r",loops+1);

		// printf("Equal !\n");

		loops++;
		}

	return 1;
	}

int poly_test_longdivision(int maxloops,int printeach)
	{
	FFPOLY p1,p2,p3;
	int maxdegree = TEST_MAXDEGREE;
	FFPOLY t1,t2;
	FFPOLY r1;
	int loops;

	// Testing long division 
	printf("Testing long division...\n");
	
	loops=0;
	while( loops < maxloops )
		{
		if( poly_rand(&p1,maxdegree/2) == 0) return -20;
		if( poly_rand(&p2,maxdegree/2) == 0) return -20;
		if( poly_rand(&p3,maxdegree/2) == 0) return -20;

		ffpoly_mulmod(&r1,&p1,&p2,NULL);
		ffpoly_reduce(&p3,&p2);
		ffpoly_addmod(&r1,&r1,&p3,NULL);

		ffpoly_divide(&t1,&t2,&r1,&p2);
		
		if(ffpoly_cmp(&t1,&p1) != 0 ||
		   ffpoly_cmp(&t2,&p3) != 0)
			{
			poly_print(&p1,"P1 = ");
			poly_print(&p2,"P2 = ");
			poly_print(&p3,"P3 = ");
			
			poly_print(&r1,"P1*P2 + (P3%P2) = ");

			poly_print(&t1,"QS = ");
			poly_print(&t2,"RM = ");
			}
		
		if((printeach > 0) && (loops % printeach) == (printeach-1))
			printf("%d\r",loops+1);
		loops++;
		}

	return 1;
	}

int poly_test_gcd(int maxloops,int printeach)	
	{
	FFPOLY p1,p2;
	int maxdegree = TEST_MAXDEGREE;
	FFPOLY t1,t2;
	FFPOLY r1,r2,r3;
	int loops;

	// Testing long division
	printf("Testing gcd division...\n");
	
	loops=0;
	while( loops < maxloops )
		{
		if( poly_rand(&p1,maxdegree) == 0) return -20;
		if( poly_rand(&p2,maxdegree) == 0) return -20;

		if(ffpoly_gcd(&r3,&p1,&p2) == 0) return -101;

		if(ffpoly_divide(&r1,&t1,&p1,&r3) == 0) return -102;
		if(ffpoly_divide(&r2,&t2,&p2,&r3) == 0) return -103;

		if(t1.pvdeg >= 0 || t2.pvdeg >= 0)
			{
			printf("gcd error\n");
			
			poly_print(&p1,"P1 = ");
			poly_print(&p2,"P2 = ");
			poly_print(&r3,"GCD= ");
			poly_print(&r1,"RM_P1/GCD = ");
			poly_print(&r2,"RM_P2/GCD = ");

			return -104;
			}

		if((printeach > 0) && (loops % printeach) == (printeach-1))
			printf("%d\r",loops+1);
		loops++;
		}

	return 1;
	}

int poly_test_substitution()
	{
	FFPOLY p1,p2,modulus;
	FFPOLY r1;

	printf("Testing substitution...\n");
	ffpoly_set(&modulus,p_iri_1,3);

	ffpoly_set(&p1,p_iri_2,3);
	ffpoly_set(&p2,p_iri_3,4);

	ffpoly_subst(&r1,&p1,&p2,&modulus);

	//poly_print(&p1,"P1: ");
	//poly_print(&p2,"P2: ");
	//poly_print(&r1,"R1: ");

	return 1;
	}

/*
int poly_test_factor(int maxloops,int printeach)
	{
	FFPOLY p1,p2,p3,modulus;
	int maxdegree = TEST_MAXDEGREE;
	FFPOLY t1,t2;
	FFPOLY r1,r2;
	FFPOLY **factors;
	int factsNo;
	int i,j;
	int rVal = 1;

	ffpoly_set(&modulus,p_iri_1,3);
	//srand(time(NULL));

    // five solutions, hidden in a square

	ffpoly_set(&r1,p_iri_2,3);
	poly_print(&r1,"R1=");

	factsNo = MAXFACTORS;

	factors = (FFPOLY **)malloc( MAXFACTORS * sizeof(FFPOLY *) );
	for(i=0;i<MAXFACTORS;i++)
		factors[i] = (FFPOLY *)malloc(sizeof(FFPOLY));

	if( ffpoly_factor(factors,&factsNo,&r1,modulus.pvdeg) == 0) 
		{
		printf("Factorization error !\n");

		rVal = -220;
		goto EXIT_HERE;
		}

	printf("-----------------------------------------\n");

	ffpoly_copy(&t1,&r1);

	if(factsNo == 0) printf("No factorization found...\n");

	for(i=0;i<factsNo;i++)
		{
		FFPOLY qs,rs;
		char tmp[200];

		sprintf(tmp,"ROOT_%02d = ",i+1);
		poly_print(factors[i],tmp);

		sprintf(tmp,"P / ROOT_%02d = ",i+1);
		ffpoly_divide(&qs,&rs,&t1,factors[i]);

		
		// if(rs.pvdeg < 0) poly_print(&qs,tmp);
		// else printf("Error !\n");
		
		ffpoly_copy(&t1,&qs);
		}

	printf("-----------------------------------------\n");

EXIT_HERE:
	for(i=0;i<MAXFACTORS;i++)
		free(factors[i]);

	free(factors);
	
	return rVal;
	}
*/


int expoly_test_addition(int maxloops,int printeach)
	{
	EXPOLY p1,p2,p3,modulus;
	EXPOLY s1,s2,s3,s4,s5,s6;
	FFPOLY cmod;
	int cmptest;
	int loops=0;

	// Testing expoly addition
	printf("Testing expoly addition...\n");
	
	ffpoly_set(&cmod,poly_1,4);

	expoly_init(&p1,0);
	expoly_init(&p2,0);
	expoly_init(&p3,0);
	
	expoly_init(&s1,0);
	expoly_init(&s2,0);
	expoly_init(&s3,0);
	expoly_init(&s4,0);
	expoly_init(&s5,0);
	expoly_init(&s6,0);

	expoly_init(&modulus,0);

	//srand(time(NULL));

	expoly_zeroset(&s1);
	expoly_zeroset(&s2);
	expoly_zeroset(&s3);
	expoly_zeroset(&s4);
	expoly_zeroset(&s5);
	expoly_zeroset(&s6);

	while(loops < maxloops)
		{
		expoly_rand(&p1,67,37);
		expoly_rand(&p2,109,57);
		expoly_rand(&p3,51,71);

		expoly_set(&modulus,p_iri_1,3);

		expoly_addmod(&s1,&s1,&p1,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s1,&s1,&p2,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s1,&s1,&p3,NULL,NULL); //&modulus,&cmod);

		expoly_addmod(&s2,&s2,&p1,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s2,&s2,&p3,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s2,&s2,&p2,NULL,NULL); //&modulus,&cmod);

		expoly_addmod(&s3,&s3,&p2,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s3,&s3,&p1,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s3,&s3,&p3,NULL,NULL); //&modulus,&cmod);

		expoly_addmod(&s4,&s4,&p2,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s4,&s4,&p3,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s4,&s4,&p1,NULL,NULL); //&modulus,&cmod);

		expoly_addmod(&s5,&s5,&p3,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s5,&s5,&p1,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s5,&s5,&p2,NULL,NULL); //&modulus,&cmod);

		expoly_addmod(&s6,&s6,&p3,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s6,&s6,&p2,NULL,NULL); //&modulus,&cmod);
		expoly_addmod(&s6,&s6,&p1,NULL,NULL); //&modulus,&cmod);
		
		//expoly_print(&s1,"p1+p2+p3: ");
		//expoly_print(&s2,"p1+p3+p2: ");
		//expoly_print(&s3,"p2+p1+p3: ");
		//expoly_print(&s4,"p2+p3+p1: ");
		//expoly_print(&s5,"p3+p1+p2: ");
		//expoly_print(&s6,"p3+p2+p1: ");

		cmptest = 0;

		if(cmptest==0) cmptest = expoly_cmp(&s1,&s2);
		if(cmptest==0) cmptest = expoly_cmp(&s1,&s3);
		if(cmptest==0) cmptest = expoly_cmp(&s1,&s4);
		if(cmptest==0) cmptest = expoly_cmp(&s1,&s5);
		if(cmptest==0) cmptest = expoly_cmp(&s1,&s6);

		if(cmptest < 0) return 0;
		else if(cmptest > 0) return 0;

		if(printeach > 0 && (loops % printeach) == (printeach-1))
			printf("%d...\r",loops+1);

		loops++;
		}

	expoly_free(&modulus);

	expoly_free(&s6);
	expoly_free(&s5);
	expoly_free(&s4);
	expoly_free(&s3);
	expoly_free(&s2);
	expoly_free(&s1);

	expoly_free(&p3);
	expoly_free(&p2);
	expoly_free(&p1);
		
	return 1;
	}

#define KARATESTLOOP    500
int expoly_test_karatsubacross()
	{
	EXPOLY p1,p2,p3,modulus;
	EXPOLY s1,s2,s3,s4,s5,s6;
	FFPOLY cmod;
	WORD32 start,end;
	FFPOLY id;
	int i,j;

	expoly_init(&p1,320);
	expoly_init(&p2,320);
	expoly_init(&p3,320);
	
	expoly_init(&s1,320);
	expoly_init(&s2,320);
	expoly_init(&s3,320);
	expoly_init(&s4,320);
	expoly_init(&s5,320);
	expoly_init(&s6,320);

	ffpoly_identity_set(&id);

	expoly_init(&modulus,0);

	expoly_monic_rand(&modulus,129,102);
	ffpoly_set(&cmod,quartz_irr_ppoly,3);

	for(i=2;i<50;i+=3)
		{
		int msec1,msec2;
		int last = karatsuba_cross;

		expoly_rand(&p1,i,102);
		expoly_rand(&p2,i,102);
		expoly_rand(&p3,i,102);

		karatsuba_cross = 2;
		start = GetChronoTime();

		for(j=0;j<KARATESTLOOP;j++)
			{
			expoly_mulmod(&s1,&p3,&p2,NULL,&cmod);
			expoly_mulmod(&s1,&p1,&p3,NULL,&cmod);
			}

		end = GetChronoTime();

		msec1 = end - start;

		karatsuba_cross = 2000;
		start = GetChronoTime();

		for(j=0;j<KARATESTLOOP;j++)
			{
			expoly_mulmod(&s1,&p3,&p2,NULL,&cmod);
			expoly_mulmod(&s1,&p1,&p3,NULL,&cmod);
			}

		end = GetChronoTime();

		msec2 = end - start;

		printf("%3d: Kar=%3.2f, Plain=%3.2f\n",i,msec1/1000.0,msec2/1000.0);

		karatsuba_cross = last;
		}

	expoly_free(&modulus);

	expoly_free(&s6);
	expoly_free(&s5);
	expoly_free(&s4);
	expoly_free(&s3);
	expoly_free(&s2);
	expoly_free(&s1);

	expoly_free(&p3);
	expoly_free(&p2);
	expoly_free(&p1);

	return 1;
	}

int expoly_test_multiplication(int maxloops,int printeach)
	{
	EXPOLY p1,p2,p3,modulus;
	EXPOLY s1,s2,s3,s4,s5,s6;
	FFPOLY cmod;
	int cmptest;
	int loops=0;

	// Testing expoly multiplication...
	printf("Testing expoly multiplication...\n");
	
	ffpoly_set(&cmod,poly_1,4);

	expoly_init(&p1,0);
	expoly_init(&p2,0);
	expoly_init(&p3,0);
	
	expoly_init(&s1,0);
	expoly_init(&s2,0);
	expoly_init(&s3,0);
	expoly_init(&s4,0);
	expoly_init(&s5,0);
	expoly_init(&s6,0);

	expoly_init(&modulus,0);

	//srand(time(NULL));

	expoly_zeroset(&s1);
	expoly_zeroset(&s2);
	expoly_zeroset(&s3);
	expoly_zeroset(&s4);

	expoly_set(&modulus,p_iri_1,3);

	while(loops < maxloops)
		{
		int last = karatsuba_cross;

		expoly_rand(&p1,5,3);
		expoly_rand(&p2,5,3);
		expoly_rand(&p3,5,3);

		karatsuba_cross = 2;	// Force plain multiplication...

		expoly_mulmod(&s1,&p1,&p3,NULL/*&modulus*/,&cmod); //&modulus,&cmod);

		karatsuba_cross = 2000;	// Force Karatsuba multiplication
		expoly_mulmod(&s2,&p1,&p3,NULL/*&modulus*/,&cmod); //&modulus,&cmod);

		if(expoly_cmp(&s1,&s2) != 0)
			{
			printf("Error!\n");
			return -2;
			}

		karatsuba_cross = last;

		expoly_addmod(&s1,&p1,&p2,&modulus,&cmod); //&modulus,&cmod);
		
		expoly_mulmod(&s1,&s1,&p3,&modulus,&cmod); //&modulus,&cmod);

		expoly_mulmod(&s2,&p1,&p3,&modulus,&cmod);
		expoly_mulmod(&s3,&p2,&p3,&modulus,&cmod);

		expoly_addmod(&s2,&s2,&s3,&modulus,&cmod);

		expoly_mulmod(&s4,&p3,&p2,&modulus,&cmod);

        expoly_mulmod(&s5,&p1,&p1,&modulus,&cmod);
        expoly_square(&s6,&p1,&modulus,&cmod);

		cmptest = 0;

		if(cmptest==0) cmptest = expoly_cmp(&s1,&s2);
		if(cmptest==0) cmptest = expoly_cmp(&s3,&s4);
		if(cmptest==0) cmptest = expoly_cmp(&s5,&s6);

		if(cmptest != 0) return 0;

		if(printeach > 0 && (loops % printeach) == (printeach-1))
			printf("%d...\r",loops+1);
	
		loops++;
		}

	expoly_free(&modulus);

	expoly_free(&s6);
	expoly_free(&s5);
	expoly_free(&s4);
	expoly_free(&s3);
	expoly_free(&s2);
	expoly_free(&s1);

	expoly_free(&p3);
	expoly_free(&p2);
	expoly_free(&p1);
		
	return 1;
	}

int expoly_test_fast_division(int maxloops,int printeach)
	{
	EXPOLY p1,p2;
	EXPOLY s1,s2,s3,s4;
	FFPOLY cmod;
	const int lps = 1000;
	int loops=0;
	int flag = 1;
	WORD32 t1,t2,t3;

	// Testing expoly division
	printf("Testing expoly fast division...\n");
	
	ffpoly_set(&cmod,poly_3,3);

	expoly_init(&p1,0);
	expoly_init(&p2,0);
	
	expoly_init(&s1,0);
	expoly_init(&s2,0);
	expoly_init(&s3,0);
	expoly_init(&s4,0);

	//srand(time(NULL));

	expoly_zeroset(&s1);
	expoly_zeroset(&s2);
	expoly_zeroset(&s3);
	expoly_zeroset(&s4);

	while(loops < maxloops)
		{
		int j;

		do
			{
			expoly_rand(&p1,129,102);
			expoly_monic_rand(&p2,10,102);

			} while(p1.pvdeg <= p2.pvdeg);

		// p1 = p2 * s1 + s2

		t1 = GetChronoTime();

		for(j=0;j<lps;j++)
			expoly_divide(&s1,&s2,&p1,&p2,&cmod);

		t2 = GetChronoTime();
		printf("Time elapsed for simple div: %3.2f\n",(t2-t1)/1000.0);

		for(j=0;j<lps;j++)
			{
			if( expoly_fast_divide(&s3,&s4,&p1,&p2,&cmod) != 1 ) return 0;
			}

		t3 = GetChronoTime();

		printf("Time elapsed for fast div  : %3.2f\n",(t3-t2)/1000.0);

		if(expoly_cmp(&s1,&s3) != 0)
			{
			printf("Quotient failed on fast division (degs=(r)%d,(w)%d)!\n",
				s1.pvdeg,s3.pvdeg);

			flag = 0;
			}

		if(expoly_cmp(&s2,&s4) != 0)
			{
			printf("Remainder failed on fast division (degs=(r)%d,(w)%d)!\n",
				s2.pvdeg,s4.pvdeg);

			flag = 0;
			}

		if(flag == 0) 
			{
			printf("Loop failed after... %d\n",loops);
			return 0;
			}

		if(printeach > 0 && (loops % printeach) == (printeach-1))
			printf("%d...\r",loops+1);
	
		loops++;
		}

	expoly_free(&s4);
	expoly_free(&s3);
	expoly_free(&s2);
	expoly_free(&s1);

	expoly_free(&p2);
	expoly_free(&p1);
		
	return 1;
	}


int expoly_test_division(int maxloops,int printeach)
	{
	EXPOLY p1,p2,p3,modulus;
	EXPOLY s1,s2,s3,s4,s5,s6;
	FFPOLY cmod;
	int cmptest;
	int loops=0;

	// Testing expoly division
	printf("Testing expoly division...\n");
	
	//ffpoly_set(&cmod,poly_3,3);
	ffpoly_set(&cmod,poly_7,5);

	//poly_print(&cmod,"CModulus: ");

	expoly_init(&p1,0);
	expoly_init(&p2,0);
	expoly_init(&p3,0);
	
	expoly_init(&s1,0);
	expoly_init(&s2,0);
	expoly_init(&s3,0);
	expoly_init(&s4,0);
	expoly_init(&s5,0);
	expoly_init(&s6,0);

	expoly_init(&modulus,0);

	//srand(time(NULL));

	expoly_zeroset(&s1);
	expoly_zeroset(&s2);
	expoly_zeroset(&s3);
	expoly_zeroset(&s4);

	expoly_zeroset(&s5);
	expoly_zeroset(&s6);

	expoly_set(&modulus,p_iri_4,3);

	while(loops < maxloops)
		{
		do
			{
			expoly_rand(&p1,5,4);
			expoly_monic_rand(&p2,2,4);
			expoly_rand(&p3,3,4);

			} while(p1.pvdeg <= p2.pvdeg || p1.pvdeg <= p3.pvdeg);

		expoly_reduce(&p1,&modulus,&cmod);
		expoly_reduce(&p2,&modulus,&cmod);
		expoly_reduce(&p3,&modulus,&cmod);

		// p1 = p2 * s1 + s2
		expoly_divide(&s1,&s2,&p1,&p2,&cmod);

		// p1 = p3 * s3 + s4
		expoly_divide(&s3,&s4,&p1,&p3,&cmod);

		// s5 = p1 = p2 * s1 + s2
		expoly_mulmod(&s5,&s1,&p2,&modulus,&cmod);
		expoly_addmod(&s5,&s5,&s2,&modulus,&cmod);

		// s6 = p1 = p3 * s3 + s4
		expoly_mulmod(&s6,&s3,&p3,&modulus,&cmod);
		expoly_addmod(&s6,&s6,&s4,&modulus,&cmod);

		cmptest = 0;

		if(cmptest==0) cmptest = expoly_cmp(&p1,&s5);
		if(cmptest==0) cmptest = expoly_cmp(&p1,&s6);

		if(cmptest != 0)
			{
			printf("Loops = %d\n",loops);
			printf("\n");
			expoly_print(&p1,"P1:");
			expoly_print(&p2,"P2:");
			expoly_print(&p3,"P3:");
			
			expoly_print(&s1,"S1:");
			expoly_print(&s2,"S2:");

			expoly_print(&s3,"S3:");
			expoly_print(&s4,"S4:");

			expoly_print(&s5,"S5:");
			expoly_print(&s6,"S6:");

			expoly_print(&modulus,"Mod :");
			poly_print(&cmod,"cmod:");


			if(expoly_cmp(&s5,&s6) != 0)
				printf("Also: S5 != S6\n");
			else printf("But S5 == S6\n");

			return 0;
			}

		if(printeach > 0 && (loops % printeach) == (printeach-1))
			printf("%d...\r",loops+1);
	
		loops++;
		}

	expoly_free(&modulus);

	expoly_free(&s6);
	expoly_free(&s5);
	expoly_free(&s4);
	expoly_free(&s3);
	expoly_free(&s2);
	expoly_free(&s1);

	expoly_free(&p3);
	expoly_free(&p2);
	expoly_free(&p1);
		
	return 1;
	}

int expoly_test_gcd(int maxloops,int printeach)
	{
	EXPOLY p1,p2,p3,modulus;
	EXPOLY s1,s2,s3,s4,s5,s6;
	FFPOLY cmod;
	int cmptest;
	int loops=0;

	// Testing expoly division
	printf("Testing expoly gcd...\n");
	
	ffpoly_set(&cmod,poly_5,3);

	//poly_print(&cmod,"CModulus: ");

	expoly_init(&p1,0);
	expoly_init(&p2,0);
	expoly_init(&p3,0);
	
	expoly_init(&s1,0);
	expoly_init(&s2,0);
	expoly_init(&s3,0);
	expoly_init(&s4,0);
	expoly_init(&s5,0);
	expoly_init(&s6,0);

	expoly_init(&modulus,0);

	//srand(time(NULL));

	expoly_zeroset(&s1);
	expoly_zeroset(&s2);
	expoly_zeroset(&s3);
	expoly_zeroset(&s4);

	expoly_zeroset(&s5);
	expoly_zeroset(&s6);

	expoly_set(&modulus,poly_5,3);

	while(loops < maxloops)
		{
		expoly_rand(&p1,4,cmod.pvdeg-1);
		expoly_rand(&p2,4,cmod.pvdeg-1);

		expoly_reduce(&p1,&modulus,&cmod);
		expoly_reduce(&p2,&modulus,&cmod);

		/* p3 divides p1 and p2 */
		if( expoly_gcd(&p3,&p1,&p2,&cmod) != 1) return 0;

		/* p1 = s1 * p3 */
		expoly_divide(&s1,&s2,&p1,&p3,&cmod);

		/* p2 = s3 * p3 */
		expoly_divide(&s3,&s4,&p2,&p3,&cmod);

		/* s5 = s1 * p3 =?= p1 */
		expoly_mulmod(&s5,&s1,&p3,&modulus,&cmod);

		/* s6 = s3 * p3 =?= p1 */
		expoly_mulmod(&s6,&s3,&p3,&modulus,&cmod);
		
		cmptest = 0;

		if(cmptest==0) cmptest = expoly_cmp(&p1,&s5);
		if(cmptest==0) cmptest = expoly_cmp(&p2,&s6);

		if(cmptest != 0)
			{
			printf("Loops = %d\n",loops);
			expoly_print(&p1,"P1 :");
			expoly_print(&p2,"P2 :");
			expoly_print(&p3,"GCD:");
			printf("\n");
			expoly_print(&s1,"S1 :");
			expoly_print(&s2,"S2 :");
			printf("\n");
			expoly_print(&s3,"S3 :");
			expoly_print(&s4,"S4 :");
			printf("\n");
			expoly_print(&s5,"S5=?=P1:");
			printf("\n");
			expoly_print(&s6,"S6=?=P2:");

			if(expoly_cmp(&s5,&s6) != 0)
				printf("Also: S5 != S6\n");
			else printf("But S5 == S6\n");

			return 0;
			}

		expoly_rand(&p1,4,cmod.pvdeg-1);
		expoly_rand(&p2,4,cmod.pvdeg-1);
		expoly_rand(&p3,4,cmod.pvdeg-1);

		expoly_reduce(&p1,&modulus,&cmod);
		expoly_reduce(&p2,&modulus,&cmod);
		expoly_reduce(&p3,&modulus,&cmod);

        expoly_mulmod(&s1,&p1,&p2,&modulus,&cmod);
        expoly_mulmod(&s2,&p1,&p3,&modulus,&cmod);

        //expoly_print(&p1,"P1");
        //expoly_print(&p2,"P2");
        //expoly_print(&p3,"P3");
        
        if( expoly_gcd(&s3,&s1,&s2,&cmod) != 1 ) return 0;
    
        //expoly_print(&s1,"S1");
        //expoly_print(&s2,"S2");
        //expoly_print(&s3,"S3");
        
        if( expoly_divide(&s4,&s5,&s1,&s3,&cmod) != 1 ) return 0;
        if( expoly_is_zero(&s5) == 0 ) return 0;

        if( expoly_divide(&s4,&s5,&s2,&s3,&cmod) != 1 ) return 0;
        if( expoly_is_zero(&s5) == 0 ) return 0;

        if(s3.pvdeg < p1.pvdeg) return 0;
        
		if(printeach > 0 && (loops % printeach) == (printeach-1))
			printf("%d...\r",loops+1);
	
		loops++;
		}

	expoly_free(&modulus);

	expoly_free(&s6);
	expoly_free(&s5);
	expoly_free(&s4);
	expoly_free(&s3);
	expoly_free(&s2);
	expoly_free(&s1);

	expoly_free(&p3);
	expoly_free(&p2);
	expoly_free(&p1);
		
	return 1;
	}

int bruteforce_sols(EXPOLY *p,FFPOLY *cmod)
    {
    FFPOLY rs,bf;
    int i,mx;
    int sol=0;
    int bc=1;

    mx = (1 << cmod->pvdeg);

    ffpoly_zeroset(&bf);

    bf.pvdeg = 0;

    for(i=0;i<mx;i++)
        {
        bf.pv[0] = i;
        
        if(i & bc)
            {
            bf.pvdeg++;
            bc<<=1;
            }       

        expoly_subst(&rs,p,&bf,cmod);
        if(ffpoly_is_zero(&rs) != 0) 
            {
#ifdef PRINT_DEBUG
            char tmp[40];
            sprintf(tmp,"BF_%02d =",sol+1);
            poly_print(&bf,tmp);
#endif
            sol++;
            }
        }

    return sol;
    }

int knownroottest()
	{
	// S=(t^5+t^3+t^2+t+1)*x^3+(t^5)*x^2+(t^3+t^2)*x+(t^3+t^2+1)
	static int mypoly[3] = { 6,1,0 };
	static int beta3[4] = { 5,4,3,2 };
	static int beta2[5] = { 5,4,2,1,0 };
	static int beta1[3] = { 4,3,1 };
	static int beta0[2] = { 3,1 };

	static int theta2[3] = { 4,3,2 };
	static int theta1[3] = { 5,2,1 };
	static int theta0[2] = { 3,2 };

	static int sigma3[5] = { 5,3,2,1,0 };
	static int sigma2[1] = { 5 };
	static int sigma1[2] = { 3,2 };
	static int sigma0[3] = { 3,2,0 };

	ROOT_POT pot;
	EXPOLY p1,p2,p3;
	EXPOLY s1,s2;
	FFPOLY cmod,fp;
	int i;

	printf("Testing known root of known poly-equation...\n");

	ffpoly_set(&cmod,mypoly,3);

	expoly_init(&p1,0);
	expoly_init(&p2,0);
	expoly_init(&p3,0);
	expoly_init(&s1,0);
	expoly_init(&s2,0);
	
	expoly_zeroset(&p1);
	expoly_zeroset(&p2);
	expoly_zeroset(&p3);

	ffpoly_identity_set(&p1.ep[4]);
	ffpoly_set(&p1.ep[3],beta3,4);
	ffpoly_set(&p1.ep[2],beta2,5);
	ffpoly_set(&p1.ep[1],beta1,3);
	ffpoly_set(&p1.ep[0],beta0,2);

	p1.pvdeg = 4;

	ffpoly_identity_set(&p2.ep[3]);
	ffpoly_set(&p2.ep[2],theta2,3);
	ffpoly_set(&p2.ep[1],theta1,3);
	ffpoly_set(&p2.ep[0],theta0,2);

	p2.pvdeg = 3;
	
	ffpoly_set(&p3.ep[3],sigma3,5);
	ffpoly_set(&p3.ep[2],sigma2,1);
	ffpoly_set(&p3.ep[1],sigma1,2);
	ffpoly_set(&p3.ep[0],sigma0,3);

	p3.pvdeg = 3;

#ifdef PRINT_DEBUG
	expoly_print(&p1,"P1 :");
	expoly_print(&p2,"P2 :");
	expoly_print(&p3,"P3 :");
	poly_print(&cmod,"MOD:");
#endif

	//expoly_mulmod(&s1,&

	if( expoly_pot_init(&pot,&cmod,p1.pvdeg) != 1) return 0;

	if( expoly_factor(&pot,&p1,&cmod) != 1) return 0;

	if(pot.factsNo > 0) 
		{
		int flag=1;

		for(i=0;i<pot.factsNo;i++)
			{
#ifdef PRINT_DEBUG
			char tmp[40];
#endif		
			expoly_subst(&fp,&p1,&pot.factors[i],&cmod);

#ifdef PRINT_DEBUG
			sprintf(tmp,"SOL_%02d=",i+1);
			poly_print(&fp,tmp);

			sprintf(tmp,"P1(SOL_%02d)=",i+1);
			poly_print(&pot.factors[i],tmp);
#endif

			if(ffpoly_is_zero(&fp) == 0) 
				{
				//sprintf(tmp,"P1(SOL_%02d)=",i+1);
				//poly_print(&fp,tmp);
				flag = 0;

				return 0;
				}
			}
		}

	if(cmod.pvdeg < 16)
		{
        int sol = bruteforce_sols(&p1,&cmod);
        if(sol != pot.factsNo) 
			{
            printf("Roots found by bruteforcing: %d\n",sol);
            return 0;
            }
        }

	expoly_pot_free(&pot);

	expoly_free(&s2);
	expoly_free(&s1);
	expoly_free(&p3);
	expoly_free(&p2);
	expoly_free(&p1);
	return 1;
	}

int root_test(int maxloops,int printeach)
	{
	//static int pmod[3] = { 9,4,0 };
	static int pmod[3] = { 10,3,0 };
	ROOT_POT pot;
	EXPOLY p1,p2,p3;
	FFPOLY cmod;
	FFPOLY fp;
	int flag=1;
	int loops=0;
	int sumsol=0;
	int i;

	printf("Testing root finding...\n");
	//ffpoly_set(&cmod,poly_3,3);
	ffpoly_set(&cmod,pmod,3);

	expoly_init(&p1,0);
	expoly_init(&p2,0);
	expoly_init(&p3,0);

	while( loops < maxloops )
		{
		if( GetIrreduciblePoly(&cmod,12,loops%100) != 1) return 0;

		expoly_monic_rand(&p1,cmod.pvdeg-1,cmod.pvdeg);
		if(p1.pvdeg > 0 && ffpoly_is_zero(&p1.ep[0]) != 0)
			ffpoly_identity_set(&p1.ep[0]);

		if( expoly_pot_init(&pot,&cmod,p1.pvdeg) != 1)
            { 
            printf("expoly initialization failed !\n");
            return 0;
            }

#ifdef PRINT_DEBUG
        printf(" [%d] ---------------------------------------\n",loops);
		expoly_print(&p1,"P1 =");
		poly_print(&cmod,"Mod=");
#endif

		if( expoly_factor(&pot,&p1,&cmod) != 1)
            {
            printf("Factorization failed\n");
            expoly_print(&p1,"P1 =");
            poly_print(&cmod,"Mod=");
            printf("Wrong!\n\n");
            return 0;
            }
#ifdef PRINT_DEBUG	
		if(pot.factsNo == 0) printf("No solutions found\n");
#endif
        flag = 1;

		if(pot.factsNo > 0) 
			{
#ifdef PRINT_DEBUG
			char tmp[500];
#endif
			for(i=0;i<pot.factsNo;i++)
				{
#ifdef PRINT_DEBUG
				sprintf(tmp,"SOL_%02d=",i+1);
				poly_print(&pot.factors[i],tmp);
#endif				
				expoly_subst(&fp,&p1,&pot.factors[i],&cmod);

#ifdef PRINT_DEBUG				
				sprintf(tmp,"P1(SOL_%02d)=",i+1);
				poly_print(&fp,tmp);
#endif
				if(ffpoly_is_zero(&fp) == 0) 
					{
				    char tmp[40];
		    
				    sprintf(tmp,"SOL_%02d    =",i+1);
				    poly_print(&pot.factors[i],tmp);

					sprintf(tmp,"P1(SOL_%02d)=",i+1);
					poly_print(&fp,tmp);
					flag = 0;

                    return 0;
					}
				}
			}

	    /*
        if(cmod.pvdeg < 16)
            {
            int sol = bruteforce_sols(&p1,&cmod);
            if(sol != pot.factsNo) 
                {
                printf("Roots found by bruteforcing: %d\n",sol);
                return 0;
                }
            }
		*/
		sumsol += pot.factsNo;

		expoly_pot_free(&pot);

		if(printeach > 0 && (loops % printeach) == (printeach-1))
			printf("%d...\r",loops+1);

		loops++;
		}

	expoly_free(&p3);
	expoly_free(&p2);
	expoly_free(&p1);

#ifdef PRINT_DEBUG				
	printf("Average solutions per cycle = %3.2f\n",((double)sumsol) / loops);
#endif

	return 1;
	}

#define MINROOTDEGTEST 128
#define MAXROOTDEGTEST 513

#define ROOTDEGTESTSTEP  64

#define MAXDEGPOLYTEST   3
#define MINDEGPOLYTEST   2

int root_specific_test(int n,int d,int *cnt)
	{
	ROOT_POT pot;
	EXPOLY poly;
	FFPOLY cmod;
	FFPOLY tmp;
	int sols;
	int i,j;
	int nRes;

	ffpoly_set(&cmod,quartz_irr_ppoly,3);

	nRes = expoly_init(&poly,2 * n+1);

	if(nRes != 1) return -99;
	expoly_zeroset(&poly);

	*cnt = 0;
	for(i=0;i<=n;i++)
		{
		if(count_bits(i) > d) ffpoly_zeroset(&tmp);
		else 
			{
			build_randpoly(&gRC4,&tmp,cmod.pvdeg,0);
			(*cnt)++;
			}

		expoly_setterm(&poly,&tmp,i);
		}

	ffpoly_identity_set(&tmp);

	expoly_setterm(&poly,&tmp,poly.pvdeg);	// Force poly to be monic !
	expoly_pot_init(&pot,&cmod,poly.pvdeg);
	
	nRes = expoly_factor(&pot,&poly,&cmod);
	if( nRes != 1 ) return -50;

	sols = pot.factsNo;
    for(j=0;j<sols;j++)
		{
        FFPOLY rs;

        if( expoly_subst(&rs,&poly,&pot.factors[j],&cmod) != 1 ) return -2;
        if( ffpoly_is_zero(&rs) !=1 ) return -3;
        }

	expoly_free(&poly);
	expoly_pot_free(&pot);

	return sols;
	}

int extrootfind_text(int rootsno, FFPOLY *cmod)
	{
	ROOT_POT pot;
	EXPOLY p1,p2,p3;
	FFPOLY tmp;
	FFPOLY *lrpoly;
	int sols;
	int i,j;
	int nRes;

	//printf("Building %d degree expoly for testing factorization\n",rootsno);

	//ffpoly_set(cmod,quartz_irr_ppoly,3);

	nRes = expoly_init(&p1,2 * rootsno+1);
	if(nRes != 1) return -99;

	nRes = expoly_init(&p2,2 * rootsno+1);
	if(nRes != 1) return -98;

	nRes = expoly_init(&p3,2 * rootsno+1);
	if(nRes != 1) return -97;

	expoly_zeroset(&p1);
	expoly_zeroset(&p2);
	expoly_zeroset(&p3);

	lrpoly = (FFPOLY *)malloc( rootsno * sizeof(FFPOLY) );
	if(!lrpoly) return -46;

	ffpoly_identity_set(&tmp);

	expoly_setterm(&p3,&tmp,1);
	expoly_setterm(&p2,&tmp,0);

	for(i=0;i<rootsno;i++) 
		{
		ffpoly_zeroset(&lrpoly[i]);
		
		build_randpoly(&gRC4,&lrpoly[i],cmod->pvdeg,0);
		expoly_setterm(&p3,&lrpoly[i],0);	// set Z - a

		nRes = expoly_mulmod(&p1,&p2,&p3,NULL,cmod);
		if(nRes != 1) return -95;

		expoly_copy(&p2,&p1);
		}

	//printf("Testing roots of built polynomial\n");
    for(j=0;j<rootsno;j++)
		{
		FFPOLY rs;

        if( expoly_subst(&rs,&p2,&lrpoly[j],cmod) != 1 ) return -2;
        if( ffpoly_is_zero(&rs) !=1 ) 
			{
			printf("?test failed, not %d item is not a root\n",j);
			return -3;
			}
        }

	//printf("Factoring %d degree polynomial...\n",p2.pvdeg);

	expoly_pot_init(&pot,cmod,p2.pvdeg);
	
	nRes = expoly_factor(&pot,&p2,cmod);
	if(nRes != 1) return -49;

	sols = pot.factsNo;
    for(j=0;j<sols;j++)
		{
        FFPOLY rs;

        if( expoly_subst(&rs,&p2,&pot.factors[j],cmod) != 1 ) return -2;
        if( ffpoly_is_zero(&rs) !=1 ) return -3;
        }

	if(sols != rootsno) 
		{
		printf("number of solutions found: %d, not matching expected (%d)\n",sols,rootsno);

		printf("deposit size: %d, pickup size: %d, polysize: %d\n",
			pot.deposit_size,pot.pickup_size,pot.size);

		for(i=0;i<pot.deposit_size;i++) {
			EXPOLY *e = &pot.deposit[i];
			printf("D%d) %d\n",i+1,e->pvdeg);

			printf("unbreakable factor\n");
			expoly_dumpdata(e);
		}
	
		printf("offending poly\n");
		expoly_dumpdata(&p2);
		return -1;
		}

	free(lrpoly);

	expoly_free(&p1);
	expoly_free(&p2);
	expoly_free(&p3);
	
	expoly_pot_free(&pot);

	return sols;
	}

#define LPOLYSIZE 129

int fast_squaremod(void)
	{
	static int s1[5] = { 102,41,29,28,0};
	static int s2[7] = { 78,59,57,48,3,2,0};
	EXPOLYMOD fm;
	EXPOLY f;
	EXPOLY sq1,sq2,sqt;
	FFPOLY cmod;
	FFPOLY base,base2;
	FFPOLY x;
	int i;

	ffpoly_set(&base,s1,5);
	ffpoly_set(&base2,s2,7);

	ffpoly_set(&cmod,quartz_irr_ppoly,3);

	expoly_init(&f,LPOLYSIZE + 1);
	expoly_init(&sq1,2 * LPOLYSIZE + 1);
	expoly_init(&sq2,2 * LPOLYSIZE + 1);
	expoly_init(&sqt,2 * LPOLYSIZE + 1);
	
	expoly_zeroset(&f);

	ffpoly_identity_set(&x);

	for(i=0;i<LPOLYSIZE;i++)
		{
		ffpoly_mulmod(&base,&base,&base2,&cmod);
		expoly_setterm(&f,&base,i);
		}

	expoly_setterm(&f,&x,i);

	expolymod_init_modulus(&fm,&f,&cmod);

	expoly_zeroset(&sqt);
	expoly_setterm(&sqt,&x,1);

	for(i=0;i<cmod.pvdeg;i++)
		{
		expoly_square(&sq1,&sqt,&f,&cmod);
		expolymod_squaremod(&sq2,&sqt,&fm);

		if(expoly_cmp(&sq1,&sq2) != 0)
			{
			int k;

			printf("Error squaring x^(2^%d)\n",i);

			for(k=0;k<MAX(sq1.pvdeg,sq2.pvdeg);k++)
				{
				printf("%3d: %08x%08x%08x%08x - %08x%08x%08x%08x\n",k,
					sq1.ep[k].pv[0],sq1.ep[k].pv[1],sq1.ep[k].pv[2],sq1.ep[k].pv[3],
					sq2.ep[k].pv[0],sq2.ep[k].pv[1],sq2.ep[k].pv[2],sq2.ep[k].pv[3]);
				}

			return 0;
			}

		expoly_copy(&sqt,&sq1);
		}

	expolymod_free_modulus(&fm);

	expoly_free(&sq1);
	expoly_free(&sq2);
	expoly_free(&sqt);

	printf("finished...\n");

	return 1;
	}

int test_factorization(int loops, int maxdeg)
	{
	int i;

	for(i=0;i<loops;i++) 
		{
		FFPOLY cmod;
		int roots_no;

		ffpoly_set(&cmod,p_bigirred,5);

		roots_no = 2 + (RC4_GetLong(&gRC4) % (maxdeg-2));

		printf("Testing factorization of a [%03d] degree expoly\n",roots_no);
		fflush(stdout);
			
		if( extrootfind_text( roots_no, &cmod ) < 1 ) return -301;
		}
	printf("-----------------------------------------\n");
	return 1;
	}

int root_extended_test(void)
	{
	int nn;
	int deg;
	int cnt;
	double base=0.0,current;

	printf("Extended poly root test...\n");

	for(deg=MINDEGPOLYTEST;deg<=MAXDEGPOLYTEST;deg++)
		{
		for(nn=MINROOTDEGTEST;nn<MAXROOTDEGTEST;nn*=2)
			{
			int n = nn + 1;
			int sols;

			do
				{
				WORD32 start,end;

				start = GetChronoTime();
				
				sols = root_specific_test(n,deg,&cnt);
				if(sols < 0) 
					{
					printf("Test failed! (%d)\n",sols);
					return sols;
					}

				end = GetChronoTime();

				current = (end-start)/1000.0;

				if(nn==MINROOTDEGTEST) 
					{
					base = current;
					if(base <= 0.0) base = 0.000001;
					}

				if(sols == 1) 
					printf("Sol. F(Z)=0, d=%4d -> %03.2f secs. ratio=%03.2f (sd=%d, terms=%d)\n",
							n,current,current/base,deg,cnt);

				} while(sols != 1);

			}

		printf("-----------------------------------------\n");
		
		}

	return 1;
	}

int mulstage()
	{
	static int s1[5] = { 102,41,29,28,0};
	static int s2[7] = { 78,59,57,48,3,2,0};

	WORD32 start,end;
	FFPOLY p1,p2,p3,p4,p5,p6,p7;
	int i;

	ffpoly_set(&p2,s1,5);
	ffpoly_set(&p3,s2,7);

	start = GetChronoTime();

    poly_rand(&p2,103);
    poly_rand(&p3,103);
    poly_rand(&p4,103);
    poly_rand(&p5,103);
    poly_rand(&p6,103);
    poly_rand(&p7,103);

	for(i=0;i<500000;i++)
        {
		ffpoly_mulmod(&p1,&p2,&p3,NULL);
		ffpoly_mulmod(&p1,&p4,&p5,NULL);
		ffpoly_mulmod(&p1,&p6,&p7,NULL);

		ffpoly_mulmod(&p1,&p4,&p3,NULL);
		ffpoly_mulmod(&p1,&p6,&p5,NULL);
		ffpoly_mulmod(&p1,&p2,&p7,NULL);

		ffpoly_mulmod(&p1,&p6,&p3,NULL);
		ffpoly_mulmod(&p1,&p2,&p5,NULL);
		ffpoly_mulmod(&p1,&p4,&p7,NULL);
        }

	end = GetChronoTime();

	printf("Elapsed: %3.2f...\n",(end-start)/1000.0);

	return 1;
	}

/*
4a1ce908 d1b2d468 6352ef33 0000007e 00000000
6e7cfeb6 68190f40 0e880bf9 00000000
a34b5ae9 44670bf7 71628a82 00000000

4a1ce908 d1b2d468 6352ef33 0000007e 00000000
6e7cfeb6 68190f40 0e880bf9 00000000
a34b5ae9 44670bf7 71628a82 0000000b 00000000

8fc4b3b0 cd7da2fe 5eb5eac7 1cadac8b 4ee1daaf ca909059 00000002 00000000
*/

int poly_test_single_reduction_103(int n,int d)
	{
	FFPOLY p1,p2,p3;
	FFPOLY cmod;
	int i;

	// Testing expoly multiplication...
	printf("Testing poly reduction mod X^103+X^9+1...\n");

	ffpoly_set(&cmod,quartz_irr_ppoly,3);

	for(i=0;i<n;i++)
		{
		ffpoly_zeroset(&p1);
		
		poly_rand(&p1,5 + (RC4_GetByte(&gRC4)%201));
		
		ffpoly_copy(&p2,&p1);
		ffpoly_copy(&p3,&p1);

		if(p2.pvdeg >= 103)
			{
			fastest_reduction_mod_103u(&p2);
			fastest_reduction_mod_103(&p3);
			//ffpoly_reduce(&p3,&cmod);
			}

		if(ffpoly_cmp(&p2,&p3) != 0)
			{
			printf("%d\n",i);

			ffpoly_dump(&p1);
			ffpoly_dump(&p2);
			ffpoly_dump(&p3);

			printf("Error\n");
			return 0;
			}

		if(d > 0 && (i % d) == (d-1))
			printf("%d...\r",i+1);
		}


	//ffpoly_mulmod(&p3,&p1,&p2,NULL);


	return 1;
	}

#ifdef USE_MMX_ASSEMBLY

int poly_test_reduction_compare(int n)	
	{
	FFPOLY p1,p2,p3;
	WORD32 t1,t2,t3;
	double f1,f2;
	int i;

	/* Testing coherence */

	printf("Testing MMX poly reduction coherence\n");

	for(i=0;i<100;i++)
		{
		poly_rand(&p1,205);
		ffpoly_copy(&p2,&p1);
		ffpoly_copy(&p3,&p1);

		fastest_reduction_mod_103u(&p2);
		fastest_reduction_mod_103u_asm(&p3);

		if(ffpoly_cmp(&p2,&p3) != 0) 
			{
			empty_mmx_state();
			printf("Coherence test failed...\n");
			
			ffpoly_dump(&p1);
			ffpoly_dump(&p2);
			ffpoly_dump(&p3);
			return -1;
			}
		}

	empty_mmx_state();

	printf("Testing MMX poly reduction speed\n");
	poly_rand(&p1,205);

	t1 = GetChronoTime();
	for(i=0;i<n;i++)
		{
		ffpoly_copy(&p2,&p1);
		fastest_reduction_mod_103u_asm(&p2);
		}

	empty_mmx_state();

	t2 = GetChronoTime();

	f1 = (t2-t1)/1000.0;
	printf("MMX Assmbly = %3.2f\n",f1);

	for(i=0;i<n;i++)
		{
		ffpoly_copy(&p3,&p1);
		fastest_reduction_mod_103u(&p3);
		}

	t3 = GetChronoTime();
	f2 = (t3-t2)/1000.0;

	printf("C Language  = %3.2f\n",f2);

	printf("Performance gain = %3.2f%%\n",(f2/f1 - 1.0) * 100.0);
	return 1;
	}

#endif

int poly_test_irreducibility(int n,int printeach)
	{
	FFPOLY p1;
	int maxsearch = n;
	int found = 0;
	int i;

	printf("Testing poly irreducibility...\n");

	for(i=0;i<125;i++)
		{
		WORD16 pq = irredpolys[i];

		ffpoly_zeroset(&p1);

		if     (pq <   0x10) p1.pv[0] = pq;
		else if(pq <  0x100) p1.pv[0] = (pq >> 4) | ((pq << 4) & 0xf0);
		else if(pq < 0x1000) p1.pv[0] = ((pq >> 8) & 0x0f) | (pq & 0xf0) | ((pq & 0x0f) << 8);
		else p1.pv[0] = ((pq >> 12) & 0x000f) | ((pq >>  4) & 0x00f0) | 
			            ((pq <<  4) & 0xf000) | ((pq << 12) & 0xf000);

		ffpoly_setterm(&p1,3+i);

		if(ffpoly_test_irreducibility(&p1) != 1)
			{
			printf("Irreducibility poly test failed. (deg=%d)\n",3+i);
			ffpoly_dump(&p1);
			return -1;
			}
		}

	for(i=0;i<maxsearch;i++)
		{
		int deg = 64 + (RC4_GetByte(&gRC4)%64);
		
		if(i>0) poly_rand(&p1,deg);
		else ffpoly_set(&p1,quartz_irr_ppoly,3);

		if(ffpoly_test_irreducibility(&p1) == 1) found++;

		if(printeach > 0 && (i % printeach) == (printeach-1))
			printf("%d/%d...\r",found,i+1);
		}

	printf("Random irreducible poly prob. %1.3f\n",(found / (double)maxsearch));
	return 1;
	}

/* ******************************************************************************* */

#define TEST_MAX_LOOPS 50000
#define PRINT_EACH      1000

int poly_test(const char *pass)
	{
	//static const char *password = "TheQuickBrownFoxJumpsOverTheLazyDog";
	static const char *password = "TheQuickBrownFgfdfgoxJumpsOverTheLazyDog";
	WORD32 start,end;
	int len;
	int i;

    if(pass == NULL)
        {
	    len = (int)strlen(password) + 1;
	    InitRC4(&gRC4,password,(int)strlen(password)+1);

        printf("Seed=%s\n",password);
        }
    else
        {
	    len = (int)strlen(pass) + 1;
	    InitRC4(&gRC4,pass,(int)strlen(pass)+1);

        printf("Seed=%s\n",pass);
        }

	start = GetChronoTime();
	
	for(i=1;i<=100000;i++)
		{
		printf("---------- Loop Test #%d ----------\n",i);
		//if( test_factorization( 10000, 50 ) < 1 ) return -301;
//#if 0
		//if( expoly_test_multiplication(TEST_MAX_LOOPS,PRINT_EACH) != 1) return -10;
		//if( mulstage() != 1 ) return -1;;
		//if( expoly_test_karatsubacross() != 1 ) return -1;

		//if( poly_test_multiplication(TEST_MAX_LOOPS,PRINT_EACH) != 1) return -3;
	
#ifdef USE_MMX_ASSEMBLY
		if( poly_test_reduction_compare(TEST_MAX_LOOPS * 1000) != 1 ) return -1;
#endif

		//if( fast_squaremod() != 1 ) return -1;
		if( fv_key_test(&gRC4) != 1) return -2;
		
    	//if( fv_test(&gRC4) != 1 ) return -1;

		if( poly_test_single_reduction_103(10*TEST_MAX_LOOPS,10*PRINT_EACH) != 1 ) return -1;
		if( poly_test_addition(TEST_MAX_LOOPS,PRINT_EACH)       != 1) return -2;
		if( poly_test_multiplication(TEST_MAX_LOOPS,PRINT_EACH) != 1) return -3;
		if( poly_test_shifts(TEST_MAX_LOOPS,PRINT_EACH)         != 1) return -4;
		if( poly_test_longdivision(TEST_MAX_LOOPS,PRINT_EACH)   != 1) return -5;
		if( poly_test_gcd(TEST_MAX_LOOPS,PRINT_EACH)            != 1) return -6;
		
		if( poly_test_fielddivision(TEST_MAX_LOOPS,PRINT_EACH)  != 1) return -7;
  		
		if( poly_test_substitution()                            != 1) return -8;

		if( poly_test_irreducibility(TEST_MAX_LOOPS,PRINT_EACH) != 1 ) return -9;
		
		if( expoly_test_addition(TEST_MAX_LOOPS,PRINT_EACH) != 1) return -101;
		if( expoly_test_multiplication(TEST_MAX_LOOPS,PRINT_EACH) != 1) return -102;
//#endif
		if( expoly_test_division(TEST_MAX_LOOPS/10,PRINT_EACH/10) != 1) return -103;
		//if( expoly_test_fast_division(TEST_MAX_LOOPS/10,PRINT_EACH/10) != 1) return -11;
		if( expoly_test_gcd(TEST_MAX_LOOPS/10,PRINT_EACH/10) != 1) return -104;

		//if( root_test(TEST_MAX_LOOPS/10,PRINT_EACH/10) != 1) return -12;

		if( root_extended_test() < 1 ) return -201;
		if( test_factorization( 10, 129 ) < 1 ) return -301;
		//if( knownroottest() != 1) return -14;
		
		end = GetChronoTime();

		printf("Average time: %3.2f secs.\n",(end-start)/(1000.0 * i));
		}
	
	printf("Finished... !\n");

	return 1;
	}



