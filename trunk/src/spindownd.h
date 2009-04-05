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
        static void run();
        
        /**
         * Reads the configuration file and configures spindown.
         */
        static void readConfig(string const &);

    private:
        static Spindown* spindown;
        static string fifoPath;
        static string confPath;
        static string pidPath;
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