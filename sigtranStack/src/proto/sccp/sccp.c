 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines sccp layer
 *	      and has definitions for:
 *           - create sccp unidata
 *           - sccp analyse 
 *           - m3ua asp handshake 
 *           - m3ua handshake 
 *           - m3ua transport
 *           - m3ua send inact 
 *           - m3ua send aspdwn 
 */

#include "sccp.h"

/*************************************************************************************
* Function : create_sccp_unitdata()
* Input    : sccp_message,called_pty_GT
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_sccp_unitdata (unsigned char *sccp_message, sccp_params_send *param, int type)
{
	int calling_pty_offset;
	int tcap_data_offset;
	int called_pty_len_offset;
	int calling_pty_len_offset;
	int len;
	int ret;
 	
	int iterator = 0;
	sccp_message [iterator++] = SCCP_MSG_TYPE_UDT;/* Tag Unit Data */   
	sccp_message [iterator++] = SCCP_MSG_CLASS; 

	/**** next fields depend on number of parameters ****/

	/* Pointer to first mandatory Parameter */
	sccp_message [iterator++] = NO_OF_POINTERS_UDT;
	
	calling_pty_offset = iterator++;
	sccp_message [calling_pty_offset] = calling_pty_offset;
	tcap_data_offset  = iterator++;
	sccp_message [tcap_data_offset] = tcap_data_offset;

	/*************** First Mandatory Parameter ****************/

	called_pty_len_offset = iterator++;
	sccp_message [called_pty_len_offset] = called_pty_len_offset;//to be filled with called pty 
																	//address length
	if ((param->called_pty.routing_indicator == ROUTE_ON_GT) && (param->called_pty.ssn_present == true)
			&& (param->called_pty.pc_present == false) && (param->called_pty.gt_present == true))
	{
		sccp_message[iterator++] = ADDRESS_IND_CLD;
	} 
		
	if (param->called_pty.pc_present)
	{
		memcpy(&sccp_message[iterator],param->called_pty.pc,2);
		iterator = iterator + 2;
	}

	if (param->called_pty.ssn_present)
	{
		sccp_message[iterator++] = param->called_pty.ssn;
	}	
	
	if (param->called_pty.gt_present)
	{
		sccp_message[iterator++] = GT_TRANSLATION_TYPE;
		
		sccp_message[iterator++] = GT_NUMBER_PLAN;
	
		sccp_message[iterator++] = GT_NATURE_ADDRESS;
		
		len = (strlen (param->called_pty.gt) > GT_LEN) ? GT_LEN : strlen (param->called_pty.gt);
		
		encode_number_or_time (param->called_pty.gt ,&sccp_message[iterator], len);
		iterator = iterator + ((len+1)/2);
	}
	sccp_message [called_pty_len_offset] = iterator - called_pty_len_offset - 1; //Length of called pty
	sccp_message [calling_pty_offset] = iterator - calling_pty_offset;

	/*************** Second Mandatory Parameter ****************/
	calling_pty_len_offset = iterator ;
	iterator = iterator + 1;
	sccp_message[calling_pty_len_offset] = calling_pty_len_offset; //will be modified in last
	
	if ((param->calling_pty.routing_indicator == ROUTE_ON_GT) && (param->calling_pty.ssn_present == true)
			&& (param->calling_pty.pc_present == false) && (param->calling_pty.gt_present == true))
	{
		sccp_message[iterator++] = ADDRESS_IND_CLG;
	} 
		
	if (param->calling_pty.pc_present)
	{
		memcpy(&sccp_message[iterator],param->calling_pty.pc,2);
		iterator = iterator + 2;
	}

	if (param->calling_pty.ssn_present)
	{
		sccp_message[iterator++] = param->calling_pty.ssn;
	}	
	
	if (param->calling_pty.gt_present)
	{
		sccp_message[iterator++] = GT_TRANSLATION_TYPE;
		
		sccp_message[iterator++] = GT_NUMBER_PLAN;
	
		sccp_message[iterator++] = GT_NATURE_ADDRESS;
		
		len = (strlen (param->called_pty.gt) > GT_LEN) ? GT_LEN : strlen (param->called_pty.gt);
		encode_number_or_time (param->calling_pty.gt ,&sccp_message[iterator], len);
		iterator = iterator + ((len+1)/2);
	}
	
	sccp_message [calling_pty_len_offset] = iterator - calling_pty_len_offset - 1;//Length of calling pty
	sccp_message [tcap_data_offset] = iterator - tcap_data_offset ;

	len =	tcap_frame (&sccp_message[iterator + 1], 
			(void*)&(param->sms_fields), type);
	sccp_message [iterator] = len;
	return (iterator + len + 1);//Length of SCCP with payload
}

/************************************************************************************
* Function : sccp_analyze()
* Input    : sccp_msg,sccp_msg_type
* Output   :  
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int sccp_analyze (unsigned char *sccp_msg, sccp_params *sccp_ret)
{
	int len;
	int iterator = 0;
	int ret;

	bool called_SSN_present = FALSE;
	bool called_PC_present = FALSE;
	bool called_GT_present = FALSE;
	bool called_route_on_GT = FALSE;
	bool called_route_on_SSN = FALSE;
	bool called_translation_type = FALSE;
	bool called_numbering_plan = FALSE;
	bool called_nature_of_addr = FALSE;
	
	int called_pty_pointer =0;
	int called_pty_offset = 0;
	int called_pty_data_len =0;

	int calling_pty_pointer =0;
	int calling_pty_offset = 0;
	int calling_pty_data_len =0;
	
	int tcap_data_pointer =0;
	int tcap_data_offset = 0;
	int tcap_data_len = 0;
	unsigned char tcap_data[MAX_TCAP_DATA] = {0};	
	
	sccp_ret->sccp_msg_type = sccp_msg[iterator++];
	sccp_ret->sccp_msg_class = sccp_msg[iterator++];
	
	if (sccp_ret->sccp_msg_type == SCCP_MSG_TYPE_UDT)
	{
		called_pty_pointer = sccp_msg[iterator++];
		called_pty_offset = iterator + (called_pty_pointer -1); //Beginning of length of called pty
		called_pty_data_len = sccp_msg[called_pty_offset];
		 	
		calling_pty_pointer = sccp_msg[iterator++];
		calling_pty_offset = iterator + (calling_pty_pointer -1); //Beginning of length of calling pty
		calling_pty_data_len = sccp_msg[calling_pty_offset];
		
		tcap_data_pointer = sccp_msg[iterator++];
		tcap_data_offset = iterator + (tcap_data_pointer - 1); //Beginning of length of tcap data
		tcap_data_len = sccp_msg[tcap_data_offset];
	}

	if ((sccp_ret->sccp_msg_type == SCCP_MSG_TYPE_XUDT) || 
			(sccp_ret->sccp_msg_type == SCCP_MSG_TYPE_LUDT))
	{	
		sccp_ret->hop_count = sccp_msg[iterator++];
		
		called_pty_pointer = sccp_msg[iterator++];
		called_pty_offset = iterator + called_pty_pointer; //Beginning of length of called pty
		called_pty_data_len = sccp_msg[called_pty_offset];
		 	
		calling_pty_pointer = sccp_msg[iterator++];
		calling_pty_offset = iterator + calling_pty_pointer; //Beginning of length of calling pty
		calling_pty_data_len = sccp_msg[calling_pty_offset];
		
		tcap_data_pointer = sccp_msg[iterator++];
		tcap_data_offset = iterator + tcap_data_pointer; //Beginning of tcap data
		tcap_data_len = sccp_msg[tcap_data_offset];
	}

	/*Extracting Called pty address parameters */
	iterator = called_pty_offset + 1;

	 if ((sccp_msg [iterator] & CHECK_FOR_PC ) == PC_PRESENT)
    {   
        called_PC_present = TRUE;
		sccp_ret->called_pty.pc_present = true;	
    }   
    if ((sccp_msg [iterator] & CHECK_FOR_SSN ) == SSN_PRESENT)
    {   
        called_SSN_present = TRUE;
		sccp_ret->called_pty.ssn_present = true;	
    }   
    if ((sccp_msg [iterator] & CHECK_FOR_GT ) != GT_NOT_PRESENT)
    {   
        called_GT_present = TRUE; //GT present
		sccp_ret->called_pty.gt_present = true;	
    }   
    
	/*Check for routing indicator */
	if ((sccp_msg [iterator] >> 6) % 2)
    {   
        called_route_on_SSN = TRUE;
		sccp_ret->called_pty.routing_indicator = ROUTE_ON_SSN;	
    } else {    
        called_route_on_GT = TRUE;
		sccp_ret->called_pty.routing_indicator = ROUTE_ON_GT;	
    }   
	
	if (( sccp_msg [iterator] & CHECK_FOR_GT_PARAMS ) == NATURE_OF_ADDR_PRESENT)
    {   
        called_nature_of_addr = TRUE;
    }
    if ((sccp_msg [iterator] & CHECK_FOR_GT_PARAMS ) == TRANSLATION_TYPE_PRESENT)
    {
        called_translation_type = TRUE;
    }
    if ((sccp_msg [iterator] & CHECK_FOR_GT_PARAMS ) == TR_NP_PRESENT)
    {
        called_translation_type = TRUE;
        called_numbering_plan = TRUE;
    }
    if ((sccp_msg [iterator] & CHECK_FOR_GT_PARAMS ) == TR_NP_NA_PRESENT)
    {
        called_translation_type = TRUE;
        called_numbering_plan = TRUE;
        called_nature_of_addr = TRUE;
    }

	iterator = iterator + 1;	
	if (called_PC_present)
    {
        memcpy( sccp_ret->called_pty.pc, &sccp_msg [iterator], 2);
        iterator = iterator + 2;
    }
    
	if (called_SSN_present)
    {
        sccp_ret->called_pty.ssn = sccp_msg[iterator++];
    }

    if (called_GT_present)
    {
        if (called_translation_type)
			iterator++;

        if (called_numbering_plan)
        	iterator++;

		if (called_nature_of_addr)
			iterator++;

        len = calling_pty_offset - iterator;//Length of GT
        
		decode_number_or_time (&sccp_msg[iterator], sccp_ret->called_pty.gt, len);
    }
	
	iterator = tcap_data_offset; //This field contains length of TCAP msg
	iterator ++; //Begining of TCAP message
		
	memcpy (tcap_data, &sccp_msg[iterator], tcap_data_len);	
	ret = parse_tcap_message (tcap_data, &sccp_ret->tcap_param);
	if (!ret)
		printf("Error in parsing");
	
	return SUCCESS;
} 	
