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
#include <string>

using std::string;

#include "general.h"
#include "disk.h"

#include "diskset.h"

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
    if (this->countEntries(*found) != 1)
      continue;

    // if the found element appears multiple times in
    // our list, we don't know which one to take...
    if (this->countEntries(*found) != 1)
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

int DiskSet::countEntries(Disk const & search)  const
{
  int count = 0;

  for( int i=0 ; i < this->size() ; i++ )
  {
    if( this->at(i)->getName() == search.getName() )
      count++;
  }

  return count;
}

void DiskSet::setCommonSpinDownTime(unsigned int sgTime)
{
  commonSpinDownTime = sgTime;
}

unsigned int DiskSet::getCommonSpindownTime()
{
    return commonSpinDownTime;
}
