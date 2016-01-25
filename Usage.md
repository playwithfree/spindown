# Usage #

Now you have your configuration file it is time to start the daemon:

```
# /etc/init.d/spindown start
```

If everything goes as planned there shouldn't be any errors and the deamon should start. The correct links for all the runlevels are already made so the deamon should start when you boot your computer. Spindown doesn't keep a log file because this would prevent a disk from properly spinning down, but it still is possible to get some status information from the deamon:

```
$ /etc/init.d/spindown status
name           watched        active         idle-time      spindown-time
sda            0              1              4              3600
sdb            1              0              22297          3600
```

You can also start the daemon yourself. If you would like to do this, please read the build in help function.

```
$ spindownd --help
Usage: spindownd [OPTION]...
spindownd is a daemon that spinsdown idle disks.

  -f, --fifo-path       The path to the fifo that is used for printing the
                          status. When the fifo doesn't exist it will be
                          created. The default is spindown.fifo in the
                          current directory.
  -c, --config-file     Path to the configuration file. The default is
                          spindown.conf in the current directory.
  -h, --help            Displays this text.
  -v, --version         Prints the version number.

For more information and contact visit <http://projects.dimis-site.be>.
```