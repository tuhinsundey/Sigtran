#ifndef __SOCKET_LAYER_H__
#define __SOCKET_LAYER_H__

#include "common.h"
#include "m3ua_nodes.h"

#define PROTO_M3UA		htonl (3)
#define PROTO_SUA		htonl (4)


/*
 * create a socket with parameters
 * as specified in the argument passed,
 * set appropriate socket options 
 * for sctp. Return errors if any.
 */
int create_socket (m3ua_asp *);

/*
 * send m3ua message over sctp.
 */
int send_m3ua_msg (m3ua_as *, int);

#endif /*eof socket_layer.h*/


