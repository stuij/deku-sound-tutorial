#include "..\Gba.h"
#include "..\Sound.h"

extern const u8 dMod0Pattern0[];
extern const u8 dMod0Pattern1[];
extern const u8 dMod0Pattern2[];
extern const u8 dMod0Pattern3[];
extern const u8 dMod0Pattern4[];
extern const u8 dMod0Pattern5[];
extern const u8 dMod0Pattern6[];
extern const u8 dMod0Pattern7[];
extern const u8 dMod0Pattern8[];
extern const u8 dMod0Pattern9[];
extern const u8 dMod0Pattern10[];
extern const u8 dMod0Pattern11[];
extern const u8 dMod0Pattern12[];

extern const s8 dSmpData0[];
extern const s8 dSmpData1[];
extern const s8 dSmpData2[];
extern const s8 dSmpData3[];
extern const s8 dSmpData4[];
extern const s8 dSmpData5[];
extern const s8 dSmpData6[];
extern const s8 dSmpData7[];

const SAMPLE_HEADER dMod0SmpTable[] = {
	{ 17, 0, 64, 0, 16, dSmpData0 }, 
	{ 65, 0, 64, 0, 64, dSmpData1 }, 
	{ 8, 0, 32, 0, 8, dSmpData2 }, 
	{ 8, 0, 16, 0, 8, dSmpData3 }, 
	{ 8, 0, 64, 0, 8, dSmpData4 }, 
	{ 1123, 0, 48, 0, 0, dSmpData5 }, 
	{ 1505, 0, 64, 0, 0, dSmpData6 }, 
	{ 1463, 0, 64, 0, 0, dSmpData7 }, 
};

const u8 *dMod0PatternTable[] = {
	dMod0Pattern0, dMod0Pattern1, dMod0Pattern2, dMod0Pattern3, dMod0Pattern4, dMod0Pattern5, dMod0Pattern6, dMod0Pattern7, 
	dMod0Pattern8, dMod0Pattern9, dMod0Pattern10, dMod0Pattern11, dMod0Pattern12, 
};

const u8 dMod0OrderTable[] = {
	0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 
};

const MOD_HEADER dModTable[] = {
	{ dMod0SmpTable, dMod0OrderTable, dMod0PatternTable, 15, 13 }, 
};

const SAMPLE_HEADER dSfxTable[] = {
	{ 0, 0, 0, 0, 0, 0 }	// Placeholder, no SFX specified
};
