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

#ifndef _CProteusController_h_
#define _CProteusController_h_

#include "wx/wxprec.h"

#include "wx/splitter.h"
#include "wx/sizer.h"
#include "wx/image.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/notebook.h"
#include "wx/spinbutt.h"
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/thread.h"
#include "wx/snglinst.h"
#include "wx/splash.h"

#include "Events.h"
#include "CProteus.h"
#include "CProgressDlg.h"
#include "CWorkerThread.h"

class PresetTreeCtrl : public wxTreeCtrl
{
public:
	PresetTreeCtrl(wxWindow* parent);
	void OnTreeItemActivated(wxCommandEvent& event);
	void OnTreeItemSelected (wxCommandEvent& event);

	wxTreeItemId mLastItem;
	
	DECLARE_EVENT_TABLE()
};


class TreePanel : public wxPanel 
{
public:
	TreePanel(wxWindow* parent);
	virtual ~TreePanel ();

	void Populate(CProteus* proteus);
	void SetSelectedName(const char* name);
	void DeleteAllItems() { mTreeCtrl->DeleteAllItems(); }
	
protected:
	PresetTreeCtrl* mTreeCtrl;
	
	DECLARE_EVENT_TABLE()
};

class EditorPanel : public wxScrolledWindow 
{
public:
	void LoadPreset(Preset* preset);
	void PopulateControls();
	void PopulateInstruments(short layer, short rom);
	void EnableVelEnvCtrls(int layer, bool enable);
	void EnableControls(bool enable);
	void OnEvent(wxCommandEvent& event);
	void OnSpinEvent(wxSpinEvent& event);
	void OnNotebookEvent(wxNotebookEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void LoadMasterParameters();

	Preset* mCurPreset;
	short mCurLayer;
	bool mInitializing;

	EditorPanel(wxWindow* parent);
	virtual ~EditorPanel();
	
	wxTextCtrl*	mPresetNameText1;
	wxTextCtrl*	mPresetNameText2;
	wxNotebook*	mLayerTabs;

	wxChoice*	mInstrument[4];
	wxChoice*	mROM[4];
	wxSlider*	mVolume[4];
	wxSlider*	mPan[4];

	wxChoice*	mKeyLow[4];
	wxSpinCtrl* mKeyLowFade[4];
	wxChoice*	mKeyHigh[4];
	wxSpinCtrl* mKeyHighFade[4];

	wxSpinCtrl* mVelocityLow[4];
	wxSpinCtrl* mVelocityLowFade[4];
	wxSpinCtrl* mVelocityHigh[4];
	wxSpinCtrl* mVelocityHighFade[4];

	wxSpinCtrl* mRTLow[4];
	wxSpinCtrl* mRTLowFade[4];
	wxSpinCtrl* mRTHigh[4];
	wxSpinCtrl* mRTHighFade[4];

	wxSpinCtrl* mTranspose[4];
	wxSpinCtrl* mTuningCoarse[4];
	wxSpinCtrl* mTuningFine[4];

	wxChoice*	mFilter[4];
	wxSpinCtrl* mFilterFreq[4];
	wxSpinCtrl* mFilterRes[4];

	wxChoice*	mLFO1Shape[4];
	wxChoice*	mLFO1Rate[4];
	wxChoice*	mLFO1Sync[4];
	wxSpinCtrl* mLFO1Delay[4];
	wxSpinCtrl* mLFO1Var[4];

	wxChoice*	mLFO2Shape[4];
	wxChoice*	mLFO2Rate[4];
	wxChoice*	mLFO2Sync[4];
	wxSpinCtrl* mLFO2Delay[4];
	wxSpinCtrl* mLFO2Var[4];

	wxChoice*	mVelEnvMode[4];
	wxSpinCtrl* mVelEnvAttack1[4];
	wxSpinCtrl* mVelEnvAttack2[4];
	wxSpinCtrl* mVelEnvDecay1[4];
	wxSpinCtrl* mVelEnvDecay2[4];
	wxSpinCtrl* mVelEnvRelease1[4];
	wxSpinCtrl* mVelEnvRelease2[4];
	wxSpinCtrl* mVelEnvAttack1Rt[4];
	wxSpinCtrl* mVelEnvAttack2Rt[4];
	wxSpinCtrl* mVelEnvDecay1Rt[4];
	wxSpinCtrl* mVelEnvDecay2Rt[4];
	wxSpinCtrl* mVelEnvRelease1Rt[4];
	wxSpinCtrl* mVelEnvRelease2Rt[4];

	wxChoice*	mFilEnvMode[4];
	wxSpinCtrl* mFilEnvAttack1[4];
	wxSpinCtrl* mFilEnvAttack2[4];
	wxSpinCtrl* mFilEnvDecay1[4];
	wxSpinCtrl* mFilEnvDecay2[4];
	wxSpinCtrl* mFilEnvRelease1[4];
	wxSpinCtrl* mFilEnvRelease2[4];
	wxSpinCtrl* mFilEnvAttack1Rt[4];
	wxSpinCtrl* mFilEnvAttack2Rt[4];
	wxSpinCtrl* mFilEnvDecay1Rt[4];
	wxSpinCtrl* mFilEnvDecay2Rt[4];
	wxSpinCtrl* mFilEnvRelease1Rt[4];
	wxSpinCtrl* mFilEnvRelease2Rt[4];

	wxChoice*	mAuxEnvMode[4];
	wxSpinCtrl* mAuxEnvAttack1[4];
	wxSpinCtrl* mAuxEnvAttack2[4];
	wxSpinCtrl* mAuxEnvDecay1[4];
	wxSpinCtrl* mAuxEnvDecay2[4];
	wxSpinCtrl* mAuxEnvRelease1[4];
	wxSpinCtrl* mAuxEnvRelease2[4];
	wxSpinCtrl* mAuxEnvAttack1Rt[4];
	wxSpinCtrl* mAuxEnvAttack2Rt[4];
	wxSpinCtrl* mAuxEnvDecay1Rt[4];
	wxSpinCtrl* mAuxEnvDecay2Rt[4];
	wxSpinCtrl* mAuxEnvRelease1Rt[4];
	wxSpinCtrl* mAuxEnvRelease2Rt[4];

	wxChoice*	mPatchcordSources[4][PATCHCORDS];
	wxChoice*	mPatchcordDests[4][PATCHCORDS];
	wxSpinCtrl* mPatchcordAmts[4][PATCHCORDS];

	wxChoice*	mPPatchcordSources[PPATCHCORDS];
	wxChoice*	mPPatchcordDests[PPATCHCORDS];
	wxSpinCtrl* mPPatchcordAmts[PPATCHCORDS];

	wxSpinCtrl* mChorus[4];
	wxSpinCtrl* mChorusWidth[4];
	wxSpinCtrl* mStart[4];
	wxSpinCtrl* mDelay[4];
	wxChoice*	mNonTranspose[4];
	wxChoice*	mSoloMode[4];
	wxChoice*	mAssignGroup[4];
	wxChoice*	mGlide[4];
	wxChoice*	mGlideCurve[4];

	wxRadioBox* mTempoButtons;
	wxSpinCtrl* mInternalTempo;

	wxChoice*	mFXAAlg;
	wxSpinCtrl* mFXADecay;
	wxSpinCtrl* mFXAHFDamp;
	wxSpinCtrl* mFXBFXA;
	wxSpinCtrl* mFXASendAmt1;
	wxSpinCtrl* mFXASendAmt2;
	wxSpinCtrl* mFXASendAmt3;
	wxSpinCtrl* mFXASendAmt4;

	wxChoice*	mFXBAlg;
	wxSpinCtrl* mFXBFeedbk;
	wxSpinCtrl* mFXBLFORate;
	wxChoice*	mFXBDelay;
	wxSpinCtrl* mFXBSendAmt1;
	wxSpinCtrl* mFXBSendAmt2;
	wxSpinCtrl* mFXBSendAmt3;
	wxSpinCtrl* mFXBSendAmt4;

	wxTextCtrl* mRTControllerA;
	wxTextCtrl* mRTControllerB;
	wxTextCtrl* mRTControllerC;
	wxTextCtrl* mRTControllerD;
	wxTextCtrl* mRTControllerE;
	wxTextCtrl* mRTControllerF;
	wxTextCtrl* mRTControllerG;
	wxTextCtrl* mRTControllerH;
	wxTextCtrl* mRTControllerI;
	wxTextCtrl* mRTControllerJ;
	wxTextCtrl* mRTControllerK;
	wxTextCtrl* mRTControllerL;
	wxButton*	mRTControllerSave;

	
	DECLARE_EVENT_TABLE()
};

class MainFrame: public wxFrame
{
public:
    MainFrame(int height, int width, int sash);
    virtual ~MainFrame();
	
    void Exit(wxCommandEvent& event);
	void BuildMenus();
	void SetMenuState(bool connected);
	void OnDownloadUserPresets(wxCommandEvent& event);
	void OnDownloadROMPresets(wxCommandEvent& event);
	void OnDownloadInstruments(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnReadme(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);

    TreePanel* mLeftFrame;
	EditorPanel* mRightFrame;
    wxSplitterWindow* mSplitter;

	wxMenuItem* mConnectMenuItem;
	wxMenuItem* mDisconnectMenuItem;
	wxMenuItem* mDownloadInstrumentsMenuItem;
	wxMenuItem* mReloadMenuItem;
	wxMenu*		mDownloadNamesMenu;
	
    DECLARE_EVENT_TABLE()
};

class CProteusControllerApp: public wxApp
{
public:
	MainFrame*					mMainFrame;
	wxCriticalSection			mCriticalSection;
	CProteus					Proteus;
	CMidiDeviceInfo				mDevices;
	CProgressDlg*				mProgressDlg;
	wxSingleInstanceChecker*	mInstanceChecker;
	wxSplashScreen*				mSplash;

	short						mROM;
	short						mPresetID;
	int							mStatus;
	
	int							mInId;
	int							mOutId;
	int							mThruId;

	bool OnInit			();
	int  OnExit			();
	
	long Connect		();
	void GetAppPath		(wxString& path);
	long LoadPreset		(short ROM, short presetID, bool force = false);
	long ChangePreset	(short ROM, short presetID);
	long RenamePreset	(Preset* preset, wxString& presetName);
	void SaveAs			(int presetId);
	void SetTempoText	();

	long LoadUserPresets();
	long LoadInstruments();
	long LoadROMPresets	();

	void OnStepEvent					(wxCommandEvent& event);
	void OnSetStepsEvent				(wxCommandEvent& event);
	void OnLoadPresetComplete			(wxCommandEvent& event);
	void OnDownloadPresetsComplete		(wxCommandEvent& event);
	void OnDownloadInstrumentsComplete	(wxCommandEvent& event);
	void OnMidiSetup					(wxCommandEvent& event);
	void OnConnect						(wxCommandEvent& event);
	void OnDisconnect					(wxCommandEvent& event);
	void OnReload						(wxCommandEvent& event);
	void OnRTControllerChange			(wxCommandEvent& event);
	void OnTempoChange					(wxCommandEvent& event);
	void OnToggleTempos					(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

class TreeData : public wxTreeItemData
{
public:
	short PresetID;
	short ROM;
};

short IdxFromMapValue(short* pMap, int numElems, short val);

#endif // _CProteusController_h_