#include "Gba.h"
#include "Data.h"
#include "Irq.h"
#include "Misc.h"
#include "Sound.h"

	// input vars, to detect wether a button was 
	// pressed just now, or being held down since last frame
u16 keyNew, keyOld;

int AgbMain()
{
	s32 i;

		// set the waitstate settings, to speed things up a bit
	REG_WSCNT = CST_ROM0_1ST_3WAIT | CST_ROM0_2ND_1WAIT;

		// initialize interrupts
	for(i = 0; i < IRQTABLE_NUM; i++)
	{
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
	FontLoad(0, 0);

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0_ENABLE;

	while(1)
	{
		VSync();
			// update input vars
		keyOld = keyNew;
		keyNew = REG_P1;

			// clear the screen
		i = 0;
		Dma3(BGS(31), &i, 2048/4, DMA_MEMSET32);

			// print the instructions
		FontPrint(0, 0, BGS(31), "Sound example");
		FontPrint(0, 1, BGS(31), "L/R: play sounds");
		FontPrint(0, 2, BGS(31), "B: stop all sounds");

		if(KEYPRESS(KEY_L))
		{
				// make sure the channel is disabled first
			sndChannel[0].data = 0;

				// set up channel vars

				// Start at the start
			sndChannel[0].pos			= 0;

				// Calculate the increment. The piano sample was 
				// originally 8363Hz, so I've hardcoded it in here. 
				// Later we'll want to make a table of info on samples 
				// so it can be done automatically.
				// also, it is 12-bit fixed-point, so shift up before the divide
			sndChannel[0].inc			= (8363<<12)/18157;

				// Set the volume to maximum
			sndChannel[0].vol			= 64;

				// The length of the original sample (also 12-bit fixed-point)
				// This will go into our sample info table too
			sndChannel[0].length		= 10516<<12;

				// Set the loop length to the special no-loop marker value
			sndChannel[0].loopLength	= 0;

				// Set the data. This will actually start the channel up so 
				// it will be processed next time SndMix() is called
			sndChannel[0].data			= (s8*)dSndPiano;
		}

		if(KEYPRESS(KEY_R))
		{
				// make sure the channel is disabled first
			sndChannel[1].data = 0;

				// set up channel vars
			sndChannel[1].pos			= 0;

				// Play at 2* frequency. This is one octave above normal
			sndChannel[1].inc			= (8363*2<<12)/18157;

				// Same
			sndChannel[1].vol			= 64;

				// Same
			sndChannel[1].length		= 10516<<12;

				// This one will actually loop. Here, the loop start 
				// position (which I checked in the MOD I ripped the 
				// sample from) is 9176. So, we take length-loopStart 
				// and convert to 12-bit fixed-point like everything else.
			sndChannel[1].loopLength	= (10514-9176)<<12;

				// Same
			sndChannel[1].data			= (s8*)dSndPiano;
		}

		if(KEYPRESS(KEY_B))
		{
			for(i = 0; i < SND_MAX_CHANNELS; i++)
			{
					// Disable all the channels by setting the special disable value
				sndChannel[i].data = 0;
			}
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

		FontPrint(0, 11, BGS(31), "C1.data:");
		FontPrint(8, 11, BGS(31), htoa((s32)sndChannel[1].data));
		FontPrint(0, 12, BGS(31), "C1.pos:");
		FontPrint(8, 12, BGS(31), htoa((s32)sndChannel[1].pos));
		FontPrint(0, 13, BGS(31), "C1.inc:");
		FontPrint(8, 13, BGS(31), htoa((s32)sndChannel[1].inc));
		FontPrint(0, 14, BGS(31), "C1.vol:");
		FontPrint(8, 14, BGS(31), htoa((s32)sndChannel[1].vol));
		FontPrint(0, 15, BGS(31), "C1.len:");
		FontPrint(8, 15, BGS(31), htoa((s32)sndChannel[1].length));
		FontPrint(0, 16, BGS(31), "C1.loop:");
		FontPrint(8, 16, BGS(31), htoa((s32)sndChannel[1].loopLength));

			// Fill the next sound buffer. This can be done anywhere you like. 
			// It takes a while if you have many channels running, 
			// so it's best to do it after you're done with everything else. 
			// If you think you could get missed frames, you might want to 
			// do this in your VBlank function, just so you know for sure 
			// it will happen every frame. 
		SndMix();
	}

	return 0;
}
