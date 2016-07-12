c=c99 -Wall -Wextra -Werror -pedantic -pedantic-errors
pc=$c -D _XOPEN_SOURCE=700

datelog: main.o opt.o dlog.o fileutil.o
	$c *.o -o datelog

main.o: main.c
	$c -c main.c -o main.o

dlog.o: dlog.c dlog.h
	${pc} -c dlog.c -o dlog.o

opt.o: lib/opt.c lib/opt.h
	cd lib && ${pc} -c opt.c -o ../opt.o

fileutil.o: lib/fileutil.o lib/fileutil.h
	cd lib && ${pc} -c fileutil.c -o ../fileutil.o
