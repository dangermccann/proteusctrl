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

#ifndef _CWorkerThread_h_
#define _CWorkerThread_h_

#include "Interfaces.h"
#include "wx/thread.h"
#include "CProteus.h"

enum
{
	eLoadPreset,
	eDownloadPresets,
	eDownloadInstruments,
};

class CProteusControllerApp;
class CWorkerThread : public wxThread, public IProgressCallback
{
public:
	CProteusControllerApp* mApp;
	CProteus* Proteus;
	short mROM;
	short mPresetID;
	int mOperation;
	
	CWorkerThread(CProteusControllerApp* app, CProteus* proteus);
	void* Entry();
	void Set(int operation, short ROM, short presetID);
	
	void SetTotalSteps(int steps);
	void Step();
	bool IsCancelled();
};


#endif // _CWorkerThread_h_