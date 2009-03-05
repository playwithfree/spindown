/**
* Spindown is a daemon that can spindown idle discs.
* Copyright (C) 2008-2009 Dimitri Michaux <dimitri.michaux@gmail.com>
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

#include "spindown.h"
#include "general.h"
#include "disk.h"
#include "log.h"

#include <string>
#include <sstream>
#include <iostream>

using std::string;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;

#include <unistd.h>
#include <dirent.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>

Spindown::Spindown( int argc, char* argv[] )
{
    disks = 0;
    cycleTime = 60;
}

Spindown::~Spindown(){};

void Spindown::wait()
{
    sleep( cycleTime );
}

void Spindown::updateStats()
{
    disks->updateDiskstats();
}

int Spindown::cycle()
{
    disks->updateDevNames();
    disks->updateDiskstats();
    spinDownDisks();
}

void Spindown::spinDownDisks()
{
    //commit buffer cache to disk 
    sync();
    
    for( int i=0 ; i < disks->size() ; i++ )
    {
        Disk* disk = disks->at(i);

        if (! disk)
            continue;

        // if more than one entry is found we cannot determine
        // which entry to spindown. So we ignore this case
        if (disks->countEntries(*disk) == 1)
        {
            unsigned int sgTime = disk->spinDownTime();
            // if no spindown time was configured, use the passed default vaule
            if(sgTime == 0)
                sgTime = disks->getCommonSpindownTime();

            //Only spindown disks that have been idle long enough, have a correct
            //devicename and are active
            if (disk->getName() != "" && disk->idleTime() >= sgTime && disk->isActive() )
                disk->spindown();
        }
    }
}

string Spindown::getStatusString( bool all )
{
    ostringstream status;

    for( int i=0 ; i < disks->size() ; i++ )
    {
        Disk* disk = disks->at(i);

        if (all || (disks->countEntries(*disk) == 1 && disk->getName()!=""))
        {
            status << (disk->getName()=="" ? "none" : disk->getName()) << " "
                << disk->isWatched() << " "
                << disk->isActive() << " "
                << disk->idleTime() << " "
                << (disk->spinDownTime() ? disk->spinDownTime() : disks->getCommonSpindownTime() )
                << endl;
        }
    }

    return status.str();
}
