/*
 * Common definitions valid for
 * all protocols 
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <signal.h>

#include "util.h"
#include "dictionary.h"

#define MAX_BUFFER			1024
#define SUA_PORT_NUM		14001
#define M3UA_PORT_NUM		14003
#define LOCALTIME_STREAM	0
#define CONTROL_STREAM		0
#define DATA_STREAM			1
#define GMT_STREAM			1

#define LOCAL_HOST			"127.0.0.1"

#define freeze(ptr)	free(ptr);	\
					ptr = NULL;	


#endif /* __COMMON_H__ */
