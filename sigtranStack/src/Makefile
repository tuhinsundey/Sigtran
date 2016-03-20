### Makefile for SRC directory ###

## Specifying Directory ##
TOPDIR = ..
PWD = $(shell pwd)

##Specifying source and includes##
INCLUDES =	-I$(TOPDIR)/include/ -I$(TOPDIR)/include/m3ua/ \
			-I$(TOPDIR)/include/sccp/ -I$(TOPDIR)/include/tcap/
ROUTE_SRC	= $(PWD)/route_logic.c 
SMSC_REAR	= $(PWD)/smsc_rear.c
SMSC_HLR	= $(PWD)/hlr_connect.c

##Compiler and compiler flags##
CC = gcc 
CC_FLAGS = -g -c 

SRC = $(TOPDIR)/start.c $(TOPDIR)/src/common.c \
		$(TOPDIR)/src/proto/m3ua/m3ua_handshake.c\
		$(TOPDIR)/src/proto/m3ua/m3ua_headers.c


##Try compiling this##
all:
	$(CC) $(CC_FLAGS) $(SMSC_REAR) $(SRC) $(INCLUDES)
	$(CC) $(CC_FLAGS) $(SMSC_HLR) $(SRC) $(INCLUDES)
	$(CC) $(CC_FLAGS) $(ROUTE_SRC) $(INCLUDES)

clean:
	rm -f *.out *.o tags

