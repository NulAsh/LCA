/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef LICENSE_DECODER_H
#define LICENSE_DECODER_H

#include "mytypes.h"

#define MIXROUNDS  12

#ifdef __cplusplus
extern "C" {
#endif

extern const WORD_8 SB1[32];
extern const WORD_8 SB2[32];
extern const WORD_8 SB3[32];
extern const WORD_8 SB4[32];
extern const WORD_8 SB5[32];

WORD_8 kg_coremix(WORD_8 a,WORD_8 b,WORD_8 c,WORD_8 d,WORD_8 e);
int TestSingleCode(QUARTZ_PUBLICKEY *qpub,WORD32 *id,const char *str);
int UndecorateQuartzCode(const char *deccode,char *undeccode);

#ifdef __cplusplus
	}
#endif



#endif
