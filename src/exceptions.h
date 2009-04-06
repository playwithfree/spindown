#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>
using std::string;

class SpindownException
{
    public:
        SpindownException(string);
        string message;
};

#endif