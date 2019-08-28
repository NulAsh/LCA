
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// KeyPair.h: interface for the CKeyPair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYPAIR_H__8ACFA53A_87AC_40DB_89EA_E7B8DD5EFFB3__INCLUDED_)
#define AFX_KEYPAIR_H__8ACFA53A_87AC_40DB_89EA_E7B8DD5EFFB3__INCLUDED_

#if _MSC_VER > 1000
#pragma warning( disable : 4786 4788)
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "SDMatrix.h"
#include "LNMatrix.h"
#include "FastKey.h"

#include "StringGen.h"
#include "Blowfish.h"

#include "callbacks.h"

#define SIGNATURE_FAILED    -8

#define MAXKEYSIZEALLOWED   (2*1024*1024)

#define LERRBUFFSIZE        1024
#define W32_FINGERPRINTSIZE    5

class CKeyPair  
{
public:
	typedef enum {
		PRIVKEY = 1,
		PUBKEY  = 2,
		ALLKEYS = 3,
		} KEYTYPE;

	friend class CDRegZ;

	class CHeader
		{
		public:
			static const char *hd;
			int version;
			WORD_8 digest[20];
			char encrypted;
			WORD_8 encblock[8];
			char   keytype;

		public:
			int TestEncryption(void);
			int DecryptBuffer(unsigned char *buff,int buffsize);
			int EncryptBuffer(unsigned char *buff,int buffsize);
			int CheckPassword(const char *password,int passsize=-1);
			int SetPassword(const char *password,int passsize=-1);
			CHeader();
			int FlushIn(FILE *h1);
			int FlushOut(FILE *h1);

		private:
			static unsigned int headersize;
			BLOWFISH_CTX ctx;
		};

protected:
	int SetError(int code,char *errstr=NULL,int subcode=0);
	
	int DumpMatrix(const char *filename);
	int Verify(ELEMENT *in, int isize, ELEMENT *out,int osize);
	int Sign(ELEMENT *in, int isize, ELEMENT *out,int osize);

	int status;

	int mhsize;
	int mvsize;
	
	int mul;
	int vsize;
	int hsize;

	char lerrstr[LERRBUFFSIZE];
	int lerrcode;
	int lerrsubcode;

	LNMatrix m1,m2;
	LNMatrix s1,s2,s3;
	SDMatrix mtx,jlt;
	CFastKey fkey;
	int signatures;
	int s_failures;
	bool privkeyavail;
	bool pubkeyavail;
	static FN_GAUGE_CALLBACK GaugeCallBack;
	
public:
	CKeyPair();
	virtual ~CKeyPair();

	static void SetGauge(FN_GAUGE_CALLBACK _GaugeCallBack) { GaugeCallBack = _GaugeCallBack; }
	static double GetFailProb(int key,int bits);
	static int GetMaxKeySlots();
	
	int DerivePublicKeyFromPrivate(void);
	int WriteCStructure(const char *path,int testcodes=0);
	int GetDefaultSuffix(KEYTYPE kt,STRING &suffix);
	void Reset(void);
	int LoadKey(KEYTYPE kt,const char *filename,const char *password=NULL,FN_PASSWORD_CALLBACK pCallBack=NULL);
	int ComputeFingerprint(WORD32 *fp,int maxsize=W32_FINGERPRINTSIZE);
	int SaveKeyPair(const char *filename, const char *password);
	int SavePublicKey(const char *filename);
	int SavePrivateKey(const char *filename, const char *password);
	int SaveKey(KEYTYPE kt, const char *filename, const char *password=NULL);
	int CodeVerify(STRING code,int *licid=NULL,int *round=NULL);

	int CreateKeyPair(int keytype);
	int CodeVerify(const char *code,int *licid=NULL,int *round=NULL);
	int CodeSign(int licid, STRING &code);
	
	int DecompressBuffer(KEYTYPE ktp,WORD_8 *buff);
	int CompressToBuffer(KEYTYPE ktp,WORD_8 *buff, int &buffsize);
	int GetReqBuffSize(KEYTYPE ktp);
	
	int GetVarsNo() { return mhsize; }
	int GetEqsNo()  { return mvsize; }
	int GetHSize()  { return hsize;  }
	int GetVSize()  { return vsize;  }
	int GetMul()    { return mul;    }
	
	int GetSignaturesNo() { return signatures; }
	int GetFailuresNo()   { return s_failures; }

	bool IsPublicKeyAvailable()  { return pubkeyavail;  }
	bool IsPrivateKeyAvailable() { return privkeyavail; }

	int Test(void);

	int hinosolscore;
	int clicid;
	int t_time;
};

#endif // !defined(AFX_KEYPAIR_H__8ACFA53A_87AC_40DB_89EA_E7B8DD5EFFB3__INCLUDED_)
