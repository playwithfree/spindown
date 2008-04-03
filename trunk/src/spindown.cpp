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
using std::ifstream;
using std::ios;
using std::cout;
using std::cerr;
using std::endl;

#include <dirent.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

Spindown::Spindown( int argc, char* argv[] )
{
  char buf[CHAR_BUF];
  //this function has to be called before daemonize because it changes the path
  runPath = getcwd( buf, CHAR_BUF );
  
  if( system("sg_start -V 2> /dev/null") != 0 )
  {
    cerr << "Error: The command sg_start is needed, but not found. Please install it." << endl;
    exit( 1 );
  }
  
  //when the user doesn't specify these file we look for them in the current directory
  fifoPath = relToAbs( "./spindown.fifo" );
  confPath = relToAbs( "./spindown.conf" );
  
  cycleTime = 60;

  //get command line parameters
  parseCommandline(argc, argv);
  
  //now load configuration file
  readConfig();
  
  daemonize();
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
      for( int i=0 ; i < Disk::disks.size() ; i++ )
        Disk::disks[i]->update( CMD_BYID, ep->d_name );
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
      for( int i=0 ; i < Disk::disks.size() ; i++ )
        Disk::disks[i]->update( CMD_DISKSTATS, str );
    }
    fin.close();
  }
}

void Spindown::checkFifo()
{
  ofstream fifoOut;

  mkfifo( fifoPath.data(), 0744 );
  
  while( true )
  {
    fifoOut.open(fifoPath.data());

    for( int i=0 ; i < Disk::disks.size() ; i++ )
    {
      if( Disk::disks[i]->isPresent() )
      {
        fifoOut << Disk::disks[i]->getName()
                << " " << Disk::disks[i]->isWatched()
                << " " << Disk::disks[i]->isActive()
                << " " << Disk::disks[i]->idleTime()
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
  if( (ini=iniparser_load(confPath.data()))==NULL )
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
      
      if( Disk::spinDownTime <= 0 )
        Disk::spinDownTime = 7200;
      
      cycleTime = iniparser_getint(ini, string(section+":cycle-time").data(), 60);
    }
    //disk?
    else if( section.substr(0,4) == "disk" )
    {
      //no need to store the object somewhere, they are automatically stored in Disk::disks
      new Disk( iniparser_getstring (ini, string(section+":id").data(),       (char*)""),
                iniparser_getstring (ini, string(section+":name").data(),     (char*)""),
                iniparser_getboolean(ini, string(section+":spindown").data(), 0),
                iniparser_getstring (ini, string(section+":command").data(),  (char*)"--stop")
              );
    }
  }
  
  //free the memory of the directory
  iniparser_freedict(ini);
}

void Spindown::daemonize()
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
    exit(EXIT_SUCCESS);

  /* Change the file mode mask */
  umask(0);

  /* Open any logs here */
  
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

void Spindown::parseCommandline(int argc, char* argv[] )
{
  string arg;

  for( int i=0 ; i<argc ; i++ )
  {
    arg = argv[i];
    
    //print version information
    //version number is set in general.h
    if( arg=="-V" || arg=="--version" )
    {
      cout << "spindownd "<< VERSION << endl
           << "This is spindownd - a daemon that spinsdown idle disks" << endl
           << "(c) 2008 Dimitri Michaux <http://projects.dimis-site.be>" << endl
           << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << endl
           << "This is free software: you are free to change and redistribute it." << endl
           << "There is NO WARRANTY, to the extent permitted by law." << endl;

      exit(EXIT_SUCCESS);
    }
    
    //show the help file text
    else if( arg=="-h" || arg=="--help" )
    {
      cout << "Usage: spindownd [OPTION]..." << endl
           << "spindownd is a daemon that spinsdown idle disks." << endl << endl
           << "  -f, --fifo-path       The path to the fifo that is used for printing the" << endl
           << "                          status. When the fifo doesn't exist it will be" << endl
           << "                          created. The default is spindown.fifo in the" << endl
           << "                          current directory." << endl
           << "  -c, --config-file     Path to the configuration file. The default is" << endl
           << "                          spindown.conf in the current directory." << endl
           << "  -h, --help            Displays this text." << endl
           << "  -V, --version         Prints the version number." << endl << endl
           << "For more information and contact visit <http://projects.dimis-site.be>." <<endl;
      exit(EXIT_SUCCESS);
    }
    
    //don't read to far
    else if( i+1 < argc )
    {
      //set fifo path
      if( arg=="-f"||arg=="--fifo-path" )
        fifoPath = relToAbs(argv[++i]);
        
      //set config file path
      else if( arg=="-c" || arg=="--config-file" )
        confPath = relToAbs(argv[++i]);
    }
  }
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
