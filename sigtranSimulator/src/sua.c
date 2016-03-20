#include "common.h"
#include "sua.h"


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
* Function : create_sua_CO()
* Input    : sig
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sua_CO (unsigned char *sua_message, 
			char message_type)
{
	return 0;
}

/*************************************************************************************
* Function : create_sua_SNM()
* Input    : sig
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sua_SNM (unsigned char *sua_message, 
			char message_type)
{
	return 0;
}

/*************************************************************************************
* Function : create_sua_ASPTM()
* Input    : sig
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sua_ASPSM (unsigned char *sua_message, 
			char message_type)
{
	int sua_length;
	sua_message[version] = SUA_VERSION_1;
	sua_message[reserved] = SUA_RESERVED_BYTE;
	sua_message[message_class] = SUA_MSG_CLASS_ASPSM;
	
	switch (message_type)
	{
		case SUA_MSG_TYPE_ASPSM_UP:
    		sua_message[3] = SUA_MSG_TYPE_ASPSM_UP;
			break;
     	case SUA_MSG_TYPE_ASPSM_DOWN:
    		sua_message[3] = SUA_MSG_TYPE_ASPSM_DOWN;
			break;
     	case SUA_MSG_TYPE_ASPSM_HTBT:
    		sua_message[3] = SUA_MSG_TYPE_ASPSM_HTBT;
			break;
     	case SUA_MSG_TYPE_ASPSM_UPACK:
    		sua_message[3] = SUA_MSG_TYPE_ASPSM_UPACK;
			break;
     	case SUA_MSG_TYPE_ASPSM_DWNACK:
    		sua_message[3] = SUA_MSG_TYPE_ASPSM_DWNACK;
			break;
     	case SUA_MSG_TYPE_ASPSM_HBTACK:
    		sua_message[3] = SUA_MSG_TYPE_ASPSM_HBTACK;
			break;
	}
	memset (&sua_message[4], 0, reserved_bytes3);

	sua_length		= ASPSM_MSG_LEN;
	sua_message[7]	= sua_length;
	return sua_length;
}

/*************************************************************************************
* Function : create_sua_ASPTM()
* Input    : sig
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sua_ASPTM (unsigned char *sua_message, 
			char message_type)
{
	int sua_length;
	sua_message[version] = SUA_VERSION_1;
	sua_message[reserved] = SUA_RESERVED_BYTE;
	sua_message[message_class] = SUA_MSG_CLASS_ASPTM;
	
	switch (message_type)
	{
		case SUA_MSG_TYPE_ASPTM_ACT:
			sua_message[3] = SUA_MSG_TYPE_ASPTM_ACT;
			break;
     	case SUA_MSG_TYPE_ASPTM_INACT:
			sua_message[3] = SUA_MSG_TYPE_ASPTM_INACT;
			break;
		case SUA_MSG_TYPE_ASPTM_ACTACK:
			sua_message[3] = SUA_MSG_TYPE_ASPTM_ACTACK;
			break;
		case SUA_MSG_TYPE_ASPTM_INACTACK:
			sua_message[3] = SUA_MSG_TYPE_ASPTM_INACTACK;
			break;
	}
	memset (&sua_message[4], 0, reserved_bytes3);

	/*** Routing Context ***/	
	memcpy (&sua_message[8], routing_context_tag, 2);
	memcpy (&sua_message[10], small_param_len, 2);
	memcpy (&sua_message[12], routing_context_val, 4);

	/*** Message Length ***/
	sua_length		= ASPTM_MSG_LEN;
	sua_message[7]	= sua_length;
	return sua_length;
}

/*
 * Any SUA message that has a ULP payload 
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
* Function : create_sua_CL()
* Input    : sig
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sua_CL (unsigned char *sua_message , 
			char message_type, char *dest_ip, char *src_ip)
{
	int sua_length;
	sua_message[version] = SUA_VERSION_1;
	sua_message[reserved] = SUA_RESERVED_BYTE;
	sua_message[message_class] = SUA_MSG_CLASS_CLDT;
	sua_message[3] = SUA_MSG_TYPE_CL_CLDT;
	memset (&sua_message[4], 0, reserved_bytes3);

	/*** Routing Context ***/	
	memcpy (&sua_message[8], routing_context_tag, 2);
	memcpy (&sua_message[10], small_param_len, 2);
	memcpy (&sua_message[12], routing_context_val, 4);

	/*** Protocol Class ***/
	memcpy (&sua_message[16], protocol_class_tag, 2);
	memcpy (&sua_message[18], small_param_len, 2);
	memset (&sua_message[20], 0, reserved_bytes3);		
	sua_message[23]	=	protocol_class_val;

	/*** Source Address ***/
	memcpy (&sua_message[24], source_address_tag, 2);
	memcpy (&sua_message[26], large_param_len, 2);
	memcpy (&sua_message[28], routing_indicator_tag, 2);
	memcpy (&sua_message[30], address_indicator_tag, 2);
	memcpy (&sua_message[32], subsystem_number_tag, 2);
	memcpy (&sua_message[34], small_param_len, 2);
	memset (&sua_message[36], 0, reserved_bytes3);	
	sua_message[39] = subsystem_number;	
	memcpy (&sua_message[40], IPv4_address_tag, 2);	
	memcpy (&sua_message[42], small_param_len, 2);
	inet_aton_buff (src_ip, &sua_message[44]);
	
	/*** Destination Address ***/
	memcpy (&sua_message[48], dest_address_tag, 2);
	memcpy (&sua_message[50], large_param_len, 2);
	memcpy (&sua_message[52], routing_indicator_tag, 2);
	memcpy (&sua_message[54], address_indicator_tag, 2);
	memcpy (&sua_message[56], subsystem_number_tag, 2);
	memcpy (&sua_message[58], small_param_len, 2);
	memset (&sua_message[60], 0, reserved_bytes3);	
	sua_message[63] = subsystem_number;	
	memcpy (&sua_message[64], IPv4_address_tag, 2);	
	memcpy (&sua_message[66], small_param_len, 2);
	inet_aton_buff (dest_ip, &sua_message[68]);

	/*** Sequence Control ***/
	memcpy (&sua_message[72], sequence_control_tag, 2);
	memcpy (&sua_message[74], small_param_len, 2);
	memset (&sua_message[76], 0, reserved_bytes4);		

	/*** Hop Counter ***/
	memcpy (&sua_message[80], hop_counter_tag, 2);
	memcpy (&sua_message[82], small_param_len, 2);
	memset (&sua_message[84], 0, reserved_bytes3);		
	sua_message[87]	= ss7_hop_counter_tag;

	/*** Importance ***/
	memcpy (&sua_message[88], importance_tag, 2);
	memcpy (&sua_message[90], small_param_len, 2);
	memset (&sua_message[92], 0, reserved_bytes3);		
	sua_message[95]	= importance_val;

    /*** Data headers and length specifications ***/
	memcpy (&sua_message[96], data_tag, 2);
	memcpy (&sua_message[98], min_cldt_data_len, 2);

	sua_message[7] = MIN_SUA_CLDT_LEN;
	sua_length = sua_message[7];
	return sua_length;
}
