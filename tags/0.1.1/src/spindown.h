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

#include <vector>

using namespace std;

#include "general.h"
#include "disk.h"
#include "thread.h"

class Spindown : public Thread
{
  public:
    Spindown();
    int execute();
    void checkFifo();
    
  private:
    //this is the array we use to keep all the disk we are watching in
    vector<Disk*> disks;
    unsigned int cycleTime;
    
    //read the disks from the configuration file
    void readIni();
    void updateDevNames();
    void updateDiskstats();
    void readConfig();
};