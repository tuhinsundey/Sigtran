 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines storage information and  
 *            has definitions for:
 *           - read Configuration
 *           - init callbacks 
 *           - init storage
 *           - dlr call 
 */

#include "storage.h"

/*************************************************************************************
* Function : read_configuration()
* Input    : none 
* Output   : 
* Purpose  : read data from configuration file 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int read_configuration ()
{
	int 	ret 		= 0;
	char    *get_conf       = NULL;
	char    ini_name[30]    = "../../config/m3ua.ini";
	char    local_name[30]  = {0};
	dictionary *ini;
	int storage_type ;

	/* pass file name to iniparser_load */
	ini = iniparser_load (ini_name);

	sprintf (local_name, "%s", "storage:storage_type");
	storage_type = iniparser_getint (ini, local_name, 0);
	
	switch (storage_type)
	{
		case MYSQL_DB:

			/* read database port */
			sprintf (local_name, "%s", "database:db_port");
			storage_values.port = iniparser_getint (ini, local_name, 0);
	
			/* read database username */
			sprintf(local_name, "%s", "database:username");
			get_conf = iniparser_getstring(ini, local_name, NULL);
			memcpy(storage_values.username, get_conf, strlen(get_conf));

			/* read database password */
			sprintf(local_name, "%s", "database:password");
			get_conf = iniparser_getstring(ini, local_name, NULL);
			memcpy(storage_values.password, get_conf, strlen(get_conf));

			/* read database name */
			sprintf(local_name, "%s", "database:database");
			get_conf = iniparser_getstring(ini, local_name, NULL);
			memcpy(storage_values.database, get_conf, strlen(get_conf));

			/* read database address */
			sprintf(local_name, "%s", "database:db_server");
			get_conf = iniparser_getstring(ini, local_name, NULL);
			memcpy(storage_values.server, get_conf, strlen(get_conf));

			break;

		case MEMCACHED:
			break;

		case CACHING:
			break;
	}
	init_callbacks (storage_type);
}

/*************************************************************************************
* Function : init_callbacks()
* Input    : storage_type
* Output   : 
* Purpose  : initialize callback functions 
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
static void init_callbacks (int storage_type)
{
	switch (storage_type)
	{
		case MYSQL_DB:
			callbacks.initialize	= &init_mysql;
			callbacks.insert	= &insert_mysql_table;
			callbacks.update	= &update_mysql_table;
			callbacks.delete	= &delete_mysql_table;
			callbacks.select	= &select_mysql_table;
			break;

		case MEMCACHED:
			callbacks.initialize	= &init_memcached;
			callbacks.insert	= &insert_memcached;
		//	callbacks.update	= &replace_memcached;
			callbacks.append	= &append_memcached;
			callbacks.delete	= &delete_memcached;
		//	callbacks.select	= &select_memcached;
			callbacks.get_value	= &get_memcached;
			break;

		case CACHING:
			break;
	}
}

/*************************************************************************************
* Function : init_storage()
* Input    : none 
* Output   : 
* Purpose  :  
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int init_storage ()
{
	int ret;
	ret = callbacks.initialize ((void *)(&storage_values));
	return ret;
}

/*************************************************************************************
* Function : dlr_call()
* Input    : type, params 
* Output   : 
* Purpose  :  
* Author   : (tuhin.shankar.dey@gmail.com)
*************************************************************************************/
int dlr_call (int type, void *params)
{
	int ret;
	switch (type)
	{
		case UPDATE:
			ret = callbacks.update (params);
			break;
		case INSERT:
			ret = callbacks.insert (params);
			break;
		case DELETE:
			ret = callbacks.delete(params);
			break;
		case SELECT:
			ret = callbacks.select(params);
			break;
	}
	return ret;
}


