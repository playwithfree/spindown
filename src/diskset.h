#ifndef DISKSET_H
#define DISKSET_H

#include "disk.h"

#include <fstream>
#include <vector>

using std::ostream;
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
     * Reads all the files in /dev/disk/by-id and passes theses results
     * to the disks.
     */
    void updateDevNames();

    /**
     * Reads the file /proc/diskstats and passes this line by line to
     * every disk.
     */
    void updateDiskstats();

    /**
     * Spinsdown all disks.
     */
    void doSpinDown();

    /**
     * Sets the common spindown time
     *
     * @param unsigned int sgTime The new spindown time
     */
    void setCommonSpinDownTime(unsigned int sgTime);

    /**
     * Writes the current disks stats & configuration to the passed
     * ofstream. The stream must be already open. This is used for
     * the status fifo
     *
     * @param ostream& out The stream to write the output to
     * @param bool     all If true, write all configured disks, even if not present
     */
    void showStats(ostream& Out, bool all = false) const;

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

  private:

    /**
     * Spindown time to use for the disks if they don't have a
     * configured spindown time
     */
    unsigned int commonSpinDownTime;

};

#endif
