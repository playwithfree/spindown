<?php $title = "Spindown";
  include("header.html"); ?>

<h1>Spindown</h1>

<p>Spindown is a daemon I've written to spindown idle disks and so saving energy and giving the disks a longer lifetime. It works by checking how many blocks are read and written from the disks. When no blocks are read or written the disk is idle. When a disk stays idle long enough spindown uses sg_start to spin the disk down. It also works with usb disks and hotswappable disks because it doesn't watch the device name (hda, sdb, ...), but the device id. This means that it doesn't matter if the disk is swapped while the deamon is running.</p>

<h2>Dependecies</h2>

<p>Spindown only depends on sg_start. This tool is part of the package sg3-utils (in Debian).</p>

<p>It also uses the <a href="http://ndevilla.free.fr/iniparser/">iniparser3.0b library by Nicolas Devillard</a>. It's a great library for parsing ini files and made writting this program a lot easier for me. I've included iniparser into the code because no distribution I know of includes it.</p>

<h2>Installation</h2>

<p>First you need to download the source code and compile it. Because there are no nasty dependencies (except for sg3_start), this should be no problem.</p>

<pre>    $ tar xvzf spindown-vx.x.x.tar.gz
    $ cd spindown-vx.x.x.tar.gz
    $ ./configure
    $ make
    # make install</pre>

<h2>Configuration</h2>

<p>After compiling and installing the deamon you still need to configure it. Do this by editing the sample configuration file (spindown.conf) with your favorite editor. Once you are satisfied with the settings save the file and copy it to /etc/spindown.conf</p>
   
<h3>Explenation of the different settings</h3>

<p>The configuration file consists of two different sections: the general section and several disk sections. There should only be one general section, but different disk sections are allowed.</p>

<pre>    [General]
    [Disk 0]
    [Disk 1]
    ...
    [Disk n]</pre>

<h4>The General section</h4>

<p>The General section has only two parameters: cycle-time and idle-time. The cycle-time parameter defines the time in seconds between checks (looking up the device name for the disk id and updating the amount of block read and written), a good value is 60. The idle-time parameter defines how long a disk should be idle before it is spundown. Don't use a to small value for this setting, because spinning the disc up and down to mutch isn't good for it's health. A good value for idle-time is 3600.</p>
    
<pre>    [General]
    cycle-time  = 60    # Number of seconds between two cycles
    idle-time   = 3600  # The time in seconds a disk has to be idle before it is spundown.
                        # Do not use a value that is to small (less than 3600)</pre>
    
<h4>The Disk sections</h4>
  
<p>There are several Disk sections allowed, but they need to have a different id. Each disk can have 3 parameters, but only the id parameter is obligated. The id parameter must contain the id of the diks, this is not the device name (sda, sdb, hda, ...), but mostly a string like this ata-Hitachi_HTS54161-SB2404SJHD07JE. You can find the id of your disk by looking in /deb/disk/by-id</p>

<pre>    $ ls -l /dev/disk/by-id/
    total 0
    lrwxrwxrwx 1 root root  9 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE -> ../../sda
    lrwxrwxrwx 1 root root 10 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE-part1 -> ../../sda1
    lrwxrwxrwx 1 root root 10 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE-part2 -> ../../sda2
    lrwxrwxrwx 1 root root 10 2008-03-20 10:38 ata-Hitachi_HTS54161-SB2404SJHD07JE-part4 -> ../../sda4
    lrwxrwxrwx 1 root root  9 2008-03-20 10:38 scsi-S_SB2404SJHD07JE -> ../../sda
    lrwxrwxrwx 1 root root 10 2008-03-20 10:38 scsi-S_SB2404SJHD07JE-part1 -> ../../sda1
    lrwxrwxrwx 1 root root 10 2008-03-20 10:38 scsi-S_SB2404SJHD07JE-part2 -> ../../sda2
    lrwxrwxrwx 1 root root 10 2008-03-20 10:38 scsi-S_SB2404SJHD07JE-part4 -> ../../sda4</pre>

<p>Lets say you'll need to know the id of sda, in this case this is ata-Hitachi_HTS54161-SB2404SJHD07JE. Be sure not to use the id's of the partitions. When there are more entries for the same disk it shouldn't matter wich one you chose. This method is really great for swappable disks like usbdrives because the device name could change when you have several.</p>

<p>You can also also use the name parameter if you know you are never going to unplug the disk. Then you can just use a device name (sda, hdb) with the name parameter.</p>
    
<p>The spindown parameter is boolean, it can either be 1 or 0. When it is 1 this means that disk will be spundown when it has been idle long enough. When this parameter is not given it defaults to 0.</p>
    
<p>The command parameter tells the spindown script wich paramters to use to spindown the disc with sg3_start. You should try to find these out by your self. For me it mostly is enough to use "--stop" or "--stop --pc=2". You can try these parameters by running sg_start yourself.</p>

<pre>    $ sg_start --stop /dev/sda</pre>

<p>Just replace sda with the name of your disk and make sure that there is no activity on this disk (by unmounting it or something like that). You should hear the disk spiningdown when the command worked. If this parameter is not defined "--stop" is used.</p>

<p>So you disc sections may look something like this:</p>

<pre>    # Add as mutch disks as you want.
    # Only id is needed, for everything else the default values will be used.
    # If spindown is not specified the disk will not be spundown.
    # The default command is --stop.
    [Disk 0]
    id = ata-Hitachi_HTS54161-SB2404SJHD07JE
    spindown = 0

    [Disk 1]
    id = usb-Sony_Storage_Media_1A06101710143-0:0
    spindown = 1
    command = --stop --pc=2</pre>

<p>Disks that are not specified will not be spundown</p>

<h2>Usage</h2>

<p>Now you have your configuration file it is time to start the daemon:</p>
    
<pre>    # /etc/init.d/spindown start</pre>

<p>If everything goes as planned there shouldn't be any errors and the deamon should start. The correct links for all the runlevels are already made so the deamon should start when you boot your computer. Spindown doesn't keep a log file because this would prevent a disk from properly spinning down, but it still is possible to get some status information from the deamon:</p>

<pre>    $ /etc/init.d/spindown status
    name           watched        active         idle-time
    sda            0              1              4
    sdb            1              0              5674</pre>

    
<h2>Download</h2>
    
<p>You can download spindown from here:</p>

<ul>
  <li><a href="files/spindown-0.1.2.tar.gz">spindown-0.1.3.tar.gz</a></li>
  <li><a href="files/spindown-0.1.2.tar.gz">spindown-0.1.2.tar.gz</a></li>
  <li><a href="files/spindown-v0.1.1.tar.gz">spindown-v0.1.1.tar.gz</a></li>
</ul>
    
<h2>Contact</h2>

<p>Please use <a href="forum/">the forum</a> if you have any questions, suggestions, requests, ...</p>

<?php $updated = "20/03/2008";
  include("footer.html"); ?>