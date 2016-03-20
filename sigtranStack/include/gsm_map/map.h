#ifndef _MAP_H_
#define _MAP_H_
#include "common.h"
#define TAG_COMP_PORTN			0x6c    // Tag: Component Portion, signifies next payload
#define TAG_DLG_RESULT			0xa2    // Tag: Response Result tag 
#define TAG_COMPONENT			0x02    // Tag: Component
#define TAG_SHORT_LEN			0x01    // Tag: Short Length
#define TAG_INVOKE_ID			0x02    // Tag: Invoke ID
#define DEFAULT_INVOKE_ID		0x01    // Tag: Invoke ID
#define TAG_CONSTRUCTOR			0x30    // Tag: Constructor  
#define TAG_SEND_ROUT_INFO		0x2d    // Tag: Send routing Info opcode
#define TAG_GSM_SMS_PDU			0x04    // Tag: GSM SMS TPDU
#define MAX_OCT_MSISDN_LEN		9
#define TAG_ASN_1_TYPE			0xa0    // Tag: Single ASN 1 type
#define LARGE_MSG_LEN_IDF		0x81
#define TAG_INTL_NUM			0x91
#define TAG_COMPONENT_PORTN		0xa1
#define TAG_COMPONENT_PORTN_LEN	0x1d
#define TAG_RETURN_RESULT_LAST	0x02
#define TAG_OPCODE				0x02
#define SRI_PACKET_IDENTIFIER		0x2d
#define TAG_CONSTRUCTOR_LEN		0x15 
#define TAG_SRI_MSISDN			0x80    
#define TAG_SRI_MSISDN_LEN		0x07    
#define TAG_NATURE_OF_NUMBER		0x91
#define TAG_PARAMETER_2			0x81	
#define TAG_SERVICE_CENTER_ADDR		0x82    // Tag: Service Center Address
#define TAG_SERVICE_CENTER_ADDR_LEN	0x07
#define TAG_GSM_MAP_HDR			0xa1
#define DEFAULT_INVOKE_ID		0x01  
#define MO_FORWARD_SM			0x2e
#define TAG_SVC_CENTR_ADDR		0x84
#define TAG_MSISDN			0x82
#define TAG_GSM_SMS_PDU			0x04
#define TAG_MAP_RETURN_RES		0xa2
#define MT_FORWARD_SM			0x2c 
#define TAG_IMSI			0x80
#define TAG_MSISDN			0x82
#define MAX_TIMESTAMP_LEN		7	
#define GSM_7_CODING			0x00
#define ERROR_MAP_TAG_NOT_PROPER 0x00
#define TAG_MAP_DELIVER_SM		0x24
/*
 * To add map layer part
 */

int gsm_map_frame (unsigned char *map_buffer, 
	void *params, int type);
/*
 *
 */

#define ascii_to_gsm7(input, output)  \
	unsigned int inputLength;\
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
		if (i%7 == 0)\
			++j;\
	}\
	output [i] = '\0';


/*
 *
 */
#define gsm7_to_ascii(input, output) unsigned int inputLength;\
	unsigned int i = 0,j = 0;\
	inputLength = strlen (input) + (strlen (input)/7);\
	while (j < inputLength)\
	{\
		if (j%7 == 0) {\
			output[i] = input[j] & 0x7f;\
			++i;\
			++j;\
		}\
		if (j%7 == 1) {\
			output [i] = (((input [j] & 0x3f) << 1) | \
				((input [j - 1] & 0x80) >> 7));\
			++i;\
			++j;\
		}\
		if (j%7 == 2) {\
			output [i] = (((input [j] & 0x1f) << 2) | \
				((input [j - 1] & 0xc0) >> 6));\
			++i;\
			++j;\
		}\
		if (j%7 == 3) {\
			output [i] = (((input [j] & 0x0f) << 3) | \
				((input [j - 1] & 0xe0) >> 5));\
			++i;\
			++j;\
		}\
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

/* Parsing the MAP message */
int parse_map_message (unsigned char *map_message, void * params);

#endif
