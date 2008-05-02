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

#include "disk.h"
#include "general.h"
#include "ininiparser3.0b/iniparser.h"

#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

Disk::Disk( string id, string name, bool sd, string sgPars, int sdTime )
{
  //if both names are empty something is wrong
  if( id == "" && name == "" )
  {
    std::cerr << "Error: there is a disk with no name or id in your configurationfile!" << std::endl;
    exit( 1 );
  }
  
  //one has to be empty
  if( id != "" && name != "" )
  {
    std::cerr << "Error: there is a disk with both name and id in your configurationfile!" << std::endl
              << "(Hint: id: " << id << " name: " << name << ")" << std::endl;
    exit( 1 );
  }
  
  devId = id;
  devName = name;
  localSpinDownTime = sdTime < 0 ? 0 : sdTime;

  totalBlocks = 0;
  
  sgParameters = sgPars;
  
  lastActive = time(NULL);
  
  active = true;
  spinDown = sd;
}

Disk* Disk::create( dictionary& ini, string const & section)
{

  string id     = iniparser_getstring (&ini, string(section+":id").data(),       (char*)"");
  string name   = iniparser_getstring (&ini, string(section+":name").data(),     (char*)"");
  bool sd       = iniparser_getboolean(&ini, string(section+":spindown").data(), 0);
  string sgPars = iniparser_getstring (&ini, string(section+":command").data(),  (char*)"--stop");
  int sgTime    = iniparser_getint (&ini, string(section+":idle-time").data(), 0);

  Disk* newDisk = new Disk(id, name, sd, sgPars, sgTime);

  return newDisk;
}

Disk::~Disk()
{
}

void Disk::update( unsigned char command, string value )
{
  switch( command )
  {
    case CMD_DISKSTATS:
      updateStats( value );
      break;
    
    case CMD_BYID:
      findDevName( value );
      break;
  }
}

void Disk::updateStats( string input )
{
  if( devName == "" )
  {
    totalBlocks = 0;
    return;
  }

  string devNameInp;  //the name of the device read from the configuration
  unsigned int newRead;
  unsigned int newWritten;

  //Make a string with a size of 32
  devNameInp.resize( 32 );

  //scan the input for the information we need
  sscanf( input.data(), "%*u %*u %s %*u %*u %u %*u %*u %*u %u", devNameInp.data(), &newRead, &newWritten );

  //We first need to remove all the null characters from the string
  devNameInp.resize( devNameInp.find_first_of((char)0) );

  //are we on the line of the right device?
  if( devNameInp == devName )
  {
    /*
      The values from /proc/diskstats can overflow because they are 32-bit integers.
      But because we are only looking for changes we don't have to detect it.
    */
    if( newRead + newWritten != totalBlocks )
    {
      lastActive = time(NULL);
      active = true;
    }

    totalBlocks = newRead + newWritten;
  }
}

void Disk::findDevName( string dev )
{
  //no need to do this when the id is empty, this means this is a nonswapable disk
  if( devId == "" )
    return;
  //we use . to reset the device name
  else if( dev == "." )
    devName = "";
  //do the normal thing
  else if( dev == devId )
  {
    string buffer;
    buffer.resize( CHAR_BUF );
    
    //the path to the device
    dev = DEVID_PATH + dev;
    //read target of the link
    readlink( (char*)dev.data(), (char*)buffer.data(), CHAR_BUF);
    //remove empty characters
    buffer.resize( buffer.find_first_of((char)0) );
    //remove ../../ from the link target
    devName = buffer.substr(buffer.find_last_of("/")+1,buffer.size()-buffer.find_last_of("/") );
  }
}

void Disk::doSpinDown(unsigned int sgTime)
{
  string command;

  // Entries with bad device names cannot be spun down
  if (devName == "")
    return;

  // if no spindown time was configured, use the passed default vaule
  if (spinDownTime())
    sgTime = spinDownTime();

  //spindown the disk if it is idle for long enough
  //and when it should be spundown and when it is active
  if( idleTime() < sgTime || !active || !spinDown )
    return;

  //build command + execute
  command = "sg_start " + sgParameters + " /dev/" + devName;
  system( command.data() );

  //set disk as inactive
  active = false;
}

int Disk::countEntries(DiskSet const & search)  const
{
  int count = 0;

  for( int i=0 ; i < search.size() ; i++ )
  {
    if( search[i]->getName() == devName  )
      count++;
  }

  return count;
}

string Disk::getName() const
{
  return devName;
}

bool Disk::isWatched() const
{
  return ((devName!="")&&spinDown) ? true : false;
}

bool Disk::isActive() const
{
  return active;
}

unsigned int Disk::idleTime() const
{
  return (unsigned int)difftime( time(NULL), lastActive );
}

unsigned int Disk::spinDownTime() const
{
  return localSpinDownTime;
}

void Disk::setStatsFrom(Disk const & disk)
{
  // only copy the internal status, don't touch configuration!
  lastActive        = disk.lastActive;
  active            = disk.active;
  spinDown          = disk.spinDown;
}

void Disk::showStats(ostream& out, unsigned int sgTime) const
{
  if (devName == "")
    return;

  out << this->getName()
      << " " << this->isWatched()
      << " " << this->isActive()
      << " " << this->idleTime()
      << " " << (this->spinDownTime() ? this->spinDownTime() : sgTime )
      << std::endl;
  return;
}
