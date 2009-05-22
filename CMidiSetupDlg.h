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

#ifndef _CMidiSetupDlg_h
#define _CMidiSetupDlg_h

#include "wx/wxprec.h"
#include "wx/statbox.h"
#include "CProteus.h"
#include "CProteusController.h"

class CMidiSetupDlg : public wxDialog
{
public:
	CMidiSetupDlg(wxWindow* parent, CProteusControllerApp* app);
	~CMidiSetupDlg();

	void OnCancel(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);

	wxChoice *mMidiIn;
	wxChoice *mMidiOut;
	wxChoice *mMidiThru;
	wxTextCtrl *mSysExId;

	CMidiDeviceInfo mDevices;

	CProteusControllerApp* mApp;
	wxString mOptionsPath;

	DECLARE_EVENT_TABLE()
};


#endif // _CMidiSetupDlg_h