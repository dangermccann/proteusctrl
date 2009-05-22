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

#ifndef _CSaveAsDlg_h
#define _CSaveAsDlg_h

#include "wx/wxprec.h"
#include "wx/statbox.h"
#include "CProteus.h"
#include "CProteusController.h"

class CSaveAsDlg : public wxDialog
{
public:
	CSaveAsDlg(wxWindow* parent, CProteusControllerApp* app);
	~CSaveAsDlg();
	
	void OnCancel(wxCommandEvent& event);
	void OnOK(wxCommandEvent& event);
	
	wxChoice *mPresetChoice;
	CProteusControllerApp* mApp;

	
	DECLARE_EVENT_TABLE()
};

#define SAVE_AS_CANCEL	0x12
#define SAVE_AS_OK		0x13

#endif	// _CSaveAsDlg_h