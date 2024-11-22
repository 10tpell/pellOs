CFILES = $(wildcard *.c)
ASMFILES = $(wildcard *.S)
OFILES = $(CFILES:%.c=build/%_c.o)
OFILES += $(ASMFILES:%.S=build/%_s.o)
GCCFLAGS = -Wall -O1 -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only #TODO: remove general-regs-only (enable fp support on boot), add -O2
GCCPATH = ../pellOs/gcc/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin
OUTPUT_APPEND = rpi4
ADDITIONAL_DEFINES = -UUART_DEBUG

all: clean kernel8.img

build/%_s.o: %.S
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -Iinclude -c -g $< -o $@

build/%_c.o: %.c
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(ADDITIONAL_DEFINES) -c -g $< -o $@

qemu:   OUTPUT_APPEND = qemu
		# ADDITIONAL_DEFINES = -DUART_DEBUG

qemu:	all

kernel8.img: $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib -g $(OFILES) -T link_$(OUTPUT_APPEND).ld -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img
	/bin/cp kernel8.img kernel8_$(OUTPUT_APPEND).img

clean:
	/bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true
	/bin/rm build/*.o > /dev/null 2> /dev/null || true