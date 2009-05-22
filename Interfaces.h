/*
	Proteus Controller - A Preset editor for E-mu Proteus compatible
	sound modules
	Copyright (C) 2004 John McCann

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	http://www.gnu.org/copyleft/gpl.html
*/

#ifndef _Interfaces_h_
#define _Interfaces_h_

class IProgressCallback
{
public:
	virtual void SetTotalSteps(int steps) = 0;
	virtual void Step() = 0;
	virtual bool IsCancelled() = 0;
};

#endif _Interfaces_h_