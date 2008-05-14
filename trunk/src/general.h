/*  This file contains some general settings. */

#define DEVID_PATH "/dev/disk/by-id/"
#define STATS_PATH "/proc/diskstats"
#define CHAR_BUF   256
#define VERSION    "0.2.1"

/*  These are commands that are send by Spindown and
    are accepted by Disk. They are unsigned chars     */
#define CMD_DISKSTATS 0
#define CMD_BYID      1
