/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef OS_DEPENDENT_H
#define OS_DEPENDENT_H

#include "mytypes.h"

#ifdef __cplusplus
extern "C" {
#endif

WORD32 GetChronoTime();
int GetRandomData(void *data,int size);

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <conio.h>

#else

#include <strings.h>
#include <unistd.h>

#define _MAX_PATH   260
#define _unlink unlink
#define _getch getch
#define _vsnprintf vsnprintf
#define _stricmp strcasecmp

int GetLastError();

#endif



#ifdef __cplusplus
	}
#endif

#endif
