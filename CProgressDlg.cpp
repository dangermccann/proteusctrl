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

#include "CProgressDlg.h"

BEGIN_EVENT_TABLE(CProgressDlg, wxDialog)
	EVT_BUTTON(CANCEL_BUTTON_ID, OnCancel)
END_EVENT_TABLE()

CProgressDlg::CProgressDlg(wxWindow* parent, char* text) : wxDialog(parent, -1, text, 		
														wxDefaultPosition, wxSize(300, 130))
{
	SetIcon(wxIcon("PRESET_ICON", wxBITMAP_TYPE_ICO_RESOURCE));

	wxStaticBox* box = new wxStaticBox(this, -1, text, wxPoint(5, 5), wxSize(285, 90));
	mGauge = new wxGauge(this, -1, 10, wxPoint(10, 20), wxSize(275, 35), wxGA_HORIZONTAL | wxGA_SMOOTH);
	mCancel = new wxButton(this, CANCEL_BUTTON_ID, "Cancel", wxPoint(120, 60), wxSize(60, 25));

	mCancelled = false;
}

void CProgressDlg::SetTotalSteps(int steps)
{
	mGauge->SetRange(steps);
}

void CProgressDlg::Step()
{
	mGauge->SetValue(mGauge->GetValue()+1);
}

void CProgressDlg::OnCancel(wxCommandEvent& event)
{
	mCancelled = true;
	Show(FALSE);
}
