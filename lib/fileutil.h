#ifndef __fileutil_h
#define __fileutil_h

#include <stddef.h>

int writefile( const char *path, const char *data, size_t len );
char *readfile( const char *path );
int file_exists( const char *path );
int append_file( const char *path_to, const char *path_from );

#endif
