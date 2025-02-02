CFILES = 	$(wildcard src/*.c) \
			$(wildcard src/peripherals/*.c) \
			$(wildcard src/scheduler/*.c) \
			$(wildcard src/utils/*.c) \
			$(wildcard src/mm/*.c) \
			$(wildcard src/userspace/*.c) \
			$(wildcard src/lib/*.c) \
			$(wildcard src/lib/elf/*.c) \
			$(wildcard src/fs/*.c) \
			$(wildcard src/fs/*/*.c)
ASMFILES = 	$(wildcard src/asm/*.S)

CFILES_USR = $(wildcard usr/src/*.c) \
			 $(wildcard usr/lib/*.c) 
ASMFILES_USR = $(wildcard usr/src/*.S)

OFILES = $(CFILES:%.c=build/%_c.o)
OFILES += $(ASMFILES:%.S=build/%_s.o)
OFILES_USR = $(CFILES_USR:%.c=usr/build/%_c.o)
OFILES_USR += $(ASMFILES_USR:%.S=usr/build/%_s.o)

GCCFLAGS = -Wall -Wextra -Wpedantic -O0 -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only #TODO: remove general-regs-only (enable fp support on boot), add -O2
GCCPATH = ../pellOs/gcc/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin
OUTPUT_APPEND = rpi4
ADDITIONAL_DEFINES = -UUART_DEBUG

all: clean userspace kernel8.img

build/%_s.o: %.S
	@$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -Iinclude -c -g $< -o $@

build/%_c.o: %.c
	@$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(ADDITIONAL_DEFINES) -Iinclude -c -g $< -o $@

usr/build/%_c.o: %.c
	@$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(ADDITIONAL_DEFINES) -Iinclude -c -g $< -o $@

usr/build/%_s.o: %.S
	@$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -Iinclude -c -g $< -o $@

qemu:   OUTPUT_APPEND = qemu
		# ADDITIONAL_DEFINES = -DUART_DEBUG

qemu:	all

userspace: $(OFILES_USR)
	/usr/bin/python tools/fs/genRamDisk.py

kernel8.img: $(OFILES)
	@$(GCCPATH)/aarch64-none-elf-ld -nostdlib -g $(OFILES) -T link_$(OUTPUT_APPEND).ld -o bin/kernel8.elf -Map=bin/kernel.map
	@$(GCCPATH)/aarch64-none-elf-objcopy -O binary bin/kernel8.elf bin/kernel8.img
	@/bin/cp bin/kernel8.img bin/kernel8_$(OUTPUT_APPEND).img

clean:
	@/bin/rm bin/kernel8.elf *.o *.img > /dev/null 2> /dev/null || true
	@/bin/rm build/src/*.o build/src/peripherals/*.o build/src/scheduler/*.o build/src/asm/*.o build/src/utils/*.o build/src/mm/*.o build/src/userspace/*.o build/src/lib/*.o build/src/fs/*.o build/src/fs/*/*.o > /dev/null 2> /dev/null || true