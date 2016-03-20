#include "m3ua_states.h"
#include "m3ua_nodes.h"

#define MESSAGE_CLASS_POS               2
#define MESSAGE_TYPE_POS                3   
#define SIZE_STATUS_PARAMS              2
#define NOTIFY_STATUS_OFFSET            9       
#define STATUS_TYPE_OFFSET              13
#define STATUS_INFO_OFFSET              15
#define AS_STATE_CHANGE                 0x01
#define AS_INACTIVE						0x02    
#define MAX_ROUTING_CONTEXT_LEN         20
#define ROUTING_CONTEXT_BEGIN           19  
#define APPLICATION_SERVER_ACTIVE       0x03
#define ROUTING_CTXT_TAG_POS            17

#define TAG_UA_INFO_STRING              0x0004
#define TAG_UA_ROUTING_CONTEXT          0x06
#define TAG_UA_DIAGNOSTIC_INFO          0x0007
#define TAG_UA_HEARTBEAT_DATA           0x0009
#define TAG_UA_TRAFFIC_MODE_TYPE        0x000b
#define TAG_UA_ERROR_CODE               0x000c
#define TAG_UA_STATUS                   0x000d
#define TAG_UA_ASP_IDENTIFIER           0x0011
#define TAG_UA_AFFECTED_POINT_CODE      0x0012
#define TAG_UA_CORRELATION_ID           0x0013


/*
 * perform handshake using this API
 */
void m3ua_handshake (m3ua_as *);

/*
 * Send data over m3ua
 */
void m3ua_transport (m3ua_as *);


/*
 * Send DAUDT message over m3ua
 */
void m3ua_do_audit(m3ua_as *);

/* 
 * Send ASP Inactive data
 */
void m3ua_send_inact (m3ua_as *); 

/* 
 * Send ASP Down data
 */
void m3ua_send_aspdwn (m3ua_as *); 

/* 
 * Analyse data packet in m3ua layer 
 */
int m3ua_analyze_data (m3ua_as *, unsigned char *);

/* 
 * Analyse ssnm packet
 */
void m3ua_analyze_ssnm (m3ua_as *as_node);

