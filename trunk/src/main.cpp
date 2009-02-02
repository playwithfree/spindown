/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009 Dimitri Michaux <dimitri.michaux@gmail.com>
 *
 * main.cpp:
 * Initializes the Spindown object and sends commnandline parameters to
 * it, it also starts the main thread from spindown and starts the loop
 * that checks the fifo.
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

#include <signal.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::ofstream;

#include "spindown.h"

void sigHandler(int);
void daemonize();
void parseCommandline(int, char**);

Spindown* spindown;

int main( int argc, char* argv[] )
{
    spindown = new Spindown( argc, argv );
    
    parseCommandline(argc, argv);

    signal(SIGHUP, sigHandler);
    signal(SIGTERM, sigHandler);
    signal(SIGINT, sigHandler);
    
    spindown->run();
  
    return 0;
}

/**
 * Handles signals
 */
void sigHandler(int signalNumber)
{
    switch( signalNumber )
    {
        case SIGHUP:
            spindown->readConfig();
            break;

	case SIGINT:
        case SIGTERM:
	    delete spindown;
            exit(0);
            break;

        default:
            break;
    }
}

/**
 * Detach from terminal and save the pid file. 
 */
void daemonize()
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
    {
	//save the pid file
	ofstream pidFile;
	pidFile.open("/var/run/spindownd.pid");
	pidFile << pid;
	pidFile.close();
	exit(EXIT_SUCCESS);
    }

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

void parseCommandline(int argc, char* argv[] )
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
		<< "Copyright (C) 2008-2009 Dimitri Michaux <http://projects.dimis-site.be>" << endl
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
		<< "  -d, --daemon	    Daemonize the the process." << endl
		<< "  -h, --help            Displays this text." << endl
		<< "  -V, --version         Prints the version number." << endl << endl
		<< "For more information and contact visit <http://projects.dimis-site.be>." <<endl;
	    exit(EXIT_SUCCESS);
	}

	else
	{
	    //set fifo path
	    if( (arg=="-f"||arg=="--fifo-path") && i+1 < argc )
		spindown->setStatusPath(argv[++i]);

	    //set config file path
	    else if( (arg=="-c" || arg=="--config-file") && i+1 < argc )
		spindown->setConfPath(argv[++i]);

	    //daemonize
	    else if( arg=="-d" || arg=="--daemon" )
		daemonize();
	}
    }
}
