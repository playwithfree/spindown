/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009 Dimitri Michaux <dimitri.michaux@gmail.com>
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

#ifndef SPINDOWN_CLASS_H
#define SPINDOWN_CLASS_H

#include <string>
#include <list>

using namespace std;

class Disk;

class Spindown
{
public:
	Spindown(string);

	void updateDisks();

	void spindownIdleDisks();

	void printSet();

	void sort();

	void pokeDisks();

	Disk& getDiskByDevice(string);

	Disk& getDiskByName(string);

	list<Disk>& getDisks();

	Disk& getDefaultDisk();

private:
	list<Disk> disks;

	Disk defaultDisk;

	string devById;

	string diskstats;
};

#endif
