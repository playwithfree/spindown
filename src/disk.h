/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008  Dimitri Michaux <dimitri.michaux@gmail.com>
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

#include <string>
using std::string;

#include <ctime>

//commands:
#define CMD_DISKSTATS 0
#define CMD_BYID      1

class Disk
{
    public:
        /**
         * The time a disk needs to be idle before it's spundown.
         * Time is in seconds
         */
        static unsigned int spinDownTime;
        
	/**
	 * Constructor: Create a new disc to monitor.
	 *
	 * @param	string	id	The id of the disc to spindown (id's in /dev/disk/by-id).
	 * @param	bool	sd	Spin this disc down or not.
	 */
        Disk( string id, bool sd, string sgPars = "--stop" );
	
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
        string getName();
	
        bool isWatched();
        
        bool isActive();
        
        unsigned int idleTime();
        
	/**
	 * Set if the disc has to be spundown
	 * 
	 * @param	bool	sd	true or false
	 */
	//void enableSpinDown( bool sd );
        
    private:
        string devId;
        string devName;
        string sgParameters;
        unsigned long int totalBlocks;
        time_t lastActive;
        bool active;
	bool spinDown;
        
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
         */
        void doSpinDown();
};

#endif
