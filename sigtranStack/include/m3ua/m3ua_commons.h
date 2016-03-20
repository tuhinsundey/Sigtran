#ifndef __M3UA_H__
#define __M3UA_H__

#include "common.h"

#define	MAX_M3UA_MSG_LEN		1024
#define	MAX_HTBT_DATA_LEN		20
#define	MIN_M3UA_CLDT_LEN		101

#define M3UA_VERSION_1			0x01
#define	M3UA_RESERVED_BYTE		0x00

#define R_SHIFT_24			24
#define R_SHIFT_16			16
#define R_SHIFT_8			8

#define TAG_SCCP			3
#define TAG_NI				2
#define TAG_MP				4
#define TAG_SLS				6

/**** Message classes declaration ****/
/* Management messages */
#define M3UA_MSG_CLASS_MGMT		0x00 

/* M3UA transfer messages messages */
#define M3UA_MSG_CLASS_TX_MSG		0x01 

/* SS7 Signaling Network management */
#define M3UA_MSG_CLASS_SSNM		0x02 

/* ASP State maintenance */
#define M3UA_MSG_CLASS_ASPSM		0x03 

/* ASP traffic maintenance eg ASP */
#define M3UA_MSG_CLASS_ASPTM		0x04 

/* Routing Key management messages */
#define M3UA_MSG_CLASS_RKM		0x09


/**** Message type declaration ****/
/** Management messages **/
#define M3UA_MSG_TYPE_MGMT_ERR		0x00
#define M3UA_MSG_TYPE_MGMT_NTFY		0x01

/** M3UA transfer messages messages **/
#define M3UA_MSG_TYPE_TX_RESRVD		0x00
#define M3UA_MSG_TYPE_TX_DATA		0x01

/** SS7 Signaling Network management **/
#define M3UA_MSG_TYPE_SSNM_RESRVD	0x00
#define M3UA_MSG_TYPE_SSNM_DUNA		0x01
#define M3UA_MSG_TYPE_SSNM_DAVA		0x02 
#define M3UA_MSG_TYPE_SSNM_DAUD		0x03 
#define M3UA_MSG_TYPE_SSNM_SCON		0x04 
#define M3UA_MSG_TYPE_SSNM_DUPU		0x05 
#define M3UA_MSG_TYPE_SSNM_DRST		0x06 

/** ASP State maintenance **/
#define M3UA_MSG_TYPE_ASP_RESRVD	0x00
#define M3UA_MSG_TYPE_ASP_UP		0x01
#define M3UA_MSG_TYPE_ASP_DOWN		0x02
#define M3UA_MSG_TYPE_ASP_HTBT		0x03
#define M3UA_MSG_TYPE_ASP_UPACK		0x04
#define M3UA_MSG_TYPE_ASP_DWN_ACK	0x05
#define M3UA_MSG_TYPE_ASP_HBT_ACK	0x06

/** ASP traffic maintenance eg ASP **/
#define M3UA_MSG_TYPE_ASP_ACT		0x01
#define M3UA_MSG_TYPE_ASP_INA		0x02
#define M3UA_MSG_TYPE_ASP_ACT_ACK	0x03
#define M3UA_MSG_TYPE_ASP_INA_ACK	0x04

/** Routing Key management messages **/
#define M3UA_MSG_TYPE_RKM_RESRVD	0x00
#define M3UA_MSG_TYPE_RKM_REG_REQ	0x01
#define M3UA_MSG_TYPE_RKM_REG_RSP	0x02
#define M3UA_MSG_TYPE_RKM_DEREG_REQ	0x03
#define M3UA_MSG_TYPE_RKM_DEREG_RSP	0x04

/***Declaration of M3UA Tag values in integer ***/
#define M3UA_TAG_NW_APP			0x0200
#define M3UA_TAG_USE_CAUSE		0x0204
#define M3UA_TAG_CON_IND		0x0205
#define M3UA_TAG_DSTN_CCRND		0x0206
#define M3UA_TAG_RTNG_KEY		0x0207
#define M3UA_TAG_REG_RES		0x0208
#define M3UA_TAG_DEREG_RES		0x0209
#define M3UA_TAG_LOCAL_RTNG_KEY		0x020a
#define M3UA_TAG_DSTN_PC		0x020b
#define M3UA_TAG_SI			0x020c
#define M3UA_TAG_ORIG_PC_LIST		0x020e
#define M3UA_TAG_PROTO_DATA		0x0210
#define M3UA_TAG_REG_STATUS		0x0212
#define M3UA_TAG_DEREG_STATUS		0x0213


#define ASPSM_MSG_LEN			8
#define ASPTM_MSG_LEN			16

#define	reserved_bytes3			3
#define	reserved_bytes4			4

#define NORMAL_TAG_LEN			4

#define CORR_ID_LEN				8	
#define NW_APRNCE_LEN			CORR_ID_LEN
#define ROUTING_CTXT_LEN		CORR_ID_LEN 

#define ROUTING_CTXT_TAG_LEN		2
#define TRAFFIC_MODE_TYPE_TAG_LEN	2
#define PROTOCOL_DATA_TAG_LEN		2
#define HEART_BEAT_TAG_LEN		2
#define MAX_HEART_BEAT_SEQ		255

#define OVERRIDE_TRAFFIC_MODE		1
#define LOADSHARE_TRAFFIC_MODE		1
#define BROADCAST_TRAFFIC_MODE		1

#define LEN_ZERO			0
#define LEN_ONE				1
#define LEN_THREE			3
#define LEN_TWO				2
#define LEN_EIGHT			8
#define LEN_SIXTEEN			16

	
#define INVALID_VERSION			0x01
#define UNSPORTED_MSG_CLASS		0x03
#define	UNSPORTED_MSG_TYPE		0x04
#define	UNSPORTED_TRFIC_MODE_TYPE	0x05
#define	UNEXPCTD_MSG			0x06
#define	PROTOCOL_ERR			0x07
#define	INVALID_STREAM_IDENTIFIER	0x09
#define	REFUSD_MGMT_BLOCKING		0x0d
#define	ASP_IDENTFIR_REQ		0x0e
#define	INVLD_ASP_IDENTIFIRE		0x0f
#define	INVLD_PARAMS_VALUE		0x11
#define	PARAM_FIELD_ERR			0x12
#define	UNEXPCD_PARAMS			0x13
#define	DESTINTON_STUS_UNKOWN		0x14
#define	INVLD_NETWRK APEARANCE		0x15
#define	MISSING_PARAMS			0x16
#define	INVLD_ROUTING_CONTXT		0x19
#define	NO_CONFIGURD_AS_FOR_ASP		0x1a



#endif /* eof m3ua.h */ 
