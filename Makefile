CC=gcc
CFLAGS=-ggdb -Wall -Wextra --std=c99 --pedantic
LIBS=`pkg-config --libs raylib`


run: yopuyopu
	./yopuyopu

yopuyopu: main.c game.c game.h
	$(CC) $(CFLAGS) -o yopuyopu main.c $(LIBS)
