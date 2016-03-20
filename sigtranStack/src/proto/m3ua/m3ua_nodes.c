 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines m3ua layer
 *	      and has definitions for:
 *           - resolve m3ua as 
 *           - m3ua connection manager 
 *           - m3ua config 
 *           - m3ua connect 
 *           - m3ua as config
 *           - m3ua send 
 *           - m3ua recv 
 *           - m3ua aspsm hdlr 
 *           - m3ua recv data 
 */

#include "m3ua_nodes.h"
#include "m3ua_commons.h"
#include "socket_layer.h"
#include "m3ua_handshake.h"

/**** Network Identifier Tag ****/
static const char network_identifier_tag[2] = {0x02, 0x00};

/**** Protocol data Tag ****/
static const char protocol_data_tag[2] = {0x02, 0x10};

/**** Point code tag ****/
static const char affected_point_code_tag[2]    = {0x00, 0x12};

/**** Routing Context Tag and primitives ****/
static const char routing_context_tag[2]    = {0x00, 0x06};
static const char routing_context_val[4]    = {0x00, 0x00, 0x00, 0x01};



/*************************************************************************************
* Function : resolve_m3ua_as()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
static m3ua_as *resolve_m3ua_as ()
{
	return NULL;	
}

/*************************************************************************************
* Function : m3ua_connection_manager()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_connection_manager ()
{
	//m3ua_recv ();
	//resolve_m3ua_as ();
	//
	//route_to_as ();
	;
}

/*************************************************************************************
* Function : m3ua_config()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
static void m3ua_config (m3ua_asp *asp_node, char *config)
{
	char *get_conf = NULL;
	dictionary *ini = NULL;

	ini = iniparser_load (config);

	if (asp_node)
	{
		asp_node->host.sin_family = AF_INET;
		get_conf = iniparser_getstring (ini, "host:ip", LOCAL_HOST);	
		asp_node->host.sin_addr.s_addr = inet_addr (get_conf);
		asp_node->host.sin_port = 
			htons ((iniparser_getint (ini, "host:port", M3UA_PORT_NUM)));

		asp_node->peer.sin_family = AF_INET;
		get_conf = iniparser_getstring (ini, "peer:ip", LOCAL_HOST);	
		asp_node->peer.sin_addr.s_addr = inet_addr (get_conf);
		asp_node->peer.sin_port = 
			htons ((iniparser_getint (ini, "peer:port", M3UA_PORT_NUM)));
	}
}

/*************************************************************************************
* Function : m3ua_connect()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
static void m3ua_connect (m3ua_asp *asp_node, char *config)
{
	m3ua_config (asp_node, config);
	if (create_socket (asp_node))
		printf ("successfully created socket with peer\n");
	else
		printf ("error creating socket with peer\n");
}

/*************************************************************************************
* Function : m3ua_init()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_init (m3ua_asp *asp_node, char *conf)
{
	m3ua_connect (asp_node, conf);
}

/*************************************************************************************
* Function : m3ua_config()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_as_config (m3ua_as *as_node, char *config)
{
	char *get_conf = NULL;
	dictionary *ini = NULL;
	unsigned char pc_buff[POINT_CODE_LEN] = {0};

	ini = iniparser_load (config);

	if (as_node)
	{
		get_conf = iniparser_getstring (ini, "as:type", "asp");

		/*
		 * Determine the type of 
		 * peer 
		 */
		if (strcasestr (get_conf, "sgw"))
			as_node->peer_type = SGW;
		else 
			as_node->peer_type = ASP;
		
		/*
		 * Fill origin
		 * GT attributes 
		 */	
		global_title_type *orig_gt = (global_title_type *) malloc 
				(sizeof (global_title_type));
		if (!orig_gt)
			printf ("malloc error...");
		memset (orig_gt, 0, sizeof (global_title_type));
		orig_gt->pc = 
			iniparser_getint (ini, "as:orig_pc", DEFAULT_ORIG_PC);
		orig_gt->ssn = 
			iniparser_getint (ini, "as:orig_ssn", SUBSYS_MSC);
		get_conf = iniparser_getstring (ini, "as:orig_gt", DEFAULT_GT);
		strcpy (orig_gt->global_title, get_conf);
		convert_int_to_hex (orig_gt->pc, orig_gt->point_code);
		//orig_gt->next_pc = NULL;
		as_node->origin = orig_gt;

		/*
		 * Fill destination
		 * GT attributes
		 */
		global_title_type *dest_gt = (global_title_type *) malloc 
				(sizeof (global_title_type));
		if (!dest_gt)
			printf ("malloc error...");
		memset (dest_gt, 0, sizeof (global_title_type));
		dest_gt->pc = 
			iniparser_getint (ini, "as:dest_pc", DEFAULT_ORIG_PC);
		dest_gt->ssn = 
			iniparser_getint (ini, "as:dest_ssn", SUBSYS_MSC);
		get_conf = iniparser_getstring (ini, "as:dest_gt", DEFAULT_GT);
		strcpy (dest_gt->global_title, get_conf);
		memset (pc_buff, 0, POINT_CODE_LEN);
		convert_int_to_hex (dest_gt->pc, dest_gt->point_code);
		//dest_gt->next_pc = NULL;
		as_node->destn = dest_gt;
	}
}

/*************************************************************************************
* Function : m3ua_send()
* Input    : 
* Output   : 
* Purpose  : Send m3ua message over SCTP
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int m3ua_send (m3ua_as *as_node)
{
	int ret;

	switch (as_node->message_class)
	{
		case M3UA_MSG_CLASS_ASPSM:
		case M3UA_MSG_CLASS_RKM:
		case M3UA_MSG_CLASS_ASPTM:
		case M3UA_MSG_CLASS_SSNM:
		case M3UA_MSG_CLASS_MGMT:
			ret = send_m3ua_msg (as_node, CONTROL_STREAM);
			break;
		case 	
			M3UA_MSG_CLASS_TX_MSG:
			ret = send_m3ua_msg (as_node, DATA_STREAM);
			break;
		default:
			ret = send_m3ua_msg (as_node, CONTROL_STREAM);
			break;
	}

	if (-1 != ret)
	{
		as_node->msg_ptr = NULL;
		as_node->msg_len = 0;
	}
}

/*************************************************************************************
* Function : m3ua_send()
* Input    : 
* Output   : 
* Purpose  : Send m3ua message over SCTP
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int m3ua_recv (m3ua_as *as_node)
{
	return recv_m3ua_msg (as_node);
}

/*************************************************************************************
* Function : m3ua_recvd_aspsm_handler()
* Input    : 
* Output   : 
* Purpose  : To handle received ASPSM message
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_aspsm_hdlr (m3ua_as *as_node)
{
	unsigned char m3ua_send_msg[MAX_M3UA_MSG_LEN]	= {0};
	int m3ua_message_length				= 0;
	int ret						= 0;	

	switch(as_node->msg_ptr[MESSAGE_TYPE_POS])
	{
		case M3UA_MSG_TYPE_ASP_RESRVD:
			break;
		case M3UA_MSG_TYPE_ASP_UP:
			break;
		case M3UA_MSG_TYPE_ASP_DOWN:
			break;
		case M3UA_MSG_TYPE_ASP_UPACK:
			break;
		case M3UA_MSG_TYPE_ASP_DWN_ACK:
			break;
		case M3UA_MSG_TYPE_ASP_HTBT:
	
			if(as_node->msg_ptr[HTBT_MSG_LEN_POS] > MIN_HTBT_MSG_LEN)
			{
				as_node->HTBT_data_len =
					as_node->msg_ptr[HTBT_PRM_LEN_POS] - HTBT_PRM_LEN;
				memcpy(as_node->HTBT_data, &as_node->msg_ptr[HTBT_DATA_POS],
					as_node->HTBT_data_len);
			}
			
			as_node->message_class  = M3UA_MSG_CLASS_ASPSM;
			as_node->message_type   = M3UA_MSG_TYPE_ASP_HBT_ACK;
			m3ua_message_length	= create_m3ua_msg (as_node, m3ua_send_msg);
			
			as_node->msg_ptr	= m3ua_send_msg;
			as_node->msg_len 	= m3ua_message_length;
			
			ret = m3ua_send (as_node);
	
			break;

		case M3UA_MSG_TYPE_ASP_HBT_ACK:
			break;
	}
}

/*************************************************************************************
* Function : m3ua_recv_data()
* Input    : 
* Output   : 
* Purpose  : Extracting SCCP data from m3ua message
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int m3ua_recv_data (m3ua_as *as_node, unsigned char *sccp_data)
{
	int m3ua_msg_len = 0;
	int sccp_msg_len = 0;

	unsigned char recv_msg_data[MAX_M3UA_MSG_LEN] = {0};
	
	as_node->msg_ptr = recv_msg_data;
	as_node->msg_len = MAX_M3UA_MSG_LEN;

	for (;;) {
		do {
			m3ua_msg_len = m3ua_recv (as_node);
			medium_delay;
		} while (!m3ua_msg_len);

		switch (as_node->msg_ptr[MESSAGE_CLASS_POS])
		{
			case M3UA_MSG_CLASS_TX_MSG:
				as_node->msg_len = 
					m3ua_analyze_data (as_node, sccp_data);
				//as_node->msg_len = m3ua_msg_len;
				return as_node->msg_len;
			case M3UA_MSG_CLASS_ASPSM:
				m3ua_aspsm_hdlr (as_node);
				memset (recv_msg_data, 0, MAX_M3UA_MSG_LEN);
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				continue;
			case M3UA_MSG_CLASS_SSNM:
				m3ua_analyze_ssnm (as_node);
				memset (recv_msg_data, 0, MAX_M3UA_MSG_LEN);
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				continue;
			default:
				memset (recv_msg_data, 0, MAX_M3UA_MSG_LEN);
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				continue;
			/*insert in Q */
		}
	}
}	
