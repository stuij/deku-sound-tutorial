CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

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
Data/SndData.c

SRC_C_ARM_FILES =\
Irq.c

SRC_S_FILES =\
Crt0.s\
Math.s\
Data/SndData.s

O_S_FILES = $(SRC_S_FILES:.s=_s.o)
O_C_FILES = $(SRC_C_FILES:.c=_c.o)
O_C_ARM_FILES = $(SRC_C_ARM_FILES:.c=_c.o)
O_FILES = $(O_S_FILES) $(O_C_FILES) $(O_C_ARM_FILES)

.PHONY: all clean reqs

all : reqs $(NAME).gba

reqs :
	cd FreqTable && make
	./make-freqtable.sh
	cd Converter && make
	./convert-data.sh

$(NAME).gba : $(NAME).elf $(MAKEFILE)
	$(OBJCOPY) -v -O binary $(NAME).elf $(NAME).gba

$(NAME).elf : $(O_FILES) $(MAKEFILE)
	$(LD) $(LDFLAGS) -o $(NAME).elf $(O_FILES)

$(O_C_FILES) :			$(@:_c.o=.c) $(MAKEFILE)
	$(CC) $(CTHUMBFLAGS)	$(@:_c.o=.c) -o $@

$(O_C_ARM_FILES) :		$(@:_c.o=.c) $(MAKEFILE)
	$(CC) $(CARMFLAGS)	$(@:_c.o=.c) -o $@

$(O_S_FILES) :			$(@:_s.o=.s) $(MAKEFILE)
	$(CC) $(ASFLAGS)		$(@:_s.o=.s) -o $@

clean:
	rm */*.o *.o Converter/converter FreqTable/freqtable \
	  Data/SndData.* PeriodTable.txt \
	  Example.elf Example.map
