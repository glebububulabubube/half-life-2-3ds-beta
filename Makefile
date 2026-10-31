.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

TARGET		:=	hl-demo
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include

ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS	:=	-g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH) $(INCLUDE) -D__3DS__
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH)
LIBS	:=	-lctru -lm
LIBDIRS	:=	$(CTRULIB)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)
export VPATH	:=	$(CURDIR)/$(SOURCES)
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(notdir $(wildcard $(SOURCES)/*.c))
SFILES		:=	$(notdir $(wildcard $(SOURCES)/*.s))
export LD	:=	$(CC)
export OFILES :=	$(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE :=	-I$(CURDIR)/$(INCLUDES) -I$(CTRULIB)/include -I$(CURDIR)/$(BUILD)
export LIBPATHS :=	-L$(CTRULIB)/lib

.PHONY: all clean
all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
