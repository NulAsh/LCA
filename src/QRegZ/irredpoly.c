
/*

License Codes Algorithms (LCA)
Code written by Giuliano Bertoletti (gbe32241@libero.it)
Copyright (C) 2003-2009 GBE 322241 Software PR

Usage of this code is subject to some restrictions, read
LICENSE.TXT for details

*/

#include "irredpoly.h"


static int irredpoly12[100][12] = {
  { 9, 12,11,8,6,5,3,2,1,0 },	/* P(00)=X^12+X^11+X^8+X^6+X^5+X^3+X^2+X+1 */
  { 7, 12,11,5,4,2,1,0 },		/* P(01)=X^12+X^11+X^5+X^4+X^2+X+1 */
  { 7, 12,11,7,6,5,4,0 },		/* P(02)=X^12+X^11+X^7+X^6+X^5+X^4+1 */
  { 9, 12,11,10,9,7,6,3,2,0 },	/* P(03)=X^12+X^11+X^10+X^9+X^7+X^6+X^3+X^2+1 */
  { 9, 12,11,8,7,6,5,3,2,0 },	/* P(04)=X^12+X^11+X^8+X^7+X^6+X^5+X^3+X^2+1 */
  { 7, 12,10,9,6,5,1,0 },		/* P(05)=X^12+X^10+X^9+X^6+X^5+X+1 */
  { 7, 12,8,5,4,3,1,0 },		/* P(06)=X^12+X^8+X^5+X^4+X^3+X+1 */
  { 7, 12,11,9,7,6,2,0 },		/* P(07)=X^12+X^11+X^9+X^7+X^6+X^2+1 */
  { 7, 12,11,9,7,6,2,0 },		/* P(08)=X^12+X^11+X^9+X^7+X^6+X^2+1 */
  { 7, 12,9,6,5,4,1,0 },		/* P(09)=X^12+X^9+X^6+X^5+X^4+X+1 */
  { 7, 12,10,7,5,4,2,0 },		/* P(10)=X^12+X^10+X^7+X^5+X^4+X^2+1 */
  { 7, 12,11,10,8,7,4,0 },		/* P(11)=X^12+X^11+X^10+X^8+X^7+X^4+1 */
  { 7, 12,10,8,7,6,2,0 },		/* P(12)=X^12+X^10+X^8+X^7+X^6+X^2+1 */
  { 9, 12,11,10,7,6,3,2,1,0 },	/* P(13)=X^12+X^11+X^10+X^7+X^6+X^3+X^2+X+1 */
  { 7, 12,11,10,7,2,1,0 },		/* P(14)=X^12+X^11+X^10+X^7+X^2+X+1 */
  { 9, 12,11,10,9,7,4,2,1,0 },	/* P(15)=X^12+X^11+X^10+X^9+X^7+X^4+X^2+X+1 */
  { 5, 12,11,10,1,0 },			/* P(16)=X^12+X^11+X^10+X+1 */
  { 7, 12,11,8,4,3,1,0 },		/* P(17)=X^12+X^11+X^8+X^4+X^3+X+1 */
  { 7, 12,10,8,7,6,2,0 },		/* P(18)=X^12+X^10+X^8+X^7+X^6+X^2+1 */
  { 9, 12,11,8,7,6,5,2,1,0 },	/* P(19)=X^12+X^11+X^8+X^7+X^6+X^5+X^2+X+1 */
  { 5, 12,9,5,2,0 },			/* P(20)=X^12+X^9+X^5+X^2+1 */
  { 9, 12,11,9,7,6,5,2,1,0 },	/* P(21)=X^12+X^11+X^9+X^7+X^6+X^5+X^2+X+1 */
  { 7, 12,9,8,7,4,3,0 },		/* P(22)=X^12+X^9+X^8+X^7+X^4+X^3+1 */
  { 5, 12,10,7,5,0 },			/* P(23)=X^12+X^10+X^7+X^5+1 */
  { 9, 12,11,10,9,8,6,5,2,0 },	/* P(24)=X^12+X^11+X^10+X^9+X^8+X^6+X^5+X^2+1 */
  { 9, 12,11,10,9,8,5,2,1,0 },	/* P(25)=X^12+X^11+X^10+X^9+X^8+X^5+X^2+X+1 */
  { 5, 12,8,7,1,0 },			/* P(26)=X^12+X^8+X^7+X+1 */
  { 7, 12,10,8,4,3,2,0 },		/* P(27)=X^12+X^10+X^8+X^4+X^3+X^2+1 */
  { 7, 12,11,9,8,3,1,0 },		/* P(28)=X^12+X^11+X^9+X^8+X^3+X+1 */
  { 5, 12,10,9,1,0 },			/* P(29)=X^12+X^10+X^9+X+1 */
  { 5, 12,4,2,1,0 },			/* P(30)=X^12+X^4+X^2+X+1 */
  { 7, 12,11,9,8,4,3,0 },		/* P(31)=X^12+X^11+X^9+X^8+X^4+X^3+1 */
  { 7, 12,11,6,4,3,2,0 },		/* P(32)=X^12+X^11+X^6+X^4+X^3+X^2+1 */
  { 11,12,11,9,8,7,5,4,3,2,1,0},/* P(33)=X^12+X^11+X^9+X^8+X^7+X^5+X^4+X^3+X^2+X+1 */
  { 7, 12,11,10,7,5,2,0 },		/* P(34)=X^12+X^11+X^10+X^7+X^5+X^2+1 */
  { 7, 12,9,7,5,4,3,0 },		/* P(35)=X^12+X^9+X^7+X^5+X^4+X^3+1 */
  { 7, 12,11,10,9,6,2,0 },		/* P(36)=X^12+X^11+X^10+X^9+X^6+X^2+1 */
  { 9, 12,11,10,7,4,3,2,1,0 },	/* P(37)=X^12+X^11+X^10+X^7+X^4+X^3+X^2+X+1 */
  { 9, 12,10,9,8,7,4,3,2,0 },	/* P(38)=X^12+X^10+X^9+X^8+X^7+X^4+X^3+X^2+1 */
  { 7, 12,11,10,8,2,1,0 },		/* P(39)=X^12+X^11+X^10+X^8+X^2+X+1 */
  { 7, 12,11,10,6,3,2,0 },		/* P(40)=X^12+X^11+X^10+X^6+X^3+X^2+1 */
  { 5, 12,6,4,1,0 },			/* P(41)=X^12+X^6+X^4+X+1 */
  { 9, 12,11,9,8,5,3,2,1,0 },	/* P(42)=X^12+X^11+X^9+X^8+X^5+X^3+X^2+X+1 */
  { 7, 12,9,6,5,2,1,0 },		/* P(43)=X^12+X^9+X^6+X^5+X^2+X+1 */
  { 7, 12,10,9,7,6,1,0 },		/* P(44)=X^12+X^10+X^9+X^7+X^6+X+1 */
  { 9, 12,10,8,7,5,4,3,1,0 },	/* P(45)=X^12+X^10+X^8+X^7+X^5+X^4+X^3+X+1 */
  { 7, 12,11,7,6,5,4,0 },		/* P(46)=X^12+X^11+X^7+X^6+X^5+X^4+1 */
  { 5, 12,8,2,1,0 },			/* P(47)=X^12+X^8+X^2+X+1 */
  { 9, 12,9,8,5,4,3,2,1,0 },	/* P(48)=X^12+X^9+X^8+X^5+X^4+X^3+X^2+X+1 */
  { 9, 12,11,10,9,8,7,4,3,0 },	/* P(49)=X^12+X^11+X^10+X^9+X^8+X^7+X^4+X^3+1 */
  { 9, 12,9,6,5,4,3,2,1,0 },	/* P(50)=X^12+X^9+X^6+X^5+X^4+X^3+X^2+X+1 */
  { 5, 12,10,2,1,0 },			/* P(51)=X^12+X^10+X^2+X+1 */
  { 7, 12,9,8,4,3,1,0 },		/* P(52)=X^12+X^9+X^8+X^4+X^3+X+1 */
  { 7, 12,9,8,6,5,4,0 },		/* P(53)=X^12+X^9+X^8+X^6+X^5+X^4+1 */
  { 7, 12,11,8,7,6,3,0 },		/* P(54)=X^12+X^11+X^8+X^7+X^6+X^3+1 */
  { 7, 12,8,7,6,5,1,0 },		/* P(55)=X^12+X^8+X^7+X^6+X^5+X+1 */
  { 7, 12,11,9,4,3,2,0 },		/* P(56)=X^12+X^11+X^9+X^4+X^3+X^2+1 */
  { 7, 12,9,8,7,6,4,0 },		/* P(57)=X^12+X^9+X^8+X^7+X^6+X^4+1 */
  { 9, 12,11,10,9,8,6,5,4,0 },	/* P(58)=X^12+X^11+X^10+X^9+X^8+X^6+X^5+X^4+1 */
  { 5, 12,9,8,5,0 },			/* P(59)=X^12+X^9+X^8+X^5+1 */
  { 9, 12,11,10,8,6,5,2,1,0 },	/* P(60)=X^12+X^11+X^10+X^8+X^6+X^5+X^2+X+1 */
  { 9, 12,11,9,6,5,4,3,1,0 },	/* P(61)=X^12+X^11+X^9+X^6+X^5+X^4+X^3+X+1 */
  { 7, 12,11,10,9,6,5,0 },		/* P(62)=X^12+X^11+X^10+X^9+X^6+X^5+1 */
  { 9, 12,11,10,8,5,3,2,1,0 },	/* P(63)=X^12+X^11+X^10+X^8+X^5+X^3+X^2+X+1 */
  { 7, 12,6,5,4,2,1,0 },		/* P(64)=X^12+X^6+X^5+X^4+X^2+X+1 */
  { 7, 12,11,10,9,8,4,0 },		/* P(65)=X^12+X^11+X^10+X^9+X^8+X^4+1 */
  { 9, 12,11,8,7,6,5,4,3,0 },	/* P(66)=X^12+X^11+X^8+X^7+X^6+X^5+X^4+X^3+1 */
  { 9, 12,11,10,8,7,5,2,1,0 },	/* P(67)=X^12+X^11+X^10+X^8+X^7+X^5+X^2+X+1 */
  { 7, 12,6,5,4,2,1,0 },		/* P(68)=X^12+X^6+X^5+X^4+X^2+X+1 */
  { 7, 12,10,8,6,3,2,0 },		/* P(69)=X^12+X^10+X^8+X^6+X^3+X^2+1 */
  { 5, 12,7,6,4,0 },			/* P(70)=X^12+X^7+X^6+X^4+1 */
  { 5, 12,6,4,1,0 },			/* P(71)=X^12+X^6+X^4+X+1 */
  { 7, 12,7,6,3,2,1,0 },		/* P(72)=X^12+X^7+X^6+X^3+X^2+X+1 */
  { 7, 12,10,8,6,3,1,0 },		/* P(73)=X^12+X^10+X^8+X^6+X^3+X+1 */
  { 7, 12,10,7,5,2,1,0 },		/* P(74)=X^12+X^10+X^7+X^5+X^2+X+1 */
  { 7, 12,11,9,8,4,3,0 },		/* P(75)=X^12+X^11+X^9+X^8+X^4+X^3+1 */
  { 7, 12,10,9,8,6,1,0 },		/* P(76)=X^12+X^10+X^9+X^8+X^6+X+1 */
  { 7, 12,11,8,5,3,2,0 },		/* P(77)=X^12+X^11+X^8+X^5+X^3+X^2+1 */
  { 5, 12,10,7,5,0 },			/* P(78)=X^12+X^10+X^7+X^5+1 */
  { 9, 12,11,10,9,7,6,5,1,0 },	/* P(79)=X^12+X^11+X^10+X^9+X^7+X^6+X^5+X+1 */
  { 9, 12,9,8,7,6,5,4,1,0 },	/* P(80)=X^12+X^9+X^8+X^7+X^6+X^5+X^4+X+1 */
  { 7, 12,11,10,9,2,1,0 },		/* P(81)=X^12+X^11+X^10+X^9+X^2+X+1 */
  { 7, 12,11,10,4,2,1,0 },		/* P(82)=X^12+X^11+X^10+X^4+X^2+X+1 */
  { 9, 12,11,10,7,6,5,3,1,0 },	/* P(83)=X^12+X^11+X^10+X^7+X^6+X^5+X^3+X+1 */
  { 5, 12,7,5,2,0 },			/* P(84)=X^12+X^7+X^5+X^2+1 */
  { 5, 12,11,2,1,0 },			/* P(85)=X^12+X^11+X^2+X+1 */
  { 7, 12,10,9,8,6,2,0 },		/* P(86)=X^12+X^10+X^9+X^8+X^6+X^2+1 */
  { 9, 12,11,9,7,6,5,2,1,0 },	/* P(87)=X^12+X^11+X^9+X^7+X^6+X^5+X^2+X+1 */
  { 7, 12,10,9,7,6,5,0 },		/* P(88)=X^12+X^10+X^9+X^7+X^6+X^5+1 */
  { 7, 12,11,9,8,7,4,0 },		/* P(89)=X^12+X^11+X^9+X^8+X^7+X^4+1 */
  { 5, 12,7,3,1,0 },			/* P(90)=X^12+X^7+X^3+X+1 */
  { 7, 12,8,7,5,4,1,0 },		/* P(91)=X^12+X^8+X^7+X^5+X^4+X+1 */
  { 5, 12,8,7,2,0 },			/* P(92)=X^12+X^8+X^7+X^2+1 */
  { 7, 12,11,10,9,6,4,0 },		/* P(93)=X^12+X^11+X^10+X^9+X^6+X^4+1 */
  { 7, 12,9,8,6,5,4,0 },		/* P(94)=X^12+X^9+X^8+X^6+X^5+X^4+1 */
  { 7, 12,8,7,6,5,1,0 },		/* P(95)=X^12+X^8+X^7+X^6+X^5+X+1 */
  { 9, 12,11,10,9,5,3,2,1,0 },	/* P(96)=X^12+X^11+X^10+X^9+X^5+X^3+X^2+X+1 */
  { 9, 12,11,8,7,6,4,3,2,0 },	/* P(97)=X^12+X^11+X^8+X^7+X^6+X^4+X^3+X^2+1 */
  { 5, 12,9,5,2,0 },			/* P(98)=X^12+X^9+X^5+X^2+1 */
  { 5, 12,9,7,6,0 }				/* P(99)=X^12+X^9+X^7+X^6+1 */
  };

int GetIrreduciblePoly(FFPOLY *fp,int deg,int id)
	{
	if(deg != 12) return -1;	
	if(id < 0 || id>=100) return -1;
	
	ffpoly_set(fp,&irredpoly12[id][1],irredpoly12[id][0]);

	return 1;
	}
