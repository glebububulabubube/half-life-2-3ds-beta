.SUFFIXES:

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

TARGET := hl-demo
SOURCES := source

ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS := -g -O2 -Wall -mword-relocations -ffunction-sections $(ARCH) -I$(CTRULIB)/include -D__3DS__
LDFLAGS := -specs=3dsx.specs -g $(ARCH)
LIBS := -lctru -lm

OFILES := main.o

all: $(TARGET).cia

$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) -L$(CTRULIB)/lib $(LIBS) -o $@

$(TARGET).cia: $(TARGET).elf
	makerom -f cia -o $@ -elf $< -target t -desc app:4

%.o: source/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *.elf *.cia
