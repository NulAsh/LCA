
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "expoly.h"
#include "osdepn.h"

#include "karatsuba.h"

int karatsuba_cross = KARATSUBA_EXPOLY_CROSS;

/* ************************************************************** */

void expoly_fast_recompute_degree(EXPOLY *p,int maxdeg)
	{
	for(p->pvdeg=maxdeg;p->pvdeg>=0;p->pvdeg--)
		if(ffpoly_is_zero(&p->ep[p->pvdeg])==0) break;

	return;
	}


static void expoly_recompute_degree(EXPOLY *p)
	{
	for(p->pvdeg=p->pvsize-1;p->pvdeg>=0;p->pvdeg--)
		if(ffpoly_is_zero(&p->ep[p->pvdeg])==0) break;

	return;
	}

/* ************************************************************** */

int expoly_init(EXPOLY *p,int _size)
	{
	if(!p) return 0;

	p->pvdeg  = EX_UNINITIALIZED_POLY;
	p->pvsize =  0;

	if(_size < 0 || _size > EX_MAXDEGREE) return 0;

	if(_size == 0) _size = EX_DEFAULTDEGREE;

	p->ep = (FFPOLY *)malloc(_size * sizeof(FFPOLY));
	if(!p->ep) return 0;

	p->pvsize = _size;
	
	return 1;
	}

int expoly_free(EXPOLY *p)
	{
	if(!p) return 0;

	if(!p->ep) return 0;
	if(p->pvsize <=0) return 0;

	free(p->ep);
	p->pvdeg  = EX_UNINITIALIZED_POLY;
	p->pvsize =  0;

	return 1;
	}

#define CHECKCONSISTENCY(p) do {\
	if(!p || !p->ep || p->pvsize<=0) return 0; \
	} while(0)

int expoly_zeroset(EXPOLY *p)
	{
	CHECKCONSISTENCY(p);

    ffpoly_zeroset_n(p->ep,p->pvsize);

	//for(i=0;i<p->pvsize;i++)
	//	ffpoly_zeroset(&p->ep[i]);

	p->pvdeg = -1;

	return 1;
	}

int expoly_copy(EXPOLY *dest,EXPOLY *src)
	{
	//int i;
	
	CHECKCONSISTENCY(dest);
	CHECKCONSISTENCY(src);

	if(dest->pvsize <= src->pvdeg) return 0;

	//for(i=dest->pvsize-1;i>src->pvdeg;i--)
	//	ffpoly_zeroset(&dest->ep[i]);

	ffpoly_zeroset_n(&dest->ep[src->pvdeg+1],dest->pvsize - src->pvdeg - 1);

	//for(i=src->pvdeg;i>=0;i--)
	//	ffpoly_copy(&dest->ep[i],&src->ep[i]);
	memcpy(dest->ep,src->ep,(src->pvdeg+1) * sizeof(FFPOLY));
	dest->pvdeg = src->pvdeg;

	return (dest->pvsize == src->pvsize) ? 1 : 2;
	}

int expoly_copy_rev(EXPOLY *dest,EXPOLY *src,int n)	/* Calculates dest = x^n*f(1/x) */
	{
	int i;
	
	CHECKCONSISTENCY(dest);
	CHECKCONSISTENCY(src);

	if(n <= EX_REVERSE_ALL) n = src->pvdeg;
	if(dest->pvsize <= n) return 0;
	if(src == dest) return -1;

	for(i=0;i<=n;i++)
		ffpoly_copy(&dest->ep[i],&src->ep[n-i]);

	//for(i=n+1;i<dest->pvsize;i++)
	//	ffpoly_zeroset(&dest->ep[i]);
	ffpoly_zeroset_n(&dest->ep[n+1],dest->pvsize - n - 1);

	expoly_fast_recompute_degree(dest,n);

	return 1;
	}

int expoly_truncate(EXPOLY *dest,EXPOLY *src,int n)		/* dest = a % x^n */
	{
	CHECKCONSISTENCY(dest);
	CHECKCONSISTENCY(src);

	if(src->pvdeg < n) return expoly_copy(dest,src);

	if(dest->pvsize < n) return 0;

    ffpoly_zeroset_n(&dest->ep[n],dest->pvdeg - n + 1); /* XXX: era pvsize */

	if(dest != src)
		ffpoly_copy_n(dest->ep,src->ep,n);
	
	expoly_fast_recompute_degree(dest,n-1);
	return 1;
	}

int expoly_newton_inverse_truncation(EXPOLY *dest,EXPOLY *src,FFPOLY *cmod,int e)
	{
	int E[EX_NEWTON_BUFFER_SIZE];
	EXPOLY g, g0, g1, g2;
	FFPOLY inv;
	int i,k,l;
	int ptr=0;
	int bb,bc;

	CHECKCONSISTENCY(dest);
	CHECKCONSISTENCY(src);
	if(!cmod || e < 0) return 0;	
	if(src->pvdeg < 0) return 0;

	if(e == 0) return 1;

	/* Inversion of constant term */
	if(ffpoly_inverse(&inv,&src->ep[0],cmod) != 1) return -1;

	if(e == 1)		// If deg(f)==1 then 
		{
		expoly_zeroset(dest);
		expoly_setterm(dest,&inv,0);

		return 1;
		}

	E[ptr++] = e;

	while( e > 1 )
		{
		if(ptr >= EX_NEWTON_BUFFER_SIZE) return -2;

		e = (e + 1) / 2;
		E[ptr++] = e;
		}

	bb = (1+3*E[0])>>1;
	bc = E[0];

	if( expoly_init(&g ,1+bc) != 1) return -3;
	if( expoly_init(&g0,1+bc) != 1) return -3;
	if( expoly_init(&g1,1+bb) != 1) return -3;
	if( expoly_init(&g2,1+bc) != 1) return -3;

	expoly_zeroset(&g);
	expoly_setterm(&g,&inv,0);

	for(i = ptr - 1; i > 0; i--)
		{
        k = E[i];
        l = E[i-1]-E[i];

		if( expoly_truncate(&g0,src,k+l) != 1 ) return 0;
        //trunc(g0, a, k+l);
		
		if( expoly_mulmod(&g1,&g0,&g,NULL,cmod) != 1 ) return 0;
        //mul(g1, g0, g);
		
		if( expoly_shiftright(&g1,&g1,k) != 1 ) return 0;
        //RightShift(g1, g1, k);
		
		if( expoly_truncate(&g1,&g1,l) != 1 ) return 0;
        //trunc(g1, g1, l);
		
		if( expoly_mulmod(&g2,&g1,&g,NULL,cmod) != 1 ) return 0;
        //mul(g2, g1, g);
		
		if( expoly_truncate(&g2,&g2,l) != 1 ) return 0;
        //trunc(g2, g2, l);
		
		if( expoly_shiftleft(&g2,&g2,k) != 1 ) return 0;
        //LeftShift(g2, g2, k);
		
		if( expoly_addmod(&g,&g,&g2,NULL,cmod) != 1 ) return 0;
        //add(g, g, g2);
		}

	if( expoly_copy(dest,&g) < 1 ) return 0;

	expoly_free(&g);
	expoly_free(&g0);
	expoly_free(&g1);
	expoly_free(&g2);

	return 1;
	}

int expoly_cmp(EXPOLY *p1,EXPOLY *p2)
	{
	int i,eq;
	
	CHECKCONSISTENCY(p1);
	CHECKCONSISTENCY(p2);
	
	if(p1->pvdeg < p2->pvdeg) return -1;
	else if(p1->pvdeg > p2->pvdeg) return 1;

	for(i = p1->pvdeg; i >= 0; i--)
		{
		eq = ffpoly_cmp(&p1->ep[i],&p2->ep[i]);
		if(eq != 0) return eq;
		}

	return 0;
	}

int expoly_identity_set(EXPOLY *p)
	{
	if(expoly_zeroset(p)==0) return 0;

	ffpoly_identity_set(&p->ep[0]);
	p->pvdeg = 0;
	
	return 1;
	}

int expoly_set(EXPOLY *p,int *deg,int degsize)
	{
	int cdeg;
	int i;

	CHECKCONSISTENCY(p);
	if(!deg) return 0;

	expoly_zeroset(p);

	cdeg = p->pvsize;
	p->pvdeg = -1;

	for(i=0;i<degsize;i++)
		{
		if(deg[i] < 0 || deg[i] >= cdeg) return 0;
		cdeg = deg[i];

		if(i==0) p->pvdeg = cdeg;

		ffpoly_identity_set(&p->ep[cdeg]);
		}
	
	return 1;
	}

int expoly_is_zero(EXPOLY *p)
	{
	if(p->pvdeg < 0) return 1;
	return 0;
	}

int expoly_is_identity(EXPOLY *p)
	{
	if(p->pvdeg != 0) return 0;
	return ffpoly_is_identity(&p->ep[0]);
	}

int expoly_setfrompoly(EXPOLY *p,FFPOLY *f)
	{
	FFPOLY id;
	int i;

	CHECKCONSISTENCY(p);
	if(!f) return 0;
	
	if(f->pvdeg >= p->pvsize) return 0;

	expoly_zeroset(p);
	
	ffpoly_identity_set(&id);

	for(i=0;i<=f->pvdeg;i++)
		if(ffpoly_getterm(f,i)) expoly_setterm(p,&id,i);

	return 1;	
	}

int expoly_setterm(EXPOLY *p,FFPOLY *f,int place)
	{
	int new_isZero;
	int old_isZero;

	CHECKCONSISTENCY(p);

	if(place < 0 || place >= p->pvsize) return 0;

	new_isZero = ffpoly_is_zero(f);
	old_isZero = ffpoly_is_zero(&p->ep[place]);

	ffpoly_copy(&p->ep[place],f);

	if(new_isZero != 0 && old_isZero == 0)
		{
		if(place == p->pvdeg)
			{
			// Ok, it's at least one degree lower
			p->pvdeg--;

			// Keep lowering the degree until we reach the zeropoly 
			// or a coeff which is not zero.
			while( p->pvdeg>0 && ffpoly_is_zero(&p->ep[p->pvdeg]) != 0 )
				p->pvdeg--;
			}
		}
	else if(new_isZero == 0 && old_isZero != 0)
		{
		// Update the degree if it was lower
		if(p->pvdeg < place) p->pvdeg = place;
		}

	return 1;
	}

int expoly_reduce(EXPOLY *rs,EXPOLY *modulus,FFPOLY *cmod)
	{
	int i,j;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(modulus);

	//if(rs->pvsize != modulus->pvsize) return 0;

	if(rs->pvdeg < modulus->pvdeg) return 1;	// Ok, no need to reduce!

	// Just assure that the poly modulus is monic, otherwise no reduction can take place
	if(ffpoly_is_identity(&modulus->ep[modulus->pvdeg]) == 0) return 0;

	for( i = rs->pvdeg - modulus->pvdeg ; i >= 0 ; i-- )
		{
		if(ffpoly_is_zero(&rs->ep[i+modulus->pvdeg]) != 0) continue;
		if(ffpoly_is_identity(&rs->ep[i+modulus->pvdeg]) == 0) 
			{
            // The coefficient is not == 1, then we use multiplicative inverses !
			FFPOLY tmp,out;

			ffpoly_copy(&tmp,&rs->ep[i+modulus->pvdeg]);
			ffpoly_zeroset(&rs->ep[i+modulus->pvdeg]);

			for( j = modulus->pvdeg-1 ; j >= 0 ; j-- )
				{
				ffpoly_mulmod(&out,&tmp,&modulus->ep[j],cmod);
                //ffpoly_identity_set(&out);
				ffpoly_addmod(&rs->ep[i+j],&rs->ep[i+j],&out,NULL /*cmod*/);
				}
			}
		else
			{
            // The coefficient is equal to 1
			ffpoly_zeroset(&rs->ep[i+modulus->pvdeg]);

			for( j = modulus->pvdeg-1 ; j >= 0 ; j-- )
				ffpoly_addmod(&rs->ep[i+j],&rs->ep[i+j],&modulus->ep[j],cmod);
			}
		}

	for( rs->pvdeg = modulus->pvdeg - 1; rs->pvdeg >=0; rs->pvdeg--)
		if(ffpoly_is_zero(&rs->ep[rs->pvdeg]) == 0) break;

	if(cmod)
		{
		for(i=0;i<=rs->pvdeg;i++)
			ffpoly_reduce(&rs->ep[i],cmod);
		}

	return 1;
	}

int expoly_addone(EXPOLY *p)
	{
	CHECKCONSISTENCY(p);
	
	if(p->pvdeg < 0) return 0;

	ffpoly_addone(&p->ep[0]);

	if(p->pvdeg == 0) p->pvdeg = MIN(0,p->ep[0].pvdeg);
	return 1;
	}

int expoly_addmod(EXPOLY *rs,EXPOLY *p1,EXPOLY *p2,EXPOLY *pmod,FFPOLY *cmod)
	{
	int i,mx;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(p1);
	CHECKCONSISTENCY(p2);
	
	if(p1->pvdeg >= rs->pvsize || p2->pvdeg >= rs->pvsize) return 0;
	
	mx = MAX(p1->pvdeg,p2->pvdeg);

	rs->pvdeg = -1;
	
	/* By splitting the operations in two cycles we gain some speed to cache issues. */
	for(i=0;i<=mx;i++)
		ffpoly_addmod(&rs->ep[i],&p1->ep[i],&p2->ep[i],NULL);

	for(i=0;i<=mx;i++)
		ffpoly_reduce(&rs->ep[i],cmod);

    ffpoly_zeroset_n(&rs->ep[mx+1],rs->pvsize - mx - 1);	
	expoly_fast_recompute_degree(rs,mx);

	if(pmod) 
		{
		if( expoly_reduce(rs,pmod,cmod) != 1 ) return 0;
		}

	return 1;
	}

int expoly_fast_add(EXPOLY *rs,EXPOLY *p1, int cmod_deg)
	{
	int dmax = MAX(rs->pvdeg,p1->pvdeg);
	int i;

	for(i=0;i<=dmax;i++)
		{
		rs->ep[i].pv[0] ^= p1->ep[i].pv[0];
		rs->ep[i].pv[1] ^= p1->ep[i].pv[1];
		rs->ep[i].pv[2] ^= p1->ep[i].pv[2];
		rs->ep[i].pv[3] ^= p1->ep[i].pv[3];
		
		ffpoly_fast_update_degree(&rs->ep[i],cmod_deg - 1);
		}

	ffpoly_zeroset_n(&rs->ep[dmax+1],rs->pvsize - dmax - 1);	
	expoly_fast_recompute_degree(rs,dmax);

	return 1;
	}

int expoly_karatsuba_multiply(EXPOLY *out,EXPOLY *p,EXPOLY *q,FFPOLY *cmod)
	{
	static FFPOLY *stk=NULL;
	int n, hn, sp;
	int sa,sb;
	int buffsize;
	static int maxalloc=0;

	if(out->pvsize <= p->pvdeg + q->pvdeg) return 0;

	sa = p->pvdeg + 1;
	sb = q->pvdeg + 1;

	n = MAX(sa, sb);
	sp = 0;
	do 
		{
		hn = (n+1) >> 1;
		sp += (hn << 2) - 1;
		n = hn;
		} while (n > 1);

	buffsize = (sp + 2*(sa+sb)-1) * sizeof(FFPOLY);

	if(maxalloc < buffsize || stk == NULL)
		{
		if(stk) free(stk);
		stk = (FFPOLY *)malloc( buffsize );
		maxalloc = buffsize;

		if(!stk) return 0;
		}

    memcpy(&stk[sa+sb-1],&p->ep[0],sa * sizeof(FFPOLY));
    memcpy(&stk[2*sa+sb-1],&q->ep[0],sb * sizeof(FFPOLY));

	expoly_recursive_karatsuba_mul(&stk[0], &stk[sa+sb-1], sa, &stk[2*sa+sb-1], sb, &stk[2*(sa+sb)-1],cmod);

	ffpoly_copy_n(out->ep,stk,sa+sb-1);
	ffpoly_zeroset_n(&out->ep[sa+sb],out->pvsize - sa - sb);

	expoly_fast_recompute_degree(out,p->pvdeg + q->pvdeg);

	//free(stk);
	return 1;
	}

int expoly_mulmod(EXPOLY *rs,EXPOLY *p1,EXPOLY *p2,EXPOLY *pmod,FFPOLY *cmod)
	{
	int tmpdeg;
	int i,j;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(p1);
	CHECKCONSISTENCY(p2);

	tmpdeg = p1->pvdeg + p2->pvdeg;

	if(pmod)
		{
		if(rs->pvsize < pmod->pvdeg + 1) return 0;
		}
	else
		{
		if(rs->pvsize < tmpdeg + 1) return 0;
		}


	if(p1->pvdeg >= karatsuba_cross && p2->pvdeg >= karatsuba_cross && cmod)
		{
		if(rs != p1 && rs != p2)
			{
			expoly_karatsuba_multiply(rs,p1,p2,cmod);
			expoly_fast_recompute_degree(rs,tmpdeg);

			if(pmod && cmod && rs->pvdeg >= pmod->pvdeg)
				{
				if( expoly_reduce(rs,pmod,cmod) != 1 ) return 0;
				}
			}
		else
			{
			EXPOLY ws;
		
			expoly_init(&ws,tmpdeg+1);
			expoly_zeroset(&ws);
	
			expoly_karatsuba_multiply(&ws,p1,p2,cmod);

			expoly_fast_recompute_degree(&ws,tmpdeg);
			if(pmod && cmod && ws.pvdeg >= pmod->pvdeg)
				{
				if( expoly_reduce(&ws,pmod,cmod) != 1 ) return 0;
				}

			expoly_copy(rs,&ws);
			expoly_free(&ws);
			}
		}
	else
		{
		EXPOLY ws;
	
		expoly_init(&ws,tmpdeg+1);
		expoly_zeroset(&ws);

		for(i=0;i<=p1->pvdeg;i++)
			{
			if(ffpoly_is_zero(&p1->ep[i]) != 0) continue;
			if(ffpoly_is_identity(&p1->ep[i]) != 0)
				{
				for(j=0;j<=p2->pvdeg;j++)
					ffpoly_addmod(&ws.ep[i+j],&ws.ep[i+j],&p2->ep[j],cmod);
				}
			else
				{
				FFPOLY tmp;

				for(j=0;j<=p2->pvdeg;j++)
					{
					ffpoly_mulmod(&tmp,&p1->ep[i],&p2->ep[j],cmod);
					ffpoly_addmod(&ws.ep[i+j],&ws.ep[i+j],&tmp,cmod);
					}
				}
			}
		
		expoly_fast_recompute_degree(&ws,tmpdeg);
		if(pmod && cmod && ws.pvdeg >= pmod->pvdeg)
			{
			if( expoly_reduce(&ws,pmod,cmod) != 1 ) return 0;
			}

		expoly_copy(rs,&ws);
		expoly_free(&ws);
		}

 	return 1;
	}

int expoly_mulscalar(EXPOLY *rs,EXPOLY *p,FFPOLY *scalar,FFPOLY *mod)
	{
	int i;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(p);

	if(!scalar || !mod) return 0;

	for(i=0;i<=p->pvdeg;i++)
		{
		if( ffpoly_mulmod(&rs->ep[i],&p->ep[i],scalar,mod) != 1 ) return 0;
		}

	return 1;
	}

int expoly_shiftleft(EXPOLY *rs,EXPOLY *p,int n)
	{
	int dest_start;
	int src_start;
	int loop_start;
	//int i,j;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(p);
		
	if(n > p->pvsize) 
		{
		expoly_zeroset(rs);
		return 1;
		}
	else if(n == 0)
		{
		expoly_copy(rs,p);
		return 1;
		}
	else if(n < 0) return expoly_shiftright(rs,p,-n);

	loop_start = 0;

	dest_start = p->pvdeg + n;
	src_start  = p->pvdeg;

	if(dest_start >= rs->pvsize) loop_start = dest_start - rs->pvsize + 1;

	ffpoly_copy_n(&rs->ep[dest_start-p->pvdeg],
		          &p->ep[src_start-p->pvdeg],
				  p->pvdeg - loop_start + 1);

	ffpoly_zeroset_n(rs->ep,dest_start - p->pvdeg);
	ffpoly_zeroset_n(&rs->ep[dest_start + 1],rs->pvsize - dest_start - 1);

	if(p->pvdeg + n < rs->pvsize) rs->pvdeg = p->pvdeg + n;
	else expoly_recompute_degree(rs);

	return 1;	
	}

int expoly_shiftright(EXPOLY *rs,EXPOLY *p,int n)
	{
	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(p);

	if(n > p->pvdeg) 
		{
		expoly_zeroset(rs);
		return 1;
		}
	else if(n==0) 
		{
		expoly_copy(rs,p);
		return 1;
		}
	else if(n < 0) 
		{
		return expoly_shiftleft(rs,p,-n);
		}

	rs->pvdeg = p->pvdeg - n;
	if(rs->pvsize <= rs->pvdeg) return 0;

	ffpoly_copy_n(rs->ep,&p->ep[n],rs->pvdeg+1);
	ffpoly_zeroset_n(&rs->ep[rs->pvdeg+1],rs->pvsize - rs->pvdeg - 1);

	return 1;
	}

int expoly_invmod(EXPOLY *g,EXPOLY *f,int l,FFPOLY *cmod)
	{
	EXPOLY g0,g1;
	EXPOLY *gh,*gl,*gt;
	int r=0;
	int mdeg;
	int i;

	CHECKCONSISTENCY(f);
	CHECKCONSISTENCY(g);

	while( l > (1 << r) )
		r++;

	mdeg = 2 * (1 << r) + f->pvdeg + 2;

	expoly_init(&g0,mdeg);
	expoly_init(&g1,mdeg);

	gh = &g0;
	gl = &g1;

	expoly_identity_set(gl);

	for(i=1;i<=r;i++)
		{
		if( expoly_square(gh,gl,NULL,cmod) != 1 ) return 0;
		if( expoly_mulmod(gl,f,gh,NULL,cmod) != 1 ) return 0;
		if( expoly_truncate(gh,gl,1 << i) != 1 ) return 0;

		gt = gl;	/* Swap gh and gl */
		gl = gh;
		gh = gt;
		}

	if( expoly_copy(g,gl) < 1 ) return 0;

	expoly_free(&g0);
	expoly_free(&g1);

	return 1;
	}

int expoly_fast_divide(EXPOLY *qs,EXPOLY *rs,EXPOLY *a, EXPOLY *b, FFPOLY *cmod)
	{
	EXPOLY b_invrev;
	EXPOLY b_rev;
	EXPOLY a_rev;
	EXPOLY qstar;
	int is_monic;
	int m;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(qs);
	CHECKCONSISTENCY(a);
	CHECKCONSISTENCY(b);

	is_monic = ffpoly_is_identity(&b->ep[b->pvdeg]);

	if(is_monic != 1) /* Not monic, can't divide... */
		return expoly_divide(qs,rs,a,b,cmod);

	m = a->pvdeg - b->pvdeg;

	if(m < 0)
		{
		expoly_zeroset(qs);
		expoly_copy(rs,b);
		
		return 1;
		}

	// I'm dividing by 1
	if(expoly_is_identity(b))	
		{
		expoly_copy(qs,a);
		expoly_zeroset(rs);

		return 1;
		}

	expoly_init(&a_rev,a->pvdeg + 1);
	expoly_init(&b_rev,b->pvdeg + 1);
	expoly_init(&b_invrev,a->pvdeg + 1);
	expoly_init(&qstar,2 * a->pvsize);

	/* Compute rev(a,deg(a)) */
	expoly_copy_rev(&a_rev,a,a->pvdeg);
	
	/* Compute rev(b,deg(b)) */
	expoly_copy_rev(&b_rev,b,b->pvdeg);

	/* Compute inv(rev(b,deg(b)) */
	if( expoly_invmod(&b_invrev,&b_rev,m,cmod) != 1 ) return 0;	
	
	/* Compute qstar = rev(a) * inv(rev(b)) */
	if( expoly_mulmod(&qstar,&a_rev,&b_invrev,NULL,cmod) != 1 ) return 0;	

	/* Compute qs = rev(qstar,m+1) */
	expoly_copy_rev(qs,&qstar,m);

	/* Compute the remainder rs = a - b*qs (which is the same as a+b*qs) */
	expoly_mulmod(rs,b,qs,NULL,cmod);
	expoly_addmod(rs,rs,a,NULL,NULL);

	expoly_free(&qstar);
	expoly_free(&b_invrev);
	expoly_free(&b_rev);
	expoly_free(&a_rev);

	return 1;
	}

int expoly_divide(EXPOLY *qs,EXPOLY *rs,EXPOLY *g, EXPOLY *divisor, FFPOLY *cmod)
	{
	int degDiff;
	int i,j;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(qs);
	CHECKCONSISTENCY(g);
	CHECKCONSISTENCY(divisor);

	if(divisor->pvdeg < 0) return 0;

	expoly_zeroset(qs);
	expoly_copy(rs,g);
		
	if(rs->pvdeg < 0) return 1;

	//if(ffpoly_is_identity(&divisor->ep[divisor->pvdeg]) == 0) 
	//	return 0; /* Not monic */

	if(rs->pvdeg < divisor->pvdeg) 
		return 1;	/* No need to reduce ! */

	// I'm dividing by 1
	if(expoly_is_identity(divisor))	
		{
		expoly_copy(qs,g);
		expoly_zeroset(rs);

		return 1;
		}

	degDiff = g->pvdeg - divisor->pvdeg;

	for(i=0;i<=degDiff;i++)
		{
		FFPOLY minv;
		if(ffpoly_is_zero(&rs->ep[g->pvdeg - i]) != 0) continue;

		ffpoly_field_div(&minv,&rs->ep[g->pvdeg - i],&divisor->ep[divisor->pvdeg],cmod);
		ffpoly_copy(&qs->ep[degDiff - i],&minv);
		
		ffpoly_zeroset(&rs->ep[g->pvdeg - i]);

		for(j=1;j<=divisor->pvdeg;j++)
			{
			FFPOLY out;

			ffpoly_mulmod(&out,&minv,&divisor->ep[divisor->pvdeg - j],cmod);
			ffpoly_addmod(&rs->ep[g->pvdeg - j - i],&rs->ep[g->pvdeg - j - i],&out,cmod);
			}

		expoly_recompute_degree(rs);
		}

	expoly_recompute_degree(qs);

	return 1;
	}

int expoly_gcd(EXPOLY *rs,EXPOLY *a,EXPOLY *b,FFPOLY *cmod)
	{
	EXPOLY f,q,g,r;
	EXPOLY *fp,*gp,*rp,*tp;
	int flag = 1;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(a);
	CHECKCONSISTENCY(b);

	expoly_init(&f,rs->pvsize);
	expoly_init(&q,rs->pvsize);
	expoly_init(&g,rs->pvsize);
	expoly_init(&r,rs->pvsize);

	if(a->pvdeg > b->pvdeg) 
		{
		expoly_copy(&f,a);
		expoly_copy(&g,b);
		}
	else
		{
		expoly_copy(&f,b);
		expoly_copy(&g,a);
		}

	fp = &f;
	gp = &g;
	rp = &r;

	while( 1 )
		{
		if( expoly_divide(&q,rp,fp,gp,cmod) == 0 )
			{
			flag = 0;
			break;
			}

		if(rp->pvdeg >= gp->pvdeg) 
			return 0;

		if(rp->pvdeg < 0) break;	/* g divide f senza resto */

		//expoly_copy(&f,&g);
		//expoly_copy(&g,&r);

		tp = fp;
		fp = gp;
		gp = rp;
		rp = tp;
		} 

	expoly_copy(rs,gp);

	expoly_free(&r);
	expoly_free(&g);
	expoly_free(&q);
	expoly_free(&f);

	return flag;
	}

int expoly_square(EXPOLY *rs,EXPOLY *p,EXPOLY *pmod,FFPOLY *cmod)
	{
	int i;
	int mx;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(p);

	if(p->pvdeg < 0)
		{
		expoly_zeroset(rs);
		return 1;
		}

	if(p->pvsize != rs->pvsize)
		return 0;

	if(pmod && p->pvsize < pmod->pvsize) return 0;

    mx = 2 * p->pvdeg;

	for(i=p->pvdeg;i>0;i--)
		{
		ffpoly_square(&rs->ep[2*i],&p->ep[i],cmod);
		ffpoly_zeroset(&rs->ep[2*i-1]);
		}

	ffpoly_square(&rs->ep[0],&p->ep[0],cmod);

	expoly_fast_recompute_degree(rs,mx);
	if(pmod && cmod)
		{
		if( expoly_reduce(rs,pmod,cmod) != 1 ) return 0;
		}

 	return 1;
	}

int expoly_powmod(EXPOLY *rs,EXPOLY *p,int n,EXPOLY *pmod,FFPOLY *cmod)
	{
	EXPOLY tmp,acc;
	int i;

    expoly_init(&tmp,0);
    expoly_init(&acc,0);

	expoly_identity_set(&tmp);
	expoly_copy(&acc,p);

	for(i=n;i>0;i>>=1)
		{
		if(i & 0x01)
			{
			if( expoly_mulmod(&tmp,&tmp,&acc,pmod,cmod) == 0 ) return 0;
			}

		if( expoly_square(&acc,&acc,pmod,cmod) == 0) return 0;
		}

	expoly_copy(rs,&tmp);

    expoly_free(&acc);
    expoly_free(&tmp);

	return 1;
	}

int expoly_muladd(EXPOLY *rs,EXPOLY *p,FFPOLY *coeff,FFPOLY *mod)
	{
	int i;

	CHECKCONSISTENCY(rs);
	CHECKCONSISTENCY(p);
	
	if(rs->pvsize != p->pvsize) return 0;

	for(i=0;i<=p->pvdeg;i++)
		{
		FFPOLY tmp;
		
		ffpoly_mulmod(&tmp,coeff,&p->ep[i],mod);
		ffpoly_addmod(&rs->ep[i],&rs->ep[i],&tmp,mod);
		}
	
	expoly_recompute_degree(rs);
	return 1;
	}

int expoly_subst(FFPOLY *rs,EXPOLY *f,FFPOLY *x,FFPOLY *mod)
	{
	FFPOLY sum,factor;
	int i;

	CHECKCONSISTENCY(f);

	if(f->pvdeg < 0) return 0;

	ffpoly_zeroset(rs);
	ffpoly_copy(rs,&f->ep[0]);
	ffpoly_copy(&factor,x);

	for(i=1;i<=f->pvdeg;i++)
		{
		ffpoly_mulmod(&sum,&f->ep[i],&factor,mod);
		ffpoly_addmod(rs,rs,&sum,mod);
		ffpoly_mulmod(&factor,&factor,x,mod);	
		}

	return 1;
	}

