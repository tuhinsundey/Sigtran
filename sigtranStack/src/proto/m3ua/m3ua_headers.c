 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines message classes
		 and message type definations 
 *	      and has definitions for:
 *           - m3ua MGMT msg 
 *           - m3ua ssnm msg 
 *           - m3ua trx asps 
 *           - m3ua aspt 
 *           - create m3ua msg
 */

#include "m3ua_headers.h"
#include "m3ua_nodes.h"

/**** Network Identifier Tag ****/
static const char network_identifier_tag[2]	= {0x02, 0x00};

/**** Protocol data Tag ****/
static const char protocol_data_tag[2]		= {0x02, 0x10};

/**** Point code tag ****/
static const char affected_point_code_tag[2]	= {0x00, 0x12};

/**** Routing Context Tag and primitives ****/
static const char routing_context_tag[2]	= {0x00, 0x06};
static const char routing_context_val[4]	= {0x00, 0x00, 0x00, 0x01};

/**** Traffic Mode Type tag ****/
static const char traffic_mode_type_tag[2]	= {0x00, 0x0b};

/**** Heart Beat tag ****/
static const char heart_beat_tag[2]		= {0x00, 0x09};

/**** Error code tag ****/
static const char error_code_tag[2]		= {0x00, 0x0c};

static int heart_beat_sequence_no		= 1;

#define ERROR_CODE_TAG_LEN		2
#define ERROR_CODE_PARAM_LEN		0x08

/*************************************************************************************
* Function : m3ua_err_msg()
* Input    : m3ua_message, as_node
* Output   : 
* Purpose  : To create management error messages
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static int m3ua_err_msg(unsigned char *m3ua_message, m3ua_as *as_node)
{
	int msg_len	= 0;
	
	memcpy(&m3ua_message[msg_len++], error_code_tag, ERROR_CODE_TAG_LEN);
	msg_len += 2;
	m3ua_message[msg_len++] = ERROR_CODE_PARAM_LEN;	
	m3ua_message += 3;
	m3ua_message[msg_len++] = as_node->mgmt_error;
	msg_len++;		
	
		
	return msg_len;
}

/** Management messages **/
/*************************************************************************************
* Function : m3ua_MGMT_msg()
* Input    : m3ua_message, message_type
* Output   : 
* Purpose  : To create management messages
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static int m3ua_MGMT_msg (unsigned char *m3ua_message, m3ua_as *as_node)
{
	int m3ua_length = 0;
	m3ua_message[m3ua_length++]		= M3UA_VERSION_1;
	m3ua_message[m3ua_length++]		= M3UA_RESERVED_BYTE;
	m3ua_message[m3ua_length++]		= as_node->message_class;
	
	switch (as_node->message_type)
	{
		case M3UA_MSG_TYPE_MGMT_ERR:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_MGMT_ERR;
			m3ua_length += m3ua_err_msg(&m3ua_message[m3ua_length], as_node);
			break;
	     	case M3UA_MSG_TYPE_MGMT_NTFY:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_MGMT_NTFY;
			break;
	}
	memset (&m3ua_message[m3ua_length++], 0, reserved_bytes3);

	/*** Routing Context ***/	
	memcpy (&m3ua_message[m3ua_length++], routing_context_tag, 
			ROUTING_CTXT_LEN);
	//m3ua_length += small_param_len;
	//memcpy (&m3ua_message[m3ua_length++], small_param_tag, small_param_len);
	//memcpy (&m3ua_message[m3ua_length++], routing_context, ROUTING_CTXT_LEN);

	/*** Message Length ***/
	m3ua_message[m3ua_length]	= m3ua_length;
	return m3ua_length;
}

/** SS7 Signaling Network management **/
/*************************************************************************************
* Function : m3ua_SSNM_msg()
* Input    : m3ua_message,message_type
* Output   : 
* Purpose  : To create SS7 network management packet
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static int m3ua_SSNM_msg (unsigned char *m3ua_message, m3ua_as *as_node)
{
	int m3ua_length = 0;
	int offset_msg_len = 0;
	int routing_context_header_len = 4;
	int affected_pc_header_len = 8;
	m3ua_message[m3ua_length++]		= M3UA_VERSION_1;
	m3ua_message[m3ua_length++]		= M3UA_RESERVED_BYTE;
	m3ua_message[m3ua_length++]		= as_node->message_class;
	
	switch (as_node->message_type)
	{
		case M3UA_MSG_TYPE_SSNM_DUNA:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_SSNM_DUNA;
			break;
		case M3UA_MSG_TYPE_SSNM_DAVA:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_SSNM_DAVA;
			break;
		case M3UA_MSG_TYPE_SSNM_DAUD:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_SSNM_DAUD;
			break;
		case M3UA_MSG_TYPE_SSNM_SCON:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_SSNM_SCON;
			break;
		case M3UA_MSG_TYPE_SSNM_DUPU:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_SSNM_DUPU;
			break;
		case M3UA_MSG_TYPE_SSNM_DRST:
			m3ua_message[m3ua_length++]	= M3UA_MSG_TYPE_SSNM_DRST;
			break;
	}
	memset (&m3ua_message[m3ua_length], 0, reserved_bytes3);
	m3ua_length += reserved_bytes3;

	offset_msg_len	= m3ua_length++;

	/*** Routing Context ***/
	memcpy (&m3ua_message[m3ua_length++], routing_context_tag, ROUTING_CTXT_TAG_LEN);
	m3ua_length += 2; 

	routing_context_header_len = as_node->routing_context_len + NORMAL_TAG_LEN;

	m3ua_message[m3ua_length++] = routing_context_header_len;

	memcpy (&m3ua_message[m3ua_length], as_node->routing_context, 
			as_node->routing_context_len);
	m3ua_length += as_node->routing_context_len;
	
	/*** Affected Point codes ***/
	memcpy (&m3ua_message[m3ua_length++], affected_point_code_tag, ROUTING_CTXT_TAG_LEN);
	m3ua_length += 2; 
	m3ua_message[m3ua_length++] = affected_pc_header_len;
	
	memcpy (&m3ua_message[m3ua_length], &(as_node->destn->point_code), 4);
	
	m3ua_length = m3ua_length + (affected_pc_header_len - 4);
		
	/*** Message Length ***/
	m3ua_message[offset_msg_len]	= m3ua_length;
	return m3ua_length;
}

/** M3UA transfer messages **/
/*************************************************************************************
* Function : m3ua_TRX_msg()
* Input    : m3ua_message,message_type
* Output   : 
* Purpose  : To create transfer packets
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static int m3ua_TRX_msg (unsigned char *m3ua_message, m3ua_as *as_node)
{
	int msg_len = 0;
	int m3ua_payload_len = 0;
	int protocol_data_len = 0;
	m3ua_message[msg_len++]		= M3UA_VERSION_1;
	m3ua_message[msg_len++]		= M3UA_RESERVED_BYTE;
	m3ua_message[msg_len++]		= as_node->message_class;
	m3ua_message[msg_len++] 	= M3UA_MSG_TYPE_TX_DATA;
	msg_len += reserved_bytes3;
	m3ua_payload_len = msg_len;
	msg_len++;

	/*** Routing Context ***/	
	memcpy (&m3ua_message[msg_len], routing_context_tag, ROUTING_CTXT_TAG_LEN);
	msg_len += ROUTING_CTXT_TAG_LEN;
	msg_len++;

	m3ua_message[msg_len++] = as_node->routing_context_len + NORMAL_TAG_LEN;
	memcpy (&m3ua_message[msg_len], as_node->routing_context, 
		as_node->routing_context_len);
	msg_len += as_node->routing_context_len;

	/*** Protocol data begins ***/
	memcpy (&m3ua_message[msg_len], protocol_data_tag, PROTOCOL_DATA_TAG_LEN);
	msg_len += PROTOCOL_DATA_TAG_LEN;
	msg_len++;
	
	protocol_data_len = msg_len;
	msg_len++;

	/* put Origin pointcode here*/
	memcpy (&m3ua_message[msg_len], as_node->origin->point_code, 
		POINT_CODE_LEN);
	msg_len += POINT_CODE_LEN;

	/* put Destination pointcode here*/
	memcpy (&m3ua_message[msg_len], as_node->destn->point_code, 
		POINT_CODE_LEN);
	msg_len += POINT_CODE_LEN;

	m3ua_message[msg_len++] = TAG_SCCP;
	m3ua_message[msg_len++] = TAG_NI;
	m3ua_message[msg_len++] = TAG_MP;
	m3ua_message[msg_len++] = TAG_SLS;

	/*** Copy the next protocol payload (sccp) ***/
	if (as_node->msg_ptr)
		memcpy (&m3ua_message[msg_len], as_node->msg_ptr, as_node->msg_len);	
	as_node->msg_len += msg_len - 2;
	m3ua_message[protocol_data_len] = as_node->msg_len - protocol_data_len + 
		PROTOCOL_DATA_TAG_LEN + 4;
	m3ua_message[m3ua_payload_len] = as_node->msg_len - m3ua_payload_len + 10;

	/*** Message Length and message pointer***/
	as_node->msg_ptr = m3ua_message;
	as_node->msg_len += 3;
	return as_node->msg_len;
}

/* Routing Key management messages */
static int m3ua_RKM_msg (unsigned char *m3ua_message, m3ua_as *as_node)
{
	return 0;
}

/*************************************************************************************
* Function : m3ua_ASPS_msg()
* Input    : m3ua_message,message_type
* Output   : 
* Purpose  : To create ASPSM packet
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static int m3ua_ASPS_msg (unsigned char *m3ua_message, m3ua_as *as_node)
{
	int msg_len = 0;
	m3ua_message[msg_len++] = M3UA_VERSION_1;
	m3ua_message[msg_len++] = M3UA_RESERVED_BYTE;
	m3ua_message[msg_len++] = as_node->message_class;
	int msg_len_offset	= 0;
	int heart_beat_indicator= 0;
	int heart_beat_param_len= 8;

	switch (as_node->message_type)
	{
		case M3UA_MSG_TYPE_ASP_UP:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_UP;
			break;
		case M3UA_MSG_TYPE_ASP_DOWN:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_DOWN;
			break;
		case M3UA_MSG_TYPE_ASP_HTBT:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_HTBT;
			heart_beat_indicator	= M3UA_MSG_TYPE_ASP_HTBT;
			break;
		case M3UA_MSG_TYPE_ASP_UPACK:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_UPACK;
			break;
		case M3UA_MSG_TYPE_ASP_DWN_ACK:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_DWN_ACK;
			break;
		case M3UA_MSG_TYPE_ASP_HBT_ACK:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_HBT_ACK;
			heart_beat_indicator	= M3UA_MSG_TYPE_ASP_HBT_ACK;
			break;
	}
	
	memset (&m3ua_message[msg_len], 0, reserved_bytes3);
	msg_len += reserved_bytes3;
	msg_len_offset = msg_len++;

	if(heart_beat_indicator)
	{
		if(heart_beat_indicator == M3UA_MSG_TYPE_ASP_HTBT)
		{
			memcpy(&m3ua_message[msg_len], heart_beat_tag, HEART_BEAT_TAG_LEN);
			msg_len += 3;
			m3ua_message[msg_len++] = heart_beat_param_len;
			msg_len += 3;
			if(heart_beat_sequence_no > MAX_HEART_BEAT_SEQ)
				heart_beat_sequence_no = 1;
			m3ua_message[msg_len++] = heart_beat_sequence_no++;
		}
		else 
		{
		//	m3ua_message[msg_len++] = as_node->HTBT_data_len + 4;
		//	memcpy(&m3ua_message[msg_len], as_node->HTBT_data, as_node->HTBT_data_len);
		//	msg_len += as_node->HTBT_data_len;
		}
	}
	m3ua_message[msg_len_offset]	= msg_len;
	return msg_len;
}

/*************************************************************************************
* Function : m3ua_ASPT_msg()
* Input    : 
* Output   : 
* Purpose  : To create ASPTM packet 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static int m3ua_ASPT_msg (unsigned char *m3ua_message, m3ua_as *as_node)
{
	int msg_len = 0;
	int offset_msg_len 	= 0;
	int routing_context_header_len	= 4;
	int traffic_mode_type_header_len= 8;
	int traffic_mode_indicated 	= 0;	

	m3ua_message[msg_len++]	= M3UA_VERSION_1;
	m3ua_message[msg_len++]	= M3UA_RESERVED_BYTE;
	m3ua_message[msg_len++]	= as_node->message_class;
	
	switch (as_node->message_type)
	{
		case M3UA_MSG_TYPE_ASP_ACT:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_ACT;
			traffic_mode_indicated 	= 1;
			break;
		case M3UA_MSG_TYPE_ASP_INA:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_INA;
			break;
		case M3UA_MSG_TYPE_ASP_ACT_ACK:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_ACT_ACK;
			traffic_mode_indicated 	= 1;
			break;
		case M3UA_MSG_TYPE_ASP_INA_ACK:
			m3ua_message[msg_len++] = M3UA_MSG_TYPE_ASP_INA_ACK;
			break;
	}
	memset (&m3ua_message[msg_len], 0, reserved_bytes3);
	msg_len += reserved_bytes3;
	offset_msg_len	= msg_len++;	
	
	if(traffic_mode_indicated)
	{
		/*** Traffic Mode Type ***/	
		memcpy (&m3ua_message[msg_len++], traffic_mode_type_tag, TRAFFIC_MODE_TYPE_TAG_LEN);
		msg_len += 2; 
	
		m3ua_message[msg_len++]	= traffic_mode_type_header_len;	
		msg_len += 3;
	
		m3ua_message[msg_len++]	= OVERRIDE_TRAFFIC_MODE;	
	}
	/*** Routing Context ***/	
	memcpy (&m3ua_message[msg_len++], routing_context_tag, ROUTING_CTXT_TAG_LEN);
	msg_len += 2; 
	
	routing_context_header_len = as_node->routing_context_len + NORMAL_TAG_LEN;
	
	m3ua_message[msg_len++] = routing_context_header_len;

	memcpy (&m3ua_message[msg_len], as_node->routing_context, 
			as_node->routing_context_len);
	msg_len += as_node->routing_context_len;
	
	m3ua_message[offset_msg_len]	= msg_len;
	
	return msg_len;
}


/*************************************************************************************
* Function : create_m3ua_msg ()
* Input    : 
* Output   : 
* Purpose  : Generic m3ua message create API
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_msg (m3ua_as *as_node, unsigned char *m3ua_message)
{
	switch (as_node->message_class)
	{
		case M3UA_MSG_CLASS_ASPTM:
			return m3ua_ASPT_msg (m3ua_message, as_node);
		case M3UA_MSG_CLASS_ASPSM:
			return m3ua_ASPS_msg (m3ua_message, as_node);
		case M3UA_MSG_CLASS_TX_MSG:	
			return m3ua_TRX_msg (m3ua_message, as_node);
		case M3UA_MSG_CLASS_RKM:	
			return m3ua_RKM_msg (m3ua_message, as_node);
		case M3UA_MSG_CLASS_MGMT:
			return m3ua_MGMT_msg (m3ua_message, as_node);
		case M3UA_MSG_CLASS_SSNM:	
			return m3ua_SSNM_msg (m3ua_message, as_node);
		default:
			return 0;
	}
	return 0;
}

