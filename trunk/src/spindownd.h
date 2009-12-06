/**
* Spindown is a daemon that can spindown idle discs.
* Copyright (C) 2008-2009  Dimitri Michaux <dimitri.michaux@gmail.com>
*
* Spindownd: this is a class with only static atributes and methods.
* It takes care of all the highlevel stuff: reading config, handling
* signals, turning the program into a daemon and configuring Spindown.
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

#ifndef SPINDOWND_CLASS_H
#define SPINDOWND_CLASS_H

#include <string>
using std::string;

class Spindown;

class Spindownd
{
    public:
        /**
         * Configures everything.
         */
        static void init(int, char*[]);

        /**
         * Updates the device list and spinsdown disks
         * everytime after cycleTime.
         */
        static void run(bool = false);

        /**
         * Reads the configuration file and configures spindown.
         */
        static void readConfig(string const &);

    private:
        static Spindown spindown;

        static string fifoPath
			,confPath
			,pidPath;

        static bool daemon;

        /**
         * Parses the commandline parameters and uses those values to
         * configure the daemon.
         */
        static void parseCommandline(int, char*[]);

        /**
         * Install the signal handlers.
         */
        static void installSignals();

        /**
         * Turns the program into a daemon.
         */
        static void daemonize();

        /**
         * Handles the HUP signal.
         * Re-reads the configuration file.
         */
        static void sigHandlerHup(int);

        /**
         * Handles the termination of the daemon.
         * Deletes all objects and logs the event.
         */
        static void sigHandlerTerm(int);

        /**
         * Handles the PIPE signal.
         * Opens the fifo, checks if it is beeing read from
         * and write the status to it.
         */
        static void sigHandlerPipe(int);

        /**
         * Turns a relative path into an absolute path.
         */
        static string relToAbs(string);

        /**
         * Time between two cycles in seconds
         */
        static unsigned int cycleTime;
};

#endif
