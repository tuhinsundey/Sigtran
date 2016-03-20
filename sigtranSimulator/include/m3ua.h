#ifndef __M3UA_H__
#define __M3UA_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tcap.h"
#include "sccp.h"
#include "parse_message.h"


#define	MAX_M3UA_MSG_LEN		1024
#define	MIN_M3UA_CLDT_LEN		101

#define M3UA_VERSION_1			0x01
#define	M3UA_RESERVED_BYTE		0x00

#define R_SHIFT_24				24
#define R_SHIFT_16				16
#define R_SHIFT_8				8

#define TAG_SCCP				3
#define TAG_NI					2
#define TAG_MP					4
#define TAG_SLS					6

/**** Message classes start ****/

/* Management messages */
#define M3UA_MSG_CLASS_MGMT		0x00 
/* M3UA transfer messages messages */
#define M3UA_MSG_CLASS_TX_MSG	0x01 
/* Signaling Network management DUNA, SCON etc */
#define M3UA_MSG_CLASS_SNM		0x02 
/* Application server process management maintenance eg ASP up/down */
#define M3UA_MSG_CLASS_ASPSM	0x03 
/* ASP traffic maintenance eg ASP active/inactive */
#define M3UA_MSG_CLASS_ASPTM	0x04 
/* Routing Key management messages */
#define M3UA_MSG_CLASS_RKM		0x09

/*Message classes end */

/**** Message type start ****/
#define M3UA_MSG_TYPE_SNM_DUNA          0x01
#define M3UA_MSG_TYPE_SNM_DAVA          0x02 
#define M3UA_MSG_TYPE_SNM_DAUD          0x03 
#define M3UA_MSG_TYPE_SNM_SCON          0x04 
#define M3UA_MSG_TYPE_SNM_DUPU          0x05 
#define M3UA_MSG_TYPE_SNM_DRST          0x06 

#define M3UA_MSG_TYPE_ASPSM_UP			0x01
#define M3UA_MSG_TYPE_ASPSM_DOWN		0x02
#define M3UA_MSG_TYPE_ASPSM_HTBT		0x03
#define M3UA_MSG_TYPE_ASPSM_UPACK		0x04
#define M3UA_MSG_TYPE_ASPSM_DWNACK		0x05
#define M3UA_MSG_TYPE_ASPSM_HBTACK		0x06

#define M3UA_MSG_TYPE_ASPTM_ACT         0x01
#define M3UA_MSG_TYPE_ASPTM_INACT       0x02
#define M3UA_MSG_TYPE_ASPTM_ACTACK      0x03
#define M3UA_MSG_TYPE_ASPTM_INACTACK	0x04

#define M3UA_MSG_TYPE_PAYLOAD_RES     	0x00
#define M3UA_MSG_TYPE_PAYLOAD_DATA     	0x01

#define M3UA_MSG_TYPE_CL_CLDT          	0x01
#define M3UA_MSG_TYPE_CL_CLDR          	0x02
#define M3UA_MSG_TYPE_CO_CORE          	0x01
#define M3UA_MSG_TYPE_CO_COAK          	0x02
#define M3UA_MSG_TYPE_CO_COREF         	0x03
#define M3UA_MSG_TYPE_CO_RELRE         	0x04
#define M3UA_MSG_TYPE_CO_RELCO         	0x05
#define M3UA_MSG_TYPE_CO_RESCO         	0x06
#define M3UA_MSG_TYPE_CO_RESRE        	0x07
#define M3UA_MSG_TYPE_CO_CODT         	0x08
#define M3UA_MSG_TYPE_CO_CODA          	0x09
#define M3UA_MSG_TYPE_CO_COERR          0x0a

#define ASPSM_MSG_LEN			8
#define ASPTM_MSG_LEN			16

#define	reserved_bytes3			3
#define	reserved_bytes4			4

/* 
 * byte positions in 
 * a SUA header.
 */
typedef	enum {
	version = 0,
	reserved,
	message_class, 
	message_type,
}m3ua_byte_pos;

/*
 * states of ASP 
 * in a node (IPSP)
 */
typedef enum {
	asp_init = 0,
	asp_up_tx,
	asp_up_ack_tx,
	asp_up_done,
	asp_active_tx,
	asp_active_ack_tx,
	asp_active_done,
	asp_inactive_tx,
	asp_inactive_ack_tx,
	asp_inactive_done,
	asp_down_tx,
	asp_down_ack_tx,
}m3ua_asp_states;

typedef enum {
	asp_sg_init = 0,
	asp_sg_up_tx,
	asp_sg_up_ack_tx,
	asp_sg_notify_tx,
	asp_sg_up_done,
	asp_sg_active_tx,
	asp_sg_active_ack_tx,
	asp_sg_active_done,
	asp_sg_inactive_tx,
	asp_sg_inactive_ack_tx,
	asp_sg_inactive_done,
	asp_sg_down_tx,
	asp_sg_down_ack_tx,
}m3ua_sg_states;

/*Input data structure for SCCP */
typedef struct _sccp_data_t {
	unsigned char* called_pty_GT;
	unsigned char* calling_pty_GT;
}sccp_fields;


/* Input data structure 
 * for creating m3ua packet*/
typedef struct _m3ua_data_t {
	unsigned int dest_pc;
	unsigned int source_pc;
	sccp_fields sccp_input_data;
}m3ua_fields;

int create_m3ua_CL (unsigned char *m3ua_message, 
					char message_type);
int create_m3ua_CO (unsigned char *m3ua_message, 
					char message_type);
int create_m3ua_SNM (unsigned char *m3ua_message, 
					char message_type);
int create_m3ua_ASPSM (unsigned char *m3ua_message, 
					char message_type);
int create_m3ua_ASPTM (unsigned char *m3ua_message, 
					char message_type, char *routing_context);
int create_m3ua_data_SRI (unsigned char *m3ua_message);
int create_m3ua_data_SRI_ACK (unsigned char *m3ua_message, unsigned int trx_id);
int create_m3ua_data_MO_SM (unsigned char *m3ua_message, char *imsi);
int create_m3ua_data_MO_SM_ACK (unsigned char *m3ua_message, unsigned int trx_id, char type);

//char *DA_MSISDN = NULL;
#endif 
