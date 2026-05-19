CC = gcc

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Name of the executable (minus the extension!) Nozomi 04-15-2026
EXEC_NAME = test

export BLOCKSDS			?= /opt/wonderful/thirdparty/blocksds/core
export BLOCKSDSEXT		?= /opt/wonderful/thirdparty/blocksds/external
export WONDERFUL_TOOLCHAIN	?= /opt/wonderful
ARM_NONE_EABI_PATH	?= $(WONDERFUL_TOOLCHAIN)/toolchain/gcc-arm-none-eabi/bin/

CC = $(ARM_NONE_EABI_PATH)arm-none-eabi-gcc

# src;obj;bin / NDS :D
INTERFACE = NDS

GAME_TITLE		:= test
GAME_SUBTITLE	:= jane mp3 decoder
GAME_AUTHOR		:= Marilyn Nozomi
GAME_ICON		:= icon.gif
GAME_FULL_TITLE := $(GAME_TITLE);$(GAME_SUBTITLE);$(GAME_AUTHOR)
NDS_NAME = $(EXEC_NAME).nds
ELF_NAME = $(EXEC_NAME).elf
NITROFSDIR := nitrofs
DEFINES	:= -DARM9 -D__NDS__
SPECS := $(BLOCKSDS)/sys/crts/ds_arm9.specs
OPTS := $(OPTS) -I. -I$(BLOCKSDS)/libs/libnds/include -I$(BLOCKSDS)/libs/maxmod/include
LIBS = -lmm9 -lnds9 -lm -lc
LDFLAGS = -L$(BLOCKSDS)/libs/libnds/lib -L$(BLOCKSDS)/libs/maxmod/lib
ARM7ELF	:= $(BLOCKSDS)/sys/arm7/main_core/arm7_dswifi_maxmod.elf

CFLAGS = $(OPTS) \
		 $(LIBS) \
		 $(DEFINES) \
		 -marm \
		 -mcpu=arm946e-s+nofp \
		 -O3 \
		 -ffast-math \
		 -fomit-frame-pointer \
		 -std=gnu17 \
		 -fmodulo-sched-allow-regmoves \
		 -fno-ira-share-save-slots \
		 -Wwrite-strings \
		 -Wno-format \
		 -Wpointer-arith \
		 -fno-exceptions \
		 -specs=$(SPECS)

OBJS := $(OBJS) \
		$(OBJ_DIR)/main.o \
		$(OBJ_DIR)/sound.o
		
# Start Nintendo DS build requirements!
all: $(BIN_DIR)/$(NDS_NAME)

# Include NitroFS directory!
NDSTOOL_ARGS	:= -d $(NITROFSDIR)
		
$(BIN_DIR)/$(NDS_NAME): $(BIN_DIR)/$(ELF_NAME) $(NITROFSDIR)
	@echo "  NDSTOOL $@"
	$(BLOCKSDS)/tools/ndstool/ndstool -c $@ \
		-7 $(ARM7ELF) -9 $(BIN_DIR)/$(ELF_NAME) \
		-b $(GAME_ICON) "$(GAME_FULL_TITLE)" \
		$(NDSTOOL_ARGS)

$(BIN_DIR)/$(ELF_NAME): $(OBJ_DIR) $(OBJS)
	@echo Linking...
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) \
	-o $(BIN_DIR)/$(ELF_NAME) $(LIBS)

# Clean up the objects.
clean:
	rm -rf $(OBJ_DIR)/*
	
# Make all required directories!
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	
$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/sound.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@

$(OBJ_DIR)/sound.o: $(SRC_DIR)/sound.c $(SRC_DIR)/sound.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(WFLAGS) -c $< -o $@