MAKEFILE INFO
=============

The Makefile is dependent upon $LINUX_INCLUDE_DIR, which contains the local
location of the header files required for this code.  On the remote systems,
this directory is "/usr/src/linux/include".

"make all" will create two libraries, and two executable files, with targets
as follows:
- lib_cli (libclidb.so)
	- Command line interpreter library
	- Supports dynamic command binding, scripting, logging output,
	  and parsing command line arguments.
- lib_dev_db (libriodb.so)
	- RapidIO Switch API utilities
	- Detailed device database, including programmed state of devices
	  and relationships of devices
	- Discovery and enumeration support
	- Shared memory support routines for rio_daemon.exe and rio_cli.exe
- daemon (rio_daemon.exe)
	- Manages a detailed database of RapidIO fabric components
	- Publishes an abbreviated database in a shared memory segment
	- The rio_daemon supports discovery and enumeration
	- It is also possible to start the rio_daemon with discovery or
	  enumeration, connect using rio_cli.exe and use the PROBE command
	  to determine what devices are in the system.
	- Optionally supports a console CLI, more fully featured than rio_cli.
- cli (rio_cli.exe)
	- Command interpreter
	- Opens the shared memory data base published by rio_daemon.exe
	  in "read only" mode
	- Can connect to rio_daemon.exe to run CLI commands to manipulate 
	  devices i.e. read/write
	- NOTE: The commands supported by rio_cli.exe are DIFFERENT from 
	  the commands supported by rio_daemon.exe!!!

LIBRARY INFO
============

The makefile copies the libraries into $LOCAL_LIBARY_DIR, by default "./libs"

In order to load these libraries, you will need to do the following 
from the command line:

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:libs"
ldconfig -N

PROGRAM INFO
============

rio_daemon.exe accepts the following command line arguments: 
        rio_demon manages a RapidIO subsystem.  Options are:
        -c, -C: rio_daemon has a console CLI.
        -d, -D: Discover a previously enumerated RapidIO network.
        -e, -E: Enumerate and initialize a RapidIO network.
        -iX, -IX: Polling interval, in seconds.
        -Mp   : Connect to the RapidIO network using port p.
        -p, -P: POSIX Sockets port number for remote connection.
        -x, -X: Initialize and then immediately exit.
        -h, -H, -?: Print this message.

rio_cli.exe may only be run when a shared memory segment has been allocated
(and possibly orphaned) by rio_daemon.exe.

To debug, use gdb as follows:
gdb --args ./rio_demon.exe -e -p12345

