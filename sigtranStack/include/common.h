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
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include "iniparser.h"
#include "dictionary.h"

//#include <util.h>
//#include "dictionary.h"

#define MAX_BUFFER			1024
#define SUA_PORT_NUM		14001
#define M3UA_PORT_NUM		2905
#define LOCALTIME_STREAM	0
#define CONTROL_STREAM		0
#define DATA_STREAM			1
#define GMT_STREAM			1
#define MAX_LISTENING_SOCK	5

#ifndef LOCAL_HOST
	#define LOCAL_HOST			"127.0.0.1"
#endif 

#define freeze(ptr)	free(ptr);	\
					ptr = NULL;	

#define short_delay		usleep(100)
#define medium_delay	usleep(35000)

/*** Adaptation Layer Tag field declarations ***/
#define UA_TAG_RESERVED		0x0000
#define UA_TAG_INFO_STRING	0x0004
#define UA_TAG_RTNG_CTXT	0x0006
#define UA_TAG_DIAG_INFO	0x0007
#define UA_TAG_HTBEAT_DATA	0x0009
#define UA_TAG_TRFF_MODE	0x000b
#define UA_TAG_ERR_CODE		0x000c
#define UA_TAG_STATUS		0x000d
#define UA_TAG_ASP_IDF		0x0011


/** Address indicators **/
#define ADDRIND_INT			0x00  /* International addr indicator */
#define ADDRIND_NAT			0x01  /* National address indicator   */

/** subsystem numbers**/
#define SUBSYS_NONE     	0x00  /* subsystem unknown or not used*/
#define SUBSYS_SCCPMGMT 	0x01  /* SCCP management subsystem    */
#define SUBSYS_ISUP     	0x03  /* ISUP subsystem               */
#define SUBSYS_OMAP     	0x04  /* operations, maint., admin.   */
#define SUBSYS_MAP      	0x05  /* mobile application part      */
#define SUBSYS_HLR      	0x06  /* home location register       */
#define SUBSYS_VLR      	0x07  /* visitor location register    */
#define SUBSYS_MSC      	0x08  /* mobile switching center      */
#define SUBSYS_EIR      	0x09  /* equipment ident. register    */
#define SUBSYS_AUTH     	0x0A  /* authentication center        */

#define R_SHIFT_24			24  /* Tag: Right Shift By 24 Bit */
#define R_SHIFT_16			16  /* Tag: Right Shift By 16 Bit */ 
#define R_SHIFT_8			8   /* Tag: Right Shift By 8 Bit */

#define ASCII_ZERO			48

#define POINT_CODE_LEN		4
#define GT_LEN				12

/** Common error Codes **/
#define MALLOC_FAILURE		0
#define SUCCESS				1


/*
 * convert an integer 
 * to hex stream
 */
void convert_int_to_hex (unsigned int, unsigned char *);

/*
 * convert a hex stream 
 * into point code format
 */
void convert_hex_to_pc (unsigned char *, char *);

/*
 * convert an ip 
 * address in char * to
 * its integer equivalent
 * in char *
 */
void inline inet_aton_buff (char *, unsigned char *);

/* 
 * encode number/time in gsm
 * hex byte stream format
 */
void encode_number_or_time (unsigned char *, 
			unsigned char *, int);

/*
 * convert a gsm hex byte stream
 * to normal character stream
 */
void decode_number_or_time (unsigned char *, 
			unsigned char *, int);

/*
 * convert a byte with hex values to int 
 */
int convert_hex_to_int (unsigned char *);


#endif /* __COMMON_H__ */
