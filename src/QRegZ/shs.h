
#ifndef SHS_H
#define SHS_H

#include "mytypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- SHS.H ------------------------------- */

/* NIST proposed Secure Hash Standard.

   Written 2 September 1992, Peter C. Gutmann.
   This implementation placed in the public domain.

   Comments to pgut1@cs.aukuni.ac.nz */

/* Useful defines/typedefs */

/* The SHS block size and message digest sizes, in bytes */

#define SHS_BLOCKSIZE   64
#define SHS_DIGESTSIZE  20

/* The structure for storing SHS info */

typedef struct {
	       WORD32 digest[ 5 ];            /* Message digest */
	       WORD32 countLo, countHi;       /* 64-bit bit count */
	       WORD32 data[ 16 ];             /* SHS data buffer */
	       } SHS_INFO;

/* Whether the machine is little-endian or not */
#ifndef BIG_ENDIAN
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif
#endif

void shsInit( SHS_INFO *shsInfo );
void shsUpdate( SHS_INFO *shsInfo, WORD_8 *buffer, int count );
void shsFinal( SHS_INFO *shsInfo );

#ifdef __cplusplus
	}
#endif


#endif
