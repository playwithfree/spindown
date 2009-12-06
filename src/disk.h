/**
 * Spindown is a daemon that can spindown idle discs.
 * Copyright (C) 2008-2009  Dimitri Michaux <dimitri.michaux@gmail.com>
 *
 * disk.h:
 * Class defination of Disk. Keeps all the parameters for a disk, plus
 * detects when a disk has been idle long enough and then spins it down,
 * it also decides which disks should be spundown.
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

#ifndef DISC_CLASS_H
#define DISC_CLASS_H

#include <string>
using std::string;

class Disk
{
public:
	Disk(string device = "", bool sd = false
			, string cmd = "sg_start --stop --pc=2", int sdTime = 1800
			, bool rp = false);

	void poke();

	void   setDevice(string);

	string getDevName() const;

	string getDevice() const;

	bool getConnected() const;
	bool getActive() const;
	bool getIdle() const;

	void 		 setBlocksTransferred(unsigned int);
	unsigned int getBlocksTransferred();

	void setSpindownTime(int);
	int	 getSpindownTime() const;

	int		getIdleTime() const;

	bool getDoSpindown() const;
	void setDoSpindown(bool);

	string	getCommand() const;
	void 	setCommand(string);

	void	setRepeat(bool);
	bool	getRepeat() const;

	void spindown();


private:
	string device
		,command
		,devName;

	time_t lastActive;

	int spindownTime;

	unsigned int blocksTransferred;

	bool doSpindown
		,repeat
		,active
		,connected;

	string searchDevName() const;

	bool searchConnected() const;
};

#endif
