CC	:= gcc
LD	:= gcc

ifdef OS
	RM := del /Q /F
else
	RM := rm -rf
endif

LIB_PATH := lib
INC_PATH := include

CCFLAGS	:= -std=c2x
CCFLAGS += -O2
CCFLAGS += -g
CCFLAGS += -Wall
CCFLAGS += -Wextra
CCFLAGS += -Wpedantic
CCFLAGS += -c

ifdef OS
	CCFLAGS += -I $(INC_PATH)
endif

ifdef OS
	LDFLAGS := -L $(LIB_PATH)
	LDFLAGS	+= -lm
	LDFLAGS += -lmingw32
	LDFLAGS += -lSDL2main
	LDFLAGS += -lSDL2
else
	LDFLAGS := -lm
	LDFLAGS += -lSDL2
endif

BIN := bin
# SRC := $(shell find src -name "*.c")
SRC := src/main.c src/wad_loader.c src/vector.c src/bsp-tree.c
OBJ := $(SRC:%.c=$(BIN)/%.o)

ifdef OS
	OUT := game.exe
else
	OUT := game
endif

$(BIN):
	mkdir -p $(BIN)/src

$(OBJ): $(BIN)/%.o: %.c
	$(CC) $< $(CCFLAGS) -o $@

build: $(OBJ) $(BIN)/src/main.o
	$(LD) $(OBJ) -o $(BIN)/$(OUT) $(LDFLAGS)

clean:
	$(RM) $(BIN)
