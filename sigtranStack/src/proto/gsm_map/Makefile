### Makefile for socket layer ###

## Specifying Directory ##
TOPDIR = ../../..
PWD = $(shell pwd)

##Specifying source and includes##
INCLUDES = -I$(TOPDIR)/include/ \
			-I$(TOPDIR)/include/gsm_map/ \
			-I$(TOPDIR)/include/tcap/
SRC	= $(PWD)/map.c

##Compiler and compiler flags##
CC = gcc 
CC_FLAGS = -c -g 

##Trying compiling this##
all:
	$(CC) $(CC_FLAGS) $(SRC) $(INCLUDES)

clean:
	rm -f *.out *.o tags

