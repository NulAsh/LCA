/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

// JRegZ.cpp: implementation of the CJRegZ class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "JRegZ.h"
#include "LNPoly.h"
#include "polyprint.h"

#include <time.h>

#define VINEGAR_POLYNO   (1 << VINEGAR_BITS)

// #define DUMPDATA

#define JREGZ_VERSION_ID        0x00010001

// JRGZ
#define JREGZ_START_SIGNATURE	0x5A47524A

// JEND
#define JREGZ_END_SIGNATURE		0x444E454A

//////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////

int CJRegZ::LoopTest(int loops, int pow, int size)
{
	static unsigned int gcount = 1;
	static unsigned int gnosol    = 0;
	int rr;

	CJRegZ _j1,_j2;
	CJRegZ *j1 = &_j1;
	CJRegZ *j2 = &_j2;

	// build the raw quadratic Fv function similar to
	// the one defined in Quartz.
	printf("Generating test key in F_[2^%d]\n",size);
	rr = j1->GenerateKeyPair();
	if(rr < 1) {
		printf("?key generation failed");
		return -1;
	}

	BYTEVECTOR hold;

	// test serialization by flushing the key to a buffer
	// and then reloading another instance from the buffer
	printf("Flushing out...\n");
	if(j1->SaveToBuffer(hold) < 1) {
		printf("?save failed\n");
		return -1;
	}

	printf("%d bytes of object state\n",hold.size());

	printf("Flushing in...\n");
	if(j2->LoadFromBuffer(hold) < 1) {
		printf("?load failed\n");
		return -1;
	}

	// the two keys are the same if serialization in/out is ok.
	int codesize =  j1->GetN();

	BOOLVECTOR b1,b2,b3,b4;
	b1.resize(codesize);

	int count = 0;
	int i;
	int nosol = 0;
	int rj;

	printf("Testing transformation...\n");

	// perform some transformation with both keys interchangeably
	for(count=1;count < loops || loops == -1;count++) {
		// pick a random B
		for(i=0;i<codesize;i++) {
			b1[i] = ((RC4_GetByte(&_j1.lRc4) & 0x01) != 0);
		}

		// now solve in Z the equation: Fv(Z)=B

		// this is expected to fail (i.e. no solution) with 
		// probability of 1/e which is roughly: 0.367879. 
		
		// Use r redundancy bit to lower the probability to a 
		// negligible factor of (1/e)^(2^r) when using it in
		// practice.

		rr = j1->PrivateBackward(b1,b2);
		if(rr < 0) {
			printf("?private backward step failed!\n");
			return -2;
		} else if(rr > 0) {
			rr = j2->PrivateForward(b2,b3);
			if(rr <= 0) {
				printf("?private forward step failed!\n");
				return -3;
			}

			// test that F( F^-1(B) ) = B

			if(b1 != b3) {
				printf("?cycle mismatch!\n");
				return -4;
			}

			// but also with the public forward transformation
			// we should get back B.

			rr = j1->PublicForward(b2,b4);
			if(rr <= 0) {
				printf("?public forward step failed!\n");
				return -3;
			}

			if(b1 != b4) {
				// PublicKeyDump();
				CMVTrap::DumpVector(b1);
				CMVTrap::DumpVector(b2);
				CMVTrap::DumpVector(b3);
				CMVTrap::DumpVector(b4);
				printf("?public forward error!\n");
				return -4;
			}
			rj = 1;
		} else {
			nosol++;
			gnosol++;
			rj = 0;
		}

		double perc = 100.0 * nosol / count;
		double gperc = 100.0 * gnosol / gcount;

		const char *status = (rj != 0 ? "ok" : "no sol.");

		// print some stats...
		printf("G: %u/%d (%7.4lf%%) | L: %d/%d (%7.4lf%%) > %s\n",
			gnosol,
			gcount,
			gperc,
			nosol,
			count,
			perc,
			status
			);

		fflush(stdout);

		std::swap(j1,j2);
		gcount++;
	}

	printf("\nOk.\n");
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJRegZ::CJRegZ()
{
	fc = new FV_COMPONENTS_STRUCT;
	
	memset(fc,0x00,sizeof(FV_COMPONENTS_STRUCT));

	SetSize(125);
	SetSecurityLevel(1);
	isok = 0;
}

CJRegZ::~CJRegZ()
{
	delete fc;
}

int CJRegZ::SetSecurityLevel(int level)
{
	//
	// polydeg = 2^(6+level) + 1
	//
	// i.e. level=1 ---> deg = 129
	//      level=2 ---> deg = 257
	//      level=3 ---> deg = 513
	//

	if(level < 1 || level > 3) return 0;

	fc->pow = level + 6;
	fc->deg = (1 << fc->pow) + 1;

	isok = 0;
	return 1;
}

// number of bits in I/O. Should be between 64 and 128.
bool CJRegZ::SetSize(int size)
{
	if(size < JREGZ_MINCODESIZE || size > JREGZ_MAXCODESIZE) {
		return false;
	}
	
	int deg = get_poly_deg(size);

	if( LNPoly::GetIrredPoly(deg,cmod) < 0 ) return false;

	codesize = size;
	isok = 0;

	return true;
}

int CJRegZ::GetStatus()
{
	return isok != 0 ? 1 : 0;
}

int CJRegZ::GenerateKeyPair()
{
	int i,j,k,l;

	isok = 0;

	mtx_s.Initialize(codesize,codesize);
	mtx_t.Initialize(codesize,codesize);

	mtx_s.CreateInvertible(lRc4);
	mtx_t.CreateInvertible(lRc4);

	mtx_s.Invert(mtx_is);
	mtx_t.Invert(mtx_it);

	/* Building alpha_i_j with 0<=(2^i + 2^j)<=129 */

	for(i=0;i<=fc->pow;i++) {
		for(j=0;j<=fc->pow;j++) {
			if(i < j && ((1<<i) + (1<<j) <= fc->deg))
				build_randpoly(&lRc4,&fc->a.alpha[i][j],cmod.pvdeg,0);
			else
				ffpoly_zeroset(&fc->a.alpha[i][j]);
		}
	}

	/* Building the beta function's epsilons */
	for(i=0;i<=fc->pow;i++)
		for(k=0;k<VINEGAR_BITS;k++)
			build_randpoly(&lRc4,&fc->b.epsilon[i][k],cmod.pvdeg,0);
		
	/* Building the beta function's nis */
	for(i=0;i<=fc->pow;i++)
		build_randpoly(&lRc4,&fc->b.ni[i],cmod.pvdeg,0);

	/* Building the gamma function's etas */
	for(k=0;k<VINEGAR_BITS;k++)
		for(l=0;l<VINEGAR_BITS;l++) {
			if(k<l) {
				build_randpoly(&lRc4,&fc->g.eta[k][l],cmod.pvdeg,0);
			} else {
				ffpoly_zeroset(&fc->g.eta[k][l]);
			}
		}
	
	/* Building the gamma function's rhos */
	for(k=0;k<VINEGAR_BITS;k++)
		build_randpoly(&lRc4,&fc->g.rho[k],cmod.pvdeg,0);

	/* Building the gamma function's tau */
	build_randpoly(&lRc4,&fc->g.tau,cmod.pvdeg,0);

	int rr = build_vinegar();
	if(rr != 1) return 0;

	isok = 1;

	PubKeyReset(codesize,codesize);
	rr = DerivePublicKey();
	if(rr != 1) {
		isok = 0;
		return 0;
	}

	return 1;
}

int CJRegZ::SaveToBuffer(BYTEVECTOR &bv)
{
	if(isok < 1) return -1;

	WORD32 signature = JREGZ_START_SIGNATURE;
	WORD32 version   = JREGZ_VERSION_ID;
	int i,j;

	SerializeOut(bv, signature);
	SerializeOut(bv, version);

	SerializeOut(bv, codesize);
	SerializeOut(bv, fc->deg);
	SerializeOut(bv, fc->pow);
	
	for(i=0;i<D_MAXPOLYFIGURE+1;i++) {
		for(j=0;j<D_MAXPOLYFIGURE+1;j++) {
			SerializeOut(bv, fc->a.alpha[i][j]);
		}
	}

	for(i=0;i<D_MAXPOLYFIGURE+1;i++) {
		for(j=0;j<VINEGAR_BITS;j++) {
			SerializeOut(bv, fc->b.epsilon[i][j]);
		}

		SerializeOut(bv,fc->b.ni[i]);
	}

	for(i=0;i<VINEGAR_BITS;i++) {
		for(j=0;j<VINEGAR_BITS;j++) {
			SerializeOut(bv,fc->g.eta[i][j]);
		}

		SerializeOut(bv,fc->g.rho[i]);
	}

	SerializeOut(bv,fc->g.tau);

	SerializeOut(bv, cmod);

	SerializeOut(bv, mtx_s);
	SerializeOut(bv, mtx_is);

	SerializeOut(bv, mtx_t);
	SerializeOut(bv, mtx_it);

	SerializeOutPublicKey(bv);

	SerializeOut(bv, JREGZ_END_SIGNATURE);
	return 1;
}

int CJRegZ::LoadFromBuffer(const BYTEVECTOR &bv)
{
	WORD32 signature = JREGZ_START_SIGNATURE;
	WORD32 version   = JREGZ_VERSION_ID;
	WORD32 cmpsig,cmpver,endsign;
	int i,j;

	CSerializer ser((unsigned char *)&bv[0],(int)bv.size());

	if(SerializeIn(ser, cmpsig) < 1) return 0;
	if(cmpsig != signature) return -1;
	
	if(SerializeIn(ser, cmpver) < 1) return 0;
	if(cmpver > version) return -2;

	if(SerializeIn(ser, codesize) < 1) return 0;
	if(SerializeIn(ser, fc->deg) < 1) return 0;
	if(SerializeIn(ser, fc->pow) < 1) return 0;
	
	for(i=0;i<D_MAXPOLYFIGURE+1;i++) {
		for(j=0;j<D_MAXPOLYFIGURE+1;j++) {
			if(SerializeIn(ser, fc->a.alpha[i][j]) < 1) return 0;
		}
	}

	for(i=0;i<D_MAXPOLYFIGURE+1;i++) {
		for(j=0;j<VINEGAR_BITS;j++) {
			if(SerializeIn(ser, fc->b.epsilon[i][j]) < 1) return 0;
		}

		if(SerializeIn(ser,fc->b.ni[i]) < 1) return 0;
	}

	for(i=0;i<VINEGAR_BITS;i++) {
		for(j=0;j<VINEGAR_BITS;j++) {
			if(SerializeIn(ser,fc->g.eta[i][j]) < 1) return 0;
		}

		if(SerializeIn(ser,fc->g.rho[i]) < 1) return 0;
	}

	if(SerializeIn(ser,fc->g.tau) < 1) return 0;

	if(SerializeIn(ser, cmod) < 1) return 0;

	if(SerializeIn(ser, mtx_s) < 1) return 0;
	if(SerializeIn(ser, mtx_is) < 1) return 0;

	if(SerializeIn(ser, mtx_t) < 1) return 0;
	if(SerializeIn(ser, mtx_it) < 1) return 0;

	if(SerializeInPublicKey(ser) < 1) return 0;

	if(SerializeIn(ser, endsign) < 1) return 0;
	
	if(endsign != JREGZ_END_SIGNATURE) return -4;

	if(build_vinegar() != 1) return -9;

	isok = 1;
	return 1;
}

int CJRegZ::PrivateForward(const BOOLVECTOR &in, BOOLVECTOR &out)
{
	if(isok < 1) return -2;
	if((int)in.size() != GetExpectedForwardInputBitsNo()) return -1;

	ELEMENT t1[256],t2[256];
	int i;

	for(i=0;i<codesize;i++) {
		t1[i] = (in[i] ? 1 : 0);
	}

	mtx_s.MulLVect(t1,codesize,t2);
	
	FFPOLY Z,B;
    ffpoly_zeroset(&Z);

    for(i=0;i<cmod.pvdeg;i++)
        if(t2[i]) ffpoly_setterm(&Z,i);

    int v = get_vinegar(t2 + cmod.pvdeg);

#ifdef DUMPDATA
	printf("--> V: %d\n",v);
	printf("--> Z:\n");
	ffpoly_dump(&Z);
#endif

	// expoly_dumpdata(&vinegar[v].e);

	/* Compute B = Fv(Z) */
    expoly_subst(&B,&vinegar[v].e,&Z,&cmod);

#ifdef DUMPDATA
	printf("--> B:\n");
	ffpoly_dump(&B);
#endif

	// vinegar remains unaffected
    for(i=0;i<cmod.pvdeg;i++)
        t2[i] = (ffpoly_getterm(&B,i) != 0 ? 1 : 0);

	mtx_t.MulLVect(t2,codesize,t1);

	out.clear();

    for(i=0;i<codesize;i++)
        out.push_back( t1[i] != 0 );

	return 1;
}

int CJRegZ::PrivateBackward(const BOOLVECTOR &in, BOOLVECTOR &out)
{
	if(isok < 1) return -2;
	if((int)in.size() != codesize) return -1;

	ELEMENT t1[256],t2[256];
	int i;

	for(i=0;i<codesize;i++) {
		t1[i] = (in[i] ? 1 : 0);
	}

	mtx_it.MulLVect(t1,codesize,t2);
	
	FFPOLY Z,B;
    ffpoly_zeroset(&B);

    for(i=0;i<cmod.pvdeg;i++) {
        if(t2[i]) ffpoly_setterm(&B,i);
	}

    int v = get_vinegar(t2 + cmod.pvdeg);

	EXPOLY fact;
	expoly_init(&fact,fc->deg + 1);

#ifdef DUMPDATA
	printf("<-- V: %d\n",v);
	printf("<-- B:\n");
	ffpoly_dump(&B);
#endif

	/* Construct and try to solve Fv(Z)-B = 0 */
	expoly_copy(&fact,&vinegar[v].e);
	ffpoly_addmod(&fact.ep[0],&fact.ep[0],&B,&cmod);

	ROOT_POT pot;

	expoly_pot_init(&pot,&cmod,fact.pvdeg);
	expoly_factor(&pot,&fact,&cmod);
	expoly_free(&fact);

	if(pot.factsNo == 0) {
		expoly_pot_free(&pot);
		return 0;	// no solutions !
	}

	choose_solution(&Z,&pot);
	expoly_pot_free(&pot);

#ifdef DUMPDATA
	printf("<-- Z:\n");
	ffpoly_dump(&Z);
#endif

	// vinegar remains unaffected
    for(i=0;i<cmod.pvdeg;i++)
        t2[i] = (ffpoly_getterm(&Z,i) != 0 ? 1 : 0);

	mtx_is.MulLVect(t2,codesize,t1);

	out.clear();

    for(i=0;i<codesize;i++)
        out.push_back( t1[i] != 0 );

	return 1;
}



/* ********************************************************************** */

int CJRegZ::get_poly_deg(int size)
{
	return size - VINEGAR_BITS;
}

int CJRegZ::build_vinegar()
{
	int i,j,v;	
	vinegar.clear();

	for(i=0;i<VINEGAR_POLYNO;i++) {
		CExPolyHold v;
		v.SetSize(fc->deg+1);
		vinegar.push_back(v);
	}

	/* The 16 vinegar polys */
	for(v=0;v<VINEGAR_POLYNO;v++) {
		FFPOLY gamma;

		expoly_zeroset(&vinegar[v].e);

		/* Setting poly alpha_i_j coefficients of X^(2^i + 2^j) */
		for(i=0;i<=fc->pow;i++)	{
			for(j=0;j<=fc->pow;j++)	{
				int deg = (1<<i) + (1<<j);
				if(i >= j || deg > fc->deg) continue;

				expoly_setterm(&vinegar[v].e,&fc->a.alpha[i][j],deg);
			}
		}

		/* Setting poly beta_i coefficients of X^(2^i) */
		for(i=0;i<=fc->pow;i++) {
			FFPOLY beta;
			int deg = (1 << i);
			
			if(compute_beta_coefficient(&beta,fc,v,i,&cmod) != 1) return 0;
			ffpoly_addmod(&vinegar[v].e.ep[deg],&vinegar[v].e.ep[deg],&beta,&cmod);
		}

		/* Setting poly gamma coefficient of X^0 */
		if( compute_gamma_coefficient(&gamma,fc,v,&cmod) != 1 ) 
			return 0;
		ffpoly_addmod(&vinegar[v].e.ep[0],&vinegar[v].e.ep[0],&gamma,&cmod);
		
		ffpoly_identity_set(&vinegar[v].e.ep[vinegar[v].e.pvdeg]);

		// expoly_dumpdata(&vinegar[v].e);
		}

	return 1;
}

int CJRegZ::compute_beta_coefficient(FFPOLY *beta,FV_COMPONENTS_STRUCT *fc,int v,int i,FFPOLY *cmod)
{
	int k;

	if(!beta || !fc || v<0 || v>=16 || i<0 || i>fc->pow || !cmod) return 0;

	ffpoly_zeroset(beta);

	/* V = (V_0, V_1, V_2, V_3) which belong to { 0,1 }^4 */
	for(k=0;k<VINEGAR_BITS;k++) {
		if(((v << k) & 0x08) != 0)	
			ffpoly_addmod(beta,beta,&fc->b.epsilon[i][k],cmod);
	}

	ffpoly_addmod(beta,beta,&fc->b.ni[i],cmod);
	return 1;
}

int CJRegZ::compute_gamma_coefficient(FFPOLY *gamma,FV_COMPONENTS_STRUCT *fc,int v,FFPOLY *cmod)
{
	int k,l;

	if(!gamma || !fc || v<0 || v>=16 || !cmod) return 0;
	
	ffpoly_zeroset(gamma);

	/* V = (V_0, V_1, V_2, V_3) which belong to { 0,1 }^4 */
	for(k=0;k<VINEGAR_BITS;k++) {
		int vk = (((v << k) & 0x08)!=0 ? 1 : 0);

		for(l=k+1;l<VINEGAR_BITS && vk;l++) {
			int vl = (((v << l) & 0x08)!=0 ? 1 : 0);
			if(vl) ffpoly_addmod(gamma,gamma,&fc->g.eta[k][l],cmod);
		}
	}

	for(k=0;k<VINEGAR_BITS;k++) {
		if(((v << k) & 0x08) != 0)	
			ffpoly_addmod(gamma,gamma,&fc->g.rho[k],cmod);
	}

	ffpoly_addmod(gamma,gamma,&fc->g.tau,cmod);
	return 1;
}

int CJRegZ::get_vinegar(ELEMENT *in)
{
	int i,v = 0;

	for(i=0;i<VINEGAR_BITS;i++) {
		v <<= 1;
		if(in[i]) v |= 1;
	}

	return v;
}

void CJRegZ::choose_solution(FFPOLY *Z,ROOT_POT *pot)
{
	FFPOLY *probe = &pot->factors[0];
	int i;
	
	/* Get the lower solution ! */
	for(i=1;i<pot->factsNo;i++) {
		if(ffpoly_cmp(probe,&pot->factors[i]) > 0) 
			probe = &pot->factors[i];
	}

	ffpoly_copy(Z,probe);
}

