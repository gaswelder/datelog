#ifndef __dlog_h
#define __dlog_h

struct dlog;
typedef struct dlog *dlog_t;

dlog_t dlog_init( const char *pername, const char *dir,
	const char *static_name );

int dlog_put( dlog_t log, const char *line );
void dlog_free( dlog_t log );

#endif
