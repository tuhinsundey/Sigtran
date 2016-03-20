 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines TCAP packet formation and  
 *            has definitions for:
 *           - Ascii to gsm7 conversion
 *           - gsm7 to ascii conversion
 *           - Encoding
 *           - Decoding 
 *	     - Parsing TCAP packet. 
 */

#include "tcap.h"

static char object_id_for_sms[7] = 
		{0x00, 0x11, 0x86, 0x05, 0x01, 0x01, 0x01};

static char appl_conxt_for_sms[7] = 
		{0x04, 0x00, 0x00, 0x01, 0x00, 0x19, 0x02};
		
static char appl_conxt_sri_res[7] = 
		{0x04, 0x00, 0x00, 0x01, 0x00, 0x14, 0x02};

/*************************************************************************************
  * Function : insert_transaction_id()
  * Output   : integer offset, normally 4
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
inline static int insert_transaction_id (unsigned char *tcap_buff, 
			int trx_id)
{
	int temp_trx_id;
	int iterator = 0;

	temp_trx_id = (trx_id >> R_SHIFT_24);
	memcpy ((void *)&tcap_buff[iterator++], &temp_trx_id, 1);
	temp_trx_id = (trx_id >> R_SHIFT_16);
	memcpy ((void *)&tcap_buff [iterator++],&temp_trx_id, 1);
	temp_trx_id = (trx_id >> R_SHIFT_8);
	memcpy ((void *)&tcap_buff [iterator++], &temp_trx_id, 1);
	memcpy ((void *)&tcap_buff [iterator++], &trx_id, 1);

	return iterator;
}

/*************************************************************************************
  * Function : tcap_frame()
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
int tcap_frame (unsigned char *message, void *param, int type)
{
	/*
	 * message format:
	 * 
	 * type: begin, end, continue
	 *
	 * --------------------------
	 * transaction ID
	 * --------------------------
	 * dialogue request
	 * --------------------------
	 * components (payload)
	 * --------------------------
	 */

	int tcap_msg_len	= 0;
	int gsm_map_msglen	= 0;
	int dlg_start_offset	= 0;
	int dlg_req_offset	= 0;
	int dlg_as_id_offset	= 0;
	int tag_field0_offset	= 0;
	int asn_type_offset	= 0;
	int tag_field1_offset	= 0;
	int app_cntxt_offset	= 0;
	int iterator		= 0; /*incremented after every byte*/
	int tcap_primitive	= 0;
	int trx_id = ((sms_fields *)(param))->trx_id;
	char *context_type = NULL;

	switch (type)
	{
		case MAP_MO_FORWARD_SM_REQ:
			tcap_primitive = TAG_GSM_TCAP_BEGIN;
			context_type = appl_conxt_for_sms;
			break;
		case MAP_MT_FORWARD_SM_REQ:
			tcap_primitive = TAG_GSM_TCAP_BEGIN;
			context_type = appl_conxt_for_sms;
			break;
		case MAP_SRI_SM_REQ:
			tcap_primitive = TAG_GSM_TCAP_BEGIN;
			context_type = appl_conxt_sri_res;
			break;
		case ALERT_SVC_CENTER:
			tcap_primitive = TAG_GSM_TCAP_BEGIN;
			break;
		case MAP_MO_FORWARD_SM_RES:
			tcap_primitive = TAG_GSM_TCAP_END;
			context_type = appl_conxt_for_sms;
			break;
		case MAP_MT_FORWARD_SM_RES:
			tcap_primitive = TAG_GSM_TCAP_END;
			context_type = appl_conxt_for_sms;
			break;
		case MAP_SRI_SM_RES:
			tcap_primitive = TAG_GSM_TCAP_END;
			context_type = appl_conxt_sri_res;
			break;
		case ALERT_SVC_CENTER_RES:
			tcap_primitive = TAG_GSM_TCAP_END;
			break;
		default:
			break;
	}

	/* type */
	message[iterator++]	= tcap_primitive; 
	tcap_msg_len = iterator;
	/* message[1] should be 0x81 in case of a large message */
	iterator++; 

	/********* Transaction ID *********/
	if (tcap_primitive == TAG_GSM_TCAP_BEGIN)
		message[iterator++]	= TAG_GSM_TCAP_TX_ID;
	if (tcap_primitive == TAG_GSM_TCAP_END)
		message[iterator++]	= TAG_GSM_TCAP_DEST_TX_ID;
			
	message[iterator++]	= TRX_ID_LEN;
	iterator += insert_transaction_id (&message[iterator], trx_id);	

	message[iterator++]	= TAG_STRT_OF_DLG_PRTN; //6b
	dlg_start_offset	= iterator;
	message[iterator++]	= dlg_start_offset; 
	/* 
	 * the value of offset has to be adjusted at 
	 * the end of dialogue portion
	 */
		
	message[iterator++]	= TAG_STRT_OF_DLG_REQ; //28
	dlg_req_offset		= iterator;	
	message[iterator++]	= dlg_req_offset;
	/* 
	 * the value of offset has to be adjusted at 
	 * the end of dialogue portion
	 */

	message[iterator++]	= TAG_DLG_AS_ID; //06
	//dlg_as_id_offset	= iterator;
	//message[iterator++]	= dlg_as_id_offset;
	message[iterator++]	= OBJECT_ID_LEN;
	memcpy (&message[iterator], &object_id_for_sms, OBJECT_ID_LEN);
	iterator += OBJECT_ID_LEN;
	/* 
	 * the value of offset has to be adjusted at 
	 * the end of dialogue portion
	 */
	
	/******** Dialogue Request *********/		
	message[iterator++]	= TAG_TAG_FIELD0; //0xa0
	tag_field0_offset	= iterator;
	message[iterator++]	= tag_field0_offset;

	message[iterator++]	= TAG_SINGLE_ASN1_TYPE; //0x60
	asn_type_offset		= iterator;
	message[iterator++]	= asn_type_offset;

	message[iterator++]	= TAG_TAG_FIELD1; //0xa1
	tag_field1_offset	= iterator;
	message[iterator++]	= tag_field1_offset;

	message[iterator++]	= TAG_APPLN_CNTXT_NAME; //0x06
	//app_cntxt_offset	= iterator;
	//message[iterator++]	= app_cntxt_offset;
	message[iterator++]	= OBJECT_ID_LEN;
	memcpy (&message[iterator], context_type, OBJECT_ID_LEN);
	iterator += OBJECT_ID_LEN;
	
	/** Now, adjust the offsets **/
	message[dlg_start_offset]	= iterator - dlg_start_offset -1;
	message[dlg_req_offset]		= iterator - dlg_req_offset -1 ;
	//message[dlg_as_id_offset]	= iterator - dlg_as_id_offset;
	message[tag_field0_offset]	= iterator - tag_field0_offset -1;
	message[asn_type_offset]	= iterator - asn_type_offset -1;
	message[tag_field1_offset]	= iterator - tag_field1_offset -1;
	//message[app_cntxt_offset]	= iterator - app_cntxt_offset;

	/******** Component Portion (the payload)*********/		
	message[iterator++]	= TAG_COMP_PORTN;

	gsm_map_msglen		= gsm_map_frame (&message[iterator + 1], param, type);

	message[iterator]	= gsm_map_msglen;
	iterator += gsm_map_msglen + 1;

	message[tcap_msg_len] = iterator - 2;

	return iterator;
}

/*************************************************************************************
  * Function : parse_message()
  * Input    : tcap_message, params
  * Output   : None
  * Purpose  : Parse the received packet
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
int parse_tcap_message (unsigned char *tcap_message , void *params)
{
	int iterator            			= 0;
	unsigned char transaction_id[TRX_ID_LEN]	= {0};
	unsigned char appl_context_id[OBJECT_ID_LEN]	= {0};
	unsigned char temp_buffer[OBJECT_ID_LEN]	= {0};
	int counter						= 0;
	int index						= 0;
	int len							= 0;
	int tcap_msg_len 				= 0;
	int dlg_start_len				= 0;	
	int dlg_req_len					= 0;	
	int dlg_as_id_len				= 0;	
	int tag_field0_len				= 0;	
	int asn_type_len				= 0;	
	int tag_field1_len				= 0;	
	int app_cntxt_len				= 0;	
	int comp_portn_len				= 0;	
   	int comp_portion_itr			= 0;
 
	switch (tcap_message[iterator++])
	{
		case TAG_GSM_TCAP_BEGIN:
			((tcap_params *)params)->tcap_primitive = TAG_GSM_TCAP_BEGIN;
			break;
		case TAG_GSM_TCAP_END:
			((tcap_params *)params)->tcap_primitive = TAG_GSM_TCAP_END;
			break;
		case TAG_GSM_TCAP_CONTINUE:
			((tcap_params *)params)->tcap_primitive = TAG_GSM_TCAP_CONTINUE;
			break;
		default:	
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_PRIMITIVE_NOT_PROPER;
			printf ("Tcap message structure is not proper");
			return 0;
	}
	tcap_msg_len = tcap_message[iterator++];
	if ( ((tcap_params *)params)->tcap_primitive	== TAG_GSM_TCAP_BEGIN )	
	{		
		if ( tcap_message[iterator++] != TAG_GSM_TCAP_TX_ID)
		{	
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
			return 0;
		}
		else /*Extracting Transaction ID */
		{	
			len = tcap_message[iterator++];
			memcpy ( temp_buffer , &tcap_message[iterator], len);	
			index = len - 1;
			for(counter = 0 ; counter < len ; counter++)
			{
				transaction_id[counter] = temp_buffer[index];
				index--;
			}
			((tcap_params *)params)->transaction_id = *(unsigned int*) transaction_id;
			iterator += len;
		}
	}
	if (((tcap_params *)params)->tcap_primitive	== TAG_GSM_TCAP_END )	
	{		
		if (tcap_message[iterator++] != TAG_GSM_TCAP_DEST_TX_ID)
		{	
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
			return 0;
		}
		else /*Extracting Transaction ID */
		{	
			len = tcap_message[iterator++];
			memcpy ( temp_buffer , &tcap_message[iterator], len);	
			index = len - 1;
			for(counter = 0 ; counter < len ; counter++)
			{
				transaction_id[counter] = temp_buffer[index];
				index--;
			}
			((tcap_params *)params)->transaction_id = *(unsigned int*) transaction_id;
			iterator += len;
		}
	}
	if (tcap_message[iterator++] != TAG_STRT_OF_DLG_PRTN)
	{	
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
		return 0;
	}
	dlg_start_len = tcap_message[iterator++];
	
	if (tcap_message[iterator++] != TAG_STRT_OF_DLG_REQ)
	{	
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
		return 0;
	}
	dlg_req_len = tcap_message[iterator++];

	if ((dlg_start_len - dlg_req_len) != 2)
	{
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_LEN_NOT_PROPER;
		return 0;
	}
	
	if ( tcap_message[iterator++] != TAG_DLG_AS_ID)
	{	
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
		return 0;
	}
	
	/* Extracting dialogue Object ID */
	dlg_as_id_len = tcap_message[iterator++];
	memcpy (((tcap_params *)params)->object_id, &tcap_message[iterator], dlg_as_id_len);
	iterator += dlg_as_id_len;

		
	if (tcap_message[iterator++] != TAG_TAG_FIELD0)
	{	
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
		return 0;
	}
	tag_field0_len = tcap_message[iterator++];

	if (((tcap_params *)params)->tcap_primitive	== TAG_GSM_TCAP_BEGIN )
	{	
		if (tcap_message[iterator++] != TAG_SINGLE_ASN1_TYPE)
		{	
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
			return 0;
		}
		asn_type_len = tcap_message[iterator++];
	}
	if (((tcap_params *)params)->tcap_primitive	== TAG_GSM_TCAP_END )
	{	
		if (tcap_message[iterator++] != TAG_SINGLE_ASN2_TYPE)
		{	
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
			return 0;
		}
		asn_type_len = tcap_message[iterator++];
		comp_portion_itr = iterator + asn_type_len;
	}
	
	if (tcap_message[iterator++] != TAG_TAG_FIELD1)
	{	
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
		return 0;
	}
	tag_field1_len = tcap_message[iterator++];
	
	if (tcap_message[iterator++] != TAG_APPLN_CNTXT_NAME)
	{	
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
		return 0;
	}
	app_cntxt_len = tcap_message[iterator++];

	if ((tag_field1_len - app_cntxt_len) != 2)
	{
		((tcap_params *)params)->tcap_error_code = ERROR_TCAP_LEN_NOT_PROPER;
		return 0;
	}

	/* Extracting Application context ID */	
	memcpy(((tcap_params *)params)->appl_context, &tcap_message[iterator], app_cntxt_len);
	iterator += app_cntxt_len;
		
	if (((tcap_params *)params)->tcap_primitive  == TAG_GSM_TCAP_BEGIN )
	{	
		if ( tcap_message[iterator++] != TAG_COMP_PORTN)
		{	
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
			return 0;
		}
		comp_portn_len = tcap_message[iterator++];
	
		if ((comp_portn_len - tcap_message[iterator + 1]) != 2)
		{
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_LEN_NOT_PROPER;
			return 0;
		}
		int ret = parse_map_message ( &tcap_message[iterator], 
			((tcap_params *)params)->map_params);
		if (ret)
			return 1;
		else 
			return 0;
	}
	
	if (((tcap_params *)params)->tcap_primitive == TAG_GSM_TCAP_END )
	{	
		iterator = comp_portion_itr;	
		if (tcap_message[iterator++] != TAG_COMP_PORTN)
		{	
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_TAG_NOT_PROPER;
			return 0;
		}
		comp_portn_len = tcap_message[iterator++];
	
		if ((comp_portn_len - tcap_message[iterator + 1]) != 2)
		{
			((tcap_params *)params)->tcap_error_code = ERROR_TCAP_LEN_NOT_PROPER;
			return 0;
		}
		int ret = parse_map_message (&tcap_message[iterator], 
				((tcap_params *)params)->map_params);
		if (ret)
			return 1;
		else 
			return 0;
	}
}
