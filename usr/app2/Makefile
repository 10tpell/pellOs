CFILES_USR = $(wildcard src/*.c) \
			 $(wildcard lib/*.c) 
ASMFILES_USR = $(wildcard src/*.S)

OFILES_USR = $(CFILES_USR:%.c=build/%_c.o)
OFILES_USR += $(ASMFILES_USR:%.S=build/%_s.o)

GCCFLAGS = -Wall -Wextra -Wpedantic -O0 -ffreestanding -nostdlib -nostartfiles -mgeneral-regs-only #TODO: remove general-regs-only (enable fp support on boot), add -O2
GCCPATH = ../../../pellOs/gcc/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin

all: clean userspace

build/%_s.o: %.S
	@$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -Iinclude -c -g $< -o $@

build/%_c.o: %.c
	@$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) $(ADDITIONAL_DEFINES) -Iinclude -c -g $< -o $@

userspace: $(OFILES_USR)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib -e main -g $(OFILES_USR) -T ../link_userspace.ld -o bin/app2.elf -Map=bin/app2.map
	/bin/cp bin/app2.elf ../../ramdisk/bin/app2.elf

clean:
	@echo Cleaning files
	@/bin/rm bin/app2.elf *.o > /dev/null 2> /dev/null || true
	@/bin/rm build/src/*.o > /dev/null 2> /dev/null || true