
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// LicenseCode.cpp: implementation of the CLicenseCode class.
//
//////////////////////////////////////////////////////////////////////

#include "LicenseCode.h"
#include "DRegZ.h"
#include "JRegZ.h"
#include "LNPoly.h"
#include "StringGen.h"
#include "Globals.h"
#include "osdepn.h"

#include "shs.h"

const char *CLicenseCode::scfilename = "lcvec";
const char *CLicenseCode::header = "LCA+GBE32241";

#define SUPPORTED_VERSION  0x00010001

#define NO_ENCRYPTION                0
#define SIMPLE_ARCFOUR_ENCRYPTION    1

//////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////

// Object factory
CMVTrap * CLicenseCode::CreateObject(int engine_id)
{
	// encoders factory
	switch(engine_id) {
	case LCA_JREGZ_ID:
	case LCA_DEFAULT_ID:
		return new CJRegZ();

	case LCA_DREGZ_ID:
		return new CDRegZ();

	default:
		return NULL;
	}
}

bool CLicenseCode::BitStringToBoolVector(const std::string s, BOOLVECTOR &v)
{
	v.clear();

	for(size_t i=0;i<s.length();i++) {
		if(s[i] == '1') v.push_back(true);
		else if(s[i] == '0') v.push_back(false);
		else return false;
	}

	return true;
}

int CLicenseCode::decode_license_string(const std::string &licstring, BOOLVECTOR &out)
{
	out.clear();

	for(size_t i=0;i<licstring.length();i++) {
		char c = licstring[i];
		if(c == '-') continue;

		int v = CStringGen::Char2Val(c);
		
		for(int j=0;j<LC_BITS_PER_SYMBOL;j++) {
			out.push_back( (v&0x01)!=0 );
			v >>= 1;
		}
	}

	return 1;
}

int CLicenseCode::encode_license_string(const BOOLVECTOR &in, std::string &licstring)
{
	if(in.size() % LC_BITS_PER_SYMBOL != 0) return -1;
	size_t i,j;

	for(i=0;i<in.size();i+=LC_BITS_PER_SYMBOL) {
		WORD_8 symb=0;

		for(j=0;j<LC_BITS_PER_SYMBOL;j++) {
			if(in[i+j]) symb |= (1 << j);
		}
	
		char c = CStringGen::Val2Char(symb);
		licstring += c;
	}

	return 1;
}

void CLicenseCode::stringhyphen(const std::string &in, std::string &out, const std::string pattern)
{
	size_t index = 0;
	int sum=0;

	out = "";

	for(index=0;index<pattern.length();index++) {
		int val = pattern[index] - '0';
		if(val <= 0 || val >= 6) {
			out = in;
			return;
		}

		int count = 0;

		while( count < val ) {
			if(count+sum >= (int)in.size()) {
				out = in;
				return;
			}

			out += in[count+sum];
			count++;
		}

		out += "-";
		sum += val;
	}

	out = out.substr(0,out.length()-1);
}

void CLicenseCode::license_string_addseps(const std::string in, std::string &out)
{
	size_t len = in.length();
	switch(len) {
	case 14:
		stringhyphen(in,out,"4334");
		break;

	case 15:
		stringhyphen(in,out,"555");
		break;

	case 16:
		stringhyphen(in,out,"4444");
		break;

	case 17:
		stringhyphen(in,out,"34343");
		break;

	case 18:
		stringhyphen(in,out,"4554");
		break;

	case 19:
		stringhyphen(in,out,"44344");
		break;

	case 20:
		stringhyphen(in,out,"44444");
		break;

	case 21:
		stringhyphen(in,out,"44544");
		break;

	case 22:
		stringhyphen(in,out,"45454");
		break;

	case 23:
		stringhyphen(in,out,"45554");
		break;

	case 24:
		stringhyphen(in,out,"55455");
		break;

	case 25:
		stringhyphen(in,out,"55555");
		break;

	default:
		out = in;
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLicenseCode::CLicenseCode()
{
	engine = CreateObject();
	if(!engine) throw CTrapException("invalid engine");

	init_params();
}

CLicenseCode::CLicenseCode(CMVTrap *_engine)
{
	if(!_engine) throw CTrapException("invalid engine");
	engine = _engine;

	init_params();
}

void CLicenseCode::init_params()
{
	export_examples = 5;
	
	int feature_bits_no = engine->GetDefaultFeatureBitsNo();
	
	redundancy_bits = engine->GetDefaultRedundancyBitsNo();
	paritysize      = engine->GetDefaultParityBitsNo();
	payloadsize     = (WORD32)(feature_bits_no + redundancy_bits);
}

CLicenseCode::~CLicenseCode()
{
	delete engine;
}

int CLicenseCode::SetEncodingLength(int _codesize, int _featurebits, int _redundancy_bits)
{
	if(_redundancy_bits < 0) {
		_redundancy_bits = engine->GetDefaultRedundancyBitsNo();
	}

	if(_featurebits < 0) {
		_featurebits = engine->GetDefaultFeatureBitsNo();
	}

	int _payloadsize = _featurebits + _redundancy_bits;

	if(_codesize < _payloadsize || _payloadsize < 7 || _redundancy_bits > 16) 
		return 0;

	int rr = engine->SetSize(_codesize);
	if(rr != 1) return 0;

	paritysize      = (WORD32)(_codesize - _payloadsize);
	payloadsize     = (WORD32)(_payloadsize);
	redundancy_bits = (WORD32)(_redundancy_bits);

	return 1;
}

int CLicenseCode::GenerateKeyPair()
{
	return engine->GenerateKeyPair();
}

int CLicenseCode::GenerateLicense(std::string s, std::string &licstring)
{
	BOOLVECTOR v;
	if(!BitStringToBoolVector(s,v)) return -101;	
	return GenerateLicense(v,licstring);
}

int CLicenseCode::GenerateLicense(const BOOLVECTOR &featurebits, std::string &licstring)
{
	if(payloadsize != (WORD32)featurebits.size() + redundancy_bits) return -1;

	ELEMENT signature[200];
	ELEMENT checksum[200];

	memset(signature,0x00,sizeof(signature));

	WORD32 redundancy_maxloops = (1 << redundancy_bits);
	WORD32 redundancy_counter = 0;

	BOOLVECTOR out;
	WORD32 i;
	int rr;

	while(redundancy_counter < redundancy_maxloops) {
		out.clear();

		int cursor = 0;

		// prefix feature bits
		for(i=0;i<featurebits.size();i++) {
			signature[cursor++] = featurebits[i];
		}

		// append redundancy
		for(i=0;i<redundancy_bits;i++) {
			signature[cursor++] = ((redundancy_counter >> i) & 0x01) ? 1 : 0;
		}

		LNPoly::ComputeParity(signature,payloadsize,checksum,paritysize);
		BOOLVECTOR in;

		for(i=0;i<payloadsize;i++) {
			in.push_back( signature[i] != 0 );
		}

		for(i=0;i<paritysize;i++) {
			in.push_back( checksum[i] != 0 );
		}

		rr = engine->PrivateBackward(in,out);
		if(rr == 1) break;

		redundancy_counter++;
	}

	if(redundancy_counter == redundancy_maxloops) {
		// we failed to apply a valid signature despite redundancy
		return 0;
	}

	while(out.size() % LC_BITS_PER_SYMBOL != 0) {
		out.push_back(false);
	}

	rr = encode_license_string(out, licstring);
	if(rr < 1) return -2;

	license_string_addseps(licstring, licstring);

	BOOLVECTOR exp_features;

	rr = VerifyLicense(licstring,exp_features);
	if(rr != 1) return -3;

	if(exp_features != featurebits) return -4;
	return 1;
}

int CLicenseCode::VerifyLicense(const std::string &licstring, BOOLVECTOR &featurebits)
{
	BOOLVECTOR pcode,out;

	ELEMENT pl[200],pr[200];

	int rr = decode_license_string(licstring, pcode);
	if(rr < 1) return -1-rr;

	int expinputsize = engine->GetExpectedForwardInputBitsNo();
	int recinputsize = (int)pcode.size();

	int guess_bits = expinputsize - (int)pcode.size();
	int max_guesses = 1 << guess_bits;
	int guess_id = 0;
	int j;

	for(j=0;j<guess_bits;j++) {
		pcode.push_back(false);
	}

	while(guess_id < max_guesses) {
		for(j=0;j<guess_bits;j++) {
			pcode[recinputsize + j] = (((guess_id >> j) & 0x01) != 0);	
		}

		rr = engine->PrivateForward(pcode,out);
		if(rr < 1) return -10-rr;

		if((WORD32)out.size() != (WORD32)(payloadsize + paritysize)) return -100;
		WORD32 i;

		for(i=0;i<payloadsize;i++) {
			pl[i] = out[i];
		}

		for(i=0;i<paritysize;i++) {
			pr[i] = out[i+payloadsize];
		}

		rr = LNPoly::VerifyParity(pl,payloadsize,pr,paritysize);
		if(rr == 1) break;

		guess_id++;
	}

	if(guess_id == max_guesses) return 0;

	featurebits.clear();

	int featurebitsno = (int)(payloadsize-redundancy_bits);

	for(j=0;j<featurebitsno;j++) {
		featurebits.push_back(out[j]);
	}

	return 1;
}

int CLicenseCode::WriteCStructure(const char *path)
{
	STRING out;
	FILE *h1;
	
	out = JoinPaths(path,scfilename) + ".h";
		
	h1 = fopen(out.c_str(),"w");
	if(!h1) return -1;

	unsigned int rc1 = CMVTrap::GetRandomWord32();
	unsigned int rc2 = CMVTrap::GetRandomWord32();

	const WORD32VECTOR pubkey = engine->GetPublicKeyVector();

	int mbsize = (int)pubkey.size();

	int polycoefs[16];
	int polysize = LNPoly::GetIrredPoly(paritysize, polycoefs,16);

	fprintf(h1,"\n#ifndef LICENSE_CODE_%08X%08X_H\n#define LICENSE_CODE_%08X%08X_H\n\n",
		rc1,rc2,rc1,rc2);

	fprintf(h1,"#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");

	int eblocks = (paritysize + payloadsize + 31) / 32;

	fprintf(h1,"// Created by %s from LCA suite\n",engine->GetAlgorithmName().c_str());
	fprintf(h1,"// see: http://www.webalice.it/giuliano.bertoletti/lca.html\n\n");
	
	int inputsize = engine->GetExpectedForwardInputBitsNo();	

	// round it off, remaining bits are guessed
	int licchars = engine->GetExpectedForwardInputBitsNo() / LC_BITS_PER_SYMBOL;

	fprintf(h1,"#define LICENSE_CHARS    %d\n",licchars);
	fprintf(h1,"#define KEYIDSIZE        %d\n",payloadsize);
	fprintf(h1,"#define G_KEYSIZE        %d\n",inputsize);
	fprintf(h1,"#define G_RESULTSIZE     %d\n",paritysize + payloadsize);
	fprintf(h1,"#define POTSIZE          %d\n",eblocks);
	fprintf(h1,"#define POLYSIZE         %d\n",polysize);
	fprintf(h1,"#define REDUNDANCY_BITS  %d\n",redundancy_bits);
	fprintf(h1,"\n");
	fprintf(h1,"#define LICENSE_VECTOR   %d\n",mbsize);
	fprintf(h1,"\n");
	fprintf(h1,"extern const unsigned int lsvec[LICENSE_VECTOR];\n");
	fprintf(h1,"extern const int ipol[POLYSIZE];\n");
	fprintf(h1,"\n");
	fprintf(h1,"#ifdef __cplusplus\n}\n#endif\n");
	fprintf(h1,"\n");
	fprintf(h1,"#endif    /* LICENSE_CODE_%08X%08X_H */\n\n",rc1,rc2);

	fclose(h1);

	out = JoinPaths(path,scfilename) + ".c";

	h1 = fopen(out.c_str(),"w");
	if(!h1) return -1;

	fprintf(h1,"\n#include \"%s.h\"\n\n",scfilename);

	fprintf(h1,"// Created by %s from LCA suite\n",engine->GetAlgorithmName().c_str());
	fprintf(h1,"// see: http://www.webalice.it/giuliano.bertoletti/lca.html\n\n");

	if(export_examples > 0) {
		int count=0;

		fprintf(h1,"/*  Example codes:\n\n");

		while(count < export_examples) {
			BOOLVECTOR pl;
			std::string outs;

			for(int i=0;i<GetFeatureBitsNo();i++) {
				pl.push_back( CMVTrap::GetRandomBit() );
			}
			
			int rr = GenerateLicense(pl,outs);
			if(rr <= 0) {
				fprintf(h1,"    %2d: <no license found>\n",count);
			} else {
				fprintf(h1,"    %2d: %s\n",count,outs.c_str());
			}

			count++;
		}
		fprintf(h1,"\n*/\n\n");
	}

	fprintf(h1,"const int ipol[POLYSIZE] = { ");
	
	int i;
	for(i=0;i<polysize;i++)
		{
		fprintf(h1,"%d",polycoefs[i]);
		if(i < polysize - 1) fprintf(h1,",");
		}
	fprintf(h1," };\n\n");

	fprintf(h1,"const unsigned int lsvec[LICENSE_VECTOR] = {\n");

	const int each = 8;
	
	for(i=0;i<mbsize;i++)
		{
		int j = i % 8;

		if(j == 0) fprintf(h1,"  ");

		fprintf(h1,"0x%08x",pubkey[i]);

		if(i < mbsize - 1) fprintf(h1,",");
		else fprintf(h1,"\n");
	
		if(j == (each-1) && i != (mbsize-1)) fprintf(h1,"\n");
		}

	fprintf(h1,"  };\n\n");

	fclose(h1);
	return 1;
}

int CLicenseCode::_loadkeypair(const BYTEVECTOR &bv, const char *password)
{
	SHS_INFO checksum;

	shsInit(&checksum);
	shsUpdate(&checksum,(WORD_8 *)&bv[0],(int)bv.size() - SHS_DIGESTSIZE);
	shsFinal(&checksum);

	if(memcmp(&checksum.digest,&bv[0] + bv.size() - SHS_DIGESTSIZE, SHS_DIGESTSIZE) != 0) {
		return -5;	// failed integrity check
	}
	
	int cursor = 0;

	if(memcmp(&bv[0],(WORD_8 *)header,strlen(header))!=0) {
		return -6;	// file format not recognized
	}

	cursor += (int)strlen(header);

	WORD32 cversion;
	memcpy(&cversion,&bv[cursor],4);
	cursor += 4;

	if(cversion > SUPPORTED_VERSION) {
		return -7;	// unsupported version
	}

	unsigned char encryption_type;
	WORD32 tsize;

	memcpy(&tsize,&bv[cursor],4);
	cursor += 4;

	encryption_type = bv[cursor++];
	
	BYTEVECTOR tbv;
	WORD32 i;

	if(encryption_type == NO_ENCRYPTION) {
		for(i=0;i<tsize;i++) {
			if(cursor >= (int)bv.size()) return -12;
			tbv.push_back(bv[cursor++]);
		}
	} else if(encryption_type == SIMPLE_ARCFOUR_ENCRYPTION) {
		SHS_INFO key,check;

		shsInit(&key);
		shsUpdate(&key,(WORD_8 *)password,(int)strlen(password));
		shsFinal(&key);

		shsInit(&check);
		shsUpdate(&check,(unsigned char *)key.digest,SHS_DIGESTSIZE);	
		shsFinal(&check);		

		if(memcmp(&check.digest,&bv[cursor],SHS_DIGESTSIZE) != 0) {
			init_params();
			return -10;	// wrong password !
		}

		cursor += SHS_DIGESTSIZE;

		RC4_CTX rc4;
		InitRC4(&rc4,(const char *)key.digest,SHS_DIGESTSIZE);

		for(i=0;i<256;i++) {
			RC4_GetByte(&rc4);
		}
		
		for(i=0;i<tsize;i++) {
			if(cursor >= (int)bv.size()) return -12;
			tbv.push_back( bv[cursor++] ^ RC4_GetByte(&rc4) );
		}
		
	} else {
		return -9;
	}

	// ok, now extract data from the inner layer
	cursor = 0;

	memcpy(&export_examples,&tbv[cursor],4);
	cursor += 4;

	WORD32 c_paritysize, c_payloadsize;
	WORD32 c_redundancy_bits, c_engine_id;

	memcpy(&c_paritysize,&tbv[cursor],4);
	cursor += 4;

	memcpy(&c_payloadsize,&tbv[cursor],4);
	cursor += 4;

	memcpy(&c_redundancy_bits,&tbv[cursor],4);
	cursor += 4;

	memcpy(&c_engine_id,&tbv[cursor],4);
	cursor += 4;

	delete engine;
	engine = CreateObject((int)c_engine_id);
	if(!engine) {
		return -8;	// invalid id!
	}

	paritysize = c_paritysize;
	payloadsize = c_payloadsize;
	redundancy_bits = c_redundancy_bits;

	tbv.erase(tbv.begin(), tbv.begin() + cursor);
	return engine->LoadFromBuffer(tbv);
}

int CLicenseCode::LoadKeyPair(const char *filename, const char *password)
{
	BYTEVECTOR bv;

	FILE *h1 = fopen(filename,"rb");
	if(!h1) return -1;

	fseek(h1,0,SEEK_END);
	long fsize = ftell(h1);
	fseek(h1,0,SEEK_SET);

	try {
		bv.resize(fsize);
	} catch(...) {
		fclose(h1);
		return -2;	// memory allocation error!
	}
	
	if(fread(&bv[0],1,fsize,h1) != (size_t)fsize) {
		fclose(h1);	// probably I/O error!
		return -3;
	}

	fclose(h1);

	if(fsize < 100) {
		return -4;	// data to short
	}

	int rr = _loadkeypair(bv,password);
	if(rr != 1) {
		engine = CreateObject();
		init_params();
	}

	return rr;
}

int CLicenseCode::SaveKeyPair(const char *filename, const char *password, bool overwrite)
{
	if(!engine) return -1;
	if(engine->GetStatus() < 1) return -2;

	if(!overwrite) {
		FILE *h1 = fopen(filename,"rb");
		if(h1) {
			fclose(h1);	// file exists !
			return -12;
		}
	}

	BYTEVECTOR bv;
	WORD32 cversion = SUPPORTED_VERSION;

	engine->SerializeOut(bv,(WORD_8 *)header,strlen(header));
	engine->SerializeOut(bv,cversion);
	
	// ************* possible encrypted payload *************
	BYTEVECTOR tbv;

	engine->SerializeOut(tbv,export_examples);
	engine->SerializeOut(tbv,paritysize);
	engine->SerializeOut(tbv,payloadsize);
	engine->SerializeOut(tbv,redundancy_bits);

	int engine_id = engine->GetEngineID();
	engine->SerializeOut(tbv,engine_id);

	// call engine serialization
	int rr = engine->SaveToBuffer(tbv);
	if(rr != 1) return -3;

	engine->SerializeOut(bv,(WORD32)tbv.size());

	// needs encryption ?
	if(password == NULL) {
		bv.push_back( NO_ENCRYPTION );
	} else {
		bv.push_back( SIMPLE_ARCFOUR_ENCRYPTION );

		SHS_INFO key,check;

		shsInit(&key);
		shsUpdate(&key,(WORD_8 *)password,(int)strlen(password));
		shsFinal(&key);

		// this would cause problems on big endian machines !
		shsInit(&check);
		shsUpdate(&check,(unsigned char *)key.digest,SHS_DIGESTSIZE);	
		shsFinal(&check);

		bv.insert(bv.end(),(unsigned char *)check.digest,(unsigned char *)check.digest+SHS_DIGESTSIZE);

		RC4_CTX rc4;
		size_t i;

		InitRC4(&rc4,(char *)key.digest,SHS_DIGESTSIZE);

		for(i=0;i<256;i++) {	
			// discard first 256 bytes.
			RC4_GetByte(&rc4);
		}

		for(i=0;i<tbv.size();i++) {
			// perform payload encryption
			tbv[i] ^= RC4_GetByte(&rc4);
		}
	}

	// append payload to main buffer
	bv.insert(bv.end(),tbv.begin(),tbv.end());

	int err = 0;

	SHS_INFO checksum;

	shsInit(&checksum);
	shsUpdate(&checksum,(WORD_8 *)&bv[0],(int)bv.size());
	shsFinal(&checksum);

	// append SHA-1 of data for integrity check
	bv.insert(bv.end(),(unsigned char *)checksum.digest,(unsigned char *)checksum.digest+SHS_DIGESTSIZE);

	FILE *h1 = fopen(filename,"wb");
	if(!h1) return -1;

	if(fwrite(&bv[0],1,bv.size(),h1) != bv.size()) err = 0;

	fclose(h1);
	return !err;
}

int CLicenseCode::AutoTest()
{
	// supports both DRegZ and JRegZ.
	int avail_ids[] = { LCA_DREGZ_ID, LCA_JREGZ_ID, -1 };

	// notice that this instance of DRegZ is not compatible at the key
	// level with the full program present of this package.

	// It's also recommended you do not use DRegZ because the algorithm
	// has been broken (succesfully cryptanalyzed)

	for(int kk=0;avail_ids[kk] >= 0;kk++) {
		CMVTrap *mvt = CLicenseCode::CreateObject(avail_ids[kk]);
		CLicenseCode lc(mvt);
		int rr;

		const char *tmpfile  = "testfile.bin";
		const char *password = "12345678";

		printf("Generating keypair with %s...\n",
			mvt->GetAlgorithmName().c_str());

		rr = lc.GenerateKeyPair();
		if(rr < 1) {
			printf("?key generation failed! (code: %d)\n",rr);
			return -3;
		}

		// just testing saving and loading routines...
		printf("Saving keypair to file: %s\n",tmpfile);
		rr = lc.SaveKeyPair(tmpfile,password,true);
		if(rr != 1) {
			printf("Failed to save tempfile: %s\n",tmpfile);
			return -4;
		}

		printf("Reloading keypair file: %s\n",tmpfile);

		// use another object build from saved file
		CLicenseCode lc2;
		rr = lc2.LoadKeyPair(tmpfile,password);

		// delete the temporary file
		_unlink(tmpfile);

		if(rr != 1) {
			printf("Failed to reload tempfile: %s\n",tmpfile);
			return -4;
		}

		// expecting a path pointing to a folder where other jdecoder translation
		// units are stored.
		
		// lc2.WriteCStructure("D:\\Programmi\\Microsoft Visual Studio\\VC98\\MyProjects\\jdecoder");

		printf("Creating some codes...\n");

		int featurebits_no = lc2.GetFeatureBitsNo();

		const int licenses_to_generate = 20;
		int count = 0;

		while(count < licenses_to_generate) {
			std::string featurebits_s;
			std::string ls;
			
			for(int i=0;i<featurebits_no;i++) {
				featurebits_s += CMVTrap::GetRandomBit() ? "1" : "0";
			}

			rr = lc2.GenerateLicense(featurebits_s,ls);
			if(rr != 1) return -1;

			count++;
			printf("%3d: %s\n",count,ls.c_str());
		}

		printf("-----------------------------------------------\n");
	}

	return 1;
}



