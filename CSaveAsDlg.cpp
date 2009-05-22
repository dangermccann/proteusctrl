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

#include "CSaveAsDlg.h"

BEGIN_EVENT_TABLE(CSaveAsDlg, wxDialog)
	EVT_BUTTON(SAVE_AS_CANCEL,	OnCancel)
	EVT_BUTTON(SAVE_AS_OK,		OnOK)
END_EVENT_TABLE()

CSaveAsDlg::CSaveAsDlg(wxWindow* parent, CProteusControllerApp* app)
						: wxDialog(parent, -1, "Save Preset As", wxDefaultPosition, wxSize(280, 100))
{
	mApp = app;

	wxStaticText* text = new wxStaticText(this, -1, "Save Preset As:", wxPoint(10, 10), wxSize(100, 20));
	mPresetChoice = new wxChoice(this, -1, wxPoint(110, 10), wxSize(150, 20));

	char buff[17];
	for(int i = 0; i < app->Proteus.mNumUserPresets; i++)
	{
		memcpy(buff, app->Proteus.mPresetNames[i], NAME_LENGTH);
		buff[16] = 0;
		wxString str(buff);
		str.Trim();
		mPresetChoice->Append(str);
	}

	wxButton* okButton = new wxButton(this, SAVE_AS_OK, "OK", wxPoint(135, 40), wxSize(50, 20));
	wxButton* cancelButton = new wxButton(this, SAVE_AS_CANCEL, "Cancel", wxPoint(190, 40), wxSize(50, 20));
}

CSaveAsDlg::~CSaveAsDlg()
{
}

void CSaveAsDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(0);
}

void CSaveAsDlg::OnOK(wxCommandEvent& event)
{
	if(mPresetChoice->GetSelection() > -1)
		mApp->SaveAs(mPresetChoice->GetSelection());
	
	EndModal(1);
}