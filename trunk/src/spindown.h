/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009 Dimitri Michaux <dimitri.michaux@gmail.com>
 *
 * spindown.h:
 * The class declaration for Spindown. This class manages the parsing
 * of the commandline, reads the configuration file, sends feedback
 * trough a fifi and manages all the disks in the system.
 * 
 * It contains all the disks and then sends all the changes to them.
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

#include <vector>
#include <string>

using std::vector;
using std::string;

#include "general.h"
#include "disk.h"

class Spindown
{
    friend void readConfig(string const &);

    public:
        /**
        * Constructor:
        * Does some basic checks and initialisations.
        * 
        * @param   int     argc
        * @param   char*[] argv
        */
        Spindown();
        
        ~Spindown();

        /**
        * Preforms one cycle.
        * Updates disk stats and spins the disks down if needed.
        */
        int cycle();
        
        /**
        * Writes the current status to the status file.
        */
        string getStatusString(bool all=false);
        
        /**
        * Updates the disk stats.
        */
        void updateDiskStats();
	
	/**
	 * Update the device names so they correspond with the device id's.
	 */
	void updateDevNames();

        /**
        * Wait cycleTime seconds.
        */
        void wait();

    private:
        /**
        * Time between two cycles in seconds
        */
        unsigned int cycleTime;
        
        /**
        * Path from where the script was started
        */
        string runPath;
        
        /**
        * The disks to administer
        */
        DiskSet* disks;

        void spinDownDisks();
};
