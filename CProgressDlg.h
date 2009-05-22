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

#ifndef _CProgressDlg_h_
#define _CProgressDlg_h_

#include "wx/wxprec.h"
#include "wx/gauge.h"
#include "wx/statbox.h"
#include "wx/thread.h"

#include "Interfaces.h"

class CProgressDlg : public wxDialog, public IProgressCallback
{
public:
	CProgressDlg(wxWindow* parent, char* text);

	wxGauge* mGauge;
	wxButton* mCancel;
	bool mCancelled;

	void OnCancel(wxCommandEvent& event);

	void SetTotalSteps(int steps);
	void Step();
	bool IsCancelled() { return mCancelled; }
	void UnlockCriticalSection();

	DECLARE_EVENT_TABLE()
};

#define CANCEL_BUTTON_ID 3

#endif // _CProgressDlg_h_