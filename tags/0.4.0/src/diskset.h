/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009 Dimitri Michaux <dimitri.michaux@gmail.com>
 *
 * diskset.h:
 * Class defination of DiskSet.
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

#ifndef DISKSET_H
#define DISKSET_H

#include <vector>
using std::vector;

class Disk;

class DiskSet : public vector<Disk*>
{
  public:  
    
    /**
     * Constructor: Create a new DiskSet
     *
     * @param unsigned int The default idle time
     */
    DiskSet(unsigned int sgTime = 7200);

    /**
     * Cleanup of the DiskSet object
     */
    ~DiskSet();

    /**
     * Sets the common spindown time
     *
     * @param unsigned int sgTime The new spindown time
     */
    void setCommonSpinDownTime(unsigned int sgTime);

    unsigned int getCommonSpindownTime();

    /**
     * Update the internal statistics of all disks.
     * The Disks of the passed DiskSet are used to update the values of
     * the Disks in this object.
     *
     * @param DiskSet& set The DiskSet to get the statistics from
     */
    void setStatsFrom(DiskSet const & set);

    /**
     * Find the passed disk in this DiskSet
     * Uses the disk names to find the passed Disk 
     *
     * @param Disk& disk The disk to search
     * @return Disk* disk The found disk, 0 if no disk was found
     */
    Disk* find(Disk const& disk);

    /**
     * Count occurences of this object in the passed DiskSet
     * This method uses the name of this object to check how many objects
     * in the passed DiskSet have the same name
     *
     * @param        DiskSet* search  The DiskSet to search
     * @return       int     Number of duplicates found
     */
    int countEntries(Disk const & search) const;

  private:

    /**
     * Spindown time to use for the disks if they don't have a
     * configured spindown time
     */
    unsigned int commonSpinDownTime;

};

#endif
