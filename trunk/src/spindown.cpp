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

#include "ininiparser3.0b/iniparser.h"
#include "ininiparser3.0b/dictionary.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;

#include <dirent.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>

Spindown::Spindown( int argc, char* argv[] )
{
    disks = 0;
    cycleTime = 60;
    
    //notify about being started
    Log::get()->message( LOG_INFO, "spindown started" );
}

Spindown::~Spindown()
{
    Log::get()->message(LOG_INFO, "spindown stopped");
}

void Spindown::wait()
{
    sleep( cycleTime );
}

int Spindown::cycle()
{
    disks->updateDevNames();
    disks->updateDiskstats();
    spinDownDisks();
}

void Spindown::spinDownDisks()
{
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

        if (all || disks->countEntries(*disk) == 1)
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

void Spindown::readConfig(string const &path)
{
    dictionary* ini;
    string section;
    string input;
    DiskSet* newDiskSet;
    int commonSpinDownTime = 7200;

    //try to open the configuration file
    if( (ini=iniparser_load(path.data()))==NULL )
        exit(1);

    newDiskSet = new DiskSet;

    //go trough the sections of the file
    for( int i=0 ; i < iniparser_getnsec(ini) ; i++ )
    {
        //read the name of the section
        section = iniparser_getsecname(ini, i);

        //general section?
        if( section=="general" )
        {
            commonSpinDownTime = iniparser_getint(ini, string(section+":idle-time").data(), 7200);

            if( commonSpinDownTime <= 0 )
                commonSpinDownTime = 7200;

            cycleTime = iniparser_getint(ini, string(section+":cycle-time").data(), 60);

            logMessages = iniparser_getboolean(ini, string(section+":syslog").data(), 0);
        }
        //disk?
        else if( section.substr(0,4) == "disk" )
        {
            // the parsing of the configuration is up to the Disk class
            Disk* newDisk = Disk::create(*ini, section);

            if (!newDisk)
                continue;

            newDiskSet->push_back(newDisk);
        }
    }

    //free the memory of the directory
    iniparser_freedict(ini);

    //initialize logging
    if( logMessages )
        Log::get()->open( (char*)"spindown", LOG_NDELAY, LOG_DAEMON );
    else
        Log::get()->close();

    // initialise both the DiskSet as the Disks
    newDiskSet->setCommonSpinDownTime(commonSpinDownTime);

    // if a previous configuration exists, copy the internal status
    // to the new configuration and delete the old one
    if (disks)
    {
        newDiskSet->updateDevNames();
        disks->updateDevNames();
        newDiskSet->setStatsFrom(*disks);
        delete disks;
    }

    disks = newDiskSet;
}
