/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009 Dimitri Michaux <dimitri.michaux@gmail.com>
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

#include <syslog.h>

#include <string>

using std::string;

#include "log.h"

Log* Log::get()
{
    static Log* log = NULL;
    
    if( log == NULL )
        log = new Log;
    
    return log;
}

Log::Log()
{
    opend = false;
}

void Log::open( char* ident, int option, int facility)
{
    if( !opend )
        openlog( ident, option, facility );
    
    opend = true;
}

void Log::message(int facility_priority, string message)
{
    if( opend )
        syslog(facility_priority, message.data());
}

void Log::close()
{
    if( opend )
        closelog();
    
    opend = false;
}

Log::~Log()
{
    close();
}
