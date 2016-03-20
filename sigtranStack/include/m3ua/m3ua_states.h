#ifndef __M3UA_STATES_H__
#define __M3UA_STATES_H__

/*
 * state declaration
 * between two ASPs to establish a
 * m3ua connection.
 */
typedef enum {
	asp_init = 0,
	asp_up_tx,
	asp_up_ack_tx,
	asp_up_done,
	asp_active_tx,
	asp_active_ack_tx,
	asp_active_done,
	asp_inactive_tx,
	asp_inactive_ack_tx,
	asp_inactive_done,
	asp_down_tx,
	asp_down_ack_tx,
	asp_handshake_complete,
}m3ua_asp_states;

/*
 * state declaration
 * between signalling gateway
 * and ASP to establish a
 * m3ua connection.
 */
typedef enum {
	asp_sg_init = 0,
	asp_sg_up_tx,
	asp_sg_up_ack_tx,
	asp_sg_notify_tx,
	asp_sg_up_done,
	asp_sg_active_tx,
	asp_sg_active_ack_tx,
	asp_sg_active_done,
	asp_sg_inactive_tx,
	asp_sg_inactive_ack_tx,
	asp_sg_inactive_done,
	asp_sg_down_tx,
	asp_sg_down_ack_tx,
	asp_sg_DAUD_tx,
	asp_sg_handshake_complete,	
}m3ua_sg_states;

#endif /* m3ua_states.h */
