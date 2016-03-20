#ifndef __SCCP_H__
#define __SCCP_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SCCP_MSG_LEN				1024
#define	MIN_SCCP_CLDT_LEN				101

#define SCCP_VERSION_1					0x01
#define	SCCP_RESERVED_BYTE				0x00


/**** Message classes start ****/

/* SCCP Management messages */
#define SCCP_MSG_CLASS_MGMT				0x01 
/* Signaling Network management DUNA, SCON etc */
#define SCCP_MSG_CLASS_SNM				0x02 
/* Application server process management maintenance eg ASP up/down */
#define SCCP_MSG_CLASS_ASPSM			0x03 
/* ASP traffic maintenance eg ASP active/inactive */
#define SCCP_MSG_CLASS_ASPTM			0x04 
/* Connectionless messages */
#define SCCP_MSG_CLASS_CLDT				0x07 
/*/ Connectionless messages */
#define SCCP_MSG_CLASS_CO				0x08 
/*Message classes end */

/**** Message type start ****/
#define SCCP_MSG_TYPE_SNM_DUNA          0x01
#define SCCP_MSG_TYPE_SNM_DAVA          0x02 
#define SCCP_MSG_TYPE_SNM_DAUD          0x03 
#define SCCP_MSG_TYPE_SNM_SCON          0x04 
#define SCCP_MSG_TYPE_SNM_DUPU          0x05 
#define SCCP_MSG_TYPE_SNM_DRST          0x06 

#define SCCP_MSG_TYPE_ASPSM_UP          0x01
#define SCCP_MSG_TYPE_ASPSM_DOWN		0x02
#define SCCP_MSG_TYPE_ASPSM_HTBT		0x03
#define SCCP_MSG_TYPE_ASPSM_UPACK		0x04
#define SCCP_MSG_TYPE_ASPSM_DWNACK		0x05
#define SCCP_MSG_TYPE_ASPSM_HBTACK		0x06

#define SCCP_MSG_TYPE_ASPTM_ACT			0x01
#define SCCP_MSG_TYPE_ASPTM_INACT		0x02
#define SCCP_MSG_TYPE_ASPTM_ACTACK		0x03
#define SCCP_MSG_TYPE_ASPTM_INACTACK	0x04

#define SCCP_MSG_TYPE_CL_CLDT          	0x01
#define SCCP_MSG_TYPE_CL_CLDR          	0x02
#define SCCP_MSG_TYPE_CO_CORE          	0x01
#define SCCP_MSG_TYPE_CO_COAK          	0x02
#define SCCP_MSG_TYPE_CO_COREF			0x03
#define SCCP_MSG_TYPE_CO_RELRE			0x04
#define SCCP_MSG_TYPE_CO_RELCO			0x05
#define SCCP_MSG_TYPE_CO_RESCO			0x06
#define SCCP_MSG_TYPE_CO_RESRE        	0x07
#define SCCP_MSG_TYPE_CO_CODT         	0x08
#define SCCP_MSG_TYPE_CO_CODA          	0x09
#define SCCP_MSG_TYPE_CO_COERR			0x0a

#define TAG_SCCP_UNITDATA				0x09

#define SSN_TYPE_MGMT					0x00
#define SSN_TYPE_HLR					0x06
#define SSN_TYPE_VLR					0x07
#define SSN_TYPE_MSC					0x08
#define SSN_TYPE_EIR					0x09

#define ASPSM_MSG_LEN					8
#define ASPTM_MSG_LEN					16


#define R_SHIFT_24              24
#define R_SHIFT_16              16
#define R_SHIFT_8               8


#define	reserved_bytes3					3
#define	reserved_bytes4					4

int create_sccp_CL (unsigned char *sccp_message, 
					char message_type, char *dest_ip, char *src_ip);
int create_sccp_CO (unsigned char *sccp_message, 
					char message_type);
int create_sccp_SNM (unsigned char *sccp_message, 
					char message_type);
int create_sccp_ASPSM (unsigned char *sccp_message, 
					char message_type);
int create_sccp_ASPTM (unsigned char *sccp_message, 
					char message_type);

int create_sccp_unitdata (unsigned char *sccp_message, 
                    unsigned char *called_pty_GT, 
                    unsigned char *calling_pty_GT);

#endif 
	
int create_sccp_unitdata_service (unsigned char *sccp_message, 
					unsigned char *called_pty_GT, 
					unsigned char *calling_pty_GT, char error_type);
