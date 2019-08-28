/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef QUARTZ_DRIVER_H
#define QUARTZ_DRIVER_H

#include "mytypes.h"

#include "expoly.h"
#include "ffpoly.h"

#include "arc4.h"
#include "quartzver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define POLY129   7
#define POLY257   8
#define POLY513   9

#define MATRIX_PACKING

#define MAX_I_ELEMENTS   8
#define MAX_J_ELEMENTS   4

#define DELTA_SECRET_LEN       10

#define PRIVKEY_BODYSIGNATURE    0x45424751

#define LINESMEM       ((QUARTZ_AFFINE_TRANSSIZE + 7)/8)

#define PRIVKEY_EXPECTED_VERSION   0x00010003

#define D_MINPOLYFIGURE  7
#define D_MAXPOLYFIGURE  9

typedef struct beta_struct {
	FFPOLY epsilon[D_MAXPOLYFIGURE+1][4];
	FFPOLY ni[D_MAXPOLYFIGURE+1];
	} BETASTRUCT;

typedef struct gamma_struct {
	FFPOLY eta[4][4];
	FFPOLY rho[4];
	FFPOLY tau;
	} GAMMASTRUCT;

typedef struct delta_struct {
	WORD_8 secret[DELTA_SECRET_LEN];
	} DELTASTRUCT;

typedef struct alpha_struct {
	FFPOLY alpha[D_MAXPOLYFIGURE+1][D_MAXPOLYFIGURE+1];
	} ALPHASTRUCT;

typedef struct fv_components_struct {
	ALPHASTRUCT a;
	BETASTRUCT  b;
	GAMMASTRUCT g;
	int deg,pow;
	} FV_COMPONENTS_STRUCT;

typedef struct fv_struct {
	EXPOLY vinegar[16];
	int deg,pow;
	int done;
	} FV_STRUCT;

typedef struct quartz_privkey
	{
	FV_STRUCT fv;
	DELTASTRUCT delta;
	WORD_8 **s;
	WORD_8 **t;
	WORD_8 **sinv;
	WORD_8 **tinv;
	WORD_8 *sk;
    WORD_8 *tk;
	WORD32 psinv[QUARTZ_AFFINE_TRANSSIZE*4];
	WORD32 ptinv[QUARTZ_AFFINE_TRANSSIZE*4];
	WORD32 psk[4];
	WORD32 ptk[4];
	int isOk;
	} QUARTZ_PRIVKEY;

#define MAX_PRIVKEYLEN  (170546)

extern const int quartz_irr_ppoly[3];
/* *********************************** Functions ************************************ */

// void build_fv_function(RC4_CTX *rc4,FV_STRUCT *fv);

int vect2int(WORD_8 *v,int len,int *n);
void xorbitvect(WORD_8 *H,const char *bitvector,int inside);

int fv_test(RC4_CTX *rc4);
int fv_key_test(RC4_CTX *rc4);

int build_randpoly(RC4_CTX *rc4,FFPOLY *p,int deg,int leadcoeff);
int init_fv_struct(FV_STRUCT *fv);
void free_fv_struct(FV_STRUCT *fv);
int build_fv_functions(RC4_CTX *rc4,FV_STRUCT *fv);
int privkey_backward(QUARTZ_PRIVKEY *qpk,WORD_8 *out,WORD_8 *in);
int privkey_forward(QUARTZ_PRIVKEY *qpk,WORD_8 *out,WORD_8 *in);


/* Functions interface */

int init_priv_key(QUARTZ_PRIVKEY *qpk,int pow);
int free_priv_key(QUARTZ_PRIVKEY *qpk);

int create_priv_key(RC4_CTX *rc4,QUARTZ_PRIVKEY *qpk);
int create_public_from_private(QUARTZ_PRIVKEY *qpriv,QUARTZ_PUBLICKEY *qpub);

int buffer2privkey(WORD_8 *buffer,int buffsize,QUARTZ_PRIVKEY *qpk);
int privkey2buffer(QUARTZ_PRIVKEY *qpk,WORD_8 *buffer,int *maxlen);
int pubkey2buffer(QUARTZ_PUBLICKEY *qpub,WORD_8 *buffer,int *maxlen);

int QuartzSignMessage(const WORD_8 *msg,int msgsize,QUARTZ_PRIVKEY *qpk,WORD_8 *signature);
int QuartzSignVariableCode(const char *bitvect,const int inside,QUARTZ_PRIVKEY *qpk,char *signature);

int InitQuartzEngine(void);
void DeInitQuartzEngine(void);

#ifdef __cplusplus
	}
#endif

#endif
