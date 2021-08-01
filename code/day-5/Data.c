#include "Gba.h"

const u16 dPalSys[16] = {
	0x7C1F,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x4B8F,0x0000,0x5EF7,0x7FFF,0x0000,
};

const u16 dFontBg[] = {
	0x2010,0x000e,0xff30,0xf0ff,0xe001,0xcc01,0xfffc,0x78ff,
	0x80ec,0x5003,0x100f,0x0017,0xcf2e,0xcccc,0xff09,0xcecf,
	0x20ce,0xff03,0x30fc,0x8913,0x3eb0,0xfccf,0x00ec,0xcc01,
	0x00cc,0x8e1f,0x4700,0xcecf,0x30cc,0x500f,0x0040,0xcf5f,
	0xee40,0x3f30,0xeeec,0xfcee,0xccec,0xce03,0xccfc,0xecee,
	0x00fc,0x2057,0x3f60,0xcccf,0x6300,0x5a00,0x0320,0x5ef0,
	0xa010,0x2030,0x90fe,0x10a3,0x1030,0x2017,0x00a0,0x1031,
	0x1077,0xec63,0x30e7,0x50ab,0x90bb,0xccdf,0x00ec,0x0097,
	0x107f,0xff07,0xdf90,0x6f90,0x8bf0,0x1b90,0x0701,0xc710,
	0x0f01,0xa4f0,0xc0eb,0x903f,0x5022,0xec54,0x1f41,0x11ee,
	0x8023,0x8023,0x0701,0xeecf,0xceee,0xcecf,0x83ec,0x0300,
	0xcece,0xeecf,0x00cc,0x0103,0xff30,0x1f41,0x9420,0x3b10,
	0x5f11,0x9c60,0x6480,0x3f60,0x3700,0xcc45,0x5c00,0xccec,
	0x20cc,0xcc4f,0xa840,0x50df,0x115f,0xeca0,0x0730,0x5fa0,
	0x8000,0xa021,0x9801,0x20fd,0x5043,0x402b,0x10c5,0x4047,
	0x105f,0xcc57,0x3f40,0x11ff,0x71bf,0x1018,0x0157,0x60f7,
	0x8087,0x60bf,0x723f,0xff20,0x5fa2,0xbfa0,0x3ff0,0x1fe0,
	0x2f01,0x7fa0,0x73d1,0x83f1,0x22ff,0x11c3,0x2263,0x5073,
	0x7207,0xc1ff,0x903f,0x4240,0x3fbb,0xceec,0xb712,0xe502,
	0xbf22,0xeb02,0x0100,0xba41,0x11ff,0x0152,0x1072,0x501b,
	0x0207,0x5263,0x2223,0x0233,0xfff3,0x1323,0x0212,0x8730,
	0x7211,0x3fb0,0x1703,0x4513,0x3fc0,0x01ff,0x40c2,0x1273,
	0x11bb,0x42a2,0x43c3,0xf03b,0x011f,0xef8a,0xda82,0x9a31,
	0x3f40,0x70ec,0x52c3,0x10c3,0x90c7,0xff77,0xe320,0xcb13,
	0xdf40,0x1be4,0x2394,0x20c0,0x7910,0x5ff0,0x34ff,0x031b,
	0x03f3,0x12fb,0x049a,0x2003,0x335f,0xe0e9,0xf75e,0xdfd0,
	0x1104,0x1721,0x7f11,0x41ec,0x801f,0x209f,0xbfbf,0x9711,
	0x04ce,0x2081,0xb11f,0x709f,0x30ff,0xe0d7,0xf79f,0x1f90,
	0x6731,0x3ff1,0x3fa0,0x04ce,0x7443,0xf0bb,0xf73f,0x0b32,
	0x7f80,0xbf61,0x0315,0xf1cc,0xb4df,0xc193,0xff5f,0x9f61,
	0x0550,0xbff1,0x1fa0,0xa325,0x1f25,0xda55,0xdf81,0x11ff,
	0x123f,0x10c7,0x4027,0x5120,0x505f,0x2537,0x25a3,0xffbb,
	0x9f51,0x1f72,0x5b60,0x9f90,0x8f62,0xfb85,0x9f82,0x01f0,
	0x90ff,0xc633,0x051f,0x261f,0x0667,0x762b,0x557b,0x768f,
	0xff43,0x8335,0x5ec0,0x6366,0x6b36,0xc081,0xd035,0x9b06,
	0x3f90,0x53ff,0xd67f,0x059f,0xb07b,0xa63f,0x15bf,0xc5ff,
	0xe0bb,0xfe3f,0x6b70,0xc3f0,0xc380,0x4386,0xaf94,0xdad5,
	0x24d0,0xffcf,0xc710,0x23e1,0x9f47,0x5726,0x5fb0,0xcfa6,
	0x3af6,0xdbc6,0x07ff,0x1593,0x03d7,0x954b,0x111f,0x616b,
	0xc07b,0xe05f,0xffff,0xbfc1,0x1ff1,0xa341,0xffa6,0x3ff1,
	0xa490,0x7f70,0x8718,0xf0ff,0x68bf,0x075f,0x6157,0xf7e7,
	0x31ff,0x3593,0xa784,0x7fa0,0x12cf,0x90ed,0xc0db,0xd0bf,
	0x931f,0xe05f,0x481f,0xffe7,0x2799,0x5f50,0x6359,0x4b29,
	0x1d09,0xdea1,0x8380,0x2257,0x59fe,0x580b,0x3977,0x495b,
	0xf84b,0x1983,0x0726,0xccf7,0xce70,0x4b16,0x7b09,0x3f56,
	0xeecc,0xeeee,0xee06,0xdddd,0xdddd,0x0b10,0x34d0,0x7fec,
	0xf0cd,0x9003,0xf007,0xf011,0xc321,0xa0db,0x206f,0x8303,
	0x8000,0xdeec,0xdddd,0x00ec,0xd081,0x8041,0x9f30,0xccde,
	0xdddd,0xcdee,0x30cc,0xedcc,0x5fe0,0x71e0,0xccde,0xdccc,
	0x00fa,0x00d0,0x4030,0xb0cf,0x0093,0xcded,0x4f00,0x91cd,
	0x7ec0,0xddee,0x0201,0xedcc,0x08de,0xffa7,0xc2f0,0x03f0,
	0x03f0,0x0b50,0x4310,0x4fd1,0xb31a,0x8e2a,0xec77,0x1c0b,
	0xa814,0xf02a,0x11cc,0xc03d,0x1a1f,0xb85c,0x601a,0x1adc,
	0x59c0,0x327b,0xcc90,0xcedc,0xff2a,0x11dc,0xcdad,0x7600,
	0x08dc,0xcf2f,0xcd7f,0x8b0a,0xb021,0x3620,0x931a,0x3620,
	0x9411,0x0860,0x09c3,0x008b,0xed0d,0xfffc,0x00dc,0xf052,
	0xd51f,0x0f02,0xb617,0x10ec,0xfe31,0x3500,0x10ef,0x233e,
	0xcffe,0x0902,0xccff,0x11dd,0xbb7d,0xf97f,0x3f30,0xe807,
	0x8b00,0x4486,0x3b29,0xdddd,0x0360,0xff3c,0x01dc,0x4b17,
	0x0ba8,0x797a,0xde53,0xf8ed,0x0320,0x5f33,0x7f93,0x3f0c,
	0x3f00,0xdddc,0x66dd,0x00cd,0x00e2,0xcfdd,0x19cd,0x6b59,
	0xcf73,0xfc20,0x05fc,0xfcfb,0xedcf,0xfcde,0x00ba,0xcefb,
	0x1f10,0x5710,0x5f8c,0x07cc,0xcedf,0xdc19,0xcdec,0x2302,
	0xdb0a,0xcdec,0x2f0b,0xde73,0x3f7c,0x7f1c,0x2902,0xdccf,
	0x2406,0xb301,0x1487,0xecbf,0xcdec,0x00ce,0xa3fc,0x2424,
	0x79c7,0x10de,0x0003,0x8023,0x283f,0xcc85,0x18de,0x4398,
	0x01cd,0xcd5e,0xfecf,0x00df,0x61ef,0xf71e,0x226b,0x9c2c,
	0x5f10,0x7b10,0x01ee,0x6a07,0x80ff,0x7717,0xb1ed,0x541f,
	0x1a3f,0xdc3f,0x1f20,0x5302,0xfe21,0xccde,0x161e,0xde77,
	0xbb1b,0x3f11,0xa410,0x3f42,0xcfdf,0x3f4d,0x1a03,0xeedc,
	0x1712,0x4780,0xb70d,0xdb13,0xf0f8,0x7001,0x1307,0xf0f7,
	0x7001,0x0007
};	// dFontBg

const u16 dFontMapBg[128] = {
	0xF000,0xF001,0xF002,0xF000,0xF000,0xF003,0xF004,0xF005,
	0xF006,0xF406,0xF007,0xF008,0xF009,0xF00A,0xF00B,0xF00C,
	0xF00D,0xF00E,0xF00F,0xF010,0xF011,0xF012,0xF013,0xF014,
	0xF015,0xF016,0xF017,0xF000,0xF000,0xF018,0xF000,0xF019,
	0xF000,0xF01A,0xF01B,0xF01C,0xF01D,0xF01E,0xF01F,0xF020,
	0xF021,0xF022,0xF023,0xF024,0xF025,0xF026,0xF027,0xF028,
	0xF029,0xF02A,0xF02B,0xF02C,0xF02D,0xF02E,0xF02F,0xF030,
	0xF031,0xF032,0xF033,0xF000,0xF40C,0xF000,0xF000,0xF034,
	0xF000,0xF035,0xF036,0xF037,0xF038,0xF039,0xF03A,0xF03B,
	0xF03C,0xF03D,0xF03E,0xF03F,0xF040,0xF041,0xF042,0xF043,
	0xF044,0xF045,0xF046,0xF047,0xF048,0xF049,0xF04A,0xF04B,
	0xF04C,0xF04D,0xF04E,0xF000,0xF000,0xF000,0xF04F,0xF050,
	0xF051,0xF052,0xF053,0xF054,0xF055,0xF056,0xF057,0xF058,
	0xF059,0xF05A,0xF05B,0xF05C,0xF05D,0xF05E,0xF05F,0xF060,
	0xF061,0xF062,0xF063,0xF064,0xF065,0xF066,0xF067,0xF068,
	0xF069,0xF06A,0xF06B,0xF06C,0xF06D,0xF06E,0xF06F,0xF06E,
};	// dFontMapBg
