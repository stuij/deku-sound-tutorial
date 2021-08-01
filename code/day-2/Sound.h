#ifndef SOUND_H
#define SOUND_H

#include "Gba.h"

#define SND_MAX_CHANNELS 4

typedef enum _SND_FREQ {
  SND_FREQ_5734,
  SND_FREQ_10512,
  SND_FREQ_13379,
  SND_FREQ_18157,
  SND_FREQ_21024,
  SND_FREQ_26758,
  SND_FREQ_31536,
  SND_FREQ_36314,
  SND_FREQ_40137,
  SND_FREQ_42048,
  SND_FREQ_43959,

  SND_FREQ_NUM

} SND_FREQ;

typedef struct _SOUND_CHANNEL {
  s8 *data;
  u32 pos;
  u32 inc;
  u32 vol;
  u32 length;
  u32 loopLength;

} SOUND_CHANNEL;

typedef struct _SOUND_VARS {
  s8 *mixBufferBase;
  s8 *curMixBuffer;
  u32 mixBufferSize;
  u8 activeBuffer;

} SOUND_VARS;

extern SOUND_CHANNEL sndChannel[SND_MAX_CHANNELS];
extern SOUND_VARS sndVars;

extern void SndInit(SND_FREQ freq);
extern void SndVSync() IN_IWRAM; // in Irq.c
extern void SndMix();

#endif
