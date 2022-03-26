CC=gcc
CFLAGS=-ggdb -Wall -Wextra --std=c99 --pedantic
RFLAGS=-O3
LIBS=`pkg-config --libs raylib`


run: yopuyopu
	./yopuyopu

release: main.c game.c game.h
	$(CC) $(RFLAGS) -o yopuyopu main.c $(LIBS)
yopuyopu: main.c game.c game.h
	$(CC) $(CFLAGS) -o yopuyopu main.c $(LIBS)
