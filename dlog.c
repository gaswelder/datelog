#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "lib/fileutil.h"

#define MAXPATH 255


struct period {
	const char *name;
	const char *format;
};

static struct period periods[] = {
	{ "month", "%Y-%m" },
	{ "day", "%Y-%m-%d" },
	{ "hour", "%Y-%m-%d-%H" },
	{ "minute", "%Y-%m-%d-%H-%M" },
	{ "second", "%Y-%m-%d-%H-%M-%S" }
};


struct dlog {
	struct period *per;
	char *dir;
	char *static_path;
	char current_path[MAXPATH];
	FILE *fp;
};

typedef struct dlog *dlog_t;



static int format_path( struct dlog *log, char *buf, size_t len );
static struct period *find_period( const char *pername );
static int check_dir( const char *path );
char *create_static_path( struct dlog *log, const char *static_name );
static int close_log( struct dlog *log );
static int switch_file( struct dlog *log );


dlog_t dlog_init( const char *pername, const char *dir,
	const char *static_name )
{
	/*
	 * Find the specified period in the list.
	 */
	struct period *per = find_period( pername );
	if( !per ) {
		fprintf(stderr, "Unknown period: %s\n", pername);
		return NULL;
	}

	/*
	 * Check if the directory exists.
	 */
	if( !check_dir( dir ) ) {
		return NULL;
	}

	dlog_t log = calloc( 1, sizeof(struct dlog) );
	if( !log ) {
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}

	log->per = per;
	if( dir ) {
		log->dir = strdup( dir );
	}
	if( static_name ) {
		log->static_path = create_static_path( log, static_name );
	}
	return log;
}

int dlog_put( dlog_t log, const char *line )
{
	/*
	 * Switch the output file if needed.
	 */
	switch_file( log );
	if( !log->fp ) {
		return 0;
	}

	if( fputs( line, log->fp ) == EOF ) {
		return 0;
	}

	return 1;
}

void dlog_free( dlog_t log )
{
	close_log( log );
	free( log->dir );
	free( log->static_path );
	free( log );
}


/*
 * Returns a pointer to the period with the given name or NULL.
 */
static struct period *find_period( const char *pername )
{
	int i;
	int n = (int)( sizeof(periods) / sizeof(periods[0]) );

	for( i = 0; i < n; i++ ) {
		if( strcmp( periods[i].name, pername ) == 0 ) {
			return &periods[i];
		}
	}
	return NULL;
}

/*
 * Returns 1 if the given path corresponds to a directory.
 */
static int check_dir( const char *path )
{
	struct stat info;

	if( stat( path, &info ) < 0 ) {
		fprintf( stderr, "Can't access directory '%s'.\n", path );
		return 0;
	}

	if( !S_ISDIR( info.st_mode ) ) {
		fprintf( stderr, "'%s' is not a directory.\n", path );
		return 0;
	}

	return 1;
}

/*
 * Allocates, formats and returns a full path for the current file.
 */
char *create_static_path( struct dlog *log, const char *static_name )
{
	char *static_path = static_path = malloc( MAXPATH );
	if( !static_path ) {
		fprintf(stderr, "Memory allocation failed.\n");
		return NULL;
	}

	int len = snprintf(static_path, MAXPATH, "%s/%s", log->dir, static_name);
	if( len < 0 || len >= MAXPATH ) {
		fprintf(stderr, "Static path is too long.\n");
		free( static_path );
		return NULL;
	}

	return static_path;
}


static int switch_file( struct dlog *log )
{
	/*
	 * Determine the name the current file should have.
	 */
	char new_path[MAXPATH];
	if( !format_path( log, new_path, sizeof new_path ) ) {
		fprintf( stderr, "Could not format new path\n" );
		return 0;
	}

	/*
	 * If the file name hasn't changed, return the current file.
	 */
	if( log->fp && strcmp( new_path, log->current_path ) == 0 ) {
		return 1;
	}

	/*
	 * If the name has changed, close previous file and open the
	 * new one.
	 */
	close_log( log );
	strcpy( log->current_path, new_path );

	const char *path = log->static_path ? log->static_path : log->current_path;
	log->fp = fopen( path, "ab+" );
	if( !log->fp ) {
		fprintf( stderr, "Could not open file '%s' for writing\n",
			path );
		return 0;
	}
	/*
	 * Set line-buffering mode.
	 */
	if( setvbuf( log->fp, NULL, _IOLBF, BUFSIZ ) != 0 ) {
		fprintf( stderr, "Could not change buffer mode\n" );
	}
	return 1;
}

/*
 * Closes current file and renames it if needed.
 */
static int close_log( struct dlog *log )
{
	if( !log->fp ) {
		return 1;
	}

	fclose( log->fp );
	log->fp = NULL;

	if( !log->static_path ) {
		return 1;
	}

	/*
	 * Move the contents from the temporary path to the corresponding
	 * file.
	 */
	if( file_exists( log->current_path ) ) {
		if( !append_file( log->current_path, log->static_path ) ) {
			fprintf( stderr, "Couldn't append %s to %s\n",
				log->static_path, log->current_path );
			return 0;
		}
		remove( log->static_path );
	}
	else {
		if( rename( log->static_path, log->current_path ) != 0 ) {
			fprintf( stderr, "Couldn't rename %s to %s\n",
				log->static_path, log->current_path );
			return 0;
		}
	}

	return 1;
}

/*
 * Puts path for the current file into the given buffer.
 */
static int format_path( struct dlog *log, char *buf, size_t len )
{
	char name[200];
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	if( !strftime(name, sizeof name, log->per->format, tm) ) {
		return 0;
	}

	int r = snprintf(buf, len, "%s/%s.log", log->dir, name);
	if( r < 0 || (size_t) r >= len ) {
		return 0;
	}

	return 1;
}
