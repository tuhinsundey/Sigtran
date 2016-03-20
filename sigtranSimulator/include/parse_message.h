#ifndef __TCAP_H__
#define __TCAP_H__

#include <stdlib.h>
#include <stdio.h>
#include <mysql/mysql.h>
#define	TAG_GSM_TCAP_BEGIN		0x62	// Tag: Begin Tcap
#define TAG_GSM_TCAP_END    	0x64    // Tag: End Tcap
#define TAG_GSM_TCAP_CONTINUE   0x65    // Tag: Continue Tcap
#define	TAG_GSM_TCAP_TX_ID		0x48	// Tag: Transaction ID

#define TAG_DLG_RESP			0x28	// Tag: Dialogue Response 
#define TAG_TCAP_OID			0x06	// Tag: OID
#define TAG_ASN_1_TYPE			0xa0	// Tag: Single ASN 1 type
#define TAG_DLG_REQUEST			0x60	// Tag: Dialogue Request 
#define TAG_APPL_CTXT_NAME		0xa1	// Tag: Application Context Name tag 
#define TAG_COMP_PORTN			0x6c	// Tag: Component Portion, signifies next payload
#define TAG_GSM_MAP_HDR			0xa1 	// Tag: GSM MAP header
#define TAG_COMPONENT			0x02 	// Tag: Component
#define TAG_INVOKE				0x01	// Tag: Invoke  
#define TAG_CONSTRUCTOR			0x30	// Tag: Constructor	
#define TAG_IMSI				0x80	// Tag: IMSI --> TBCD
#define TAG_SVC_CENTR_ADDR		0x84 	// Tag: Service Centre Address 
#define TAG_GSM_SMS_PDU			0x04 	// Tag: GSM SMS TPDU
#define TAG_ORIG_ADDR			0x0a 	// Tag: Originating Address, 
#define TAG_GSM_TCAP_TX_ID_DEST		0x49    // Tag: Destination Transaction ID
#define TAG_DLG_RESPONSE     	        0x61    // Tag: Dialogue Response for TCAP END 
#define TAG_DLG_RESULT      		0xa2    // Tag: Response Result tag 
#define TAG_DLG_RESULT_SRC_DGN  	0xa3    // Tag: Response Result Siurce diagnostic Tag 
#define TAG_DLG_USR_DATA        	0xbe    // Tag: User data tag in dialogue response 
#define TAG_MAP_RETURN_RES		0xa2    // Tag: Return result last
#define R_SHIFT_24              24
#define R_SHIFT_16              16 
#define R_SHIFT_8               8
#define TAG_SEND_ROUT_INFO		0x2d    // Tag: Send routing Info opcode
#define MAX_TCAP_MSG_LEN		250
#define MAX_IMSI_LEN			15
#define MAX_OCT_MSISDN_LEN		9
#define MAX_TIME_STMP_LEN		9

#define LARGE_MSG_LEN_IDF		0x81

#define MT_FORWARD_SM			0x2c 	// val: mt-forwardSM(44)
#define MO_FORWARD_SM			0x2e 	// val: mo-forwardSM(46)

#define MO_FORWARD_ACK          0x00    // input to function for: mo-forward-ack
#define MT_FORWARD_ACK          0x01    // input to function for: mt_forward-ack

#define	ASCII_ZERO 				48
#define	MAX_SMS_TXT_LEN			172
#define	MAX_7BIT_SMS			160
#define MO_OPCODE_VALUE 2

#define ascii_to_gsm7(input, output)  unsigned int inputLength;\
    unsigned int i = 0,j = 0;\
    inputLength = strlen (input);\
    while (j < strlen (input))\
    {\
        if (j%8 == 0) {\
            output [i] = input[j] | \
							((input [j + 1] & 0x01) << 7);\
            ++i;\
            ++j;\
            if (j == strlen (input))\
            break;\
        }\
        if (j%8 == 1) {\
            output[i] = ((input [j] >> 1) | \
							((input [j + 1] & 0x03) << 6));\
            ++i;\
            ++j;\
            if (j == strlen (input))\
            break;\
        }\
        if (j%8 == 2) {\
            output [i] = ((input [j] >> 2) | \
							((input [j + 1] & 0x07) <<5));\
            ++i;\
            ++j;\
            if (j == strlen (input))\
            break;\
        }\
        if (j%8 == 3) {\
            output [i] = ((input [j] >> 3) | \
							((input [j + 1] & 0x0f) << 4));\
            ++i;\
            ++j;\
            if (j == strlen (input))\
            break;\
        }\
        if (j%8 == 4) {\
            output [i]= ((input [j] >> 4) | \
							((input [j + 1] & 0x1f) <<3));\
            ++i;\
            ++j;\
            if (j == strlen (input))\
            break;\
        }\
        if (j % 8 == 5) {\
            output [i]= ((input [j] >> 5) | \
							((input [j + 1] & 0x3f) << 2));\
            ++i;\
            ++j;\
            if (j == strlen (input))\
            break;\
        }\
        if (j%8 == 6) {\
            output [i]= ((input [j] >> 6) | \
							((input [j + 1] & 0x7f) << 1));\
            ++i;\
            ++j;\
            if (j == strlen (input))\
            break;\
        }\
		\
		if (i%7 == 0)\
			++j;\
    }\
    output [i] = '\0';


#define gsm7_to_ascii(input, output) unsigned int inputLength;\
	unsigned int i = 0,j = 0;\
	inputLength = strlen (input) + (strlen (input)/7);\
	while (j < inputLength)\
	{   \
		if (j%7 == 0) {\
            output[i] = input[j] & 0x7f;\
			++i;\
			++j;\
		} \
		if (j%7 == 1) {\
			output [i] = (((input [j] & 0x3f) << 1) | \
							((input [j - 1] & 0x80) >> 7));\
			++i;\
			++j;\
		} \
		if (j%7 == 2) {\
			output [i] = (((input [j] & 0x1f) << 2) | \
							((input [j - 1] & 0xc0) >> 6));\
			++i;\
			++j;\
		} \
		if (j%7 == 3) {\
			output [i] = (((input [j] & 0x0f) << 3) | \
							((input [j - 1] & 0xe0) >> 5));\
			++i;\
			++j;\
		} \
		if (j%7 == 4) {\
			output [i] = (((input [j] & 0x07) << 4) | \
							((input [j - 1] & 0xf0) >> 4));\
			++i;\
			++j;\
		} \
		if (j%7 == 5) {\
			output [i] = (((input [j] & 0x03) << 5) | \
							((input [j - 1] & 0xf8) >> 3));\
			++i;\
			++j;\
		} \
		if (j%7 == 6) {\
			output [i] = (((input [j] & 0x01) << 6) | \
							((input [j - 1] & 0xfc) >> 2));\
            output [i + 1]	= ((input [j] & 0xfe) >> 1);\
			i = i + 2;\
			j++;\
		}\
	}\
	output [j] = '\0';




/*
 * Convert normal text to gsm 7-bit format.
 *
 * Input: char pointer to the original text
 * Output: char pointer to 7- bit 
 * 		   encoded string.
 * Data encoding scheme (0) 
 */
//void ascii_to_gsm7 (char *input, char *output);


/*
 * Convert gsm 7-bit format to normal text.
 *
 * Input: char pointer to 7- bit 
 * 		   encoded string.
 * Output: char pointer to the original text
 */
//void gsm7_to_ascii (char *input, char *output);


/*
 * Structure: Destination/Source number
 */
typedef struct _mobile_code_t
{
	unsigned char number[MAX_IMSI_LEN];
    int length;
} mobile_code;


/*
 * Structure: TCAP components
 *    for a GSM SMS
 * 
 * Members: 
 * 	trx_id -> Transaction ID
 *  SM_RP_DA -> Dest Address	
 *  SM_RP_OA -> Sorce Address
 *  TP_Orig_Dest_Number -> 
 *	  MSISDN at either endpoints
 */
typedef struct _sms_fields_t
{
	unsigned int trx_id;
	mobile_code SM_RP_DA;
	mobile_code SM_RP_OA;
	mobile_code TP_Orig_Dest_Number;
	unsigned char SMSC_timestamp[14];
	char sms_data[160];
	char global_msg_id[16];	
} sms_fields;


/*
 * Convert numbers values in accordance to 
 * GSM SMS specs in MAP.
 * Used for endpoint numbers,
 * Timestamp. 
 */ 
void encode_number_or_time (unsigned char *input, unsigned char * output, 
							int input_length);

/*
 * Decode the Number and timestamp
 */
void decode_number_or_time (unsigned char * input, unsigned char * output, 
								int input_length);


/*
 * API to submit GSM SMS
 * in MT path.
 *
 * Input(s): sms field structure.
 *		   : message length.
 * Output  : character buffer of 
 * 			whole of TCAP message
 *			with MAP specs as in 3gpp.
 */
unsigned char *forward_sm_MT_packet (sms_fields * input_data, 
										int *message_length);


/*
 * API to submit GSM SMS
 * in MO path.
 *
 * Input(s): sms field structure.
 *		   : message length.
 * Output  : character buffer of 
 * 			whole of TCAP message
 *			with MAP specs as in 3gpp.
 */
unsigned char *forward_sm_MO_packet (sms_fields * input_data, 
										int *message_length);

/*
 * API to send back an ack 
 * to an SMS
 *
 * Inputs : Message length.
 * 		  : type -> MO or MT.
 *		  : transaction ID
 *
 * Output : Whole of encoded
 *			TCAP message buffer
 */
 
unsigned char *forward_sm_end_ack_packet (int *message_length, char type, 
							unsigned int trx_id);

unsigned char *forward_sm_ack_packet_continue (int *message_length, char type, 
							unsigned int src_trx_id, unsigned int dest_trx_id);
/*
 * API to parse message, endpoint numbers
 * from a GSM SMS.
 */
char *parse_tcap_sms (char *my_databuffer, int *trx_id);
void init_msqlHandle();
int  gsmsc_tcap_query (char *query);
unsigned char * form_sri_ack (unsigned int trx_id, unsigned char * IMSI, 
	unsigned char * LMSI, int *ack_length);

int parse_message(char *m3ua_message, unsigned int *trx_id);
void parse_sri_ack_packet( char *m3ua_message, char *imsi);

void read_database_configuration ();
char *get_receiver();

#endif /* tcap.h */
