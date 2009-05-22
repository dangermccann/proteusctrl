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

#include "CMidiSetupDlg.h"

#define MIDI_SETUP_CANCEL	0x10
#define MIDI_SETUP_OK		0x11

BEGIN_EVENT_TABLE(CMidiSetupDlg, wxDialog)
	EVT_BUTTON(MIDI_SETUP_CANCEL,	OnCancel)
	EVT_BUTTON(MIDI_SETUP_OK,		OnOK)
END_EVENT_TABLE()

CMidiSetupDlg::CMidiSetupDlg(wxWindow* parent, CProteusControllerApp* app)
				: wxDialog(parent, -1, "Midi Setup", wxDefaultPosition, wxSize(280, 190))
{
	mApp = app;
	app->GetAppPath(mOptionsPath);
	mOptionsPath.Append("\\options.ini");

	mDevices.Initialize(mOptionsPath);

	wxStaticText* static1 = new wxStaticText(this, -1, "Midi In:", wxPoint(20, 20), wxSize(80, 20));
	wxStaticText* static2 = new wxStaticText(this, -1, "Midi Out:", wxPoint(20, 45), wxSize(80, 20));
	wxStaticText* static3 = new wxStaticText(this, -1, "Keyboard:", wxPoint(20, 70), wxSize(80, 20));
	wxStaticText* static4 = new wxStaticText(this, -1, "SysEx ID:", wxPoint(20, 95), wxSize(80, 20));

	mMidiIn   = new wxChoice(this, -1, wxPoint(80, 20), wxSize(150, 20));
	mMidiOut  = new wxChoice(this, -1, wxPoint(80, 45), wxSize(150, 20));
	mMidiThru = new wxChoice(this, -1, wxPoint(80, 70), wxSize(150, 20));

	unsigned int i = 0;
	for(i = 0; i < mDevices.NumInDevices; i++)
	{
		mMidiIn->Append(mDevices.InDevices[i]);
	}

	mMidiIn->Select(mDevices.SelectedInDeviceId);

	for(i = 0; i < mDevices.NumOutDevices; i++)
	{
		mMidiOut->Append(mDevices.OutDevices[i]);
	}
	mMidiOut->Select(mDevices.SelectedOutDeviceId);

	mMidiThru->Append("None");
	for(i = 0; i < mDevices.NumInDevices; i++)
	{
		mMidiThru->Append(mDevices.InDevices[i]);
	}
	mMidiThru->Select(mDevices.SelectedThruDeviceId + 1);

	mSysExId = new wxTextCtrl(this, -1, "0", wxPoint(80, 95), wxSize(30, 20));
	mSysExId->SetValue(wxString::Format("%i", mDevices.SelectedSysExId));

	wxButton* okButton = new wxButton(this, MIDI_SETUP_OK, "OK", wxPoint(135, 120), wxSize(50, 20));
	wxButton* cancelButton = new wxButton(this, MIDI_SETUP_CANCEL, "Cancel", wxPoint(190, 120), wxSize(50, 20));

}

CMidiSetupDlg::~CMidiSetupDlg()
{
}

void CMidiSetupDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(0);
}

void CMidiSetupDlg::OnOK(wxCommandEvent& event)
{
	if(mMidiIn->GetSelection() < 0 || mMidiOut->GetSelection() < 0)
	{
		wxMessageDialog* mdgdlg = new wxMessageDialog(this, "You must select a MIDI input and output device.", 
			"Proteus Controller");
		mdgdlg->ShowModal();
		delete mdgdlg;
		return;
	}

	WritePrivateProfileString("MidiSetup", "InDevice", mMidiIn->GetString(mMidiIn->GetSelection()), mOptionsPath);
	WritePrivateProfileString("MidiSetup", "OutDevice", mMidiOut->GetString(mMidiOut->GetSelection()), mOptionsPath);
	WritePrivateProfileString("MidiSetup", "ThruDevice", mMidiThru->GetString(mMidiThru->GetSelection()), mOptionsPath);
	WritePrivateProfileString("MidiSetup", "SysExId", mSysExId->GetValue(), mOptionsPath);

	EndModal(1);
}