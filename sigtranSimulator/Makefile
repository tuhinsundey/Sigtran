#
# Makefile for sigtran simulator
#
TOPDIR = $(shell pwd)

# Compiler for C code being used
CC = gcc

# Standard flags for compiler
CFLAGS = -Wall -ldl -g -o

# Libraries to be used
LIBRARIES = -lsctp -liniparser -lmysqlclient_r

SRC = $(TOPDIR)/src
INCLUDE_FILES = $(TOPDIR)/include

# Include path to find standard headers
INCLUDES =  -I/user/include -Iinclude 

# SUA Source files
SUA_SOURCES	=	$(SRC)/sctp_sua.c $(SRC)/sua.c $(SRC)/tcap.c $(SRC)/util.c $(SRC)/caching.c 
M3UA_SOURCES	=	$(SRC)/parse_message.c $(SRC)/sctp_m3ua.c  $(SRC)/m3ua.c $(SRC)/sccp.c  $(SRC)/tcap.c $(SRC)/util.c $(SRC)/caching.c

# Header files
HEADERS	=	$(INCLUDE_FILES)/sua.h $(INCLUDE_FILES)/tcap.h  $(INCLUDE_FILES)/common.h  $(INCLUDE_FILES)/util.h $(INCLUDE_FILES)/parse_message.h

# Target binaries
SUA = sua_peer
M3UA = m3ua_peer

# simulator peer
all: 
	$(CC) $(INCLUDES) $(CFLAGS) $(SUA) $(SUA_SOURCES) $(HEADERS) $(LIBRARIES)
	$(CC) $(INCLUDES) $(CFLAGS) $(M3UA) $(M3UA_SOURCES) $(HEADERS) $(LIBRARIES)


# clean all binaries
clean:
	rm -f $(SUA) $(M3UA) *.o *.log tags

