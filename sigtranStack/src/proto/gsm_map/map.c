 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines GSM MAP packet formation and  
 *            has definitions for:
 *          - mo forward req
 *          - mo forward res
 *          - mt forward req
 *          - mt forward res
 *          - alert svc cntr req
 *          - alert svc cntr res
 *          - mo sri res 
 *          - mo sri req 
 *          - gsm map frame 
 *          - parse map message 
 */

#include <string.h>
#include "map.h"
#include "tcap.h"

/*************************************************************************************
  * Function : mo_forward_req()
  * Output   : None
  * Purpose  : 
  * Author   : tuhin.shankar.dey@gmail.com
  * Note     : 
**************************************************************************************/
static int mo_forward_req (unsigned char *map_buffer, void *params)
{
	int msg_len = 0;
	int comp_portn_offset = 0;
	int map_hdr_offset = 0;
	int constructor_offset = 0;
	int gsm_pdu_offset = 0;
	int msg_len_offset = 0;
	
	int tp_rp, tp_udhi, tp_srr, tp_vhf, tp_rd, 
		tp_mti, tp_mr, tp_da, tp_pid, tp_udl, tp_vpf, tp_dcs;

	tp_mti = tp_mr = 1;

	map_buffer[msg_len++] = TAG_GSM_MAP_HDR; 
	map_hdr_offset        = msg_len++;

	/** Opcode **/
	map_buffer[msg_len++] = TAG_INVOKE_ID; 
	map_buffer[msg_len++] = TAG_SHORT_LEN; 
	map_buffer[msg_len++] = DEFAULT_INVOKE_ID;
	map_buffer[msg_len++] = TAG_OPCODE; 
	map_buffer[msg_len++] = TAG_SHORT_LEN; 
	map_buffer[msg_len++] = MO_FORWARD_SM; 

	map_buffer[msg_len++] = TAG_CONSTRUCTOR; 
	constructor_offset    = msg_len;
	map_buffer[msg_len++] = constructor_offset; 

	/** Destination address **/
	map_buffer[msg_len++] = TAG_SVC_CENTR_ADDR; 
	map_buffer[msg_len++] = 
		(((sms_fields *)(params))->SM_RP_DA.length + 1)/2 + 1;
	map_buffer[msg_len++] = TAG_INTL_NUM; 
	encode_number_or_time (((sms_fields *)(params))->SM_RP_DA.number,
		&map_buffer[msg_len], ((sms_fields *)(params))->SM_RP_DA.length); 	
	msg_len += (((sms_fields *)(params))->SM_RP_DA.length + 1)/2 ;

	/** Originating Address **/
	map_buffer[msg_len++] = TAG_MSISDN; 
	map_buffer[msg_len++] = 
		(((sms_fields *)(params))->SM_RP_OA.length + 1)/2 + 1;
	map_buffer[msg_len++] = TAG_INTL_NUM; 
	encode_number_or_time (((sms_fields *)(params))->SM_RP_OA.number,
		&map_buffer[msg_len], ((sms_fields *)(params))->SM_RP_OA.length); 	
	msg_len += (((sms_fields *)(params))->SM_RP_OA.length + 1)/2 ;
	

	/*** GSM PDU Starts ***/
	map_buffer[msg_len++] = TAG_GSM_SMS_PDU;
	gsm_pdu_offset        = msg_len;
	map_buffer[msg_len++] = gsm_pdu_offset;
	map_buffer[msg_len++] = tp_rp + tp_udhi + tp_srr + tp_vpf 
				+ tp_rd + tp_mti;
	map_buffer[msg_len++] = tp_mr;
	map_buffer[msg_len++] = 
		((sms_fields *)(params))->TP_Orig_Dest_Number.length;	
	map_buffer[msg_len++] = TAG_INTL_NUM; 
	encode_number_or_time (((sms_fields *)(params))->TP_Orig_Dest_Number.number, 
		&map_buffer[msg_len], ((sms_fields *)(params))->TP_Orig_Dest_Number.length); 
	msg_len += (((sms_fields *)(params))->TP_Orig_Dest_Number.length + 1)/2;
	map_buffer[msg_len++] = tp_pid;
	map_buffer[msg_len++] = tp_dcs;
	msg_len_offset = msg_len;
	map_buffer[msg_len++] = msg_len;
	ascii_to_gsm7 (((sms_fields *)(params))->sms_data, (&map_buffer[msg_len]));
	/** length of the encoded message **/
	map_buffer[msg_len_offset] = strlen ((const char *)&map_buffer[msg_len]);
	msg_len += map_buffer[msg_len_offset];	
	
	/** adjust the offsets **/ 
	map_buffer[map_hdr_offset] = msg_len - map_hdr_offset -1 ;
	map_buffer[constructor_offset] = msg_len - constructor_offset -1;
	map_buffer[gsm_pdu_offset] = msg_len - gsm_pdu_offset -1;

	return (msg_len);		
}
/*************************************************************************************
  * Function : mo_forward_res()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
static int mo_forward_res (unsigned char *map_buffer, void *params)
{
	int msg_len = 0;
	int ret_res_offset = 0;

	map_buffer[msg_len++] = TAG_MAP_RETURN_RES;
	ret_res_offset        = msg_len;
	map_buffer[msg_len++] = ret_res_offset;
	map_buffer[msg_len++] = TAG_COMPONENT;	
	map_buffer[msg_len++] = TAG_INVOKE_ID;	
	map_buffer[msg_len++] = DEFAULT_INVOKE_ID;		

	/** adjust the offset for return result last **/
	map_buffer[ret_res_offset] = msg_len - ret_res_offset;

	return msg_len;		
}
/*************************************************************************************
  * Function : mt_forward_req()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
static int mt_forward_req (unsigned char *map_buffer, void *params)
{
	int msg_len = 0;
	int comp_portn_offset = 0;
	int map_hdr_offset = 0;
	int constructor_offset = 0;
	int gsm_pdu_offset = 0;
	int msg_len_offset = 0;
	unsigned char timestamp_buf[SMSC_TIMESTAMP_LEN];
	int tp_rp, tp_udhi, tp_srr, tp_vhf, tp_rd, 
		tp_mti, tp_mr, tp_da, tp_pid, tp_udl, tp_vpf, tp_dcs;
	
	tp_mti = tp_mr = 1;

	map_buffer[msg_len++] = TAG_GSM_MAP_HDR; 
	map_hdr_offset        = msg_len++;

	/** Opcode **/
	map_buffer[msg_len++] = TAG_INVOKE_ID; 
	map_buffer[msg_len++] = TAG_SHORT_LEN; 
	map_buffer[msg_len++] = DEFAULT_INVOKE_ID; 
	map_buffer[msg_len++] = TAG_OPCODE; 
	map_buffer[msg_len++] = TAG_SHORT_LEN; 
	map_buffer[msg_len++] = MT_FORWARD_SM; 

	map_buffer[msg_len++] = TAG_CONSTRUCTOR; 
	constructor_offset    = msg_len;
	map_buffer[msg_len++] = constructor_offset; 

	/** Destination address **/
	map_buffer[msg_len++] = TAG_IMSI; 
	map_buffer[msg_len++] = 
		(((sms_fields *)(params))->SM_RP_DA.length + 1)/2 ;
	encode_number_or_time (((sms_fields *)(params))->SM_RP_DA.number,
		&map_buffer[msg_len], ((sms_fields *)(params))->SM_RP_DA.length); 	
	msg_len += (((sms_fields *)(params))->SM_RP_DA.length + 1)/2;

	/** Originating address **/
	map_buffer[msg_len++] = TAG_SVC_CENTR_ADDR; 
	map_buffer[msg_len++] = 
		(((sms_fields *)(params))->SM_RP_OA.length + 1)/2 + 1;
	map_buffer[msg_len++] = TAG_INTL_NUM; 
	encode_number_or_time (((sms_fields *)(params))->SM_RP_OA.number,
		&map_buffer[msg_len], ((sms_fields *)(params))->SM_RP_OA.length); 	
	msg_len += (((sms_fields *)(params))->SM_RP_OA.length + 1)/2;
		
	/*** GSM PDU Starts ***/
	map_buffer[msg_len++] = TAG_GSM_SMS_PDU;
	gsm_pdu_offset        = msg_len;
	map_buffer[msg_len++] = gsm_pdu_offset;
	map_buffer[msg_len++] = TAG_MAP_DELIVER_SM; 
	map_buffer[msg_len++] = 
		((sms_fields *)(params))->TP_Orig_Dest_Number.length;	
	map_buffer[msg_len++] = TAG_INTL_NUM; 
	encode_number_or_time (((sms_fields *)(params))->TP_Orig_Dest_Number.number, 
		&map_buffer[msg_len], ((sms_fields *)(params))->TP_Orig_Dest_Number.length); 
	msg_len += (((sms_fields *)(params))->TP_Orig_Dest_Number.length + 1)/2;
	map_buffer[msg_len++] = tp_pid;
	map_buffer[msg_len++] = tp_dcs;
	encode_number_or_time (((sms_fields *)(params))->SMSC_timestamp, 
		timestamp_buf, MAX_TIMESTAMP_LEN);
	msg_len += MAX_TIMESTAMP_LEN;
	msg_len_offset = msg_len;
	map_buffer[msg_len++] = msg_len_offset;
	ascii_to_gsm7 (((sms_fields *)params)->sms_data, (&map_buffer[msg_len]));
	/** length of the encoded message **/
	map_buffer[msg_len_offset] = strlen ((const char *)&map_buffer[msg_len]);
	msg_len += map_buffer[msg_len_offset];	

	/** adjust the offsets **/ 
	map_buffer[map_hdr_offset] = msg_len - map_hdr_offset -1;
	map_buffer[constructor_offset] = msg_len - constructor_offset -1;
	map_buffer[gsm_pdu_offset] = msg_len - gsm_pdu_offset -1;

	return msg_len;		
}
/*************************************************************************************
  * Function : mt_forward_res()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
static int mt_forward_res (unsigned char *map_buffer, void *params)
{
	int msg_len = 0;
	int ret_res_offset = 0;

	map_buffer[msg_len++] = TAG_MAP_RETURN_RES;
	ret_res_offset = msg_len;
	map_buffer[msg_len++] = ret_res_offset;
	map_buffer[msg_len++] = TAG_COMPONENT;	
	map_buffer[msg_len++] = TAG_INVOKE_ID;	
	map_buffer[msg_len++] = DEFAULT_INVOKE_ID;

	/** adjust the offset for return result last **/
	map_buffer[ret_res_offset] = msg_len - ret_res_offset;

	return msg_len;		
}

/*************************************************************************************
  * Function : alert_svc_cntr_req()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
static int alert_svc_cntr_req (unsigned char *map_buffer, void *params)
{
	int msg_len = 0;
	return msg_len;		
}

/*************************************************************************************
  * Function : alert_svc_cntr_res()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
static int alert_svc_cntr_res (unsigned char *map_buffer, void *params)
{
	int msg_len = 0;
	return msg_len;		
}

/*************************************************************************************
  * Function : mo_sri_res()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
static int  mo_sri_res (unsigned char *map_buffer,void *params)
{
	int string_iter  = 0;
	int LMSI_len 	 = 0;
	int IMSI_len 	 = 0;
	int msg_len  	 = 0;	
	
	int tag_comp_offset           = 0;
	int tag_dialogue_offset       = 0;
	int tag_constructor_offset_1   = 0;
	int tag_constructor_offset_2   = 0;
	
	IMSI_len 	= strlen(((sms_fields*)(params))->IMSI);
	LMSI_len	= strlen(((sms_fields*)(params))->LMSI);

	/* ============== components ============== */

	map_buffer [msg_len++] 	 = TAG_DLG_RESULT;
	tag_dialogue_offset 	 = msg_len++;
				
	map_buffer [msg_len++]   = TAG_INVOKE_ID;
	map_buffer [msg_len++]   = TAG_SHORT_LEN;
	map_buffer [msg_len++]   = DEFAULT_INVOKE_ID;

	map_buffer [msg_len++]   = TAG_CONSTRUCTOR;
	tag_constructor_offset_1 = msg_len++;	

	map_buffer [msg_len++]   = TAG_OPCODE;
        
	/***opcode - localvalue (0)***/
	map_buffer [msg_len++]   = TAG_SHORT_LEN; 
	map_buffer [msg_len++]   = TAG_SEND_ROUT_INFO;

	map_buffer [msg_len++]   = TAG_CONSTRUCTOR;
	tag_constructor_offset_2 = msg_len++;
        
	map_buffer [msg_len++]   = TAG_GSM_SMS_PDU;
	map_buffer [msg_len++]   = (IMSI_len + 1)/2;
	encode_number_or_time(((sms_fields *)(params))->IMSI,
			 &map_buffer[msg_len], IMSI_len);
	msg_len += (IMSI_len + 1)/2;

	map_buffer [msg_len++]   = TAG_ASN_1_TYPE;
	map_buffer [msg_len++] 	 = (((LMSI_len + 1) / 2) + 1) + 2;
	map_buffer [msg_len++] 	 = LARGE_MSG_LEN_IDF;
	map_buffer [msg_len++] 	 = ((LMSI_len + 1) / 2) + 1;
	map_buffer [msg_len++] 	 = TAG_INTL_NUM;

	encode_number_or_time(((sms_fields *)(params))->LMSI,
			 &map_buffer[msg_len], LMSI_len);
	msg_len += (LMSI_len +1)/2 ;//+ 1;
	map_buffer [msg_len] 	 ='\0';
	
	map_buffer[tag_dialogue_offset] 	= msg_len - tag_dialogue_offset -1; 
	map_buffer[tag_constructor_offset_1] 	= msg_len - tag_constructor_offset_1 -1;
	map_buffer[tag_constructor_offset_2] 	= msg_len - tag_constructor_offset_2 -1;
	
	return msg_len;
}

/*************************************************************************************
  * Function : mo_sri_req()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
static int  mo_sri_req (unsigned char *map_buffer,void *params)
{
	int msg_len 			= 0;
	int tag_comp_offset		= 0;
	int tag_constructor_offset	= 0;	
		
	map_buffer[msg_len++] 	= TAG_COMPONENT_PORTN;
	tag_comp_offset 	= msg_len++;
	
	map_buffer[msg_len++] 	= TAG_RETURN_RESULT_LAST; 
	map_buffer[msg_len++] 	= TAG_SHORT_LEN;

	map_buffer[msg_len++] 	= TAG_INVOKE_ID;

	/* Opcode */
	map_buffer[msg_len++] 	= TAG_OPCODE;
	map_buffer[msg_len++] 	= TAG_SHORT_LEN;
	map_buffer[msg_len++] 	= SRI_PACKET_IDENTIFIER;

	/*Constructor*/
	map_buffer[msg_len++] 	= TAG_CONSTRUCTOR;
	tag_constructor_offset  = msg_len++;

	 /* msisdn */
	map_buffer[msg_len++] 	= TAG_SRI_MSISDN;/* Parameter - 1 Tag */
	map_buffer[msg_len++] 	= ((((sms_fields *)(params))->SM_RP_DA.length) + 1)/2 + 1;
	map_buffer[msg_len++] 	= TAG_NATURE_OF_NUMBER;

	encode_number_or_time (((sms_fields *)(params))->SM_RP_DA.number,
			&map_buffer[msg_len], ((sms_fields *)(params))->SM_RP_DA.length);
	msg_len += ((((sms_fields *)(params))->SM_RP_DA.length) + 1)/2;	
	
	/* SM_RP_PRI */
	map_buffer[msg_len++] 	= TAG_PARAMETER_2;/*Parameter - 2 tag */
	map_buffer[msg_len++] 	= TAG_SHORT_LEN;/* Length in bytes */
	msg_len++;

	/* Service Center Address */
	map_buffer[msg_len++] 	= TAG_SERVICE_CENTER_ADDR;/* Parameter - 3 tag */
	// map_buffer[msg_len++] = TAG_SERVICE_CENTER_ADDR_LEN;
	map_buffer[msg_len++] 	= ((((sms_fields *)(params))->SM_RP_OA.length) + 1)/2 + 1;
	map_buffer[msg_len++] 	= TAG_NATURE_OF_NUMBER;
		
	encode_number_or_time (((sms_fields *)(params))->SM_RP_OA.number,
			&map_buffer[msg_len], ((sms_fields *)(params))->SM_RP_OA.length);
	msg_len += ((((sms_fields *)(params))->SM_RP_OA.length) + 1)/2;
	
	map_buffer[tag_comp_offset] 		= msg_len - tag_comp_offset -1;
	map_buffer[tag_constructor_offset] 	= msg_len - tag_constructor_offset -1;
	
	return msg_len;
}

/*************************************************************************************
  * Function : gsm_map_frame()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
int gsm_map_frame (unsigned char *map_buffer, 
	void *params, int type)
{
	int msg_len = 0;

	switch (type)
	{
		case MAP_MO_FORWARD_SM_REQ:
			msg_len = mo_forward_req (map_buffer, params);
			break;
		case MAP_MT_FORWARD_SM_REQ:
			msg_len = mt_forward_req (map_buffer, params);
			break;
		case MAP_SRI_SM_REQ:
			msg_len = mo_sri_req (map_buffer, params);
			break;
		case ALERT_SVC_CENTER:
			msg_len = alert_svc_cntr_req (map_buffer, params);
			break;
		case MAP_MO_FORWARD_SM_RES:
			msg_len = mo_forward_res (map_buffer, params);
			break;
		case MAP_MT_FORWARD_SM_RES:
			msg_len = mt_forward_res (map_buffer, params);
			break;
		case MAP_SRI_SM_RES:
			msg_len = mo_sri_res (map_buffer, params);
			break;
		case ALERT_SVC_CENTER_RES:
			msg_len = alert_svc_cntr_res (map_buffer, params);
			break;
		default:
			break;	
	}
	return msg_len;
}

/*************************************************************************************
  * Function : parse_map_message()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
int parse_map_message (unsigned char *map_message, void * params)
{
	unsigned char sms_buffer[SMS_TEXT_LEN]          = {0};
	unsigned char msisdn [IMSI_LEN]					= {0};						
	int iterator            = 0;
	int counter      		= 0;
	int sms_len = 0;
	int map_msg_len = 0;
	int invoke_id_len = 0;
	int opcode_len = 0;
	int tag_constructor_len = 0;
	int msisdn_len = 0;
	int smsc_centre_len = 0;
	int gsm_pdu_len = 0;
	char DCS = 0x00;

	if ((map_message[iterator] != TAG_GSM_MAP_HDR) && 
			(map_message[iterator] != TAG_DLG_RESULT))
	{   
		((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
		return 0;
	}   
	iterator += 1;
	
	if (map_message[iterator - 1] == TAG_GSM_MAP_HDR )
	{
		map_msg_len = map_message[iterator++];
		if (map_message[iterator++] != TAG_INVOKE_ID)
    	{	   
        	((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
        	return 0;
    	}   
		invoke_id_len = map_message[iterator++];
		iterator += invoke_id_len;
	
		if (map_message[iterator++] != TAG_OPCODE)
    	{   
        	((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
        	return 0;
    	}   
		opcode_len =  map_message[iterator++];
	
		if (map_message[iterator] == SRI_PACKET_IDENTIFIER)
		{   
			iterator += 1;
			((map_params *)params)->map_opcode = SRI_PACKET_IDENTIFIER;
	
			if (map_message[iterator++] != TAG_CONSTRUCTOR)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}	   
			tag_constructor_len = map_message[iterator++];
		
			if ( map_message[iterator++] != TAG_SRI_MSISDN)
    		{   
        		((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
        		return 0;
    		}   
			msisdn_len = map_message[iterator++];
			iterator ++;
			memset(msisdn, 0, IMSI_LEN);
			for (counter=0; counter < msisdn_len -1; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time(msisdn, ((map_params *)params)->msisdn_to_HLR, msisdn_len - 1);	
			
			if (map_message[iterator++] != TAG_PARAMETER_2)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}   
			iterator ++; //Length of TAG_PARAMETER_2
			iterator ++; //Val
		
			if (map_message[iterator++] != TAG_SERVICE_CENTER_ADDR)
    		{   
        		((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
        		return 0;
    		}   
			msisdn_len = map_message[iterator++];
			iterator++;//Type of number skipped
	
			memset(msisdn, 0, IMSI_LEN);
			for(counter=0; counter < msisdn_len -1; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time (msisdn, 
				((map_params *)params)->service_centre_address, msisdn_len - 1);
		}
		else if (map_message[iterator] == MT_FORWARD_SM)
		{   
			iterator += 1;
			((map_params *)params)->map_opcode = MT_FORWARD_SM;
		
			if (map_message[iterator++] != TAG_CONSTRUCTOR)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}   
			tag_constructor_len = map_message[iterator++];
		
			/*Extracting IMSI of destination number */
			if (map_message[iterator++] != TAG_IMSI)
			{   
				((map_params *)params)->map_error_code = 
					ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}   
			msisdn_len = map_message[iterator++];
			memset(msisdn, 0, IMSI_LEN);
			for(counter=0; counter < msisdn_len; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time (msisdn, 
				((map_params *)params)->IMSI_Receiver, msisdn_len);	
	
			if (strlen (((map_params *)params)->IMSI_from_HLR) == IMSI_LEN)
				((map_params *)params)->IMSI_from_HLR[IMSI_LEN - 1] = '\0';
			
			/*Extracting MSISDN Number of sending SMSC */
			if ( map_message[iterator++] != TAG_MSISDN)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}   
			msisdn_len = map_message[iterator++];
			iterator++;
			memset(msisdn, 0, IMSI_LEN);
			for(counter=0; counter < msisdn_len -1; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}

			decode_number_or_time (msisdn, ((map_params *)params)->MSISDN_SMSC, msisdn_len - 1);	
		
			/* Parsing  GSM PDU */
			if ( map_message[iterator++] != TAG_GSM_SMS_PDU)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}   
			gsm_pdu_len = map_message[iterator++];
			iterator++;
			msisdn_len = map_message[iterator++];
			iterator++;	
			memset(msisdn, 0, IMSI_LEN);
			/*Extracting MSISDN of sender */
			
			for (counter=0; counter < ((msisdn_len + 1)/2); counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time (msisdn, 
				((map_params *)params)->MSISDN_sender, ((msisdn_len+1)/2));	
			
			/*Extracting SMS data */
			iterator++; //PID
			DCS = map_message[iterator++]; //DCS
			iterator = iterator + 8;	//Bypassing Service centre timestamp
		
			sms_len = map_message[iterator++];
			memcpy (sms_buffer, &map_message[iterator], sms_len);
			if ( DCS == GSM_7_CODING)
			{
				gsm7_to_ascii (sms_buffer, ((map_params *)params)->sms_data);
			}
			else
			{	
				memcpy (((map_params *)params)->sms_data, sms_buffer, sms_len);
			}
		}
		else if ( map_message[iterator] == MO_FORWARD_SM)
		{
			iterator += 1;
			((map_params *)params)->map_opcode = MT_FORWARD_SM;
			
			if (map_message[iterator++] != TAG_CONSTRUCTOR)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}  	 
			tag_constructor_len = map_message[iterator++];
		
			/*Extracting MSISDN  of SMSC */
			if ( map_message[iterator++] != TAG_SVC_CENTR_ADDR)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}  
			msisdn_len = map_message[iterator++];
			iterator++;
			memset(msisdn, 0, IMSI_LEN);
			for(counter=0; counter < msisdn_len -1; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time(msisdn, ((map_params *)params)->MSISDN_SMSC, msisdn_len - 1);	
		
			/*Extracting MSISDN Number of sender */
			if ( map_message[iterator++] != TAG_MSISDN)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}  	 
			msisdn_len = map_message[iterator++];
			iterator++;
			memset(msisdn, 0, IMSI_LEN);
			for(counter=0; counter < msisdn_len -1; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time(msisdn, ((map_params *)params)->MSISDN_Sender, msisdn_len - 1);	
		
			/* Parsing GSM PDU */
			if ( map_message[iterator++] != TAG_GSM_SMS_PDU)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}	   
			gsm_pdu_len = map_message[iterator++];
			iterator++;
			iterator++;
			msisdn_len = map_message[iterator++];
			iterator++;	
			memset(msisdn, 0, IMSI_LEN);
			
			/*	Extracting MSISDN of receiver */
			for(counter=0; counter < ((msisdn_len + 1)/2); counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time(msisdn, ((map_params *)params)->MSISDN_receiver, ((msisdn_len + 1)/2));	
			
			/*	Extracting SMS data */
			iterator++; //PID
			DCS = map_message[iterator++]; //DCS
			sms_len = map_message[iterator++];
			memcpy (sms_buffer, &map_message[iterator], sms_len);
			if ( DCS == GSM_7_CODING)
			{
				gsm7_to_ascii (sms_buffer, ((map_params *)params)->sms_data);
			}
			else
			{	
				memcpy (((map_params *)params)->sms_data, sms_buffer, sms_len);
			}
		}
	} 
	else if ( map_message[iterator -1 ] ==  TAG_DLG_RESULT )
	{
		map_msg_len = map_message[iterator++];
		
		if ( map_message[iterator++] != TAG_INVOKE_ID)
		{    	
			((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
			return 0;
		}   
		invoke_id_len = map_message[iterator++];
		iterator += invoke_id_len;
    
		if ( map_message[iterator++] != TAG_CONSTRUCTOR)
		{   
			((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
			return 0;
		}   
		tag_constructor_len =  map_message[iterator++];
        
		if ( map_message[iterator++] != TAG_OPCODE)
		{   
			((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;	
			return 0;
		}  
		opcode_len =  map_message[iterator++];
    
		if ( map_message[iterator] == TAG_SEND_ROUT_INFO)
		{   
			iterator += 1;
			((map_params *)params)->map_opcode = TAG_SEND_ROUT_INFO;
    		
			if ( map_message[iterator++] != TAG_CONSTRUCTOR)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}    
			tag_constructor_len = map_message[iterator++];
			
			if ( map_message[iterator++] != TAG_GSM_SMS_PDU)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}    
			msisdn_len = map_message[iterator++];
			memset(msisdn, 0, IMSI_LEN);
			
			/*	Extracting IMSI returned from HLR */
			for(counter=0; counter < msisdn_len; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time(msisdn, ((map_params *)params)->IMSI_from_HLR, msisdn_len);	
		
			if (strlen (((map_params *)params)->IMSI_from_HLR) == IMSI_LEN)
				((map_params *)params)->IMSI_from_HLR[IMSI_LEN - 1] = '\0';
	
			if ( map_message[iterator++] != TAG_ASN_1_TYPE)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}    
			tag_constructor_len = map_message[iterator++];
			
			if ( map_message[iterator++] != TAG_PARAMETER_2)
			{   
				((map_params *)params)->map_error_code = ERROR_MAP_TAG_NOT_PROPER;
				return 0;
			}    
			msisdn_len = map_message[iterator++];
			iterator++;//Number type indicator
			memset(msisdn, 0, IMSI_LEN);

			/*	Extracting LMSI returned from HLR */
			for(counter=0; counter < msisdn_len -1; counter++)
			{
				msisdn[counter] = map_message[iterator];
				iterator++;
			}
			decode_number_or_time(msisdn, ((map_params *)params)->LMSI_from_HLR, msisdn_len - 1);	
		}	
	}
}	
/* end of fi e map.c */
