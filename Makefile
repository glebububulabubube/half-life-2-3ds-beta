.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

TARGET				:=	hl-demo
BUILD				:=	build
SOURCES				:=	source
INCLUDES			:=	include
APP_TITLE			:=	HL-DEMO
APP_DESCRIPTION		:=	Original FPS raycaster demo
APP_AUTHOR			:=	Homebrew

ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS	:=	-g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH)
CFLAGS	+=	$(INCLUDE) -D__3DS__
CXXFLAGS :=	$(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11
ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS	:=	-lctru -lm
LIBDIRS	:=	$(CTRULIB)

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT		:=	$(CURDIR)/$(TARGET)
export TOPDIR		:=	$(CURDIR)
export VPATH		:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR		:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

ifeq ($(strip $(CPPFILES)),)
export LD	:=	$(CC)
else
export LD	:=	$(CXX)
endif

export OFILES_SOURCES :=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES := $(OFILES_SOURCES)
export HFILES :=

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export _3DSXDEPS	:=	$(OUTPUT).smdh
export _3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh

.PHONY: all clean
all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@mkdir -p $@

clean:
	@rm -fr $(BUILD) $(TARGET).3dsx $(TARGET).smdh $(TARGET).elf $(TARGET).cia

else

export OUTPUT := $(TOPDIR)/$(TARGET)

.PHONY: all
all: $(OUTPUT).cia $(OUTPUT).3dsx

$(OUTPUT).smdh:
	@echo building smdh ...
	@smdhtool --create "$(APP_TITLE)" "$(APP_DESCRIPTION)" "$(APP_AUTHOR)" $@

$(OUTPUT).elf: $(OFILES)
	@echo linking $(notdir $@)
	@$(LD) $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

$(OUTPUT).3dsx: $(OUTPUT).elf $(OUTPUT).smdh
	@echo building 3dsx ...
	@3dsxtool $< $@ $(_3DSXFLAGS)

$(OUTPUT).cia: $(OUTPUT).elf
	@echo building cia ...
	@makerom -f cia -o $@ -elf $(OUTPUT).elf -target t

%.o: %.c
	@echo building $(notdir $<)
	@$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	@echo building $(notdir $<)
	@$(CXX) -c $(CXXFLAGS) $< -o $@

%.o: %.s
	@echo assembling $(notdir $<)
	@$(CC) -c $(ASFLAGS) $< -o $@

-include $(DEPSDIR)/*.d

endif
