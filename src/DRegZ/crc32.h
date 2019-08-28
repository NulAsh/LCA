
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef CRC_32_H_43534234324324324232
#define CRC_32_H_43534234324324324232

#ifdef __cplusplus
extern "C" {
#endif

unsigned long crc(unsigned char *buf, int len);
unsigned long update_crc(unsigned long crc, unsigned char *buf,int len);

#define CRC32_MASK 0xffffffffL

#ifdef __cplusplus
	}
#endif



#endif
