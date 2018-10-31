CC=gcc
TARGET_BIN=../../bin
CFLAGS=-I. -std=c99 -O2

all: argparse.h core.h stack.h main.c
	$(CC) -o $(TARGET_BIN)/xtail main.c  $(CFLAGS)
