#ifndef _TCAP_H_
#define _TCAP_H_

#include "common.h"

#define	TAG_GSM_TCAP_BEGIN				0x62	/* Tag: Begin Tcap */
#define TAG_GSM_TCAP_END				0x64	/* Tag: End Tcap */
#define TAG_GSM_TCAP_CONTINUE			0x65	/* Tag: Continue Tcap */ 
#define	TAG_GSM_TCAP_TX_ID				0x48	/* Tag: Transaction ID */
#define TAG_STRT_OF_DLG_PRTN			0x6b	/* Tag: Start Of Dialogue Portion */
#define TAG_STRT_OF_DLG_REQ				0x28	/* Tag: Start Of Dialogue Request */
#define TAG_DLG_AS_ID					0x06	/* Tag: Dialogue ID */
#define OBJECT_ID_LEN					7	/* Tag: Object Id Length */
#define TAG_TAG_FIELD0					0xa0	
#define TAG_SINGLE_ASN1_TYPE			0x60
#define TAG_SINGLE_ASN2_TYPE			0x61
#define TAG_TAG_FIELD1					0xa1
#define TAG_APPLN_CNTXT_NAME			0x06	/* Tag: Application Context Name */
#define TAG_COMP_PORTN					0x6c	/* Tag: Component Portion, signifies next payload */

#define	ASCII_ZERO						48
#define	TAG_GSM_TCAP_TX_ID				0x48	/* Tag: Transaction ID */
#define TAG_GSM_TCAP_DEST_TX_ID			0x49	/* Tag: Destination Tx ID */
#define ERROR_TCAP_LEN_NOT_PROPER 		0x00
#define ERROR_TCAP_TAG_NOT_PROPER 		0x01
#define ERROR_TCAP_PRIMITIVE_NOT_PROPER 0x02
#define MSISDN_LEN						12		/* Length of MSISDN */	
#define IMSI_LEN						16		/*Length of IMSI */
#define SMS_TEXT_LEN					160
#define TRX_ID_LEN						4			
#define SMSC_TIMESTAMP_LEN				14

#define MAP_MO_FORWARD_SM_REQ 			1
#define MAP_MT_FORWARD_SM_REQ			2
#define	MAP_SRI_SM_REQ					3
#define ALERT_SVC_CENTER				4
#define MAP_MO_FORWARD_SM_RES			5
#define MAP_MT_FORWARD_SM_RES			6
#define MAP_SRI_SM_RES					7
#define ALERT_SVC_CENTER_RES			8

#define TAG_PADDING             0x07
#define TAG_USER_INFO_ITEM      0x28
#define TAG_USER_INFO_ITEM_LEN      0x0d
#define TAG_COMPONENT_PORTN         0xa1
#define TAG_COMPONENT_PORTN_LEN     0x1d
#define TAG_CONSTRUCTOR_LEN         0x15    
#define TAG_NATURE_OF_NUMBER        0x91
#define TAG_DLG_REQUEST_LEN     0x20    // Tag: Dialogue Request 
#define TAG_APPLICATION_CONTEXT     0x06    
#define TAG_APPLICATION_CONTEXT_LEN     0x07    
#define TAG_DIRECT_REFERENCE        0x06
#define TAG_DIRECT_REFERENCE_LEN    0x07
#define TAG_USER_INFO           0xbe
#define TAG_USER_INFO_LEN       0x0f
#define TAG_TCAP_DLG_PORTN      0x6b    // Tag: Dialogue Portion  
#define TAG_TCAP_DLG_PORTN_LEN      0x2f    // Tag: Dialogue Portion  
#define SRI_PACKET_LEN          0x58
#define SRI_TRANSACTION_ID_LEN      0x04
#define SRI_PACKET_IDENTIFIER       0x2d
#define TAG_LEN_DLG_ID          0x07 
#define TAG_GSM_MOBILE_APPLICATION  0x6c;
#define TAG_GSM_MOBILE_APPLICATION_LEN  0x1f;
#define TAG_SERVICE_CENTER_ADDR     0x82    // Tag: Service Center Address
#define TAG_OID             0x06;
#define TAG_SRI_DLG_REQUEST         0xa0;   
#define TAG_SRI_DLG_REQUEST_LEN     0x22;
#define TAG_PROTOCOL_VERSION        0x80;   
#define TAG_ENCODING            0xa0;   
#define TAG_ENCODING_TYPE       0xa0;   
#define TAG_ENCODING_LEN        0x02;
#define TAG_SRI_MSISDN          0x80    
#define TAG_SRI_MSISDN_LEN      0x07    
#define TAG_SERVICE_CENTER_ADDR_LEN     0x07

typedef struct {
	int map_error_code;
	int map_opcode;
	unsigned char msisdn_to_HLR[MSISDN_LEN];
	unsigned char service_centre_address[MSISDN_LEN];
	unsigned char IMSI_Receiver[IMSI_LEN];
	unsigned char MSISDN_SMSC[MSISDN_LEN];
	char MSISDN_sender[MSISDN_LEN];
	unsigned char sms_data[SMS_TEXT_LEN];
	unsigned char MSISDN_receiver[MSISDN_LEN];
	unsigned char MSISDN_Sender[MSISDN_LEN];
	unsigned char IMSI_from_HLR[IMSI_LEN];
	unsigned char LMSI_from_HLR[MSISDN_LEN];
} map_params;
		
typedef struct  {
	int tcap_error_code;
	int tcap_primitive;
	unsigned int transaction_id;
	unsigned char object_id[OBJECT_ID_LEN];
	unsigned char appl_context[OBJECT_ID_LEN];
	map_params map_params;
} tcap_params;


/*
 * Structure: Destination/Source number
 */
typedef struct 
{
    unsigned char number[IMSI_LEN];
    int length;
} mobile_code;

/*
 * Structure: TCAP components
 *    for a GSM SMS
 * 
 * Members: 
 *  trx_id -> Transaction ID
 *  SM_RP_DA -> Dest Address    
 *  SM_RP_OA -> Sorce Address
 *  TP_Orig_Dest_Number -> 
 *    MSISDN at either endpoints
 */
typedef struct {
    unsigned int trx_id;
    mobile_code SM_RP_DA;
    mobile_code SM_RP_OA;
    mobile_code TP_Orig_Dest_Number;
    unsigned char SMSC_timestamp[SMSC_TIMESTAMP_LEN];
	char IMSI[IMSI_LEN];
	char LMSI[MSISDN_LEN];
    char sms_data[SMS_TEXT_LEN];
    char global_msg_id[IMSI_LEN];
}sms_fields;

/* */
int tcap_frame (unsigned char *, void *, int);

/* */
int parse_tcap_message (unsigned char *, void *);

#endif
