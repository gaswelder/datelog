# datelog
Small logging utility

Suppose we, as many do, have log messages pouring from a long-running
process (say, beertapd) into a log file (say, "glass.log"). Many use
logrotate to replace the "full glass" with an empty one, but that
requires pausing the process.

datelog instead receives the output and "pours" it itself to the
glasses, replacing them as needed without the process knowing about it.
Every "glass" is named with its date so an administrator could come in
and analyze what the process output was at some specific day or month;
or a maintenance lady could come in and move away old cups.

The simplest form is:

	process | datelog

Then the output will be stored in files named 'Y-<monthnum>.log'. If we
need to change files, say, every hour:

	process | datelog -p hour

If we want the glasses put in a specific directory rather than "right
here", then:

	process | datelog -d /var/log/beer/

datelog can pass the messages along, so we can have something like:

	server | datelog -o | grep 'error: ' | syslog -t beer

If the administrator needs to check the current cup often but doesn't
want to look at the calendar to find out its name, then datelog can
mark the current "glass" with a special constant name:

	server | datelog -d /var/log/beer/ -c now.log

Then, in the example above, the current file will always be at
/var/log/beer/now.log. The file will be renamed when it will be time
to get the new one.
