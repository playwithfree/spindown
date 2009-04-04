/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009 Dimitri Michaux <dimitri.michaux@gmail.com>
 *
 * log.h:
 * The class declaration for Log. This class can log messages trough
 * syslog.
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

#ifndef _LOG_H
#define _LOG_H

// Include here altough it is not needed. It will make all the defines
// from syslog.h available to every file that includes this one.
#include <syslog.h>

#include <string>
using std::string;

class Log
{
    public:
        static Log* get();
        
        Log();
        
        void open(char*, int, int);
        
        void message(int, string);
        
        void close();

        ~Log();
        
    private:
        bool opend;
};

#endif
