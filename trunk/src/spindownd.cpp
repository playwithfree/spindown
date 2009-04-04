#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;

#include "general.h"
#include "log.h"
#include "disk.h"
#include "diskset.h"
#include "spindown.h"

#include "ininiparser3.0b/iniparser.h"
#include "ininiparser3.0b/dictionary.h"

#include "spindownd.h"

string Spindownd::fifoPath;
string Spindownd::confPath;
string Spindownd::pidPath;
bool Spindownd::daemon;
Spindown* Spindownd::spindown;

void Spindownd::init(int argc, char* argv[])
{
    fifoPath = relToAbs("./spindown.fifo");
    confPath = relToAbs("./spindown.conf");
    pidPath = relToAbs("./spindownd.pid");
    daemon = false;
    
    // First create spindown object, because it can be
    // configured in parseCommandline.
    spindown = new Spindown();
    
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

void Spindownd::run()
{
    while( true )
    {        
        spindown->cycle();
        spindown->wait();
    }
}

void Spindownd::installSignals()
{
    signal(SIGHUP, Spindownd::sigHandler);
    signal(SIGTERM, Spindownd::sigHandler);
    signal(SIGINT, Spindownd::sigHandler);
    signal(SIGPIPE, Spindownd::sigHandler);
}

void Spindownd::sigHandler(int signalNumber)
{
    switch( signalNumber )
    {
        case SIGHUP:
            readConfig(confPath);
            break;
            
        case SIGPIPE:
            {
                // We need to make sure that there is a process that has the fifo
                // open for reading else the whole daemon will lock up. If no program
                // opens the fifo we'll repport the error and continue.
                int file = open(fifoPath.data(), O_NONBLOCK | O_WRONLY);

                if(errno==0)
                {
                    string status = spindown->getStatusString();
                    write (file, status.data(), status.size());
                }
                else
                    Log::get()->message(LOG_INFO, "Failed to write to the fifo.");

                close(file);
            }
            break;

        case SIGINT:
        case SIGTERM:
            delete spindown;

            remove(fifoPath.data());
            remove(pidPath.data());

            Log::get()->message(LOG_INFO, "Daemon stopped.");

            exit(EXIT_SUCCESS);
            break;

        default:
            break;
    }
}

void Spindownd::parseCommandline(int argc, char* argv[] )
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
            << "  -f, --fifo-path      The path to the fifo that is used to communicate the" << endl
            << "                          status." << endl
            << "  -c, --config-file     Path to the configuration file. The default is" << endl
            << "                          spindown.conf in the current directory." << endl
            << "  -p, --pid-file        Path to the pid-file. The default is" << endl
            << "                          spindownd.pid in the current directory." << endl
            << "  -d, --daemon          Daemonize the the process." << endl
            << "  -h, --help            Displays this text." << endl
            << "  -V, --version         Prints the version number." << endl << endl
            << "For more information and contact visit <http://code.google.com/p/spindown/>." <<endl;
            exit(EXIT_SUCCESS);
        }

        else
        {
            //set fifo path
            if( (arg=="-s"||arg=="--status-file") && i+1 < argc )
                std::cerr << "Statusfile is no longer used. Please use -f or --fifo-path." << endl
                        << "    Please read the help text for more info." << endl;
                
            if( (arg=="-f"||arg=="--fifo-path"||arg=="-s"||arg=="--status-file") && i+1 < argc )
                fifoPath = relToAbs(argv[++i]);

            //set config file path
            else if( (arg=="-c" || arg=="--config-file") && i+1 < argc )
                confPath = relToAbs(argv[++i]);

            //daemonize
            else if( arg=="-d" || arg=="--daemon" )
                daemon = true;
            
            //pid path
            else if( arg=="-p" || arg=="--pid-file" )
                pidPath = relToAbs(argv[++i]);
        }
    }
}

void Spindownd::readConfig(string const &path)
{
    dictionary* ini;
    string section;
    string input;
    struct stat sbuf;
    DiskSet* newDiskSet, oldDiskSet;
    int commonSpinDownTime = 7200;

    stat(confPath.data(), &sbuf);

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
    if( (ini=iniparser_load(path.data()))==NULL )
    {
        std::cerr << "Can't open the configuration file: " << path << endl;
        exit(1);
    }

    newDiskSet = new DiskSet;

    // Go trough the sections of the file
    for( int i=0 ; i < iniparser_getnsec(ini) ; i++ )
    {
        //read the name of the section
        section = iniparser_getsecname(ini, i);

        //general section?
        if( section=="general" )
        {
            commonSpinDownTime = iniparser_getint(ini, string(section+":idle-time").data(), 7200);

            if( commonSpinDownTime <= 0 )
                commonSpinDownTime = 7200;

            spindown->cycleTime = iniparser_getint(ini, string(section+":cycle-time").data(), 60);

            if( iniparser_getboolean(ini, string(section+":syslog").data(), 0) )
                Log::get()->open( (char*)"spindown", LOG_NDELAY, LOG_DAEMON );
            else
                Log::get()->close();
        }
        //disk?
        else if( section.substr(0,4) == "disk" )
        {
            // the parsing of the configuration is up to the Disk class
            Disk* newDisk = Disk::create(*ini, section);

            if (!newDisk)
                continue;

            newDiskSet->push_back(newDisk);
        }
    }

    //free the memory of the directory
    iniparser_freedict(ini);

    // initialise both the DiskSet as the Disks
    newDiskSet->setCommonSpinDownTime(commonSpinDownTime);

    // If a previous configuration exists, copy the internal status
    // to the new configuration and delete the old one.
    if (spindown->disks)
    {
        Spindown::updateDevNames(newDiskSet);
        spindown->updateDevNames();
        newDiskSet->setStatsFrom(*spindown->disks);
        delete spindown->disks;
    }

    spindown->disks = newDiskSet;
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
    static string runPath = "";
    if( runPath == "" )
    {
        char buf[CHAR_BUF];
        runPath = getcwd( buf, CHAR_BUF );
    }
    
    //if it starts with "./" it's relative
    if( path.substr(0,2) == "./" )
    {
        path.erase(0,1);
        path.insert( 0, runPath );
    }

    //everything else that doesn't start with "/" it's relative
    else if( path.substr(0,1) != "/" )
        path.insert( 0, runPath+"/" );

    return path;
}