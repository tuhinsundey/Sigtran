 /***************************************************************************
 *                                                                           
 *     This program is free software; you can redistribute it and/or modify  
 *     it under the terms of the GNU General Public License as published by  
 *     the Free Software Foundation; either version 2 of the License, or     
 *     (at your option) any later version.                                   
 *                                                                           
 ****************************************************************************/

/*
 *   Purpose: This file defines main and  
 *            has definitions for:
 *           - main
 *           - hybernate main
 *           - handle signal
 *           - Decoding 
 */

#include "common.h"
#include "smsc_rear.h"
#include "hlr_connect.h"
#include "route_logic.h"

/*
 * hybernate the main thread
 * after initialising everything and block indefinitely
 */
static void hybernate_main ()
{
	select (0, NULL, NULL, NULL, NULL);
}

/*
 * Handle signals
 *
 */
static void handle_signal()
{
	struct sigaction sig_handle;
	sig_handle.sa_handler = (void *)shed_connections;
	sigaction (SIGTSTP, &sig_handle, NULL);
	sigaction (SIGINT, &sig_handle, NULL);
	sigaction (SIGTERM, &sig_handle, NULL);
	sigaction (SIGABRT, &sig_handle, NULL);
}

int main ()
{
	/*
	 * start should initialize the 
	 * following entities:
	 *
	 * 1) SMSC to get the traffic from
	 * 2) HLR to query IMSI and LMSI
	 * 3) SMSC(s) to terminate
	 */
	handle_signal();
	printf ("connecting with smsc_rear...\n");
	smsc_rear_init ();	
	//smsc_hind_init ();
	sleep (2);
	//printf ("connecting with hlr...\n");
	//hlr_init ();
	//sleep (2);
	//shed_connections ();
	
	hybernate_main();
	
	return 0;
}

