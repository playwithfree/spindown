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
using std::ofstream;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;

#include <dirent.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>

Spindown::Spindown( int argc, char* argv[] )
{
    char buf[CHAR_BUF];
    //this function has to be called before daemonize because it changes the path
    runPath = getcwd( buf, CHAR_BUF );

    //when the user doesn't specify these file we look for them in the current directory
    statusPath = relToAbs( "./spindown.status" );
    confPath = relToAbs( "./spindown.conf" );

    disks = 0;
    cycleTime = 60;
}

Spindown::~Spindown()
{
    remove( statusPath.data() );
    Log::get()->message( LOG_INFO, "spindown stopped" );
}

void Spindown::setConfPath(string path)
{
    confPath = relToAbs(path);
}

void Spindown::setStatusPath(string path)
{
    statusPath = relToAbs(path);
}

int Spindown::run()
{
    //now load configuration file
    readConfig();

    //notify about being started
    Log::get()->message( LOG_INFO, "spindown started" );

    while( true )
    {
	disks->updateDevNames();
	disks->updateDiskstats();
	disks->doSpinDown();

	updateStatus();

	sleep( cycleTime );
    }

    return 0;
}

void Spindown::updateStatus()
{
    ofstream statusFile;
    statusFile.open(statusPath.data());

    disks->updateDiskstats();
    disks->showStats(statusFile);

    statusFile.close();
}

void Spindown::readConfig(string* path)
{
  dictionary* ini;
  string section;
  string input;
  DiskSet* newDiskSet;
  int commonSpinDownTime = 7200;

  if( !path)
    path = &confPath;

  //try to open the configuration file
  if( (ini=iniparser_load(path->data()))==NULL )
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

string Spindown::relToAbs( string path )
{
  //if it starts with "./" it's relative
  if( path.substr(0,2) == "./" )
  {
    path.erase(0,1);
    path.insert( 0, runPath );
  }

  //everything else that doesn't start with "/" it's relative
  else if( path.substr(0,1) != "/" )
    path.insert( 0, runPath+"/" );

  return path;
}
