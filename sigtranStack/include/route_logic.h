#ifndef __ROUTE_LOGIC_H__
#define __ROUTE_LOGIC_H__

#include "common.h"
#include "sccp.h"
#include "m3ua_nodes.h"

/*
 * register an m3ua_as node
 * to be used to route on\
 * its  gt, pc or ssn
 */
void register_route (m3ua_as *);

/*
 * resolve a route:
 *
 * put the route attributes of sccp
 * in sccp_route structure.
 *
 * set m3ua_as as NULL, after resolution,
 * as_node should contain a value
 */
int resolve_route (sccp_route *, m3ua_as *);

/*
 * close all connection when 
 * handling a signal or have to stop
 * for something
 */
void shed_connections (void);

#endif
