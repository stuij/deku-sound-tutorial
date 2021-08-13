// ----- Change log -----
// 10/08/04 - Called SndUpdate instead of SndMix
// ----------------------

#include "Data.h"
#include "Data/SndData.h"
#include "Gba.h"
#include "Irq.h"
#include "Misc.h"
#include "Sound.h"

// input vars, to detect wether a button was
// pressed just now, or being held down since last frame
u16 keyNew, keyOld;

int AgbMain() {
  s32 i;

  // set the waitstate settings, to speed things up a bit
  REG_WSCNT = CST_ROM0_1ST_3WAIT | CST_ROM0_2ND_1WAIT;

  // initialize interrupts
  for (i = 0; i < IRQTABLE_NUM; i++) {
    IntrTable[i] = (u32)IrqNull;
  }
  // set VBlank interrupt
  IntrTable[IRQTABLE_VBL] = (u32)SndVSync;
  REG_DISPSTAT = DSTAT_VBL_IRQ;
  REG_IE = IRQ_VBL;

  SndInit(SND_FREQ_18157);

  // sound will actually start playing here (although the buffer
  // is empty at this point, so you won't hear anything)
  REG_IME = 1;

  // initialize BG for printing text
  REG_BG0CNT = BG_CHAR(0) | BG_SCREEN(31);
  LoadBgPal16(15, dPalSys);
  FontLoad(0, 1);
  i = 0;
  Dma3(BGC0, &i, 32 / 4, DMA_MEMSET32);

  REG_DISPCNT = DCNT_MODE0 | DCNT_BG0_ENABLE;

  while (1) {
    VSync();
    // update input vars
    keyOld = keyNew;
    keyNew = REG_P1;

    // clear the screen
    i = 0;
    Dma3(BGS(31), &i, 2048 / 4, DMA_MEMSET32);

    // print the instructions
    FontPrint(0, 0, BGS(31), "Sound example");
    FontPrint(0, 1, BGS(31), "Start: play song");

    if (KEYPRESS(KEY_START)) {
      SndPlayMOD(MOD_BIT_Introtune);
    }

    // Print out some info on the screen
    FontPrint(0, 4, BGS(31), "C0.data:");
    FontPrint(8, 4, BGS(31), htoa((s32)sndChannel[0].data));
    FontPrint(0, 5, BGS(31), "C0.pos:");
    FontPrint(8, 5, BGS(31), htoa((s32)sndChannel[0].pos));
    FontPrint(0, 6, BGS(31), "C0.inc:");
    FontPrint(8, 6, BGS(31), htoa((s32)sndChannel[0].inc));
    FontPrint(0, 7, BGS(31), "C0.vol:");
    FontPrint(8, 7, BGS(31), htoa((s32)sndChannel[0].vol));
    FontPrint(0, 8, BGS(31), "C0.len:");
    FontPrint(8, 8, BGS(31), htoa((s32)sndChannel[0].length));
    FontPrint(0, 9, BGS(31), "C0.loop:");
    FontPrint(8, 9, BGS(31), htoa((s32)sndChannel[0].loopLength));

    FontPrint(0, 11, BGS(31), "mod.curOrder:");
    FontPrint(15, 11, BGS(31), htoa(sndMod.curOrder));
    FontPrint(0, 12, BGS(31), "mod.curRow:");
    FontPrint(15, 12, BGS(31), htoa(sndMod.curRow));
    FontPrint(0, 13, BGS(31), "mod.tick:");
    FontPrint(15, 13, BGS(31), htoa(sndMod.tick));
    FontPrint(0, 14, BGS(31), "mod.state:");
    FontPrint(15, 14, BGS(31), htoa(sndMod.state));
    FontPrint(0, 15, BGS(31), "smpsToTick:");
    FontPrint(15, 15, BGS(31), htoa(sndVars.samplesUntilMODTick));

    // Fill the next sound buffer, and update the music.
    // This can be done anywhere you like.
    // It takes a while if you have many channels running,
    // so it's best to do it after you're done with everything else.
    // If you think you could get missed frames, you might want to
    // do this in your VBlank function, just so you know for sure
    // it will happen every frame.

    // Wait until VBlank is over, to use BG color for profiling
    while (REG_VCOUNT != 0)
      ;
    PAL_BG[0] = RGB(7, 19, 23);
    SndUpdate();
    PAL_BG[0] = RGB(0, 7, 15);
  }

  return 0;
}
