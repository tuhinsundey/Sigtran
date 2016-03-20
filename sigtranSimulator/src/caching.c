#include "caching.h"
#include "iniparser.h"
#include "dictionary.h"	
/* server connection globals */
static int cached_lib_fd;
struct sockaddr_in cached_server;
struct sockaddr_in local_addr;

const char *cachelib = "libgcache.so";
char *basic_query = "insert send ";
static void *handle = NULL;

static char caching_ip[30];
static int caching_port;	

/* library objects */
caching_callbacks caching_cb;

#if 0
getNewJdbObject getNewJdbObj_callback; 
addPacketS addPacketsStr_callback; 
addPacketNS addPacketsStrn_callback;
addPacketI ff;
addPacketI fb;
addPacketI addPacketsInt_callback; 
encodeJdbObjectWL encodeJdbObjWL_callback; 
encodeJdbObject encodeJdbObj_callback; 
destroyJdbObject destroy_callback; 
decodeJdbObject dc;
getValueAt gv;
#endif 

/*************************************************************************************
* Function : insert()
* Input    : sms_entry
* Output   : 
* Purpose  : Insert data in cache
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void insert (sms_fields *sms_entry)
{
	char msg[256];

	void *objq = caching_cb.getNewJdbObject();
	void *obj = caching_cb.getNewJdbObject();
	char insert_query[250] = {0};
	
	strcpy (insert_query, basic_query);
	strcat (insert_query, sms_entry->global_msg_id);

	caching_cb.addPacketS (obj, sms_entry->global_msg_id);
	caching_cb.addPacketS (obj, (char *)(sms_entry->SM_RP_DA).number);
	caching_cb.addPacketS (obj, (char *)(sms_entry->SM_RP_OA).number);
	caching_cb.addPacketS (obj, sms_entry->sms_data);

	caching_cb.addPacketS (obj, "58829");
	caching_cb.addPacketI (obj, 1);
	caching_cb.addPacketI (obj, 1);
	caching_cb.addPacketI (obj, 1);
	caching_cb.addPacketI (obj, 1);
	caching_cb.addPacketI (obj, 1);
	caching_cb.addPacketI (obj, 1);
	caching_cb.addPacketI (obj, 1);
	
	caching_cb.addPacketC (obj, "000007000000000R");
	
	caching_cb.addPacketC (objq, insert_query);

	char *objectWithLength = caching_cb.encodeJdbObjectWL (obj, 1);
	caching_cb.addPacketC (objq, objectWithLength);

	int length = 0;
	char *objectWithoutLength = caching_cb.encodeJdbObject (objq , 0, &length);

	send (cached_lib_fd, (char *)objectWithoutLength, length, 0);
	recv (cached_lib_fd, msg, 1 ,0);

	caching_cb.destroyJdbObject (obj);
	caching_cb.destroyJdbObject (objq);
	freeze (objectWithLength);
	freeze (objectWithoutLength);

	memset (msg, 0, 256);	
}


/*************************************************************************************
* Function : cached_lib_init
* Input    : none
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void cached_lib_init ()
{
	/* initialize api lib */
	if (!(handle = dlopen (cachelib, RTLD_LAZY)))
	{
		printf ("unable to open dynamic lib...\n");
		exit (0);
	}

	/* open sock with server */
	cached_lib_fd = socket (AF_INET, SOCK_STREAM, 0);
	memset (&cached_server, 0, sizeof (cached_server));
	cached_server.sin_family = AF_INET;
	cached_server.sin_port = htons (caching_port);
	cached_server.sin_addr.s_addr = inet_addr (caching_ip);

	/* try connectin it with the server */
	if (connect (cached_lib_fd, (struct sockaddr *)&cached_server, sizeof (cached_server)))
	{
		printf ("couldn't connect with the server...\n");
		exit (0);
	}
	
	char *error;

	/* initialize callbacks */
	caching_cb.getNewJdbObject = dlsym (handle, "getNewJdbObject");
    if ((error = dlerror()) != NULL)  {
       fprintf(stderr, "%s\n", error);
       exit(EXIT_FAILURE);
    }

	caching_cb.addPacketS = dlsym (handle, "addPacketS");
    if ((error = dlerror()) != NULL)  {
       fprintf(stderr, "%s\n", error);
       exit(EXIT_FAILURE);
    }
	
	caching_cb.addPacketI = dlsym (handle, "addPacketI");
    if ((error = dlerror()) != NULL)  {
       fprintf(stderr, "%s\n", error);
       exit(EXIT_FAILURE);
    }
	
	caching_cb.encodeJdbObjectWL = dlsym (handle, "encodeJdbObjectWL");
    if ((error = dlerror()) != NULL)  {
       fprintf(stderr, "%s\n", error);
       exit(EXIT_FAILURE);
    }
	
	caching_cb.encodeJdbObject = dlsym (handle, "encodeJdbObject");
    if ((error = dlerror()) != NULL)  {
       fprintf(stderr, "%s\n", error);
       exit(EXIT_FAILURE);
    }
	
	caching_cb.destroyJdbObject = dlsym (handle, "destroyJdbObject");
    if ((error = dlerror()) != NULL)  {
       fprintf(stderr, "%s\n", error);
       exit(EXIT_FAILURE);
    }
}


/*************************************************************************************
* Function : read_configuration_file()
* Input    : ini
* Output   : 
* Purpose  : To read cache configuration
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
static void read_configuration_file(dictionary *ini)
{
	char local_name[25] = {0};
	char *get_conf = NULL;	
		
	sprintf (local_name, "%s", "cache:ip");
	get_conf = iniparser_getstring (ini, local_name, NULL);
	memcpy (caching_ip, get_conf, strlen (get_conf));
	caching_port  = iniparser_getint (ini, "cache:port", 0);
	log_msg (LOG_INFO, "starting with caching server at %s:%d", 
			caching_ip, caching_port);	
}


/*************************************************************************************
* Function : init_caching()
* Input    : none
* Output   : 
* Purpose  : 
* Author   : (tuhin.shankar.dey@gmail.com)
**************************************************************************************/
void init_caching (dictionary *ini)
{
	read_configuration_file(ini);
	cached_lib_init ();

}

