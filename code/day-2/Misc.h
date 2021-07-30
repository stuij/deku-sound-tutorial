#ifndef MISC_H
#define MISC_H

#include "Gba.h"

	// input stuff. keyNew and keyOld are in Main.c
#define KEY(x)			(!(keyNew & (x)))
#define KEYPRESS(x)		(!(keyNew & (x)) && (keyOld & (x)))

extern u16 keyNew, keyOld;

	// functions in Misc.c
extern inline void LoadBgPal16(u8 palIdx, const u16 *src);
extern inline void Dma3(void *dest, const void *src, u32 length, u16);
extern u8 *htoa(u32 number);

	// functions in Math.s
extern void VSync();
extern s32 UnCompLZ77VRam(const void *src, void *dest);

	// functions in Font.c
extern void FontLoad(u8 charBlock, u16 tileStart);
extern void FontPrint(u8 x, u8 y, u16 *dest, char *text);

#endif
