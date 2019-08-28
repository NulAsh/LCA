/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef LICENSE_CODE_VERIFIER_STUB
#define LICENSE_CODE_VERIFIER_STUB

#include "mytypes.h"
#include "quartzver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ID_PUBKEY  0x01
#define ID_PRIVKEY 0x00

#define ID_DATABLOCK       0x02
#define ID_CRYPTOMATERIAL  0x03

#define VERSION_KEY        0x0102

#define MAXPASSSIZE  60

#define MAXPRIVKEYSIZE  (1024*1024)
#define MAXPUBKEYSIZE   (1024*1024)

extern const char *headerkey;


typedef struct {
	char headstr[8];
	WORD_8 keytype;
	WORD16 version;
	WORD_8 encrypted;
	WORD32 deg;
	WORD32 pow;
	} HEADERKEY;

int ReadPublicKeyFromDisk(const char *filename,QUARTZ_PUBLICKEY *qpub);
int VerfityLicenseCode(int ac,char **av);

#ifdef __cplusplus
	}
#endif

#endif
