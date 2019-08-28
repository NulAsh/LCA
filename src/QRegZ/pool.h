/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#ifndef RANDOM_POOL_MANAGER_H
#define RANDOM_POOL_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

int InitPool(const char *filename,unsigned char *_pool,int _poolsize);
void UpdatePool(const unsigned char *_pool,int _poolsize);
void GetPoolRnd(unsigned char *block,int size);
int DeInitPool(const char *filename);

#ifdef __cplusplus
	}
#endif

#endif
