#include "Gba.h"
#include "Irq.h"
#include "Misc.h"
#include "Sound.h"

	// used by Crt0.S
u32 IntrTable[IRQTABLE_NUM];

void IrqNull()
{
}

void SndVSync()
{
	if(sndVars.activeBuffer == 1)	// buffer 1 just got over
	{
			// Start playing buffer 0
		REG_DM1CNT = 0;
		REG_DM1SAD = (u32)sndVars.mixBufferBase;
		REG_DM1CNT_H =	DMA_DEST_FIXED | DMA_REPEAT | DMA_WORD | 
						DMA_MODE_FIFO | DMA_ENABLE;

			// Set the current buffer pointer to the start of buffer 1
		sndVars.curMixBuffer = sndVars.mixBufferBase + sndVars.mixBufferSize;
		sndVars.activeBuffer = 0;
	}
	else	// buffer 0 just got over
	{
			// DMA points to buffer 1 already, so don't bother stopping and resetting it

			// Set the current buffer pointer to the start of buffer 0
		sndVars.curMixBuffer = sndVars.mixBufferBase;
		sndVars.activeBuffer = 1;
	}
}

	// This is only called by SndUpdate
void SndMix(u32 samplesToMix)
{
	s32 i, curChn;

		// Largest buffer size grabbed from freqTable in Sound.c
	s16 tempBuffer[736];

		// zero as much of the buffer as we'll actually use, 
		// rounding samples up to nearest 2 for memset32
	i = 0;
	REG_DM3DAD = (u32)tempBuffer;
	REG_DM3SAD = (u32)&i;
	REG_DM3CNT = ((samplesToMix+1)*sizeof(s16)/4) | (DMA_MEMSET32 << 16);

	for(curChn = 0; curChn < SND_MAX_CHANNELS; curChn++)
	{
		SOUND_CHANNEL *chnPtr = &sndChannel[curChn];

			// check special active flag value
		if(chnPtr->data != 0)
		{
				// this channel is active, so mix its data into the intermediate buffer
			for(i = 0; i < samplesToMix; i++)
			{
					// mix a sample into the intermediate buffer
				tempBuffer[i] += chnPtr->data[ chnPtr->pos>>12 ] * chnPtr->vol;
				chnPtr->pos += chnPtr->inc;

					// loop the sound if it hits the end
				if(chnPtr->pos >= chnPtr->length)
				{
						// check special loop on/off flag value
					if(chnPtr->loopLength == 0)
					{
							// disable the channel and break from the i loop
						chnPtr->data = 0;
						i = samplesToMix;
					}
					else
					{
							// loop back
						while(chnPtr->pos >= chnPtr->length)
						{
							chnPtr->pos -= chnPtr->loopLength;
						}
					}
				}
			}	// end for i = 0 to bufSize
		}	// end data != 0
	}	// end channel loop

		// now downsample the 16-bit buffer and copy it into the actual playing buffer
	for(i = 0; i < samplesToMix; i++)
	{
			// >>6 to divide off the volume, >>2 to divide by 4 channels 
			// to prevent overflow. Could make a define for this up with 
			// SOUND_MAX_CHANNELS, but I'll hardcode it for now
		sndVars.curMixBuffer[i] = tempBuffer[i] >> 8;
	}

		// curMixBuffer will get reset on next VBlank anyway, so we can 
		// move the pointer forward to avoid having to make a variable 
		// to keep track of how many samples have been mixed so far
	sndVars.curMixBuffer += samplesToMix;

}	// SndMix
