/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009  Dimitri Michaux <dimitri.michaux@gmail.com>
 *
 * disk.h:
 * Class defination of Disk. Keeps all the parameters for a disk, plus
 * detects when a disk has been idle long enough and then spins it down,
 * it also decides which disks should be spundown.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * http://www.gnu.org/licenses/gpl.html
 *
 * Contact: Dimitri Michaux <dimitri.michaux@gmail.com>
 */

#ifndef DISC_CLASS_H
#define DISC_CLASS_H

#include "general.h"
#include "ininiparser3.0b/dictionary.h"
#include "diskset.h"

#include <string>
#include <vector>
#include <fstream>

using std::vector;
using std::string;
using std::ostream;

#include <ctime>

class Disk
{

    friend class DiskSet;

  public:

    /**
     * Factory method: Create a new disc to monitor.
     * It reads all neccessary configurations from the passed dictionary
     * The following directives are evaluated:
     *    string id         The id of the disc to spindown (id's in /dev/disk/by-id).
     *    string name       The device name
     *    bool   spindown   Spin this disc down or not.
     *    string command    The parameters to use with sg_start
     *    int    idle-time  The time to wait before spinning down the disk
     *
     * @param	dictionary& dict The configuration to parse
     * @param	string& section The configuration section of this disk
     * @return Disk* Returns the new Disk object or 0 on error.
     */
    static Disk* create(dictionary& dict, string const & section);

    /**
     * Constructor: Create a new disc to monitor.
     * Either name or id should be empty, but not both
     *
     * @param	string	id	     The id of the disc to spindown (id's in /dev/disk/by-id).
     * @param string   name    The device name
     * @param	bool	   sd      Spin this disc down or not.
     * @param string   sgPars  The parameters to use with sg_start
     * @param int      sgTime  The time to wait before spinning down the disk
     */
    Disk( string id, string name, bool sd, string sgPars = "--stop", int sgTime = 0 );


    /**
     * Cleanup of the Disk object
     */
    ~Disk();

    /**
     * Update a paramaters from the disc.
     * This function expects processes a line from either /dev/disk/by-id or
     * either /proc/diskstats. This function automaticly grabs the values it needs
     * form value.
     *
     * @param       string  command This can either be "by-id" or "diskstats"
     * @param       string  value   For diskstats a line from diskstats, for by-id
     *                                one of the links to a device in by-id
     */
    void update( unsigned char command, string value );

    /**
     * Returns the id of the device as in /dev/disk/by-id
     *
     * @return 	string	The id of the device.
     */
    //string getId();

    /**
     * Returns the name of the device
     *
     * @return	string	The name of the device
     */
    string getName() const;

    /**
     * Returns true if the disk is watched
     */
    bool isWatched() const;

    /**
     * Returns true as long as the disk is active
     */
    bool isActive() const;

    /**
     * Returns the time the disk has been idle in seconds.
     * This value is can be wrong because it depends on the next cycle
     * to be updated.
     */
    unsigned int idleTime() const;

    /**
     * Set if the disc has to be spundown
     *
     * @param	bool	sd	true or false
     */
    //void enableSpinDown( bool sd );

    /**
     * Returns the number of seconds this disk needs to be idle before
     * it is spun down. Return 0 if not configured
     *
     * @return	int	The configured spindown time
     */
    unsigned int spinDownTime() const;

    /**
     * Writes the current disks stats & configuration to the passed
     * ofstream. The stream must be already open. This is used for
     * the status fifo
     */
    void showStats(ostream& out, unsigned int sgTime) const;

  protected:

    /**
     * Update the internal statistics from the passed Disk
     * This function is used to initialise a new object with the
     * statistics of an already existing object. The configuration
     * values are not changed
     *
     * @param       Disk*   disk   A valid Disk object
     */
    void setStatsFrom(Disk const & disk);


  private:
    /**
     * The id of the device as found in /dev/disk/by-id/
     */
    string devId;

    /**
     * The name of the device sda sdb hda
     */
    string devName;

    /**
     * The command to execute
     */
    string command;

    /**
     * Total blocks transferd: read + write
     */
    unsigned long int totalBlocks;

    /**
     * The time of the last activity
     */
    time_t lastActive;

    /**
     * True if there was activity in the last cycle
     */
    bool active;

    /**
     * Tells if the disk has to be spundown
     */
    bool spinDown;

    /**
     * The time a disk needs to be idle before it's spundown.
     * Time is in seconds, 0 is unconfigured
     */
    unsigned int localSpinDownTime;

    /**
     * Finds the dev name for the current dev id and puts it in devName
     * If no dev name is found devName will be empty. This means the device
     * is not present.
     *
     * When this function receives "." as dev it resets the device name.
     * This is because readdir starts with ".".
     *
     * @param       string  dev   the name of a link in /dev/disk/by-id
     */
    void findDevName( string dev );

    /**
     * Update the values
     * This function only needs a line from diskstats and then
     * only uses the information it needs
     *
     * @param       char*   input   a char array containing a line from diskstats
     */
    void updateStats( string input );

    /**
     * Spinsdown the disc with the right command.
     *
     * @param       unsigned int  sgTime   the default spindown time
     */
    void doSpinDown(unsigned int sgTime);

    /**
     * Count occurences of this object in the passed DiskSet
     * This method uses the name of this object to check how many objects
     * in the passed DiskSet have the same name
     *
     * @param        DiskSet* search  The DiskSet to search
     * @return       int     Number of duplicates found
     */
    int countEntries(DiskSet const & search) const;
};

#endif
