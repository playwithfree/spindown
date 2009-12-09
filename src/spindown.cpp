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

#include <dirent.h>

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

#include "disk.h"
#include "log.h"
#include "exceptions.h"

#include "spindown.h"


struct DiskSort
{
	bool operator()( Disk& a, Disk& b )
	{
		return a.getDevice() < b.getDevice();
	}
};


Spindown::Spindown(string stats)
{
    diskstats = stats;
}


void Spindown::pokeDisks()
{
	 list<Disk>::iterator disk;

	 // Find the disk and update the stats
	 for(disk = disks.begin() ; disk != disks.end() ; ++disk)
		 disk->poke();
}


void Spindown::updateDisks()
{
	ifstream file(diskstats.data(), ios::in);

	istringstream stream;

	string 	  	 devName, line;

	unsigned int read, write;

	int iDontCare;

	Disk disk;


	// See if the file is open
	if(!file)
	{
		string msg("Can't open file: ");
		msg += diskstats;
		Log::get()->message(LOG_INFO, msg);

		return;
	}


	// Update every disk
	getline(file, line);

	while(!file.eof())
	{
		stream.str(line);

		stream >> iDontCare >> iDontCare >> devName
			>> iDontCare >> iDontCare >> read
			>> iDontCare >> iDontCare >> write;

		getline(file, line);

		if(devName.length() != 3
			|| (devName.substr(0,2) != "sd"
				&& devName.substr(0,2) != "hd"))
		{
			continue;
		}

		try
		{
			getDiskByName(devName).setBlocksTransferred(read + write);
		}

		catch(SpindownException e)
		{
			Disk newDisk = defaultDisk;

			newDisk.setDevice("/dev/" + devName);
			newDisk.setBlocksTransferred(read + write);

			disks.push_back(newDisk);

			sort();
		}
	}
}


void Spindown::sort()
{
	DiskSort s;

	disks.sort(s);
}


void Spindown::spindownIdleDisks()
{
	 list<Disk>::iterator disk;

	 // Find the disk and update the stats
	 for(disk = disks.begin() ; disk != disks.end() ; ++disk)
	 {
		 if( (disk->getConnected() && disk->getIdle() && disk->getDoSpindown())
			 && (disk->getActive() || disk->getRepeat()) )
		 {
			 try
			 {
				 if(disk->getActive())
				 {
					 Log::get()->message(LOG_INFO
							 , "Spinning down: " + disk->getDevice());
				 }

				 disk->spindown();
			 }

			 catch(SpindownException e)
			 {
				 Log::get()->message(LOG_INFO, e.message);
			 }
		 }
	 }
}


void Spindown::printSet()
{
	list<Disk>::iterator disk;

	cout << "devName connected active watch idletime blocks cmd" << endl;

	cout << "default" << " "
		<< (defaultDisk.getConnected() ? "1" : "0") << " "
		<< (defaultDisk.getActive() ? "1" : "0") << " "
		<< defaultDisk.getDoSpindown() << " "
		<< defaultDisk.getIdleTime() << " "
		<< defaultDisk.getBlocksTransferred() << " "
		<< defaultDisk.getCommand() << endl;

	// Find the disk and update the stats
	for(disk = disks.begin() ; disk != disks.end() ; ++disk)
	{
		cout << disk->getDevice() << " "
			<< (disk->getConnected() ? "1" : "0") << " "
			<< (disk->getActive() ? "1" : "0") << " "
			<< disk->getDoSpindown() << " "
			<< disk->getIdleTime() << " "
			<< disk->getBlocksTransferred() << " "
			<< disk->getCommand() << endl;
	}
}

list<Disk>& Spindown::getDisks()
{
	return disks;
}

Disk& Spindown::getDefaultDisk()
{
	return defaultDisk;
}

Disk& Spindown::getDiskByDevice(string name)
{
	 list<Disk>::iterator disk;

	 // Find the disk and update the stats
	 for(disk = disks.begin() ; disk != disks.end() ; ++disk)
	 {
		 if(name == disk->getDevice())
			 return (*disk);
	 }

	 throw SpindownException("no disk");
}

Disk& Spindown::getDiskByName(string name)
{
	 list<Disk>::iterator disk;

	 // Find the disk and update the stats
	 for(disk = disks.begin() ; disk != disks.end() ; ++disk)
	 {
		 if(name == disk->getDevName() && disk->getConnected())
			 return (*disk);
	 }

	 throw SpindownException("no disk");
}
