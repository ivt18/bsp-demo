CC	:= cc
LD	:= cc

CCFLAGS	:= -std=c2x
CCFLAGS += -O2
CCFLAGS += -g
CCFLAGS += -Wall
CCFLAGS += -Wextra
CCFLAGS += -Wpedantic
CCFLAGS += -c

LDFLAGS := -lm
LDFLAGS += -lSDL2

BIN := bin
OUT := game
SRC := $(shell find src -name "*.c")
OBJ := $(SRC:%.c=$(BIN)/%.o)

$(BIN):
	mkdir -p $(BIN)/src

$(OBJ): $(BIN)/%.o: %.c
	$(CC) $< $(CCFLAGS) -o $@

build: $(OBJ) $(BIN)/src/main.o
	$(LD) $(OBJ) -o $(BIN)/$(OUT) $(LDFLAGS)

clean:
	rm -rf $(BIN)
