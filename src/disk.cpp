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

unsigned int Disk::spinDownTime = 7200;

vector<Disk*> Disk::disks;

Disk::Disk( string id, string name, bool sd, string sgPars )
{
  if( id == "" && name == "" )
  {
    std::cerr << "Error: there is a disk with no name or id in your configurationfile!" << std::endl;
    exit( 1 );
  }
  
  if( id != "" && name != "" )
  {
    std::cerr << "Error: there is a disk with both name and id in your configurationfile!" << std::endl
              << "(Hint: id: " << id << " name: " << name << ")" << std::endl;
    exit( 1 );
  }
  
  devId = id;
  devName = name;
  
  totalBlocks = 0;
  
  sgParameters = sgPars;
  
  lastActive = time(NULL);
  
  active = true;
  spinDown = sd;
  
  //put the disk in the array.
  disks.push_back( this );
}

Disk::~Disk()
{
  for( vector<Disk*>::iterator i = disks.begin() ; i != disks.end() ; i++ )
  {
    if( *i == this )
    {
      disks.erase( i );
      break;
    }
  }
}

void Disk::update( unsigned char command, string value )
{
  if( command == CMD_DISKSTATS )
    updateStats( value );
  else if( command == CMD_BYID )
    findDevName( value );
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
  sscanf(input.data(), "%*u %*u %s %*u %*u %u %*u %*u %*u %u", devNameInp.data(), &newRead, &newWritten );

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
    
    //spindown the disk if it is idle for long enough
    //and when it should be spundown and when it is active
    if( idleTime()>=spinDownTime && active && spinDown && hasDuplicates() )
      doSpinDown();
    
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

void Disk::doSpinDown()
{
  string command;
  //build command + execute
  command = "sg_start " + sgParameters + " /dev/" + devName;
  system( command.data() );

  //set disk as inactive
  active = false;
}

bool Disk::hasDuplicates()
{
  for( int i=0 ; i < disks.size() ; i++ )
  {
    if( disks[i]->getName() == devName && disks[i] != this  )
      return true;
  }
  
  return false;
}

string Disk::getName()
{
  return devName;
}

bool Disk::isWatched()
{
  return ((devName!="")&&spinDown) ? true : false;
}

bool Disk::isActive()
{
  return active;
}

unsigned int Disk::idleTime()
{
  return (unsigned int)difftime( time(NULL), lastActive );
}

bool Disk::isPresent()
{
  if( !hasDuplicates() && devName != "" )
    return true;
  else
    return false;
}
