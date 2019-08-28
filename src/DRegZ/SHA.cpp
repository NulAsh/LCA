
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// SHA.cpp: implementation of the SHS class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <memory.h>

#include "SHA.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// SHS

#define LITTLE_ENDIAN

/* The SHS f()-functions */

#define f1(x,y,z) ((x&y)|(~x&z))   						/* Rounds  0-19 */
#define f2(x,y,z) (x^y^z)            					/* Rounds 20-39 */
#define f3(x,y,z) ((x&y)|(x&z)|(y&z))				    /* Rounds 40-59 */
#define f4(x,y,z) (x^y^z)		            			/* Rounds 60-79 */

/* The SHS Mystrious constants */

#define K1 0x5A827999L  /* Rounds  0-19 */
#define K2 0x6ED9EBA1L  /* Rounds 20-39 */
#define K3 0x8F1BBCDCL  /* Rounds 40-59 */
#define K4 0xCA62C1D6L  /* Rounds 60-79 */

/* SHS initial values */

#define h0init 0x67452301L
#define h1init 0xEFCDAB89L
#define h2init 0x98BADCFEL
#define h3init 0x10325476L
#define h4init 0xC3D2E1F0L

/* 32-bit rotate - kludged with shifts */

#define S(n,X) ( (X<<n) | (X>>(32-n)) )

/* The initial expanding function */

#define expand(count) W[count] = W[count-3]^W[count-8]^W[count-14]^W[count-16]; 

/* The four SHS sub-rounds */

#define subRound1(count) \
	{\
	temp=S(5,A)+f1(B,C,D)+E+W[count]+K1;\
	E=D;\
	D=C;\
	C=S(30,B);\
	B=A;\
	A=temp;\
	}

#define subRound2(count) \
	{\
	temp=S(5,A)+f2(B,C,D)+E+W[count]+K2;\
	E=D;\
	D=C;\
	C=S(30,B);\
	B=A;\
	A=temp;\
	}

#define subRound3(count) \
	{\
	temp=S(5,A)+f3(B,C,D)+E+W[count]+K3;\
	E=D;\
	D=C;\
	C=S(30,B);\
	B=A;\
	A=temp;\
	}

#define subRound4(count) \
	{\
	temp=S(5,A)+f4(B,C,D)+E+W[count]+K4;\
	E=D;\
	D=C;\
	C=S(30,B);\
	B=A;\
	A=temp;\
	}

/* The two buffer of 5 32-bit words */

//static unsigned int h0,h1,h2,h3,h4;
static unsigned int A,B,C,D,E;

/* Initialize the SHS values */


SHS::SHS()
	{
	store = new unsigned char[SHS_BLOCKSIZE];
	Init();
	}

SHS::~SHS()
	{
	delete[] store;
	store = NULL;
	}


void SHS::Init()
	{
	/* Set the h-vars to their initial values */
	digest[0]=h0init;
	digest[1]=h1init;
	digest[2]=h2init;
	digest[3]=h3init;
	digest[4]=h4init;

	/* Initialize bit count */
	countLo=0L;
	countHi=0L;

	memset(store,0x00,SHS_BLOCKSIZE);
	stored = 0;
	}

/* Perform the SHS transformation. Note that this code, like MD5, seems to
	break some optimizing compilers - it may be necessary to split it into
	sections, eg based on the four subrounds */

void SHS::Transform()
	{
	unsigned int W[80],temp;
	short i;

	/* Step A. Copy the data buffer into the local work buffer */
	for(i=0;i<16;i++)
		W[i]=data[i];

	/* Step B. Expand the 16 words into 64 temporary data words */
    expand( 16 ); expand( 17 ); expand( 18 ); expand( 19 ); expand( 20 );
    expand( 21 ); expand( 22 ); expand( 23 ); expand( 24 ); expand( 25 );
    expand( 26 ); expand( 27 ); expand( 28 ); expand( 29 ); expand( 30 );
    expand( 31 ); expand( 32 ); expand( 33 ); expand( 34 ); expand( 35 );
    expand( 36 ); expand( 37 ); expand( 38 ); expand( 39 ); expand( 40 );
    expand( 41 ); expand( 42 ); expand( 43 ); expand( 44 ); expand( 45 );
    expand( 46 ); expand( 47 ); expand( 48 ); expand( 49 ); expand( 50 );
    expand( 51 ); expand( 52 ); expand( 53 ); expand( 54 ); expand( 55 );
    expand( 56 ); expand( 57 ); expand( 58 ); expand( 59 ); expand( 60 );
    expand( 61 ); expand( 62 ); expand( 63 ); expand( 64 ); expand( 65 );
    expand( 66 ); expand( 67 ); expand( 68 ); expand( 69 ); expand( 70 );
    expand( 71 ); expand( 72 ); expand( 73 ); expand( 74 ); expand( 75 );
    expand( 76 ); expand( 77 ); expand( 78 ); expand( 79 );

	/* Step C. Setup first buffer */
	A=digest[0];
	B=digest[1];
	C=digest[2];
	D=digest[3];
	E=digest[4];

	/* Step D. Serious mangling, divided into four sub-rounds */

	subRound1( 0);	subRound1( 1);	subRound1( 2);	subRound1( 3);
	subRound1( 4);	subRound1( 5);	subRound1( 6);	subRound1( 7);
	subRound1( 8);	subRound1( 9);	subRound1(10);	subRound1(11);
	subRound1(12);	subRound1(13);	subRound1(14);	subRound1(15);
	subRound1(16);	subRound1(17);	subRound1(18);	subRound1(19);

	subRound2(20);	subRound2(21);	subRound2(22);	subRound2(23);
	subRound2(24);	subRound2(25);	subRound2(26);	subRound2(27);
	subRound2(28);	subRound2(29);	subRound2(30);	subRound2(31);
	subRound2(32);	subRound2(33);	subRound2(34);	subRound2(35);
	subRound2(36);	subRound2(37);	subRound2(38);	subRound2(39);

	subRound3(40);	subRound3(41);	subRound3(42);	subRound3(43);
	subRound3(44);	subRound3(45);	subRound3(46);	subRound3(47);
	subRound3(48);	subRound3(49);	subRound3(50);	subRound3(51);
	subRound3(52);	subRound3(53);	subRound3(54);	subRound3(55);
	subRound3(56);	subRound3(57);	subRound3(58);	subRound3(59);

	subRound4(60);	subRound4(61);	subRound4(62);	subRound4(63);
	subRound4(64);	subRound4(65);	subRound4(66);	subRound4(67);
	subRound4(68);	subRound4(69);	subRound4(70);	subRound4(71);
	subRound4(72);	subRound4(73);	subRound4(74);	subRound4(75);
	subRound4(76);	subRound4(77);	subRound4(78);	subRound4(79);

	/* Step E. Build SHS message digest */
	digest[0] += A;
	digest[1] += B;
	digest[2] += C;
	digest[3] += D;
	digest[4] += E;
	}

#ifdef LITTLE_ENDIAN

/* When run on a little-endian CPU we need to perform byte reversal on an
	array of longwords. It is possible to make the code endianness -
	independent by fiddling around with data at the byte level, but this
	makes for very slow code, so we rely on the user to sort out endianness
	at compile time */

static void byteReverse(unsigned int *buffer,int byteCount)
	{
	unsigned long value;
	short count;

	byteCount/=sizeof(unsigned int);
	for(count=0;count<byteCount;count++)
		{
		value=(buffer[count]<<16) | (buffer[count]>>16);
		buffer[count]=((value&0xFF00FF00L)>>8)|((value&0x00FF00FFL)<<8);
		}
	}

#endif /* LITTLE ENDIAN */

void SHS::Update(const unsigned char *buffer,int count)
	{
	while(count > 0)
		{
		if(stored + count >= SHS_BLOCKSIZE)
			{
			int stLen = SHS_BLOCKSIZE - stored;
			memcpy(store + stored,buffer,stLen);

			TrueUpdate(store,SHS_BLOCKSIZE);

			count  -= stLen;
			buffer += stLen;
			stored = 0;
			}
		else
			{
			memcpy(store + stored,buffer,count);
			stored += count;
			count = 0;
			}
		}
	}


/* Update SHS for a block of data. This code assumes that the buffer size
	is a multiple of SHS_BLOCKSIZE bytes long, which makes the code a lot
	more efficient since it does away with the need to handle partial blocks
	between calls to shsUpdate() */

void SHS::TrueUpdate(const unsigned char *buffer,int count)
	{
	// if(count != SHS_BLOCKSIZE) exit(0);

	/* Update bitcount */
	if ((countLo + (((unsigned long)count) << 3)) < countLo)
		 countHi++;  /* Carry from low to high bit count */

	countLo += ( ((unsigned int)count)<< 3);
	countHi += ( ((unsigned int)count)>>29);

	/* Process data in SHS_BLOCKSIZE chuncks */
	memcpy(data,buffer,SHS_BLOCKSIZE);
	#ifdef LITTLE_ENDIAN
	byteReverse(data,SHS_BLOCKSIZE);
	#endif
	Transform();
	}

void SHS::Final(unsigned char *fill)
	{
	short count = stored;

    if( ( countLo + ( ( unsigned long ) count << 3 ) ) < countLo )
		countHi++; /* Carry from low to high bitCount */

	countLo += ( ((unsigned int)count)<< 3);
	countHi += ( ((unsigned int)count)>>29);

	unsigned int lowbitcount =countLo; 
	unsigned int highbitcount=countHi;

	memcpy(data,store,count);

	/* Compute number of bytes mod 64 */

	count = (int)((countLo>>3)&0x3F);

	// if(count != stored) exit(0);

	/* Set the first char of padding to 0x80. This is safe because there is
		always at least one byte free. */

	((unsigned char *)data)[count++]=0x80;

	/* Pad out to 56 mod 64 */
	if(count>56)
		{
		/* Two lots of padding. Pad the first block to 64 bytes */
		memset((unsigned char *)&data + count,0,64-count);
		#ifdef LITTLE_ENDIAN
		byteReverse(data,SHS_BLOCKSIZE);
		#endif /* LITTLE ENDIAN */
		Transform();

		/* Now fill the next block with 56 bytes */
		memset(&data,0,56);
		}
	else
		{
		/* Pad block to 56 bytes */
		memset((unsigned char *)&data+count,0,56-count);
		#ifdef LITTLE_ENDIAN
		byteReverse(data,SHS_BLOCKSIZE);
		#endif /* LITTLE ENDIAN */
		}

	/* Append length in bits and transform */

	data[14]=highbitcount;
	data[15]=lowbitcount;

	Transform();
	#ifdef LITTLE_ENDIAN
	byteReverse(data,SHS_DIGESTSIZE);
	#endif /* LITTLE ENDIAN */

	memcpy(hash,digest,BlockSize());
	if(fill != NULL) memcpy(fill,hash,BlockSize());
	}

#undef LITTLE_ENDIAN

// *************************************************************************************

