#ifndef __CACHING_H__
#define __CACHING_H__

#include <stdio.h>
#include <time.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"	
#include "tcap.h"

#define CACHED_ADDR     "192.168.56.14"
#define CACHED_PORT     6001

typedef struct {
	void *(*getNewJdbObject) ();
	void (*destroyJdbObject)(void *);

	void (*addPacketS)(void *, char *);
	void (*addPacketI)(void *, int);
	void (*addPacketL)(void *, char *);
	void (*addPacketD)(void *, char *);
	void (*addPacketF)(void *, char *);
	void (*addPacketB)(void *, char *);
	void (*addPacketC)(void *, char *);

	void (*addPacketNS)(void *, char *, int);
	void (*addPacketNI)(void *, int, int);
	void (*addPacketNL)(void *, char *, int);
	void (*addPacketND)(void *, char *, int);
	void (*addPacketNF)(void *, char *, int);
	void (*addPacketNB)(void *, char *, int);
	void (*addPacketNC)(void *, char *, int);

	char *(*encodeJdbObjectWL)(void *, int);
	char *(*encodeJdbObject)(void *, int, int *);
	void *(*decodeJdbObject)(char *, int, int);
	char  (*getTypeAt)(void *, int);
	void *(*getValueAt)(void *, int );
} caching_callbacks;

void insert (sms_fields *);

//void init_caching ();
void init_caching (dictionary *);
static void read_configuration_file (dictionary *);

#endif /* end tag for header */
