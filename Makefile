#
#    ___________  ___  ___  ___  ______ _   ____  _________ ___________ 
#   /  ___| ___ \/ _ \ |  \/  |  |  _  \ | | |  \/  || ___ \  ___| ___ \
#   \ `--.| |_/ / /_\ \| .  . |  | | | | | | | .  . || |_/ / |__ | |_/ /
#    `--. \    /|  _  || |\/| |  | | | | | | | |\/| ||  __/|  __||    / 
#   /\__/ / |\ \| | | || |  | |  | |/ /| |_| | |  | || |   | |___| |\ \ 
#   \____/\_| \_\_| |_/\_|  |_/  |___/  \___/\_|  |_/\_|   \____/\_| \_|
#                                                                       
# Namco System 2x6 SRAM dumper
# Copyright (c) 2025 Matias Israelson - MIT license    
#                                                                       
#

EE_BIN = ACSRAM_DUMPER.ELF
EE_OBJS_DIR = embed/
EE_OBJS = main.o modelname.o ioprp.o pad.o iop/acsram_dumper/ee_rpc.o \
	$(addprefix $(EE_OBJS_DIR), usbd.o bdm.o bdmfs_fatfs.o usbmass_bd.o fileXio.o iomanX.o acsram.o acsram_dumper.o mmceman.o)

DEBUG ?= 1
EE_CFLAGS += -fdata-sections -ffunction-sections -DNEWLIB_PORT_AWARE
EE_LDFLAGS += -Wl,--gc-sections
EE_LIBS += -liopreboot -ldebug -lpatches -lfileXio -lcdvd -lpadx


ifeq ($(DEBUG), 1)
  EXCEPTION_HANDLER = 1
  EE_CFLAGS += -DDEBUG -O0 -g
else
  EE_CFLAGS += -Os
  EE_LDFLAGS += -s
endif
ifeq ($(EXCEPTION_HANDLER),1)
  EE_CFLAGS += -DCATCH_EXCEPTIONS
  EE_LIBS += -Lexceptionman -lexceptionman
  DEPENDENCIES += exceptionman/libexceptionman.a
exceptionman/libexceptionman.a: exceptionman/
	$(MAKE) -C $<

endif
all: $(DEPENDENCIES) $(EE_BIN) 

clean:
	rm -rf $(EE_OBJS) $(EE_BIN)
	$(MAKE) -C iop/ clean 

ioprp.img:
	wget https://github.com/israpps/wLaunchELF_ISR/raw/system-2x6-support/iop/__precompiled/IOPRP_FILEIO.IMG -O $@

%.c: %.img
	bin2c $< $@ ioprp

.PHONY: iop/acsram_dumper.irx

iop/acsram_dumper.irx: iop/acsram_dumper/main.c
	$(MAKE) -C iop/acsram_dumper/

iop/mmceman.irx:
	wget https://github.com/ps2-mmce/mmceman/releases/download/latest/mmceman.irx -O "$@"

vpath %.irx iop/
vpath %.irx $(PS2SDK)/iop/irx/
IRXTAG = $(notdir $(addsuffix _irx, $(basename $<)))
$(EE_OBJS_DIR)%.c: %.irx
	$(DIR_GUARD)
	@bin2c $< $@ $(IRXTAG)

# Include makefiles
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal