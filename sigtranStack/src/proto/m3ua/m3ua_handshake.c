 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines handshake
 *	      and has definitions for:
 *           - m3ua do audit
 *           - m3ua sg handshake 
 *           - m3ua asp handshake 
 *           - m3ua handshake 
 *           - m3ua transport
 *           - m3ua send inact 
 *           - m3ua send aspdwn 
 */

#include "m3ua_handshake.h"
#include "m3ua_headers.h"

/*************************************************************************************
* Function : m3ua_analyze_data 
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int m3ua_analyze_data (m3ua_as *as_node, 
		unsigned char *sccp_payload)
{
	int iter = MESSAGE_TYPE_POS;
	int msg_len = 0;
	int tag_val = 0;
	int proto_data_len = 0;
	if (as_node->msg_ptr[iter] == M3UA_MSG_TYPE_TX_DATA)
	{
		iter += NORMAL_TAG_LEN;
		msg_len = (as_node->msg_ptr[iter - 1]) * 256 + 
			as_node->msg_ptr[iter];
		iter++;	

		/** Check for tags and their values **/
		for (;;) {
			tag_val = as_node->msg_ptr[iter] * 256 +
				as_node->msg_ptr[iter + 1];
			
			switch (tag_val)
			{
				case M3UA_TAG_NW_APP:
					iter += NORMAL_TAG_LEN;
					if (as_node->network_appearance_len)
					{
						if (strncmp (as_node->network_appearance, 
							(char *)&as_node->msg_ptr[iter], NORMAL_TAG_LEN))
							printf ("error... incorrect Network Appearance\n");
					}
					else 
					{
						memcpy (as_node->network_appearance, 
						(void *)&as_node->msg_ptr[iter], NORMAL_TAG_LEN);
						as_node->network_appearance_len = NORMAL_TAG_LEN;
					}
					iter += NORMAL_TAG_LEN;
					continue;
				case UA_TAG_RTNG_CTXT:
					iter += NORMAL_TAG_LEN;
					if (as_node->routing_context_len)
					{
						if (strncmp (as_node->routing_context, 
							(char *)&as_node->msg_ptr[iter], NORMAL_TAG_LEN))
							printf ("error... Routing Context is incorrect\n");
					}
					else
						printf ("error... Routing Context not present");
					iter += NORMAL_TAG_LEN;
					continue;
				case M3UA_TAG_PROTO_DATA:
					iter += PROTOCOL_DATA_TAG_LEN;
					proto_data_len = as_node->msg_ptr[iter] * 256 +
						as_node->msg_ptr[iter + 1];
					iter += PROTOCOL_DATA_TAG_LEN;
					if (strncmp (as_node->origin->point_code, 
						(char *)&as_node->msg_ptr[iter], POINT_CODE_LEN))
						printf ("error... invalid origin point code\n");							
					iter += POINT_CODE_LEN;
					if (strncmp (as_node->destn->point_code, 
						(char *)&as_node->msg_ptr[iter], POINT_CODE_LEN))
						printf ("error... invalid destination point code\n");
					iter += POINT_CODE_LEN;
					if (as_node->msg_ptr[iter] != TAG_SCCP)
						printf ("error... different protocol identifier %d\n",
						as_node->msg_ptr[iter]);
					iter++;
					if (!as_node->msg_ptr[iter])
						printf ("warning... NI not present\n");
					iter++;
					if (!as_node->msg_ptr[iter])
						printf ("warning... MP not present\n");
					iter++;
					if (!as_node->msg_ptr[iter])
						printf ("warning... SLS not present\n");
					iter++;
					memcpy (sccp_payload, (void *)&as_node->msg_ptr[iter], 
						proto_data_len);
					printf ("%d\n", msg_len - proto_data_len);// - (proto_data_len + iter));	
					return proto_data_len;
				default :
					printf ("error... Unknown tag %d in m3ua data packet", 
						tag_val);
					return 0;
			}
		}
	}
}

/*************************************************************************************
* Function : m3ua_analyze_data 
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_analyze_ssnm (m3ua_as *as_node)
{
	switch (as_node->msg_ptr[MESSAGE_TYPE_POS]) {
		case M3UA_MSG_TYPE_SSNM_DAVA:
			printf ("destination available\n");
		default:
			break;
	}
}	

/*************************************************************************************
* Function : m3ua_do_audit()
* Input    : as_node
* Output   : 
* Purpose  :
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_do_audit (m3ua_as *as_node)
{
	int m3ua_message_length = 0;
	int recevd_bytes	= 0;
	int ret			= 0;
	unsigned char m3ua_send_msg [MAX_M3UA_MSG_LEN] = {0};
	unsigned char m3ua_recv_msg [MAX_M3UA_MSG_LEN] = {0};

	as_node->message_type = M3UA_MSG_TYPE_SSNM_DAUD;
	as_node->message_class= M3UA_MSG_CLASS_SSNM;
	m3ua_message_length   = create_m3ua_msg (as_node, m3ua_send_msg);	
	
	as_node->msg_ptr = m3ua_send_msg;
	as_node->msg_len = m3ua_message_length;
	ret = m3ua_send (as_node);
	memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);
#if 0
	memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);
	as_node->msg_ptr = m3ua_recv_msg;
	as_node->msg_len = MAX_M3UA_MSG_LEN;
	do {
		recevd_bytes = m3ua_recv (as_node);
		
	} while (!recevd_bytes);
#endif
}


/*************************************************************************************
* Function : m3ua_sg_handshake()
* Input    : as_node
* Output   : 
* Purpose  : m3ua handshake
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
static void m3ua_sg_handshake (m3ua_as *as_node)
{
	int ret, recevd_bytes = 0;
	int status_type = 0;
	int status_info = 0;
	int m3ua_message_length = 0;
	char status_tag;
	unsigned char m3ua_send_msg [MAX_M3UA_MSG_LEN] = {0};
	unsigned char m3ua_recv_msg [MAX_M3UA_MSG_LEN] = {0};
	as_node->msg_len = MAX_M3UA_MSG_LEN;
	
	as_node->host_state.sg_state = asp_sg_init;	
	while (as_node->host_state.sg_state != asp_sg_handshake_complete)
	{
		switch (as_node->host_state.sg_state) 
		{
			case asp_sg_init:
				/* specify the message type and class */
				as_node->message_class  = M3UA_MSG_CLASS_ASPSM;
				as_node->message_type   = M3UA_MSG_TYPE_ASP_UP;
		
				/* Send ASP_UP message to the peer */
				m3ua_message_length = create_m3ua_msg (as_node, m3ua_send_msg);
				as_node->msg_ptr = m3ua_send_msg;
				as_node->msg_len = m3ua_message_length;
				ret = m3ua_send (as_node);
				memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);

				/* Assigning states */
				as_node->host_state.sg_state = asp_sg_up_tx;	
				as_node->peer_state.sg_state = asp_sg_init;	
				break;

			case asp_sg_up_tx:
				/*Receive ASP_UP_ACK Packet*/	
				memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);		
				as_node->msg_ptr = m3ua_recv_msg;
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				do {
					recevd_bytes = m3ua_recv (as_node);
				} while (!recevd_bytes);
				if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPSM
					 && m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_UPACK)				
				{	
					/* Assigning states */
					as_node->peer_state.sg_state = asp_sg_up_ack_tx;	
				
					/*Receive Notify Packet*/					
					memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);		
					as_node->msg_ptr = m3ua_recv_msg;
					as_node->msg_len = MAX_M3UA_MSG_LEN;
					do {
						recevd_bytes = m3ua_recv (as_node);
					} while (!recevd_bytes);
					/*Check for Notify packet is received*/
					if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_MGMT && 
						m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_MGMT_NTFY)
					{
						/* Assigning states */
						as_node->peer_state.sg_state = asp_sg_notify_tx;	
						as_node->host_state.sg_state = asp_sg_up_done;	
			
						/*collect Routing context */
						/*
 						 * To check parameter's tag 
 						 * for status_type, 
 						 * status_info
	 					 */
		
						status_tag = m3ua_recv_msg[NOTIFY_STATUS_OFFSET];
						if(status_tag == TAG_UA_STATUS)
						{	
							status_type = m3ua_recv_msg[STATUS_TYPE_OFFSET];
							status_info = m3ua_recv_msg[STATUS_INFO_OFFSET];
			
							if(status_type == AS_STATE_CHANGE 
								&& status_info == AS_INACTIVE) {
								/*
								 *To check routing context....
								 *Fixed position is not known
								 **/	
								if (m3ua_recv_msg[ROUTING_CTXT_TAG_POS] 
									== TAG_UA_ROUTING_CONTEXT) {
									/*
									 *routing context found,
									 *Copy to as node 
									 */
									as_node->routing_context_len =
										 m3ua_recv_msg[ROUTING_CONTEXT_BEGIN] 
										- NORMAL_TAG_LEN;
									memcpy(as_node->routing_context, 
										&m3ua_recv_msg[MAX_ROUTING_CONTEXT_LEN],
											 as_node->routing_context_len);
								}
							}
							/* specify the message type and class */
							as_node->message_class	= M3UA_MSG_CLASS_ASPTM;
							as_node->message_type	= M3UA_MSG_TYPE_ASP_ACT;
					
							/* Send ASP_ACTIVE message to the peer */
							m3ua_message_length = create_m3ua_msg (as_node, m3ua_send_msg);
							as_node->msg_ptr = m3ua_send_msg;
							as_node->msg_len = m3ua_message_length;
							ret = m3ua_send (as_node);
							memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);
			
							/* Assigning states */
							as_node->host_state.sg_state = asp_sg_active_tx;
						}
					}
				}
				break;

			case asp_sg_active_tx:
				short_delay;
				/*Receive active ack packet*/
				memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);		
				as_node->msg_ptr = m3ua_recv_msg;
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				do {
					recevd_bytes = m3ua_recv (as_node);
				} while (!recevd_bytes);
					
				/*Check for ACTIVE ACK packet*/
				if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPTM 
					&& m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_ACT_ACK)	
				{
					/* Assigning states */
					as_node->peer_state.sg_state = asp_sg_active_ack_tx;
					/*Receive Notify Packet*/
					memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);		
					as_node->msg_ptr = m3ua_recv_msg;
					as_node->msg_len = MAX_M3UA_MSG_LEN;
					do {
						recevd_bytes = m3ua_recv (as_node);
					} while (!recevd_bytes);
	
					/*Check for Notify Packet*/
					if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_MGMT 
						&& m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_MGMT_NTFY) {

						/* Assigning states */
						as_node->peer_state.sg_state = asp_sg_notify_tx;

						status_type = m3ua_recv_msg[STATUS_TYPE_OFFSET];
						status_info = m3ua_recv_msg[STATUS_INFO_OFFSET];
			
						if(status_type == AS_STATE_CHANGE 
							&& status_info == APPLICATION_SERVER_ACTIVE){
							{
								/* Assigning states */
								as_node->host_state.sg_state = asp_sg_handshake_complete;
								as_node->peer_state.sg_state = asp_sg_handshake_complete;
								printf("ASP <--> SG handshake Complete...\n");
								break;
							}	
						}
						
						/*Send DAUD Packet*/					
						as_node->message_class= M3UA_MSG_CLASS_SSNM;
						as_node->message_type = M3UA_MSG_TYPE_SSNM_DAUD;
						m3ua_message_length = create_m3ua_msg (as_node, m3ua_send_msg);
						as_node->msg_ptr = m3ua_send_msg;
						as_node->msg_len = m3ua_message_length;
						ret = m3ua_send (as_node);
						memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);
	
						/* Assigning states */
						as_node->host_state.sg_state = asp_sg_DAUD_tx;			
					}
				}
				break;
			case asp_sg_up_ack_tx:
			case asp_sg_notify_tx:
			case asp_sg_up_done:
			case asp_sg_active_ack_tx:
			case asp_sg_active_done:
			case asp_sg_inactive_ack_tx:
			case asp_sg_inactive_tx:
			case asp_sg_inactive_done:
			case asp_sg_down_ack_tx:
			case asp_sg_down_tx:
			case asp_sg_DAUD_tx:
			case asp_sg_handshake_complete:
			default:
				break;
		}
	}		
}

/*************************************************************************************
* Function : m3ua_asp_handshake()
* Input    : as_node
* Output   : 
* Purpose  : m3ua handshake
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
static void m3ua_asp_handshake (m3ua_as *as_node)
{
	int ret, recevd_bytes = 0;
	int m3ua_message_length = 0;
	unsigned char m3ua_send_msg [MAX_M3UA_MSG_LEN] = {0};
	unsigned char m3ua_recv_msg [MAX_M3UA_MSG_LEN] = {0};
	as_node->msg_len = MAX_M3UA_MSG_LEN;

	as_node->host_state.asp_state = asp_init;	
	while (as_node->host_state.asp_state != asp_handshake_complete)
	{
		switch (as_node->host_state.asp_state) 
		{
			case asp_init:
				/* specify the message type and class */
				as_node->message_class  = M3UA_MSG_CLASS_ASPSM;
				as_node->message_type   = M3UA_MSG_TYPE_ASP_UP;
		
				/* Send ASP_UP message to the peer */
				m3ua_message_length = create_m3ua_msg (as_node, m3ua_send_msg);
				as_node->msg_ptr = m3ua_send_msg;
				as_node->msg_len = m3ua_message_length;
				ret = m3ua_send (as_node);
				memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);
								
				/* Assigning states */
				as_node->host_state.asp_state = asp_up_tx;
				break;

			case asp_up_tx :
				/*Receive Packet*/
				memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);		
				as_node->msg_ptr = m3ua_recv_msg;
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				do {
					recevd_bytes = m3ua_recv (as_node);
				} while (!recevd_bytes);

				if( m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPSM &&	
					m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_UP )
				{
					/* Assigning states */
					as_node->peer_state.asp_state = asp_up_tx;
				}
				else if( m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPSM &&
					m3ua_recv_msg[MESSAGE_TYPE_POS]  == M3UA_MSG_TYPE_ASP_UPACK ) 
				{
					/* Assigning states */
					as_node->host_state.asp_state = asp_up_done;
				}
	
				/*Receive Packet*/
				memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);
				as_node->msg_ptr = m3ua_recv_msg;
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				do {
					recevd_bytes = m3ua_recv (as_node);
				} while (!recevd_bytes);
					
				if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPSM &&			
					m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_UP )
				{
					/* Assigning states */
					as_node->peer_state.asp_state = asp_up_tx;	
				}
				else if( m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPSM &&		
					m3ua_recv_msg[MESSAGE_TYPE_POS]  == M3UA_MSG_TYPE_ASP_UPACK )
				{
					/* Assigning states */
					as_node->host_state.asp_state = asp_up_done;
				}
				break;

			case asp_up_done:
				if (as_node->peer_state.asp_state == asp_up_tx)
				{
					/* specify the message type and class */
					as_node->message_class = M3UA_MSG_CLASS_ASPSM;		
					as_node->message_type = M3UA_MSG_TYPE_ASP_UPACK;		
					
					/* Send ASP_UP_ACK message to the peer */
					m3ua_message_length = create_m3ua_msg (as_node, m3ua_send_msg);
					as_node->msg_ptr = m3ua_send_msg;
					as_node->msg_len = m3ua_message_length;
					ret = m3ua_send (as_node);
					memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);
				}
			
				/* specify the message type and class */
				as_node->message_class = M3UA_MSG_CLASS_ASPTM;		
				as_node->message_type = M3UA_MSG_TYPE_ASP_ACT;		
				
				/* Send ASP_ACT message to the peer */
				m3ua_message_length = create_m3ua_msg (as_node, m3ua_send_msg);
				as_node->msg_ptr = m3ua_send_msg;
				as_node->msg_len = m3ua_message_length;
				ret = m3ua_send (as_node);
				memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);

				/* Assigning states */
				as_node->host_state.asp_state = asp_active_tx;	
				break;
	
			case asp_active_tx:
				/*Receive Packet*/
				memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);		
				as_node->msg_ptr = m3ua_recv_msg;
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				do {
					recevd_bytes = m3ua_recv (as_node);
				} while (!recevd_bytes);
		
				if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPTM &&			
					m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_ACT ) 				
				{
					/* Assigning states */
					as_node->peer_state.asp_state = asp_active_tx;	
				}
				else if( m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPTM &&			
					m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_ACT_ACK ) 				
				{
					/* Assigning states */
					as_node->host_state.asp_state = asp_active_done;	
				}

				/*Receive Packet*/
				memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);		
				as_node->msg_ptr = m3ua_recv_msg;
				as_node->msg_len = MAX_M3UA_MSG_LEN;
				do {
					recevd_bytes = m3ua_recv (as_node);
				} while (!recevd_bytes);
		
				if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPTM &&			
					m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_ACT) 				
				{
					/* Assigning states */
					as_node->peer_state.asp_state = asp_active_tx;	
				}
				else if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPTM &&			
					m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_ACT_ACK) 				
				{
					/* Assigning states */
					as_node->host_state.asp_state = asp_active_done;	
				}

				break;

			case asp_active_done:
				if (as_node->peer_state.asp_state == asp_active_tx)
				{	
					/* specify the message type and class */
					as_node->message_class = M3UA_MSG_CLASS_ASPTM;		
					as_node->message_type = M3UA_MSG_TYPE_ASP_ACT_ACK;		
				
					/* Send ASP_ACT message to the peer */
					m3ua_message_length = create_m3ua_msg (as_node, m3ua_send_msg);
					as_node->msg_ptr = m3ua_send_msg;
					as_node->msg_len = m3ua_message_length;
					ret = m3ua_send (as_node);
					memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);
				}
				as_node->host_state.asp_state = asp_handshake_complete;
				printf("ASP<-->ASP Handshake complete...\n");	
				break;

			case asp_inactive_ack_tx:
			case asp_inactive_tx:
			case asp_inactive_done:
			case asp_down_ack_tx:
			case asp_down_tx:
			default:
				break;
		}
	}
}

/*************************************************************************************
* Function : m3ua_handshake()
* Input    : as_node
* Output   : 
* Purpose  : m3ua handshake
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_handshake (m3ua_as *as_node)
{
	switch (as_node->peer_type)
	{
		case ASP:
		case IPSP:
			m3ua_asp_handshake (as_node);
			break;
		case SGW:
			m3ua_sg_handshake (as_node);
			break;
		default:
			printf ("invalid connection type");
	}
}

/*************************************************************************************
* Function : m3ua_transport()
* Input    : as_node
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_transport (m3ua_as *as_node)
{
	unsigned char data_msg[MAX_M3UA_MSG_LEN] = {0};	
	int m3ua_message_length = 0;
	int ret, recevd_bytes = 0;
	unsigned char m3ua_recv_msg [MAX_M3UA_MSG_LEN] = {0};
	
	as_node->msg_ptr = NULL;
	as_node->msg_len = 0;
		
	as_node->message_class	= M3UA_MSG_CLASS_TX_MSG;
	as_node->message_type	= M3UA_MSG_TYPE_TX_DATA;
	m3ua_message_length	= create_m3ua_msg (as_node, data_msg);
	ret = m3ua_send (as_node);
}

/*************************************************************************************
* Function : m3ua_send_inact()
* Input    : as_node
* Output   : 
* Purpose  :
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_send_inact (m3ua_as *as_node)
{
	int m3ua_message_length = 0;
	int recevd_bytes	= 0;
	int ret			= 0;
	unsigned char m3ua_send_msg [MAX_M3UA_MSG_LEN] = {0};
	unsigned char m3ua_recv_msg [MAX_M3UA_MSG_LEN] = {0};

	as_node->message_type = M3UA_MSG_TYPE_ASP_INA;
	as_node->message_class= M3UA_MSG_CLASS_ASPTM;
	m3ua_message_length   = create_m3ua_msg (as_node, m3ua_send_msg);	
	
	as_node->msg_ptr = m3ua_send_msg;
	as_node->msg_len = m3ua_message_length;
	ret = m3ua_send (as_node);
	memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);

	memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);
	as_node->msg_ptr = m3ua_recv_msg;
	as_node->msg_len = MAX_M3UA_MSG_LEN;
	short_delay;
	do {
		recevd_bytes = m3ua_recv (as_node);
		short_delay;
	} while (!recevd_bytes);
	
	if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPTM 
			&& m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_INA_ACK)    
	{   
		/* Assigning states */
		as_node->host_state.asp_state =	asp_inactive_done;  	
	}
}

/*************************************************************************************
* Function : m3ua_send_aspdwn()
* Input    : as_node
* Output   : 
* Purpose  :
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void m3ua_send_aspdwn (m3ua_as *as_node)
{
	int m3ua_message_length = 0;
	int recevd_bytes	= 0;
	int ret			= 0;
	unsigned char m3ua_send_msg [MAX_M3UA_MSG_LEN] = {0};
	unsigned char m3ua_recv_msg [MAX_M3UA_MSG_LEN] = {0};

	as_node->message_type = M3UA_MSG_TYPE_ASP_DOWN;
	as_node->message_class= M3UA_MSG_CLASS_ASPSM;
	m3ua_message_length   = create_m3ua_msg (as_node, m3ua_send_msg);	
	
	as_node->msg_ptr = m3ua_send_msg;
	as_node->msg_len = m3ua_message_length;
	ret = m3ua_send (as_node);
	memset (m3ua_send_msg, 0, MAX_M3UA_MSG_LEN);

	memset (m3ua_recv_msg, 0, MAX_M3UA_MSG_LEN);
	as_node->msg_ptr = m3ua_recv_msg;
	as_node->msg_len = MAX_M3UA_MSG_LEN;
	do {
		recevd_bytes = m3ua_recv (as_node);
	} while (!recevd_bytes);
	
	if (m3ua_recv_msg[MESSAGE_CLASS_POS] == M3UA_MSG_CLASS_ASPSM 
			&& m3ua_recv_msg[MESSAGE_TYPE_POS] == M3UA_MSG_TYPE_ASP_DWN_ACK)    
	{   
		/* Assigning states */
		as_node->host_state.asp_state =	asp_down_ack_tx;  	
	}
}
