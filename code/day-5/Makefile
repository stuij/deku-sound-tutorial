PATH=C:\DevKit~1;C:\DevKit~1\Bin;%PATH%

NAME = Example
CARMFLAGS	= -c -O2 -marm -mthumb-interwork -ffreestanding
CTHUMBFLAGS	= -c -O2 -mthumb -mthumb-interwork -ffreestanding
ASFLAGS	= -c -mthumb-interwork -ffreestanding
LDFLAGS	= -T lnkscript -Map $(NAME).map

MAKEFILE = Makefile

SRC_C_FILES =\
Main.c\
Misc.c\
Sound.c\
Font.c\
Data.c\
Data\SndData.c

SRC_C_ARM_FILES =\
Irq.c

SRC_S_FILES =\
Crt0.S\
Math.S\
Data\SndData.S

O_S_FILES = $(SRC_S_FILES:.S=_s.o)
O_C_FILES = $(SRC_C_FILES:.c=_c.o)
O_C_ARM_FILES = $(SRC_C_ARM_FILES:.c=_c.o)
O_FILES = $(O_S_FILES) $(O_C_FILES) $(O_C_ARM_FILES)

all : $(NAME).gba

$(NAME).gba : $(NAME).elf $(MAKEFILE)
	objcopy -v -O binary $(NAME).elf $(NAME).gba

$(NAME).elf : $(O_FILES) $(MAKEFILE)
	ld $(LDFLAGS) -o $(NAME).elf $(O_FILES)

$(O_C_FILES) : $(SRC_C_FILES) $(MAKEFILE)
	gcc $(CTHUMBFLAGS) $(@:_c.o=.c) -o $@

$(O_C_ARM_FILES) : $(SRC_C_FILES) $(MAKEFILE)
	gcc $(CARMFLAGS) $(@:_c.o=.c) -o $@

$(O_S_FILES) : $(SRC_S_FILES) $(MAKEFILE)
	gcc $(ASFLAGS) $(@:_s.o=.S) -o $@

