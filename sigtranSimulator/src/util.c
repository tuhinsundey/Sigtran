
//#include <config.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif /* HAVE_SYSLOG_H */
#include <unistd.h>
#include <time.h>
#include "util.h"

#define MAXSYSLOGMSGLEN 2048	
#ifndef NDEBUG
unsigned gsmsc_debug_facilities = 0xffff;
int gsmsc_debug_level = 0;
#endif

static const char *global_ident = NULL;
static log_function_type *current_log_function = log_file;
static FILE *current_log_file = NULL;

void
log_init(const char *ident)
{
	global_ident = ident;
	current_log_file = stderr;
}

void
log_open(const char *filename)
{
	if (filename) {
		FILE *file = fopen(filename, "a");
		if (!file) {
			log_msg(LOG_ERR, "Cannot open %s for appending, logging to stderr",
				filename);
		} else {
			current_log_file = file;
		}
	}
}

void
log_finalize(void)
{
#ifdef HAVE_SYSLOG_H
	closelog();
#endif /* HAVE_SYSLOG_H */
	if (current_log_file != stderr) {
		fclose(current_log_file);
	}
	current_log_file = NULL;
}

static lookup_table_type log_priority_table[] = {
	{ LOG_ERR, "error" },
	{ LOG_WARNING, "warning" },
	{ LOG_NOTICE, "notice" },
	{ LOG_INFO, "info" },
	{ 0, NULL }
};

lookup_table_type *
lookup_by_id(lookup_table_type *table, int id)
{
    while (table->name != NULL) {
        if (table->id == id)
            return table;
        table++;
    }
    return NULL;
}

struct tm gw_localtime(time_t t)
{   
    struct tm tm;
    
#ifndef HAVE_LOCALTIME_R
   // lock(GWTIME);
//    tm = *localtime(&t);
   // unlock(GWTIME);
    tm = *localtime(&t);
#else   
    localtime_r(&t, &tm);
#endif  

    return tm;
}

void
log_file(int priority, const char *message)
{
	size_t length;
	lookup_table_type *priority_info;
	const char *priority_text = "unknown";
	time_t	t;
	struct tm tm;
	char *p, prefix[1024] = {0};
	
	p = prefix;
	
	assert(global_ident);
	assert(current_log_file);

	priority_info = lookup_by_id(log_priority_table, priority);
	if (priority_info) {
		priority_text = priority_info->name;
	}

	time(&t);
	tm = gw_localtime(t);

	sprintf(p, "%04d-%02d-%02d %02d:%02d:%02d ",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    p = strchr(p, '\0');
	
	/* Bug #104, add time_t timestamp */
	fprintf(current_log_file, "[%s] %s[%d]: %s: %s", prefix, global_ident, (int) getpid(), priority_text, message);
	length = strlen(message);
	if (length == 0 || message[length - 1] != '\n') {
		fprintf(current_log_file, "\n");
	}
	fflush(current_log_file);
}



void
log_syslog(int priority, const char *message)
{
#ifdef HAVE_SYSLOG_H
	syslog(priority, "%s", message);
#endif /* !HAVE_SYSLOG_H */
	log_file(priority, message);
}

void
log_set_log_function(log_function_type *log_function)
{
	current_log_function = log_function;
}

void
log_msg(int priority, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	log_vmsg(priority, format, args);
	va_end(args);
}


void
log_vmsg(int priority, const char *format, va_list args)
{
	char message[MAXSYSLOGMSGLEN];
	vsnprintf(message, sizeof(message), format, args);
	current_log_function(priority, message);
}

/*
void *
xalloc_zero(size_t size)
{
	void *result = xalloc(size);
	memset(result, 0, size);
	return result;
}


void *
xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (!ptr) {
		log_msg(LOG_ERR, "realloc failed: %s", strerror(errno));
		exit(1);
	}
	return ptr;
}
*/
int
write_data(FILE *file, const void *data, size_t size)
{
	size_t result;

	if (size == 0)
		return 1;
	
	result = fwrite(data, 1, size, file);

	if (result == 0) {
		log_msg(LOG_ERR, "write failed: %s", strerror(errno));
		return 0;
	} else if (result < size) {
		log_msg(LOG_ERR, "short write (disk full?)");
		return 0;
	} else {
		return 1;
	}
}

int
timespec_compare(const struct timespec *left,
		 const struct timespec *right)
{
	/* Compare seconds.  */
	if (left->tv_sec < right->tv_sec) {
		return -1;
	} else if (left->tv_sec > right->tv_sec) {
		return 1;
	} else {
		/* Seconds are equal, compare nanoseconds.  */
		if (left->tv_nsec < right->tv_nsec) {
			return -1;
		} else if (left->tv_nsec > right->tv_nsec) {
			return 1;
		} else {
			return 0;
		}
	}
}

