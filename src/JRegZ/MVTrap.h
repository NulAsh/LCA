/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// MVTrap.h: interface for the CMVTrap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MVTRAP_H__DCF8BD01_7331_4F63_9162_5658C34F33A3__INCLUDED_)
#define AFX_MVTRAP_H__DCF8BD01_7331_4F63_9162_5658C34F33A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "mytypes.h"

#include "arc4.h"
#include "shs.h"
#include "expoly.h"
#include "LNMatrix.h"

#include "mytypes.h"

#include <string>

#define LCA_DEFAULT_ID      0
#define LCA_DREGZ_ID        1
#define LCA_QREGZ_ID        2
#define LCA_JREGZ_ID        3

typedef std::vector<WORD_8> BYTEVECTOR;
typedef std::vector<bool>   BOOLVECTOR;

typedef std::vector<WORD32> WORD32VECTOR;

class CTrapException : public std::exception {
protected:
	char *errstr;

public:
	CTrapException(const char *s)
	{
		errstr = new char[strlen(s)+1];
		strcpy(errstr,s);
	}

	virtual ~CTrapException() throw()
	{
		delete[] errstr;
	}

	virtual const char* what() const throw()
	{
		return errstr;
	}
};

class CMVTrap  
{
protected:
	class CSerializer {
	public:
		WORD_8 *bv;
		int size;
		int index;

	public:
		CSerializer(WORD_8 *_b, int _size)
		{
			bv = _b;
			size = _size;
			index = 0;
		}

		int Get(WORD_8 &b)
		{
			if(index >= size) return 0;
			b = bv[index++];
			return 1;
		}

		int Get(WORD32 &w)
		{
			if(index + 4 > size) return 0;

			w = ((int)bv[index+0] <<  0) + ((int)bv[index+1] <<  8) + 
				((int)bv[index+2] << 16) + ((int)bv[index+3] << 24);

			index+=4;
			return 1;
		}

		int Avail()
		{
			return size - index;
		}

		WORD_8 *GetPtr()
		{
			return bv + index;
		}

		int ReadBytes(BYTEVECTOR &out, int n)
		{
			if(n > Avail()) n = Avail();

			out.clear();

			for(int i=0;i<n;i++) {
				out.push_back(bv[index++]);
			}

			return n;
		}

		int Consume(int n)
		{
			int avail = Avail();
			if(n > avail) n = avail;

			index += n;
			return n;
		}
	};

protected:
	friend class CLicenseCode;
	static RC4_CTX lRc4;
	static bool lRc4inited;

	inline bool exor(const bool a, const bool b) const
	{
		return (a & (!b)) || (b & (!a));
	}

	int SerializeInPublicKey(CSerializer &b);
	int SerializeIn(CSerializer &b, LNMatrix &L);
	int SerializeIn(CSerializer &b, EXPOLY &e);
	int SerializeIn(CSerializer &b, FFPOLY &fp);
	int SerializeIn(CSerializer &b, int &i);
	int SerializeIn(CSerializer &b, WORD32 &w);
	int SerializeIn(CSerializer &b, WORD_8 &c);
	
	void SerializeOutPublicKey(BYTEVECTOR &bv);
	void SerializeOut(BYTEVECTOR &bv, void *buff, size_t size);
	void SerializeOut(BYTEVECTOR &bv, LNMatrix &L);
	void SerializeOut(BYTEVECTOR &bv, EXPOLY &e);
	void SerializeOut(BYTEVECTOR &bv, int n);
	void SerializeOut(BYTEVECTOR &bv, WORD32 n);
	void SerializeOut(BYTEVECTOR &bv, FFPOLY &fp);
	
	int PublicKeyDump(void);
	int DerivePublicKey();
	int PubKeySetQuadTerm(int eq, int i, int j, bool bit);
	int PubKeySetKnownTerm(int eq, bool bit);
	int PubKeyReset(int eqs, int vars);
	static int map_xyp2id(int x,int y, int a);

	virtual int GetEngineID() = 0;

	WORD32VECTOR pubkey;
	int eqs,vars;
	int psize;

public:
	virtual int GetDefaultParityBitsNo() const = 0;
	virtual int GetDefaultFeatureBitsNo() const = 0;
	virtual int GetDefaultRedundancyBitsNo() const = 0;
	virtual int GetExpectedForwardInputBitsNo() const = 0;

	static WORD32 GetRandomWord32(void);
	static bool GetRandomBit(void) { return (GetRandomWord32() & 0x01) != 0; }
	static void SeedPrng(const void *_buff, int _buffsize);
	static void DumpVector(const BOOLVECTOR &b);

	int PublicForward(const BOOLVECTOR &in, BOOLVECTOR &out);
	
	virtual int PrivateForward(const BOOLVECTOR &in, BOOLVECTOR &out)=0;
	virtual int PrivateBackward(const BOOLVECTOR &in, BOOLVECTOR &out)=0;
	virtual int GenerateKeyPair(void)=0;
	virtual int LoadFromBuffer(const BYTEVECTOR &bv)=0;
	virtual int SaveToBuffer(BYTEVECTOR &bv)=0;
	virtual int SetSecurityLevel(int level)=0;
	virtual std::string GetAlgorithmName() const =0;

	const WORD32VECTOR &GetPublicKeyVector() { return pubkey; }

	CMVTrap();
	virtual ~CMVTrap();

	virtual bool SetSize(int size)=0;
	virtual int GetStatus()=0;

};

#endif // !defined(AFX_MVTRAP_H__DCF8BD01_7331_4F63_9162_5658C34F33A3__INCLUDED_)
