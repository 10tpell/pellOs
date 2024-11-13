CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
GCCPATH = ../pellOs/gcc/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin

all: clean kernel8.img

boot.o: boot.S
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -Iinclude -c -g boot.S -o boot.o

%.o: %.c
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c -g $< -o $@

kernel8.img: boot.o $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib -g boot.o $(OFILES) -T link.ld -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img

clean:
	/bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true