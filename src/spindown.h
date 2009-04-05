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

#ifndef SPINDOWN_CLASS_H
#define SPINDOWN_CLASS_H

#include <string>
using std::string;

class Disk;
class DiskSet;

class Spindown
{
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
         * Writes the current status to the status file.
         */
        string getStatusString(bool all=false) const;

        /**
         * Updates the disk stats.
         */
        static void updateDiskStats(DiskSet*);

        /**
         * Overload function, calls updateDiskStats with the DiskSet of the
         * current object.
         */
        void updateDiskStats();

        /**
         * Update the device names so they correspond with the device id's.
         */
        static void updateDevNames(DiskSet*);

        /**
         * Overload function, calls updateDevNames with the DiskSet of the
         * current object.
         */
        void updateDevNames();

        /**
         * Decides which disks should be spundown.
         */
        void spinDownIdleDisks();

        /**
         * Returns a pointer to the internal disk set.
         */
        DiskSet* getDisks();

        /**
         * Replaces the internal disk set by a new one.
         *
         * @param DiskSet* newDiskSet   the new disk set
         * @param bool     update       set to true if you want to copy the stats
         *                                  from the old set to the new.
         */
        void setDisks(DiskSet*, bool=false);

    private:
        /**
         * Path from where the script was started
         */
        string runPath;

        /**
         * The disks to administer
         */
        DiskSet* disks;
};

#endif
