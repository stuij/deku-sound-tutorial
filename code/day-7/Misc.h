#ifndef MISC_H
#define MISC_H

#include "Gba.h"

// input stuff. keyNew and keyOld are in Main.c
#define KEY(x) (!(keyNew & (x)))
#define KEYPRESS(x) (!(keyNew & (x)) && (keyOld & (x)))

extern u16 keyNew, keyOld;

// functions in Misc.c
inline void Dma3(void *dest, const void *src, u32 count, u16 flags) {
  REG_DM3SAD = (u32)src;
  REG_DM3DAD = (u32)dest;
  REG_DM3CNT_L = count;
  REG_DM3CNT_H = flags;
}

inline void LoadBgPal16(u8 idx, const u16 *src) {
  Dma3((void *)(((void *)PAL_BG) + (idx << 5)), (void *)src, 16, DMA_MEMCPY16);
}

extern void memset(void *dest, u8 val, u32 size);
extern u8 *htoa(u32 number);

// functions in Math.s
extern void VSync();
extern s32 UnCompLZ77VRam(const void *src, void *dest);

// functions in Font.c
extern void FontLoad(u8 charBlock, u16 tileStart);
extern void FontPrint(u8 x, u8 y, u16 *dest, char *text);

#endif
