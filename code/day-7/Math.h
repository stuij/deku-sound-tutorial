#ifndef MATH_H
#define MATH_H

#include "Gba.h"

typedef s16				f8_8;
typedef s32				f16_16;

#define INT_TO_F88(x)	((x) << 8)
#define F88_TO_INT(x)	((x) >> 8)
#define INT_TO_F1616(x)	((x) << 16)
#define F1616_TO_INT(x)	((x) >> 16)
#define abs(x)			((x) < 0 ? -(x) : (x))
#define min(x, y)		((x) < (y) ? (x) : (y))
#define max(x, y)		((x) > (y) ? (x) : (y))

extern u32 sqrt(u32 num);
extern s32 div(s32 numer, s32 denom);
extern s32 divI(s32 numer, s32 denom);
extern s32 mod(s32 numer, s32 denom);
extern s32 fpMul(s32 m1, s32 m2);
extern s32 fpDiv(s32 numer, s32 denom);

#endif
