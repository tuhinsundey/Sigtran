#ifndef _UTIL_H_
#define _UTIL_H_

//#include <config.h>

#include <sys/time.h>
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>

/* Whether the C compiler accepts the "format" attribute */
#define HAVE_ATTR_FORMAT 1

#define ALIGN_UP(n, alignment)  \
	(((n) + (alignment) - 1) & (~((alignment) - 1)))
#define PADDING(n, alignment)   \
	(ALIGN_UP((n), (alignment)) - (n))

#ifdef HAVE_ATTR_FORMAT
#define ATTR_FORMAT(archetype, string_index, first_to_check) \
    __attribute__ ((format (archetype, string_index, first_to_check)))
#else /* !HAVE_ATTR_FORMAT */
#define ATTR_FORMAT(archetype, string_index, first_to_check) /* empty */
#endif /* !HAVE_ATTR_FORMAT */
#if defined(__cplusplus)
#define ATTR_UNUSED(x)
#elif defined(HAVE_ATTR_UNUSED)
#define ATTR_UNUSED(x)  x __attribute__((unused))
#else /* !HAVE_ATTR_UNUSED */
#define ATTR_UNUSED(x)  x
#endif /* !HAVE_ATTR_UNUSED */

/*
 * Initialize the logging system.  All messages are logged to stderr
 * until log_open and log_set_log_function are called.
 */
void log_init(const char *ident);

/*
 * Open the system log.  If FILENAME is not NULL, a log file is opened
 * as well.
 */
void log_open(const char *filename);

/*
 * Finalize the logging system.
 */
void log_finalize(void);

/*
 * Type of function to use for the actual logging.
 */
typedef void log_function_type(int priority, const char *message);

/*
 * The function used to log to the log file.
 */
log_function_type log_file;

/*
 * The function used to log to syslog.  The messages are also logged
 * using log_file.
 */
log_function_type log_syslog;

/*
 * Set the logging function to use (log_file or log_syslog).
 */
void log_set_log_function(log_function_type *log_function);

/*
 * Log a message using the current log function.
 */
void log_msg(int priority, const char *format, ...)
	ATTR_FORMAT(printf, 2, 3);

/*
 * Log a message using the current log function.
 */
void log_vmsg(int priority, const char *format, va_list args);


/*
 * (Re-)allocate SIZE bytes of memory.  Report an error if the memory
 * could not be allocated and exit the program.  These functions never
 * returns NULL.
 */
void *xalloc(size_t size);
void *xalloc_zero(size_t size);
void *xrealloc(void *ptr, size_t size);

/*
 * Write SIZE bytes of DATA to FILE.  Report an error on failure.
 *
 * Returns 0 on failure, 1 on success.
 */
int write_data(FILE *file, const void *data, size_t size);

/* A general purpose lookup table */
typedef struct lookup_table lookup_table_type;
struct lookup_table {
    int id;
    const char *name;
};

/*
 *  * Looks up the table entry by name, returns NULL if not found.
 *   */
lookup_table_type *lookup_by_name(lookup_table_type table[], const char *name);

/*
 *  * Looks up the table entry by id, returns NULL if not found.
 *   */
lookup_table_type *lookup_by_id(lookup_table_type table[], int id);


/*
 * Print debugging information using fprintf(3).
 */
#define DEBUG_PARSER           0x0001U
#define DEBUG_ZONEC            0x0002U
#define DEBUG_QUERY            0x0004U
#define DEBUG_DBACCESS         0x0008U
#define DEBUG_NAME_COMPRESSION 0x0010U

#ifdef NDEBUG
#define DEBUG(facility, level, args)  /* empty */
#else
extern unsigned gsmsc_debug_facilities;
extern int gsmsc_debug_level;
#ifndef DEBUG
#define DEBUG(facility, level, args)				\
	do {							\
		if ((facility) & smpp_debug_facilities &&	\
		    (level) <= smpp_debug_level) {		\
			fprintf args ;				\
		}						\
	} while (0)
#endif
#endif

#endif /* _UTIL_H_ */
