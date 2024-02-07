build: main.c
	mkdir -p build
	cc main.c -g -pedantic -o build/main.o -lm -lSDL2 

clean:
	rm -rf build
