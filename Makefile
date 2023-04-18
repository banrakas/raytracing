CFLAGS = -O3 -Wall
LDFLAGS = -lm -lSDL2

build: src/main.c
	gcc $(CFLAGS) src/*.c $(LDFLAGS) -o raytracer

run: raytracer
	rm image.ppm
	./raytracer > image.ppm
