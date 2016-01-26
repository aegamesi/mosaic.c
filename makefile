CC=gcc

mosaic: mosaic.c lodepng.c
	$(CC) -o build/mosaic mosaic.c lodepng.c -I. -O3
