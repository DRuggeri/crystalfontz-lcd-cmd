#Makefile for Crystalfontz UNIX examples

CC = gcc
LIBS = 
CFLAGS = -g -O -Iinclude -Wall
LDFLAGS = -g

LIBSRC = include/serial.c include/cf_packet.c include/show_packet.c
LIBOBJ = $(LIBSRC:%.c=%.o)

all: cmd

cmd: cmd.o $(LIBOBJ)
	$(CC) $(LDFLAGS) $(LIBOBJ) cmd.o -o lcd-cmd

clean:
	rm -f $(LIBOBJ) 632_634/example-632_634.o example-632_634 631/example-631.o example-631 633/example-633.o example-633 635/example-635.o example-635
