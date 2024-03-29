// ----- Change log -----
// 10/08/04 - Modified mixer to take any number of samples to mix,
//            rather than strictly a full buffer, added MOD updating
//            functions, and SndUpdate to mix sound and update MOD as
//            needed for a full frame
// 10/21/04 - Moved MOD_NO_NOTE/MOD_NO_SAMPLE defines here,
//            since nothing outside player needs to know them
// 05/05/05 - Converted to use periods instead of Hz.
//			  MOD timing is done fixed-point, mixer moved to IWRAM.
//			  Added SFX functions.
// ----------------------

#include "Math.h"
#include "Misc.h"
#include "Sound.h"

// ----- Constants -----

// For patterns to specify that there is no note. We have 5 octaves,
// so only notes 0-59 are used, and 63 is the highest that still fits
// in the same number of bits
#define MOD_NO_NOTE 63
// Valid samples are 0-30, use 31 as a blank that still fits in 5 bits
#define MOD_NO_SAMPLE 31

#define MOD_DEFAULT_SPEED 6   // Starting speed is 6 ticks per row
#define MOD_DEFAULT_TEMPO 125 // Starting tempo is 125 bpm
#define MOD_PATTERN_ROWS 64   // All patterns are 64 rows

#define AMIGA_VAL 3579545

typedef enum _MOD_UPDATE_FLAGS {
  // If set, play the note after effect processing
  // (if there is one), if clear, never play note
  MOD_UPD_FLG_PLAY_NOTE = BIT00,
  // If set, set the mixer channel volume to the
  // MOD channel volume after effect processing
  MOD_UPD_FLG_SET_VOL = BIT01,
  // If set, recalculate the mixer channel
  // increment after effect processing
  MOD_UPD_FLG_SET_FREQ = BIT02,

} MOD_UPDATE_FLAGS;

typedef enum _MOD_EFFECT_TABLE {
  MOD_EFFECT_TABLE_ROW,
  MOD_EFFECT_TABLE_MID,

  MOD_EFFECT_TABLE_NUM,

} MOD_EFFECT_TABLE;

// ----- Structures -----

// structure to store all the frequency settings we allow
typedef struct _FREQ_TABLE {
  u16 timer;
  u16 freq;
  u16 bufSize;

} FREQ_TABLE;

typedef struct _MOD_UPDATE_VARS {
  MOD_CHANNEL *modChn;
  SOUND_CHANNEL *sndChn;

  u8 note;
  u8 sample;
  u8 effect;
  u8 param;

  u8 updateFlags;

} MOD_UPDATE_VARS;

typedef void (*EFFECT_FUNC_PTR)(MOD_UPDATE_VARS *vars);

// ----- Local function prototypes -----

// MOD functions
static void MODPlay(MOD_HEADER const *modHeader);
static void MODStop();
static void MODProcessRow();
static void MODUpdateEffects();
static void MODPlayNote(MOD_UPDATE_VARS *vars);
static void MODSetTempo(u32 tempo);
static void MODHandleUpdateFlags(MOD_UPDATE_VARS *vars);

// Effect functions
static void MODFXSpeed(MOD_UPDATE_VARS *vars);

// ----- Variables -----

// Globals, as seen in tutorial day 2. In the tutorial, I hardcoded
// SOUND_MAX_CHANNELS to 4, but it's the same effect either way
SOUND_CHANNEL sndChannel[SND_MAX_CHANNELS];
SOUND_VARS sndVars;
MOD sndMod;

// This is the actual double buffer memory. The size is taken from
// the highest entry in the frequency table below.
s8 sndMixBuffer[736 * 2] IN_EWRAM;

// ----- Tables -----

static const FREQ_TABLE freqTable[SND_FREQ_NUM] = {
    // timer, frequency, and buffer size for frequencies
    // that time out perfectly with VBlank.
    // These are in the order of the SND_FREQ enum in Sound.h.
    {62610, 5734, 96},   {63940, 10512, 176}, {64282, 13379, 224},
    {64612, 18157, 304}, {64738, 21024, 352}, {64909, 26758, 448},
    {65004, 31536, 528}, {65074, 36314, 608}, {65118, 40137, 672},
    {65137, 42048, 704}, {65154, 43959, 736}};

// Save ourselves the trouble of initializing each member in code
static const MOD_UPDATE_VARS modDefaultVars[MOD_EFFECT_TABLE_NUM] = {
    {
        // MOD_EFFECT_TABLE_ROW
        NULL,          // modChn
        NULL,          // sndChn
        MOD_NO_NOTE,   // note
        MOD_NO_SAMPLE, // sample
        0,             // effect
        0,             // param
           // Play note if there is one, but still do nothing if there isn't.
           // Don't set volume or frequency unless something specifically needs
           // it.
        MOD_UPD_FLG_PLAY_NOTE // updateFlags
    },
    {
        // MOD_EFFECT_TABLE_MID
        NULL,          // modChn
        NULL,          // sndChn
        MOD_NO_NOTE,   // note
        MOD_NO_SAMPLE, // sample
        0,             // effect
        0,             // param
           // Don't do anything unless something specifically needs it
        0 // updateFlags
    }};

static const EFFECT_FUNC_PTR modEffectTable[MOD_EFFECT_TABLE_NUM][16] = {
    {
        // Row-tick updates
        NULL,      // 0x0: Arpeggio
        NULL,      // 0x1: Porta up
        NULL,      // 0x2: Porta down
        NULL,      // 0x3: Tone porta
        NULL,      // 0x4: Vibrato
        NULL,      // 0x5: Volslide+Tone porta
        NULL,      // 0x6: Volslide+Vibrato
        NULL,      // 0x7: Tremolo
        NULL,      // 0x8: Set panning
        NULL,      // 0x9: Sample offset
        NULL,      // 0xA: Volume slide
        NULL,      // 0xB: Jump to order
        NULL,      // 0xC: Set volume
        NULL,      // 0xD: Break to row
        NULL,      // 0xE: Special
        MODFXSpeed // 0xF: Speed/Tempo
    },
    {
        // Non row-tick updates
        NULL, // 0x0: Arpeggio
        NULL, // 0x1: Porta up
        NULL, // 0x2: Porta down
        NULL, // 0x3: Tone porta
        NULL, // 0x4: Vibrato
        NULL, // 0x5: VolSlide+Tone porta
        NULL, // 0x6: VolSlide+Vibrato
        NULL, // 0x7: Tremolo
        NULL, // 0x8: Set panning
        NULL, // 0x9: Sample offset
        NULL, // 0xA: Volume slide
        NULL, // 0xB: Jump to order
        NULL, // 0xC: Set volume
        NULL, // 0xD: Break to row
        NULL, // 0xE: Special
        NULL  // 0xF: Speed/Tempo
    }};

static const u16 notePeriodTable[] = {
    // Finetune 0
    1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016, 960,
    906,                                                        // C0-B0
    856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453, // C1-B1
    428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226, // C2-B2
    214, 202, 190, 180, 169, 160, 151, 142, 134, 127, 120, 113, // C3-B3
    107, 101, 95, 90, 84, 80, 75, 71, 67, 63, 60, 56,           // C4-B4
                                                      // Finetune 1
    1700, 1604, 1514, 1430, 1348, 1274, 1202, 1134, 1070, 1010, 954,
    900,                                                        // C0-B0
    850, 802, 757, 715, 674, 637, 601, 567, 535, 505, 477, 450, // C1-B1
    425, 401, 378, 357, 337, 318, 300, 283, 267, 252, 238, 225, // C2-B2
    212, 200, 189, 178, 168, 159, 150, 141, 133, 126, 119, 112, // C3-B3
    106, 100, 94, 89, 84, 79, 75, 70, 66, 63, 59, 56,           // C4-B4
                                                      // Finetune 2
    1688, 1592, 1504, 1418, 1340, 1264, 1194, 1126, 1064, 1004, 948,
    894,                                                        // C0-B0
    844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474, 447, // C1-B1
    422, 398, 376, 354, 335, 316, 298, 281, 266, 251, 237, 223, // C2-B2
    211, 199, 188, 177, 167, 158, 149, 140, 133, 125, 118, 111, // C3-B3
    105, 99, 94, 88, 83, 79, 74, 70, 66, 62, 59, 55,            // C4-B4
                                                     // Finetune 3
    1676, 1582, 1492, 1408, 1330, 1256, 1184, 1118, 1056, 996, 940,
    888,                                                        // C0-B0
    838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470, 444, // C1-B1
    419, 395, 373, 352, 332, 314, 296, 279, 264, 249, 235, 222, // C2-B2
    209, 197, 186, 176, 166, 157, 148, 139, 132, 124, 117, 111, // C3-B3
    104, 98, 93, 88, 83, 78, 74, 69, 66, 62, 58, 55,            // C4-B4
                                                     // Finetune 4
    1664, 1570, 1482, 1398, 1320, 1246, 1176, 1110, 1048, 990, 934,
    882,                                                        // C0-B0
    832, 785, 741, 699, 660, 623, 588, 555, 524, 495, 467, 441, // C1-B1
    416, 392, 370, 349, 330, 311, 294, 277, 262, 247, 233, 220, // C2-B2
    208, 196, 185, 174, 165, 155, 147, 138, 131, 123, 116, 110, // C3-B3
    104, 98, 92, 87, 82, 77, 73, 69, 65, 61, 58, 55,            // C4-B4
                                                     // Finetune 5
    1652, 1558, 1472, 1388, 1310, 1238, 1168, 1102, 1040, 982, 926,
    874,                                                        // C0-B0
    826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463, 437, // C1-B1
    413, 389, 368, 347, 327, 309, 292, 275, 260, 245, 231, 218, // C2-B2
    206, 194, 184, 173, 163, 154, 146, 137, 130, 122, 115, 109, // C3-B3
    103, 97, 92, 86, 81, 77, 73, 68, 65, 61, 57, 54,            // C4-B4
                                                     // Finetune 6
    1640, 1548, 1460, 1378, 1302, 1228, 1160, 1094, 1032, 974, 920,
    868,                                                        // C0-B0
    820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460, 434, // C1-B1
    410, 387, 365, 344, 325, 307, 290, 273, 258, 243, 230, 217, // C2-B2
    205, 193, 182, 172, 162, 153, 145, 136, 129, 121, 115, 108, // C3-B3
    102, 96, 91, 86, 81, 76, 72, 68, 64, 60, 57, 54,            // C4-B4
                                                     // Finetune 7
    1628, 1536, 1450, 1368, 1292, 1220, 1150, 1086, 1026, 968, 914,
    862,                                                        // C0-B0
    814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457, 431, // C1-B1
    407, 384, 362, 342, 323, 305, 287, 271, 256, 242, 228, 215, // C2-B2
    203, 192, 181, 171, 161, 152, 143, 135, 128, 121, 114, 107, // C3-B3
    101, 96, 90, 85, 80, 76, 71, 67, 64, 60, 57, 53,            // C4-B4
                                                     // Finetune 8
    1814, 1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016,
    960,                                                        // C0-B0
    907, 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, // C1-B1
    453, 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, // C2-B2
    226, 214, 202, 190, 180, 169, 160, 151, 142, 134, 127, 120, // C3-B3
    113, 107, 101, 95, 90, 84, 80, 75, 71, 67, 63, 60,          // C4-B4
                                                       // Finetune 9
    1800, 1700, 1604, 1514, 1430, 1350, 1272, 1202, 1134, 1070, 1010,
    954,                                                        // C0-B0
    900, 850, 802, 757, 715, 675, 636, 601, 567, 535, 505, 477, // C1-B1
    450, 425, 401, 378, 357, 337, 318, 300, 283, 267, 252, 238, // C2-B2
    225, 212, 200, 189, 178, 168, 159, 150, 141, 133, 126, 119, // C3-B3
    112, 106, 100, 94, 89, 84, 79, 75, 70, 66, 63, 59,          // C4-B4
                                                       // Finetune 10
    1788, 1688, 1592, 1504, 1418, 1340, 1264, 1194, 1126, 1064, 1004,
    948,                                                        // C0-B0
    894, 844, 796, 752, 709, 670, 632, 597, 563, 532, 502, 474, // C1-B1
    447, 422, 398, 376, 354, 335, 316, 298, 281, 266, 251, 237, // C2-B2
    223, 211, 199, 188, 177, 167, 158, 149, 140, 133, 125, 118, // C3-B3
    111, 105, 99, 94, 88, 83, 79, 74, 70, 66, 62, 59,           // C4-B4
                                                      // Finetune 11
    1774, 1676, 1582, 1492, 1408, 1330, 1256, 1184, 1118, 1056, 996,
    940,                                                        // C0-B0
    887, 838, 791, 746, 704, 665, 628, 592, 559, 528, 498, 470, // C1-B1
    443, 419, 395, 373, 352, 332, 314, 296, 279, 264, 249, 235, // C2-B2
    221, 209, 197, 186, 176, 166, 157, 148, 139, 132, 124, 117, // C3-B3
    110, 104, 98, 93, 88, 83, 78, 74, 69, 66, 62, 58,           // C4-B4
                                                      // Finetune 12
    1762, 1664, 1570, 1482, 1398, 1320, 1246, 1176, 1110, 1048, 988,
    934,                                                        // C0-B0
    881, 832, 785, 741, 699, 660, 623, 588, 555, 524, 494, 467, // C1-B1
    440, 416, 392, 370, 349, 330, 311, 294, 277, 262, 247, 233, // C2-B2
    220, 208, 196, 185, 174, 165, 155, 147, 138, 131, 123, 116, // C3-B3
    110, 104, 98, 92, 87, 82, 77, 73, 69, 65, 61, 58,           // C4-B4
                                                      // Finetune 13
    1750, 1652, 1558, 1472, 1388, 1310, 1238, 1168, 1102, 1040, 982,
    926,                                                        // C0-B0
    875, 826, 779, 736, 694, 655, 619, 584, 551, 520, 491, 463, // C1-B1
    437, 413, 389, 368, 347, 327, 309, 292, 275, 260, 245, 231, // C2-B2
    218, 206, 194, 184, 173, 163, 154, 146, 137, 130, 122, 115, // C3-B3
    109, 103, 97, 92, 86, 81, 77, 73, 68, 65, 61, 57,           // C4-B4
                                                      // Finetune 14
    1736, 1640, 1548, 1460, 1378, 1302, 1228, 1160, 1094, 1032, 974,
    920,                                                        // C0-B0
    868, 820, 774, 730, 689, 651, 614, 580, 547, 516, 487, 460, // C1-B1
    434, 410, 387, 365, 344, 325, 307, 290, 273, 258, 243, 230, // C2-B2
    217, 205, 193, 182, 172, 162, 153, 145, 136, 129, 121, 115, // C3-B3
    108, 102, 96, 91, 86, 81, 76, 72, 68, 64, 60, 57,           // C4-B4
                                                      // Finetune 15
    1724, 1628, 1536, 1450, 1368, 1292, 1220, 1150, 1086, 1026, 968,
    914,                                                        // C0-B0
    862, 814, 768, 725, 684, 646, 610, 575, 543, 513, 484, 457, // C1-B1
    431, 407, 384, 362, 342, 323, 305, 287, 271, 256, 242, 228, // C2-B2
    215, 203, 192, 181, 171, 161, 152, 143, 135, 128, 121, 114, // C3-B3
    107, 101, 96, 90, 85, 80, 76, 71, 67, 64, 60, 57,           // C4-B4

}; // notePeriodTable

// ----- Functions -----

// --- Sound functions ---

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

  sndVars.mixFreq = freqTable[freq].freq;
  sndVars.mixFreqPeriod = div(AMIGA_VAL << 9, sndVars.mixFreq)
                          << 3; // Total 12 bits to the left

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
void SndUpdate() {
  s32 i;
  s32 samplesLeft = sndVars.mixBufferSize;
  void (*_SndMix)(u32) = SndMix;

  while (samplesLeft > 0) {
    // Check if the song needs updated
    if (sndVars.samplesUntilMODTick < (1 << 12) &&
        sndMod.state == MOD_STATE_PLAY) {
      // Update the song and set the number of samples to mix before the next
      // update
      MODUpdate();
      sndVars.samplesUntilMODTick += sndVars.samplesPerMODTick;
    }

    // Figure out if this is the last batch of samples for this frame
    if ((sndVars.samplesUntilMODTick >> 12) < samplesLeft &&
        sndMod.state == MOD_STATE_PLAY) {
      // Song will need updated before we're out of samples, so mix up to the
      // song tick
      _SndMix(sndVars.samplesUntilMODTick >> 12);
      // Subtract the number we just mixed
      samplesLeft -= sndVars.samplesUntilMODTick >> 12;

      // No more left, so song will get updated next time through the loop
      sndVars.samplesUntilMODTick -= (sndVars.samplesUntilMODTick >> 12) << 12;
    } else {
      // Either song is not playing, so just mix a full buffer, or
      // not enough samples left to make it to another song tick,
      // so mix what's left and exit
      _SndMix(samplesLeft);
      // This is how many samples will get mixed first thing next frame
      // before updating the song
      sndVars.samplesUntilMODTick -= samplesLeft << 12;
      // Mixed the last of the 304 samples, this will break from the while loop
      samplesLeft = 0;
    }
  }

  // Free up SFX channels that ended
  for (i = 0; i < SND_MAX_CHANNELS; i++) {
    if ((sndVars.channelBlocked & (1 << i)) && sndChannel[i].data == NULL) {
      sndVars.channelBlocked &= ~(1 << i);
    }
  }

} // SndUpdate

// --- Global functions for game to control music ---

void SndPlayMOD(u32 modIdx) {
  MODStop();
  MODPlay(&dModTable[modIdx]);
}

void SndStopMOD() { MODStop(); }

void SndPauseMOD() {
  if (sndMod.state == MOD_STATE_PLAY)
    sndMod.state = MOD_STATE_PAUSE;
}

void SndUnpauseMOD() {
  if (sndMod.state == MOD_STATE_PAUSE)
    sndMod.state = MOD_STATE_PLAY;
}

// Returns the channel the sound is playing on (or SND_CHN_INVALID
// if none are available)
u32 SndPlaySFX(u32 sfxIdx) {
  s32 channel = SND_MAX_CHANNELS - 1;

  while ((channel >= 0) && (sndVars.channelBlocked & (1 << channel)))
    channel--;

  if (channel >= 0) {
    SndPlaySFXChn(sfxIdx, channel);
    return channel;
  } else // No free channels, decremented until it went below 0
  {
    return SND_CHN_INVALID;
  }
}

void SndPlaySFXChn(u32 sfxIdx, u32 channel) {
  const SAMPLE_HEADER *sfxHeader = &dSfxTable[sfxIdx];
  SOUND_CHANNEL *sndChn = &sndChannel[channel];

  sndVars.channelBlocked |= (1 << channel);
  sndChn->data = NULL;
  sndChn->pos = 0;
  sndChn->inc = div(sndVars.mixFreqPeriod, 428);
  sndChn->length = sfxHeader->length << 13;
  sndChn->loopLength = sfxHeader->loopLength << 13;
  sndChn->vol = sfxHeader->vol;
  sndChn->data = sfxHeader->smpData;
}

void SndStopSFX(u32 channel) {
  if (sndVars.channelBlocked & (1 << channel)) {
    sndChannel[channel].data = NULL;
    sndVars.channelBlocked &= ~(1 << channel);
  }
}

void SndStopAllSFX() {
  s32 i;

  for (i = 0; i < SND_MAX_CHANNELS; i++) {
    if (sndVars.channelBlocked & (1 << i))
      SndStopSFX(i);
  }
}

// --- MOD functions ---

void MODPlay(MOD_HEADER const *modHeader) {
  memset(&sndMod, 0, sizeof(MOD));

  sndMod.sample = modHeader->sample;
  sndMod.pattern = modHeader->pattern;
  sndMod.order = modHeader->order;
  sndMod.orderCount = modHeader->orderCount;
  sndMod.curOrder = 0;
  sndMod.curRow = 0;
  sndMod.tick = 0;
  sndMod.speed = MOD_DEFAULT_SPEED;
  sndMod.rowPtr = sndMod.pattern[sndMod.order[0]];
  sndMod.state = MOD_STATE_PLAY;

  // Set to default
  MODSetTempo(MOD_DEFAULT_TEMPO);
  // Update the MOD first thing next time the mixer is called
  sndVars.samplesUntilMODTick = 0;

} // MODPlay

void MODStop() {
  s32 i;

  for (i = 0; i < SND_MAX_CHANNELS; i++)
    sndChannel[i].data = NULL;

  sndMod.state = MOD_STATE_STOP;

} // MODStop

void MODUpdate() {
  if (sndMod.state != MOD_STATE_PLAY)
    return;

  if (++sndMod.tick >= sndMod.speed) {
    sndMod.tick = 0;

    if (++sndMod.curRow >= MOD_PATTERN_ROWS) {
      sndMod.curRow = 0;

      if (++sndMod.curOrder >= sndMod.orderCount) {
        s32 i;

        for (i = 0; i < SND_MAX_CHANNELS; i++)
          sndChannel[i].data = NULL;

        sndMod.state = MOD_STATE_STOP;

        return;
      } else {
        sndMod.rowPtr = sndMod.pattern[sndMod.order[sndMod.curOrder]];
      }
    }

    MODProcessRow();
  } else {
    MODUpdateEffects();
  }

} // MODUpdate

static void MODProcessRow() {
  s32 curChannel;

  for (curChannel = 0; curChannel < SND_MAX_CHANNELS; curChannel++) {
    // Quick initialization, with values for row-tick
    MOD_UPDATE_VARS vars = modDefaultVars[MOD_EFFECT_TABLE_ROW];

    vars.modChn = &sndMod.channel[curChannel];
    vars.sndChn = &sndChannel[curChannel];

    // Read in the pattern data, advancing rowPtr to the next channel in the
    // process
    vars.note = *sndMod.rowPtr++;
    vars.sample = *sndMod.rowPtr++;
    vars.effect = *sndMod.rowPtr++;
    vars.param = *sndMod.rowPtr++;

    // Set these for the mid-ticks
    vars.modChn->effect = vars.effect;
    vars.modChn->param = vars.param;

    // Set sample and volume BEFORE effect processing, because some
    // effects read the sample from the MOD channel rather than vars,
    // and others need to override the default volume
    if (vars.sample !=
        MOD_NO_SAMPLE) // Never set local to memory anymore (explained below)
    {
      // Set sample memory
      vars.modChn->sample = vars.sample;
      vars.modChn->vol = sndMod.sample[vars.sample].vol;
      // Don't set mixer channel volume until after effect processing
      // vars.sndChn->vol          = vars.modChn->vol;
      vars.updateFlags |= MOD_UPD_FLG_SET_VOL;
    }

    // Effect 0 is arpeggio, but is also used as no-effect if the param is 0 too
    if ((vars.effect != 0 || vars.param != 0) &&
        (modEffectTable[MOD_EFFECT_TABLE_ROW][vars.effect] != NULL))
      (*modEffectTable[MOD_EFFECT_TABLE_ROW][vars.effect])(&vars);

    if (!(sndVars.channelBlocked & (1 << curChannel)))
      MODHandleUpdateFlags(&vars);
  }

} // MODProcessRow

static void MODUpdateEffects() {
  s32 curChannel;

  for (curChannel = 0; curChannel < SND_MAX_CHANNELS; curChannel++) {
    // Bail if there's no effect to update
    if (sndMod.channel[curChannel].effect != 0 ||
        sndMod.channel[curChannel].param != 0) {
      // Initialize with mid-tick values now
      MOD_UPDATE_VARS vars = modDefaultVars[MOD_EFFECT_TABLE_MID];

      vars.modChn = &sndMod.channel[curChannel];
      vars.sndChn = &sndChannel[curChannel];

      // Already made sure there was an effect, so just check function.
      // Notice that we're using the mid-tick table now.
      if (modEffectTable[MOD_EFFECT_TABLE_MID][vars.modChn->effect] != NULL)
        (*modEffectTable[MOD_EFFECT_TABLE_MID][vars.modChn->effect])(&vars);

      if (!(sndVars.channelBlocked & (1 << curChannel)))
        MODHandleUpdateFlags(&vars);
    }
  }

} // MODUpdateEffects

static void MODHandleUpdateFlags(MOD_UPDATE_VARS *vars) {
  if ((vars->note != MOD_NO_NOTE) &&
      (vars->updateFlags & MOD_UPD_FLG_PLAY_NOTE))
    MODPlayNote(vars);

  if (vars->updateFlags & MOD_UPD_FLG_SET_VOL)
    vars->sndChn->vol = vars->modChn->vol;

  // mixFreqPeroid is already shifted up 12 bits for fixed-point
  if (vars->updateFlags & MOD_UPD_FLG_SET_FREQ)
    vars->sndChn->inc = div(sndVars.mixFreqPeriod, vars->modChn->period);

} // MODHandleUpdateFlags

static void MODPlayNote(MOD_UPDATE_VARS *vars) {
  const SAMPLE_HEADER *sample;

  // Here's that special case that they didn't specify a sample before playing a
  // note
  if (vars->modChn->sample == MOD_NO_SAMPLE) {
    return;
  }

  // Handy
  sample = &sndMod.sample[vars->modChn->sample];

  // 60 notes total, and one full set of notes for each finetune level
  vars->modChn->period = notePeriodTable[sample->finetune * 60 + vars->note];

  // Set up the mixer channel
  vars->sndChn->data = sample->smpData;
  vars->sndChn->pos = 0;
  // Let update flags take care of setting the inc, just to recycle code
  // because it may also need to be set by effects without playing a note
  // vars->sndChn->inc        = vars->modChn->frequency * sndVars.rcpMixFreq >>
  // 16;
  vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;

  // Next sndChn member is volume, but skip setting it because
  // it doesn't change unless a new sample was specified, in
  // which case it was already set back in MODProcessRow

  // If looping, use loopStart+loopLength as the ending point,
  // otherwise just use normal length.
  // Length and loop length are still half what the real size is
  // (to fit in 16 bits, as per MOD format), so they need to be
  // shifted left 1. They also need to be shifted left 12 because
  // the mixer compares the 12-bit fixed-point position against
  // them directly, so that comes to a total shift left of 13
  vars->sndChn->length =
      (sample->loopLength != 0 ? sample->loopStart + sample->loopLength
                               : sample->length)
      << 13;
  vars->sndChn->loopLength = sample->loopLength << 13;

} // MODPlayNote

static void MODSetTempo(u32 tempo) {
  sndMod.tempo = tempo;

  sndVars.samplesUntilMODTick -= sndVars.samplesPerMODTick;
  sndVars.samplesPerMODTick = div(sndVars.mixFreq * 5 << 12, tempo * 2);
  sndVars.samplesUntilMODTick += sndVars.samplesPerMODTick;

} // MODSetTempo

// --- MOD effect functions ---

static void MODFXSpeed(MOD_UPDATE_VARS *vars) {
  if (vars->param < 32) // 0-31 = set speed
    sndMod.speed = vars->param;
  else // 32-255 = set tempo
    MODSetTempo(vars->param);
}
