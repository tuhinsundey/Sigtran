/*
 *  sctpsrvr.c
 */
#include "common.h"
#include "util.h"
#include "sua.h"
#include "tcap.h"
#include "iniparser.h"
#include "dictionary.h"
#include <fcntl.h>

/* Global variables */
char src_ip[30] = {0};
char dest_ip[30]= {0};
char simulator_type[30] = {0};
struct sockaddr_in host_addr, peer_addr;
unsigned char sua_message[400] = {0};
int global_sock_fd, flags;
int tcap_message_length, sua_message_length;
unsigned int src_trx_id;
double trx_id = 12345;
int recevd_bytes;
char handshake = 0;  
sms_fields sms;


void sua_connect (void);

/*************************************************************************************
* Function : simSigHandle()
* Input    : sig
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void simSigHandle (int sig)
{
	printf ("caught signal %d", sig);
	close (global_sock_fd);
	sleep (1);
	exit (0);	 
}


/*************************************************************************************
* Function : handleSignal()
* Input    : None
* Output   : 
* Purpose  : For signal Handle
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void handleSignal ()
{
    struct sigaction sim_sigHandle;
    memset (&sim_sigHandle, 0, sizeof(struct sigaction));

    sim_sigHandle.sa_handler    = simSigHandle;
    sim_sigHandle.sa_flags      = SA_SIGINFO;

	sigaction (SIGHUP, &sim_sigHandle, NULL);
	sigaction (SIGABRT, &sim_sigHandle, NULL);
	sigaction (SIGINT, &sim_sigHandle, NULL);
	sigaction (SIGSEGV, &sim_sigHandle, NULL);
	sigaction (SIGPIPE, &sim_sigHandle, NULL);
}


/*************************************************************************************
* Function : main()
* Input    : None
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int main()
{
	struct sctp_event_subscribe events;
	struct sockaddr_in host_addr, peer_addr; 
	struct sctp_initmsg initmsg;
	int listenSock, connSock;
	char buffer[MAX_BUFFER + 1];
	time_t currentTime, startTime;
	//int sendbuff = 999999;
	int ret;

	char ini_name[15] = "config.ini";
	dictionary *ini = NULL;
	char *get_conf = NULL;
	int  local_port, peer_port;
	char local_name[25] = {0};

	handleSignal ();
	ini = iniparser_load(ini_name);

	sprintf (local_name, "%s", "ip:src_ip");
	get_conf = iniparser_getstring (ini, local_name, LOCAL_HOST);
	memcpy (src_ip, get_conf, strlen (get_conf));

	sprintf (local_name, "%s", "ip:destination_ip");
	get_conf = iniparser_getstring (ini, local_name, LOCAL_HOST);
	memcpy (dest_ip, get_conf, strlen(get_conf));

	local_port = iniparser_getint (ini, "port:local", SUA_PORT_NUM);
	peer_port  = iniparser_getint (ini, "port:peer", SUA_PORT_NUM);

	sprintf (local_name, "%s", "type:sim_type");
	get_conf = iniparser_getstring (ini, local_name, NULL);
	memcpy(simulator_type, get_conf, strlen (get_conf));

	log_init("GSMSC_SUA");		
	char log_filename[30] = "gsmscsua.log";	
	log_open(log_filename);
	log_msg(LOG_INFO, "SUA is GETTING STARTED");
	
	struct linger linger_params;
	linger_params.l_onoff = 1;
	linger_params.l_linger = 0;
	
	/* Create SCTP TCP-Style Socket */
	listenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);

	memset (&host_addr, 0, sizeof (host_addr));
	host_addr.sin_family		= AF_INET;
	host_addr.sin_addr.s_addr= htonl (INADDR_ANY);
	host_addr.sin_port		= htons (local_port);
    
	ret = bind (listenSock, (struct sockaddr *)&host_addr, 
			sizeof(host_addr));

	memset (&peer_addr, 0, sizeof (peer_addr));
	peer_addr.sin_family		= AF_INET;
	peer_addr.sin_addr.s_addr = inet_addr (dest_ip);
	peer_addr.sin_port		  = htons (peer_port);

	ret = connect (listenSock, (struct sockaddr *)&peer_addr, 
				sizeof(peer_addr));

	if (!ret)
	{
		ret = setsockopt (listenSock, SOL_SOCKET, SO_LINGER, 
			&linger_params, sizeof (linger_params));
		if (ret)
			printf ("=====setsockopt for SO_LINGER failed !!!\n");
		sua_message_length = create_sua_ASPSM
				(sua_message, SUA_MSG_TYPE_ASPSM_UP);
		ret = sctp_sendmsg (listenSock, (void *)sua_message,
				sua_message_length, NULL, 0, htonl(4),
				0, LOCALTIME_STREAM, 0, 0);	
	
		global_sock_fd = listenSock;
		sua_connect ();	
	}

	/* Accept connections from any interface */
	/* Specify that a maximum of 5 streams will be available per socket */
	memset (&initmsg, 0, sizeof(initmsg));
	initmsg.sinit_num_ostreams	= 3;
	initmsg.sinit_max_instreams	= 4;
	initmsg.sinit_max_attempts	= 4;
	ret = setsockopt (listenSock, IPPROTO_SCTP, SCTP_INITMSG, 
			&initmsg, sizeof(initmsg));
  
	/* Place the server socket into the listening state */
	listen (listenSock, 5);

	/* Server loop... */
	while (1) {
		/* Await a new client connection */
		printf ("Awaiting a new connection\n");
		/* New client socket has connected */
		connSock 	= accept (listenSock, (struct sockaddr *)NULL, 
					NULL);
		/* Grab the current time */
		currentTime 	= time(NULL);
		/* Send local time on stream 0 (local time stream) */
		snprintf (buffer, MAX_BUFFER, "%s\n", ctime(&currentTime));

		//ret = setsockopt (connSock, SOL_SOCKET, SO_SNDBUF, 
		//		&sendbuff, sizeof(sendbuff));
		currentTime = time(NULL);
		startTime = time(NULL);
		memset ((void *)&events, 0, sizeof(events));
		events.sctp_data_io_event = 1;
		ret = setsockopt (connSock, SOL_SCTP, SCTP_EVENTS, 
				(const void *)&events, sizeof(events));

		if (connSock)
		{
			/* 
 			 *	Received ASP_UP message,
 			 *	Send Back an ASP_UP message
 			 */
			ret = setsockopt (connSock, SOL_SOCKET, SO_LINGER, 
				&linger_params, sizeof (events));
			if (ret)
				printf ("=====setsockopt for SO_LINGER failed !!!\n");
			sua_message_length = create_sua_ASPSM 
					(sua_message, SUA_MSG_TYPE_ASPSM_UP);
			ret = sctp_sendmsg (connSock, (void *)sua_message, 
				sua_message_length, NULL, 0, htonl(4), 
				0, LOCALTIME_STREAM, 0, 0 );
		}
		global_sock_fd = connSock;
		sua_connect ();
 		currentTime = time(NULL);
	}
	return 0;
}


/*************************************************************************************
* Function : sua_connect()
* Input    : None
* Output   : 
* Purpose  : To act as a client 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void sua_connect (void)	
{
	int ret;
	struct sctp_sndrcvinfo sndrcvinfo;

	while (1) {
		recevd_bytes = sctp_recvmsg (global_sock_fd, 
			(void *)sua_message, 400, 
			(struct sockaddr *)NULL, 0, 
			&sndrcvinfo, &flags);
		if (recevd_bytes > 0)
		{     
			sua_message[recevd_bytes] = 0;
			#if 0	
			/* Received ASP_UP message */
			if(sua_message[message_class] 
				== SUA_MSG_CLASS_ASPSM && 
				sua_message[message_type] 
				== SUA_MSG_TYPE_ASPSM_UP)
			{
				sua_message_length = 
				create_sua_ASPSM (sua_message, 
					SUA_MSG_TYPE_ASPSM_UP);
				/* Send ASP_UP message to the peer */
				ret = sctp_sendmsg 
					(connSock, (void *)sua_message, 
					sua_message_length, NULL, 0, 
				htonl(4), 0, 
				LOCALTIME_STREAM, 0, 0);
			}
			#endif
			/* Received ASP_UP_ACK message */
			if(sua_message[message_class] 
				== SUA_MSG_CLASS_ASPSM && 
				sua_message[message_type] 
				== SUA_MSG_TYPE_ASPSM_UPACK) {

				sua_message_length = create_sua_ASPSM (sua_message, 
									SUA_MSG_TYPE_ASPSM_UPACK);
				/* Send ASP_UP_ACK message to the peer */
				ret = sctp_sendmsg (global_sock_fd, (void *)sua_message, 
					sua_message_length, NULL, 0, htonl(4), 
					0, LOCALTIME_STREAM, 0, 0);
			}

			/* Received ASP_ACTIVE message */
			else if (sua_message[message_class] 
				== SUA_MSG_CLASS_ASPTM && 
				sua_message[message_type]
				== SUA_MSG_TYPE_ASPTM_ACT) {
				sua_message_length = create_sua_ASPTM (sua_message, 
									SUA_MSG_TYPE_ASPTM_ACT);
				/* Send ASP_ACTIVE message to the peer */
				ret = sctp_sendmsg (global_sock_fd, (void *)sua_message, 
						sua_message_length, NULL, 0, 
						htonl(4), 0, LOCALTIME_STREAM, 0, 0);
			}

			/* Received ASP_ACTIVE_ACK message */
			else if (sua_message[message_class]	
				== SUA_MSG_CLASS_ASPTM && 
				sua_message[message_type]	
				== SUA_MSG_TYPE_ASPTM_ACTACK) {
				sua_message_length = create_sua_ASPTM 
					(sua_message, SUA_MSG_TYPE_ASPTM_ACTACK);
				/* Send ASP_ACTIVE_ACK message to the peer */
				ret = sctp_sendmsg 
					(global_sock_fd, (void *)sua_message, 
					sua_message_length, NULL, 0, 
					htonl(4), 0, 
					LOCALTIME_STREAM, 0, 0);
				handshake = 1;
					
				if (strcmp(simulator_type, "ACK")) {
					sua_message_length = create_sua_CL (sua_message, 
						SUA_MSG_TYPE_CL_CLDT, dest_ip, src_ip);	
					
			   		strncpy ((char *)sms.SM_RP_DA.number, 
							"123456789123456",15);
			    	sms.SM_RP_DA.length=15;
					strncpy ((char *)sms.SM_RP_OA.number,
							"1234567891",10);
					sms.SM_RP_OA.length=10;
					strncpy ((char *)sms.TP_Orig_Dest_Number.number,
							"1234567891",10);
			    	sms.TP_Orig_Dest_Number.length=10;
			    	strncpy ((char *)sms.SMSC_timestamp,
							"12345678912345",14);
			    	memset(sms.sms_data, 0, 160);
			    	strncpy(sms.sms_data, "hello world", 10);     

			   		sms.trx_id = trx_id;
					++trx_id;
					unsigned char *tcap_message = 
								forward_sm_MO_packet (&sms,
								&tcap_message_length);
					if (!tcap_message)
						continue;
					memcpy (&sua_message[100], 
						tcap_message, tcap_message_length);
					sua_message[7]	+= tcap_message_length - 1;
					sua_message[99]	+= tcap_message_length; 
					freeze (tcap_message);
					while(1) {
						sms.trx_id = trx_id;
						++trx_id;
						tcap_message_length = 0;
						tcap_message = forward_sm_MO_packet 
								(&sms, &tcap_message_length);
						memcpy (&sua_message[100], tcap_message, 
								tcap_message_length);

						ret = sctp_sendmsg (global_sock_fd, 
								(void *)sua_message, sua_message[7],	
								NULL, 0, htonl(4), 0, 
								LOCALTIME_STREAM, 0, 0);
	
						freeze (tcap_message);
					}
				}
			}
			else if ((sua_message[100] == 0x62) && handshake == 1 &&
						!(strcmp(simulator_type, "ACK"))) {
			
				src_trx_id = ntohl(*(unsigned int*) &sua_message[105]);
				sua_message_length = create_sua_CL (sua_message,
						SUA_MSG_TYPE_CL_CLDT, dest_ip, src_ip);  

				/* Now Send a TCAP Continue Ack Message */  
				unsigned char *tcap_message_ack = 
					forward_sm_ack_packet_continue (&tcap_message_length,
						MT_FORWARD_SM, src_trx_id, htonl(src_trx_id + 1));

				memcpy (&sua_message[100], tcap_message_ack,tcap_message_length);
				sua_message[7]  += tcap_message_length - 1;
				sua_message[99] += tcap_message_length; 
				ret = sctp_sendmsg (global_sock_fd,
					(void *)sua_message, sua_message[7],
					NULL, 0, htonl(4), 0,
					LOCALTIME_STREAM, 0, 0); 

				freeze (tcap_message_ack);
				/* Now Send a TCAP End Ack Message */  
				memset(sua_message,0,400);
				sua_message_length = create_sua_CL (sua_message,
						SUA_MSG_TYPE_CL_CLDT,dest_ip, src_ip);  
				unsigned char *tcap_message_ack_end = 
                       forward_sm_end_ack_packet (&tcap_message_length,
                       MT_FORWARD_SM,src_trx_id);
				memcpy (&sua_message[100], tcap_message_ack_end, 
					tcap_message_length);
				sua_message[7]  += tcap_message_length - 1;
				sua_message[99] += tcap_message_length; 
				
				ret = sctp_sendmsg (global_sock_fd, 
					(void *)sua_message, sua_message[7],
					NULL, 0, htonl(4), 0,
					LOCALTIME_STREAM, 0, 0); 
				freeze (tcap_message_ack_end);
			} 
			else
				continue;
		}
		else {
			usleep (10);
			continue;
		}
	}
}

