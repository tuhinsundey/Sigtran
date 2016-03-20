#include "socket_layer.h"

/*************************************************************************************
    * Function : disable_nagle 
    * Input    : sock_fd
    * Output   : return vlaue
    * Purpose  : to disable nagle's algorithm
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void disable_nagle (int sock_fd)
{
	int nagle_flag = 1;
	int ret = setsockopt (sock_fd, IPPROTO_SCTP, SCTP_NODELAY, 
		(void *)&nagle_flag, sizeof (int));
	if (ret) {
		printf ("error with setsockopt SCTP_NODELAY... error code %d\n",
			errno);
	}
}

/*************************************************************************************
    * Function : disable_linger
    * Input    : sock_fd
    * Output   : return value
    * Purpose  : disable lingering
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void disable_linger (int sockf_fd)
{
	struct linger linger_params;
	linger_params.l_onoff = 1;
	linger_params.l_linger = 0;
	
	int ret = setsockopt (sockf_fd, SOL_SOCKET, SO_LINGER, 
		&linger_params, sizeof (linger_params));
	if (ret) {
		printf ("error with setsockopt SO_LINGER... error code %d\n",
			errno);
	}
}

/*************************************************************************************
    * Function : set_stream
    * Input    : sock_fd
    * Output   : return value
    * Purpose  : to set streams to sctp socket 
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void set_streams (int sock_fd)
{
	struct sctp_initmsg initmsg;
	
	memset (&initmsg, 0, sizeof(initmsg));
	initmsg.sinit_num_ostreams  = 3;
	initmsg.sinit_max_instreams = 4;
	initmsg.sinit_max_attempts  = 4;
	int ret = setsockopt (sock_fd, IPPROTO_SCTP, SCTP_INITMSG, 
		&initmsg, sizeof (initmsg));
	if (ret < 0) {   
		printf ("setsockopt for SCTP_INITMSG failed... error code %d\n", 
			errno);	
	}   
}

/*************************************************************************************
    * Function : set_sctp_events
    * Input    : sock_fd
    * Output   : return
    * Purpose  : enable watching events
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void set_sctp_events (int sock_fd)
{
	struct sctp_event_subscribe events;
	int ret = setsockopt (sock_fd, SOL_SCTP, SCTP_EVENTS, 
			(const void *)&events, sizeof(events));
	if (ret < 0) {   
		printf ("setsockopt for SCTP_EVENTS failed... error code %d\n", 
			errno);	
	}   
}

/*************************************************************************************
    * Function : set_recv_timeout
    * Input    : sock_fd
    * Output   : return
    * Purpose  : set timeout value for recv.
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static set_recv_timeout (int sock_fd)
{
	int ret;	
	struct timeval recv_timeout;
	recv_timeout.tv_sec		=	60;
  	recv_timeout.tv_usec	= 	3 * 1000;
	ret = setsockopt (sock_fd, SOL_SOCKET, 
		SO_RCVTIMEO, (struct timeval *)&recv_timeout, sizeof (struct timeval));
	if (ret < 0) {   
		printf ("setsockopt for SO_RCVTIMEO failed... error code %d\n", 
			errno);	
	}   
}

/*************************************************************************************
    * Function : create_socket
    * Input    : sock_fd
    * Output   : return
    * Purpose  : create a socket from m3ua layer.
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int create_socket (m3ua_asp *m3ua_params)
{
	int ret = 0;
	struct sctp_sndrcvinfo sndrcvinfo;

	m3ua_params->conn_id = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP); 	

	if (m3ua_params->conn_id < 0)	
	{
		printf ("error creating socket... exiting\n");
		exit (0);
	}

	set_sctp_events (m3ua_params->conn_id);
	set_streams (m3ua_params->conn_id);

	ret = bind (m3ua_params->conn_id, (struct sockaddr *)&m3ua_params->host, 
			sizeof (struct sockaddr_in));

	ret = connect (m3ua_params->conn_id, (struct sockaddr *)&m3ua_params->peer, 
		sizeof (struct sockaddr_in));

	if (ret)
	{
		printf ("unable to connect... error code %d\n", errno);
		exit (0);
	}	

	disable_nagle (m3ua_params->conn_id);

	/** set the sock fd in blocking mode **/	
	int fd_flag;
	fd_flag = fcntl (m3ua_params->conn_id, F_GETFL);
	fd_flag &= ~O_NONBLOCK;
	fcntl (m3ua_params->conn_id, F_SETFL, fd_flag);

	set_recv_timeout (m3ua_params->conn_id);

	return SUCCESS;
}

/*************************************************************************************
    * Function : send_m3ua_msg
    * Input    : m3ua_as pointer, stream ID
    * Output   : return
    * Purpose  : send a message over sctp socket.
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int send_m3ua_msg (m3ua_as *as_node, int stream)
{
	int ret;
	ret = sctp_sendmsg (as_node->parent->conn_id, (void *) as_node->msg_ptr, 
		as_node->msg_len, NULL, 0, PROTO_M3UA, 0, stream, 0, 0);
	if (ret < 0)
		printf ("unable to send ... error code %d\n", errno);

	return ret;
}

/*************************************************************************************
    * Function : recv_m3ua_msg
    * Input    : m3ua_as pointer
    * Output   : return
    * Purpose  : receive a message over sctp socket.
    * Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
int recv_m3ua_msg (m3ua_as *as_node)
{
	int ret, flags;
	struct sctp_sndrcvinfo sndrcvinfo;

	ret = sctp_recvmsg (as_node->parent->conn_id, (void *) as_node->msg_ptr, 
	    as_node->msg_len, (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags);

	//if (ret < 0)
	//	printf ("unable to receive... error code %d\n", errno);
	return ret;
}
