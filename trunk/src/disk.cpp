/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009  Dimitri Michaux <dimitri.michaux@gmail.com>
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

#include <time.h>
#include <dirent.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

using namespace std;

#include "log.h"
#include "exceptions.h"

#include "disk.h"

Disk::Disk(string dev, bool sd, string cmd, int sdTime, bool rp)
{
    repeat     	 = rp;
    device		 = dev;
    command      = cmd;
    doSpindown	 = sd;
    spindownTime = sdTime;

    active     	  	  = true;
    lastActive  	  = time(NULL);
    blocksTransferred = 0;

    poke();
}


void Disk::setDevice(string dev)
{
    device = dev;
}


string Disk::getDevice() const
{
	return device;
}


void Disk::poke()
{
	devName	  = searchDevName();
	connected = searchConnected();
}


string Disk::searchDevName() const
{
	string dir;

	dir = device.substr(0, device.find_last_of('/') + 1);

	if(dir == "/dev/")
		return device.substr(device.find_last_of('/') + 1);

	if(dir == "/dev/disk/by-id/")
	{
		char   buf[256], link[device.length()];
		size_t len;

		strcpy(link,device.data());

		if((len = readlink(link, buf, 256)) != -1)
		{
			buf[len] = '\0';

			string buffer(buf);

			return buffer.substr(buffer.find_last_of('/') + 1);
		}
	}

	return "";
}


bool Disk::searchConnected() const
{
    struct stat sbuf;

    if(stat(device.data(), &sbuf) == 0)
    	return true;

    else
    	return false;
}


string Disk::getDevName() const
{
	return devName;
}


bool Disk::getConnected() const
{
	return connected;
}


bool Disk::getActive() const
{
    return active;
}


bool Disk::getIdle() const
{
    if(getIdleTime() >= spindownTime)
        return true;
    else
        return false;
}


void Disk::setBlocksTransferred(unsigned int blocks)
{
    if( blocksTransferred != blocks )
    {
        //if the disk was not active, but now is, log a message
        if( !active )
            Log::get()->message( LOG_INFO, getDevName() + " is now active." );

        lastActive = time(NULL);
        active     = true;
    }

    blocksTransferred = blocks;
}


unsigned int Disk::getBlocksTransferred()
{
	return blocksTransferred;
}


void Disk::spindown()
{
    if(connected && doSpindown)
    {
        int ret;
        string cmd = command + " " + device;

        if((ret=system(cmd.data())) == 0)
        {
            active = false;
        }

        else
        {
            ostringstream oss;
            oss << "failed: \"" << cmd << "\" returned " << ret;
            throw SpindownException( oss.str() );
        }
    }
}


void Disk::setSpindownTime(int sdt)
{
	spindownTime = sdt;
}


int Disk::getSpindownTime() const
{
	return spindownTime;
}


int Disk::getIdleTime() const
{
	return difftime(time(NULL), lastActive);
}


bool Disk::getDoSpindown() const
{
	return doSpindown;
}

void Disk::setDoSpindown(bool sd)
{
	doSpindown = sd;
}

string Disk::getCommand() const
{
	return command;
}

void Disk::setCommand(string cmd)
{
	command = cmd;
}

bool Disk::getRepeat() const
{
	return repeat;
}

void Disk::setRepeat(bool rp)
{
	repeat = rp;
}
