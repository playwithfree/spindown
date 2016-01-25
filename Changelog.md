## Spindown v0.4.0 ##
### 05-04-2009 ###
  * Added a configuration option that allows you to run the spindown command every cycle, even when the disk is already inactive.
  * Added functions to the init file that would take over the lsb functions when they are not available.
  * Code cleanup.

### 04-04-2009 ###
  * Made a class to keep all the extra functions that used to be in main.
  * Removed includes that weren't used any longer.

### 03-04-2009 ###
  * Fixed a security issue that allowed attackers to execute malicious commands as root when the configuration file wasn't properly secured.

### 02-04-2009 ###
  * Fixed a bug that let spindown display the stats of disks that were not present in the system when they were defined in the configuration file with there device name.
  * Fixed another bug that could let spindown hang when SIGPIPE is send but the fifo is never opend.

### 24-03-2009 ###
  * Previous changelog was wrong: "The status file is now only updated when the daemon receives the SIGPOLL." Should have been "... daemon receives the SIGPIPE signal.
  * Fixed [Issue 4](https://code.google.com/p/spindown/issues/detail?id=4): Spindown status not showing 80% of time.
> > Happend when spindownd wasn't ready with writing to the status file when cat was already reading from it. Fixed by reverting back to the fifo.
  * Added a directory to /var/run to keep the 2 files in spindown uses.

---

## Spindown v0.3.0 ##
### 13-02-2009 ###
  * The status file is now only updated when the daemon receives the SIGPOLL.

### 02-02-2009 ###
  * Removed unnecessary threads, spindown now runs in exactly one thread, no more, no less.
  * Moved some code that didn't belong in the spindown object to main.cpp (signal handeling, commandline parameter parsing, daemon code and writting to files).
  * Init script now follows LSB.

## Spindown v0.2.2 ##
### 07-09-2008 ###
  * When the spindown command fails, this will be logged.
  * Synchronize writes before spindown.

### 06-09-2008 ###
  * The command configuration options now expects the whole command to execute and not just only the parameters. It still defaults to "sg\_start --stop".

### 25-06-2008 ###
  * Added Gentoo init scripts (by Robin Neatherway)

### 19-06-2008 ###
  * Remove the fifo when shutting down the program.


---


## Spindown v0.2.1 ##

### 19-05-2008 ###
  * Logging of messages can now be turned off or on in the configuration file.
  * Logging happens when disk is spundown, becomes back active and when the program is started or stopped.
  * Updated config file.

### 18-05-2008 ###
  * Message are logged when starting and stopping spindown and when spinning down a disk.

### 14-05-2008 ###
  * Wrote basic definitions for the Log class. This class will make it possible for spindown to log messages using syslog.

### 09-05-2008 ###
  * Update example configuration file.
  * Now DiskSet has its own header file.
  * Everytime you ask the status using the init script the status gets updated.


---


## Spindown v0.2 ##

### 26-04-2008 ###
  * It is now possible to set the spindown time for every individual disk (by Thomas Brunko)
  * Now you can reload the settings by sending SIGHUP to spindownd (by Thomas Brunko)


---


## Spindown v0.1.3 ##

### 03-04-2008 ###
  * Check for sg\_start at startup.
  * Output from init script now makes more sence when there is an error.
  * Code cleanups + added documentation

### 02-04-2008 ###
  * Changed the way the array with disks is stored, this makes it easier for me.
  * Disks check automatically for duplicates
  * Now you can also add a disk by it's device name (only use this for non swappable disks).
  * Various code cleanups.

### 01-04-2008 ###
  * Now it is impossible to have duplicate disks. When a duplicate is found both of them
> > are ignored.
  * Fixed a fault in the init script

### 29-03-2008 ###
  * configure is not needed anymore, the spindown init script can now easly be used in debug mode. Just call it with DEBUG=1 ./spindown [OPTION](OPTION.md)
  * The way the daemon is launched and stopped has been changed.


---


## Spindown v0.1.2 ##

### 28-03-2008 ###
  * Configure didn't set the path to the conf file.
  * spindown init script now checks if the lockfile is present.
  * fixed very stupid bug, used == instead of =
  * spindown now also support make dist

### 27-03-2008 ###
  * CONFPATH in spindown wasn't set with configure
  * changed pkill with kill because I kill with the pid
  * configure emptied the general.h
  * Fixed a bug that made spindownd segfault when only -c or -f was given but no value.

### 25-03-2008 ###
  * The path to fifo and path to conf file can be set from the commandline.
  * Added a function (relToAbs) to change relative paths ./file to absolute ones /path/to/file
  * Change spindown init script so it works with the new configuration style
  * configure doesn't generate the general.h file anymore, nothing about config there anylonger
  * added help text to spindownd
  * added version information to general.h
  * spindownd now can print it's version
  * Plus some code cleanups


---


## Spindown v0.1.1 ##

### 17-03-2008 ###
  * configure didn't configure the pid path
  * Added GPLv3 in COPYING

### 16-03-2008 ###
  * Commands are now unsigned char, not string
  * Configure generates init script for debuging
  * Added pid file

### 15-03-2008 ###
  * Now using ifstream for reading from diskstats.
  * Added compile and install information to readme.
  * Optimized the spaces function in spindown.

### 14-03-2008 ###
  * Deamonized the program.

### 11-03-2008 ###
  * Added seperate thread that writes to a fifo to display the status

### 29-02-2008 ###
  * Added GPLv3
  * Added TODO file

### 27-02-2008 ###
  * Added systemcall to sg\_start

### 26-02-2008 ###
  * Created spindown class
  * Included iniParser: stand-alone ini Parser library in ANSI C

### 25-02-2008 ###
  * Creation of changelog