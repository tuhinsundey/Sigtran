 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines rear end initialization process and  
 *            has definitions for:
 *           - smsc_connect
 *           - th sccp recv
 *           - th process sccp msg
 *           - sccp rear init 
 */

#include "m3ua_nodes.h"
#include "m3ua_handshake.h"
#include "route_logic.h"
#include "smsc_rear.h"

unsigned char message_data[400] = {0};

m3ua_asp *local_asp_node;
m3ua_as *local_as;
pthread_mutex_t queue_lock;

static int smsc_connect ()
{
	m3ua_asp *asp_node = (m3ua_asp *) malloc (sizeof (m3ua_asp));
	if (!asp_node)
		return MALLOC_FAILURE;  
  
	memset (asp_node, 0, sizeof (m3ua_asp));
	m3ua_init (asp_node, "./config/smsc_rear.ini");
	local_asp_node = asp_node;
  
	m3ua_as *as_node = NULL; 
	as_node = (m3ua_as *) malloc (sizeof (m3ua_as));
	if (!as_node)
    		return MALLOC_FAILURE;
  
	m3ua_as_config (as_node, "./config/smsc_rear.ini");
	as_node->parent = asp_node;

	m3ua_handshake (as_node);
  
	/* do handshake and initialize it to use */
	/* has to be blocking */
  
	local_as = as_node;
  
	/* 
	 *register the attributes, 
	 * store them to be used 
	 *by other entities, 
	 */
	register_route (as_node);
	m3ua_do_audit (as_node);
	return SUCCESS;
} 

static void th_sccp_recv (void)
{
	int msg_len;
	char *msg_ptr = NULL;	
	for (;;)
	{
		;
		/* insert inside queue */
	}       
}

static void th_process_sccp_msg ()
{
	sccp_params sccp_ret;
	sccp_route *route_ptr = NULL;
	unsigned char sccp_message[MAX_SCCP_MSG_LEN] = {0};
	int corr_ID = 0;
	
#if 0	
	tcap_msg_type msg_type;
	m3ua_as *route_as = NULL;
	int corr_ID;  
	for (;;)
	{
		/* pop out message from queue */
		msg_type = sccp_analyze (); /* SRI, FORWARD_SM */
		sccp_route *route_ptr;
		store ("");	
		/* store the routing context to be used to deliver the message */
		/* store the message attributes in STORAGE */
		sccp_frame_packet ();
		if (!(resolve_route (route_ptr, route_as)))
		printf ("ERROR");
		sccp_send (sccp *buffer, route_as); 	
	} 
#endif
	while (local_as->host_state.sg_state == asp_sg_handshake_complete) {

		sccp_params_send sccp_params_temp;
		sccp_params_temp.sccp_msg_type   = SCCP_MSG_TYPE_UDT;
		sccp_params_temp.sccp_msg_class  = SCCP_PROTO_CLASS_1;
        
		sccp_params_temp.called_pty.ssn_present  = true;
		sccp_params_temp.called_pty.pc_present  = false;
		sccp_params_temp.called_pty.gt_present  = true;
		memcpy (sccp_params_temp.called_pty.gt, "919912345678", 12);
		sccp_params_temp.called_pty.ssn  = SSN_TYPE_HLR;
		sccp_params_temp.called_pty.routing_indicator = ROUTE_ON_GT;
			//sccp_ret.called_pty.routing_indicator;
        
		sccp_params_temp.calling_pty.ssn_present  = true;
		sccp_params_temp.calling_pty.pc_present  = false;
		sccp_params_temp.calling_pty.gt_present  = true;
		memcpy (sccp_params_temp.calling_pty.gt,"919912345678",12);
		sccp_params_temp.calling_pty.ssn  = SSN_TYPE_MSC;
		sccp_params_temp.calling_pty.routing_indicator  = ROUTE_ON_GT;
    
		/****** MO Forward SM ********/
		/*sccp_params_temp.sms_fields.trx_id = 1234567;  
		memset(sccp_params_temp.sms_fields.SM_RP_DA.number, 0 ,13);
		memcpy (sccp_params_temp.sms_fields.SM_RP_DA.number, 
		(unsigned char *)"919923456789", 12);  
		sccp_params_temp.sms_fields.SM_RP_DA.length = 12;  
		memset(sccp_params_temp.sms_fields.SM_RP_OA.number, 0 ,13);
		memcpy (sccp_params_temp.sms_fields.SM_RP_OA.number, 
				(unsigned char *)"919923456790", 12);  
		sccp_params_temp.sms_fields.SM_RP_OA.length = 12;  
		memset(sccp_params_temp.sms_fields.TP_Orig_Dest_Number.number, 0 ,13);
		memcpy (sccp_params_temp.sms_fields.TP_Orig_Dest_Number.number, 
				(unsigned char *)"919923456790", 12);  
		sccp_params_temp.sms_fields.TP_Orig_Dest_Number.length = 12;  
		memset(sccp_params_temp.sms_fields.sms_data, 0 , 160);
		memcpy (sccp_params_temp.sms_fields.sms_data , "tuhin.shankar.dey@gmail.com", 14);  
		int msg_len = create_sccp_unitdata (message_data, 
				&sccp_params_temp, MAP_MO_FORWARD_SM_REQ);
		as_node->msg_ptr = message_data;
		as_node->msg_len = msg_len;
		m3ua_transport (as_node);*/
	
		/****** MT Forward SM ********/
    
		sccp_params_temp.sms_fields.trx_id = 1234567;  
		memset(sccp_params_temp.sms_fields.SM_RP_DA.number, 0 , 16);
		memcpy (sccp_params_temp.sms_fields.SM_RP_DA.number, 
				(unsigned char *)"919923456789234", 15);  
		sccp_params_temp.sms_fields.SM_RP_DA.length = 15;  
		memset (sccp_params_temp.sms_fields.SM_RP_OA.number, 0 , 13);
		memcpy (sccp_params_temp.sms_fields.SM_RP_OA.number, 
				(unsigned char *)"919923456790", 12);  
		sccp_params_temp.sms_fields.SM_RP_OA.length = 12;  
		memset(sccp_params_temp.sms_fields.TP_Orig_Dest_Number.number, 0 , 13);
		memcpy (sccp_params_temp.sms_fields.TP_Orig_Dest_Number.number, 
				(unsigned char *)"919923456790", 12);  
		sccp_params_temp.sms_fields.TP_Orig_Dest_Number.length = 12;  
		memset(sccp_params_temp.sms_fields.sms_data, 0 , 160);
		memcpy (sccp_params_temp.sms_fields.sms_data , "tuhin.shankar.dey@gmail.com...", 14);  
		
	#if 0
		int msg_len = create_sccp_unitdata (message_data, 
				&sccp_params_temp, MAP_MT_FORWARD_SM_REQ);
		
		local_as->msg_ptr = sccp_message;
		local_as->message_class = M3UA_MSG_CLASS_ASPSM;
		local_as->message_type = M3UA_MSG_TYPE_ASP_HTBT;
		msg_len = create_m3ua_msg (local_as, sccp_message);
		local_as->msg_len = msg_len;

		int ret = m3ua_send (local_as);
	#endif
		int ret = m3ua_recv_data (local_as, message_data);
			printf ("received data of len %d\n", ret);
		short_delay;
	}
}

void smsc_rear_init ()
{
	pthread_t conn_thread;

	if (smsc_connect ())
		printf ("smsc started successfully \n");
	if (pthread_create (&conn_thread, NULL, 
		(void *)&th_process_sccp_msg, NULL))
		printf ("error creating thread... \n");
}

