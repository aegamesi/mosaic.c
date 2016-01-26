CC=gcc

mosaic: mosaic.c lodepng.c
	$(CC) -o mosaic mosaic.c lodepng.c -I. -O3