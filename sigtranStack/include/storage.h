#include "common.h"
#include "storage_mysql.h"
#include "storage_memcached.h"
#include "iniparser.h"

/* Storage types */
#define MYSQL_DB	0
#define MEMCACHED 	1
#define CACHING 	2

/* Operation Tag */
#define UPDATE 		0
#define INSERT		1
#define DELETE		2
#define SELECT		3

int storage_type;

typedef struct {
	int num_table;
	char *table_name;
	int (*initialize) (void *params);
	int (*insert) (void *params);
	int (*update) (void *params);
	int (*delete) (void *params);
	int (*select) (void *params);
	int (*append) (void *params);
	char* (*get_value)(void *params);
}storage_cb;

storage_cb callbacks;
	
typedef struct {
	char database[20];
	char username[20];
	char password[20];
	char server[30];
	int port;
}storage_params;

storage_params storage_values;


/* read the configuration */
//storage_type = read_cfg ("config.ini");
/*initialized storage params by reading the config file */
static void init_callbacks (int storage_type);



int dlr_call (int , void *);
int init_storage ();
int read_configuration ();
