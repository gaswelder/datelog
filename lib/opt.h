#ifndef __FLAG_H
#define __FLAG_H

/*
 * Option value types.
 */
enum {
	OPT_STR = 's',
	OPT_INT = 'i',
	OPT_UINT = 'u',
	OPT_BOOL = 'b',
	OPT_SIZE = 'z'
};

/*
 * Declares an option.
 */
void opt(int type, const char *name, const char *desc, void *value_pointer);

/*
 * Parses the given arguments list, returns array of arguments after
 * options. Returns NULL on error.
 */
char **opt_parse( int argc, char **argv );


void opt_summary(const char *s);
void opt_usage(void);


#endif
