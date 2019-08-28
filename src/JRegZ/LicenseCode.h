/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// LicenseCode.h: interface for the CLicenseCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LICENSECODE_H__08C71D56_F263_408C_85D5_AB8FE6144321__INCLUDED_)
#define AFX_LICENSECODE_H__08C71D56_F263_408C_85D5_AB8FE6144321__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "MVTrap.h"

#define LC_BITS_PER_SYMBOL  5

class CLicenseCode  
{
protected:
	int _loadkeypair(const BYTEVECTOR &bv, const char *password);
	CMVTrap *engine;
	WORD32 paritysize;
	WORD32 payloadsize;
	WORD32 redundancy_bits;

	void init_params();

	static const char *scfilename;
	static const char *header;

	static int decode_license_string(const std::string &licstring, BOOLVECTOR &out);
	static int encode_license_string(const BOOLVECTOR &in, std::string &licstring);
	static void stringhyphen(const std::string &in, std::string &out, const std::string pattern);
	static void license_string_addseps(const std::string in, std::string &out);
	static bool BitStringToBoolVector(const std::string s, BOOLVECTOR &v);

public:
	static int AutoTest(void);
	static CMVTrap * CreateObject(int engine_id=LCA_DEFAULT_ID);

	int SetEncodingLength(int _codesize, int _featurebits, int _redundancy_bits);
	
	int GetRedundancyBitsNo(void) { return (int)redundancy_bits; }
	int GetFeatureBitsNo(void) { return (int)(payloadsize - redundancy_bits); }
	int GetParityBitsNo(void) { return (int)(paritysize); }
	
	int GetFullStringCodeSize(void) { return (int)(paritysize + payloadsize); }
		
	int GenerateKeyPair(void);
	int GenerateLicense(const BOOLVECTOR &featurebits, std::string &licstring);
	int GenerateLicense(std::string s, std::string &licstring);
	int VerifyLicense(const std::string &licstring, BOOLVECTOR &featurebits);
	int WriteCStructure(const char *path);
	int LoadKeyPair(const char *filename, const char *password);
	int SaveKeyPair(const char *filename, const char *password, bool overwrite=false);
	
	CLicenseCode(CMVTrap *_engine);
	CLicenseCode();
	virtual ~CLicenseCode();

	int export_examples;

};

#endif // !defined(AFX_LICENSECODE_H__08C71D56_F263_408C_85D5_AB8FE6144321__INCLUDED_)
