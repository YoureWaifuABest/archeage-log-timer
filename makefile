CC=gcc
CFLAGS=-Wall -lsodium -lncurses -lform -lpthread

all: 
	$(CC) -o log_timer main.c $(CFLAGS)
