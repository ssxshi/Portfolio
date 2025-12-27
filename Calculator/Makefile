CC = gcc
CFLAGS = -I./raylibInclude -I./Include
LDFLAGS = -L./raylibLib
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = *.c
OUT = main

$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS) $(LIBS)

run:
	.\$(OUT)

clean:
	del $(OUT)
