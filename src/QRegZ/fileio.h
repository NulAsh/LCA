
#ifndef FILE_INPUT_OUTPUT_H
#define FILE_INPUT_OUTPUT_H

#include "mytypes.h"
#include "shs.h"
#include "quartzsig.h"
#include "verystub.h"
#include "osdepn.h"

#ifdef __cplusplus
extern "C" {
#endif

int ReSeed_CSPRNG(int ac,char **av);
int CreateQuartzKey(int ac,char **av);
int ChangePassword(int ac,char **av);
int DerivePublicFromPrivate(int ac,char **av);

int ReadPrivateKeyFromDisk(const char *filename,QUARTZ_PRIVKEY *qpk);
int ProbePrivateKeyHeaderFromDisk(const char *filename,HEADERKEY *header);

typedef struct {
	WORD_8 chunk;
	WORD_8 digest[SHS_BLOCKSIZE];
	} ENCRYPTEDBLOCK;

#ifdef __cplusplus
}
#endif

#endif
