#include "common.h"
#include "m3ua.h"

extern char routing_context[8];

int SCCP_LEN = 0x78; 
char *DA_MSISDN = NULL;

static int opc = 65793;
static int dpc = 66308;

unsigned int trx_id_MO = 1234567;
unsigned int  trx_id_SRI =2345671;

/**** Network Identifier Tag ****************/
static const char network_identifier[2] = {0x02, 0x00};

/**** Routing Context Tag and primitives ****/
static const char affected_point_code_tag[2]	= {0x00, 0x12};

/**** Routing Context Tag and primitives ****/
static const char routing_context_tag[2]	= {0x00, 0x06};
static const char routing_context_val[4]	= {0x00, 0x00, 0x00, 0x01};

/**** Protocol Class tag and primitives ****/
static const char protocol_class_tag[2]	= {0x01, 0x15};
static const char protocol_class_val	= 0x0;

/**** Address tags and primitives ****/
static const char source_address_tag[2]	= {0x01, 0x02};
static const char dest_address_tag[2]	= {0x01, 0x03};
/* Route on SSN + IP (4) */
static const char routing_indicator_tag[2]	= {0x00, 0x04};
/* Reserved bit:0, GT: False, PC: False, SSN: True */
static const char address_indicator_tag[2]	= {0x00, 0x01};
static const char subsystem_number_tag[2]	= {0x80, 0x03};
static const char subsystem_number	= 0x07;
static const char IPv4_address_tag[2]	= {0x80, 0x04};

/**** Sequence Control tags and primitives ****/
static const char sequence_control_tag[2]	= {0x01, 0x16};

/**** Hop Counter tag and primitives ****/
static const char hop_counter_tag[2]	= {0x01, 0x01};
static const char ss7_hop_counter_tag	= 0x0f;

/**** Importance tag and primitives ****/
static const char importance_tag[2]	= {0x01, 0x13};
static const char importance_val		= 0x05;

/**** Data Tag ****/
static const char data_tag[2]		= {0x01, 0x0b};

/**** Lengths of fields for various tags ****/
static const char small_param_len[2]	= {0x00, 0x08};
static const char large_param_len[2]	= {0x00, 0x18};
static const char min_cldt_data_len[2]	= {0x00, 0x04};


/*************************************************************************************
 Function : convert_int_to_hex()
* Input    : number,output
* Output   : 
* Purpose  : converting integer to hex
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void convert_int_to_hex( unsigned int number, unsigned char * output)
{	
	unsigned int temp_number;

	temp_number = ( number >> R_SHIFT_24);
    memcpy ((void *)&output[0], &temp_number, 1);
    temp_number = ( number >> R_SHIFT_16);
    memcpy ((void *)&output[1],&temp_number, 1);
    temp_number = ( number >> R_SHIFT_8);
    memcpy ((void *)&output[2], &temp_number, 1);
    memcpy ((void *)&output[3], &number, 1);
} 


/*************************************************************************************
* Function : inet_aton_buff()
* Input    : ip_addr,result
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void inline inet_aton_buff (char *ip_addr, 
					unsigned char *result)
{
    long int long_addr = 0;
    long_addr = ntohl (inet_addr (ip_addr));
    int remainder, iter = 7, small_iter;
    unsigned char conv_vals[8] = {0};

    for (; long_addr > 16; --iter)
    {
        remainder = long_addr % 16;
        conv_vals[iter] = remainder;
        long_addr /= 16;
    }
    remainder = long_addr % 16;
    conv_vals[iter] = remainder;
    iter = 0;

    for (small_iter = 0; small_iter < 4; ++small_iter)
    {
        result[small_iter] = (conv_vals[iter] * 16) +
                            conv_vals[iter + 1];
        iter += 2;
    }
}


/*************************************************************************************
* Function : create_m3ua_CO()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_CO (unsigned char *m3ua_message, 
			char message_type)
{
	return 0;
}


/*************************************************************************************
* Function : create_m3ua_SNM()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_SNM (unsigned char *m3ua_message, 
			char message_type)
{
	return 0;
}


/*************************************************************************************
* Function : create_m3ua_data_SRI()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_data_SRI (unsigned char *m3ua_message)
{
	int m3ua_length = 0;
	int sccp_protocol_data_len = 0;
	int iterator = 0;
	int temp_length;
	int msg_len;
	//char *DA_MSISDN = NULL;
	unsigned char *tcap_message = NULL;	
	
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_TX_MSG;
	m3ua_message[message_type] =  M3UA_MSG_TYPE_CL_CLDT;

	sms_fields sri_input;
	sri_input.trx_id = ++trx_id_SRI;

	//memcpy(sri_input.SM_RP_DA.number,msisdn, 12); 
	DA_MSISDN = get_receiver();
	if(!DA_MSISDN){
		m3ua_length = 0;
		return m3ua_length;
	}
	printf("IN M3UA : DA_MSISDN = %s\n", DA_MSISDN);
	memcpy((char *)sri_input.SM_RP_DA.number, DA_MSISDN, strlen(DA_MSISDN)); 
	//free(DA_MSISDN);
	//sri_DA_msisdn++;
	sri_input.SM_RP_DA.length = 12;
//	memcpy(sri_input.SM_RP_OA.number,msisdn, 12); 
	sprintf((char *)sri_input.SM_RP_OA.number, "%ld", sri_OA_msisdn); 
	sri_OA_msisdn++;
	sri_input.SM_RP_OA.length = 12;
	//	memset(tcap_message, 0, sizeof(tcap_message)); 
	tcap_message = sri_SM_req(&sri_input, (int *)&msg_len);	
	
	sccp_protocol_data_len = 30 + msg_len; //SCCP_LEN;
	
	m3ua_length = sccp_protocol_data_len + 40;
	convert_int_to_hex (m3ua_length, &m3ua_message[4]);		
		
	//char routing_context[4] ={0x00, 0x00, 0x00, 0x64};	
	iterator = 8;
	char temp [8] = {0x02, 0x00, 0x00, 0x08, 0x01, 0xab, 0xcd, 0xef};
	memcpy(&m3ua_message[iterator], &temp, 8); 

	iterator = 16;	
	m3ua_message[iterator]     = 0x00;
	m3ua_message[iterator + 1] = 0x06;
	m3ua_message[iterator + 2] = 0x00;
	m3ua_message[iterator + 3] = 0x08;
	memcpy(&m3ua_message[iterator+4], routing_context, 4);

	iterator = 24;
	m3ua_message[iterator]   = 0x02;
	m3ua_message[iterator + 1] = 0x10;
    
	temp_length = sccp_protocol_data_len + 16;
	memcpy ((void *)&m3ua_message[iterator + 3], &temp_length, 1);
	temp_length = ( (sccp_protocol_data_len + 8)  >> R_SHIFT_8);
	memcpy ((void *)&m3ua_message[iterator + 2], &temp_length, 1); 

	iterator = 28 ;
	convert_int_to_hex (opc, &m3ua_message[iterator]);	
	
	iterator = 32;
	convert_int_to_hex (dpc,&m3ua_message[iterator]);	
	
	iterator = 36;
	m3ua_message[iterator]      = TAG_SCCP;
	m3ua_message[iterator + 1]  = TAG_NI;
	m3ua_message[iterator + 2]  = TAG_MP;
	m3ua_message[iterator + 3]  = TAG_SLS;
	
	iterator += create_sccp_unitdata (&m3ua_message[iterator + 4], 
					(unsigned char *)"919969679389", 
					(unsigned char *)"919869299992");

	m3ua_message [iterator + 4] = msg_len;
	memcpy (&m3ua_message[iterator + 5], tcap_message, 
				tcap_message [1] + 2);	
	freeze (tcap_message);		
	
	return m3ua_length;
}


/*************************************************************************************
* Function : create_m3ua_data_SRI_ACK()
* Input    : 
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_data_SRI_ACK (unsigned char *m3ua_message, unsigned int trx_id)
{
	int m3ua_length = 0;
	int sccp_protocol_data_len = 0;
	int iterator = 0;
	int temp_length;
	int msg_len;
	char msisdn []= "919890798176"; 
	unsigned char IMSI[] = "123456789098765";
	unsigned char LMSI[] = "919890798176";
	
	unsigned char *tcap_message = NULL;	
	
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_TX_MSG;
	m3ua_message[message_type] =  M3UA_MSG_TYPE_CL_CLDT;

	sms_fields sri_input;
	sri_input.trx_id = trx_id;

	memcpy(sri_input.SM_RP_DA.number,msisdn, 12); 
	sri_input.SM_RP_DA.length = 12;
	memcpy(sri_input.SM_RP_OA.number,msisdn, 12); 
	sri_input.SM_RP_OA.length = 12;
	//	memset(tcap_message, 0, sizeof(tcap_message)); 

	tcap_message = form_sri_ack(trx_id, IMSI, LMSI, &msg_len);	
	
	sccp_protocol_data_len = 30 + msg_len; //SCCP_LEN;
	
	m3ua_length = sccp_protocol_data_len + 32;
	convert_int_to_hex (m3ua_length, &m3ua_message[4]);		
		
//	char routing_context[4] ={0x00, 0x00, 0x00, 0x64};	
	
	iterator = 8;
	m3ua_message[iterator]     = 0x00;
	m3ua_message[iterator + 1] = 0x06;
	m3ua_message[iterator + 2] = 0x00;
	m3ua_message[iterator + 3] = 0x08;
	memcpy(&m3ua_message[iterator+4], routing_context, 4);

	iterator = 16;
	m3ua_message[iterator]   = 0x02;
	m3ua_message[iterator + 1] = 0x10;
    
	temp_length = sccp_protocol_data_len + 16;
	memcpy ((void *)&m3ua_message[iterator + 3], &temp_length, 1);
	temp_length = ( (sccp_protocol_data_len + 8)  >> R_SHIFT_8);
	memcpy ((void *)&m3ua_message[iterator + 2], &temp_length, 1); 

	iterator = 20 ;
	convert_int_to_hex (dpc, &m3ua_message[iterator]);	
	
	iterator = 24;
	convert_int_to_hex (opc,&m3ua_message[iterator]);	
	
	iterator = 28;
	m3ua_message[iterator] 		= TAG_SCCP;
	m3ua_message[iterator + 1]  = TAG_NI;
	m3ua_message[iterator + 2]  = TAG_MP;
	m3ua_message[iterator + 3]  = TAG_SLS;
	
	iterator += create_sccp_unitdata (&m3ua_message[iterator + 4], 
					(unsigned char *)"919969679389", 
					(unsigned char *)"919869299992");

	m3ua_message [iterator + 4] = msg_len ;
	memcpy (&m3ua_message[iterator + 5], tcap_message, 
				tcap_message [1] + 2);	
	freeze (tcap_message);		
	return m3ua_length;
}


/*************************************************************************************
* Function : create_m3ua_data_MO_SM()
* Input    : m3ua_message,IMSI
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_data_MO_SM (unsigned char *m3ua_message, char *imsi)
{
	int m3ua_length = 0;
	int sccp_protocol_data_len = 0;
	int iterator = 0;
	int temp_length;
	int msg_len;
//	char msisdn []= "919890798176"; 
	unsigned char *tcap_message = NULL;	
	
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_TX_MSG;
	m3ua_message[message_type] =  M3UA_MSG_TYPE_CL_CLDT;

	sms_fields sri_input;
	sri_input.trx_id = ++trx_id_MO;
	memset(sri_input.sms_data,0,sizeof(sri_input.sms_data));
	memcpy(sri_input.sms_data,"tuhin.shankar.dey@gmail.com",11);

	memcpy(sri_input.SM_RP_DA.number, imsi, 12);
	mo_DA_msisdn++;
	sri_input.SM_RP_DA.length = 12;
	sprintf((char *)sri_input.SM_RP_OA.number, "%ld", mo_OA_msisdn); 
	sri_input.SM_RP_OA.length = 12;

	memcpy(sri_input.TP_Orig_Dest_Number.number, imsi, 12);
	sri_input.TP_Orig_Dest_Number.length = 12; 
	mo_OA_msisdn++;
  
	tcap_message = forward_sm_MO_packet(&sri_input, (int *)&msg_len);	
	sccp_protocol_data_len = 30 + msg_len; //SCCP_LEN;
	
	m3ua_length = sccp_protocol_data_len + 32;
	convert_int_to_hex (m3ua_length, &m3ua_message[4]);		
		
	char routing_context[4] ={0x00, 0x00, 0x00, 0x64};	
	iterator = 8;
	m3ua_message[iterator]     = 0x00;
	m3ua_message[iterator + 1] = 0x06;
	m3ua_message[iterator + 2] = 0x00;
	m3ua_message[iterator + 3] = 0x08;
	memcpy(&m3ua_message[iterator+4], routing_context, 4);

	iterator = 16;
	m3ua_message[iterator]   = 0x02;
	m3ua_message[iterator + 1] = 0x10;
    
	temp_length = sccp_protocol_data_len + 16;
	memcpy ((void *)&m3ua_message[iterator + 3], &temp_length, 1);
	temp_length = ( (sccp_protocol_data_len + 8)  >> R_SHIFT_8);
	memcpy ((void *)&m3ua_message[iterator + 2], &temp_length, 1); 

	iterator = 20 ;
	convert_int_to_hex (opc, &m3ua_message[iterator]);	
	
	iterator = 24;
	convert_int_to_hex (dpc,&m3ua_message[iterator]);	
	
	iterator = 28;
	m3ua_message[iterator] 		= TAG_SCCP;
	m3ua_message[iterator + 1]  = TAG_NI;
	m3ua_message[iterator + 2]  = TAG_MP;
	m3ua_message[iterator + 3]  = TAG_SLS;
	
	iterator += create_sccp_unitdata (&m3ua_message[iterator + 4], 
					(unsigned char *)"919969679389", 
					(unsigned char *)"919869299992");

	m3ua_message [iterator + 4] = msg_len ;
	memcpy (&m3ua_message[iterator + 5], tcap_message, 
				tcap_message [1] + 2);	
	freeze (tcap_message);		
	return m3ua_length;
}


/*************************************************************************************
* Function : create_m3ua_data_MO_SM_ACK()
* Input    : M3ua_message,trx_id,type
* Output   : 
* Purpose  : To create MO_SM_ACK  
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_data_MO_SM_ACK (unsigned char *m3ua_message, unsigned int trx_id, char type)
{
	int m3ua_length = 0;
	int sccp_protocol_data_len = 0;
	int iterator = 0;
	int temp_length;
	int msg_len;
	char msisdn []= "919890798176"; 
	unsigned char *tcap_message = NULL;	
	
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_TX_MSG;
	m3ua_message[message_type] =  M3UA_MSG_TYPE_CL_CLDT;

	sms_fields sri_input;
	sri_input.trx_id = trx_id;

	memcpy(sri_input.SM_RP_DA.number,msisdn, 12); 
	sri_input.SM_RP_DA.length = 12;
	memcpy(sri_input.SM_RP_OA.number,msisdn, 12); 
	sri_input.SM_RP_OA.length = 12;
	tcap_message = forward_sm_end_ack_packet(&msg_len, type, trx_id);
	
	sccp_protocol_data_len = 30 + msg_len; //SCCP_LEN;
	
	m3ua_length = sccp_protocol_data_len + 32;
	convert_int_to_hex (m3ua_length, &m3ua_message[4]);		
	
		
	char routing_context[4] ={0x00, 0x00, 0x00, 0x64};	
	iterator = 8;
	m3ua_message[iterator]     = 0x00;
	m3ua_message[iterator + 1] = 0x06;
	m3ua_message[iterator + 2] = 0x00;
	m3ua_message[iterator + 3] = 0x08;
	memcpy(&m3ua_message[iterator+4], routing_context, 4);

	iterator = 16;
	m3ua_message[iterator]   = 0x02;
	m3ua_message[iterator + 1] = 0x10;
    
	temp_length = sccp_protocol_data_len + 16;
	memcpy ((void *)&m3ua_message[iterator + 3], &temp_length, 1);
	temp_length = ( (sccp_protocol_data_len + 8)  >> R_SHIFT_8);
	memcpy ((void *)&m3ua_message[iterator + 2], &temp_length, 1); 

	iterator = 20 ;
	convert_int_to_hex (dpc, &m3ua_message[iterator]);	
	
	iterator = 24;
	convert_int_to_hex (opc,&m3ua_message[iterator]);	
	
	iterator = 28;
	m3ua_message[iterator] 		= TAG_SCCP;
	m3ua_message[iterator + 1]  = TAG_NI;
	m3ua_message[iterator + 2]  = TAG_MP;
	m3ua_message[iterator + 3]  = TAG_SLS;
	
	iterator += create_sccp_unitdata (&m3ua_message[iterator + 4], 
					(unsigned char *)"919969679389", 
					(unsigned char *)"919869299992");

	m3ua_message [iterator + 4] = msg_len ;
	memcpy (&m3ua_message[iterator + 5], tcap_message, 
				tcap_message [1] + 2);	
	freeze (tcap_message);		
	return m3ua_length;
}


/*************************************************************************************
* Function : create_m3ua_ASPTM()
* Input    : m3ua_message,message_type
* Output   : 
* Purpose  : To create ASPSM packet
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_ASPSM (unsigned char *m3ua_message, 
			char message_type)
{
	int m3ua_length;
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_ASPSM;
	
	switch (message_type)
	{
	case M3UA_MSG_TYPE_ASPSM_UP:
    		m3ua_message[3] = M3UA_MSG_TYPE_ASPSM_UP;
			break;
     	case M3UA_MSG_TYPE_ASPSM_DOWN:
    		m3ua_message[3] = M3UA_MSG_TYPE_ASPSM_DOWN;
			break;
     	case M3UA_MSG_TYPE_ASPSM_HTBT:
    		m3ua_message[3] = M3UA_MSG_TYPE_ASPSM_HTBT;
			break;
     	case M3UA_MSG_TYPE_ASPSM_UPACK:
    		m3ua_message[3] = M3UA_MSG_TYPE_ASPSM_UPACK;
			break;
     	case M3UA_MSG_TYPE_ASPSM_DWNACK:
    		m3ua_message[3] = M3UA_MSG_TYPE_ASPSM_DWNACK;
			break;
     	case M3UA_MSG_TYPE_ASPSM_HBTACK:
    		m3ua_message[3] = M3UA_MSG_TYPE_ASPSM_HBTACK;
			break;
	}
	memset (&m3ua_message[4], 0, reserved_bytes3);

	m3ua_length		= ASPSM_MSG_LEN;
	m3ua_message[7]	= m3ua_length;
	return m3ua_length;
}

/*************************************************************************************
* Function : create_m3ua_DAUD()
* Input    : m3ua_message,message_type
* Output   : 
* Purpose  : To create ASPSM packet
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_DAUD (unsigned char *m3ua_message, 
			char message_type, char *routing_context)
{
	int m3ua_length;
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_SNM;
	
	switch (message_type)
	{
	case M3UA_MSG_TYPE_SNM_DUNA:
    		m3ua_message[3] = M3UA_MSG_TYPE_SNM_DUNA;
			break;
     	case M3UA_MSG_TYPE_SNM_DAVA:
    		m3ua_message[3] = M3UA_MSG_TYPE_SNM_DAVA;
			break;
     	case M3UA_MSG_TYPE_SNM_DAUD:
    		m3ua_message[3] = M3UA_MSG_TYPE_SNM_DAUD;
			break;
     	case M3UA_MSG_TYPE_SNM_SCON:
    		m3ua_message[3] = M3UA_MSG_TYPE_SNM_SCON;
			break;
     	case M3UA_MSG_TYPE_SNM_DUPU:
    		m3ua_message[3] = M3UA_MSG_TYPE_SNM_DUPU;
			break;
     	case M3UA_MSG_TYPE_SNM_DRST:
    		m3ua_message[3] = M3UA_MSG_TYPE_SNM_DRST;
			break;
	}
	memset (&m3ua_message[4], 0, reserved_bytes3);

	m3ua_length		= ASPSNM_MSG_LEN;
	m3ua_message[7]	= m3ua_length;

	/*** Network Identifier ***/
	memcpy (&m3ua_message[8], network_identifier, 2);
	memcpy (&m3ua_message[10], small_param_len, 2);
	m3ua_message[12] = 0x01;
	m3ua_message[13] = 0xab;
	m3ua_message[14] = 0xcd;
	m3ua_message[15] = 0xef;

	/*** Routing Context ***/   
    memcpy (&m3ua_message[16], routing_context_tag, 2); 
    memcpy (&m3ua_message[18], small_param_len, 2); 
    //memcpy (&m3ua_message[12], routing_context_val, 4);
    memcpy (&m3ua_message[20], routing_context, 4); 
	
	memcpy (&m3ua_message[24], affected_point_code_tag, 2);
	memcpy (&m3ua_message[26], small_param_len, 2);
	
	/* Below is value of Point code */
	m3ua_message[28] = 0x00;
	m3ua_message[29] = 0x01;
	m3ua_message[30] = 0x03;
	m3ua_message[31] = 0x04;
	
	return m3ua_length;
}

/*************************************************************************************
* Function : create_m3ua_ASPTM()
* Input    : 
* Output   : 
* Purpose  : To create ASPTM packet 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_m3ua_ASPTM (unsigned char *m3ua_message, 
			char message_type, char *routing_context)
{
	int m3ua_length;
	m3ua_message[version]       = M3UA_VERSION_1;
	m3ua_message[reserved]      = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_ASPTM;
	
	switch (message_type)
	{
		case M3UA_MSG_TYPE_ASPTM_ACT:
			m3ua_message[3] = M3UA_MSG_TYPE_ASPTM_ACT;
			break;
     		case M3UA_MSG_TYPE_ASPTM_INACT:
			m3ua_message[3] = M3UA_MSG_TYPE_ASPTM_INACT;
			break;
		case M3UA_MSG_TYPE_ASPTM_ACTACK:
			m3ua_message[3] = M3UA_MSG_TYPE_ASPTM_ACTACK;
			break;
		case M3UA_MSG_TYPE_ASPTM_INACTACK:
			m3ua_message[3] = M3UA_MSG_TYPE_ASPTM_INACTACK;
			break;
	}
	memset (&m3ua_message[4], 0, reserved_bytes3);

	/*** Routing Context ***/	
	memcpy (&m3ua_message[8], routing_context_tag, 2);
	memcpy (&m3ua_message[10], small_param_len, 2);
	//memcpy (&m3ua_message[12], routing_context_val, 4);
	memcpy (&m3ua_message[12], routing_context, 4);

	/*** Message Length ***/
	m3ua_length		= ASPTM_MSG_LEN;
	m3ua_message[7]	= m3ua_length;
	return m3ua_length;
}

/*
 * Any SCCP message that has a ULP payload 
 * may have a connection-less data (CLDT).
 * This kind of data is generally a "Basic
 * Connectionless" data. A TCAP message is 
 * an example of a connectionless data. This
 * Kind of packet will have following fields:
 *
 * Protocol Class
 * Source Address
 * Destination Address
 * Sequence Control
 * SS7 hop Counter
 * Importance
 * Data (SS7 message)
 */    
/*************************************************************************************
* Function : create_sccp_CL()
* Input    : m3ua_message,message_type,dest_ip,src_ip
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sccp_CL (unsigned char *m3ua_message, char message_type, 
			char *dest_ip, char *src_ip)
{
	int m3ua_length;
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	m3ua_message[message_class] = M3UA_MSG_CLASS_MGMT;
	m3ua_message[3] = M3UA_MSG_TYPE_CL_CLDT;
	memset (&m3ua_message[4], 0, reserved_bytes3);

	/*** Routing Context ***/	
	memcpy (&m3ua_message[8], routing_context_tag, 2);
	memcpy (&m3ua_message[10], small_param_len, 2);
	memcpy (&m3ua_message[12], routing_context_val, 4);

	/*** Protocol Class ***/
	memcpy (&m3ua_message[16], protocol_class_tag, 2);
	memcpy (&m3ua_message[18], small_param_len, 2);
	memset (&m3ua_message[20], 0, reserved_bytes3);		
	m3ua_message[23]	=	protocol_class_val;

	/*** Source Address ***/
	memcpy (&m3ua_message[24], source_address_tag, 2);
	memcpy (&m3ua_message[26], large_param_len, 2);
	memcpy (&m3ua_message[28], routing_indicator_tag, 2);
	memcpy (&m3ua_message[30], address_indicator_tag, 2);
	memcpy (&m3ua_message[32], subsystem_number_tag, 2);
	memcpy (&m3ua_message[34], small_param_len, 2);
	memset (&m3ua_message[36], 0, reserved_bytes3);	
	m3ua_message[39] = subsystem_number;	
	memcpy (&m3ua_message[40], IPv4_address_tag, 2);	
	memcpy (&m3ua_message[42], small_param_len, 2);
	inet_aton_buff (src_ip, &m3ua_message[44]);
	
	/*** Destination Address ***/
	memcpy (&m3ua_message[48], dest_address_tag, 2);
	memcpy (&m3ua_message[50], large_param_len, 2);
	memcpy (&m3ua_message[52], routing_indicator_tag, 2);
	memcpy (&m3ua_message[54], address_indicator_tag, 2);
	memcpy (&m3ua_message[56], subsystem_number_tag, 2);
	memcpy (&m3ua_message[58], small_param_len, 2);
	memset (&m3ua_message[60], 0, reserved_bytes3);	
	m3ua_message[63] = subsystem_number;	
	memcpy (&m3ua_message[64], IPv4_address_tag, 2);	
	memcpy (&m3ua_message[66], small_param_len, 2);
	inet_aton_buff (dest_ip, &m3ua_message[68]);

	/*** Sequence Control ***/
	memcpy (&m3ua_message[72], sequence_control_tag, 2);
	memcpy (&m3ua_message[74], small_param_len, 2);
	memset (&m3ua_message[76], 0, reserved_bytes4);		

	/*** Hop Counter ***/
	memcpy (&m3ua_message[80], hop_counter_tag, 2);
	memcpy (&m3ua_message[82], small_param_len, 2);
	memset (&m3ua_message[84], 0, reserved_bytes3);		
	m3ua_message[87]	= ss7_hop_counter_tag;

	/*** Importance ***/
	memcpy (&m3ua_message[88], importance_tag, 2);
	memcpy (&m3ua_message[90], small_param_len, 2);
	memset (&m3ua_message[92], 0, reserved_bytes3);		
	m3ua_message[95]	= importance_val;

	/*** Data headers and length specifications ***/
	memcpy (&m3ua_message[96], data_tag, 2);
	memcpy (&m3ua_message[98], min_cldt_data_len, 2);

	m3ua_message[7] = MIN_M3UA_CLDT_LEN;
	m3ua_length = m3ua_message[7];
	return m3ua_length;
}



int create_m3ua_error_msg (unsigned char *m3ua_message, 
			char message_type)
{
	int m3ua_length;
	m3ua_message[version] = M3UA_VERSION_1;
	m3ua_message[reserved] = M3UA_RESERVED_BYTE;
	//m3ua_message[message_class] = M3UA_MSG_CLASS_ASPTM;
	m3ua_message[message_class] = 0x00;
	m3ua_message[3] = 0x00;
			
	m3ua_message[4] = 0x00;
	m3ua_message[5] = 0x00;
	m3ua_message[6] = 0x00;
	m3ua_message[7] = 0x28;
		
	m3ua_message[8]  = 0x00;
	m3ua_message[9]  = 0x0c;
	m3ua_message[10] = 0x00;
	m3ua_message[11] = 0x08;
			
	m3ua_message[12] = 0x00;
	m3ua_message[13] = 0x00;
	m3ua_message[14] = 0x00;
	m3ua_message[15] = 0x01;
			
	memcpy (&m3ua_message[16], routing_context_tag, 2);
	memcpy (&m3ua_message[18], small_param_len, 2);
	memcpy (&m3ua_message[20], routing_context_val, 4);
			
	m3ua_message[24] = 0x00;				
	m3ua_message[25] = 0x12;				
	m3ua_message[26] = 0x00;				
	m3ua_message[27] = 0x08;
			
	m3ua_message[28] = 0x00;					
	
	convert_int_to_hex (opc, &m3ua_message[29]);	
					
	m3ua_message[32] = 0x02;	
	m3ua_message[33] = 0x00;	
	m3ua_message[34] = 0x00;	
	m3ua_message[35] = 0x08;	
			
	m3ua_message[36] = 0x00;	
	m3ua_message[37] = 0x00;	
	m3ua_message[38] = 0x01;	
	m3ua_message[39] = 0x02;	

	/*** Message Length ***/
	m3ua_length		= 40;
	return m3ua_length;
}


