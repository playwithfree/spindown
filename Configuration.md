# Configuration #

After compiling and installing the deamon you still need to configure it. Do this by editing the sample configuration file (spindown.conf.example) with your favorite editor. Once you are satisfied with the settings save the file and copy it to /etc/spindown.conf.

## The sections ##

The configuration file consists of two different sections: the general section and several disk sections. There should only be one general section, but different disk sections are allowed.

```
[General]
[Disk 0]
[Disk 1]
...
[Disk n]
```

### The General section ###

The General section has only three parameters: cycle-time, idle-time and syslog.

#### Cycle-time parameter ####

The cycle-time parameter defines the time in seconds between checks (looking up the device name for the disk id and updating the amount of block read and written), a good value is 60.

#### Idle-time parameter ####

The idle-time parameter defines how long a disk should be idle before it is spundown. Don't use a to small value for this setting, because spinning the disc up and down to much isn't good for it's health. A good value for idle-time is 3600. As of version 0.2 of spindown it is also possible to set this time per disk.

##### Syslog parameter #####

The syslog parameter turns logging of informational messages trough syslog on or off. Depending on you Linux installation these messages will probably be added to "/var/log/daemon.log". Logging is not recommended when you are trying to spindown the disk with the root filesystem on, because writting to the log will spin the disk back up. This parameter is supported since version 0.2.1.

##### Example of General section #####

```
[General]
cycle-time = 60     # Number of seconds between cycles.

idle-time = 3600    # The time in seconds a disk has to be idle before it is spundown.
                    # Do not use a value that is to small (less than 3600) unless you
                    # know what you are doing.

syslog = 0          # Turn this on if you want spindown to log messages using syslog.
                    # You will not want to turn this on if you are trying to spindown
                    # the disk with the root filesystem on.
```

### The Disk sections ###

There are several Disk sections allowed, but they need to have a different id or name. Each disk can have several parameters, but only the id or name parameter is obligated (not both).

#### Id parameter ####

The id parameter must contain the id of the diks, this is not the device name (sda, sdb, hda, ...), but mostly a string like this ata-Hitachi\_HTS54161-SB2404SJHD07JE. You can find the id of your disk by looking in /deb/disk/by-id

```
$ ls -l /dev/disk/by-id/
total 0
lrwxrwxrwx 1 root root  9 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE -> ../../sda
lrwxrwxrwx 1 root root 10 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE-part1 -> ../../sda1
lrwxrwxrwx 1 root root 10 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE-part2 -> ../../sda2
lrwxrwxrwx 1 root root 10 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE-part4 -> ../../sda4
lrwxrwxrwx 1 root root  9 2008-03-20 10:38 scsi-S_SB2404SJHD07JE -> ../../sda
lrwxrwxrwx 1 root root 10 2008-03-20 10:38 scsi-S_SB2404SJHD07JE-part1 -> ../../sda1
lrwxrwxrwx 1 root root 10 2008-03-20 10:38 scsi-S_SB2404SJHD07JE-part2 -> ../../sda2
lrwxrwxrwx 1 root root 10 2008-03-20 10:38 scsi-S_SB2404SJHD07JE-part4 -> ../../sda4
```

Lets say you'll need to know the id of sda, in this case this is ata-Hitachi\_HTS54161-SB2404SJHD07JE. Be sure not to use the id's of the partitions. When there are more entries for the same disk it shouldn't matter wich one you chose. This method is really great for swappable disks like usb drives because the device name could change when you have several.

#### Name parameter ####

You can also also use the name parameter if you know you are never going to unplug the disk. Then you can just use a device name (sda, hdb) with the name parameter.

#### Spindown parameter ####

The spindown parameter is boolean, it can either be 1 or 0. When it is 1 this means that disk will be spundown when it has been idle long enough. When this parameter is not given it defaults to 0.

#### Command parameter ####

The command parameter tells the spindown script which command to use to spindown the disk. You should try to find these out by your self. For me it mostly is enough to use "sg\_start --stop" or "sg\_start --stop --pc=2". You can try these parameters by running sg\_start yourself.

```
$ sg_start --stop /dev/sda
```

Some users have can't spindown ATA drives with sg\_start. If this is the case for you, you should try hdparm.

```
$ hdparm -y /dev/hda
```

Just replace sda/hda with the name of your disk and make sure that there is no activity on this disk (by unmounting it or something like that). You should hear the disk spinning down when the command worked. If this parameter is not defined "sg\_start --stop" is used.

If you found the command that works for disk, just fill it in without the path to the device. So, if you used "sg\_start --stop /dev/sda" you should fill in "command = sg\_start --stop".

#### Idle-time parameter ####

As of version 0.2 of spindown it is also possible to set the idle-time per disk. This time is also given in seconds.

#### Repeat parameter ####
Since version 0.4.0. Boolean value, if you set this to 1 it will tell spindown to send the command even when the disk is already spundown. This can be handy when you have a program like smartd running which is known to spin certain disks up when it reads SMART information from the disk. The default value is 0.

#### Example of disk section ####

So your disk sections may look something like this:

```
# Add as much disks as you want.
# Only id is needed, for everything else the default values will be used.
# If spindown is not specified the disk will not be spun down.
# The default command is --stop.
[Disk 0]
id = ata-Hitachi_HTS54161-SB2404SJHD07JE
spindown = 0

[Disk 1]
id = usb-Sony_Storage_Media_1A06101710143-0:0
spindown = 1
command = sg_start --stop --pc=2
idle-time = 900

[Disk 2]
name = sda
spindown = 1
```

Disks that are not specified will not be spun down.