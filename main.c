#include <stdio.h>
#include "lib/opt.h"
#include "dlog.h"

/*
 * datelog [-p <period>] [-d <dir>] [-c <current file name>] [-o]
 */
int main(int argc, char *argv[])
{
	const char *pername = "month";
	const char *dir = ".";
	const char *static_name = NULL;
	int output = 0;

	/*
	 * Parse the arguments.
	 */
	opt_summary( "datelog [-o] [-p period] [-d dir] [-c current logname]" );
	opt(OPT_STR, "p",
		"Log period ('month'/'day'/'hour'/'minute'/'second')", &pername);
	opt(OPT_STR, "d", "Directory for log files", &dir);
	opt(OPT_STR, "c", "File name for current log file", &static_name);
	opt(OPT_BOOL, "o", "Output received lines to stdout", &output);

	char **args = opt_parse(argc, argv);
	if( !args ) {
		opt_usage();
		return 1;
	}
	if( *args ) {
		fprintf(stderr, "no arguments are expected\n");
		return 1;
	}

	dlog_t log = dlog_init( pername, dir, static_name );
	if( !log ) {
		return 1;
	}

	char buf[BUFSIZ];
	while( fgets(buf, sizeof buf, stdin) )
	{
		if( output ) {
			fprintf( stdout, "%s", buf );
		}
		if( !dlog_put( log, buf ) ) {
			fprintf( stderr, "Write to a log failed.\n" );
			break;
		}
	}

	dlog_free( log );
	return 0;
}
