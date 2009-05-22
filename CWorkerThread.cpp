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

#include "CWorkerThread.h"
#include "CProteusController.h"
#include "Events.h"
#include "wx/string.h"

CWorkerThread::CWorkerThread(CProteusControllerApp* app, CProteus* proteus) : wxThread()
{
	mApp = app;
	Proteus = proteus;
	
	mROM = 0;
	mPresetID = 0;
}

void* CWorkerThread::Entry()
{
	switch(mOperation)
	{
		case eLoadPreset:
		{
			long err = Proteus->LoadPreset(mROM, mPresetID, this);
			if(err) return NULL;
			
			wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, eLoadPresetComplete );	
			wxPostEvent( mApp, event );
		}
		break;

		case eDownloadPresets:
		{
			
			wxString fileName;
			mApp->GetAppPath(fileName);

			if(mROM == 0)
			{
				fileName.append("\\UserPresets.txt");
				mApp->Proteus.DownloadUserPresetNames(this);
				mApp->Proteus.SaveUserPresetNames(fileName.GetData());
			}
			else
			{
				fileName.append(wxString::Format("\\ROMPresets_%i_.txt", mROM));
				mApp->Proteus.DownloadROMPresetNames(mROM, this);
				mApp->Proteus.SaveROMPresetNames(fileName, mROM);
			}
			
			wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, eDownloadPresetsComplete );	
			wxPostEvent( mApp, event );
		}
		break;

		case eDownloadInstruments:
		{
			wxString fileName;
			mApp->GetAppPath(fileName);
			fileName.append("\\InstrumentNames.txt");
			mApp->Proteus.DownloadInstrumentNames(this);
			mApp->Proteus.SaveInstrumentNames(fileName.GetData());

			wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, eDownloadInstrumentsComplete );	
			wxPostEvent( mApp, event );
		}
		break;
	}
	
	return NULL;
}

void CWorkerThread::Set(int operation, short ROM, short presetID)
{
	mOperation = operation;

	if(ROM == -1)
		mROM = 0;
	else
		mROM = ROM; 
	
	mPresetID = presetID;
}

void CWorkerThread::SetTotalSteps(int steps)
{
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, eSetStepsEvent);
	event.SetInt(steps);
    wxPostEvent( mApp, event );
}

void CWorkerThread::Step()
{
	wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, eStepEvent );	
    wxPostEvent( mApp, event );
}

bool CWorkerThread::IsCancelled()
{
	if(TestDestroy())
		return true;
	
	if(mApp->mProgressDlg)
		return mApp->mProgressDlg->mCancelled;
	
	return false;
}