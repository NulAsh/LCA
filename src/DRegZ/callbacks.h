
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef CALLBACKS_INCLUDE_FILE_H_4532523_H
#define CALLBACKS_INCLUDE_FILE_H_4532523_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	EXPANDING_KEY = 1,
	JOLTING_KEY   = 2,	
	} PASSTYPE;


typedef struct _gauge
	{
	char caption[200];
	PASSTYPE pass;
	int gauge;
	int t_gauge;
	} GAUGE;

#ifdef _WIN32
#define DREGZ_CALLTYPE _cdecl
#else
#define DREGZ_CALLTYPE
#endif

typedef int (DREGZ_CALLTYPE *FN_PASSWORD_CALLBACK)(char *password,int maxsize);
typedef int (DREGZ_CALLTYPE *FN_GAUGE_CALLBACK)(GAUGE *gauge);

#ifdef __cplusplus
}
#endif


#endif
