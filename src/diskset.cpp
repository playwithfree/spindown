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

#include "disk.h"
#include "diskset.h"
#include "general.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using std::string;
using std::vector;
using std::ifstream;
using std::ios;

#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

DiskSet::DiskSet(unsigned int sgTime) :
  commonSpinDownTime(sgTime)
{
}

DiskSet::~DiskSet()
{
  // remove all stored disks
  for( int i=0 ; i < this->size() ; i++ )
  {
    Disk* disk = this->at(i);

    if (! disk)
      continue;

    delete disk;
  }
}

void DiskSet::setStatsFrom(DiskSet const & set)
{

  for( int i=0 ; i < set.size() ; i++ )
  {
    if (!set[i])
      continue;

    Disk* found = find(*set[i]);

    if (!found)
      continue;

    // if the found element appears multiple times in
    // the other set, we don't know which one to take...
    if (found->countEntries(set) != 1)
      continue;

    // if the found element appears multiple times in
    // our list, we don't know which one to take...
    if (found->countEntries(*this) != 1)
      continue;

    found->setStatsFrom(*set[i]);
  }
  return;
}

Disk* DiskSet::find(Disk const & disk)
{
  Disk* found = 0;

  for( int i=0 ; !found && i < this->size() ; i++ )
  {
    Disk* check = this->at(i);

    if (! check)
      continue;

    if (disk.getName() == check->getName())
      found = check;
  }
  return found;
}

void DiskSet::updateDevNames()
{
  //dir pointer
  DIR *dp;
  //structure containing file data
  struct dirent *ep;
  
  if ( (dp = opendir (DEVID_PATH)) != NULL)
  {
    while (ep = readdir (dp))
    {
      for( int i=0 ; i < this->size() ; i++ )
        this->at(i)->update( CMD_BYID, ep->d_name );
    }
    (void) closedir (dp);
  }
}

void DiskSet::updateDiskstats()
{
  ifstream fin( STATS_PATH, ios::in );
  char str[CHAR_BUF];

  if( fin )
  {
    while( fin.getline(str,CHAR_BUF) )
    {
      for( int i=0 ; i < this->size() ; i++ )
        this->at(i)->update( CMD_DISKSTATS, str );
    }
    fin.close();
  }
}

void DiskSet::doSpinDown()
{
  for( int i=0 ; i < this->size() ; i++ )
  {
    Disk* disk = this->at(i);

    if (! disk)
      continue;

    // if more than one entry is found we cannot determine
    // which entry to spindown. So we ignore this case
    if (disk->countEntries(*this) != 1)
      continue;

    // the actual decision to spindown is made by the
    // individual objects
    disk->doSpinDown(commonSpinDownTime);
  }
}

void DiskSet::showStats(ostream& out, bool all)  const
{
  for( int i=0 ; i < this->size() ; i++ )
  {
    Disk* disk = this->at(i);

    if (!all && disk->countEntries(*this) != 1)
      continue;

    disk->showStats(out,this->commonSpinDownTime);
  }
}

void DiskSet::setCommonSpinDownTime(unsigned int sgTime)
{
  commonSpinDownTime = sgTime;
}
