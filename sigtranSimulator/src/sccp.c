#include "common.h"
#include "sccp.h"
#include "tcap.h"

int sccp_len = 0x78; 

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
* Function : create_sccp_CO()
* Input    : sccp_message, message_type
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sccp_CO (unsigned char *sccp_message, 
			char message_type)
{
	return 0;
}


/*************************************************************************************
* Function : create_sccp_SNM()
* Input    : sccp_message, message_type
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sccp_SNM (unsigned char *sccp_message, 
			char message_type)
{
	return 0;
}


/************************************************************************************
* Function : create_sccp_unitdata_service()
* Input    : sccp_message,called_pty_GT, error_type
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sccp_unitdata_service (unsigned char *sccp_message, 
					unsigned char *called_pty_GT, 
					unsigned char *calling_pty_GT, char error_type)
{
	int iterator = 0;
	//sccp_message [iterator + 0] = TAG_SCCP_UNITDATA;/* Tag Unit Data */   
	//sccp_message [iterator + 1] = SCCP_MSG_CLASS_MGMT; 
	//int tcap_message_length;
	sccp_message [iterator + 0] = 0x0a;//TAG_SCCP_UNITDATA_SERIVCE;/* Tag Unit Data */   
        sccp_message [iterator + 1] = 0x02;//RETURN cause; 

	/**** next fields depend on number of parameters ****/

	/* Pointer to first mandatory Parameter */
	sccp_message [iterator + 2] = 0x03;
	/* Pointer to sencond mandatory Parameter*/	
	sccp_message [iterator + 3] = 0x0e;
	/* Pointer to third mandatory Parameter */	
	sccp_message [iterator + 4] = 0x19;

	iterator += 4;
	/*************** First Mandatory Parameter ****************/

	/* called party address length */
	sccp_message [iterator + 1] = 0x0b;

	/* 
	 * Address Indicator: 1001 0010 -> 0x92 -> 146 (128 + 0 + 16 + 2 + 0) 
	 * Route on GT : 0x00
	 * Global Title Indicator : 0x04
	 * Subsystem Number Indicator: SSN Present 0x01
	 * Pointcode : Pointcode not present: 0x00 
	 */ 
	sccp_message [iterator + 2] = 0x92;

	/* Subsystem number: 6 (hlr) */
	sccp_message [iterator + 3] = SSN_TYPE_HLR;
 
	/*
	 * Global Title : 9 bytes  
	 *
	 * Translation type: 0x00
	 *
	 * Numbering plan: ISDN/telephony 0x01
	 * Encoding scheme: BCD, even number of digits 0x02
	 * Nature of address indicator: International number 0x04
	 * 
	 */
	sccp_message [iterator + 4] = 0x00;
	sccp_message [iterator + 5] = 0x12;
	sccp_message [iterator + 6] = 0x04, 

	/*
	 * Address Information (digits) : 919969679389
	 *
	 * Coutry code (byte rotated): India 0x19 
	 * number (each byte rotated): 0x99, 0x96, 0x76, 0x39, 0x98, 
	 */
	encode_number_or_time (called_pty_GT, 
			&sccp_message[iterator + 7], strlen ((char *)(called_pty_GT)));	

	/*************** Second Mandatory Parameter ****************/

	iterator += 1 + strlen ((char *)(called_pty_GT));
	/* calling party address length */ 
	sccp_message [iterator] = 0x0b;

	/* 
	 * Address Indicator: 1001 0010 -> 0x92 -> 146 (0 + 16 + 2 + 0) 
	 * Route on GT : 0x00
	 * Global Title Indicator : 0x04
	 * Subsystem Number Indicator: SSN Present 0x01
	 * Pointcode : Pointcode not present: 0x00
	 */
	sccp_message [iterator + 1] = 0x12; 

	/* Subsystem number: 8 (msc) */
	sccp_message [iterator + 2] = SSN_TYPE_MSC; 
 
	/*
	 * Global Title : 9 bytes  
	 *
	 * Translation type: 0x00
	 *
	 * Numbering plan: ISDN/telephony 0x01
	 * Encoding scheme: BCD, even number of digits 0x02
	 * Nature of address indicator: International number 0x04
	 * 
	 */
	sccp_message [iterator + 3] = 0x00;
	sccp_message [iterator + 4] = 0x12;
	sccp_message [iterator + 5] = 0x04;
	//iterator = iterator + 6 ;
	/*
	 * Address Information (digits) : 919869299992
	 *
	 * Coutry code (byte rotated): India 0x19 
	 * number (each byte rotated): 0x99, 0x96, 0x76, 0x39, 0x98, 
	 */
	encode_number_or_time (calling_pty_GT, &sccp_message[iterator + 6], 
			strlen ((char *)(calling_pty_GT)));	
	iterator += 12;
	return iterator;
}



/************************************************************************************
* Function : create_sccp_unitdata()
* Input    : sccp_message,called_pty_GT
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sccp_unitdata (unsigned char *sccp_message, 
					unsigned char *called_pty_GT, 
					unsigned char *calling_pty_GT)
{
	int iterator = 0;
	//sccp_message [iterator + 0] = TAG_SCCP_UNITDATA;/* Tag Unit Data */   
	//sccp_message [iterator + 1] = SCCP_MSG_CLASS_MGMT; 
	//int tcap_message_length;
	sccp_message [iterator + 0] = 0x0a;//TAG_SCCP_UNITDATA;/* Tag Unit Data */   
        sccp_message [iterator + 1] = 0x02;//SCCP_MSG_CLASS_MGMT; 

	/**** next fields depend on number of parameters ****/

	/* Pointer to first mandatory Parameter */
	sccp_message [iterator + 2] = 0x03;
	/* Pointer to sencond mandatory Parameter*/	
	sccp_message [iterator + 3] = 0x0e;
	/* Pointer to third mandatory Parameter */	
	sccp_message [iterator + 4] = 0x19;

	iterator += 4;
	/*************** First Mandatory Parameter ****************/

	/* called party address length */
	sccp_message [iterator + 1] = 0x0b;

	/* 
	 * Address Indicator: 1001 0010 -> 0x92 -> 146 (128 + 0 + 16 + 2 + 0) 
	 * Route on GT : 0x00
	 * Global Title Indicator : 0x04
	 * Subsystem Number Indicator: SSN Present 0x01
	 * Pointcode : Pointcode not present: 0x00 
	 */ 
	sccp_message [iterator + 2] = 0x92;

	/* Subsystem number: 6 (hlr) */
	sccp_message [iterator + 3] = SSN_TYPE_HLR;
 
	/*
	 * Global Title : 9 bytes  
	 *
	 * Translation type: 0x00
	 *
	 * Numbering plan: ISDN/telephony 0x01
	 * Encoding scheme: BCD, even number of digits 0x02
	 * Nature of address indicator: International number 0x04
	 * 
	 */
	sccp_message [iterator + 4] = 0x00;
	sccp_message [iterator + 5] = 0x12;
	sccp_message [iterator + 6] = 0x04, 

	/*
	 * Address Information (digits) : 919969679389
	 *
	 * Coutry code (byte rotated): India 0x19 
	 * number (each byte rotated): 0x99, 0x96, 0x76, 0x39, 0x98, 
	 */
	encode_number_or_time (called_pty_GT, 
			&sccp_message[iterator + 7], strlen ((char *)(called_pty_GT)));	

	/*************** Second Mandatory Parameter ****************/

	iterator += 1 + strlen ((char *)(called_pty_GT));
	/* calling party address length */ 
	sccp_message [iterator] = 0x0b;

	/* 
	 * Address Indicator: 1001 0010 -> 0x92 -> 146 (0 + 16 + 2 + 0) 
	 * Route on GT : 0x00
	 * Global Title Indicator : 0x04
	 * Subsystem Number Indicator: SSN Present 0x01
	 * Pointcode : Pointcode not present: 0x00
	 */
	sccp_message [iterator + 1] = 0x12; 

	/* Subsystem number: 8 (msc) */
	sccp_message [iterator + 2] = SSN_TYPE_MSC; 
 
	/*
	 * Global Title : 9 bytes  
	 *
	 * Translation type: 0x00
	 *
	 * Numbering plan: ISDN/telephony 0x01
	 * Encoding scheme: BCD, even number of digits 0x02
	 * Nature of address indicator: International number 0x04
	 * 
	 */
	sccp_message [iterator + 3] = 0x00;
	sccp_message [iterator + 4] = 0x12;
	sccp_message [iterator + 5] = 0x04;
	//iterator = iterator + 6 ;
	/*
	 * Address Information (digits) : 919869299992
	 *
	 * Coutry code (byte rotated): India 0x19 
	 * number (each byte rotated): 0x99, 0x96, 0x76, 0x39, 0x98, 
	 */
	encode_number_or_time (calling_pty_GT, &sccp_message[iterator + 6], 
			strlen ((char *)(calling_pty_GT)));	
	iterator += 12;
	return iterator;
}


/*************************************************************************************
* Function : create_sccp_header()
* Input    : sccp_message
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sccp_header (unsigned char *sccp_message)
{
	return 0;
}

