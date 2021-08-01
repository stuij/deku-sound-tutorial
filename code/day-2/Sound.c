#include "Sound.h"
#include "Gba.h"

// structure to store all the frequency settings we allow
typedef struct _FREQ_TABLE {
  u16 timer;
  u16 freq;
  u16 bufSize;

} FREQ_TABLE;

static const FREQ_TABLE freqTable[SND_FREQ_NUM] = {
  // timer, frequency, and buffer size for frequencies
  // that time out perfectly with VBlank.
  // These are in the order of the SND_FREQ enum in Sound.h.
  {62610, 5734, 96},   {63940, 10512, 176}, {64282, 13379, 224},
  {64612, 18157, 304}, {64738, 21024, 352}, {64909, 26758, 448},
  {65004, 31536, 528}, {65073, 36314, 608}, {65118, 40137, 672},
  {65137, 42048, 704}, {65154, 43959, 736}
};

// Globals, as seen in tutorial day 2. In the tutorial, I hardcoded
// SOUND_MAX_CHANNELS to 4, but it's the same ffect either way
SOUND_CHANNEL sndChannel[SND_MAX_CHANNELS];
SOUND_VARS sndVars;

// This is the actual double buffer memory. The size is taken from
// the highest entry in the frequency table above.
s8 sndMixBuffer[736 * 2] IN_EWRAM;

// Call this once at startup
void SndInit(SND_FREQ freq) {
  s32 i;

  // enable sound
  REG_SGCNT0_H = SOUNDA_LOUT | SOUNDA_ROUT | SOUNDA_FIFORESET | SOUNDA_VOL_100;
  REG_SGCNT1 = SOUND_ENABLE;

  // clear the whole buffer area
  i = 0;
  Dma3(sndMixBuffer, &i, 736 * 2 / 4, DMA_MEMSET32);

  // initialize main sound variables
  sndVars.mixBufferSize = freqTable[freq].bufSize;
  sndVars.mixBufferBase = sndMixBuffer;
  sndVars.curMixBuffer = sndVars.mixBufferBase;
  sndVars.activeBuffer = 1; // 1 so first swap will start DMA

  // initialize channel structures
  for (i = 0; i < SND_MAX_CHANNELS; i++) {
    sndChannel[i].data = 0;
    sndChannel[i].pos = 0;
    sndChannel[i].inc = 0;
    sndChannel[i].vol = 0;
    sndChannel[i].length = 0;
    sndChannel[i].loopLength = 0;
  }

  // start up the timer we will be using
  REG_TM0D = freqTable[freq].timer;
  REG_TM0CNT = TIMER_ENABLE;

  // set up the DMA settings, but let the VBlank interrupt
  // actually start it up, so the timing is right
  REG_DM1CNT = 0;
  REG_DM1DAD = (u32)&REG_SGFIFOA;

} // SndInit

// Call this every frame to fill the buffer. It can be
// called anywhere as long as it happens once per frame.
void SndMix() {
  s32 i, curChn;

  // If you want to use a higher frequency than 18157,
  // you'll need to make this bigger.
  // To be safe, it would be best to set it to the buffer
  // size of the highest frequency we allow in freqTable
  s16 tempBuffer[304];

  // zero the buffer
  i = 0;
  Dma3(tempBuffer, &i, sndVars.mixBufferSize * sizeof(s16) / 4, DMA_MEMSET32);

  for (curChn = 0; curChn < SND_MAX_CHANNELS; curChn++) {
    SOUND_CHANNEL *chnPtr = &sndChannel[curChn];

    // check special active flag value
    if (chnPtr->data != 0) {
      // this channel is active, so mix its data into the intermediate buffer
      for (i = 0; i < sndVars.mixBufferSize; i++) {
        // mix a sample into the intermediate buffer
        tempBuffer[i] += chnPtr->data[chnPtr->pos >> 12] * chnPtr->vol;
        chnPtr->pos += chnPtr->inc;

        // loop the sound if it hits the end
        if (chnPtr->pos >= chnPtr->length) {
          // check special loop on/off flag value
          if (chnPtr->loopLength == 0) {
            // disable the channel and break from the i loop
            chnPtr->data = 0;
            i = sndVars.mixBufferSize;
          } else {
            // loop back
            while (chnPtr->pos >= chnPtr->length) {
              chnPtr->pos -= chnPtr->loopLength;
            }
          }
        }
      } // end for i = 0 to bufSize
    }   // end data != 0
  }     // end channel loop

  // now downsample the 16-bit buffer and copy it into the actual playing buffer
  for (i = 0; i < sndVars.mixBufferSize; i++) {
    // >>6 to divide off the volume, >>2 to divide by 4 channels
    // to prevent overflow. Could make a define for this up with
    // SOUND_MAX_CHANNELS, but I'll hardcode it for now
    sndVars.curMixBuffer[i] = tempBuffer[i] >> 8;
  }

} // SndMix
