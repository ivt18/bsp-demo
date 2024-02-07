build: main.c
	mkdir -p build
	cc main.c -g -pedantic -o build/main.o -lSDL2

clean:
	rm -f build/main.o
