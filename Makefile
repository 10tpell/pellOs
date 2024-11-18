CFILES = $(wildcard *.c)
ASMFILES = $(wildcard *.S)
OFILES = $(CFILES:%.c=build/%_c.o)
OFILES += $(ASMFILES:%.S=build/%_s.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles #-mgeneral-regs-only #TODO: remove general-regs-only (enable fp support on boot)
GCCPATH = ../pellOs/gcc/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-elf/bin
LD_FILE = link_rpi4.ld

# OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
# OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

all: clean kernel8.img

build/%_s.o: %.S
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -Iinclude -c -g $< -o $@

build/%_c.o: %.c
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c -g $< -o $@

qemu:	LD_FILE = link_qemu.ld
qemu:	all

kernel8.img: $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib -g $(OFILES) -T $(LD_FILE) -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf kernel8.img

clean:
	/bin/rm kernel8.elf *.o *.img > /dev/null 2> /dev/null || true