#ifndef _SCCP_H_
#define _SCCP_H_

#include "tcap.h"
#include "common.h"
#define SSN_TYPE_MSC    			0x08
#define SSN_TYPE_HLR    			0x06

#define SCCP_SRI        			0x00
#define SCCP_SRI_ACK    			0x01
#define SCCP_SMS        			0x02

/* SCCP message type codes */
#define SCCP_MSG_TYPE_UDT			0x09	
#define SCCP_MSG_TYPE_UDTS			0x0a
#define SCCP_MSG_TYPE_XUDT			0x11
#define SCCP_MSG_TYPE_XUDTS			0x12
#define SCCP_MSG_TYPE_LUDT			0x13
#define SCCP_MSG_TYPE_LUDTS			0x14

/* SCCP protocol class codes*/
#define SCCP_PROTO_CLASS_0			0x00
#define SCCP_PROTO_CLASS_1			0x01
#define SCCP_PROTO_CLASS_2			0x02
#define SCCP_PROTO_CLASS_3			0x03

/* SCCP return cause codes */
#define SCCP_ERR_NO_TRN_0			0x00
#define SCCP_ERR_NO_TRN_1			0x01
#define SCCP_ERR_SYS_CONG			0x02
#define SCCP_ERR_SYS_FAIL			0x03
#define SCCP_ERR_UNEQP_USR			0x04
#define SCCP_ERR_MTP_FAIL			0x05
#define SCCP_ERR_NET_CONG			0x06
#define SCCP_ERR_UNQLF				0x07
#define SCCP_ERR_MSG_TP				0x08
#define SCCP_ERR_LOCAL_PR			0x09
#define SCCP_ERR_NO_REASSEMBLY		0x0a
#define SCCP_ERR_SCCP_FAIL			0x0b
#define SCCP_ERR_HOP_COUNTER		0x0c
#define SCCP_ERR_SGMT_NO_SUPPORT	0x0d
#define SCCP_ERR_SGMT_FAIL			0x0e

#define SCCP_MSG_CLASS				0x01
#define MAX_SCCP_MSG_LEN			768
#define MAX_ADDR_LEN				15

#define CHECK_FOR_PC				0x01
#define CHECK_FOR_SSN				0x02
#define CHECK_FOR_GT				0x3c
#define CHECK_FOR_ROUTE				0x40
#define CHECK_FOR_GT_PARAMS			0x3c
#define PC_PRESENT					0x01
#define SSN_PRESENT					0x02
#define GT_NOT_PRESENT				0x00
#define ROUTE_ON_GT					0x00
#define ROUTE_ON_SSN				0x01
#define NATURE_OF_ADDR_PRESENT		0x04
#define TRANSLATION_TYPE_PRESENT	0x08
#define	TR_NP_PRESENT				0x0c
#define TR_NP_NA_PRESENT			0x10
#define MAX_PC_LEN					2
#define MAX_GT_LEN					12
#define	MAX_TCAP_DATA				256
#define TRUE						1
#define FALSE						0
#define ADDRESS_IND_CLD				0x92
#define ADDRESS_IND_CLG				0x12
#define NO_OF_POINTERS_UDT			3
#define GT_TRANSLATION_TYPE			0x00
#define GT_NUMBER_PLAN				0x12
#define GT_NATURE_ADDRESS			0x04

/**** Protocol Class tag and primitives ****/
static const char protocol_class_tag[2] = {0x01, 0x15};
static const char protocol_class_val    = 0x0;

/**** Address tags and primitives ****/
static const char source_address_tag[2] = {0x01, 0x02};
static const char dest_address_tag[2]   = {0x01, 0x03};
/* Route on SSN + IP (4) */
static const char routing_indicator_tag[2]  = {0x00, 0x04};
/* Reserved bit:0, GT: False, PC: False, SSN: True */
static const char address_indicator_tag[2]  = {0x00, 0x01};
static const char subsystem_number_tag[2]   = {0x80, 0x03};
static const char subsystem_number  = 0x07;
static const char IPv4_address_tag[2]   = {0x80, 0x04};

/**** Sequence Control tags and primitives ****/
static const char sequence_control_tag[2]   = {0x01, 0x16};

/**** Hop Counter tag and primitives ****/
static const char hop_counter_tag[2]    = {0x01, 0x01};
static const char ss7_hop_counter_tag   = 0x0f;

/**** Importance tag and primitives ****/
static const char importance_tag[2] = {0x01, 0x13};
static const char importance_val        = 0x05;

/**** Data Tag ****/
static const char data_tag[2]       = {0x01, 0x0b};

typedef enum {
	INVALID_PTR = 0,
	ROUTE_TYPE_ABSENT,
	GT_UNRESOLVED,
	INVALID_ROUTE_TYPE,
	ROUTE_RESOLVED,		
} gtt_res;

/* SCCP route parameters */
typedef struct {
	int ssn_present;
	int gt_present;
	int pc_present;
	char gt[MAX_GT_LEN];
	char pc[MAX_PC_LEN];	
	int ssn;
	int routing_indicator;
}sccp_route;

/* SCCP output params containing TCAP and MAP params */
typedef struct {
	int sccp_msg_type;
	int sccp_msg_class;
	int hop_count;
	sccp_route called_pty;
	sccp_route calling_pty;
	int sccp_error_code;	
	tcap_params tcap_param;
} sccp_params;

/* SCCP output params containing TCAP and MAP params */
typedef struct {
	int sccp_msg_type;
	int sccp_msg_class;
	int hop_count;
	sccp_route called_pty;
	sccp_route calling_pty;
	int sccp_error_code;	
	sms_fields sms_fields;
} sccp_params_send;

/* creating sccp_unitdata_msg */
int create_sccp_unitdata (unsigned char *, sccp_params_send *, int type);

/* For extracting SCCP/TCAP/MAP params */
int sccp_analyze (unsigned char *sccp_msg, sccp_params *);

#endif
