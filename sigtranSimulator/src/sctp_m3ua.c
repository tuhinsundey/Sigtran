/*
 *  sctpsrvr.c
 */
#include "common.h"
#include "m3ua.h"
#include "parse_message.h"
#include "util.h"
#include "tcap.h"
#include "iniparser.h"
#include "dictionary.h"
#include <fcntl.h>

char routing_context[8] = {0};	

#define m3ua_send_SM_MT_ack m3ua_send_SM_MO_ack
#define m3ua_send_SM_MT(imsi)  m3ua_send_SM_MO(char *imsi)

/* Global variables */
char src_ip[30] = {0};
char dest_ip[30]= {0};
char simulator_type[30] = {0};
struct sockaddr_in host_addr, peer_addr;
unsigned char m3ua_message[400] = {0};
int global_sock_fd, flags, run_mode = 0;
int tcap_message_length, m3ua_message_length;
unsigned int src_trx_id;
unsigned int trx_id = 12345;
int recevd_bytes;
char handshake = 0;  
sms_fields sms;


m3ua_asp_states host_state, peer_state = asp_init;
tcap_states message_status = tcap_ready;
extern char *DA_MSISDN;

static unsigned char MSISDN[12] = {0};
char *IMSI  = NULL ;

unsigned char m3ua_packet [400] = {0}; 

void m3ua_connect (void);
int m3ua_send_SRI ();
void m3ua_send_SRI_ack (unsigned int);
void m3ua_send_SM_MO( char *imsi);
void m3ua_send_SM_MO_ack(unsigned , char );
void do_asp_handshake (void);
void do_sg_handshake (char *);

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
	//sleep (1);
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
    * Function : set_sockoptions
    * Input    : nothing
    * Output   : nothing
    * Purpose  : to set options to the sockfd
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void set_sockoptions ( )
{
	int ret;
	int nagle_flag = 1;
	struct linger linger_params;
	linger_params.l_onoff = 1;
	linger_params.l_linger = 0;

	ret = setsockopt (global_sock_fd, SOL_SOCKET, SO_LINGER, 
			&linger_params, sizeof (linger_params));
	if (ret)
		log_msg (LOG_ERR, "setsockopt for SO_LINGER failed!!!");

	ret = setsockopt (global_sock_fd, IPPROTO_SCTP, SCTP_NODELAY,
            		(void *)&nagle_flag, sizeof (int));
	if (ret)
		log_msg (LOG_ERR, "setsockopt for SCTP_NODELAY failed!!!");
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
	struct sctp_sndrcvinfo sndrcvinfo;
	struct sockaddr_in host_addr, peer_addr; 
	struct sctp_initmsg initmsg;
	int listenSock, connSock;

	int ret;
	int flag = 0;
	char ini_name[15] = "config.ini";
	dictionary *ini = NULL;
	char *get_conf = NULL;
	int  local_port, peer_port;
	char local_name[25] = {0};
	int ret_message_class = 0;		

	handleSignal ();

	ini = iniparser_load (ini_name);

	log_init ("GSMSC_M3UA");		
	char log_filename[30] = "gsmscm3ua.log";	
	log_open (log_filename);
	log_msg (LOG_INFO, "M3UA is GETTING STARTED");
	
	sprintf (local_name, "%s", "ip:src_ip");
	get_conf = iniparser_getstring (ini, local_name, LOCAL_HOST);
	memcpy (src_ip, get_conf, strlen (get_conf));

	sprintf (local_name, "%s", "ip:destination_ip");
	get_conf = iniparser_getstring (ini, local_name, LOCAL_HOST);
	memcpy (dest_ip, get_conf, strlen(get_conf));

	local_port = iniparser_getint (ini, "port:local", M3UA_PORT_NUM);
	peer_port  = iniparser_getint (ini, "port:peer", M3UA_PORT_NUM);

	get_conf = iniparser_getstring (ini, "type:sim_type", NULL);
	memcpy (simulator_type, get_conf, strlen (get_conf));
	get_conf = iniparser_getstring (ini, "type:host", NULL);
	if (get_conf) {
		if (!strcasecmp (get_conf, "server"))
			run_mode = 1;
	}

	read_database_configuration();
	init_msqlHandle();	

	/* Create SCTP TCP-Style Socket */
	listenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);

	memset (&host_addr, 0, sizeof (host_addr));
	host_addr.sin_family		= AF_INET;
	host_addr.sin_addr.s_addr	= htonl (INADDR_ANY);
	host_addr.sin_port		= htons (local_port);
    
	ret = bind (listenSock, (struct sockaddr *)&host_addr, 
			sizeof(host_addr));

	memset (&peer_addr, 0, sizeof (peer_addr));
	peer_addr.sin_family		= AF_INET;
	peer_addr.sin_addr.s_addr 	= inet_addr (dest_ip);
	peer_addr.sin_port		= htons (peer_port);

	ret = connect (listenSock, (struct sockaddr *)&peer_addr, 
				sizeof(peer_addr));
	if (!ret)
	{
		global_sock_fd = listenSock;
		m3ua_message_length = create_m3ua_ASPSM
				(m3ua_message, M3UA_MSG_TYPE_ASPSM_UP);
		ret = sctp_sendmsg (listenSock, (void *)m3ua_message,
				m3ua_message_length, NULL, 0, htonl (3),
				0, CONTROL_STREAM, 0, 0);	
				
		recevd_bytes = sctp_recvmsg (global_sock_fd, 
			(void *)m3ua_message, 400, 
			(struct sockaddr *)NULL, 0, 
			&sndrcvinfo, &flags);
		if(!(m3ua_message[message_type]== 0x04)){
			printf("Handshake cannot complete");
		}	
			
		char imsi[20] = {0};
		//char routing_context[8] = {0};	
		host_state = asp_up_tx;
		//do_asp_handshake();
		do_sg_handshake(routing_context);
		log_msg (LOG_INFO, "====Handshake Complete with peer====");
		while (1)
		{   
			if (!m3ua_send_SRI()){
				continue;
			}    
			while (1)
			{   
				memset(m3ua_message,0,400);
				recevd_bytes = sctp_recvmsg (global_sock_fd, 
                    			(void *)m3ua_message, 400, (struct sockaddr *)NULL, 
					0, &sndrcvinfo, &flags);

					if (recevd_bytes > 0){
					/*received sri_sm_ack ... send MO/MT*/ 
					if (m3ua_message[62] == TAG_GSM_TCAP_END 
						&& m3ua_message[121] == 0x2d) {
						memset(imsi, 0, 20);	
						parse_sri_ack_packet(m3ua_message, imsi);
						m3ua_send_SM_MO(DA_MSISDN);
					}
					if (m3ua_message[62] == TAG_GSM_TCAP_END && 
							m3ua_message[121] == 0x00)
						break;
				}   
			}
		}   
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
		/* Send local time on stream 0 (local time stream) */

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
			m3ua_message_length = create_m3ua_ASPSM 
					(m3ua_message, M3UA_MSG_TYPE_ASPSM_UP);
			ret = sctp_sendmsg (connSock, (void *)m3ua_message, 
				m3ua_message_length, NULL, 0, htonl(3), 
				0, CONTROL_STREAM, 0, 0 );
			host_state = asp_up_tx;
		}
		global_sock_fd = connSock;
		flag=1;
		ret = setsockopt (global_sock_fd, IPPROTO_SCTP, SCTP_NODELAY,
                		(void *)&flag, sizeof (int));
		if (ret)
			log_msg(LOG_ERR, "setsockopt for SCTP_NODELAY failed!!!");
		do_asp_handshake ();
		log_msg (LOG_INFO, "====Handshake Complete with peer====");
		//init_caching(ini);
 		while(1)
		{	
			recevd_bytes = sctp_recvmsg (global_sock_fd, 
           	 		(void *)m3ua_message, 400, 
           	 		(struct sockaddr *)NULL, 0,  &sndrcvinfo, &flags);

        	if (recevd_bytes > 0) {
				ret_message_class = parse_message (m3ua_message, 
									&trx_id);	
				if (ret_message_class == TAG_SEND_ROUT_INFO){
					memset (MSISDN, 0, 12);
                    decode_number_or_time (&m3ua_message[126], MSISDN, 6);
                    log_msg (LOG_INFO, "======receiver address : %s\n", MSISDN);
                    IMSI = get_IMSI ((char *)MSISDN);

                    if(IMSI == NULL) {
                        char temp_imsi[20] = "12345678901234";
                        IMSI = malloc (16);
                        memcpy(IMSI,temp_imsi, 15);
                    }
                    log_msg (LOG_INFO, "======IMSI address : %s\n", IMSI);
                    m3ua_send_SRI_ack (trx_id);
                    freeze (IMSI);
				}
				else if (ret_message_class == MO_FORWARD_SM)
					m3ua_send_SM_MO_ack (trx_id, MO_FORWARD_SM);
				else if (ret_message_class == MT_FORWARD_SM)
					m3ua_send_SM_MT_ack (trx_id, MT_FORWARD_SM);
				else {
					log_msg (LOG_ERR, "====wrong tcap message class\n");
					continue;
				}
			}
		}
		return 0;
	}
}

/*************************************************************************************
    * Function : m3ua_send_SRI
    * Input    : none 
    * Output   : 
    * Purpose  : Cretae SRI packet
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int m3ua_send_SRI()
{
	memset (m3ua_packet, 0, 400);
	int len = create_m3ua_data_SRI (m3ua_packet);
	if (len == 0)
		return 0;
	m3ua_packet[len++] = 0x00;
	m3ua_packet[len++] = 0x13;
	m3ua_packet[len++] = 0x00;
	m3ua_packet[len++] = 0x08;
	m3ua_packet[len++] = 0xfc;
	m3ua_packet[len++] = 0xdc;
	m3ua_packet[len++] = 0x12;
	m3ua_packet[len++] = 0x98;
	
	sctp_sendmsg (global_sock_fd, (void *)m3ua_packet, 
		len, NULL, 0, htonl(3), 
		0, DATA_STREAM, 0, 0 );
	return 1;
}


/*************************************************************************************
    * Function : create_send_SRI_ack
    * Input    : trx_id 
    * Output   : 
    * Purpose  : Create SRI_acke packet
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void m3ua_send_SRI_ack(unsigned int trx_id)
{
	memset(m3ua_packet,0,400);
	int len = create_m3ua_data_SRI_ACK (m3ua_packet, trx_id);
	m3ua_packet[len++] = 0x00;
    m3ua_packet[len++] = 0x13;
    m3ua_packet[len++] = 0x00;
    m3ua_packet[len++] = 0x08;
    m3ua_packet[len++] = 0xfc;
    m3ua_packet[len++] = 0xdc;
    m3ua_packet[len++] = 0x12;
    m3ua_packet[len++] = 0x98;
	sctp_sendmsg (global_sock_fd, (void *)m3ua_packet, 
		len, NULL, 0, htonl(3), 
		0, DATA_STREAM, 0, 0 );
}


/*************************************************************************************
    * Function : m3ua_send_SM_MO
    * Input    : imsi 
    * Output   : 
    * Purpose  : To create MO_forward_SM packet
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void m3ua_send_SM_MO(char *imsi)
{
	memset(m3ua_packet,0,400);
	int len = create_m3ua_data_MO_SM(m3ua_packet, imsi);
	m3ua_packet[len++] = 0x00;
    m3ua_packet[len++] = 0x13;
    m3ua_packet[len++] = 0x00;
    m3ua_packet[len++] = 0x08;
    m3ua_packet[len++] = 0xfc;
    m3ua_packet[len++] = 0xdc;
    m3ua_packet[len++] = 0x12;
    m3ua_packet[len++] = 0x98;
	sctp_sendmsg (global_sock_fd, (void *)m3ua_packet, 
			len, NULL, 0, htonl(3), 
			0, DATA_STREAM, 0, 0 );
}


/*************************************************************************************
    * Function : m3ua_send_SM_MO_ack
    * Input    : trx_id and type 
    * Output   : 
    * Purpose  : To create ack packet
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void m3ua_send_SM_MO_ack(unsigned trx_id, char type)
{
	memset(m3ua_packet,0,400);
	int len = create_m3ua_data_MO_SM_ACK (m3ua_packet, trx_id, type);
	m3ua_packet[len++] = 0x00;
    m3ua_packet[len++] = 0x13;
    m3ua_packet[len++] = 0x00;
    m3ua_packet[len++] = 0x08;
    m3ua_packet[len++] = 0xfc;
    m3ua_packet[len++] = 0xdc;
    m3ua_packet[len++] = 0x12;
    m3ua_packet[len++] = 0x98;
	sctp_sendmsg (global_sock_fd, (void *)m3ua_packet, 
		len, NULL, 0, htonl(3), 
		0, DATA_STREAM, 0, 0 );
}



/*************************************************************************************
    * Function : do_asp_handshake (void)	
    * Input    : None
    * Output   : 
    * Purpose  : For handshaking 
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void do_asp_handshake (void)	
{
	int ret;
	struct sctp_sndrcvinfo sndrcvinfo;
	char rounting_context[10] = {0};	

	while (1) {
		if (asp_up_ack_tx == host_state)
		{
			m3ua_message_length = create_m3ua_ASPTM (m3ua_message, 
								M3UA_MSG_TYPE_ASPTM_ACT, rounting_context);
			/* Send ASP_ACTIVE message to the peer */
			ret = sctp_sendmsg (global_sock_fd, (void *)m3ua_message, 
					m3ua_message_length, NULL, 0, 
					htonl (3), 0, CONTROL_STREAM, 0, 0);
			host_state = asp_active_tx;
			printf ("host asp active...\n");
		}

		if (asp_active_ack_tx == peer_state 
			&& asp_active_ack_tx == host_state) {
			printf ("host & peer asp active acked...\n");
			break;
		}
		recevd_bytes = sctp_recvmsg (global_sock_fd, 
			(void *)m3ua_message, 400, 
			(struct sockaddr *)NULL, 0, 
			&sndrcvinfo, &flags);
		if (recevd_bytes > 0)
		{     
			m3ua_message[recevd_bytes] = 0;
			/* Peer State */
			if(m3ua_message[message_class] 
				== M3UA_MSG_CLASS_ASPSM && 
				m3ua_message[message_type] 
				== M3UA_MSG_TYPE_ASPSM_UP)
			{
				peer_state = asp_up_tx;

				m3ua_message_length = 
				create_m3ua_ASPSM (m3ua_message, 
					M3UA_MSG_TYPE_ASPSM_UPACK);
				/* Send ASP_UP message to the peer */
				ret = sctp_sendmsg 
					(global_sock_fd, (void *)m3ua_message, 
					m3ua_message_length, NULL, 0, 
					htonl(3), 0, 
					CONTROL_STREAM, 0, 0);
				printf ("peer asp up, sending ack...\n");
				peer_state = asp_up_ack_tx;
			}
			/* Host State */
			if(m3ua_message[message_class] 
				== M3UA_MSG_CLASS_ASPSM && 
				m3ua_message[message_type] 
				== M3UA_MSG_TYPE_ASPSM_UPACK) {

				printf ("host asp up acked...\n");
				host_state = asp_up_ack_tx;
			}

			/* Peer State */
			else if (m3ua_message[message_class] 
				== M3UA_MSG_CLASS_ASPTM && 
				m3ua_message[message_type]
				== M3UA_MSG_TYPE_ASPTM_ACT) {
				peer_state = asp_active_tx;
				m3ua_message_length = create_m3ua_ASPTM (m3ua_message, 
									M3UA_MSG_TYPE_ASPTM_ACTACK, rounting_context);
				/* Send ASP_ACTIVE message to the peer */
				ret = sctp_sendmsg (global_sock_fd, (void *)m3ua_message, 
						m3ua_message_length, NULL, 0, 
						htonl (3), 0, CONTROL_STREAM, 0, 0);
				printf ("peer asp active acked...\n");
				peer_state = asp_active_ack_tx;
			}
			/* Received ASP_ACTIVE_ACK message */
			else if (m3ua_message[message_class]	
				== M3UA_MSG_CLASS_ASPTM && 
				m3ua_message[message_type]	
				== M3UA_MSG_TYPE_ASPTM_ACTACK) {
				printf ("host asp active acked...\n");
				host_state = asp_active_ack_tx;
			}
			else {
				usleep (1);
				continue;
			}
		}
		else {
			usleep (10);
			continue;
		}
	}
}




void do_sg_handshake (char *routing_context)	
{
	int ret;
	struct sctp_sndrcvinfo sndrcvinfo;

	char status_type = 0;
	char status_info = 0;
	//char routing_context[8] = {0};	

	while (1) {

		recevd_bytes = sctp_recvmsg (global_sock_fd, 
			(void *)m3ua_message, 400, 
			(struct sockaddr *)NULL, 0, 
			&sndrcvinfo, &flags);

		if(m3ua_message[message_class] == 0x00 && m3ua_message[message_type] == 0x01)	/*If Notify packet*/
		{
			//collect Routing context
			memcpy(&status_type, &m3ua_message[13], 1);
			memcpy(&status_info, &m3ua_message[15], 1);
		
			if(status_type == 0x01 && status_info == 0x02){
				if(m3ua_message[17] == 0x06){
					memcpy(routing_context, &m3ua_message[20], 4);
					printf("The routing context is %s", routing_context);
				}
			}
	
			m3ua_message_length = create_m3ua_ASPTM (m3ua_message, 
				M3UA_MSG_TYPE_ASPTM_ACT, routing_context);
			
			/* Send ASP_ACTIVE message to the peer */
			ret = sctp_sendmsg (global_sock_fd, (void *)m3ua_message, 
					m3ua_message_length, NULL, 0, 
					htonl (3), 0, CONTROL_STREAM, 0, 0);
			host_state = asp_active_tx;
			printf ("host asp active...\n");

			recevd_bytes = sctp_recvmsg (global_sock_fd, 
				(void *)m3ua_message, 400, 
				(struct sockaddr *)NULL, 0, 
				&sndrcvinfo, &flags);
			
			if(m3ua_message[message_class] == 0x04 && m3ua_message[message_type] == 0x03){	/*If Active Ack*/
				/*	m3ua_message_length = create_m3ua_DAUD (m3ua_message, 
                	M3UA_MSG_TYPE_SNM_DAUD, routing_context);
					ret = sctp_sendmsg (global_sock_fd, (void *)m3ua_message, 
					m3ua_message_length, NULL, 0,  
					htonl (3), 0, CONTROL_STREAM, 0, 0);*/
				recevd_bytes = sctp_recvmsg (global_sock_fd, 
					(void *)m3ua_message, 400, 
					(struct sockaddr *)NULL, 0, 
					&sndrcvinfo, &flags);

				if(m3ua_message[message_class] == 0x00 && m3ua_message[message_type] == 0x01){
					memcpy(&status_type, &m3ua_message[13], 1);
					memcpy(&status_info, &m3ua_message[15], 1);
					if(status_type == 0x01 && status_info == 0x03){
						m3ua_message_length = create_m3ua_DAUD (m3ua_message, 
                		M3UA_MSG_TYPE_SNM_DAUD, routing_context);
						ret = sctp_sendmsg (global_sock_fd, (void *)m3ua_message, 
						m3ua_message_length, NULL, 0,  
						htonl (3), 0, CONTROL_STREAM, 0, 0);
						printf("Handshake complete");
						sleep(2);
						break;
					}
				}
			}
		}
	}
}
