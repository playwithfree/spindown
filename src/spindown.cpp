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

#include "spindown.h"
#include "general.h"
#include "disk.h"

#include "ininiparser3.0b/iniparser.h"
#include "ininiparser3.0b/dictionary.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
using std::ofstream;
using std::cout;
using std::endl;

#include <dirent.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

Spindown::Spindown()
{
  /* Our process ID and Session ID */
  pid_t pid, sid;
        
  /* Fork off the parent process */
  pid = fork();
  if (pid < 0)
    exit(EXIT_FAILURE);

  /* If we got a good PID, then
  we can exit the parent process. */
  if (pid > 0)
  {
    cout << pid << endl;
    exit(EXIT_SUCCESS);
  }

  /* Change the file mode mask */
  umask(0);

  /* Open any logs here */
  readConfig();
  
  /* Create a new SID for the child process */
  sid = setsid();
  if (sid < 0)
    exit(EXIT_FAILURE);
  
  /* Change the current working directory */
  if ((chdir("/")) < 0)
    exit(EXIT_FAILURE);
  
  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

int Spindown::execute()
{
  while( true )
  {
    updateDevNames();
    updateDiskstats();
    sleep( cycleTime );
  }
  
  return 0;
}

void Spindown::updateDevNames()
{
  //dir pointer
  DIR *dp;
  //structure containing file data
  struct dirent *ep;
  
  if ( (dp = opendir (DEVID_PATH)) != NULL)
  {
    while (ep = readdir (dp))
    {
      for( int i=0 ; i < disks.size() ; i++ )
        disks[i]->update( CMD_BYID, ep->d_name );
    }
    (void) closedir (dp);
  }
}

void Spindown::updateDiskstats()
{
  ifstream fin( STATS_PATH, ios::in );
  char str[CHAR_BUF];

  if( fin )
  {
    while( fin.getline(str,CHAR_BUF) )
    {
      for( int i=0 ; i < disks.size() ; i++ )
        disks[i]->update( CMD_DISKSTATS, str );
    }
    fin.close();
  }
}

void Spindown::checkFifo()
{
  ofstream fifoOut;

  mkfifo( FIFO_PATH, 0744 );
  
  while( true )
  {
    fifoOut.open(FIFO_PATH);

    for( int i=0 ; i < disks.size() ; i++ )
    {
      if( disks[i]->getName() != "" )
      {
        fifoOut << disks[i]->getName()
                << " " << disks[i]->isWatched()
                << " " << disks[i]->isActive()
                << " " << disks[i]->idleTime()
                << endl;
      }
    }

    fifoOut.close();
    sleep(1);
  }
}

void Spindown::readConfig()
{
  dictionary* ini;
  string section;
  string input;
  
  //try to open the configuration file
  if( (ini=iniparser_load(CONF_PATH))==NULL )
    exit(1);
  
  //go trough the sections of the file
  for( int i=0 ; i < iniparser_getnsec(ini) ; i++ )
  {
    //read the name of the section
    section = iniparser_getsecname(ini, i);
    
    //general section?
    if( section=="general" )
    {
      Disk::spinDownTime = iniparser_getint(ini, string(section+":idle-time").data(), 7200);
      cycleTime = iniparser_getint(ini, string(section+":cycle-time").data(), 60);
    }
    //disk?
    else if( section.substr(0,section.find_first_of(" ")) == "disk" )
    {
      input = iniparser_getstring(ini, string(section+":id").data(), (char*)"");
      if( input != "" )
        disks.push_back( new Disk(input, iniparser_getboolean(ini, string(section+":spindown").data(), 0),
                         iniparser_getstring(ini, string(section+":command").data(), (char*)"--stop")) );
    }
  }
  
  //free the memory of the directory
  iniparser_freedict(ini);
}
