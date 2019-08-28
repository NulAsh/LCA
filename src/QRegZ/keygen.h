/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef KEY_GENERATOR_H
#define KEY_GENERATOR_H

#include "mytypes.h"
#include "arc4.h"
#include "decoder.h"
#include "osdepn.h"

#ifdef __cplusplus
extern "C" {
#endif

int hooktest();

int CreateLicenseCode(int ac,char **av);
int CreateManyLicenseCodes(int ac,char **av);

#ifdef __cplusplus
}
#endif

#endif
