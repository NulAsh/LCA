
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include <stdio.h>
#include "fred103.h"

#define SUBDUMPS
#define USEFASTESTMODRED103

extern unsigned char idxlup[256];

const RED103 red103[256-103] = {
	{ 0x80000000,7,0x00000002,0x01000000,5,4 }, /* 255 */
	{ 0x40000000,7,0x00000001,0x00800000,5,4 }, /* 254 */
	{ 0x20000000,7,0x80000000,0x00400000,4,4 }, /* 253 */
	{ 0x10000000,7,0x40000000,0x00200000,4,4 }, /* 252 */
	{ 0x08000000,7,0x20000000,0x00100000,4,4 }, /* 251 */
	{ 0x04000000,7,0x10000000,0x00080000,4,4 }, /* 250 */
	{ 0x02000000,7,0x08000000,0x00040000,4,4 }, /* 249 */
	{ 0x01000000,7,0x04000000,0x00020000,4,4 }, /* 248 */
	{ 0x00800000,7,0x02000000,0x00010000,4,4 }, /* 247 */
	{ 0x00400000,7,0x01000000,0x00008000,4,4 }, /* 246 */
	{ 0x00200000,7,0x00800000,0x00004000,4,4 }, /* 245 */
	{ 0x00100000,7,0x00400000,0x00002000,4,4 }, /* 244 */
	{ 0x00080000,7,0x00200000,0x00001000,4,4 }, /* 243 */
	{ 0x00040000,7,0x00100000,0x00000800,4,4 }, /* 242 */
	{ 0x00020000,7,0x00080000,0x00000400,4,4 }, /* 241 */
	{ 0x00010000,7,0x00040000,0x00000200,4,4 }, /* 240 */
	{ 0x00008000,7,0x00020000,0x00000100,4,4 }, /* 239 */
	{ 0x00004000,7,0x00010000,0x00000080,4,4 }, /* 238 */
	{ 0x00002000,7,0x00008000,0x00000040,4,4 }, /* 237 */
	{ 0x00001000,7,0x00004000,0x00000020,4,4 }, /* 236 */
	{ 0x00000800,7,0x00002000,0x00000010,4,4 }, /* 235 */
	{ 0x00000400,7,0x00001000,0x00000008,4,4 }, /* 234 */
	{ 0x00000200,7,0x00000800,0x00000004,4,4 }, /* 233 */
	{ 0x00000100,7,0x00000400,0x00000002,4,4 }, /* 232 */
	{ 0x00000080,7,0x00000200,0x00000001,4,4 }, /* 231 */
	{ 0x00000040,7,0x00000100,0x80000000,4,3 }, /* 230 */
	{ 0x00000020,7,0x00000080,0x40000000,4,3 }, /* 229 */
	{ 0x00000010,7,0x00000040,0x20000000,4,3 }, /* 228 */
	{ 0x00000008,7,0x00000020,0x10000000,4,3 }, /* 227 */
	{ 0x00000004,7,0x00000010,0x08000000,4,3 }, /* 226 */
	{ 0x00000002,7,0x00000008,0x04000000,4,3 }, /* 225 */
	{ 0x00000001,7,0x00000004,0x02000000,4,3 }, /* 224 */
	{ 0x80000000,6,0x00000002,0x01000000,4,3 }, /* 223 */
	{ 0x40000000,6,0x00000001,0x00800000,4,3 }, /* 222 */
	{ 0x20000000,6,0x80000000,0x00400000,3,3 }, /* 221 */
	{ 0x10000000,6,0x40000000,0x00200000,3,3 }, /* 220 */
	{ 0x08000000,6,0x20000000,0x00100000,3,3 }, /* 219 */
	{ 0x04000000,6,0x10000000,0x00080000,3,3 }, /* 218 */
	{ 0x02000000,6,0x08000000,0x00040000,3,3 }, /* 217 */
	{ 0x01000000,6,0x04000000,0x00020000,3,3 }, /* 216 */
	{ 0x00800000,6,0x02000000,0x00010000,3,3 }, /* 215 */
	{ 0x00400000,6,0x01000000,0x00008000,3,3 }, /* 214 */
	{ 0x00200000,6,0x00800000,0x00004000,3,3 }, /* 213 */
	{ 0x00100000,6,0x00400000,0x00002000,3,3 }, /* 212 */
	{ 0x00080000,6,0x00200000,0x00001000,3,3 }, /* 211 */
	{ 0x00040000,6,0x00100000,0x00000800,3,3 }, /* 210 */
	{ 0x00020000,6,0x00080000,0x00000400,3,3 }, /* 209 */
	{ 0x00010000,6,0x00040000,0x00000200,3,3 }, /* 208 */
	{ 0x00008000,6,0x00020000,0x00000100,3,3 }, /* 207 */
	{ 0x00004000,6,0x00010000,0x00000080,3,3 }, /* 206 */
	{ 0x00002000,6,0x00008000,0x00000040,3,3 }, /* 205 */
	{ 0x00001000,6,0x00004000,0x00000020,3,3 }, /* 204 */
	{ 0x00000800,6,0x00002000,0x00000010,3,3 }, /* 203 */
	{ 0x00000400,6,0x00001000,0x00000008,3,3 }, /* 202 */
	{ 0x00000200,6,0x00000800,0x00000004,3,3 }, /* 201 */
	{ 0x00000100,6,0x00000400,0x00000002,3,3 }, /* 200 */
	{ 0x00000080,6,0x00000200,0x00000001,3,3 }, /* 199 */
	{ 0x00000040,6,0x00000100,0x80000000,3,2 }, /* 198 */
	{ 0x00000020,6,0x00000080,0x40000000,3,2 }, /* 197 */
	{ 0x00000010,6,0x00000040,0x20000000,3,2 }, /* 196 */
	{ 0x00000008,6,0x00000020,0x10000000,3,2 }, /* 195 */
	{ 0x00000004,6,0x00000010,0x08000000,3,2 }, /* 194 */
	{ 0x00000002,6,0x00000008,0x04000000,3,2 }, /* 193 */
	{ 0x00000001,6,0x00000004,0x02000000,3,2 }, /* 192 */
	{ 0x80000000,5,0x00000002,0x01000000,3,2 }, /* 191 */
	{ 0x40000000,5,0x00000001,0x00800000,3,2 }, /* 190 */
	{ 0x20000000,5,0x80000000,0x00400000,2,2 }, /* 189 */
	{ 0x10000000,5,0x40000000,0x00200000,2,2 }, /* 188 */
	{ 0x08000000,5,0x20000000,0x00100000,2,2 }, /* 187 */
	{ 0x04000000,5,0x10000000,0x00080000,2,2 }, /* 186 */
	{ 0x02000000,5,0x08000000,0x00040000,2,2 }, /* 185 */
	{ 0x01000000,5,0x04000000,0x00020000,2,2 }, /* 184 */
	{ 0x00800000,5,0x02000000,0x00010000,2,2 }, /* 183 */
	{ 0x00400000,5,0x01000000,0x00008000,2,2 }, /* 182 */
	{ 0x00200000,5,0x00800000,0x00004000,2,2 }, /* 181 */
	{ 0x00100000,5,0x00400000,0x00002000,2,2 }, /* 180 */
	{ 0x00080000,5,0x00200000,0x00001000,2,2 }, /* 179 */
	{ 0x00040000,5,0x00100000,0x00000800,2,2 }, /* 178 */
	{ 0x00020000,5,0x00080000,0x00000400,2,2 }, /* 177 */
	{ 0x00010000,5,0x00040000,0x00000200,2,2 }, /* 176 */
	{ 0x00008000,5,0x00020000,0x00000100,2,2 }, /* 175 */
	{ 0x00004000,5,0x00010000,0x00000080,2,2 }, /* 174 */
	{ 0x00002000,5,0x00008000,0x00000040,2,2 }, /* 173 */
	{ 0x00001000,5,0x00004000,0x00000020,2,2 }, /* 172 */
	{ 0x00000800,5,0x00002000,0x00000010,2,2 }, /* 171 */
	{ 0x00000400,5,0x00001000,0x00000008,2,2 }, /* 170 */
	{ 0x00000200,5,0x00000800,0x00000004,2,2 }, /* 169 */
	{ 0x00000100,5,0x00000400,0x00000002,2,2 }, /* 168 */
	{ 0x00000080,5,0x00000200,0x00000001,2,2 }, /* 167 */
	{ 0x00000040,5,0x00000100,0x80000000,2,1 }, /* 166 */
	{ 0x00000020,5,0x00000080,0x40000000,2,1 }, /* 165 */
	{ 0x00000010,5,0x00000040,0x20000000,2,1 }, /* 164 */
	{ 0x00000008,5,0x00000020,0x10000000,2,1 }, /* 163 */
	{ 0x00000004,5,0x00000010,0x08000000,2,1 }, /* 162 */
	{ 0x00000002,5,0x00000008,0x04000000,2,1 }, /* 161 */
	{ 0x00000001,5,0x00000004,0x02000000,2,1 }, /* 160 */
	{ 0x80000000,4,0x00000002,0x01000000,2,1 }, /* 159 */
	{ 0x40000000,4,0x00000001,0x00800000,2,1 }, /* 158 */
	{ 0x20000000,4,0x80000000,0x00400000,1,1 }, /* 157 */
	{ 0x10000000,4,0x40000000,0x00200000,1,1 }, /* 156 */
	{ 0x08000000,4,0x20000000,0x00100000,1,1 }, /* 155 */
	{ 0x04000000,4,0x10000000,0x00080000,1,1 }, /* 154 */
	{ 0x02000000,4,0x08000000,0x00040000,1,1 }, /* 153 */
	{ 0x01000000,4,0x04000000,0x00020000,1,1 }, /* 152 */
	{ 0x00800000,4,0x02000000,0x00010000,1,1 }, /* 151 */
	{ 0x00400000,4,0x01000000,0x00008000,1,1 }, /* 150 */
	{ 0x00200000,4,0x00800000,0x00004000,1,1 }, /* 149 */
	{ 0x00100000,4,0x00400000,0x00002000,1,1 }, /* 148 */
	{ 0x00080000,4,0x00200000,0x00001000,1,1 }, /* 147 */
	{ 0x00040000,4,0x00100000,0x00000800,1,1 }, /* 146 */
	{ 0x00020000,4,0x00080000,0x00000400,1,1 }, /* 145 */
	{ 0x00010000,4,0x00040000,0x00000200,1,1 }, /* 144 */
	{ 0x00008000,4,0x00020000,0x00000100,1,1 }, /* 143 */
	{ 0x00004000,4,0x00010000,0x00000080,1,1 }, /* 142 */
	{ 0x00002000,4,0x00008000,0x00000040,1,1 }, /* 141 */
	{ 0x00001000,4,0x00004000,0x00000020,1,1 }, /* 140 */
	{ 0x00000800,4,0x00002000,0x00000010,1,1 }, /* 139 */
	{ 0x00000400,4,0x00001000,0x00000008,1,1 }, /* 138 */
	{ 0x00000200,4,0x00000800,0x00000004,1,1 }, /* 137 */
	{ 0x00000100,4,0x00000400,0x00000002,1,1 }, /* 136 */
	{ 0x00000080,4,0x00000200,0x00000001,1,1 }, /* 135 */
	{ 0x00000040,4,0x00000100,0x80000000,1,0 }, /* 134 */
	{ 0x00000020,4,0x00000080,0x40000000,1,0 }, /* 133 */
	{ 0x00000010,4,0x00000040,0x20000000,1,0 }, /* 132 */
	{ 0x00000008,4,0x00000020,0x10000000,1,0 }, /* 131 */
	{ 0x00000004,4,0x00000010,0x08000000,1,0 }, /* 130 */
	{ 0x00000002,4,0x00000008,0x04000000,1,0 }, /* 129 */
	{ 0x00000001,4,0x00000004,0x02000000,1,0 }, /* 128 */
	{ 0x80000000,3,0x00000002,0x01000000,1,0 }, /* 127 */
	{ 0x40000000,3,0x00000001,0x00800000,1,0 }, /* 126 */
	{ 0x20000000,3,0x80000000,0x00400000,0,0 }, /* 125 */
	{ 0x10000000,3,0x40000000,0x00200000,0,0 }, /* 124 */
	{ 0x08000000,3,0x20000000,0x00100000,0,0 }, /* 123 */
	{ 0x04000000,3,0x10000000,0x00080000,0,0 }, /* 122 */
	{ 0x02000000,3,0x08000000,0x00040000,0,0 }, /* 121 */
	{ 0x01000000,3,0x04000000,0x00020000,0,0 }, /* 120 */
	{ 0x00800000,3,0x02000000,0x00010000,0,0 }, /* 119 */
	{ 0x00400000,3,0x01000000,0x00008000,0,0 }, /* 118 */
	{ 0x00200000,3,0x00800000,0x00004000,0,0 }, /* 117 */
	{ 0x00100000,3,0x00400000,0x00002000,0,0 }, /* 116 */
	{ 0x00080000,3,0x00200000,0x00001000,0,0 }, /* 115 */
	{ 0x00040000,3,0x00100000,0x00000800,0,0 }, /* 114 */
	{ 0x00020000,3,0x00080000,0x00000400,0,0 }, /* 113 */
	{ 0x00010000,3,0x00040000,0x00000200,0,0 }, /* 112 */
	{ 0x00008000,3,0x00020000,0x00000100,0,0 }, /* 111 */
	{ 0x00004000,3,0x00010000,0x00000080,0,0 }, /* 110 */
	{ 0x00002000,3,0x00008000,0x00000040,0,0 }, /* 109 */
	{ 0x00001000,3,0x00004000,0x00000020,0,0 }, /* 108 */
	{ 0x00000800,3,0x00002000,0x00000010,0,0 }, /* 107 */
	{ 0x00000400,3,0x00001000,0x00000008,0,0 }, /* 106 */
	{ 0x00000200,3,0x00000800,0x00000004,0,0 }, /* 105 */
	{ 0x00000100,3,0x00000400,0x00000002,0,0 }, /* 104 */
	{ 0x00000080,3,0x00000200,0x00000001,0,0 }, /* 103 */
	};

const WORD32 L103[256] = {
	0x00000000,0x00040200,0x00080400,0x000c0600,0x00100800,0x00140a00,0x00180c00,0x001c0e00,
	0x00201000,0x00241200,0x00281400,0x002c1600,0x00301800,0x00341a00,0x00381c00,0x003c1e00,
	0x00402000,0x00442200,0x00482400,0x004c2600,0x00502800,0x00542a00,0x00582c00,0x005c2e00,
	0x00603000,0x00643200,0x00683400,0x006c3600,0x00703800,0x00743a00,0x00783c00,0x007c3e00,
	0x00804000,0x00844200,0x00884400,0x008c4600,0x00904800,0x00944a00,0x00984c00,0x009c4e00,
	0x00a05000,0x00a45200,0x00a85400,0x00ac5600,0x00b05800,0x00b45a00,0x00b85c00,0x00bc5e00,
	0x00c06000,0x00c46200,0x00c86400,0x00cc6600,0x00d06800,0x00d46a00,0x00d86c00,0x00dc6e00,
	0x00e07000,0x00e47200,0x00e87400,0x00ec7600,0x00f07800,0x00f47a00,0x00f87c00,0x00fc7e00,
	0x01008000,0x01048200,0x01088400,0x010c8600,0x01108800,0x01148a00,0x01188c00,0x011c8e00,
	0x01209000,0x01249200,0x01289400,0x012c9600,0x01309800,0x01349a00,0x01389c00,0x013c9e00,
	0x0140a000,0x0144a200,0x0148a400,0x014ca600,0x0150a800,0x0154aa00,0x0158ac00,0x015cae00,
	0x0160b000,0x0164b200,0x0168b400,0x016cb600,0x0170b800,0x0174ba00,0x0178bc00,0x017cbe00,
	0x0180c000,0x0184c200,0x0188c400,0x018cc600,0x0190c800,0x0194ca00,0x0198cc00,0x019cce00,
	0x01a0d000,0x01a4d200,0x01a8d400,0x01acd600,0x01b0d800,0x01b4da00,0x01b8dc00,0x01bcde00,
	0x01c0e000,0x01c4e200,0x01c8e400,0x01cce600,0x01d0e800,0x01d4ea00,0x01d8ec00,0x01dcee00,
	0x01e0f000,0x01e4f200,0x01e8f400,0x01ecf600,0x01f0f800,0x01f4fa00,0x01f8fc00,0x01fcfe00,
	0x02010000,0x02050200,0x02090400,0x020d0600,0x02110800,0x02150a00,0x02190c00,0x021d0e00,
	0x02211000,0x02251200,0x02291400,0x022d1600,0x02311800,0x02351a00,0x02391c00,0x023d1e00,
	0x02412000,0x02452200,0x02492400,0x024d2600,0x02512800,0x02552a00,0x02592c00,0x025d2e00,
	0x02613000,0x02653200,0x02693400,0x026d3600,0x02713800,0x02753a00,0x02793c00,0x027d3e00,
	0x02814000,0x02854200,0x02894400,0x028d4600,0x02914800,0x02954a00,0x02994c00,0x029d4e00,
	0x02a15000,0x02a55200,0x02a95400,0x02ad5600,0x02b15800,0x02b55a00,0x02b95c00,0x02bd5e00,
	0x02c16000,0x02c56200,0x02c96400,0x02cd6600,0x02d16800,0x02d56a00,0x02d96c00,0x02dd6e00,
	0x02e17000,0x02e57200,0x02e97400,0x02ed7600,0x02f17800,0x02f57a00,0x02f97c00,0x02fd7e00,
	0x03018000,0x03058200,0x03098400,0x030d8600,0x03118800,0x03158a00,0x03198c00,0x031d8e00,
	0x03219000,0x03259200,0x03299400,0x032d9600,0x03319800,0x03359a00,0x03399c00,0x033d9e00,
	0x0341a000,0x0345a200,0x0349a400,0x034da600,0x0351a800,0x0355aa00,0x0359ac00,0x035dae00,
	0x0361b000,0x0365b200,0x0369b400,0x036db600,0x0371b800,0x0375ba00,0x0379bc00,0x037dbe00,
	0x0381c000,0x0385c200,0x0389c400,0x038dc600,0x0391c800,0x0395ca00,0x0399cc00,0x039dce00,
	0x03a1d000,0x03a5d200,0x03a9d400,0x03add600,0x03b1d800,0x03b5da00,0x03b9dc00,0x03bdde00,
	0x03c1e000,0x03c5e200,0x03c9e400,0x03cde600,0x03d1e800,0x03d5ea00,0x03d9ec00,0x03ddee00,
	0x03e1f000,0x03e5f200,0x03e9f400,0x03edf600,0x03f1f800,0x03f5fa00,0x03f9fc00,0x03fdfe00
	};

void fastest_reduction_mod_103(FFPOLY *poly)
	{
	const RED103 *ptr;
	WORD32 p,q;
	union 
		{
		WORD_8 *uc;
		WORD32 *ul;
		} pp;
	int i;

	pp.ul = poly->pv;

	if(poly->pvdeg < 103 || poly->pvdeg > 255) return;
	
	ptr = &red103[255-poly->pvdeg];

	for(i=poly->pvdeg;i>=(103+4);i-=4)
		{
		/* ************************************* */
		if(pp.ul[ptr->maskpos] & ptr->mask)
			{
			pp.ul[ptr->m1] ^= ptr->l1;			
			pp.ul[ptr->m2] ^= ptr->l2;
			}

		ptr++;
		/* ************************************* */
		if(pp.ul[ptr->maskpos] & ptr->mask)
			{
			pp.ul[ptr->m1] ^= ptr->l1;			
			pp.ul[ptr->m2] ^= ptr->l2;
			}

		ptr++;
		/* ************************************* */
		if(pp.ul[ptr->maskpos] & ptr->mask)
			{
			pp.ul[ptr->m1] ^= ptr->l1;			
			pp.ul[ptr->m2] ^= ptr->l2;
			}

		ptr++;
		/* ************************************* */
		if(pp.ul[ptr->maskpos] & ptr->mask)
			{
			pp.ul[ptr->m1] ^= ptr->l1;			
			pp.ul[ptr->m2] ^= ptr->l2;
			}

		ptr++;
		/* ************************************* */
		}
	poly->pvdeg = i;

	for(i=poly->pvdeg;i>=103;i--)
		{
		if(pp.ul[ptr->maskpos] & ptr->mask)
			{
			pp.ul[ptr->m1] ^= ptr->l1;			
			pp.ul[ptr->m2] ^= ptr->l2;
			}

		ptr++;
		}

	pp.ul[7] = 0;
	pp.ul[6] = 0;
	pp.ul[5] = 0;
	pp.ul[4] = 0;
	pp.ul[3] &= 0x7f;

	p = (103 >> 3) & 0x1f;
	q = (103 & 0xf8)-1;

	do
		{
		int k = idxlup[pp.uc[p]];
		if(k != 0) 
			{
			poly->pvdeg = k+q;
			return;
			}
		
		q-=8;
		p--;
		} while(p>=0);

	poly->pvdeg = -1;
	return;
	}



#ifdef USEFASTESTMODRED103

/*
   Hardwired reduction mod X^103 + X^9 + 1 (expects deg(rs) < 206)
   
   Reverses the equations used for reduction.
   For example bit 102 depends on bit 205 and bit 196...

   By cleverly grouping bits in word32s we can gain some speed.
*/

void fastest_reduction_mod_103u(FFPOLY *rs)
	{
	WORD32 p,q;
	union 
		{
		WORD_8 *uc;
		WORD32 *ul;
		} poly;

	poly.ul = rs->pv;

	poly.ul[7] = 0;
	poly.ul[6] &= 0x3fff;	/* Cut degrees higher than 205 */

	poly.ul[3] ^= (                     (poly.ul[6] >> 7) ^ 
		           (poly.ul[6] << 2) ^ (poly.ul[5] >> 30));
	poly.ul[2] ^= ((poly.ul[6] << 25) ^ (poly.ul[5] >> 7) ^ 
		           (poly.ul[5] << 2) ^ (poly.ul[4] >> 30));
	poly.ul[1] ^= ((poly.ul[5] << 25) ^ (poly.ul[4] >> 7) ^ 
		           (poly.ul[4] << 2) ^ (poly.ul[3] >> 30));
	poly.ul[0] ^= ((poly.ul[4] << 25) ^ (poly.ul[3] >> 7) ^ 
		           ((poly.ul[3] << 2) &0xfffffe00) );

	poly.ul[6] = 0;
	poly.ul[5] = 0;
	poly.ul[4] = 0;
	poly.ul[3] &= 0x7f;	/* Cut result to deg 102 at most */
	
	// 0x00000027 0x14932474 0xb300b45f 0x4c26f14d
	// 0x00000027 0x14932474 0xb300b45f 0x4c26f14d

	/* Recomputing degree, usually cycles only once */
	p = (103 >> 3) & 0x1f;
	q = (103 & 0xf8)-1;			

	do
		{
		int k = idxlup[poly.uc[p]];
		if(k != 0) 
			{
			rs->pvdeg = k+q;
			return;
			}
		
		q-=8;
		p--;
		} while(p>=0);

	rs->pvdeg = -1;
	}

#else

void fastest_reduction_mod_103u(FFPOLY *rs)
	{
	WORD32 p,q;
	union 
		{
		WORD_8 *uc;
		WORD32 *ul;
		} poly;

	poly.ul = rs->pv;

	q = poly.ul[6];

	poly.ul[3] ^= (L103[(q >> 8)&0x3f] >> 8);
	poly.ul[3] ^= ((q & 0x00000080) ? 0x00000201 : 0);   /* 199 */

	//p = *((unsigned long *)&us[5]);
	p = (poly.us[6] << 16) | (poly.us[5]);
	p ^= L103[q & 0x7f];

	q = poly.ul[5];
	p ^= ((q & 0x80000000) ? 0x00020100 : 0);   /* 191 */
	p ^= ((q & 0x40000000) ? 0x00010080 : 0);   /* 190 */

	//*((unsigned long *)&us[5]) = p;
	poly.us[5] = (WORD16)(p & 0xffff);
	poly.us[6] = (WORD16)(p >> 16);

	poly.ul[2] = poly.ul[2] ^
		      (L103[(q >> 24)&0x3f] << 8) ^
			  (L103[(unsigned char)((q >> 16)&0xff)]) ^
			  (L103[(unsigned char)((q >>  8)&0xff)] >> 8) ^
			  ((q & 0x00000080) ? 0x00000201 : 0);   /* 167 */

	p = (poly.us[4] << 16) | (poly.us[3]);
	
	//p = *((unsigned long *)&us[3]);
	p ^= L103[q & 0x7f];

	q = poly.ul[4];

	p ^= ((q & 0x80000000) ? 0x00020100 : 0);   /* 159 */
	p ^= ((q & 0x40000000) ? 0x00010080 : 0);   /* 158 */
	//*((unsigned long *)&us[3]) = p;
	poly.us[3] = (WORD16)(p & 0xffff);
	poly.us[4] = (WORD16)(p >> 16);

	poly.ul[1] = poly.ul[1] ^
			  (L103[(q >> 24)&0x3f] << 8) ^
			  (L103[(unsigned char)((q >> 16)&0xff)]) ^
			  (L103[(unsigned char)((q >>  8)&0xff)] >> 8) ^
		      ((q & 0x00000080) ? 0x00000201 : 0);   /* 135 */

	p = (poly.us[2] << 16) | (poly.us[1]);
	//p = *((unsigned long *)&us[1]);
	p ^= L103[q & 0x7f];

	q = poly.ul[3];

	p ^= ((q & 0x80000000) ? 0x00020100 : 0);   /* 127 */
	p ^= ((q & 0x40000000) ? 0x00010080 : 0);   /* 126 */
	//*((unsigned long *)&us[1]) = p;
	poly.us[1] = (WORD16)(p & 0xffff);
	poly.us[2] = (WORD16)(p >> 16);

	poly.ul[0] = poly.ul[0] ^
			  (L103[(q >> 24)&0x3f] << 8) ^
			  (L103[(unsigned char)((q >> 16)&0xff)]) ^
			  (L103[(unsigned char)((q >>  8)&0xff)] >> 8) ^
			  ((q & 0x00000080) ? 0x00000201 : 0);   /* 103 */

	poly.ul[7] = 0;
	poly.ul[6] = 0;
	poly.ul[5] = 0;
	poly.ul[4] = 0;
	poly.ul[3] &= 0x7f;

	/* Adjust degree... */

	p = (103 >> 3) & 0x1f;			
	q = (103 & 0xf8)-1;

	do
		{
		int k = idxlup[poly.uc[p]];
		if(k != 0) 
			{
			rs->pvdeg = k+q;
			return;
			}
		
		q-=8;
		p--;
		} while(p>=0);

	rs->pvdeg = -1;
	return;
	}

#endif
