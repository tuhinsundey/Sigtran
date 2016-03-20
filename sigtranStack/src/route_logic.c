#include "route_logic.h"

/** list of all nodes **/
m3ua_as *route_nodes[10] = {NULL};
static int num_routes = 0;


void register_route (m3ua_as *as_node)
{
	route_nodes[num_routes] = as_node;
	++num_routes;
	printf ("number of routes = %d\n", num_routes);
}

static m3ua_as *resolve_ssn (int ssn)
{
	m3ua_as *resolved_as = NULL;
	int iter = 0;
	for (iter; iter <= num_routes; iter++)
	{
		if (ssn == route_nodes[iter]->destn->ssn) {
			resolved_as = route_nodes[iter];
			break;	
		}
	}	
	return resolved_as;
}

static m3ua_as *resolve_pc (char *point_code)
{
	m3ua_as *resolved_as = NULL;
	int iter = 0;
	for (iter; iter <= num_routes; iter++)
	{
		if (!strcmp (point_code, 
				route_nodes[iter]->destn->point_code)) {
			resolved_as = route_nodes[iter];
			break;	
		}
	}	
	return resolved_as;
}

static m3ua_as *resolve_gt (char *global_title)
{
	m3ua_as *resolved_as = NULL;
	int iter = 0;
	for (iter; iter <= num_routes; iter++)
	{
		if (!strcmp (global_title, 
				route_nodes[iter]->destn->global_title)) {
			resolved_as = route_nodes[iter];
			break;	
		}
	}	
	return resolved_as;
}


/*
 *In performing GTT, an STP does not need 
 *to know the exact final destination of a message. 
 *It can, instead, perform intermediate GTT, in which 
 *it uses its tables to find another STP further 
 *along the route to the destination. That STP, in turn, 
 *can perform final GTT, routing the message 
 *to its actual destination. Intermediate GTT 
 *minimizes the need for STPs to maintain extensive 
 *information about nodes that are far removed from them. 
 *GTT also is used at the STP to share load among mated 
 *SCPs in both normal and failure scenarios. In these 
 *instances, when messages arrive at an STP for final 
 *GTT and routing to a database, the STP can select from 
 *among available redundant SCPs. It can select an SCP on 
 *either a priority basis (referred to as primary backup) 
 *or so as to equalize the load across all available SCPs (
 *referred to as load sharing).
 */
int resolve_route (sccp_route *route_ptr, m3ua_as *as_node)
{
	if (!route_ptr)
		return INVALID_PTR;
  
	if (!route_ptr->routing_indicator)
		return ROUTE_TYPE_ABSENT;

	m3ua_as *ret = NULL;
	switch (route_ptr->routing_indicator)
	{	
		case ROUTE_ON_GT:
			if (!(ret = resolve_gt (route_ptr->gt)))
			{
				if (route_ptr->ssn_present && 
					!(ret = resolve_ssn (route_ptr->ssn)))
				{
					printf ("error... unable to resolve GT\n");
					return GT_UNRESOLVED;
				}	
			}				
			break;
		case ROUTE_ON_SSN:
			if (!(ret = resolve_pc (route_ptr->pc)))
			{
				if (route_ptr->ssn_present && 
					!(ret = resolve_ssn (route_ptr->ssn)))
				{
					printf ("error... unable to resolve point_code");
					return GT_UNRESOLVED;
				}	
			}				
			break;
		default:
			return INVALID_ROUTE_TYPE;        
	}
	as_node = ret;
	return ROUTE_RESOLVED;
}

void shed_connections ()
{
	int conn = 0;
	for (conn; conn < num_routes; conn++)
	{
		route_nodes[conn]->host_state.sg_state = asp_sg_inactive_tx;
		m3ua_send_inact (route_nodes[conn]);
		short_delay;
		route_nodes[conn]->host_state.sg_state = asp_sg_down_tx;
		m3ua_send_aspdwn (route_nodes[conn]);
		short_delay;
	}
	exit (0);
}

