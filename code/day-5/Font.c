#include "Data.h"
#include "Gba.h"
#include "Misc.h"

static u16 fontTileStart = 0;

void FontLoad(u8 charBlock, u16 tileStart) {
  fontTileStart = tileStart;
  UnCompLZ77VRam(dFontBg, (void *)((0x6000000 + (((u32)charBlock) << 14)) +
                                   (((u32)tileStart) << 5)));
}

void FontPrint(u8 x, u8 y, u16 *dest, char *text) {
  // Compute address. Assume 32x32 tile screen.
  dest += x + (y << 5);

  while (*text) {
    *dest++ = fontTileStart + dFontMapBg[*text - 32];
    text++;
  }
}
