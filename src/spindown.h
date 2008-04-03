/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008  Dimitri Michaux <dimitri.michaux@gmail.com>
 *
 * spindown.h:
 * The class declaration for Spindown. This class manages the parsing
 * of the commandline, reads the configuration file, sends feedback
 * trough a fifi and manages all the disks in the system.
 * 
 * It contains all the disks and then sends all the changes to them.
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
#include <string>

using std::vector;
using std::string;

#include "general.h"
#include "disk.h"
#include "thread.h"

class Spindown : public Thread
{
  public:
    /**
     * Constructor:
     * Does some basic checks and initialisations.
     * 
     * @param   int     argc
     * @param   char*[] argv
     */
    Spindown(int, char* []);
    
    /**
     * This function will run as a thread.
     * It does the general loop.
     */
    int execute();
    
    /**
     * A loop that checks the fifo and then writes to it when it is opend
     */
    void checkFifo();
    
  private:
    /**
     * Time between two cycles in seconds
     */
    unsigned int cycleTime;
    
    /**
     * Path to the fifo
     */
    string fifoPath;
    
    /**
     * Path to the configuration file
     */
    string confPath;
    
    /**
     * Path from where the script was started
     */
    string runPath;
    
    /**
     * Turn the program into a daemon.
     * Forks, changes the dir to "/" and closes stdin stdout stderr
     */
    void daemonize();
    
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
     * Reads the config file from /proc/diskstats and passes it
     * line by line to every disk.
     */
    void readConfig();
    
    /**
     * Parses the command line parameters.
     * 
     * @param   int     argc
     * @param   char*[] argv
     */
    void parseCommandline(int, char* []);
    
    /**
     * Transforms relative paths to absolute ones.
     * This has to be done because we change dir with daemonize.
     */
    string relToAbs( string );
};
