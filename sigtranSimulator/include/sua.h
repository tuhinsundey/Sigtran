#ifndef __SUA_H__
#define __SUA_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SUA_MSG_LEN			1024
#define	MIN_SUA_CLDT_LEN		101

#define SUA_VERSION_1			0x01
#define	SUA_RESERVED_BYTE		0x00


/**** Message classes start ****/

/* SUA Management messages */
#define SUA_MSG_CLASS_MGMT		0x01 
/* Signaling Network management DUNA, SCON etc */
#define SUA_MSG_CLASS_SNM		0x02 
/* Application server process management maintenance eg ASP up/down */
#define SUA_MSG_CLASS_ASPSM		0x03 
/* ASP traffic maintenance eg ASP active/inactive */
#define SUA_MSG_CLASS_ASPTM		0x04 
/* Connectionless messages */
#define SUA_MSG_CLASS_CLDT		0x07 
/*/ Connectionless messages */
#define SUA_MSG_CLASS_CO		0x08 
/*Message classes end */

/**** Message type start ****/
#define SUA_MSG_TYPE_SNM_DUNA          	0x01
#define SUA_MSG_TYPE_SNM_DAVA          	0x02 
#define SUA_MSG_TYPE_SNM_DAUD          	0x03 
#define SUA_MSG_TYPE_SNM_SCON          	0x04 
#define SUA_MSG_TYPE_SNM_DUPU          	0x05 
#define SUA_MSG_TYPE_SNM_DRST          	0x06 

#define SUA_MSG_TYPE_ASPSM_UP          	0x01
#define SUA_MSG_TYPE_ASPSM_DOWN         0x02
#define SUA_MSG_TYPE_ASPSM_HTBT         0x03
#define SUA_MSG_TYPE_ASPSM_UPACK        0x04
#define SUA_MSG_TYPE_ASPSM_DWNACK       0x05
#define SUA_MSG_TYPE_ASPSM_HBTACK       0x06

#define SUA_MSG_TYPE_ASPTM_ACT          0x01
#define SUA_MSG_TYPE_ASPTM_INACT        0x02
#define SUA_MSG_TYPE_ASPTM_ACTACK       0x03
#define SUA_MSG_TYPE_ASPTM_INACTACK     0x04

#define SUA_MSG_TYPE_CL_CLDT          	0x01
#define SUA_MSG_TYPE_CL_CLDR          	0x02
#define SUA_MSG_TYPE_CO_CORE          	0x01
#define SUA_MSG_TYPE_CO_COAK          	0x02
#define SUA_MSG_TYPE_CO_COREF          	0x03
#define SUA_MSG_TYPE_CO_RELRE          	0x04
#define SUA_MSG_TYPE_CO_RELCO         	0x05
#define SUA_MSG_TYPE_CO_RESCO          	0x06
#define SUA_MSG_TYPE_CO_RESRE        	0x07
#define SUA_MSG_TYPE_CO_CODT         	0x08
#define SUA_MSG_TYPE_CO_CODA          	0x09
#define SUA_MSG_TYPE_CO_COERR          	0x0a

#define ASPSM_MSG_LEN			8
#define ASPTM_MSG_LEN			16

#define	reserved_bytes3			3
#define	reserved_bytes4			4

typedef	enum {
	version = 0,
	reserved,
	message_class, 
	message_type,
}sua_byte_pos;

int create_sua_CL (unsigned char *sua_message, 
					char message_type, char *dest_ip, char *src_ip);
int create_sua_CO (unsigned char *sua_message, 
					char message_type);
int create_sua_SNM (unsigned char *sua_message, 
					char message_type);
int create_sua_ASPSM (unsigned char *sua_message, 
					char message_type);
int create_sua_ASPTM (unsigned char *sua_message, 
					char message_type);

#endif 
