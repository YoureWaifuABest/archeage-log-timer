CC=gcc
CFLAGS=-Wall -lncurses -lform -lpthread

all: 
	$(CC) -o log_timer main.c $(CFLAGS)
