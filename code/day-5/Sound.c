// ----- Change log -----
// 10/08/04 - Modified mixer to take any number of samples to mix,
//            rather than strictly a full buffer, added MOD updating
//            functions, and SndUpdate to mix sound and update MOD as
//            needed for a full frame
// ----------------------

#include "Math.h"
#include "Misc.h"
#include "Sound.h"

// ----- Constants -----

#define MOD_DEFAULT_SPEED 6
#define MOD_DEFAULT_TEMPO 125

// ----- Structures -----

// structure to store all the frequency settings we allow
typedef struct _FREQ_TABLE {
  u16 timer;
  u16 freq;
  u16 bufSize;

} FREQ_TABLE;

// ----- Local function prototypes -----

static void MODProcessRow();
static void MODPlayNote(u32 channelIdx, u32 note, u32 sample, u32 effect,
                        u32 param);
static void MODSetTempo(u32 tempo);

// ----- Variables -----

// Globals, as seen in tutorial day 2. In the tutorial, I hardcoded
// SOUND_MAX_CHANNELS to 4, but it's the same effect either way
SOUND_CHANNEL sndChannel[SND_MAX_CHANNELS];
SOUND_VARS sndVars;
MOD sndMod;

// This is the actual double buffer memory. The size is taken from
// the highest entry in the frequency table above.
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

const u16 noteFreqTable[] = {
    // Finetune 0
    2090, 2215, 2348, 2485, 2639, 2796, 2963, 3139, 3326, 3523, 3728,
    3950, // C0-B0
    4181, 4430, 4697, 4971, 5279, 5593, 5926, 6279, 6653, 7046, 7457,
    7901, // C1-B1
    8363, 8860, 9395, 9943, 10559, 11186, 11852, 12559, 13306, 14092, 14914,
    15838, // C2-B2
    16726, 17720, 18839, 19886, 21180, 22372, 23705, 25208, 26713, 28185, 29829,
    31677, // C3-B3
    33453, 35441, 37679, 39772, 42613, 44744, 47727, 50416, 53426, 56818, 59659,
    63920, // C4-B4
           // Finetune 1
    2105, 2231, 2364, 2503, 2655, 2809, 2977, 3156, 3345, 3544, 3752,
    3977, // C0-B0
    4211, 4463, 4728, 5006, 5310, 5619, 5955, 6313, 6690, 7088, 7504,
    7954, // C1-B1
    8422, 8926, 9469, 10026, 10621, 11256, 11931, 12648, 13406, 14204, 15040,
    15909, // C2-B2
    16884, 17897, 18939, 20109, 21306, 22512, 23863, 25386, 26913, 28409, 30080,
    31960, // C3-B3
    33769, 35795, 38080, 40219, 42613, 45310, 47727, 51136, 54235, 56818, 60670,
    63920, // C4-B4
           // Finetune 2
    2120, 2248, 2380, 2524, 2671, 2831, 2997, 3178, 3364, 3565, 3775,
    4003, // C0-B0
    4241, 4496, 4760, 5048, 5342, 5663, 5995, 6357, 6728, 7130, 7551,
    8007, // C1-B1
    8482, 8993, 9520, 10111, 10685, 11327, 12011, 12738, 13456, 14261, 15103,
    16051, // C2-B2
    16964, 17987, 19040, 20223, 21434, 22655, 24023, 25568, 26913, 28636, 30335,
    32248, // C3-B3
    34090, 36157, 38080, 40676, 43127, 45310, 48372, 51136, 54235, 57734, 60670,
    65082, // C4-B4
           // Finetune 3
    2135, 2262, 2399, 2542, 2691, 2849, 3023, 3201, 3389, 3593, 3808,
    4031, // C0-B0
    4271, 4525, 4798, 5084, 5382, 5699, 6046, 6403, 6779, 7187, 7616,
    8062, // C1-B1
    8543, 9062, 9596, 10169, 10781, 11399, 12093, 12829, 13558, 14375, 15232,
    16124, // C2-B2
    17127, 18170, 19244, 20338, 21563, 22799, 24186, 25752, 27117, 28867, 30594,
    32248, // C3-B3
    34418, 36525, 38489, 40676, 43127, 45891, 48372, 51877, 54235, 57734, 61716,
    65082, // C4-B4
           // Finetune 4
    2151, 2279, 2415, 2560, 2711, 2872, 3043, 3224, 3415, 3615, 3832,
    4058, // C0-B0
    4302, 4559, 4830, 5120, 5423, 5745, 6087, 6449, 6831, 7231, 7664,
    8116, // C1-B1
    8604, 9131, 9674, 10256, 10847, 11509, 12175, 12922, 13662, 14492, 15362,
    16270, // C2-B2
    17209, 18262, 19348, 20572, 21694, 23093, 24350, 25938, 27324, 29101, 30858,
    32541, // C3-B3
    34418, 36525, 38908, 41144, 43652, 46487, 49034, 51877, 55069, 58681, 61716,
    65082, // C4-B4
           // Finetune 5
    2166, 2297, 2431, 2578, 2732, 2891, 3064, 3248, 3441, 3645, 3865,
    4095, // C0-B0
    4333, 4595, 4863, 5157, 5464, 5782, 6129, 6496, 6883, 7290, 7731,
    8191, // C1-B1
    8667, 9201, 9727, 10315, 10946, 11584, 12258, 13016, 13767, 14610, 15495,
    16419, // C2-B2
    17376, 18451, 19454, 20691, 21960, 23243, 24517, 26128, 27534, 29340, 31126,
    32839, // C3-B3
    34752, 36902, 38908, 41622, 44191, 46487, 49034, 52640, 55069, 58681, 62799,
    65535, // C4-B4
           // Finetune 6
    2182, 2312, 2451, 2597, 2749, 2914, 3085, 3271, 3468, 3675, 3890,
    4123, // C0-B0
    4365, 4624, 4903, 5195, 5498, 5829, 6171, 6543, 6937, 7350, 7781,
    8247, // C1-B1
    8730, 9249, 9806, 10405, 11013, 11659, 12343, 13111, 13874, 14730, 15563,
    16495, // C2-B2
    17461, 18546, 19667, 20811, 22095, 23395, 24686, 26320, 27748, 29583, 31126,
    33143, // C3-B3
    35093, 37286, 39335, 41622, 44191, 47099, 49715, 52640, 55930, 59659, 62799,
    65535, // C4-B4
           // Finetune 7
    2198, 2330, 2468, 2616, 2770, 2934, 3112, 3296, 3488, 3697, 3916,
    4152, // C0-B0
    4397, 4660, 4937, 5233, 5541, 5868, 6225, 6592, 6977, 7395, 7832,
    8305, // C1-B1
    8794, 9321, 9888, 10466, 11082, 11736, 12472, 13208, 13982, 14791, 15699,
    16649, // C2-B2
    17633, 18643, 19776, 20933, 22233, 23549, 25031, 26515, 27965, 29583, 31399,
    33453, // C3-B3
    35441, 37286, 39772, 42112, 44744, 47099, 50416, 53426, 55930, 59659, 62799,
    65535, // C4-B4
           // Finetune 8
    1973, 2090, 2215, 2348, 2485, 2639, 2796, 2963, 3139, 3326, 3523,
    3728, // C0-B0
    3946, 4181, 4430, 4697, 4971, 5279, 5593, 5926, 6279, 6653, 7046,
    7457, // C1-B1
    7901, 8363, 8860, 9395, 9943, 10559, 11186, 11852, 12559, 13306, 14092,
    14914, // C2-B2
    15838, 16726, 17720, 18839, 19886, 21180, 22372, 23705, 25208, 26713, 28185,
    29829, // C3-B3
    31677, 33453, 35441, 37679, 39772, 42613, 44744, 47727, 50416, 53426, 56818,
    59659, // C4-B4
           // Finetune 9
    1988, 2105, 2231, 2364, 2503, 2651, 2814, 2977, 3156, 3345, 3544,
    3752, // C0-B0
    3977, 4211, 4463, 4728, 5006, 5303, 5628, 5955, 6313, 6690, 7088,
    7504, // C1-B1
    7954, 8422, 8926, 9469, 10026, 10621, 11256, 11931, 12648, 13406, 14204,
    15040, // C2-B2
    15909, 16884, 17897, 18939, 20109, 21306, 22512, 23863, 25386, 26913, 28409,
    30080, // C3-B3
    31960, 33769, 35795, 38080, 40219, 42613, 45310, 47727, 51136, 54235, 56818,
    60670, // C4-B4
           // Finetune 10
    2001, 2120, 2248, 2380, 2524, 2671, 2831, 2997, 3178, 3364, 3565,
    3775, // C0-B0
    4003, 4241, 4496, 4760, 5048, 5342, 5663, 5995, 6357, 6728, 7130,
    7551, // C1-B1
    8007, 8482, 8993, 9520, 10111, 10685, 11327, 12011, 12738, 13456, 14261,
    15103, // C2-B2
    16051, 16964, 17987, 19040, 20223, 21434, 22655, 24023, 25568, 26913, 28636,
    30335, // C3-B3
    32248, 34090, 36157, 38080, 40676, 43127, 45310, 48372, 51136, 54235, 57734,
    60670, // C4-B4
           // Finetune 11
    2017, 2135, 2262, 2399, 2542, 2691, 2849, 3023, 3201, 3389, 3593,
    3808, // C0-B0
    4035, 4271, 4525, 4798, 5084, 5382, 5699, 6046, 6403, 6779, 7187,
    7616, // C1-B1
    8080, 8543, 9062, 9596, 10169, 10781, 11399, 12093, 12829, 13558, 14375,
    15232, // C2-B2
    16197, 17127, 18170, 19244, 20338, 21563, 22799, 24186, 25752, 27117, 28867,
    30594, // C3-B3
    32541, 34418, 36525, 38489, 40676, 43127, 45891, 48372, 51877, 54235, 57734,
    61716, // C4-B4
           // Finetune 12
    2031, 2151, 2279, 2415, 2560, 2711, 2872, 3043, 3224, 3415, 3623,
    3832, // C0-B0
    4063, 4302, 4559, 4830, 5120, 5423, 5745, 6087, 6449, 6831, 7246,
    7664, // C1-B1
    8135, 8604, 9131, 9674, 10256, 10847, 11509, 12175, 12922, 13662, 14492,
    15362, // C2-B2
    16270, 17209, 18262, 19348, 20572, 21694, 23093, 24350, 25938, 27324, 29101,
    30858, // C3-B3
    32541, 34418, 36525, 38908, 41144, 43652, 46487, 49034, 51877, 55069, 58681,
    61716, // C4-B4
           // Finetune 13
    2045, 2166, 2297, 2431, 2578, 2732, 2891, 3064, 3248, 3441, 3645,
    3865, // C0-B0
    4090, 4333, 4595, 4863, 5157, 5464, 5782, 6129, 6496, 6883, 7290,
    7731, // C1-B1
    8191, 8667, 9201, 9727, 10315, 10946, 11584, 12258, 13016, 13767, 14610,
    15495, // C2-B2
    16419, 17376, 18451, 19454, 20691, 21960, 23243, 24517, 26128, 27534, 29340,
    31126, // C3-B3
    32839, 34752, 36902, 38908, 41622, 44191, 46487, 49034, 52640, 55069, 58681,
    62799, // C4-B4
           // Finetune 14
    2061, 2182, 2312, 2451, 2597, 2749, 2914, 3085, 3271, 3468, 3675,
    3890, // C0-B0
    4123, 4365, 4624, 4903, 5195, 5498, 5829, 6171, 6543, 6937, 7350,
    7781, // C1-B1
    8247, 8730, 9249, 9806, 10405, 11013, 11659, 12343, 13111, 13874, 14730,
    15563, // C2-B2
    16495, 17461, 18546, 19667, 20811, 22095, 23395, 24686, 26320, 27748, 29583,
    31126, // C3-B3
    33143, 35093, 37286, 39335, 41622, 44191, 47099, 49715, 52640, 55930, 59659,
    62799, // C4-B4
           // Finetune 15
    2076, 2198, 2330, 2468, 2616, 2770, 2934, 3112, 3296, 3488, 3697,
    3916, // C0-B0
    4152, 4397, 4660, 4937, 5233, 5541, 5868, 6225, 6592, 6977, 7395,
    7832, // C1-B1
    8305, 8794, 9321, 9888, 10466, 11082, 11736, 12472, 13208, 13982, 14791,
    15699, // C2-B2
    16649, 17633, 18643, 19776, 20933, 22233, 23549, 25031, 26515, 27965, 29583,
    31399, // C3-B3
    33453, 35441, 37286, 39772, 42112, 44744, 47099, 50416, 53426, 55930, 59659,
    62799, // C4-B4

}; // noteFreqTable

// ----- Functions -----

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
  sndVars.rcpMixFreq = div(1 << 28, sndVars.mixFreq);

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
  s32 samplesLeft = sndVars.mixBufferSize;

  while (samplesLeft > 0) {
    // Check if the song needs updated
    if (sndVars.samplesUntilMODTick == 0 && sndMod.state == MOD_STATE_PLAY) {
      // Update the song and set the number of samples to mix before the next
      // update
      MODUpdate();
      sndVars.samplesUntilMODTick = sndVars.samplesPerMODTick;
    }

    // Figure out if this is the last batch of samples for this frame
    if (sndVars.samplesUntilMODTick < samplesLeft &&
        sndMod.state == MOD_STATE_PLAY) {
      // Song will need updated before we're out of samples, so mix up to the
      // song tick
      SndMix(sndVars.samplesUntilMODTick);
      // Subtract the number we just mixed
      samplesLeft -= sndVars.samplesUntilMODTick;

      // No more left, so song will get updated next time through the loop
      sndVars.samplesUntilMODTick = 0;
    } else {
      // Either song is not playing, so just mix a full buffer, or
      // not enough samples left to make it to another song tick,
      // so mix what's left and exit
      SndMix(samplesLeft);
      // This is how many samples will get mixed first thing next frame
      // before updating the song
      sndVars.samplesUntilMODTick -= samplesLeft;
      // Mixed the last of the 304 samples, this will break from the while loop
      samplesLeft = 0;
    }
  }

} // SndUpdate

// This is only called by SndUpdate
void SndMix(u32 samplesToMix) {
  s32 i, curChn;

  // If you want to use a higher frequency than 18157,
  // you'll need to make this bigger.
  // To be safe, it would be best to set it to the buffer
  // size of the highest frequency we allow in freqTable
  s16 tempBuffer[304];

  // zero as much of the buffer as we'll actually use,
  // rounding samples up to nearest 2 for memset32
  i = 0;
  Dma3(tempBuffer, &i, (samplesToMix + 1) * sizeof(s16) / 4, DMA_MEMSET32);

  for (curChn = 0; curChn < SND_MAX_CHANNELS; curChn++) {
    SOUND_CHANNEL *chnPtr = &sndChannel[curChn];

    // check special active flag value
    if (chnPtr->data != 0) {
      // this channel is active, so mix its data into the intermediate buffer
      for (i = 0; i < samplesToMix; i++) {
        // mix a sample into the intermediate buffer
        tempBuffer[i] += chnPtr->data[chnPtr->pos >> 12] * chnPtr->vol;
        chnPtr->pos += chnPtr->inc;

        // loop the sound if it hits the end
        if (chnPtr->pos >= chnPtr->length) {
          // check special loop on/off flag value
          if (chnPtr->loopLength == 0) {
            // disable the channel and break from the i loop
            chnPtr->data = 0;
            i = samplesToMix;
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
  for (i = 0; i < samplesToMix; i++) {
    // >>6 to divide off the volume, >>2 to divide by 4 channels
    // to prevent overflow. Could make a define for this up with
    // SOUND_MAX_CHANNELS, but I'll hardcode it for now
    sndVars.curMixBuffer[i] = tempBuffer[i] >> 8;
  }

  // curMixBuffer will get reset on next VBlank anyway, so we can
  // move the pointer forward to avoid having to make a variable
  // to keep track of how many samples have been mixed so far
  sndVars.curMixBuffer += samplesToMix;

} // SndMix

void SndPlayMOD(u32 modIdx) {
  const MOD_HEADER *modHeader = &dModTable[modIdx];

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
  sndVars.samplesUntilMODTick = 0;

} // SndPlayMOD

void MODUpdate() {
  if (++sndMod.tick >= sndMod.speed) {
    sndMod.tick = 0;

    if (++sndMod.curRow >= 64) {
      sndMod.curRow = 0;

      if (++sndMod.curOrder >= sndMod.orderCount) {
        sndMod.state = MOD_STATE_STOP;
        return;
      } else {
        sndMod.rowPtr = sndMod.pattern[sndMod.order[sndMod.curOrder]];
      }
    }

    MODProcessRow();
  } else {
    // We'll update the non row-tick effects here later
  }

} // MODUpdate

static void MODProcessRow() {
  s32 curChannel;

  for (curChannel = 0; curChannel < SND_MAX_CHANNELS; curChannel++) {
    u8 note, sample, effect, param;

    // Read in the pattern data, advancing rowPtr to the next channel in the
    // process
    note = *sndMod.rowPtr++;
    sample = *sndMod.rowPtr++;
    effect = *sndMod.rowPtr++;
    param = *sndMod.rowPtr++;

    // Use sample memory if no sample, or set sample memory if there is one
    if (sample == MOD_NO_SAMPLE) {
      sample = sndMod.channel[curChannel].sample;
    } else {
      // Set sample memory
      sndMod.channel[curChannel].sample = sample;
      // Another tricky thing to know about MOD: Volume is only set when
      // specifying new samples, NOT when playing notes, and it is set even
      // when there is a sample, but no note specified (although the sample
      // playing doesn't change in that case)
      sndMod.channel[curChannel].vol = sndMod.sample[sample].vol;
      sndChannel[curChannel].vol = sndMod.channel[curChannel].vol;
    }

    // See if there's any note to play
    if (note != MOD_NO_NOTE) {
      MODPlayNote(curChannel, note, sample, effect, param);
    }
  }

} // MODProcessRow

static void MODPlayNote(u32 channelIdx, u32 note, u32 sampleIdx, u32 effect,
                        u32 param) {
  SOUND_CHANNEL *sndChn;
  MOD_CHANNEL *modChn;
  const SAMPLE_HEADER *sample;
  u8 finetune;

  // Here's that special case that they didn't specify a sample before playing a
  // note
  if (sampleIdx == MOD_NO_SAMPLE) {
    return;
  }

  // These make things less cumbersome
  sndChn = &sndChannel[channelIdx];
  modChn = &sndMod.channel[channelIdx];
  sample = &sndMod.sample[sampleIdx];

  // 60 notes total, and one full set of notes for each finetune level
  modChn->frequency = noteFreqTable[sample->finetune * 60 + note];

  // Set up the mixer channel
  sndChn->data = sample->smpData;
  sndChn->pos = 0;
  sndChn->inc = modChn->frequency * sndVars.rcpMixFreq >> 16; // Explained below

  // Next member is volume, but skip setting it because it doesn't change unless
  // a new sample was specified, in which case it was already set back in
  // MODProcessRow

  // If looping, use loopStart+loopLength as the ending point,
  // otherwise just use normal length.
  // Length and loop length are still half what the real size is
  // (to fit in 16 bits, as per MOD format), so they need to be
  // shifted left 1. They also need to be shifted left 12 because
  // the mixer compares the 12-bit fixed-point position against
  // them directly, so that comes to a total shift left of 13
  sndChn->length =
      (sample->loopLength != 0 ? sample->loopStart + sample->loopLength
                               : sample->length)
      << 13;
  sndChn->loopLength = sample->loopLength << 13;

} // MODPlayNote

static void MODSetTempo(u32 tempo) {
  u32 modFreq;

  sndMod.tempo = tempo;
  modFreq = div(tempo * 2, 5);

  sndVars.samplesUntilMODTick -= sndVars.samplesPerMODTick;
  sndVars.samplesPerMODTick = div(sndVars.mixFreq, modFreq);
  sndVars.samplesUntilMODTick += sndVars.samplesPerMODTick;

} // MODSetTempo
