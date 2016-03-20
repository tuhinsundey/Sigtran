### Makefile for socket layer ###
## Specifying Directory ##

TOPDIR = ../..
PWD = $(shell pwd)

##Specifying source and includes##
INCLUDES = -I$(TOPDIR)/include/  \
		-I/usr/include/mysql	

LIBRARIES = -liniparser -lmemcached -lmysqlclient_r

##Compiler and compiler flags##
CC = gcc 
CC_FLAGS = -c -g 

##Trying compiling this##
all:
	$(CC) $(CC_FLAGS) $(PWD)/storage.c $(INCLUDES) $(LIBRARIES)
	$(CC) $(CC_FLAGS) $(PWD)/mysql/storage_mysql.c $(INCLUDES) $(LIBRARIES)
	$(CC) $(CC_FLAGS) $(PWD)/memcached/storage_memcached.c $(INCLUDES) $(LIBRARIES)
		
#	$(CC) -o gsmsc $(PWD)/*.o $(INCLUDS) $(LIBRARIES)
clean:
	rm -f *.out *.o tags
