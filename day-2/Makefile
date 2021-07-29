CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

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
Crt0.s\
Math.s\
SndData.s

O_FILES = $(SRC_S_FILES:.s=.o) $(SRC_C_FILES:.c=.o) $(SRC_C_ARM_FILES:.c=.o)

all : $(NAME).gba

$(NAME).gba : $(NAME).elf
	$(OBJCOPY) -v -O binary $(NAME).elf $(NAME).gba

$(NAME).elf : $(O_FILES)
	$(LD) $(LDFLAGS) -o $(NAME).elf $(O_FILES)

$(SRC_C_FILES:.c=.o) : $(SRC_C_FILES)
	$(CC) $(CTHUMBFLAGS) $(@:.o=.c) -o $@

$(SRC_C_ARM_FILES:.c=.o) : $(SRC_C_FILES)
	$(CC) $(CARMFLAGS) $(@:.o=.c) -o $@

$(SRC_S_FILES:.s=.o) : $(SRC_S_FILES)
	$(CC) $(ASFLAGS) $(@:.o=.s) -o $@
