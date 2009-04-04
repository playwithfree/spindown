#ifndef SPINDOWND_CLASS_H
#define SPINDOWND_CLASS_H

#include <string>
using std::string;

class Spindown;

class Spindownd
{
    public:
        static void init(int, char*[]);
        static void run();
        static void readConfig(string const &);

    private:
        static void parseCommandline(int, char*[]);
        static void installSignals();
        static void daemonize();
        static void sigHandler(int);
        static string relToAbs(string);
        
        static Spindown* spindown;
        static string fifoPath;
        static string confPath;
        static string pidPath;
        static bool daemon;
};

#endif