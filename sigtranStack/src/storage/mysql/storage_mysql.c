 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines storage information 
 *	      for mysql and has definitions for:
 *           - init mysql 
 *           - insert mysql table
 *           - append mysql table 
 *           - delete mysql table 
 *           - select mysql table 
 */

#include "storage_memcached.h"
#include "storage.h"
#include "storage_mysql.h"

MYSQL *msqlHandle = NULL;

/*************************************************************************************
* Function : init_mysql()
* Input    : storage_values
* Output   : 
* Purpose  : initialize mysql
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int init_mysql (void *storage_values)
{
	int ret = 0;
	storage_params *parameters = NULL;
	parameters = (storage_params *)storage_values;	
	
	msqlHandle = mysql_init (NULL);
	
	if (!msqlHandle) {
		printf("Parse_message: MYSQL returned error : %s \n",
			mysql_error (msqlHandle));
	}

	if (!(mysql_real_connect (msqlHandle, parameters-> server, parameters->username,
			parameters->password, parameters->database, parameters->port, NULL,
			CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS))) {
		printf("Parse_message: MYSQL Error : [%s] \n", mysql_error (msqlHandle));
		mysql_close (msqlHandle);
		msqlHandle = NULL;
	}
	else{
		ret = 1;
		printf("Parse_message:MsqlHandle connection established successfully\n");
	}	
	
	return ret;
}


/*************************************************************************************
* Function : insert_mysql_table()
* Input    : params
* Output   : 
* Purpose  : insert into mysql table
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int insert_mysql_table (void *params)
{
	int ret 			= 0;
	int iterator 			= 0;	
	char token_names[20][20]	= {0};
	char token_values[20][20] 	= {0};
	char primary_key_value[20] 	= {0};
	char table_indicator 		= 0;
	char query[300] 		= {0};
	char table_name[30] 		= {0};
	char str[]			= {'#','\0'};
	int table_ind 			= 0;
	
	parse_params ((void*)params, (char *)&token_names, (char *)&token_values, primary_key_value, &table_indicator);

	table_ind = table_indicator - ASCII_ZERO;
	switch (table_ind)
	{
		case 1:	
			sprintf(table_name, "%s", "smsc_send_sms");
			break;
		case 2:
			sprintf(table_name, "%s", "smsc_sent_sms");
			break;
		case 3:
			sprintf(table_name, "%s", "smsc_deliver_sms");
			break;
	}

	iterator = 0;
	sprintf(query, "INSERT into %s(%s", table_name, token_names[iterator++]);

	while(!strcmp(token_names[iterator], str) == 0)
	{
		sprintf(query, "%s,%s", query, token_names[iterator++]);
	}

	iterator = 0;
	sprintf(query, "%s)values(%s", query, token_values[iterator++]);

	while(!strcmp(token_names[iterator], str) == 0)
	{
		sprintf(query, "%s,%s", query, token_values[iterator++]);
	}
	sprintf(query, "%s);",query);	
	mysql_real_query(msqlHandle, query, strlen(query));
}
	
/*************************************************************************************
* Function : update_mysql_table()
* Input    : params
* Output   : 
* Purpose  : update mysql table
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int update_mysql_table (void *params)
{
	int  ret			= 0;
	int  iterator			= 0;	
	char query[300]			= {0};
	char token_names[20][20] 	= {0};
	char token_values[20][20] 	= {0};
	char primary_key_value[20] 	= {0};
	char table_indicator		= 0;
	char table_name[30]		= {0};	
	int table_ind			= 0;	
	char str[]			= {'#','\0'};	

	parse_params ((void *)params, &token_names, &token_values, &primary_key_value, &table_indicator);
	iterator = 0;
	
	table_ind = table_indicator - ASCII_ZERO;

	switch (table_ind)
	{
		case 1:	
			sprintf(table_name, "%s", "smsc_send_sms");
			break;
		case 2:
			sprintf(table_name, "%s", "smsc_sent_sms");
			break;
		case 3:
			sprintf(table_name, "%s", "smsc_deliver_sms");
			break;
	}

	sprintf(query, "UPDATE %s set", table_name);	
	while(1)
	{
		sprintf(query,"%s %s=%s", query, token_names[iterator], token_values[iterator]);	
		iterator++;	
		if(!strcmp(token_names[iterator], str) == 0)
			break;
		else	
			sprintf(query,"%s,", query);	
	}
	
	sprintf(query,"%s where global_msg_id = %s;", query, primary_key_value);	
	mysql_real_query(msqlHandle, query, strlen(query));
	
	return ret;	
}

/*************************************************************************************
* Function : delete_mysql_table()
* Input    : params
* Output   : 
* Purpose  : delete row from mysql table
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int delete_mysql_table (void *params)
{
	int ret				= 0;
	int iterator			= 0;
	char query[300]			= {0};
	char token_names[20][20] 	= {0};
	char token_values[20][20] 	= {0};
	char primary_key_value[20] 	= {0};
	char table_indicator		= 0;
	char table_name[30]		= {0};
	int table_ind			= 0;
	
	parse_params ((void *)params, (char *)&token_names, (char *)&token_values, primary_key_value, &table_indicator);
	
	table_ind = table_indicator - ASCII_ZERO;	
	switch (table_ind)
	{
		case 1:	
			sprintf(table_name, "%s", "smsc_send_sms");
			break;
		case 2:
			sprintf(table_name, "%s", "smsc_sent_sms");
			break;
		case 3:
			sprintf(table_name, "%s", "smsc_deliver_sms");
			break;
	}

	sprintf(query, "DELETE from %s where global_msg_id = %s;",table_name, primary_key_value);
	mysql_real_query(msqlHandle, query, strlen(query));

	return ret;	
}

/*************************************************************************************
* Function : select_mysql_table()
* Input    : params
* Output   : 
* Purpose  : select values from mysql table
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int select_mysql_table (void *params)
{
	int ret 			= 0;
	int iterator			= 0;
	char query[300]			= {0};
	char token_names[20][20] 	= {0};
	char token_values[20][20] 	= {0};
	char primary_key_value[20] 	= {0};
	char table_name[30]		= {0};
	char table_indicator 		= 0;
	int table_ind			= 0;
	char str[]			= {'#','\0'};	

	parse_params ((void*)params, (char *)&token_names, (char *)&token_values, primary_key_value, &table_indicator);
	
	sprintf(query, "SELECT %s", token_names[iterator++]);	
	
	table_ind = table_indicator - ASCII_ZERO;
	switch (table_ind)
	{
		case 1:	
			sprintf(table_name, "%s", "smsc_send_sms");
			break;
		case 2:
			sprintf(table_name, "%s", "smsc_sent_sms");
			break;
		case 3:
			sprintf(table_name, "%s", "smsc_deliver_sms");
			break;
	}

	while(!strcmp(token_names[iterator], str) == 0)
	{
		sprintf(query, "%s,%s", query, token_names[iterator++]);
	}
		
	sprintf(query, "%s from %s", query, table_name);
	
	mysql_real_query(msqlHandle, query, strlen(query));
	return ret;	
}

