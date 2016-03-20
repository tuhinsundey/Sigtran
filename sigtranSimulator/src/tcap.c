 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/
/*
 *   Purpose: This code-file defines TCAP packet formation and  
 *            has definitions for:
 *           - Ascii to gsm7 conversion
 *           - gsm7 to ascii conversion
 *           - Forming forward_sm MT packet 
 *           - Forming forward_sm MO packet 
 *           - Encoding
 *           - Decoding 
 *	         - Parsing TCAP packet. 
 */


#include <stdio.h>
#include <string.h>
#include "tcap.h"
#include "parse_message.h"

#define TAG_PADDING 			0x07
#define TAG_USER_INFO_ITEM 		0x28
#define TAG_USER_INFO_ITEM_LEN 		0x0d
#define TAG_INVOKE_ID 			0x01
#define TAG_COMPONENT_PORTN 		0xa1
#define TAG_COMPONENT_PORTN_LEN 	0x1d
#define TAG_CONSTRUCTOR_LEN 		0x15	
#define TAG_NATURE_OF_NUMBER 		0x91
#define TAG_DLG_REQUEST_LEN		0x20	// Tag: Dialogue Request 
#define TAG_APPLICATION_CONTEXT 	0x06	
#define TAG_APPLICATION_CONTEXT_LEN 	0x07	
#define TAG_DIRECT_REFERENCE 		0x06
#define TAG_DIRECT_REFERENCE_LEN 	0x07
#define TAG_USER_INFO 			0xbe
#define TAG_USER_INFO_LEN 		0x0f
#define TAG_TCAP_DLG_PORTN		0x6b 	// Tag: Dialogue Portion  
#define TAG_TCAP_DLG_PORTN_LEN		0x2f 	// Tag: Dialogue Portion  
#define SRI_PACKET_LEN 			0x58
#define SRI_TRANSACTION_ID_LEN 		0x04
#define SRI_PACKET_IDENTIFIER 		0x2d
#define TAG_LEN_DLG_ID 			0x07 
#define TAG_GSM_MOBILE_APPLICATION 	0x6c;
#define	TAG_GSM_MOBILE_APPLICATION_LEN 	0x1f;
#define TAG_SERVICE_CENTER_ADDR		0x82    // Tag: Service Center Address
#define	TAG_OID 			0x06;
#define TAG_SRI_DLG_REQUEST 		0xa0;	
#define TAG_SRI_DLG_REQUEST_LEN 	0x22;
#define TAG_PROTOCOL_VERSION 		0x80;	
#define TAG_ENCODING 			0xa0;	
#define TAG_ENCODING_TYPE 		0xa0;	
#define TAG_ENCODING_LEN 		0x02;
#define TAG_SRI_MSISDN 			0x80	
#define TAG_SRI_MSISDN_LEN 		0x07	
#define TAG_SERVICE_CENTER_ADDR_LEN 	0x07

		
/*************************************************************************************
  * Function : encode_number_or_time()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
void encode_number_or_time(unsigned char *input, unsigned char * output, int input_length)
{
	int counter_out = 0, counter_in = 0;
	char input_len = 0;
	memcpy ((void *)&input_len, (void *)&input_length, sizeof (char));

	while (counter_in < input_length -1)
	{
		output [counter_out] = (((input [counter_in] - ASCII_ZERO)) | 
								((input [counter_in + 1] - ASCII_ZERO) << 4));
		++counter_out ;
		counter_in = counter_in + 2;
	}

	if (input_length % 2 == 1) {
		output [counter_out] = (((input [counter_in] - ASCII_ZERO)) |
							   	(input_len << 4));
	}   

	output [counter_out + 1] = '\0';
}   
 

/*************************************************************************************
  * Function : decode_number_or_time()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
void decode_number_or_time (unsigned char *input, unsigned char *output, 
		int input_length)
{
	int counter_out = 0,counter_in = 0;
	while (counter_in < input_length)
	{
		counter_out = counter_in * 2;
		output [counter_out] = ((input [counter_in] & 0x0f) + 48);
		output [counter_out + 1] = 
			(((input [counter_in] & 0xf0) >> 4) + 48);
		++counter_in;   
	}   
	output [counter_out + 2] = '\0';
}  


/*************************************************************************************
  * Function : sri_SM_req()
  * Input    : input_data, mesasge_length
  * Output   : None
  * Purpose  : To Create SRI packet
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
unsigned char *sri_SM_req (sms_fields *input_data, int *message_length)
{
	int iterator 	= 0;
//	unsigned char sri[300]	= {0};
	unsigned char *sri = NULL;
	sri = (unsigned char*) malloc (MAX_TCAP_MSG_LEN);
	unsigned int temp_trx_id = 0;
	int string_iter = 0;
	unsigned char temp_char_DA[30] = {0};
	unsigned char temp_service_center_addr[30] = {0};
	
	sri[iterator++] = TAG_GSM_TCAP_BEGIN;
	sri[iterator++] = SRI_PACKET_LEN;
	sri[iterator++] = TAG_GSM_TCAP_TX_ID;
	sri[iterator++] = SRI_TRANSACTION_ID_LEN;
	
	temp_trx_id = (input_data->trx_id >>R_SHIFT_24);
        memcpy ((void *)&sri[iterator++], &temp_trx_id, 1);
        temp_trx_id = (input_data->trx_id >>R_SHIFT_16);
        memcpy ((void *)&sri [iterator++], &temp_trx_id, 1);
        temp_trx_id = (input_data->trx_id >> R_SHIFT_8);
        memcpy ((void *)&sri [iterator++], &temp_trx_id, 1);
        memcpy ((void *)&sri [iterator++], &input_data->trx_id, 1);	

	
	sri[iterator++] = TAG_TCAP_DLG_PORTN; 
	sri[iterator++] = TAG_TCAP_DLG_PORTN_LEN; /*length till end of dialogue*/
	sri[iterator++] = 0x28;
	sri[iterator++] = 0x2d; /*length till end of dialogue*/

	/*DIALOGUE PORTION*/
	sri[iterator++] = TAG_OID;
	sri[iterator++] = TAG_LEN_DLG_ID;
	iterator++;

	/*OID - ID as Dialogue */
	sri[iterator++] = 0x11;
	sri[iterator++] = 0x86;
	sri[iterator++] = 0x05;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0xa0;/* Tag of dialogue request  */
	sri[iterator++] = 0x22;/*length till end of dialogue*/
	
	sri[iterator++] = TAG_DLG_REQUEST;/* Tag of dialogue request  */
	sri[iterator++] = TAG_DLG_REQUEST_LEN;/*length till end of dialogue*/
	
	/*Dialogue Request*/
	sri[iterator++] = 0x80;
	sri[iterator++] = 0x02;
	sri[iterator++] = TAG_PADDING;/*Padding*/
	sri[iterator++] = TAG_PROTOCOL_VERSION;/*Protocol Version*/
	sri[iterator++] = 0xa1;
	sri[iterator++] = 0x09;/*length in bytes*/
	
	sri[iterator++] = TAG_APPLICATION_CONTEXT;/*TAG_APPLICATION CONTEXT NAME*/
	sri[iterator++] = TAG_APPLICATION_CONTEXT_LEN;/*Length in bytes*/
	
	/*Short Message Gateway context Name*/
	sri[iterator++] = 0x04;
	sri[iterator++] = 0x62;
	iterator ++;
	sri[iterator++] = 0x01;
	iterator ++;
	sri[iterator++] = 0x14;
	sri[iterator++] = 0x02;

	sri[iterator++] = TAG_USER_INFO;/* User Information Tag  */
	sri[iterator++] = TAG_USER_INFO_LEN;/* Length of User Information Tag  */
	
	/* User Information */
	sri[iterator++] = TAG_USER_INFO_ITEM;
	sri[iterator++] = TAG_USER_INFO_ITEM_LEN;

	sri[iterator++] = TAG_DIRECT_REFERENCE;
	sri[iterator++] = TAG_DIRECT_REFERENCE_LEN;
	
	sri[iterator++] = 0x04;
	iterator +=2;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;

	/* Encoding */	
	sri[iterator++] = TAG_ENCODING;/* Encoding Tag */
	sri[iterator++] = TAG_ENCODING_LEN;/* Length of encoding in bytes*/
	
	sri[iterator++] = TAG_ENCODING_TYPE; /* Encoding */
	iterator++;		/* Encoding */ 

	sri[iterator++] = TAG_GSM_MOBILE_APPLICATION;
	sri[iterator++] = TAG_GSM_MOBILE_APPLICATION_LEN;
	
	/* GSM MOBILE APPLICATION */
	sri[iterator++] = TAG_COMPONENT_PORTN;
	sri[iterator++] = TAG_COMPONENT_PORTN_LEN;
	
	sri[iterator++] = 0X02;/*Tag return result last*/
	sri[iterator++] = 0X01;/*Length*/

	sri[iterator++] = TAG_INVOKE_ID;/*value Invoke ID*/

	/* Opcode */
	sri[iterator++] = 0X02;
	sri[iterator++] = 0X01;
	sri[iterator++] = SRI_PACKET_IDENTIFIER;
	
	/*Constructor*/
	sri[iterator++] = TAG_CONSTRUCTOR;/* Constructor Tag */
	sri[iterator++] = TAG_CONSTRUCTOR_LEN;/* Length of remaining packet */

	/* msisdn */
	sri[iterator++] = TAG_SRI_MSISDN;/* Parameter - 1 Tag */
	sri[iterator++] = TAG_SRI_MSISDN_LEN;/* Length in bytes */
	sri[iterator++] = TAG_NATURE_OF_NUMBER;/* Nature of Number */
	
	encode_number_or_time (input_data->SM_RP_DA.number,
                                   temp_char_DA, input_data->SM_RP_DA.length);
	for (string_iter = 0; string_iter < (input_data->SM_RP_DA.length +1)/2; 
		++string_iter)
    	{
		sri[iterator++] = temp_char_DA[string_iter];
	}
	
	/* SM_RP_PRI */	
	sri[iterator++] = 0X81;/* Parameter - 2 tag */
	sri[iterator++] = 0X01;/* Length in bytes */
	iterator++;

	/* Service Center Address */	
	sri[iterator++] = TAG_SERVICE_CENTER_ADDR;/* Parameter - 3 tag */
	sri[iterator++] = TAG_SERVICE_CENTER_ADDR_LEN;/* Length in bytes */
	sri[iterator++] = TAG_NATURE_OF_NUMBER;

	encode_number_or_time (input_data->SM_RP_OA.number,
                                   temp_service_center_addr, input_data->SM_RP_OA.length);
	for (string_iter = 0; string_iter < (input_data->SM_RP_OA.length +1)/2;
                        ++string_iter)
	{
                sri[iterator++] = temp_service_center_addr[string_iter];
	}
	*message_length = iterator;
	return(sri);	
}


/*************************************************************************************
  * Function : forward_sm_MT_packet()
  * Input    : Input_data, message_length, imsi
  * Output   : None
  * Purpose  : To create_forward_sm_MT_packet
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
unsigned char *forward_sm_MT_packet (sms_fields *input_data, 
	int *message_length, char *imsi)
{
	unsigned char text_buf [MAX_7BIT_SMS] = {0};
	unsigned char *tcap_buff;
	unsigned int temp_trx_id;

	int string_iter;
	int iterator = 0;
	int iterator_last = 0;

	unsigned char temp_char_DA [MAX_OCT_MSISDN_LEN] = {0};
	unsigned char temp_char_OA [MAX_OCT_MSISDN_LEN] = {0};
	unsigned char temp_char_TP [MAX_OCT_MSISDN_LEN] = {0};
	unsigned char timestamp_buf [MAX_TIME_STMP_LEN] = {0};

	tcap_buff = (unsigned char *) malloc (MAX_TCAP_MSG_LEN);
	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN); 
	memset (text_buf, 0, MAX_7BIT_SMS);
	char *input = input_data->sms_data;
	char *output = (char *)text_buf;
	ascii_to_gsm7(input, output);	
	//ascii_to_gsm7 (input_data->sms_data, (char *) text_buf);
	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN);

	/* Tag: TCAP Begin */
	tcap_buff [iterator + 0] = TAG_GSM_TCAP_BEGIN; 
	/* Tag: Transaction ID */
	tcap_buff [iterator + 3] = TAG_GSM_TCAP_TX_ID;  
	/* Tag: Length of Transacation ID in bytes */
	tcap_buff [iterator + 4] = 0x04; 

	/*
	 * Put the Transaction ID here
	 */
	temp_trx_id = (input_data->trx_id >>R_SHIFT_24);
	memcpy ((void *)&tcap_buff[iterator + 5],&temp_trx_id, 1);
	temp_trx_id = (input_data->trx_id >>R_SHIFT_16);
	memcpy ((void *)&tcap_buff [iterator + 6],&temp_trx_id, 1);
	temp_trx_id = (input_data->trx_id >> R_SHIFT_8);
	memcpy ((void *)&tcap_buff [iterator + 7], &temp_trx_id, 1);
	memcpy ((void *)&tcap_buff [iterator + 8], &input_data->trx_id, 1);

	tcap_buff [iterator + 9] = 0x6b;   // Tag: Dialogue Portion
	tcap_buff [iterator + 10] = 0x1a;   // val: length of OID + 4

	tcap_buff [iterator + 11] = TAG_DLG_RESP;  // Tag: Dialogue Response 
	tcap_buff [iterator + 12] = 0x18;  // val: Length 0f OID + 2

	tcap_buff [iterator + 13] = TAG_TCAP_OID; // Tag: OID 
	tcap_buff [iterator + 14] = 0x07; // val: length of message (OID)
	tcap_buff [iterator + 16] = 0x11; // val: 5th byte of OID
	tcap_buff [iterator + 17] = 0x86; // val: 4th byte of OID
	tcap_buff [iterator + 18] = 0x05; // val: 3rd byte of OID
 	tcap_buff [iterator + 19] = 0x01; // val: 2nd byte of OID
	tcap_buff [iterator + 20] = 0x01; // val: 1st byte of OID
	tcap_buff [iterator + 21] = 0x01; // val: 0th byte of OID

	tcap_buff [iterator + 22] = TAG_ASN_1_TYPE; // Tag: Single ASN 1 type
	tcap_buff [iterator + 23] = 0x0d; // val: length till application context

	tcap_buff [iterator + 24] = TAG_DLG_REQUEST; // Tag: Dialogue Request 
	tcap_buff [iterator + 25] = 0x0b; // val: length till application context

	tcap_buff [iterator + 26] = TAG_APPL_CTXT_NAME; // Tag: Application Context Name tag 
	tcap_buff [iterator + 27] = 0x09; // Val: length of Context name  

	tcap_buff [iterator + 28] = TAG_TCAP_OID; // Tag: Object Identifier tag
	tcap_buff [iterator + 29] = 0x07; // val: length of Object Identifier
	tcap_buff [iterator + 30] = 0x04; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
                            // shortMsgMT-RelayContext-v2
	tcap_buff [iterator + 33] = 0x01; // val: 3rd byte of context  
	tcap_buff [iterator + 35] = 0x19; // val: 1st byte of context
	tcap_buff [iterator + 36] = 0x02; // val: 0th byte of context

	tcap_buff [iterator + 37] = TAG_COMP_PORTN; // Tag: Component Portion, signifies next payload
	tcap_buff [iterator + 40] = TAG_GSM_MAP_HDR; // Tag: GSM MAP header
	tcap_buff [iterator + 43] = TAG_COMPONENT; // Tag: Component
	tcap_buff [iterator + 44] = TAG_INVOKE;  // Tag: Invoke  
	tcap_buff [iterator + 45] = 0x01;  // val: Tag ID

	tcap_buff [iterator + 46] = TAG_COMPONENT; // Tag: opcode:locavalue
	tcap_buff [iterator + 47] = 0x01; // val: length in bytes
	tcap_buff [iterator + 48] = MT_FORWARD_SM;

	tcap_buff [iterator + 49] = TAG_CONSTRUCTOR; // Tag: Constructor   
	tcap_buff [iterator + 52] = TAG_IMSI; // Tag: IMSI --> TBCD
	
	memcpy((char *)input_data->SM_RP_DA.number, imsi, strlen(imsi));	
	input_data->SM_RP_DA.length = strlen(imsi);	
	
	temp_trx_id = ((input_data->SM_RP_DA.length + 1)/2);
	memcpy (&tcap_buff [iterator + 53], (void *)&temp_trx_id,1);//val: length in bytes IMSI 15 digits

	encode_number_or_time (input_data->SM_RP_DA.number, 
					temp_char_DA, input_data->SM_RP_DA.length);
	for (string_iter = 0; string_iter < (input_data->SM_RP_DA.length +1)/2 ; 
			++string_iter)
	{
		tcap_buff[ 54 + string_iter] = temp_char_DA[string_iter];
	}
	iterator = 54 + string_iter;
	
	tcap_buff [iterator] = TAG_MSISDN;//TAG_SVC_CENTR_ADDR; // Tag: Service Centre Address 
	temp_trx_id = ((input_data->SM_RP_OA.length +1)/2 + 1);
	memcpy (&tcap_buff [iterator + 1], (void *)&temp_trx_id,1);
	tcap_buff [iterator + 2] = 0x91; // val: Service centre - 0th byte 
	iterator = iterator + 3;
	
	encode_number_or_time (input_data->SM_RP_OA.number,
				temp_char_OA, input_data->SM_RP_OA.length);
	for (string_iter = 0; string_iter < (input_data->SM_RP_OA.length +1)/2 ; 
			++string_iter)
	{
		tcap_buff [iterator + string_iter] = temp_char_OA [string_iter];
	}
	iterator = iterator + string_iter; 

	tcap_buff [iterator] = TAG_GSM_SMS_PDU; // Tag: GSM SMS TPDU
	iterator_last = iterator + 1;
	tcap_buff [iterator + 3] = 0x24; // val bit-by-bit: TP-RP(7), 
					 	   // TP-UDHI(6), TP-SRI(5), 
						   // TP-MMS(2), TP-MTI(0-1)
	temp_trx_id = input_data->TP_Orig_Dest_Number.length;
	memcpy(&tcap_buff [iterator + 4], (void *)&temp_trx_id,1);// val: 
	tcap_buff [iterator + 5] = 0x91; // val bit-by-bit: no-extension(7th), 
                           // type of number(4 to 6), 
                           // Numbering Plan(0 to 3) 
	iterator = iterator + 6;
	encode_number_or_time (input_data->TP_Orig_Dest_Number.number,
				temp_char_TP, input_data->TP_Orig_Dest_Number.length);
	for (string_iter = 0; 
		string_iter < ((input_data->TP_Orig_Dest_Number.length + 1)/2); 
		++string_iter)
	{
		tcap_buff [iterator + string_iter ] = temp_char_TP [string_iter];
	}
	
	iterator = iterator + string_iter;
	tcap_buff [iterator]   = 0x00; //TP- PID
	tcap_buff [iterator + 1] = 0x00; //TP - DCS	
	iterator += 2;
	encode_number_or_time (input_data->SMSC_timestamp, timestamp_buf, 14);
    for (string_iter = 0; string_iter < 7; ++string_iter)
    {
		tcap_buff [iterator + string_iter] = timestamp_buf [string_iter];
	}

	iterator = iterator + string_iter;	
	tcap_buff [iterator ] = strlen ((char *)text_buf);
	unsigned int tcap_buff_iter = iterator + 1;
	for (string_iter = 0; string_iter < strlen ((char *)text_buf); 
			++string_iter)
	{
		if (text_buf [string_iter] == '\0') {
			printf ("=== Found '/ 0' at position %d\n", string_iter);
			break;
		}

		if (text_buf [string_iter] < 0) {
			tcap_buff [tcap_buff_iter] = text_buf [string_iter] + 256;
		}
		else
			tcap_buff [tcap_buff_iter] = text_buf [string_iter];
		++tcap_buff_iter;
	}
	tcap_buff [iterator_last] 		= LARGE_MSG_LEN_IDF; // val: gsm sms message length 
	tcap_buff [iterator_last + 1] 	= 0x0d + string_iter + // val: gsm sms message length
										(input_data->TP_Orig_Dest_Number.length + 1)/2;  
	tcap_buff [50] 	= LARGE_MSG_LEN_IDF;

	/* length of whole of remaining message */	
	tcap_buff [51] 	= 0x15 + string_iter + 
						(input_data->SM_RP_DA.length +1)/2 + 
						(input_data->SM_RP_OA.length +1)/2 + 
						(input_data->TP_Orig_Dest_Number.length +1)/2; 
	tcap_buff [38] 	= LARGE_MSG_LEN_IDF;
	/* length of remaining message - 2 */
	tcap_buff [39] 	= 0x21 + string_iter + 
							(input_data->SM_RP_DA.length +1)/2 +
							(input_data->SM_RP_OA.length +1)/2 +
							(input_data->TP_Orig_Dest_Number.length +1)/2;
	tcap_buff [41] 	= LARGE_MSG_LEN_IDF; 
	/* length of remaining message - 2 */
	tcap_buff [42] 	= 0x1e + string_iter + 
						(input_data->SM_RP_DA.length +1)/2 + 
						(input_data->SM_RP_OA.length +1)/2 + 
						(input_data->TP_Orig_Dest_Number.length +1)/2; 
	tcap_buff [1]	= LARGE_MSG_LEN_IDF;
	tcap_buff [2] 	= tcap_buff_iter - 3;
	tcap_buff [tcap_buff_iter] = '\0';
	
	*message_length = tcap_buff_iter;	 
	return tcap_buff;
}


/*************************************************************************************
  * Function : forward_sm_MO_packet()
  * Input    : input_data, message_length 
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
unsigned char *forward_sm_MO_packet (sms_fields *input_data, int *message_length)
{
	unsigned char text_buf [160] = {0};
	unsigned char *tcap_buff;
	unsigned int temp_trx_id;
	tcap_buff = (unsigned char *) malloc (MAX_TCAP_MSG_LEN);
	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN); 
	memset (text_buf, 0, 160);
	char *input = input_data->sms_data;
	char *output = (char *)text_buf;
	ascii_to_gsm7(input, output);	
//	ascii_to_gsm7 (input_data->sms_data, (char *) text_buf);
	int string_iter;
    	int iterator = 0;
	int iterator_last = 0;
	unsigned char temp_char_DA[9] = {0};
	unsigned char temp_char_OA[9] = {0};
	unsigned char temp_char_TP[9] = {0};

	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN);
	tcap_buff [iterator + 0] = TAG_GSM_TCAP_BEGIN; // Tag: Begin Tcap
	tcap_buff [iterator + 3] = TAG_GSM_TCAP_TX_ID;   // Tag: Transaction ID 
	tcap_buff [iterator + 4] = 0x04;   // val: Transaction ID length (no. of bytes)
    /*
     * Put the Transaction ID here
     */
	temp_trx_id = (input_data->trx_id >>R_SHIFT_24);
	memcpy ((void *)&tcap_buff[iterator + 5],&temp_trx_id, 1);
	temp_trx_id = (input_data->trx_id >>R_SHIFT_16);
	memcpy ((void *)&tcap_buff[iterator + 6],&temp_trx_id, 1);
	temp_trx_id = (input_data->trx_id >>R_SHIFT_8);
	memcpy ((void *)&tcap_buff[iterator + 7], &temp_trx_id, 1);
	memcpy ((void *)&tcap_buff[iterator + 8], &input_data->trx_id, 1);

	tcap_buff [iterator + 9] = 0x6b;   // Tag: Dialogue Portion
	tcap_buff [iterator + 10] = 0x1a;   // val: length of OID + 4

	tcap_buff [iterator + 11] = TAG_DLG_RESP;  // Tag: Dialogue Response 
	tcap_buff [iterator + 12] = 0x18;  // val: Length 0f OID + 2

	tcap_buff [iterator + 13] = TAG_TCAP_OID; // Tag: OID 
	tcap_buff [iterator + 14] = 0x07; // val: length of message (OID)
	tcap_buff [iterator + 16] = 0x11; // val: 5th byte of OID
	tcap_buff [iterator + 17] = 0x86; // val: 4th byte of OID
	tcap_buff [iterator + 18] = 0x05; // val: 3rd byte of OID
 	tcap_buff [iterator + 19] = 0x01; // val: 2nd byte of OID
	tcap_buff [iterator + 20] = 0x01; // val: 1st byte of OID
	tcap_buff [iterator + 21] = 0x01; // val: 0th byte of OID

	tcap_buff [iterator + 22] = TAG_ASN_1_TYPE; // Tag: Single ASN 1 type
	tcap_buff [iterator + 23] = 0x0d; // val: length till application context

	tcap_buff [iterator + 24] = TAG_DLG_REQUEST; // Tag: Dialogue Request 
	tcap_buff [iterator + 25] = 0x0b; // val: length till application context

	tcap_buff [iterator + 26] = TAG_APPL_CTXT_NAME; // Tag: Application Context Name tag 
	tcap_buff [iterator + 27] = 0x09; // Val: length of Context name  

	tcap_buff [iterator + 28] = TAG_TCAP_OID; // Tag: Object Identifier tag
	tcap_buff [iterator + 29] = 0x07; // val: length of Object Identifier
	tcap_buff [iterator + 30] = 0x04; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
                            // shortMsgMT-RelayContext-v2
	tcap_buff [iterator + 33] = 0x01; // val: 3rd byte of context  
	tcap_buff [iterator + 35] = 0x15; // val: 1st byte of context
	tcap_buff [iterator + 36] = 0x02; // val: 0th byte of context

	tcap_buff [iterator + 37] = TAG_COMP_PORTN; // Tag: Component Portion, signifies next payload
	tcap_buff [iterator + 40] = TAG_GSM_MAP_HDR; // Tag: GSM MAP header
	tcap_buff [iterator + 43] = TAG_COMPONENT; // Tag: Component
	tcap_buff [iterator + 44] = TAG_INVOKE;  // Tag: Invoke  
	tcap_buff [iterator + 45] = 0x01;  // val: Tag ID

	tcap_buff [iterator + 46] = TAG_COMPONENT; // Tag: opcode:locavalue
	tcap_buff [iterator + 47] = 0x01; // val: length in bytes
	tcap_buff [iterator + 48] = MO_FORWARD_SM;

	tcap_buff [iterator + 49] = TAG_CONSTRUCTOR; // Tag: Constructor   
	tcap_buff [iterator + 52] = TAG_SVC_CENTR_ADDR; // Tag: Service center Address
	temp_trx_id = ((input_data->SM_RP_DA.length +1)/2 + 1);
	memcpy (&tcap_buff [iterator + 53], (void *)&temp_trx_id, 1);	
    	tcap_buff [iterator + 54] = 0x91; // val: to find the kind of Number 
	
	encode_number_or_time (input_data->SM_RP_DA.number, 
			temp_char_DA, input_data->SM_RP_DA.length);
	for (string_iter = 0; string_iter <(input_data->SM_RP_DA.length +1)/2 ; ++string_iter)
	{
		tcap_buff[ 55 + string_iter] = temp_char_DA[string_iter];
	}
	iterator = 55 + string_iter;
 
	tcap_buff [iterator ] = TAG_MSISDN; // Tag: Service Centre Address 
	temp_trx_id = ((input_data->SM_RP_OA.length +1)/2 + 1);
	memcpy(&tcap_buff [iterator + 1], (void *)&temp_trx_id,1);	
	tcap_buff [iterator + 2] = 0x91; // val: Service centre - 0th byte 
	
	iterator = iterator + 3;
	encode_number_or_time (input_data->SM_RP_OA.number, temp_char_OA,
		input_data->SM_RP_OA.length);
	for (string_iter = 0; string_iter <(input_data->SM_RP_OA.length +1)/2 ; ++string_iter)
    	{
		tcap_buff [iterator + string_iter] = temp_char_OA [string_iter];
	}
	iterator = iterator + string_iter;

	tcap_buff [iterator ] = TAG_GSM_SMS_PDU; // Tag: GSM SMS TPDU
	iterator_last = iterator + 1;
	tcap_buff [iterator + 3] = 0x01; // val bit-by-bit: TP-RP(7), 
					 	   // TP-UDHI(6), TP-SRR(5), 
						   // TP-VPF(4-3),TP-RD(2), TP-MTI(0-1)
	tcap_buff [iterator + 4] = 0x01 ; //TP - MR SMS message reference
	temp_trx_id = input_data->TP_Orig_Dest_Number.length;
	memcpy (&tcap_buff [iterator + 5], (void *)&temp_trx_id, 
			sizeof (char));// val: length in bytes  of destn no.
	tcap_buff [iterator + 6] = 0x91; // val bit-by-bit: no-extension(7th), 
                           // type of number(4 to 6), 
                           // Numbering Plan(0 to 3) 
	iterator = iterator + 7;
	encode_number_or_time (input_data->TP_Orig_Dest_Number.number, 
		temp_char_TP, input_data->TP_Orig_Dest_Number.length);

   	 for (string_iter = 0; 
		string_iter <((input_data->TP_Orig_Dest_Number.length +1)/2); 
		++string_iter)
    	{
		tcap_buff [iterator + string_iter] = temp_char_TP [string_iter];
	}
	iterator = iterator + string_iter;
	tcap_buff [iterator] = 0x00; // TP - PID  
	tcap_buff [iterator + 1] = 0x00; // TP - DCS
	iterator += 2;

	tcap_buff [iterator ] = strlen ((char *)text_buf) ;
	unsigned int tcap_buff_iter = iterator + 1;
	for (string_iter = 0; string_iter < strlen ((char *)text_buf); 
		++string_iter)
	{
		if (text_buf[string_iter] == '\0') {
			//printf ("=== Found '/ 0' at position %d\n", string_iter);
			break;
		}

		if (text_buf[string_iter] < 0) {
			tcap_buff [tcap_buff_iter] = text_buf [string_iter] + 256;
		}
		else
			tcap_buff [tcap_buff_iter] = text_buf [string_iter];
		++tcap_buff_iter;
	}
	tcap_buff [iterator_last ]    = LARGE_MSG_LEN_IDF; // val: gsm sms message length 
	tcap_buff [iterator_last +1]  = 0x07 + (input_data->TP_Orig_Dest_Number.length +1)/2 + string_iter; // val: gsm sms message length 
	tcap_buff [50] 				= LARGE_MSG_LEN_IDF; // val: length of whole of remaining message
	tcap_buff [51] 				= 0x10 + (input_data->SM_RP_DA.length +1)/2 + (input_data->SM_RP_OA.length +1)/2 +
				(input_data->TP_Orig_Dest_Number.length +1)/2 + string_iter; // val: length of whole of remaining message
	tcap_buff [38] 				= LARGE_MSG_LEN_IDF; // val: length 0f message - 2
	tcap_buff [39] 				= 0x1c + (input_data->SM_RP_DA.length +1)/2 + (input_data->SM_RP_OA.length +1)/2 + 
				(input_data->TP_Orig_Dest_Number.length +1)/2 +string_iter; // val: length 0f message - 2
	tcap_buff [41] 				= LARGE_MSG_LEN_IDF; // val: length 0f message - 2
	tcap_buff [42] 				= 0x19 + (input_data->SM_RP_DA.length +1)/2 + (input_data->SM_RP_OA.length +1)/2 + 
				(input_data->TP_Orig_Dest_Number.length +1)/2 +string_iter; // val: length 0f message - 2
	tcap_buff [1] 				= LARGE_MSG_LEN_IDF;
	tcap_buff [2] 				= tcap_buff_iter -3;
	tcap_buff [tcap_buff_iter] 		= '\0';
	
	*message_length = tcap_buff_iter;	 
	 return tcap_buff;
}


/*************************************************************************************
  * Function : forward_sm_end_ack_packet()
  * Input    : message_length, type, trx_id
  * Output   : None
  * Purpose  : To create ack package 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
unsigned char *forward_sm_end_ack_packet (int *message_length, char type, unsigned int trx_id)
{
	unsigned char *tcap_buff;
    	char  ack_type;
	unsigned int temp_trx_id;
    	int iterator =0;
    	ack_type = type;
    
	tcap_buff = (unsigned char *) malloc (MAX_TCAP_MSG_LEN);
	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN);
	//memset (text_buf, 0, 160);
	printf ("\n");
    
	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN);
	tcap_buff [iterator + 0] = TAG_GSM_TCAP_END; // Tag: Begin Tcap
	tcap_buff [iterator + 1] = 0x2e;   // Length 
	tcap_buff [iterator + 2] = TAG_GSM_TCAP_TX_ID_DEST;   // Tag: Transaction ID 
	/*
	* Put the Transaction ID here
	*/
	tcap_buff [iterator +3] = 0x04;   // val: Transaction ID length (no. of bytes)
	temp_trx_id = (trx_id >>R_SHIFT_24);
	memcpy ((void *)&tcap_buff[iterator +4],&temp_trx_id, 1);
	temp_trx_id = (trx_id >>R_SHIFT_16);
	memcpy ((void *)&tcap_buff[iterator +5],&temp_trx_id, 1);
	temp_trx_id = (trx_id >>R_SHIFT_8);
	memcpy ((void *)&tcap_buff[iterator +6], &temp_trx_id, 1);
	memcpy ((void *)&tcap_buff[iterator +7], &trx_id, 1);

	tcap_buff [iterator + 8] = 0x6b;   // Tag: Dialogue Portion
	tcap_buff [iterator + 9] = 0x1f;   // val: length of OID + 4

	tcap_buff [iterator + 10] = TAG_DLG_RESP;  // Tag: Dialogue Response 
	tcap_buff [iterator + 11] = 0x1d;  // val: Length 0f OID + 2

	tcap_buff [iterator + 12] = TAG_TCAP_OID; // Tag: OID 
	tcap_buff [iterator + 13] = 0x07; // val: length of message (OID)
	tcap_buff [iterator + 14] = 0; // val: length of message (OID)
	tcap_buff [iterator + 15] = 0x11; // val: 5th byte of OID
	tcap_buff [iterator + 16] = 0x86; // val: 4th byte of OID
	tcap_buff [iterator + 17] = 0x05; // val: 3rd byte of OID
	tcap_buff [iterator + 18] = 0x01; // val: 2nd byte of OID
	tcap_buff [iterator + 19] = 0x01; // val: 1st byte of OID
	tcap_buff [iterator + 20] = 0x01; // val: 0th byte of OID

	tcap_buff [iterator + 21] = TAG_ASN_1_TYPE; // Tag: Single ASN 1 type
	tcap_buff [iterator + 22] = 0x12; // val: length till application context
 
	tcap_buff [iterator + 23] = TAG_DLG_RESPONSE; // Tag: Dialogue Response 
	tcap_buff [iterator + 24] = 0x10; // val: length till application context
 
	tcap_buff [iterator + 25] = TAG_APPL_CTXT_NAME; // Tag: Application Context Name tag 
	tcap_buff [iterator + 26] = 0x09; // Val: length of Context name  
 
	tcap_buff [iterator + 27] = TAG_TCAP_OID; // Tag: Object Identifier tag
	tcap_buff [iterator + 28] = 0x07; // val: length of Object Identifier
	tcap_buff [iterator + 29] = 0x04; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
	tcap_buff [iterator + 30] = 0; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
	tcap_buff [iterator + 31] = 0; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
                           // shortMsgMT-RelayContext-v2
	tcap_buff [iterator + 32] = 0x01; // val: 3rd byte of context  
	tcap_buff [iterator + 33] = 0; // val: 3rd byte of context  
   
	if (ack_type == MO_FORWARD_ACK)	{
    	tcap_buff [iterator + 34] = 0x15; // val: 1st byte of context
		tcap_buff [iterator + 35] = 0x02; // val: 0th byte of context
	}
	
	if (ack_type == MT_FORWARD_ACK) {
    	tcap_buff [iterator + 34] = 0x19; // val: 1st byte of context
		tcap_buff [iterator + 35] = 0x03; // val: 0th byte of context
	}
    
	tcap_buff [iterator + 36] = TAG_DLG_RESULT;  //Result Tag     
	tcap_buff [iterator + 37] = 0x03; //Length

	tcap_buff [iterator + 38] = 0x02;
	tcap_buff [iterator + 39] = 0x01;
	tcap_buff [iterator + 40] = 0x00;
    
	tcap_buff [iterator + 41] = TAG_COMP_PORTN; // Tag: Component Portion, signifies next payload
	tcap_buff [iterator + 42] = 0x05;//length
	tcap_buff [iterator + 43] = TAG_MAP_RETURN_RES;//TAG_GSM_MAP_HDR; // Tag: GSM MAP header
	tcap_buff [iterator + 44] = 0x03;//length
	tcap_buff [iterator + 45] = TAG_COMPONENT; // Tag: Component
	tcap_buff [iterator + 46] = TAG_INVOKE;  // Tag: Invoke  
	tcap_buff [iterator + 47] = 0x01;  // va  

	*message_length = 48;
	return tcap_buff;
}


/*************************************************************************************
  * Function : forward_sm_ack_packet_continue()
  * Input    : message_length, type, src_trx_id, dest_trx_id
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
unsigned char *forward_sm_ack_packet_continue (int *message_length, char  type,
	 unsigned int src_trx_id, unsigned int dest_trx_id)
{
	unsigned char *tcap_buff;
    	char  ack_type;
	unsigned int temp_trx_id;
    	int iterator =0;
    	ack_type = type;
    
	tcap_buff = (unsigned char *) malloc (MAX_TCAP_MSG_LEN);
	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN);
	//memset (text_buf, 0, 160);
	printf ("\n");
    
	memset (tcap_buff, 0, MAX_TCAP_MSG_LEN);
	tcap_buff [iterator + 0] = TAG_GSM_TCAP_CONTINUE; // Tag: Continue Tcap
	tcap_buff [iterator + 1] = 0x34;   // Length 
	tcap_buff [iterator + 2] = TAG_GSM_TCAP_TX_ID;   // Tag: Transaction ID 
	/*
	* Put the Transaction ID here
	*/
	tcap_buff [iterator +3] = 0x04;   // val: Transaction ID length (no. of bytes)
	temp_trx_id = (src_trx_id >>R_SHIFT_24);
	memcpy ((void *)&tcap_buff[iterator +4],&temp_trx_id, 1);
	temp_trx_id = (src_trx_id >>R_SHIFT_16);
	memcpy ((void *)&tcap_buff[iterator +5],&temp_trx_id, 1);
	temp_trx_id = (src_trx_id >>R_SHIFT_8);
	memcpy ((void *)&tcap_buff[iterator +6], &temp_trx_id, 1);
	memcpy ((void *)&tcap_buff[iterator +7], &src_trx_id, 1);
   
	tcap_buff [iterator + 8] = TAG_GSM_TCAP_TX_ID_DEST;   // Tag: Transaction ID 
	/*
	* Put the Transaction ID here
	*/
	tcap_buff [iterator + 9] = 0x04;   // val: Transaction ID length (no. of bytes)
	temp_trx_id = (dest_trx_id >>R_SHIFT_24);
	memcpy ((void *)&tcap_buff[iterator +10],&temp_trx_id, 1);
	temp_trx_id = (dest_trx_id >>R_SHIFT_16);
	memcpy ((void *)&tcap_buff[iterator +11],&temp_trx_id, 1);
	temp_trx_id = (dest_trx_id >>R_SHIFT_8);
	memcpy ((void *)&tcap_buff[iterator +12], &temp_trx_id, 1);
	memcpy ((void *)&tcap_buff[iterator +13], &dest_trx_id, 1);

	iterator = iterator + 6;

	tcap_buff [iterator + 8] = 0x6b;   // Tag: Dialogue Portion
	tcap_buff [iterator + 9] = 0x1f;   // val: length of OID + 4

	tcap_buff [iterator + 10] = TAG_DLG_RESP;  // Tag: Dialogue Response 
	tcap_buff [iterator + 11] = 0x1d;  // val: Length 0f OID + 2

	tcap_buff [iterator + 12] = TAG_TCAP_OID; // Tag: OID 
	tcap_buff [iterator + 13] = 0x07; // val: length of message (OID)
	tcap_buff [iterator + 14] = 0; // val: length of message (OID)
	tcap_buff [iterator + 15] = 0x11; // val: 5th byte of OID
	tcap_buff [iterator + 16] = 0x86; // val: 4th byte of OID
	tcap_buff [iterator + 17] = 0x05; // val: 3rd byte of OID
	tcap_buff [iterator + 18] = 0x01; // val: 2nd byte of OID
	tcap_buff [iterator + 19] = 0x01; // val: 1st byte of OID
	tcap_buff [iterator + 20] = 0x01; // val: 0th byte of OID

	tcap_buff [iterator + 21] = TAG_ASN_1_TYPE; // Tag: Single ASN 1 type
	tcap_buff [iterator + 22] = 0x12; // val: length till application context
 
	tcap_buff [iterator + 23] = TAG_DLG_RESPONSE; // Tag: Dialogue Response 
	tcap_buff [iterator + 24] = 0x10; // val: length till application context
 
	tcap_buff [iterator + 25] = TAG_APPL_CTXT_NAME; // Tag: Application Context Name tag 
	tcap_buff [iterator + 26] = 0x09; // Val: length of Context name  
 
	tcap_buff [iterator + 27] = TAG_TCAP_OID; // Tag: Object Identifier tag
	tcap_buff [iterator + 28] = 0x07; // val: length of Object Identifier
	tcap_buff [iterator + 29] = 0x04; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
	tcap_buff [iterator + 30] = 0; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
	tcap_buff [iterator + 31] = 0; // val: 6th byte of context -- 04 00 00 01 00 19 02 for
                           // shortMsgMT-RelayContext-v2
	tcap_buff [iterator + 32] = 0x01; // val: 3rd byte of context  
	tcap_buff [iterator + 33] = 0; // val: 3rd byte of context  
   
 	if (ack_type == MO_FORWARD_ACK){
    		tcap_buff [iterator + 34] = 0x15; // val: 1st byte of context
		tcap_buff [iterator + 35] = 0x02; // val: 0th byte of context
	}
	
   	 if (ack_type == MT_FORWARD_ACK){
    		tcap_buff [iterator + 34] = 0x19; // val: 1st byte of context
		tcap_buff [iterator + 35] = 0x03; // val: 0th byte of context
	}
    
	tcap_buff [iterator + 36] = TAG_DLG_RESULT;  //Result Tag     
	tcap_buff [iterator + 37] = 0x03; //Length

	tcap_buff [iterator + 38] = 0x02;
	tcap_buff [iterator + 39] = 0x01;
	tcap_buff [iterator + 40] = 0x00;
    
	tcap_buff [iterator + 41] = TAG_COMP_PORTN; // Tag: Component Portion, signifies next payload
	tcap_buff [iterator + 42] = 0x05;//length
	tcap_buff [iterator + 43] = TAG_GSM_MAP_HDR; // Tag: GSM MAP header
	tcap_buff [iterator + 44] = 0x03;//length
	tcap_buff [iterator + 45] = TAG_COMPONENT; // Tag: Component
	tcap_buff [iterator + 46] = TAG_INVOKE;  // Tag: Invoke  
	tcap_buff [iterator + 47] = 0x01;  // va  

	*message_length = 54;
	return tcap_buff;
}


/*************************************************************************************
  * Function : form_sri_ack()
  * Input    : trx_id, IMSI, LMSI, ack_length
  * Output   : None
  * Purpose  : To form SRI_ACK packet 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
unsigned char *form_sri_ack (unsigned int trx_id, unsigned char * IMSI, 
	unsigned char * LMSI, int *ack_length)
{
	unsigned char *ack_buf = NULL;
	unsigned int temp_trx_id;
	unsigned char temp_IMSI [MAX_OCT_MSISDN_LEN] = {0};
	int string_iter =0;
	int IMSI_len;
	int LMSI_len;
	IMSI_len = strlen ((char *)IMSI);
	LMSI_len = strlen ((char *)LMSI);
 
	ack_buf = (unsigned char *) malloc (MAX_TCAP_MSG_LEN);
	int iterator = 0;

	ack_buf [iterator + 0] = TAG_GSM_TCAP_END;
	ack_buf [iterator + 2] = TAG_GSM_TCAP_TX_ID_DEST;
	ack_buf [iterator + 3] = 0x04;
	
	/* copy transaction ID */
	temp_trx_id = (trx_id >> R_SHIFT_24);
	memcpy ((void *)&ack_buf[iterator + 4], &temp_trx_id, 1);
	temp_trx_id = (trx_id >> R_SHIFT_16);
	memcpy ((void *)&ack_buf[iterator + 5],&temp_trx_id, 1);
	temp_trx_id = (trx_id >> R_SHIFT_8);
	memcpy ((void *)&ack_buf[iterator + 6], &temp_trx_id, 1);
	memcpy ((void *)&ack_buf[iterator + 7], &trx_id, 1);	
	
	ack_buf [iterator + 8]  = TAG_TCAP_DLG_PORTN;
	ack_buf [iterator + 9]  = 0x26;//Length
	ack_buf [iterator + 10] = TAG_DLG_RESP;
	ack_buf [iterator + 11] = 0x24; //Length
	ack_buf [iterator + 12] = TAG_TCAP_OID;
	ack_buf [iterator + 13] = 0x07;
	ack_buf [iterator + 14] = 0x00;
	ack_buf [iterator + 15] = 0x11;
	ack_buf [iterator + 16] = 0x86;
	ack_buf [iterator + 17] = 0x05;
	ack_buf [iterator + 18] = 0x01;
	ack_buf [iterator + 19] = 0x01;
	ack_buf [iterator + 20] = 0x01;

	ack_buf [iterator + 21] = TAG_ASN_1_TYPE;
	ack_buf [iterator + 22] = 0x19;//Length

	/* ==================================== */
	ack_buf [iterator + 23] = TAG_DLG_RESPONSE;
	ack_buf [iterator + 24] = 0x17; //Length till the end of dialogue response
	ack_buf [iterator + 25] = TAG_APPL_CTXT_NAME;
	ack_buf [iterator + 26] = 0x09;
	ack_buf [iterator + 27] = TAG_TCAP_OID;
	ack_buf [iterator + 28] = 0x07;
	/* ShortMessageGatewayContext-v2 */
	ack_buf [iterator + 29] = 0x04;
	ack_buf [iterator + 30] = 0x00;
	ack_buf [iterator + 31] = 0x00;
	ack_buf [iterator + 32] = 0x01;
	ack_buf [iterator + 33] = 0x00;
	ack_buf [iterator + 34] = 0x14;
	ack_buf [iterator + 35] = 0x02;

	ack_buf [iterator + 36] = TAG_DLG_RESULT;
	ack_buf [iterator + 37] = 0x03;
	ack_buf [iterator + 38] = TAG_COMPONENT;
	ack_buf [iterator + 39] = 0x01;
	ack_buf [iterator + 40] = 0x00;

	ack_buf [iterator + 41] = TAG_DLG_RESULT_SRC_DGN;
	ack_buf [iterator + 42] = 0x05;
	ack_buf [iterator + 43] = TAG_APPL_CTXT_NAME;
	ack_buf [iterator + 44] = 0x03;
	/* **dialogue service user** */
	ack_buf [iterator + 45] = TAG_COMPONENT;
	ack_buf [iterator + 46] = 0x01;
	ack_buf [iterator + 47] = 0x00; /* ... dialogue end here */

	/* ======================================== */
	ack_buf [iterator + 48] = TAG_COMP_PORTN; 
	ack_buf [iterator + 49] = 19 + ((LMSI_len + 1) / 2) + (IMSI_len + 1)/2; //Length
	/* ============== components ============== */

	ack_buf [iterator + 50] = TAG_DLG_RESULT; 
	ack_buf [iterator + 51] = 17 + ((LMSI_len + 1) / 2) + (IMSI_len + 1)/2; //Length

	ack_buf [iterator + 52] = TAG_COMPONENT; 
	ack_buf [iterator + 53] = 0x01; 
	ack_buf [iterator + 54] = 0x01; 


	ack_buf [iterator + 55] = TAG_CONSTRUCTOR; 
	ack_buf [iterator + 56] = 12 + ((LMSI_len + 1) / 2) + (IMSI_len + 1)/2; //Length
	ack_buf [iterator + 57] = TAG_COMPONENT; 
	/* **opcode - localvalue (0)** */	
	ack_buf [iterator + 58] = 0x01; 
	ack_buf [iterator + 59] = TAG_SEND_ROUT_INFO; 

	ack_buf [iterator + 60] = TAG_CONSTRUCTOR; 
	ack_buf [iterator + 61] = 7 + ((LMSI_len + 1) / 2) + (IMSI_len + 1)/2; //Length 
	ack_buf [iterator + 62] = TAG_GSM_SMS_PDU; 

	ack_buf [iterator + 63] = (IMSI_len + 1)/2; 
	
	encode_number_or_time (IMSI, temp_IMSI,IMSI_len);
	for (string_iter = 0; string_iter < (IMSI_len +1)/2 ; 
            ++string_iter)
	{    
		ack_buf[ 64 + string_iter] = temp_IMSI[string_iter];	
	}    
   
	iterator = 64 + (IMSI_len +1)/2 ;
	ack_buf [iterator ] = TAG_ASN_1_TYPE; 
	ack_buf [iterator + 1] = (((LMSI_len + 1) / 2) + 1) + 2; 
	ack_buf [iterator + 2] = LARGE_MSG_LEN_IDF; 
	ack_buf [iterator + 3] = ((LMSI_len + 1) / 2) + 1; 
	ack_buf [iterator + 4] = 0x91; 

	iterator = iterator + 5 ;

	memset(temp_IMSI,0,MAX_OCT_MSISDN_LEN);
	encode_number_or_time(LMSI,temp_IMSI,LMSI_len);
	for (string_iter = 0; string_iter < (LMSI_len +1)/2 ; ++string_iter)
	{    
		ack_buf[ iterator + string_iter] = temp_IMSI[string_iter];
	}    
	
	iterator = iterator + (LMSI_len +1)/2 ;//+ 1;
	ack_buf [iterator] ='\0';	
	*ack_length = iterator;
	ack_buf[1] = iterator - 2;
	
	return ack_buf;	
}






unsigned char *alert_service_center_without_result (sms_fields *input_data, int *message_length)
{
	int iterator 	= 0;
	unsigned char *sri = NULL;
	sri = (unsigned char*) malloc (MAX_TCAP_MSG_LEN);
	unsigned int temp_trx_id = 0;
	int string_iter = 0;
	unsigned char temp_char_DA[30] = {0};
	unsigned char temp_service_center_addr[30] = {0};
	
	sri[iterator++] = TAG_GSM_TCAP_BEGIN;
	sri[iterator++] = SRI_PACKET_LEN - 3;
	sri[iterator++] = TAG_GSM_TCAP_TX_ID;
	sri[iterator++] = SRI_TRANSACTION_ID_LEN;
	
	temp_trx_id = (input_data->trx_id >>R_SHIFT_24);
	memcpy ((void *)&sri[iterator++], &temp_trx_id, 1);
	temp_trx_id = (input_data->trx_id >>R_SHIFT_16);
	memcpy ((void *)&sri [iterator++], &temp_trx_id, 1);
	temp_trx_id = (input_data->trx_id >> R_SHIFT_8);
	memcpy ((void *)&sri [iterator++], &temp_trx_id, 1);
	memcpy ((void *)&sri [iterator++], &input_data->trx_id, 1);	

	
	sri[iterator++] = TAG_TCAP_DLG_PORTN; 
	sri[iterator++] = TAG_TCAP_DLG_PORTN_LEN; /*length till end of dialogue*/
	sri[iterator++] = 0x28;
	sri[iterator++] = 0x2d; /*length till end of dialogue*/

	/*DIALOGUE PORTION*/
	sri[iterator++] = TAG_OID;
	sri[iterator++] = TAG_LEN_DLG_ID;
	iterator++;

	/*OID - ID as Dialogue */
	sri[iterator++] = 0x11;
	sri[iterator++] = 0x86;
	sri[iterator++] = 0x05;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0xa0;/* Tag of dialogue request  */
	sri[iterator++] = 0x22;/*length till end of dialogue*/
	
	sri[iterator++] = TAG_DLG_REQUEST;/* Tag of dialogue request  */
	sri[iterator++] = TAG_DLG_REQUEST_LEN;/*length till end of dialogue*/
	
	/*Dialogue Request*/
	sri[iterator++] = 0x80;
	sri[iterator++] = 0x02;
	sri[iterator++] = TAG_PADDING;/*Padding*/
	sri[iterator++] = TAG_PROTOCOL_VERSION;/*Protocol Version*/
	sri[iterator++] = 0xa1;
	sri[iterator++] = 0x09;/*length in bytes*/
	
	sri[iterator++] = TAG_APPLICATION_CONTEXT;/*TAG_APPLICATION CONTEXT NAME*/
	sri[iterator++] = TAG_APPLICATION_CONTEXT_LEN;/*Length in bytes*/
	
	/*Short Message Gateway context Name*/
	sri[iterator++] = 0x04;
	sri[iterator++] = 0x62;
	iterator ++;
	sri[iterator++] = 0x01;
	iterator ++;
	sri[iterator++] = 0x14;
	sri[iterator++] = 0x02;

	sri[iterator++] = TAG_USER_INFO;/* User Information Tag  */
	sri[iterator++] = TAG_USER_INFO_LEN;/* Length of User Information Tag  */
	
	/* User Information */
	sri[iterator++] = TAG_USER_INFO_ITEM;
	sri[iterator++] = TAG_USER_INFO_ITEM_LEN;

	sri[iterator++] = TAG_DIRECT_REFERENCE;
	sri[iterator++] = TAG_DIRECT_REFERENCE_LEN;
	
	sri[iterator++] = 0x04;
	iterator +=2;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;
	sri[iterator++] = 0x01;

	/* Encoding */	
	sri[iterator++] = TAG_ENCODING;/* Encoding Tag */
	sri[iterator++] = TAG_ENCODING_LEN;/* Length of encoding in bytes*/
	
	sri[iterator++] = TAG_ENCODING_TYPE; /* Encoding */
	iterator++;		/* Encoding */ 

	
	sri[iterator++] = TAG_GSM_MOBILE_APPLICATION;
	sri[iterator++] = 0x1c;
	
	/* GSM MOBILE APPLICATION */
	sri[iterator++] = TAG_COMPONENT_PORTN;
	sri[iterator++] = TAG_COMPONENT_PORTN_LEN - 3;
	
	sri[iterator++] = 0X02;/*Tag return result last*/
	sri[iterator++] = 0X01;/*Length*/

	sri[iterator++] = TAG_INVOKE_ID;/*value Invoke ID*/

	/* Opcode */
	sri[iterator++] = 0X02;
	sri[iterator++] = 0X01;
	//sri[iterator++] = SRI_PACKET_IDENTIFIER;
	sri[iterator++] = 0x31;
	
	/*Constructor*/
	sri[iterator++] = TAG_CONSTRUCTOR;/* Constructor Tag */
	sri[iterator++] = TAG_CONSTRUCTOR_LEN - 3;/* Length of remaining packet */

	/* msisdn */
	sri[iterator++] = 0x04;/* Parameter - 1 Tag */
	sri[iterator++] = TAG_SRI_MSISDN_LEN;/* Length in bytes */
	sri[iterator++] = TAG_NATURE_OF_NUMBER;/* Nature of Number */
	
	encode_number_or_time (input_data->SM_RP_DA.number,
                                   temp_char_DA, input_data->SM_RP_DA.length);
	for (string_iter = 0; string_iter < (input_data->SM_RP_DA.length +1)/2; 
		++string_iter)
    {
		sri[iterator++] = temp_char_DA[string_iter];
	}
	
	/* SM_RP_PRI */	
	/* Service Center Address */	
	sri[iterator++] = 0x04;/* Parameter - 3 tag */
	sri[iterator++] = TAG_SERVICE_CENTER_ADDR_LEN;/* Length in bytes */
	sri[iterator++] = TAG_NATURE_OF_NUMBER;

	encode_number_or_time (input_data->SM_RP_OA.number,
					temp_service_center_addr, input_data->SM_RP_OA.length);
	for (string_iter = 0; string_iter < (input_data->SM_RP_OA.length +1)/2; 
			++string_iter)
	{
		sri[iterator++] = temp_service_center_addr[string_iter];
	}
	*message_length = iterator;
	return(sri);	
}

