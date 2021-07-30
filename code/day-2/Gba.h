#ifndef GBA_H
#define GBA_H

//typedefs. woo~

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned long	u32;

typedef signed char		s8;
typedef signed short	s16;
typedef signed long		s32;

typedef unsigned char	byte;
typedef unsigned short	hword;
typedef unsigned long	word;

typedef unsigned char Bool;

typedef void (*fp)(void);

//these make life easier

#define BIT00 0x1
#define BIT01 0x2
#define BIT02 0x4
#define BIT03 0x8
#define BIT04 0x10
#define BIT05 0x20
#define BIT06 0x40
#define BIT07 0x80
#define BIT08 0x100
#define BIT09 0x200
#define BIT10 0x400
#define BIT11 0x800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#define BIT16 0x10000
#define BIT17 0x20000
#define BIT18 0x40000
#define BIT19 0x80000
#define BIT20 0x100000
#define BIT21 0x200000
#define BIT22 0x400000
#define BIT23 0x800000
#define BIT24 0x1000000
#define BIT25 0x2000000
#define BIT26 0x4000000
#define BIT27 0x8000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000

//locations for all the different types of memory

#define WRAM			((u32*)0x2000000)
#define IRAM			((u32*)0x3000000)
#define PAL_BG			((u16*)0x5000000)
#define PAL_OBJ			((u16*)0x5000200)
#define VRAM			((u16*)0x6000000)
#define OAM_MEM			((u16*)0x6010000)
#define OAM_DATA		((u32*)0x7000000)

//registers galore!

#define REG_INTERRUPT	*(volatile u32*)0x3007FFC
#define REG_DISPCNT		*(volatile u32*)0x4000000
#define REG_DISPCNT_L	*(volatile u16*)0x4000000
#define REG_DISPCNT_H	*(volatile u16*)0x4000002
#define REG_DISPSTAT	*(volatile u16*)0x4000004
#define REG_VCOUNT		*(volatile u16*)0x4000006
#define REG_BG0CNT		*(volatile u16*)0x4000008
#define REG_BG1CNT		*(volatile u16*)0x400000A
#define REG_BG2CNT		*(volatile u16*)0x400000C
#define REG_BG3CNT		*(volatile u16*)0x400000E
#define REG_BG0HOFS		*(volatile u16*)0x4000010
#define REG_BG0VOFS		*(volatile u16*)0x4000012
#define REG_BG1HOFS		*(volatile u16*)0x4000014
#define REG_BG1VOFS		*(volatile u16*)0x4000016
#define REG_BG2HOFS		*(volatile u16*)0x4000018
#define REG_BG2VOFS		*(volatile u16*)0x400001A
#define REG_BG3HOFS		*(volatile u16*)0x400001C
#define REG_BG3VOFS		*(volatile u16*)0x400001E
#define REG_BG2PA		*(volatile u16*)0x4000020
#define REG_BG2PB		*(volatile u16*)0x4000022
#define REG_BG2PC		*(volatile u16*)0x4000024
#define REG_BG2PD		*(volatile u16*)0x4000026
#define REG_BG2X		*(volatile u32*)0x4000028
#define REG_BG2X_L		*(volatile u16*)0x4000028
#define REG_BG2X_H		*(volatile u16*)0x400002A
#define REG_BG2Y		*(volatile u32*)0x400002C
#define REG_BG2Y_L		*(volatile u16*)0x400002C
#define REG_BG2Y_H		*(volatile u16*)0x400002E
#define REG_BG3PA		*(volatile u16*)0x4000030
#define REG_BG3PB		*(volatile u16*)0x4000032
#define REG_BG3PC		*(volatile u16*)0x4000034
#define REG_BG3PD		*(volatile u16*)0x4000036
#define REG_BG3X		*(volatile u32*)0x4000038
#define REG_BG3X_L		*(volatile u16*)0x4000038
#define REG_BG3X_H		*(volatile u16*)0x400003A
#define REG_BG3Y		*(volatile u32*)0x400003C
#define REG_BG3Y_L		*(volatile u16*)0x400003C
#define REG_BG3Y_H		*(volatile u16*)0x400003E
#define REG_WIN0H		*(volatile u16*)0x4000040
#define REG_WIN1H		*(volatile u16*)0x4000042
#define REG_WIN0V		*(volatile u16*)0x4000044
#define REG_WIN1V		*(volatile u16*)0x4000046
#define REG_WININ		*(volatile u16*)0x4000048
#define REG_WINOUT		*(volatile u16*)0x400004A
#define REG_MOSAIC		*(volatile u32*)0x400004C
#define REG_MOSAIC_L	*(volatile u32*)0x400004C
#define REG_MOSAIC_H	*(volatile u32*)0x400004E
#define REG_BLDMOD		*(volatile u16*)0x4000050
#define REG_COLEV		*(volatile u16*)0x4000052
#define REG_COLEY		*(volatile u16*)0x4000054
#define REG_SG10		*(volatile u32*)0x4000060
#define REG_SG10_L		*(volatile u16*)0x4000060
#define REG_SG10_H		*(volatile u16*)0x4000062
#define REG_SG11		*(volatile u16*)0x4000064
#define REG_SG20		*(volatile u16*)0x4000068
#define REG_SG21		*(volatile u16*)0x400006C
#define REG_SG30		*(volatile u32*)0x4000070
#define REG_SG30_L		*(volatile u16*)0x4000070
#define REG_SG30_H		*(volatile u16*)0x4000072
#define REG_SG31		*(volatile u16*)0x4000074
#define REG_SG40		*(volatile u16*)0x4000078
#define REG_SG41		*(volatile u16*)0x400007C
#define REG_SGCNT0		*(volatile u32*)0x4000080
#define REG_SGCNT0_L	*(volatile u16*)0x4000080
#define REG_SGCNT0_H	*(volatile u16*)0x4000082
#define REG_SGCNT1		*(volatile u16*)0x4000084
#define REG_SGBIAS		*(volatile u16*)0x4000088
#define REG_SGWR0		*(volatile u32*)0x4000090
#define REG_SGWR0_L		*(volatile u16*)0x4000090
#define REG_SGWR0_H		*(volatile u16*)0x4000092
#define REG_SGWR1		*(volatile u32*)0x4000094
#define REG_SGWR1_L		*(volatile u16*)0x4000094
#define REG_SGWR1_H		*(volatile u16*)0x4000096
#define REG_SGWR2		*(volatile u32*)0x4000098
#define REG_SGWR2_L		*(volatile u16*)0x4000098
#define REG_SGWR2_H		*(volatile u16*)0x400009A
#define REG_SGWR3		*(volatile u32*)0x400009C
#define REG_SGWR3_L		*(volatile u16*)0x400009C
#define REG_SGWR3_H		*(volatile u16*)0x400009E
#define REG_SGFIFOA		*(volatile u32*)0x40000A0
#define REG_SGFIFOA_L	*(volatile u16*)0x40000A0
#define REG_SGFIFOA_H	*(volatile u16*)0x40000A2
#define REG_SGFIFOB		*(volatile u32*)0x40000A4
#define REG_SGFIFOB_L	*(volatile u16*)0x40000A4
#define REG_SGFIFOB_H	*(volatile u16*)0x40000A6
#define REG_DM0SAD		*(volatile u32*)0x40000B0
#define REG_DM0SAD_L	*(volatile u16*)0x40000B0
#define REG_DM0SAD_H	*(volatile u16*)0x40000B2
#define REG_DM0DAD		*(volatile u32*)0x40000B4
#define REG_DM0DAD_L	*(volatile u16*)0x40000B4
#define REG_DM0DAD_H	*(volatile u16*)0x40000B6
#define REG_DM0CNT		*(volatile u32*)0x40000B8
#define REG_DM0CNT_L	*(volatile u16*)0x40000B8
#define REG_DM0CNT_H	*(volatile u16*)0x40000BA
#define REG_DM1SAD		*(volatile u32*)0x40000BC
#define REG_DM1SAD_L	*(volatile u16*)0x40000BC
#define REG_DM1SAD_H	*(volatile u16*)0x40000BE
#define REG_DM1DAD		*(volatile u32*)0x40000C0
#define REG_DM1DAD_L	*(volatile u16*)0x40000C0
#define REG_DM1DAD_H	*(volatile u16*)0x40000C2
#define REG_DM1CNT		*(volatile u32*)0x40000C4
#define REG_DM1CNT_L	*(volatile u16*)0x40000C4
#define REG_DM1CNT_H	*(volatile u16*)0x40000C6
#define REG_DM2SAD		*(volatile u32*)0x40000C8
#define REG_DM2SAD_L	*(volatile u16*)0x40000C8
#define REG_DM2SAD_H	*(volatile u16*)0x40000CA
#define REG_DM2DAD		*(volatile u32*)0x40000CC
#define REG_DM2DAD_L	*(volatile u16*)0x40000CC
#define REG_DM2DAD_H	*(volatile u16*)0x40000CE
#define REG_DM2CNT		*(volatile u32*)0x40000D0
#define REG_DM2CNT_L	*(volatile u16*)0x40000D0
#define REG_DM2CNT_H	*(volatile u16*)0x40000D2
#define REG_DM3SAD		*(volatile u32*)0x40000D4
#define REG_DM3SAD_L	*(volatile u16*)0x40000D4
#define REG_DM3SAD_H	*(volatile u16*)0x40000D6
#define REG_DM3DAD		*(volatile u32*)0x40000D8
#define REG_DM3DAD_L	*(volatile u16*)0x40000D8
#define REG_DM3DAD_H	*(volatile u16*)0x40000DA
#define REG_DM3CNT		*(volatile u32*)0x40000DC
#define REG_DM3CNT_L	*(volatile u16*)0x40000DC
#define REG_DM3CNT_H	*(volatile u16*)0x40000DE
#define REG_TM0D		*(volatile u16*)0x4000100
#define REG_TM0CNT		*(volatile u16*)0x4000102
#define REG_TM1D		*(volatile u16*)0x4000104
#define REG_TM1CNT		*(volatile u16*)0x4000106
#define REG_TM2D		*(volatile u16*)0x4000108
#define REG_TM2CNT		*(volatile u16*)0x400010A
#define REG_TM3D		*(volatile u16*)0x400010C
#define REG_TM3CNT		*(volatile u16*)0x400010E
#define REG_SCD0		*(volatile u16*)0x4000120
#define REG_SCD1		*(volatile u16*)0x4000122
#define REG_SCD2		*(volatile u16*)0x4000124
#define REG_SCD3		*(volatile u16*)0x4000126
#define REG_SCCNT		*(volatile u32*)0x4000128
#define REG_SCCNT_L		*(volatile u16*)0x4000128
#define REG_SCCNT_H		*(volatile u16*)0x400012A
#define REG_P1			*(volatile u16*)0x4000130
#define REG_P1CNT		*(volatile u16*)0x4000132
#define REG_R			*(volatile u16*)0x4000134
#define REG_HS_CTRL		*(volatile u16*)0x4000140
#define REG_JOYRE		*(volatile u32*)0x4000150
#define REG_JOYRE_L		*(volatile u16*)0x4000150
#define REG_JOYRE_H		*(volatile u16*)0x4000152
#define REG_JOYTR		*(volatile u32*)0x4000154
#define REG_JOYTR_L		*(volatile u16*)0x4000154
#define REG_JOYTR_H		*(volatile u16*)0x4000156
#define REG_JSTAT		*(volatile u32*)0x4000158
#define REG_JSTAT_L		*(volatile u16*)0x4000158
#define REG_JSTAT_H		*(volatile u16*)0x400015A
#define REG_IE			*(volatile u16*)0x4000200
#define REG_IF			*(volatile u16*)0x4000202
#define REG_WSCNT		*(volatile u16*)0x4000204
#define REG_IME			*(volatile u16*)0x4000208
#define REG_PAUSE		*(volatile u16*)0x4000300

//where tile image data goes in VRAM (modes 0-2)

#define BGC(n)			((u16*)(0x06000000 + ((n) << 14)))

#define BGC0			((u16*)0x06000000)
#define BGC1			((u16*)0x06004000)
#define BGC2			((u16*)0x06008000)
#define BGC3			((u16*)0x0600C000)

//tile indices go in these to display on screen (modes 0-2)

#define BGS(n)			((u16*)(0x06000000 + ((n) << 11)))

//sprite tile addresses

#define SPRITE_TILE(n)	(((u16*)OAM_MEM) + ((n) << 4))

//16-color sprite palette address

#define SPRITE_PAL(n)	(((u16*)PAL_OBJ) + ((n) << 4))

//register settings, so I don't have to remember which bit does what

//these are for DISPCNT
#define DCNT_MODE0			0x0
#define DCNT_MODE1			0x1
#define DCNT_MODE2			0x2
#define DCNT_MODE3			0x3
#define DCNT_MODE4			0x4
#define DCNT_MODE5			0x5

#define DCNT_BACKBUF		BIT04
#define DCNT_OAM_HBL		BIT05
#define DCNT_OBJ_MAP_1D		BIT06
#define DCNT_FORCE_BLANK	BIT07

#define DCNT_BG0_ENABLE		BIT08
#define DCNT_BG1_ENABLE		BIT09
#define DCNT_BG2_ENABLE		BIT10
#define DCNT_BG3_ENABLE		BIT11
#define DCNT_OBJ_ENABLE		BIT12
#define DCNT_WIN0_ENABLE	BIT13
#define DCNT_WIN1_ENABLE	BIT14
#define DCNT_WINOBJ_ENABLE	BIT15

#define GET_MODE			(REG_DISPCNT & 7)

//these are for DISPSTAT
#define DSTAT_VBL_MASK		BIT00
#define DSTAT_HBL_MASK		BIT01
#define DSTAT_VBL_IRQ		BIT03
#define DSTAT_HBL_IRQ		BIT04
#define DSTAT_YTRIG_IRQ		BIT05
#define DSTAT_YTRIGGER(n)	(((n) & 0xff) << 8)
#define SET_YTRIGGER(n)	(REG_DISPSTAT = (REG_DISPSTAT & 0x00ff) | YTRIGGER(n))

//for BGxCNT regs
#define BG_MOSAIC			BIT06
#define BG_256COLOR			BIT07
#define BG_WRAP				BIT13
#define BG_SIZE(n)			(((n) & 3) << 14)
#define BG_CHAR(n)			(((n) & 3) << 2)
#define BG_SCREEN(n)		(((n) & 31) << 8)
#define BG_PRIORITY(n)		((n) & 3)
#define GET_BG_SCREEN(n)	((u16*)(0x6000000 + ((((n) >> 8) & 31) << 11)))

//for WININ
#define WIN_W0_BG0			BIT00
#define WIN_W0_BG1			BIT01
#define WIN_W0_BG2			BIT02
#define WIN_W0_BG3			BIT03
#define WIN_W0_OBJ			BIT04
#define WIN_W0_BLEND		BIT05
#define WIN_W1_BG0			BIT08
#define WIN_W1_BG1			BIT09
#define WIN_W1_BG2			BIT10
#define WIN_W1_BG3			BIT11
#define WIN_W1_OBJ			BIT12
#define WIN_W1_BLEND		BIT13

//for WINOUT
#define WIN_OUT_BG0			BIT00
#define WIN_OUT_BG1			BIT01
#define WIN_OUT_BG2			BIT02
#define WIN_OUT_BG3			BIT03
#define WIN_OUT_OBJ			BIT04
#define WIN_OUT_BLEND		BIT05
#define WIN_OBJ_BG0			BIT08
#define WIN_OBJ_BG1			BIT09
#define WIN_OBJ_BG2			BIT10
#define WIN_OBJ_BG3			BIT11
#define WIN_OBJ_OBJ			BIT12
#define WIN_OBJ_BLEND		BIT13

//for mosaic
#define MOSAIC_BG_H(n)		(n)
#define MOSAIC_BG_V(n)		((n) << 4)
#define MOSAIC_OBJ_H(n)		((n) << 8)
#define MOSAIC_OBJ_V(n)		((n) << 12)
#define MOSAIC(a, b, c, d)	((a) | ((b) << 4) | ((c) << 8) | ((d) << 12))

//for BLDMOD
#define BLEND_MODE_NORMAL	BIT06
#define BLEND_MODE_FADEIN	BIT07
#define BLEND_MODE_FADEOUT	(BIT06 | BIT07)
#define BLEND_T1_BG0		BIT00
#define BLEND_T1_BG1		BIT01
#define BLEND_T1_BG2		BIT02
#define BLEND_T1_BG3		BIT03
#define BLEND_T1_OBJ		BIT04
#define BLEND_T1_BD			BIT05
#define BLEND_T2_BG0		BIT08
#define BLEND_T2_BG1		BIT09
#define BLEND_T2_BG2		BIT10
#define BLEND_T2_BG3		BIT11
#define BLEND_T2_OBJ		BIT12
#define BLEND_T2_BD			BIT13

//except this one's for COLEV
#define BLEND(x, y)			(((x) & 31) | (((y) & 31) << 8))

//for TIMERxCNT regs
#define TIMER_64			BIT00
#define TIMER_256			BIT01
#define TIMER_1024			(BIT00 | BIT01)
#define TIMER_CASCADE		BIT02
#define TIMER_IRQ			BIT06
#define TIMER_ENABLE		BIT07

//for DMAxCNT
#define DMA_DEST_DECWRITE	BIT05
#define DMA_DEST_FIXED		BIT06
#define DMA_DEST_INC		(BIT05 | BIT06)
#define DMA_SRC_DECWRITE	BIT07
#define DMA_SRC_FIXED		BIT08
#define DMA_REPEAT			BIT09
#define DMA_WORD			BIT10
#define DMA_MODE_VBL		BIT12
#define DMA_MODE_HBL		BIT13
#define DMA_MODE_FIFO		(BIT12 | BIT13)
#define DMA_IRQ				BIT14
#define DMA_ENABLE			BIT15
#define DMA_MEMCPY16		(DMA_ENABLE)
#define DMA_MEMCPY32		(DMA_WORD | DMA_ENABLE)
#define DMA_MEMSET16		(DMA_SRC_FIXED | DMA_ENABLE)
#define DMA_MEMSET32		(DMA_SRC_FIXED | DMA_WORD | DMA_ENABLE)

//for IE/IF
#define IRQ_VBL				BIT00
#define IRQ_HBL				BIT01
#define IRQ_YTRIG			BIT02
#define IRQ_TM0				BIT03
#define IRQ_TM1				BIT04
#define IRQ_TM2				BIT05
#define IRQ_TM3				BIT06
#define IRQ_COMMS			BIT07
#define IRQ_DMA0			BIT08
#define IRQ_DMA1			BIT09
#define IRQ_DMA2			BIT10
#define IRQ_DMA3			BIT11
#define IRQ_KEY				BIT12
#define IRQ_CART			BIT13

//for SGCNT1
#define SOUND_ENABLE		BIT07

//for SGCNT0_H
#define SOUND_RATIO(x)		(x)
#define SOUNDA_VOL_100		BIT02
#define SOUNDB_VOL_100		BIT03
#define SOUNDA_ROUT			BIT08
#define SOUNDA_LOUT			BIT09
#define SOUNDA_TIMER		BIT10
#define SOUNDA_FIFORESET	BIT11
#define SOUNDB_ROUT			BIT12
#define SOUNDB_LOUT			BIT13
#define SOUNDB_TIMER		BIT14
#define SOUNDB_FIFORESET	BIT15

//for WSCNT
#define CST_SRAM_4WAIT		0x0000
#define CST_SRAM_3WAIT		0x0001
#define CST_SRAM_2WAIT		0x0002
#define CST_SRAM_8WAIT		0x0003
#define CST_ROM0_1ST_4WAIT	0x0000
#define CST_ROM0_1ST_3WAIT	0x0004
#define CST_ROM0_1ST_2WAIT	0x0008
#define CST_ROM0_1ST_8WAIT	0x000c
#define CST_ROM0_2ND_2WAIT	0x0000
#define CST_ROM0_2ND_1WAIT	0x0010
#define CST_ROM1_1ST_4WAIT	0x0000
#define CST_ROM1_1ST_3WAIT	0x0020
#define CST_ROM1_1ST_2WAIT	0x0040
#define CST_ROM1_1ST_8WAIT	0x0060
#define CST_ROM1_2ND_4WAIT	0x0000
#define CST_ROM1_2ND_1WAIT	0x0080
#define CST_ROM2_1ST_4WAIT	0x0000
#define CST_ROM2_1ST_3WAIT	0x0100
#define CST_ROM2_1ST_2WAIT	0x0200
#define CST_ROM2_1ST_8WAIT	0x0300
#define CST_ROM2_2ND_8WAIT	0x0000
#define CST_ROM2_2ND_1WAIT	0x0400

#define CST_PHI_OUT_NONE	0x0000 // terminal output clock fixed Lo
#define CST_PHI_OUT_4MCK	0x0800 // 4MHz
#define CST_PHI_OUT_8MCK	0x1000 // 8MHz
#define CST_PHI_OUT_16MCK	0x1800 // 16MHz
#define CST_PREFETCH_ENABLE	0x4000 // Enable prefetch buffer
#define CST_AGB				0x0000 // AGB Game Pak
#define CST_CGB				0x8000 // CGB Game Pak

//for REG_P1
#define KEY_A			BIT00
#define KEY_B			BIT01
#define KEY_SELECT		BIT02
#define KEY_START		BIT03
#define KEY_RIGHT		BIT04
#define KEY_LEFT		BIT05
#define KEY_UP			BIT06
#define KEY_DOWN		BIT07
#define KEY_R			BIT08
#define KEY_L			BIT09

//everything else

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define IN_EWRAM __attribute__ ((section (".ewram")))
#define IN_IWRAM __attribute__ ((section (".iwram")))
#define ALIGN(n)  __attribute__ ((aligned ((n))))
extern u32 __FarFunction(u32 (*ptr)(), ...);
extern void __FarProcedure(u32);

#define RGB(r, g, b) ((r) | ((g) << 5) | ((b) << 10))

#endif
