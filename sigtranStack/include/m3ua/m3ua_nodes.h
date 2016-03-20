#ifndef __M3UA_NODES_H__
#define __M3UA_NODES_H__

#include "m3ua_commons.h"
#include "m3ua_states.h"

#define DEFAULT_ORIG_PC		1234
#define DEFAULT_DEST_PC		3456
#define DEFAULT_GT			"919292929292"

/*** Heart_Beat message  ***/
#define HTBT_MSG_LEN_POS	7
#define HTBT_PRM_LEN_POS	11
#define MIN_HTBT_MSG_LEN	8	
#define HTBT_DATA_POS		12		
#define HTBT_PRM_LEN		4	
	
/* 
 * enum types to define
 * the type of node
 * being used
 */
typedef enum{
	ASP=0,
	SGW,
	IPSP,
}m3ua_node_type;

/*
 * global tile type/routing indicators
 */
struct global_title_type {
	int pc;
	int ssn;
	char point_code[POINT_CODE_LEN];
	char global_title[GT_LEN];
	//struct global_title_type *next_pc;
};

typedef struct global_title_type global_title_type; 

/*
 * params to be used for connection
 */
struct m3ua_asp {
	int num_as;
	int conn_id;
	struct sockaddr_in host;
	struct sockaddr_in peer;
	struct m3ua_as *local_as;
};

typedef struct m3ua_asp m3ua_asp;

/*
 * m3ua states wrt the
 * type of peer
 */
union m3ua_states {
	m3ua_asp_states asp_state;
	m3ua_sg_states	sg_state;
};

typedef union m3ua_states m3ua_states;

/*
 * params maintained by 
 * each as to send/receive 
 * m3ua message from peer
 */
struct m3ua_as {

	/* specify type of the peer */
	m3ua_node_type peer_type;

	/* host and peer states */
	m3ua_states host_state;
	m3ua_states peer_state;

	/* type and class of message */
	int message_type;
	int message_class;

	/* routing keys to be used in m3ua packet */
	char routing_context[ROUTING_CTXT_LEN];
	int  routing_context_len;
	char network_appearance[NW_APRNCE_LEN];
	int  network_appearance_len;
	char corr_id[CORR_ID_LEN];
	int corr_id_len;

	/* dest/source info */
	int num_dest;
	global_title_type *origin;
	global_title_type *destn;

	/* pointer to parent asp node */
	m3ua_asp *parent;

	//struct m3ua_as *next_as;
	/* pointer to message attributes */
	unsigned char *msg_ptr;
	int msg_len;
	
	/*  management error no  */
        int mgmt_error;

	/* heartbeat data */
	char HTBT_data[MAX_HTBT_DATA_LEN];
	int HTBT_data_len;
};

typedef struct m3ua_as m3ua_as;

/*
 * read m3ua parameters 
 * and initialize the socket layer 
 * for m3ua
 */
void m3ua_init (m3ua_asp *, char *); 

/*
 * read m3ua AS parameters
 * before initialization or 
 * AS handshake
 */
void m3ua_as_config (m3ua_as *, char *);

/*
 * wrapper function over 
 * sctp_recvmsg to receive 
 * messages at m3ua layer
 */
int m3ua_recv (m3ua_as *);

/*
 * wrapper function over 
 * sctp_sendmsg to send
 * messages at m3ua layer
 */
int m3ua_send (m3ua_as *);

/*
 * Handling received aspsm
 * packet	
 */
void m3ua_aspsm_hdlr (m3ua_as *);

/*
 * function to receive
 * data message at m3ua layer 
 * and pass on to ulp.
 */
int m3ua_recv_data (m3ua_as *, unsigned char *);


#endif /* eof m3ua_nodes.h */
