CROSS   = riscv64-unknown-elf-
CC      = $(CROSS)gcc
OBJDUMP = $(CROSS)objdump

ARCH    = -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany
CFLAGS  = $(ARCH) -nostdlib -nostartfiles -ffreestanding -g -O0
LDFLAGS = -T link.ld

TARGET  = kernel.elf
SRCS_S  = Startup.S trap.S
SRCS_C  = main.c

QEMU    = qemu-system-riscv64
QFLAGS  = -machine virt -cpu rv64 -bios none -nographic

all: $(TARGET)

$(TARGET): $(SRCS_S) $(SRCS_C) link.ld
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS_S) $(SRCS_C) -o $(TARGET)

run: $(TARGET)
	$(QEMU) $(QFLAGS) -kernel $(TARGET)

debug: $(TARGET)
	$(QEMU) $(QFLAGS) -kernel $(TARGET) -s -S

dump: $(TARGET)
	$(OBJDUMP) -d $(TARGET) | less

clean:
	rm -f $(TARGET)

.PHONY: all run debug clean dump
