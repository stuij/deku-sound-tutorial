PATH=C:\DevKit~1;C:\DevKit~1\Bin;%PATH%

NAME = Example
CARMFLAGS	= -c -O2 -marm -mthumb-interwork -ffreestanding
CTHUMBFLAGS	= -c -O2 -mthumb -mthumb-interwork -ffreestanding
ASFLAGS	= -c -mthumb-interwork -ffreestanding
LDFLAGS	= -T lnkscript -Map $(NAME).map

SRC_C_FILES =\
Main.c\
Misc.c\
Sound.c\
Font.c\
Data.c

SRC_C_ARM_FILES =\
Irq.c

SRC_S_FILES =\
Crt0.S\
Math.S\
SndData.S

O_FILES = $(SRC_S_FILES:.S=.o) $(SRC_C_FILES:.c=.o) $(SRC_C_ARM_FILES:.c=.o)

all : $(NAME).gba

$(NAME).gba : $(NAME).elf
	objcopy -v -O binary $(NAME).elf $(NAME).gba

$(NAME).elf : $(O_FILES)
	ld $(LDFLAGS) -o $(NAME).elf $(O_FILES)

$(SRC_C_FILES:.c=.o) : $(SRC_C_FILES)
	gcc $(CTHUMBFLAGS) $(@:.o=.c) -o $@

$(SRC_C_ARM_FILES:.c=.o) : $(SRC_C_FILES)
	gcc $(CARMFLAGS) $(@:.o=.c) -o $@

$(SRC_S_FILES:.S=.o) : $(SRC_S_FILES)
	gcc $(ASFLAGS) $(@:.o=.S) -o $@

