#include "common.h"
#include<libmemcached/memcached.h>

#define MAIN_SERVER 	1
#define HLR_SERVER	2
#define DLR_SERVER	3

int init_memcached (void *);
int insert_memcached (void *);
char* get_memcached (void *);
int select_memcached (void *);
int append_memcached (void *);
int delete_memcached (void *);
