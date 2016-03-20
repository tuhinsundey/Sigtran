### Makefile for socket layer ###

## Specifying Directory ##
TOPDIR = ../../..
PWD = $(shell pwd)

##Specifying source and includes##
INCLUDES =	-I$(TOPDIR)/include/ -I$(TOPDIR)/include/m3ua/ \
			-I$(TOPDIR)/include/sccp/ -I$(TOPDIR)/include/socket\
			-I$(TOPDIR)/include/tcap/
SRC	= $(PWD)/m3ua_nodes.c 

##Compiler and compiler flags##
CC = gcc 
CC_FLAGS = -g -c 

##Trying compiling this##
all:
	$(CC) $(CC_FLAGS) $(SRC) $(INCLUDES)

clean:
	rm -f *.out *.o tags

