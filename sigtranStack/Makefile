## Makefile for the sigtran stack ##

TOPDIR = $(shell pwd)

INCLUDES = -I$(TOPDIR)/include/ -I$(TOPDIR)/include/m3ua/ -I$(TOPDIR)/include/sccp/ \
			-I$(TOPDIR)/include/tcap/ -I$(TOPDIR)/src/storage/mysql -I/usr/include/mysql \
			-I$(TOPDIR)/include/src/storage/memcached

### Libraries to be used ###
LIBS = -lsctp -liniparser -lmysqlclient_r -lmemcached -lpthread

### Directories ###
SRC_DIR		= $(TOPDIR)/src
PROTO_DIR	= $(SRC_DIR)/proto
SOCKETS		= $(SRC_DIR)/socket
STORAGE		= $(SRC_DIR)/storage
M3UA_DIR	= $(PROTO_DIR)/m3ua 
TCAP		= $(PROTO_DIR)/tcap
SCCP		= $(PROTO_DIR)/sccp
GSM_MAP		= $(PROTO_DIR)/gsm_map

### Object file formats ###
OBJ_FILE = *.o

### Source files to be compiled ###
SRC	= $(TOPDIR)/start.c	$(SRC_DIR)/common.c \
		$(M3UA_DIR)/m3ua_handshake.c\
		$(M3UA_DIR)/m3ua_headers.c

### Compiler and compiler flags ###
CC = gcc 
CFLAGS = -Wall -g -o

### Target binary ###
SIGTRAN_BIN = gsmsc_sig

### Directory for all object files ###
BIN_DIR = $(TOPDIR)/bin

all:
	(cd $(SRC_DIR); make; mv *.o $(BIN_DIR))
	(cd $(SOCKETS); make; mv *.o $(BIN_DIR))
	(cd $(M3UA_DIR); make; mv *.o $(BIN_DIR))
	(cd $(TCAP); make; mv *.o $(BIN_DIR))
	(cd $(SCCP); make; mv *.o $(BIN_DIR))
	(cd $(GSM_MAP); make; mv *.o $(BIN_DIR))
	(cd $(STORAGE); make; mv *.o $(BIN_DIR))
	$(CC) $(CFLAGS) $(SIGTRAN_BIN) $(BIN_DIR)/$(OBJ_FILE) $(INCLUDES) $(LIBS)

clean:
	rm -f *.out *.o $(BIN_DIR)/$(OBJ_FILE) $(SIGTRAN_BIN) tags

