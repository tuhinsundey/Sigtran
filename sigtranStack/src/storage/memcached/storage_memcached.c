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
 *	      for memcached and has definitions for:
 *           - parse params
 *           - init memcached 
 *           - insert memcached
 *           - get memcached 
 *           - append memcached 
 *           - delete memcached 
 *           - select memcached 
 */

#include "storage_memcached.h"
#include "storage_mysql.h"	
#include "storage.h"

memcached_server_st *MAIN_server_handle = NULL;
memcached_server_st *HLR_server_handle	= NULL;
memcached_server_st *DLR_server_handle	= NULL;

memcached_st *MAIN_server_communicator;
memcached_st *HLR_server_communicator;
memcached_st *DLR_server_communicator;

memcached_return MAIN_server_return_value;
memcached_return HLR_server_return_value;
memcached_return DLR_server_return_value;

/*************************************************************************************
* Function : parse_params()
* Input    : params, token_names, token_valued, primary_key_value, table_indicator 
* Output   : 
* Purpose  : parsing 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
void parse_params (void *params, char *token_names, char *token_values, 
		char *primary_key_value, char *table_indicator)
{
	char str[2] 		= {'#','\0'};
	char delimiter[]	= ";";
	char *compare_string 	= (char *)params;
 	char *saveptr		= NULL;

	do {
		char *temp_token_names = strtok_r(compare_string, delimiter, &saveptr);
		memcpy(token_names, temp_token_names, strlen(temp_token_names));
		token_names += 20;
		compare_string = NULL;
	} while (strcmp (str, token_names - 20));
	
	do {
		char *temp_token_values = strtok_r(compare_string, delimiter, &saveptr);
		memcpy(token_values, temp_token_values, strlen(temp_token_values));
		token_values += 20;
		compare_string = NULL;
	} while (strcmp (str, token_values - 20));
	
	char *temp_primary_key_value = strtok_r (compare_string, delimiter, &saveptr);
	memcpy(primary_key_value, temp_primary_key_value, strlen(temp_primary_key_value));
	compare_string = NULL;
	
	char *ptr = strtok_r(compare_string, delimiter, &saveptr);
	compare_string = NULL;
	char *temp_table_indicator = strtok_r(compare_string, delimiter, &saveptr);
	memcpy(table_indicator, temp_table_indicator, strlen(temp_table_indicator));
}

/*************************************************************************************
* Function : init_memcached()
* Input    : storage_values
* Output   : 
* Purpose  : initializing memcached 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int init_memcached (void *storage_values)
{
	const char program[100] = 
		"/usr/local/bin/memcached";
	char *args[10];
	args[0] = "memcached";
	args[1] = "-u";
	args[2] = "root";
	args[3] = "-l";
	args[4] = "127.0.0.1";
	args[5] = "-p";
	args[6] = "11211";
	args[7] = NULL;
	pid_t child_pid_1;
	
	child_pid_1 = fork ();
	if (0 == child_pid_1) {
		execv (program, args);
		printf("execv returned error... exiting");
		exit(0);
	}

	MAIN_server_communicator = memcached_create (NULL);
	MAIN_server_handle = 
		memcached_server_list_append (MAIN_server_handle, 
			"LOCAL_HOST", 11211, &MAIN_server_return_value);
	MAIN_server_return_value  = 
		memcached_server_push (MAIN_server_communicator, MAIN_server_handle);
	
	args[0] = "memcached";
	args[1] = "-u";
	args[2] = "root";
	args[3] = "-l";
	args[4] = "127.0.0.1";
	args[5] = "-p";
	args[6] = "20000";
	args[7] = NULL;
	pid_t child_pid_2;

	child_pid_2 = fork ();
	if (0 == child_pid_2)
	{
		execv (program, args);
		printf("execv returned error... exiting");
		exit(0);
	}
	HLR_server_communicator  = memcached_create (NULL);
	HLR_server_handle  = memcached_server_list_append 
		(HLR_server_handle, LOCAL_HOST, 20000, &HLR_server_return_value);
	HLR_server_return_value   = 
		memcached_server_push (HLR_server_communicator, HLR_server_handle);
	
	args[0] = "memcached";
	args[1] = "-u";
        args[2] = "root";
	args[3] = "-l";
	args[4] = "127.0.0.1";
	args[5] = "-p";
	args[6] = "30000";
	args[7] = NULL;
	pid_t child_pid_3;
	
	child_pid_3 = fork ();
	if (0 == child_pid_3) {
		execv (program, args);
		printf("execv returned error... exiting");
		exit(0);
	}
	DLR_server_communicator  = memcached_create (NULL);
	DLR_server_handle  = memcached_server_list_append 
		(DLR_server_handle, LOCAL_HOST, 30000, &DLR_server_return_value);
	DLR_server_return_value   = 
		memcached_server_push (DLR_server_communicator, DLR_server_handle);
	sleep(2);
	return 1;
}

/*************************************************************************************
* Function : insert_memcached()
* Input    : params
* Output   : 
* Purpose  : insertng into memcached 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int insert_memcached (void *params)
{
	char token_names[20][20]	= {0}; 	
	char token_values[20][20]	= {0};
	char primary_key_value[20]	= {0};
	char table_indicator		= 0;
	int table_ind			= 0;	
		
	size_t  key_len 	= 0;
	size_t  value_len 	= 0;
	int iterator		= 0;
	char str[]		= {'#','\0'};

	parse_params (params, (char *)&token_names, (char *)&token_values, 
		primary_key_value, &table_indicator);
	
	const char *key		= primary_key_value;
	key_len				= strlen (primary_key_value);
	const char *value	= (char *)&token_values[0];
	value_len			= strlen(value);

	table_ind = table_indicator - ASCII_ZERO;

	switch (table_ind)
	{
		case MAIN_SERVER:
			MAIN_server_return_value = memcached_set (MAIN_server_communicator, 
					key, key_len, value, value_len, (time_t)0, (uint32_t)0); 
	
			if(!(MAIN_server_return_value == MEMCACHED_SUCCESS)) {
				printf("ERROR in memcached_set\n");
				return 0;
			}
		
			printf("The key set in memcached 1 is %s\n", key);			

			iterator = 1;
			while(!strcmp(token_values[iterator], str)==0)
			{
				value 		= NULL;
				value 		= token_values[iterator];
				value_len 	= 0;
				value_len 	= strlen(value);

				MAIN_server_return_value = memcached_append (MAIN_server_communicator,
						 key, key_len, value, value_len, (time_t)0, (uint32_t)0); 

				if(!(MAIN_server_return_value == MEMCACHED_SUCCESS))
					return 0;
				iterator++;
			}
			break;

		case HLR_SERVER:
			HLR_server_return_value  = memcached_set (HLR_server_communicator,
						 key, key_len, value, value_len, (time_t)0, (uint32_t)0); 

			if(!(HLR_server_return_value == MEMCACHED_SUCCESS))
				return 0;

			printf("The key set in memcached 2 is %s\n", key);			
			iterator = 1;
			while(!strcmp(token_values[iterator], str)==0)
			{
				value 		= NULL;
				value 		= token_values[iterator];
				value_len	= 0;
				value_len 	= strlen(value);
		
				HLR_server_return_value = memcached_append (HLR_server_communicator,
						 key, key_len, value, value_len, (time_t)0, (uint32_t)0); 
				
				if(!(HLR_server_return_value == MEMCACHED_SUCCESS))
					return 0;
				iterator++;
			}
			break;

		case DLR_SERVER:

			DLR_server_return_value  = memcached_set (DLR_server_communicator,
						 key, key_len, value, value_len, (time_t)0, (uint32_t)0);
 
			if(!(DLR_server_return_value == MEMCACHED_SUCCESS))
				return 0;

			printf("The key set in memcached 3 is %s\n", key);			
			iterator = 1;
			while(!strcmp(token_values[iterator], str)==0)
			{
				value 		= NULL;
				value 		= token_values[iterator];
				value_len	= 0;
				value_len 	= strlen(value);
				
				DLR_server_return_value = memcached_append (DLR_server_communicator,
						 key, key_len, value, value_len, (time_t)0, (uint32_t)0); 
				
				if(!(DLR_server_return_value == MEMCACHED_SUCCESS))
					return 0;
				iterator++;
			}
			break;
	}
	return 1;
}


/*************************************************************************************
* Function : get_memcached()
* Input    : params
* Output   : 
* Purpose  : selecting values from memcached 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
char* get_memcached (void *params)
{
	char token_names[20][20]	= {0}; 	
	char token_values[20][20]	= {0};
	char primary_key_value[20]	= {0};
	char table_indicator		= {0};
	
	uint32_t flags;
	size_t  key_len 		= 0;
	size_t value_len		= 0;
	int table_ind			= 0;
	char *memcached_fetched_value	= NULL;

	parse_params ((void *)params, (char *)&token_names, 
		(char *)&token_values, primary_key_value, &table_indicator);

	const char *key	= primary_key_value;
	key_len			= strlen (primary_key_value);

	table_ind = table_indicator - ASCII_ZERO;
	printf("The table_indicator is %d\n", table_ind);	

	switch (table_ind)
	{
		case MAIN_SERVER:

			MAIN_server_return_value = memcached_mget (MAIN_server_communicator,
							&key, &key_len, 1);

			if(!(MAIN_server_return_value == MEMCACHED_SUCCESS)){
				printf("Key Not found in Memcache 1 \n");
				return NULL;
			}
			
			key_len = strlen(primary_key_value);
			value_len = 0;
	
			memcached_fetched_value = 
				memcached_fetch (MAIN_server_communicator, primary_key_value,
				&key_len, &value_len, &flags, &MAIN_server_return_value);

			if(!(MAIN_server_return_value == MEMCACHED_SUCCESS))
				return NULL;

			printf("Key is Present in Memcached 1 %s\n", memcached_fetched_value);	

			break;

		case HLR_SERVER:

			HLR_server_return_value = memcached_mget (HLR_server_communicator,
							 &key, &key_len, 1);

			if(!(HLR_server_return_value == MEMCACHED_SUCCESS)){
				printf("Key Not found in Memcache 2\n");
				return NULL;
			}

			memcached_fetched_value = 
				memcached_fetch (HLR_server_communicator, primary_key_value, 
					&key_len, &value_len, &flags, &HLR_server_return_value);

			if(!(HLR_server_return_value == MEMCACHED_SUCCESS))
				return NULL;
	
			printf("Key is Present in Memcached 2 %s\n", memcached_fetched_value);
			break;

		case DLR_SERVER:

			DLR_server_return_value = memcached_mget (DLR_server_communicator,
							 &key, &key_len, 1);

			if(!(DLR_server_return_value == MEMCACHED_SUCCESS)){
				printf("Key Not found in Memcache 3\n");	
				return NULL;
			}
			
			memcached_fetched_value = 
				memcached_fetch (DLR_server_communicator, primary_key_value, 
					&key_len, &value_len, &flags, &DLR_server_return_value);

			if(!(DLR_server_return_value == MEMCACHED_SUCCESS))
				return NULL;

			printf("Key is Present in Memcached 3 %s\n", memcached_fetched_value);
			break;
	}
	return memcached_fetched_value;
}


/*************************************************************************************
* Function : select_memcached()
* Input    : params
* Output   : 
* Purpose  : selecting values from memcached 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int select_memcached (void *params)
{
	int ret = 0;
	return ret;
}

/*************************************************************************************
* Function : append_memcached()
* Input    : params
* Output   : 
* Purpose  : appending values into memcached 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int append_memcached (void *params)
{
	char token_names[20][20]	= {0};
	char token_values[20][20]	= {0};
	char primary_key_value[20]	= {0};
	char table_indicator		= 0;
		
	char *key 		= NULL;
	size_t  key_len 	= 0;
	size_t  value_len 	= 0;
	char *value 		= NULL;
	int iterator 		= 0;
	int table_ind = table_indicator - ASCII_ZERO;
	char str[]		= {'#','\0'};	

	parse_params (params, (char *)&token_names, 
		(char *)&token_values, primary_key_value, &table_indicator);
	
	key		= primary_key_value;
	key_len 	= strlen(primary_key_value);

	switch (table_ind)
	{
		case MAIN_SERVER:

			while(!strcmp(token_values[iterator], str)==0)
			{
				value = NULL;
				value = token_values[iterator];
				value_len = 0;
				value_len = strlen(value);

				MAIN_server_return_value = memcached_append (MAIN_server_communicator,
							 key, key_len, value, value_len, (time_t)0, (uint32_t)0); 
	
				if(!(MAIN_server_return_value == MEMCACHED_SUCCESS))
					return 0;
				iterator++;
			}
			break;

		case HLR_SERVER:
			while(token_values[iterator]!=NULL)
			{
				value 		= NULL;
				value 		= token_values[iterator];
				value_len	= 0;	
				value_len 	= strlen(value);
		
				HLR_server_return_value = memcached_append (HLR_server_communicator,
						 key, key_len, value, value_len, (time_t)0, (uint32_t)0); 
	
				if(!(HLR_server_return_value == MEMCACHED_SUCCESS))
					return 0;
				iterator++;
			}
			break;

		case DLR_SERVER:
			while(token_values[iterator]!=NULL)
			{
				value 		= NULL;
				value 		= token_values[iterator];
				value_len	= 0;	
				value_len 	= strlen(value);
				
				DLR_server_return_value = memcached_append (DLR_server_communicator,
						 key, key_len, value, value_len, (time_t)0, (uint32_t)0);
	
				if(!(DLR_server_return_value == MEMCACHED_SUCCESS))
					return 0;
				iterator++;
			}
			break;
	}
	return 1;
}


/*************************************************************************************
* Function : delete_memcached()
* Input    : params
* Output   : 
* Purpose  : deleting values from memcached 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int delete_memcached (void *params)
{
	char token_names[20][20]		= {0};
	char token_values[20][20]		= {0};
	char primary_key_value[20]		= {0};
	char table_indicator			= 0;
		
		
	char *key 	= NULL;
	size_t  key_len 	= 0;
	int table_ind	= table_indicator - ASCII_ZERO;

	parse_params (params, (char *)&token_names, 
		(char *)&token_values, primary_key_value, &table_indicator);
	
	key		= primary_key_value;
	key_len 	= strlen(primary_key_value);
		
	switch (table_ind)
	{
		case MAIN_SERVER:

			MAIN_server_return_value = memcached_delete (MAIN_server_communicator,
						 key, key_len, (time_t)0);

			if(MAIN_server_return_value == MEMCACHED_SUCCESS)
				return 1;
			break;

		case HLR_SERVER:

			HLR_server_return_value  = memcached_delete (HLR_server_communicator,
						 key, key_len, (time_t)0);

			if(HLR_server_return_value == MEMCACHED_SUCCESS)
				return 1;
			break;

		case DLR_SERVER:

			DLR_server_return_value  = memcached_delete (DLR_server_communicator,
						 key, key_len, (time_t)0);

			if(DLR_server_return_value == MEMCACHED_SUCCESS)
				return 1;
			break;
	}
	return 0;
}

/*
void main()
{
	
	init_memcached_1();	
	char params[100]="global_msg_id;txr_id;#;107;100;#;111;#;1;";	
	insert_memcached((void *)params);
	
	char params_7[100]="global_msg_id;txr_id;#;707;100;#;777;#;1;";	
	insert_memcached((void *)params_7);
		
	char params_8[100]="global_msg_id;#;007;#;777;#;1;";	
	get_memcached((void*)params_8);
	
	char params_2[100]="global_msg_id;txr_id;#;207;100;#;222;#;2;";	
	insert_memcached((void *)params_2);

	char params_3[100]="global_msg_id;txr_id;#;307;100;#;333;#;3;";	
	insert_memcached((void *)params_3);

		

	char params_4[100]="global_msg_id;#;007;#;111;#;1;";	
	get_memcached((void*)params_4);	

	char params_5[100]="global_msg_id;#;007;#;222;#;2;";	
	get_memcached((void*)params_5);
	
	char params_6[100]="global_msg_id;#;007;#;333;#;3;";	
	get_memcached((void*)params_6);	

	while(1)
	{
		sleep(1000);
	}
}*/
