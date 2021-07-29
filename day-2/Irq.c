#include "Gba.h"
#include "Irq.h"
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
