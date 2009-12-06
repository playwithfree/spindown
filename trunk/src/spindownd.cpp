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

#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <getopt.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include "log.h"
#include "disk.h"
#include "spindown.h"
#include "exceptions.h"

#include "iniparser3.0b/iniparser.h"
#include "iniparser3.0b/dictionary.h"

#include "spindownd.h"

string Spindownd::fifoPath
	,Spindownd::confPath
	,Spindownd::pidPath;

bool Spindownd::daemon;

Spindown Spindownd::spindown("/proc/diskstats");

unsigned int Spindownd::cycleTime;

void Spindownd::init(int argc, char* argv[])
{
    fifoPath  = relToAbs("./spindown.fifo");
    confPath  = relToAbs("./spindown.conf");
    pidPath   = relToAbs("./spindownd.pid");
    daemon    = false;
    cycleTime = 3;

    // Configure daemon and paths
    parseCommandline(argc, argv);

    // Read the configuration file.
    readConfig(confPath);

    // Create the fifo
    mkfifo( fifoPath.data(), 0744 );

    // Install signalhandlers
    installSignals();

    // Do this last to give the program a chance to print error messages, if any
    if(daemon)
        daemonize();
}

void Spindownd::run(bool print)
{
    Log::get()->message(LOG_INFO, "Daemon started.");

    while( true )
    {
    	spindown.pokeDisks();

		spindown.updateDisks();

		spindown.spindownIdleDisks();

		if(print)
			 spindown.printSet();

        sleep(cycleTime);
    }
}

void Spindownd::installSignals()
{
    signal(SIGPIPE, Spindownd::sigHandlerPipe);
    signal(SIGHUP, Spindownd::sigHandlerHup);
    signal(SIGTERM, Spindownd::sigHandlerTerm);
    signal(SIGINT, Spindownd::sigHandlerTerm);
}

void Spindownd::sigHandlerHup(int)
{
    readConfig(confPath);
}

void Spindownd::sigHandlerPipe(int)
{
	list<Disk>::iterator disk;
	ostringstream status;
    int file;


    // We need to make sure that there is a process that has the fifo
    // open for reading else the whole daemon will lock up. If no program
    // opens the fifo we'll repport the error and continue.
    file = open(fifoPath.data(), O_NONBLOCK | O_WRONLY);


    if(errno==0)
    {
    	for(disk = spindown.getDisks().begin() ; disk != spindown.getDisks().end() ; ++disk)
    	{
			if(disk->getConnected())
			{
				status << disk->getDevName() << " "
					<< disk->getDoSpindown() << " "
					<< disk->getActive() << " "
					<< disk->getIdleTime()	<< " "
					<< disk->getSpindownTime() << endl;
			}
    	}

        write(file, status.str().data(), status.str().size());
    }
    else
        Log::get()->message(LOG_INFO, "Failed to write to the fifo.");

    close(file);
}

void Spindownd::sigHandlerTerm(int)
{
    remove(fifoPath.data());
    remove(pidPath.data());

    Log::get()->message(LOG_INFO, "Daemon stopped.");

    exit(EXIT_SUCCESS);
}

void Spindownd::parseCommandline(int argc, char* argv[] )
{
	int c, i;

    const char* shortOptions = "Vhdf:c:p:";

    static struct option longOptions[] =
	{
		{"version",		no_argument,		0,	'V'},
		{"help",		no_argument,		0,	'h'},
		{"daemon",		required_argument,	0,	'd'},

		{"fifo-path",	required_argument,	0,	'f'},
		{"config-file",	required_argument,	0,	'c'},
		{"pid-file",	required_argument,	0,	'p'},

		{0, 0, 0, 0}
	};


	while((c = getopt_long(argc, argv, shortOptions, longOptions, &i)) != -1)
	{
		switch(c)
		{
			case 'V':
	            cout << "spindownd 0.5.0" << endl
					<< "This is spindownd - a daemon that spinsdown idle disks" << endl
					<< "Copyright (C) 2008-2009 Dimitri Michaux <http://code.google.com/p/spindown/>" << endl
					<< "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>" << endl
					<< "This is free software: you are free to change and redistribute it." << endl
					<< "There is NO WARRANTY, to the extent permitted by law." << endl;

	            exit(EXIT_SUCCESS);

			case 'h':
	            cout << "Usage: spindownd [OPTION]..." << endl
					<< "spindownd is a daemon that spinsdown idle disks." << endl << endl
					<< "  -f, --fifo-path      The path to the fifo that is used to communicate the" << endl
					<< "                          status." << endl
					<< "  -c, --config-file     Path to the configuration file. The default is" << endl
					<< "                          spindown.conf in the current directory." << endl
					<< "  -p, --pid-file        Path to the pid-file. The default is" << endl
					<< "                          spindownd.pid in the current directory." << endl
					<< "  -d, --daemon          Daemonize the the process." << endl
					<< "  -h, --help            Displays this text." << endl
					<< "  -V, --version         Prints the version number." << endl << endl
					<< "For more information and contact visit <http://code.google.com/p/spindown/>." << endl;

	            exit(EXIT_SUCCESS);

			case 'd':
				daemon = true;
				break;

			case 'f':
				fifoPath = relToAbs(optarg);
				break;

			case 'c':
				confPath = relToAbs(optarg);
				break;

			case 'p':
				pidPath = relToAbs(optarg);
				break;

			default:
				exit(EXIT_FAILURE);
		}
	}
}

void Spindownd::readConfig(string const &path)
{
    dictionary* ini;

    string section
		,input
		,device
		,cmd;

    Disk* disk;

    list<Disk>::iterator i;

    struct stat sbuf;

	int 	idleTime;

	bool 	sd, rp;

	char empty[] = "\0"
		,defCmd[] = "sg_start --stop --pc=2";


    // Get information about config file.
    if(stat(confPath.data(), &sbuf) != 0)
    {
        std::cerr << "Error: Can't access the configuration file."<< endl;
        exit (1);
    }

    // Check if the file is owned by other people as the user that is running
    // the daemon.
    if( getgid() != sbuf.st_gid || getuid() != sbuf.st_uid )
    {
        std::cerr << "Error: Configuration file is not owned by the same user and/or "
                << " group that is running the daemon."<< endl;
        exit (1);
    }

    // Check if the file is writeable by other people as the user that is running
    // the daemon.
    if( (sbuf.st_mode & S_IWOTH) == S_IWOTH )
    {
        std::cerr << "Error: Configuration file is writeable by others." << endl;
        exit(1);
    }

    // Try to open the configuration file
    if((ini=iniparser_load(path.data()))==NULL)
    {
        std::cerr << "Can't open the configuration file: " << path << endl;
        exit(1);
    }


    // Get general configuration
	cycleTime = iniparser_getint(ini, string("spindown:cycle-time").data(), 3);

	if( iniparser_getboolean(ini, string(section+"spindown:syslog").data(), 0) )
		Log::get()->open((char*)"spindown", LOG_NDELAY, LOG_DAEMON);
	else
		Log::get()->close();


    // Get default configuration
	idleTime = iniparser_getint(ini, string("default disk:idle-time").data(), 1800);
	sd		 = iniparser_getboolean(ini, string("default disk:spindown").data(), false);
	cmd		 = iniparser_getstring(ini, string("default disk:command").data(), defCmd);
	rp		 = iniparser_getboolean(ini, string("default disk:repeat").data(), false);

	spindown.getDefaultDisk().setSpindownTime(idleTime);
	spindown.getDefaultDisk().setDoSpindown(sd);
	spindown.getDefaultDisk().setCommand(cmd);
	spindown.getDefaultDisk().setRepeat(rp);

	// Apply default configuration
	for(i = spindown.getDisks().begin() ; i != spindown.getDisks().end() ; ++i)
	{
    	i->setSpindownTime(idleTime);
    	i->setDoSpindown(sd);
    	i->setCommand(cmd);
    	i->setRepeat(rp);
	}


    // Go trough all the disks in the file
    for( int i=0 ; i < iniparser_getnsec(ini) ; i++ )
    {
        // Read the name of the section.
        section = iniparser_getsecname(ini, i);

        if(section.substr(0,4) == "disk")
        {
        	idleTime = iniparser_getint(ini, string(section+":idle-time").data(), spindown.getDefaultDisk().getSpindownTime());
        	sd		 = iniparser_getboolean(ini, string(section+":spindown").data(), spindown.getDefaultDisk().getDoSpindown());
        	cmd		 = iniparser_getstring(ini, string(section+":command").data(), empty);
        	device	 = iniparser_getstring(ini, string(section+":device").data(), empty);
        	rp		 = iniparser_getboolean(ini, string(section+":repeat").data(), spindown.getDefaultDisk().getRepeat());

        	if(strcmp(device.data(), empty) == 0)
			{
		        Log::get()->message(LOG_WARNING, "Ignoring " + section + ", because no device is specified.");

		        continue;
			}

        	if(strcmp(cmd.data(), empty) == 0)
        		cmd = spindown.getDefaultDisk().getCommand();

        	try
        	{
        		Disk& disk = spindown.getDiskByName(device.substr(device.find_last_of('/') + 1));

    			disk.setSpindownTime(idleTime);
    			disk.setDoSpindown(sd);
    			disk.setCommand(cmd);
    			disk.setDevice(device);
    			disk.setRepeat(rp);
        	}

        	catch(SpindownException e)
        	{
				try
				{
					Disk& disk = spindown.getDiskByDevice(device);

	    			disk.setSpindownTime(idleTime);
	    			disk.setDoSpindown(sd);
	    			disk.setCommand(cmd);
	    			disk.setDevice(device);
	    			disk.setRepeat(rp);
				}

				catch(SpindownException e)
				{
					Disk disk(device, sd, cmd, idleTime, rp);

					spindown.getDisks().push_back(disk);

					continue;
				}
        	}
        }
    }

	spindown.sort();

    iniparser_freedict(ini);
}

void Spindownd::daemonize()
{
    // Our process ID and Session ID
    pid_t pid, sid;

    // Fork off the parent process
    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    // If we got a good PID, then we can exit the parent process.
    if (pid > 0)
    {
        //save the pid file
        ofstream pidFile;

        pidFile.open(pidPath.data());

        pidFile << pid;

        pidFile.close();

        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Create a new SID for the child process
    sid = setsid();

    if (sid < 0)
        exit(EXIT_FAILURE);

    // Change the current working directory
    if ((chdir("/")) < 0)
        exit(EXIT_FAILURE);

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

string Spindownd::relToAbs( string path )
{
    char buf[256];
    static string runPath = getcwd(buf, 256);

    // If it starts with "./" it's relative.
    if( path.substr(0,2) == "./" )
    {
        path.erase(0, 1);
        path.insert(0, runPath);
    }

    // Everything else that doesn't start with "/" it's relative.
    else if( path.substr(0,1) != "/" )
        path.insert( 0, runPath+"/" );

    return path;
}
