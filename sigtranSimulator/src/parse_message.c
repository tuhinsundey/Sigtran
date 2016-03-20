
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
 *
 * Purpose: This code-file defines the SUA Upper layer  Part application and  
 *          has definitions for:
 *          - Do pingpong with remote Upper layer part
 *          - Handle pong
 *          - initialise the Upper layer part application 
 *          - Upper layer Part STDIN function -> reading keyboard and 
 *            sending the info to SUA
 *          - Handle expired timers
 *          - Connectionless Data Ind Notification(from SUA)
 *          - CO Connection Ind Notification(from SUA)
 *          - CO Connection Confirmation Ind Notification(from SUA)
 *          - CO Data Ind Notification(from SUA)
 *          - CO DisConnection Ind Notification(from SUA)
 */
#include <stdio.h>
#include "util.h"
#include "iniparser.h"
#include "dictionary.h"
#include <mysql/mysql.h>
#include <string.h>
#include "parse_message.h"
#include "tcap.h"
#define MO_FORWARD_ACK          0x00    


#include <sys/time.h>

extern char own_hostname[512];

static  MYSQL	*msqlHandle		= NULL;
static	char mysql_server[30]		=  {0};
static	char mysql_username[30]		=  {0};
static	char mysql_password[30]		=  {0};
static	char mysql_db[30]		=  {0};
static	unsigned int mysql_port_val	=   0;

/*************************************************************************************
  * Function : read_database_configuration()
  * Output   : None
  * Purpose  : To read database information  from sig.ini file
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
void read_database_configuration ()
{
	char	*get_conf	= NULL;
	char	ini_name[30] 	= "config.ini";
	char	local_name[30]	= {0};
	dictionary *ini;
     
	/* pass file name to iniparser_load */
	ini = iniparser_load (ini_name);

	/* read database port */
	sprintf (local_name, "%s", "database:db_port");
	mysql_port_val = iniparser_getint (ini, local_name, 0); 
	log_msg (LOG_INFO, "The mysql port value is %d\n", 
				mysql_port_val);

	/* read database username */ 	
	sprintf(local_name, "%s", "database:username");
	get_conf = iniparser_getstring(ini, local_name, NULL);                      
	memcpy(mysql_username, get_conf, strlen(get_conf));
	printf( "The mysql_username is %s\n",	mysql_username);

	/* read database password */	
	sprintf(local_name, "%s", "database:password");
	get_conf = iniparser_getstring(ini, local_name, NULL);  
	memcpy(mysql_password, get_conf, strlen(get_conf));
	printf( "The mysql_password is %s\n",
		   	mysql_password);
	
	/* read database name */
	sprintf(local_name, "%s", "database:database");
	get_conf = iniparser_getstring(ini, local_name, NULL);  
	memcpy(mysql_db, get_conf, strlen(get_conf));
	printf( "The mysql_database is %s\n", mysql_db);

	/* read database address */
	sprintf(local_name, "%s", "database:db_server");
	get_conf = iniparser_getstring(ini, local_name, NULL);  
	memcpy(mysql_server, get_conf, strlen(get_conf));
	printf( "The mysql_server is %s\n", mysql_server);
}


/*************************************************************************************
  * Function : init_mysql()
  * Input    : None
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
void init_msqlHandle ()
{
	msqlHandle = mysql_init (NULL);
	
	if (!msqlHandle) {    
		printf( "Parse_message: MYSQL returned error : %s \n",
			   	mysql_error (msqlHandle));
	}     

	if (!(mysql_real_connect (msqlHandle, mysql_server, mysql_username,
			mysql_password, mysql_db, mysql_port_val, NULL, 
			CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS))) {    
	 	printf( "Parse_message: MYSQL Error : [%s] \n", mysql_error (msqlHandle));
	 	mysql_close (msqlHandle);
		msqlHandle = NULL;
	}
	else
		printf( "Parse_message:MsqlHandle connection established successfully\n");
}


/*************************************************************************************
  * Function : gsmsc_tcap_query ()
  * Input    : queryString
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
int gsmsc_tcap_query (char *queryString)
{
	if (msqlHandle) {	
		if ((mysql_real_query 
			(msqlHandle, queryString, strlen(queryString)))) {
			log_msg (LOG_ERR, 
				"Parse_message: gsmsc_mysql_connect_execute(): ERROR %s", 
				mysql_error (msqlHandle));
			//usleep (100);
			mysql_close (msqlHandle);
			//usleep (100);
			msqlHandle = NULL;
			init_msqlHandle ();
			return 0;
		}   
		return 1;
	}
	init_msqlHandle ();
	//printf( "Mysql Handle cannot initialise");
   	return 0;
}


/*************************************************************************************
  * Function : get_receiver()
  * Input    : None
  * Output   : None
  * Purpose  : To get receiver from database
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
char *get_receiver()
{
	MYSQL_ROW row ;
	MYSQL_RES *res = NULL;
	char query[200] = {0};
	char *receiver = NULL; 

	sprintf(query, "%s", "CALL tt");
	gsmsc_tcap_query(query);
	res = mysql_store_result(msqlHandle);
	if(res){
		row = mysql_fetch_row(res);
		if(res)
			mysql_free_result(res);
		if(row){
			if(row[0]){					
				receiver = (char *)malloc(20);
				memset(receiver, 0, 20);
				strncpy(receiver, (char *) row[0], strlen(row[0]));		
			}else
				return NULL;
		}	
	}
	log_msg (LOG_INFO, "reciever value: %s\n", receiver);
	return receiver;
}


/*************************************************************************************
  * Function : get_IMSI ()
  * Input    : MSISDN	
  * Output   : None
  * Purpose  : To get IMSI 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
char *get_IMSI(char * MSISDN)
{
    MYSQL_ROW row ;
    MYSQL_RES *res = NULL;
    char query[200] = {0};
    char msisdn[12] = {0};
    char *IMSI = NULL;

    memset(msisdn,0,12);
    memcpy (msisdn, MSISDN, 12);
    sprintf (query, "select IMSI from hlr where receiver = '%s';", MSISDN);

    if (!gsmsc_tcap_query(query))
        return NULL;
    res = mysql_store_result(msqlHandle);
    if(res){
        row = mysql_fetch_row(res);
        if(row){
            if(row[0]){
                IMSI = (char *)malloc(20);
                memset(IMSI, 0, 20);
                memcpy(IMSI, row[0], strlen(row[0]));
                mysql_free_result(res);
            }else{
                mysql_free_result(res);
                return NULL;
            }
        }
    }

    printf("IN PARSE_MESSAGE :  DA_IMSI = %s\n", IMSI);
    return IMSI;
}


/*************************************************************************************
  * Function : convert_hex_to_pc ()
  * Input    : m3ua_msg_pc_ptr,ptr
  * Output   : None
  * Purpose  : 
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
void convert_hex_to_pc (unsigned char *ptr, char *m3ua_msg_pc_ptr)
{
	unsigned char msb_byte ;
	unsigned char middle_byte ;
	unsigned char lsb_byte ;
	unsigned char temp_byte;
	
	temp_byte = (unsigned char) m3ua_msg_pc_ptr[2];
	msb_byte = temp_byte >> 3;
	temp_byte = m3ua_msg_pc_ptr[2];
	middle_byte = temp_byte << 5;
	temp_byte = (unsigned char) m3ua_msg_pc_ptr[3];
	temp_byte = temp_byte >> 3;
	middle_byte = middle_byte | temp_byte;
	temp_byte = (unsigned char) m3ua_msg_pc_ptr[3];
	lsb_byte = temp_byte & 7;	

	sprintf ((char*)ptr, "%d-%d-%d", (msb_byte), (middle_byte), (lsb_byte));	
}


/*************************************************************************************
  * Function : parse_sri_ack_packet()
  * Input    : m3ua_message,IMSI
  * Output   : None
  * Purpose  : Parse_SRI_ack_packet
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
void parse_sri_ack_packet( char *m3ua_message, char *imsi)
{
	char my_databuffer[200] = {0};	
	//unsigned char imsi[9]   	= {0};
	unsigned char temp_imsi[16] 	= {0};
	int imsi_len			= 0;
	
	if( m3ua_message[54] == 0x64 && (((m3ua_message[53])-(m3ua_message[56])==0x03) || ((m3ua_message[53])-(m3ua_message[55]) == 0x02))) {	
		memset(my_databuffer, 0, sizeof(my_databuffer));	
		memcpy(my_databuffer, &m3ua_message[54], (int)m3ua_message[53]);
		
		/*
		 *To get IMSI from SRI_ACK
		 */	
		if (TAG_GSM_TCAP_END == ((unsigned char) my_databuffer[0]) && 
				TAG_SEND_ROUT_INFO  == (unsigned char) my_databuffer [59]) {
			imsi_len = my_databuffer[63];		
			memcpy(temp_imsi, &my_databuffer[64], imsi_len);
		        decode_number_or_time (temp_imsi, (unsigned char *)imsi, imsi_len);
	 	      	printf ("\nIN_PARSE_MESSAGE : = The imsi value is %s\n", imsi);
		}		
	}
}



/*************************************************************************************
  * Function : parse_message()
  * Input    : m3ua_message, trx_id
  * Output   : None
  * Purpose  : Parse the received packet
  * Author   : (tuhin.shankar.dey@gmail.com)
  * Note     : 
**************************************************************************************/
int parse_message(char *m3ua_message, unsigned int *trx_id)
{
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	int gsm_msg_len 		= 0;
  	char *my_text			= NULL;
	int iterator			= 0;
	int imsi_len			= 0;
	int service_center_address_len 	= 0;
	int msisdn_len 			= 0;
	unsigned int tcap_msg_len 	= 0;
	char query[320] 		= {0};	
	unsigned char imsi[9]   	= {0};
	unsigned char temp_imsi[16] 	= {0};
	unsigned char originating_address[9] 		  	= {0};
	unsigned char temp_originating_address[16] 	  	= {0};
	unsigned char service_center_address[9] 	  	= {0};
	unsigned char temp_service_center_address[16] 		= {0};
	unsigned char msisdn[9]		={0};
	unsigned char temp_msisdn[16]	= {0};
	unsigned char transaction_id[30]= {0};
	char global_msg_id[30] 		= {0};
	int i				= 0;
	//int ret 			= 0;
  	int index = 0;

	struct timeval tv;
        struct timezone tz;
        struct tm *tm;
        time_t epoch_time;	
	char in_time[30] = {0};
	
	unsigned char opc[20] = {0};
	unsigned char dpc[20] = {0};
	char my_databuffer[200] = {0};	

	convert_hex_to_pc(opc, &m3ua_message[12]);
	convert_hex_to_pc(dpc, &m3ua_message[16]);
	/* must give following condition */
	/*use variable except hard coded values*/
	if((m3ua_message[62] == 0x62 || m3ua_message[62] == 0x64) && (((m3ua_message[61])-(m3ua_message[64])==0x03) || ((m3ua_message[61])-(m3ua_message[63]) == 0x02))) 	{	
		memset(my_databuffer, 0, sizeof(my_databuffer));	
		memcpy(my_databuffer, &m3ua_message[62], (int)m3ua_message[61]);
	
		if (TAG_GSM_TCAP_BEGIN == ((unsigned char) my_databuffer[0]) && 
				TAG_SEND_ROUT_INFO  == (unsigned char) my_databuffer [66]) {
			i = 0;
			memset(transaction_id, 0, 30);
			for(iterator=7; iterator > 3; iterator--)
			{
				transaction_id[i] = my_databuffer[iterator];
				i++;
			}
			transaction_id[i]='\0';	
			memset(global_msg_id, 0, 30);
			sprintf (global_msg_id, "47672%d", *(unsigned int*) transaction_id);
			
			*trx_id = (*(unsigned int*)transaction_id);	
			msisdn_len = my_databuffer[78]; 
	        	index = 80;
        		for(iterator=0; iterator < msisdn_len -1; iterator++)
        		{   
				msisdn[iterator] = my_databuffer[index + iterator];
        		}   
        		decode_number_or_time(msisdn, temp_msisdn, msisdn_len - 1);
		
			epoch_time = time(NULL);
        	        tm = gmtime(&epoch_time);

        	        gettimeofday(&tv, &tz);
        	        //tm = (struct tm *)localtime(&tv.tv_sec);
			memset(in_time, 0, strlen(in_time));
			sprintf(in_time, "%s", asctime(tm));
        	       /* sprintf(in_time,"%02d%02d%02d%02d%02d%02d%06ld", tm->tm_year % 100,
				tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
				tm->tm_min,tm->tm_sec,tv.tv_usec);*/
			i = 0;
			while(temp_msisdn[i])
			{
				if(temp_msisdn[i] == 0x3f){
					temp_msisdn[i] = '\0'; 
					break;
				}
				i++;
			}
		
			sprintf(query, "select imsi from hlr where receiver = '%s'", temp_msisdn);
			gsmsc_tcap_query (query);
				
			res = mysql_store_result(msqlHandle);		 	
			row = mysql_fetch_row(res);
			char imsi_receiver[30] = {0};
			memcpy(imsi_receiver, row[0], strlen(row[0]));	


			//sprintff (query, "INSERT into smsc_send_sms (opcode, global_msg_id, arrv_time, receiver) values ('%d','%s','%s','%s')", 
			sprintf (query, "INSERT into smsc_sent_sms (global_msg_id, in_time, receiver, imsi, opcode) values ('%s','%s','%s','%s','1')", 
						global_msg_id, in_time, temp_msisdn, imsi_receiver);

			gsmsc_tcap_query (query);
			log_msg (LOG_INFO, "The query is  %s", query);
			res = NULL;
			res = mysql_store_result(msqlHandle);
			if(res) {
				mysql_free_result(res);
			}
			log_msg (LOG_INFO, "The msisdn is  %s", temp_msisdn);
			return TAG_SEND_ROUT_INFO;
		}

		/*
		 * check if the message is 
		 * of type MT-ForwardSM
		 */	
		else if (MT_FORWARD_SM == ((unsigned char) my_databuffer [48]) && 
				TAG_GSM_TCAP_BEGIN == (unsigned char) my_databuffer [0]) {
			my_text = (char *) malloc (160);
			if (!my_text)
  				memset (my_text, 0, 160);
			if (LARGE_MSG_LEN_IDF == tcap_msg_len || 0 > my_databuffer [1])	{
				tcap_msg_len = (unsigned char) my_databuffer [2];
				index = tcap_msg_len + 2;
			}
			else {
				tcap_msg_len = (unsigned char) my_databuffer [1];
				index = tcap_msg_len + 3;
			}
		
			gsm_msg_len = 0;
			for (;index >= 1; --index)
  			{
   			 	if ((my_databuffer[index] == gsm_msg_len) &&
						!my_databuffer[index - 1])
   		 			break;
		     		++gsm_msg_len;
  			}
		
			/* Decoding Message from MT/MO forward SM */
			char *input = &my_databuffer[index+1];
			char *output = my_text;
			gsm7_to_ascii (input, output);
			//gsm7_to_ascii (&my_databuffer [index + 1], my_text);
			
			for (iterator = 8; iterator > 4; --iterator)
			{   
				transaction_id [i] = my_databuffer [iterator];
				i++;
			}
	 		transaction_id [i] = '\0';	
			*trx_id = *(unsigned int *)transaction_id;
	 	 
			/* 
			 * Decoding IMSI from MT forward_SM 
			 */
			index = 53;
			imsi_len = my_databuffer [index];
			if ((unsigned char ) my_databuffer [index - 1] == 0x80) {
				index = index + 1;
				for(iterator = 0; iterator < imsi_len; iterator++)
            			{
            				imsi [iterator] = my_databuffer [index + iterator];
            			}
            			imsi [iterator] ='\0';
		            	decode_number_or_time (imsi, temp_imsi, imsi_len);
	 	      	     	printf ("The imsi value is %s", temp_imsi);
       			     	index = index + imsi_len;
       		 	}
       	 		else {
		        	index += 2;
				for (iterator = 0; iterator < (imsi_len - 1); iterator++) 
            			{
					imsi [iterator] = my_databuffer [index + iterator];
        	    		}
        	    		imsi [iterator] ='\0';
       	     			decode_number_or_time (imsi, temp_imsi, imsi_len -1 );
       	    	 		printf ("The imsi value is %s", temp_imsi);
       	     			index += imsi_len - 1;
       		 	}	
	
			//Decoding Service Center Address from MT forward_SM
	
			service_center_address_len =  my_databuffer[index + 1];
		       	index += 3;
		        
			for(iterator=0; iterator < service_center_address_len - 1; iterator++)
		        {
				service_center_address[iterator] = my_databuffer[ index + iterator];
			}
			
			service_center_address[iterator]='\0';
			decode_number_or_time (service_center_address, 
			temp_service_center_address, (service_center_address_len - 1));
			printf ("The service_center_address value is %s", 
				temp_service_center_address);
	
			//Decoding Originating Address from MT forward_SM
			index = index + service_center_address_len + 3;
			imsi_len = my_databuffer [index];
			index = index + 2;
			
			for(iterator=0; iterator < (imsi_len + 1)/2; iterator++)
			{
				originating_address [iterator] = my_databuffer [index + iterator];
			}
			
			originating_address [iterator] = '\0';
			decode_number_or_time(originating_address, temp_originating_address,
				 (imsi_len +1)/2);
			printf("The orinating address value is %s", temp_originating_address);
			memset(query, 0, 320);
			sprintf (query, 
				"INSERT into smsc_sent_sms (sender, receiver, msgdata) 	values('%s','%s','%s')", 
				temp_originating_address, temp_imsi, my_text);
	
			gsmsc_tcap_query (query);
			res = NULL;
			res = mysql_store_result(msqlHandle);
			if(res) {
				mysql_free_result(res);
	
			}
			return MT_FORWARD_SM; 
		}
		else if (MO_FORWARD_SM == ((unsigned char) my_databuffer[48]) &&
				 TAG_GSM_TCAP_BEGIN == (unsigned char) my_databuffer [0]) {
			printf("IN MO CASE");
				my_text = (char *) malloc (160);
  			if (!my_text)
  				memset (my_text, 0, 160);
			if (LARGE_MSG_LEN_IDF == tcap_msg_len || 0 > my_databuffer [1]) {
				tcap_msg_len = (unsigned char) my_databuffer [2];
				index = tcap_msg_len + 2;
			}
			else {
				tcap_msg_len = (unsigned char) my_databuffer [1];
				index = tcap_msg_len + 3;
			}
  		
			for (index = tcap_msg_len + 2; index >= 1; --index)
  			{
				printf("%x\t", my_databuffer[index]);
   			 	if ((my_databuffer[index] == gsm_msg_len) &&
						!my_databuffer[index - 1])
   			 		break;
	  	 	  	++gsm_msg_len;
  			}
			//Decoding Message from MT/MO forward SM
			char *input = &my_databuffer[index+1];
			char *output = my_text;
			gsm7_to_ascii (input, output);
	
			i=0;	
			for(iterator =8; iterator>4; iterator--)
		   	{   
				transaction_id[i] = my_databuffer[iterator];
				i++;
			}   
		 	transaction_id[i]='\0';	
       		 	*trx_id = *(unsigned int *)transaction_id;
			memset (global_msg_id, 0, 30);
			sprintf (global_msg_id, "47672%d", *trx_id);
	
			//Decoding Service Center Address from MO forward_sm
			index = 53;
			service_center_address_len = my_databuffer [index];
			printf ("The legth of service center address is %d\n", 
				service_center_address_len);
			if ((unsigned char) my_databuffer [index - 1] == 0x80) {
				++index;
				for (iterator=0; iterator < service_center_address_len; iterator++)
				{
					service_center_address [iterator] = my_databuffer [index+iterator];
				}
				decode_number_or_time (service_center_address, 
					temp_service_center_address, service_center_address_len);
				printf("The service center address is %s\n", temp_service_center_address);
				index += service_center_address_len;
			}
			else {
				index += 2;
				for (iterator=0; 
					iterator < (service_center_address_len - 1); iterator++)
				{
					service_center_address[iterator] = my_databuffer[index+iterator];
				}
				decode_number_or_time (service_center_address, temp_service_center_address,
					service_center_address_len -1 );
				printf ("The service center address is %s\n", 
					temp_service_center_address);
				index = index + service_center_address_len -1 ;
			}
	
			//Decoding MSISDN from MO forward_sm	
			msisdn_len = my_databuffer[index + 1];
			index += 3;
			for(iterator=0; iterator < msisdn_len -1 ; iterator++)
			{
				msisdn[iterator] = my_databuffer[index + iterator];
			}
			decode_number_or_time (msisdn, temp_msisdn,msisdn_len - 1);
			
			printf ("The msisdn is %s\n", temp_msisdn);
			index = index + msisdn_len + 4;
			//Decoding  Originating Address from MO forward_sm
			imsi_len = my_databuffer[index];
			index = index + 2;
			for(iterator=0; iterator < ((imsi_len + 1)/2 ); iterator++)
			{
				originating_address [iterator] = 
					my_databuffer [index + iterator];
			}
			decode_number_or_time (originating_address, 
					temp_originating_address, (imsi_len + 1)/2);
			printf ("The originating address is %s\n", temp_originating_address);
			epoch_time = time(NULL);
			tm = gmtime(&epoch_time);
			gettimeofday(&tv, &tz);
			//tm = (struct tm *)localtime(&tv.tv_sec);
			memset(in_time, 0, strlen(in_time));
			sprintf(in_time, "%s", asctime(tm));
		/*	sprintf(in_time, "%02d%02d%02d%02d%02d%02d%06ld", tm->tm_year % 100, tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,tv.tv_usec);*/
			memset (query, 0, 320);

			sms_fields sms_entry;

			strcpy (sms_entry.global_msg_id, global_msg_id);
			strcpy (sms_entry.sms_data, my_text);
			strcpy ((char *)sms_entry.SM_RP_DA.number, 
				(char *)temp_msisdn);
			strcpy ((char *)sms_entry.SM_RP_OA.number, 
				(char *)temp_originating_address);

			if (!strncmp ((char *)temp_originating_address, "91", 2))
				insert (&sms_entry);		
			else 
				printf ("received number %s not meant for india termination", 
				temp_originating_address);

			sprintf (query, 
				"INSERT into smsc_sent_sms (sender, receiver, msgdata, global_msg_id, in_time, opcode) values ('%s','%s','%s','%s','%s','2')",
			   	temp_msisdn, temp_originating_address, my_text, global_msg_id, in_time);	
			log_msg (LOG_INFO, "executing mysql query\n%s\n", query);	
	
			gsmsc_tcap_query (query);
			res = NULL;
			res = mysql_store_result(msqlHandle);
			if(res) {
				mysql_free_result(res);
	
			}
			return MO_FORWARD_SM; 
		}
		else if (TAG_GSM_TCAP_CONTINUE == ((unsigned char) my_databuffer[0])) {
			i=0;
			memset(transaction_id, 0, 30);
			
			for(iterator = 7; iterator>3; iterator--)
			{
				transaction_id[i] = my_databuffer[iterator];
				i++;
			}
		 	transaction_id[i]='\0';	
			printf("The transaction id is %d",*(unsigned int *)transaction_id);
			
			memset(global_msg_id, 0, 30);
			sprintf(global_msg_id, "47672%d", *(unsigned int*)transaction_id);
			
			memset(query, 0, 320);
			sprintf (query, 
				"UPDATE smsc_send_sms set process = 3 where global_msg_id = '%s'",
			   	global_msg_id);	
			printf("The query is %s",query);
	
			if (!msqlHandle) {
				init_msqlHandle ();
			}
			
			gsmsc_tcap_query (query);
			res = NULL;
			res = mysql_store_result(msqlHandle);
			if(res) {
				mysql_free_result(res);
	
			}
			return TAG_GSM_TCAP_CONTINUE;
		}
		else if (TAG_GSM_TCAP_END == ((unsigned char) my_databuffer[0])) {
			i = 0;
			memset(transaction_id, 0, 30);
			
			for(iterator=7; iterator > 3; iterator--)
			{
				transaction_id[i] = my_databuffer[iterator];
				i++;
			}
		 	transaction_id[i]='\0';	
			memset(global_msg_id, 0, 30);
			sprintf(global_msg_id, "47672%d", *(unsigned int*)transaction_id);
				
			memset(query, 0, 320);
			sprintf (query, 
				"UPDATE smsc_send_sms set process = 4 where global_msg_id = '%s' and process = 3", 
				global_msg_id);	
			printf("The query is %s",query);
			gsmsc_tcap_query (query);
			res = NULL;
			res = mysql_store_result (msqlHandle);
			if(res) {
				mysql_free_result (res);
			}
			return TAG_GSM_TCAP_END;
		}
	}
	return 0;
}















