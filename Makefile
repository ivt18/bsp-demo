CC	:= cc

CCFLAGS	:= -std=c2x
CCFLAGS += -O2
CCFLAGS += -g
CCFLAGS += -Wall
CCFLAGS += -Wextra
CCFLAGS += -Wpedantic

LDFLAGS := -lm
LDFLAGS += -lSDL2

BIN := bin
OUT := game

build: main.c
	mkdir -p $(BIN)
	$(CC) main.c $(CCFLAGS) -o $(BIN)/$(OUT) $(LDFLAGS)

clean:
	rm -rf $(BIN)
