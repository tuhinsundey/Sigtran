#include "hlr_connect.h"

unsigned char hlr_req_data[400] = {0};

m3ua_asp *local_asp_node;
m3ua_as *local_as;
pthread_mutex_t queue_lock;
unsigned char hlr_data[400] = {0};

static int hlr_connect ()
{
	m3ua_asp *asp_node = (m3ua_asp *) malloc (sizeof (m3ua_asp));
	if (!asp_node)
		return MALLOC_FAILURE;  
  
	memset (asp_node, 0, sizeof (m3ua_asp));
	m3ua_init (asp_node, "./config/hlr_connect.ini");
	local_asp_node = asp_node;
  
	m3ua_as *as_node = NULL; 
	as_node = (m3ua_as *) malloc (sizeof (m3ua_as));
	if (!as_node)
    	return MALLOC_FAILURE;
  
	m3ua_as_config (as_node, "./config/hlr_connect.ini");
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
	sccp_params_send sccp_params_temp;
	sccp_params_temp.sccp_msg_type   = SCCP_MSG_TYPE_UDT;
	sccp_params_temp.sccp_msg_class  = SCCP_PROTO_CLASS_1;
        
	sccp_params_temp.called_pty.ssn_present  = true;
	sccp_params_temp.called_pty.pc_present  = false;
	sccp_params_temp.called_pty.gt_present  = true;
	memcpy (sccp_params_temp.called_pty.gt, "919912345678",12);
	sccp_params_temp.called_pty.ssn  = SSN_TYPE_HLR;
	sccp_params_temp.called_pty.routing_indicator  = ROUTE_ON_GT;
        
	sccp_params_temp.calling_pty.ssn_present  = true;
	sccp_params_temp.calling_pty.pc_present  = false;
	sccp_params_temp.calling_pty.gt_present  = true;
	memcpy (sccp_params_temp.calling_pty.gt,"919912345678",12);
	sccp_params_temp.calling_pty.ssn  = SSN_TYPE_MSC;
	sccp_params_temp.calling_pty.routing_indicator  = ROUTE_ON_GT;
    
	/****** FOR sendRoutingInfoForSM*********/
	sccp_params_temp.sms_fields.trx_id = 1234567;  
	memcpy (sccp_params_temp.sms_fields.SM_RP_DA.number, 
		(unsigned char *)"919923456789", 12);  
	sccp_params_temp.sms_fields.SM_RP_DA.length = 12;  
	memcpy (sccp_params_temp.sms_fields.SM_RP_OA.number, 
		(unsigned char *)"919923456790", 12);  
	sccp_params_temp.sms_fields.SM_RP_OA.length = 12;  
	int msg_len = create_sccp_unitdata (hlr_data, &sccp_params_temp, MAP_SRI_SM_REQ);
	local_as->msg_ptr = hlr_data;
	local_as->msg_len = msg_len;
	m3ua_transport (local_as);
	medium_delay;
}

void hlr_init ()
{
	pthread_t conn_thread;

	if (hlr_connect ())
		printf ("connected with hlr... \n");
	if (pthread_create (&conn_thread, NULL, 
		(void *)&th_process_sccp_msg, NULL))
		printf ("error creating thread... \n");
}

