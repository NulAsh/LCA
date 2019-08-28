
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// KeyPair.cpp: implementation of the CKeyPair class.
//
//////////////////////////////////////////////////////////////////////

// #include <windows.h>
#include "KeyPair.h"
#include "LNPoly.h"
#include "SHA.h"
#include "crc32.h"
#include "Globals.h"

//////////////////////////////////////////////////////////////////////

#define BF_LEFT_CHECK    0xA9FBC4D1
#define BF_RIGHT_CHECK   0x33E1D975

#define KEY_VERSION_ID   0x00010001

CKeyPair::CHeader::CHeader()
	{
	memset(encblock,0x00,sizeof(encblock));
	memset(digest,0x00,sizeof(digest));

	version = KEY_VERSION_ID;
	encrypted = 0;
	}

int CKeyPair::CHeader::SetPassword(const char *password,int passsize)
{
	memset(encblock,0x00,sizeof(encblock));

	if(passsize == -1) passsize = (int)strlen(password);

	unsigned long left=BF_LEFT_CHECK,right=BF_RIGHT_CHECK;
	SHS shs;

	shs.Update((unsigned char *)password,passsize);
	shs.Final();

	Blowfish_Init(&ctx,shs.GetHashPtr(),shs.BlockSize());
	Blowfish_Encrypt(&ctx,&left,&right);

	memcpy(encblock,&left,sizeof(left));
	memcpy(encblock+sizeof(left),&right,sizeof(right));

	encrypted = 1;

	return 1;
}


int CKeyPair::CHeader::CheckPassword(const char *password,int passsize)
{
	unsigned long left,right;
	SHS shs;

	if(passsize == -1) passsize = (int)strlen(password);

	memcpy(&left ,encblock             ,sizeof(left ));
	memcpy(&right,encblock+sizeof(left),sizeof(right));

	shs.Update((unsigned char *)password,passsize);
	shs.Final();

	Blowfish_Init(&ctx,shs.GetHashPtr(),shs.BlockSize());
	Blowfish_Decrypt(&ctx,&left,&right);

	if(left != BF_LEFT_CHECK || right != BF_RIGHT_CHECK) return 0;
	return 1;

}

const char *CKeyPair::CHeader::hd = "DRegZKey";
unsigned int CKeyPair::CHeader::headersize = 8 + 4 + 1 + 20 + 1 + 8 + 4;

int CKeyPair::CHeader::FlushOut(FILE *h1)
{
	unsigned char buff[100];
	unsigned int ptr = 0;

	memcpy(&buff[ptr],hd,strlen(hd));
	ptr+=(int)strlen(hd);

	memcpy(&buff[ptr],&version,sizeof(version));
	ptr+=sizeof(version);

	memcpy(&buff[ptr],&keytype,sizeof(keytype));
	ptr+=sizeof(keytype);

	memcpy(&buff[ptr],digest,sizeof(digest));
	ptr+=sizeof(digest);

	memcpy(&buff[ptr],&encrypted,sizeof(encrypted));
	ptr+=sizeof(encrypted);

	memcpy(&buff[ptr],encblock,sizeof(encblock));
	ptr+=sizeof(encblock);

	WORD32 fcrc = crc(buff,ptr);
	
	memcpy(&buff[ptr],&fcrc,sizeof(fcrc));
	ptr+=sizeof(fcrc);

	if(ptr != headersize) return -1;

	if(fwrite(buff,1,ptr,h1) != ptr) return 0;
	return 1;	
}

int CKeyPair::CHeader::FlushIn(FILE *h1)
{
	unsigned char buff[100];
	unsigned int ptr = 0;
	int rr;

	if(fread(buff,1,headersize,h1) != headersize) return -3;

	rr = memcmp(hd,&buff[ptr],strlen(hd));
	if(rr != 0) return -1;
		
	ptr+=(int)strlen(hd);

	memcpy(&version,&buff[ptr],sizeof(version));
	ptr+=sizeof(version);

	memcpy(&keytype,&buff[ptr],sizeof(keytype));
	ptr+=sizeof(keytype);

	memcpy(digest,&buff[ptr],sizeof(digest));
	ptr+=sizeof(digest);

	memcpy(&encrypted,&buff[ptr],sizeof(encrypted));
	ptr+=sizeof(encrypted);

	memcpy(encblock,&buff[ptr],sizeof(encblock));
	ptr+=sizeof(encblock);

	WORD32 tcrc = crc(buff,ptr);
	
	rr = memcmp(&tcrc,&buff[ptr],sizeof(tcrc));
	if(rr != 0) return -2;
	ptr+=sizeof(tcrc);

	if(ptr != headersize) return -1;
	return 1;	
}

int CKeyPair::CHeader::EncryptBuffer(unsigned char *buff, int buffsize)
{
	if((buffsize & 0x07) != 0) return -1;

	unsigned long oleft = 0, oright = 0;
	unsigned long nl,nr;

	unsigned long *lbuff = (unsigned long *)(buff + 0);
	unsigned long *rbuff = (unsigned long *)(buff + 4);

	int blocks = buffsize >> 3;

	for(int i=0;i<blocks;i++)
		{
		nl = oleft  ^ *lbuff;
		nr = oright ^ *rbuff;

		oleft  = *lbuff;
		oright = *rbuff;

		Blowfish_Encrypt(&ctx,&nl,&nr);

		*lbuff = nl;
		*rbuff = nr;

		lbuff+=2;
		rbuff+=2;
		}

	return blocks;
}

int CKeyPair::CHeader::DecryptBuffer(unsigned char *buff, int buffsize)
{
	if((buffsize & 0x07) != 0) return -1;

	unsigned long oleft = 0, oright = 0;
	unsigned long nl,nr;

	unsigned long *lbuff = (unsigned long *)(buff + 0);
	unsigned long *rbuff = (unsigned long *)(buff + 4);

	int blocks = buffsize >> 3;

	for(int i=0;i<blocks;i++)
		{
		nl =  *lbuff;
		nr =  *rbuff;

		Blowfish_Decrypt(&ctx,&nl,&nr);

		*lbuff = oleft  ^ nl;
		*rbuff = oright ^ nr;

		oleft  = *lbuff;
		oright = *rbuff;

		lbuff+=2;
		rbuff+=2;
		}

	return blocks;
}

int CKeyPair::CHeader::TestEncryption()
{
	//SetPassword("1234567890ABCDEF");

	const int keysize = 256;
	char key[keysize];

	RandPool.GetBlock((unsigned char *)key,keysize);
	SetPassword(key,keysize);

	const int buffsize = 4096;
	unsigned char buff1[buffsize],buff2[buffsize];

	RandPool.GetBlock(buff1,buffsize);
	memcpy(buff2,buff1,buffsize);

	int rr;

	rr = EncryptBuffer(buff1,buffsize);
	if(rr < 0) return -1;

	if(memcmp(buff1,buff2,buffsize)==0) return -2;

	rr = DecryptBuffer(buff1,buffsize);
	if(rr < 0) return -3;

	if(memcmp(buff1,buff2,buffsize)!=0) return 0;

	return 1;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef struct {
	int hsize,vsize,mul;
	double failprob;
	} KEYSLOT;

#define KEYSLOTSNO    4

KEYSLOT ks[KEYSLOTSNO] = {
	{ 16,15, 8, 0.20397 },	// In: 128   Out: 120 [- 36 = 84]
	{ 14,13, 9, 0.20472 },  // In: 126   Out: 117 [- 36 = 81]
	{ 12,11,11, 0.19666 },  // In: 132   Out: 121 [- 36 = 85]
	{  8,7, 16, 0.16683 },  // In: 128   Out: 112 [- 36 = 76]
	};

// ----------------------------
//
// Usefull irred polys
//
//  X^76+X^5+X^4+X^2+1
//  X^81+X^4+1
//  X^84+X^5+1
//  X^85+X^8+X^2+X+1
//
// ---------------------------------------------------------------  
//                  Repeated failure probability
// ---------------------------------------------------------------  
// Key=0  { 16,15, 8, 0.20205 }       Key=2  { 12,11,11, 0.20093 }  
//
// bits=0, prob=2.020500e-001         bits=0, prob=2.009300e-001    
// bits=1, prob=4.082420e-002         bits=1, prob=4.037286e-002    
// bits=2, prob=1.666616e-003         bits=2, prob=1.629968e-003    
// bits=3, prob=2.777607e-006         bits=3, prob=2.656796e-006    
// bits=4, prob=7.715102e-012         bits=4, prob=7.058567e-012    
// bits=5, prob=5.952280e-023         bits=5, prob=4.982337e-023    
// bits=6, prob=3.542964e-045         bits=6, prob=2.482368e-045    
// bits=7, prob=1.255259e-089         bits=7, prob=6.162152e-090    
// bits=8, prob=1.575676e-178         bits=8, prob=3.797211e-179    
// bits=9, prob=0.000000e+000         bits=9, prob=0.000000e+000    
// ---------------------------------------------------------------  
// Key=1  { 14,13, 9, 0.20754 }       Key=3  {  8,7, 16, 0.16648 }  
//
// bits=0, prob=2.075400e-001         bits=0, prob=1.664800e-001    
// bits=1, prob=4.307285e-002         bits=1, prob=2.771559e-002    
// bits=2, prob=1.855271e-003         bits=2, prob=7.681540e-004    
// bits=3, prob=3.442029e-006         bits=3, prob=5.900605e-007    
// bits=4, prob=1.184756e-011         bits=4, prob=3.481714e-013    
// bits=5, prob=1.403647e-022         bits=5, prob=1.212233e-025    
// bits=6, prob=1.970226e-044         bits=6, prob=1.469509e-050    
// bits=7, prob=3.881790e-088         bits=7, prob=2.159457e-100    
// bits=8, prob=1.506829e-175         bits=8, prob=4.663255e-200    
// bits=9, prob=0.000000e+000         bits=9, prob=0.000000e+000    
// ---------------------------------------------------------------  

FN_GAUGE_CALLBACK CKeyPair::GaugeCallBack = NULL;

double CKeyPair::GetFailProb(int key,int bits)
{
	if(key < 0 || key >= KEYSLOTSNO) return -1.0;
	if(bits < 0 || bits > 1000) return -2.0;

	double fp = ks[key].failprob;

	for(int i=0;i<bits;i++)
		fp = fp * fp;

	return fp;
}


int CKeyPair::GetMaxKeySlots()
{
	return KEYSLOTSNO;
}

CKeyPair::CKeyPair()
{
	Reset();

	t_time     = 0;
	signatures = 0;
	s_failures = 0;

	clicid = 0;
	hinosolscore = 0;
}

CKeyPair::~CKeyPair()
{
	Reset();
}


int CKeyPair::SetError(int code, char *errstr, int subcode)
{
	if(errstr != NULL) strncpy(lerrstr,errstr,LERRBUFFSIZE);
	lerrcode = code;
	lerrsubcode = subcode;

	return code;
}

void CKeyPair::Reset()
{
	privkeyavail = false;
	pubkeyavail  = false;

	memset(lerrstr,0x00,LERRBUFFSIZE);
	lerrcode = lerrsubcode = 0;

	status = 0;
}

int CKeyPair::GetDefaultSuffix(KEYTYPE kt,STRING &suffix)
{
	char str[200];
	char ext[10];

	switch(kt)
		{
		case PRIVKEY:
			if( privkeyavail == false ) return 0;
			strcpy(ext,".prv");
			break;

		case PUBKEY:
			if( pubkeyavail == false ) return 0;
			strcpy(ext,".pub");
			break;

		case ALLKEYS:
			if( privkeyavail == false || pubkeyavail == false ) return 0;
			strcpy(ext,".key");
			break;
		}

	sprintf(str,"_%dx%dx%d%s",hsize,vsize,mul,ext);

	suffix = str;
	return 1;
}

int CKeyPair::WriteCStructure(const char *path,int testcodes)
{
	if(testcodes > 0)
		{
		if(privkeyavail == false) return -101;
		PrintLog("Creating some samples codes...\n");

		for(int i=0;i<testcodes;i++)
			{
			STRING license;

			int rr = CodeSign(i,license);
			if(rr < 0) return -102+rr;

			//PrintLog("Code #%d/%d\n",i+1,testcodes);
			fkey.testcodes.push_back(license);
			}
		}

	return fkey.WriteCStructure(path);
}


int CKeyPair::DerivePublicKeyFromPrivate()
{
	if(!privkeyavail) return 0;
	if(pubkeyavail) return 1;

	PrintLog("Jolting public key from private...\n");
	int rr = mtx.Jolt(jlt,m1,s1,GaugeCallBack);
	if(rr <= 0) return rr;

	//PrintLog("Creating fastkey...\n");
	jlt.CreateFastKey(fkey);

	WORD32 fingerprint[5];
	ComputeFingerprint(fingerprint,5);

	fkey.SetFingerPrint(fingerprint,5);

	pubkeyavail  = true;
	return 1;
}

int CKeyPair::CreateKeyPair(int keytype)
{
	if(keytype < 0 || keytype >= KEYSLOTSNO) return -1;

	hsize = ks[keytype].hsize;
	vsize = ks[keytype].vsize;
	mul   = ks[keytype].mul;

	mhsize = hsize * mul;
	mvsize = vsize * mul;

	PrintLog("Creating key...\n");

	m1.Initialize(mvsize,mvsize);
	m1.CreateInvertible();
	
	m1.Invert(m2);

	s1.Initialize(mhsize,mhsize);
	s1.CreateInvertible();
	
	s1.Invert(s2);
	
	mtx.Init(hsize,vsize,mul);
	mtx.SetRandom();

	privkeyavail = true;
	status = 1;

	DerivePublicKeyFromPrivate();
	// fkey.WriteCStructure("decoder");

	// printf("Time elapsed: %3.2f\n",(end-start)/1000.0);
	
	return 1;
}

int CKeyPair::Sign(ELEMENT *in, int isize, ELEMENT *out, int osize)
{
	if(status <     1) return -1;
	if(isize != mvsize) return -2;
	if(osize != mhsize) return -3;

	if( !privkeyavail ) return -4;
	if( !pubkeyavail ) 
		{
		mtx.Jolt(jlt,m1,s1);
		jlt.CreateFastKey(fkey);
		}

	ELEMENT er[MAXMTXSIZE];
	ELEMENT xv1[MAXMTXSIZE];

	if( m2.MulLVect(in,mvsize,er) != 1 ) return SetError(-7,"Error vsize !\n");

	SOLLIST sols;
	int rr;

	rr = mtx.SolveEx(sols,er,mvsize);
	if(rr <= 0)
		{
		s_failures++;
		return SIGNATURE_FAILED;
		}

	rr = SDMatrix::PickVectFromList(0,sols,xv1,mhsize);
	if(rr <= 0) return SetError(-9,"Error picking solution");
	
	if( s2.MulLVect(xv1,mhsize,out) != 1 ) return SetError(-10,"Error xrsize!\n");

	signatures++;
	return 1;
}

int CKeyPair::Verify(ELEMENT *in, int isize, ELEMENT *out, int osize)
{
	if(status <      1) return -1;
	if(isize != mhsize) return -2;
	if(osize != mvsize) return -3;

	if(!pubkeyavail) return -4;

	ELEMENT vr2[1000];
	memset(vr2,0x00,1000 * sizeof(ELEMENT));

	jlt.Evaluate(in,isize,out,mvsize);
	fkey.Evaluate(in,isize,vr2,mvsize);

	if(memcmp(out,vr2,mvsize) != 0)
		{
		/*
		PrintBuffer((WORD_8 *)in,isize);
		printf("-------------------------------------------------------\n");
		PrintBuffer((WORD_8 *)out,mvsize);
		printf("-------------------------------------------------------\n");
		PrintBuffer((WORD_8 *)vr2,mvsize);
		printf("-------------------------------------------------------\n");
		printf("Different buffers (darn!).\n");
		*/
		return 0;
		}

	return 1;
}


int CKeyPair::DumpMatrix(const char *filename)
{
	return jlt.DumpToFile(filename);
}

int CKeyPair::GetReqBuffSize(KEYTYPE ktp)
{
	switch(ktp)
		{
		case PRIVKEY:
			return 6 + m1.GetReqBuffSize() + s1.GetReqBuffSize() + mtx.GetReqBuffSize();

		case PUBKEY:
			return 4 + jlt.GetReqBuffSize();

		case ALLKEYS:
			return GetReqBuffSize(PRIVKEY) + GetReqBuffSize(PUBKEY);

		default:
			return -1;
		}
}

int CKeyPair::CompressToBuffer(KEYTYPE ktp, WORD_8 *buff, int &buffsize)
{
	int reqsize = GetReqBuffSize(ktp);
	if(buffsize < reqsize)
		{
		buffsize = reqsize;
		return -1;
		}

	int top=0;
	int rr;

	switch(ktp)
		{
		case PRIVKEY:

			*(WORD16 *)(buff + 0) = (WORD16)hsize;
			*(WORD16 *)(buff + 2) = (WORD16)vsize;
			*(WORD16 *)(buff + 4) = (WORD16)mul;
			buff+=6;
			reqsize-=6;
			top+=6;

			rr = m1.CompressToBuffer(buff,reqsize);
			if(rr < 0) return rr;
			reqsize-=rr;
			buff+=rr;
			top+=rr;

			rr = s1.CompressToBuffer(buff,reqsize);
			if(rr < 0) return rr;
			reqsize-=rr;
			buff+=rr;
			top+=rr;

			rr = mtx.CompressToBuffer(buff,reqsize);
			if(rr < 0) return rr;
			reqsize-=rr;
			buff+=rr;
			top+=rr;

			if(reqsize != 0) return -10;
			break;

		case PUBKEY:

			*(WORD16 *)(buff + 0) = (WORD16)mhsize;
			*(WORD16 *)(buff + 2) = (WORD16)mvsize;
			buff+=4;
			reqsize-=4;
			top+=4;
			
			rr = jlt.CompressToBuffer(buff,reqsize);
			if(rr < 0) return rr;
			reqsize-=rr;
			buff+=rr;
			top+=rr;

			if(reqsize != 0) return -10;
			break;

		case ALLKEYS:
			rr = CompressToBuffer(PRIVKEY,buff,reqsize);
			if(rr < 0) return rr;
			reqsize-=rr;
			buff+=rr;
			top+=rr;

			rr = CompressToBuffer(PUBKEY,buff,reqsize);
			if(rr < 0) return rr;
			reqsize-=rr;
			buff+=rr;
			top+=rr;

			if(reqsize != 0) return -11;
			break;

		default:
			return -12;
		}

	return top;
}

int CKeyPair::DecompressBuffer(KEYTYPE ktp, WORD_8 *buff)
{
	int rr,top=0;

	switch(ktp)
		{
		case PRIVKEY:
			hsize = *(WORD16 *)(buff + 0);
			vsize = *(WORD16 *)(buff + 2);
			mul   = *(WORD16 *)(buff + 4);
			buff+=6;
			top+=6;

			rr = m1.DecompressBuffer(buff);
			if(rr < 0) return rr;
			buff+=rr;
			top+=rr;

			rr = s1.DecompressBuffer(buff);
			if(rr < 0) return rr;
			buff+=rr;
			top+=rr;

			rr = mtx.DecompressBuffer(buff);
			if(rr < 0) return rr;
			buff+=rr;
			top+=rr;

			rr = m1.Invert(m2);
			if(rr < 0) return rr;

			rr = s1.Invert(s2);
			if(rr < 0) return rr;

			privkeyavail = true;
			break;

		case PUBKEY:
			mhsize = *(WORD16 *)(buff + 0);
			mvsize = *(WORD16 *)(buff + 2);
			buff+=4;
			top+=4;

			rr = jlt.DecompressBuffer(buff);
			if(rr < 0) return rr;
			buff+=rr;
			top+=rr;

			rr = jlt.CreateFastKey(fkey);
			if(rr < 0) return rr;
			else
				{
				WORD32 fingerprint[5];
				ComputeFingerprint(fingerprint,5);

				fkey.SetFingerPrint(fingerprint,5);
				}

			pubkeyavail = true;
			break;

		case ALLKEYS:
			rr = DecompressBuffer(PRIVKEY,buff);
			if(rr < 0) return rr;
			buff+=rr;
			top+=rr;
			
			rr = DecompressBuffer(PUBKEY,buff);
			if(rr < 0) return rr;
			buff+=rr;
			top+=rr;

			break;

		default:
			return -11;
		}

	status = 1;
	return top;
}

int CKeyPair::Test()
{
#ifdef AUTOTEST
	printf("Autotesting...\n");

	CKeyPair kk;

	int buffsize = GetReqBuffSize(ALLKEYS);
	WORD_8 *buff = new WORD_8[buffsize+1];

	int rr = CompressToBuffer(ALLKEYS,buff,buffsize);	
	if(rr < 0) return rr;

	printf("Ok, %d bytes buffered...\n",rr);

	rr = kk.DecompressBuffer(ALLKEYS,buff);
	printf("Ok, %d bytes digested...\n",rr);

	delete[] buff;

	int eqs = GetEqsNo();
	ELEMENT e1[MAXMTXSIZE],e2[MAXMTXSIZE];

	int varsno = GetVarsNo();
	ELEMENT xv1[MAXMTXSIZE];

	printf("Testing: %d eqs., %d vars.\n",eqs,varsno);

	int rounds;
	
	rounds = 0;
	do 
		{
		if(rounds > 0) printf("Trying a different key...\n");

		SDVect::RandElementVector(e1,36);
		LNPoly::ComputeParity(e1,36,e1+36,eqs-36);

		printf("Signing (round=%d)...\n",rounds+1);
		rr = Sign(e1,eqs,xv1,varsno);
		
		rounds++;
		} while( rr == -8 );	// Resigning ?...

	if(rr != 1) return -1;

	rr = kk.Verify(xv1,varsno,e2,eqs);
	if(rr != 1) return -1;

	rounds = 0;
	do 
		{
		if(rounds > 0) printf("Trying a different key...\n");

		SDVect::RandElementVector(e1,36);
		LNPoly::ComputeParity(e1,36,e1+36,eqs-36);

		printf("Signing (round=%d)...\n",rounds+1);
		rr = kk.Sign(e1,eqs,xv1,varsno);
		
		rounds++;
		} while( rr == -8 );	// Resigning ?...

	if(rr != 1) return -1;

	rr = Verify(xv1,varsno,e2,eqs);
	if(rr != 1) return -1;

#endif
	/* Finished... */
	return 1;
}


int CKeyPair::CodeSign(int licid, STRING &code)
	{
	int eqs = GetEqsNo();
	ELEMENT e1[MAXMTXSIZE],e2[MAXMTXSIZE];

	int varsno = GetVarsNo();
	ELEMENT xv1[MAXMTXSIZE];

	int round = 0;
	int rr,i;

	while(round < TOTROUNDS) 
		{
		int mask = round ^ (TOTROUNDS-1);
		round++;

		for(i=0;i<LICIDBITS;i++)
			e1[i] = (licid >> i) & 0x01;

		for(i=0;i<ROUNDBITS;i++)
			e1[i+LICIDBITS] = ((mask >> i) & 0x01);
		
		LNPoly::ComputeParity(e1,KEYIDSIZE,e1+KEYIDSIZE,eqs-KEYIDSIZE);
		rr = Sign(e1,eqs,xv1,varsno);
		
		if     (rr == 1) break;
		else if(rr != SIGNATURE_FAILED) return -100-rr;
		} 

	if(round > hinosolscore) hinosolscore = round;

	if(round == TOTROUNDS) return 0;
	if(round == (TOTROUNDS-1) && licid == 0) return 0;

	rr = Verify(xv1,varsno,e2,eqs);
	if(rr != 1) return -2;

	rr = LNPoly::VerifyParity(e2,KEYIDSIZE,e2+KEYIDSIZE,eqs-KEYIDSIZE);
	if(rr != 1) return -3;

	rr = CStringGen::Element2CodeStr(xv1,125,code);
	if(rr <= 0) return rr;

	return round;
	}	

int CKeyPair::CodeVerify(STRING code, int *licid, int *round)
{
	return CodeVerify(code.c_str(),licid,round);
}

int CKeyPair::CodeVerify(const char *code, int *licid,int *round)
{
	int eqs = GetEqsNo();

	int psize = GetVarsNo();
	ELEMENT p[MAXMTXSIZE];
	memset(p,0x00,MAXMTXSIZE);

	int cbits = CStringGen::CodeStr2Element(code,p,MAXMTXSIZE);
	if( cbits != 125 ) return -97;
	
	int delta = psize - cbits;
	if(delta < 0) return -15;

	int top = 1 << delta;

	int loops,rr;

	for(loops=0;loops<top;loops++)
		{
		ELEMENT e2[MAXMTXSIZE];
		memset(e2,0x00,MAXMTXSIZE);

		int i,jj;

		for(jj=0;jj<delta;jj++)
			p[cbits+jj] = (loops >> jj) & 0x01;

		rr = fkey.Evaluate(p,psize,e2,eqs);
		if(rr != 1) return -2;

		rr = LNPoly::VerifyParity(e2,KEYIDSIZE,e2+KEYIDSIZE,eqs-KEYIDSIZE);
		if(rr == 1)
			{
			if(licid != NULL)
				{
				*licid = 0;

				for(i=0;i<LICIDBITS;i++)
					if(e2[i] != 0) *licid |= (1 << i);
				}

			if(round != NULL)
				{
				*round = 0;

				for(i=0;i<ROUNDBITS;i++)
					if(e2[i+LICIDBITS] != 0) *round |= (1 << i);
				}

			// Signature verified...
			return 1;
			}
		}

	// Signature not verified...
	return 0;	
}


int CKeyPair::SaveKey(KEYTYPE kt, const char *filename, const char *password)
{
	CHeader h;
	int rr;

	if(password != NULL && strlen(password) > 0) h.SetPassword(password);
	h.keytype = (char)kt;

	unsigned int buffsize = (GetReqBuffSize(kt) + 7) & 0xFFFFFFF8;
	WORD_8 *buff = new WORD_8[buffsize];
	if(!buff) return -1;

	memset(buff, 0x00, buffsize);

	int size = buffsize;
	size = CompressToBuffer(kt,buff,size);
	if(size <= 0 || size > (int)buffsize) 
		{
		delete[] buff;
		return -2;
		}

	SHS shs;

	shs.Update(buff,buffsize);
	shs.Final();

	memcpy(h.digest,shs.GetHashPtr(),shs.BlockSize());

	if(h.encrypted) 
		{
		rr = h.EncryptBuffer(buff,buffsize);
		if(rr < 0) 
			{
			delete[] buff;
			return rr;
			}
		}

	FILE *h1 = fopen(filename,"wb");
	if(!h1) 
		{
		delete[] buff;
		return -10;
		}

	rr = h.FlushOut(h1);
	if(rr < 0) 
		{
		fclose(h1);
		delete[] buff;
		return rr;
		}

	if( fwrite(&buffsize,1,sizeof(buffsize),h1) != sizeof(buffsize))
		{
		fclose(h1);
		delete[] buff;
		return -11;
		}

	if( fwrite(buff,1,buffsize,h1) != buffsize)
		{
		fclose(h1);
		delete[] buff;
		return -11;
		}

	fclose(h1);
	delete[] buff;

	return 1;	
}

int CKeyPair::ComputeFingerprint(WORD32 *fp, int maxsize)
{
	SHS shs;
	int blocksize = shs.BlockSize();
	
	if(maxsize * sizeof(WORD32) < (unsigned int)blocksize) return 0;

	int buffsize = GetReqBuffSize(ALLKEYS);
	WORD_8 *tmp = new WORD_8[buffsize];

	int rr = CompressToBuffer(ALLKEYS,tmp,buffsize);

	shs.Update(tmp,buffsize);
	
	delete[] tmp;

	shs.Final();

	if(rr <= 0) return 0;

	memcpy(fp,shs.GetHashPtr(),blocksize);
	return blocksize;
}


int CKeyPair::SavePrivateKey(const char *filename, const char *password)
{
	return SaveKey(PRIVKEY,filename,password);
}

int CKeyPair::SaveKeyPair(const char *filename, const char *password)
{
	return SaveKey(ALLKEYS,filename,password);
	
}

int CKeyPair::SavePublicKey(const char *filename)
{
	return SaveKey(PUBKEY,filename);
}

int CKeyPair::LoadKey(KEYTYPE kt, const char *filename, const char *password, FN_PASSWORD_CALLBACK pCallBack)
	{
	FILE *h1 = fopen(filename,"rb");
	if(!h1) return -1;

	CHeader h;
	int rr;

	rr = h.FlushIn(h1);
	if(rr != 1) return rr;

	if( ((int)kt & h.keytype) != (int)kt) return -101;

	int ok2proceed = 1;

	if(h.encrypted)
		{
		ok2proceed = 0;

		if(password)
			{
			rr = h.CheckPassword(password);
			if(rr == 1) ok2proceed = 1;
			}
			
		if(ok2proceed == 0 && pCallBack != NULL)
			{
			int pcount = 0;
			
			do
				{
				const int psize = 1024;
				char pass[psize];

				memset(pass,0x00,psize);

				rr = pCallBack(pass,psize);
				if(rr <= 0) break;

				rr = h.CheckPassword(pass);
				if(rr == 1) ok2proceed = 1;
				else pcount++;

				} while(pcount < 3 && ok2proceed == 0);
			}
		}

	if(ok2proceed != 1)
		{
		fclose(h1);
		return -2;
		}

	int buffsize;

	if(fread(&buffsize,1,sizeof(buffsize),h1) != sizeof(buffsize))
		{
		fclose(h1);
		return -3;
		}

	if(buffsize < 1 || buffsize > MAXKEYSIZEALLOWED)
		{
		fclose(h1);
		return -4;
		}	

	unsigned char *buff = new unsigned char[buffsize];
	if(!buff)
		{
		fclose(h1);
		return -5;
		}

	if( fread(buff,1,buffsize,h1) != (unsigned int)buffsize )
		{
		delete[] buff;
		fclose(h1);
		return -6;
		}

	if(h.encrypted)
		{
		rr = h.DecryptBuffer(buff,buffsize);
		if(rr < 0)
			{
			delete[] buff;
			fclose(h1);
			return -7;
			}
		}

	SHS shs;
	shs.Update(buff,buffsize);
	shs.Final();

	if(memcmp(h.digest,shs.GetHashPtr(),shs.BlockSize()) != 0)
		{
		delete[] buff;
		fclose(h1);
		return -8;
		}

	KEYTYPE kt2 = (KEYTYPE)h.keytype;

	Reset();
	rr = DecompressBuffer(kt2,buff);

	if(rr < 0)
		{
		Reset();
		
		delete[] buff;
		fclose(h1);
		return -9;
		}

	delete[] buff;
	fclose(h1);

	return 1;
	}




















