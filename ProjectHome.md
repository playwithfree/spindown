# Spindown #

Spindown is a daemon that can spindown idle disks and so saving energy and giving the disks a longer lifetime. It works by checking how many blocks are read and written from the disks. When no blocks are read or written the disk is idle. When a disk stays idle long enough spindown uses sg\_start to spin the disk down. It also works with usb disks and hotswappable disks because it doesn't watch the device name (hda, sdb, ...), but the device id. This means that it doesn't matter if the disk is swapped while the deamon is running.

# Upgrade note! #

Users upgrading from v0.2.1 or older to v0.2.2 or newer need to update there configuration file. The command parameter in this version expects the command to execute to spindown the disk and not just the parameters for sg\_start.

So, if your command parameter used to read "command = --stop" it should now read "command = sg\_start --stop".

This has been done because some users had trouble spinning IDE disks down and had to use hdparm. I'm sorry for the inconvenience.

# Features #

  * Configure every disk separately
  * Logging trough syslog
  * Easy to configure
  * Spindown hot swappable disks

# Dependencies #

Spindown only depends on sg\_start. This tool is part of the package sg3-utils (in Debian). You can find the sources here: [The Linux SCSI Generic (sg) Driver](http://sg.torque.net/sg/index.html)

It also uses the [iniparser3.0b library by Nicolas Devillard](http://ndevilla.free.fr/iniparser/). It's a great library for parsing ini files and made writting this program a lot easier for me. I've included iniparser into the code because no distribution I know of includes it.

# Getting started #

To get started you might be interested in reading the following pages in the wiki:
  1. [Installation](http://code.google.com/p/spindown/wiki/Installation)
  1. [Configuration](http://code.google.com/p/spindown/wiki/Configuration)
  1. [Usage](http://code.google.com/p/spindown/wiki/Usage)

# Getting support #
There is Google Group that is [Spindown Support](http://groups.google.com/group/spindown-support) where you can go to for help. You may also use this group for anything else related to spindown.