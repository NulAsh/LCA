// SHS.h: interface for the SHS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHS_H__BA9C7567_FC43_465D_A5A7_A37732820605__INCLUDED_)
#define AFX_SHS_H__BA9C7567_FC43_465D_A5A7_A37732820605__INCLUDED_

#define SHS_TEST_BUFFSIZE  (128*1024)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SHS

/* SHS.H */
/* Nist Secure Hash Standard. */

/* Written 2 September 1992, Peter C. Gutmann.
	This implementation placed in public domain. */

/* The SHS block size and message digest sizes, in bytes */

#define SHS_BLOCKSIZE	64
#define SHS_DIGESTSIZE	20

#include <stdio.h>

#define TESTBUFFSIZE    (1024*1024)

/* The structure for storing SHS info */

class GenericHashModule
	{
	public:
		GenericHashModule() { ; }
		virtual ~GenericHashModule() { ; }

	public:
		virtual void Init()=0;
		virtual void Update(const unsigned char *,int)=0;
		virtual void Final(unsigned char *fill=NULL)=0;
		
		virtual const int BlockSize()=0;
		virtual unsigned char *GetHashPtr()=0;

		static GenericHashModule *HashFactory(int hashID);

	};

class SHS : public GenericHashModule
	{
	private:
		void Transform();

		unsigned int digest[5];   			/* Message digest */
		unsigned int countLo,countHi;		/* 64 bit-bit count */
		unsigned int data[16];

		unsigned int hash[5];

		unsigned char *store;
		int stored;

	private:
		void TrueUpdate(const unsigned char *,int);

	public:
		SHS();
		~SHS();

		void Init();
		void Update(const unsigned char *,int);
		void Final(unsigned char *fill=NULL);

		const int BlockSize() { return SHS_DIGESTSIZE; }
		unsigned char *GetHashPtr() { return (unsigned char *)hash; }
	};

/* Whether the machine is little endian or not */

int TestHash(int buffsize=SHS_TEST_BUFFSIZE);

#endif // !defined(AFX_SHS_H__BA9C7567_FC43_465D_A5A7_A37732820605__INCLUDED_)
