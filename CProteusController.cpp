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

#include "CProteusController.h"
#include "Commands.h"
#include "Interfaces.h"
#include "CMidiSetupDlg.h"
#include "CSaveAsDlg.h"
#include "Mshtmhst.h"

IMPLEMENT_APP(CProteusControllerApp)

BEGIN_EVENT_TABLE(CProteusControllerApp, wxApp)
	EVT_MENU(eStepEvent,					CProteusControllerApp::OnStepEvent)
	EVT_MENU(eSetStepsEvent,				CProteusControllerApp::OnSetStepsEvent)
	EVT_MENU(eLoadPresetComplete,			CProteusControllerApp::OnLoadPresetComplete)
	EVT_MENU(eDownloadPresetsComplete,		CProteusControllerApp::OnDownloadPresetsComplete)
	EVT_MENU(eDownloadInstrumentsComplete,	CProteusControllerApp::OnDownloadInstrumentsComplete)
	EVT_MENU(eMenuMidiSetup,				CProteusControllerApp::OnMidiSetup)
	EVT_MENU(eMenuConnect,					CProteusControllerApp::OnConnect)
	EVT_MENU(eMenuDisconnect,				CProteusControllerApp::OnDisconnect)
	EVT_MENU(eMenuReload,					CProteusControllerApp::OnReload)
	EVT_MENU(eRTControllerChange,			CProteusControllerApp::OnRTControllerChange)
END_EVENT_TABLE()

bool CProteusControllerApp::OnInit()
{
	mInstanceChecker = new wxSingleInstanceChecker("ProetusController");
	if(mInstanceChecker->IsAnotherRunning())
	{
		return FALSE;
	}
	
	wxBitmap bitmap("SPLASH_BITMAP", wxBITMAP_TYPE_BMP_RESOURCE);

	mSplash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
			3000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);
	wxYield();

    mProgressDlg = NULL;
	mStatus		 = eStatusNA;

	Proteus.mApp = this;
	
	wxString optionsPath;
	GetAppPath(optionsPath);
	optionsPath.Append("\\options.ini");
	mDevices.Initialize(optionsPath);

	int height, width, sash;
	height	= GetPrivateProfileInt("WindowMetrics", "Height", 725, optionsPath);
	width	= GetPrivateProfileInt("WindowMetrics", "Width", 950, optionsPath);
	sash	= GetPrivateProfileInt("WindowMetrics", "Sash", 200, optionsPath);
	
    mMainFrame = new MainFrame(height, width, sash);
	mMainFrame->BuildMenus();
	mMainFrame->mRightFrame->EnableControls(false);
	mMainFrame->SetStatusText("Disconnected");

	if(mDevices.SelectedInDeviceId == -1 || mDevices.SelectedOutDeviceId == -1)
	{
		mSplash->Show(FALSE);
		delete mSplash;
		mSplash = NULL;

		CMidiSetupDlg* dlg = new CMidiSetupDlg(mMainFrame, this);
		dlg->ShowModal();
		mDevices.Initialize(optionsPath);

		mSplash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
			3000, NULL, -1, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxSTAY_ON_TOP);
		wxYield();
	}

	long err = -1;
	if(mDevices.SelectedInDeviceId != -1 && mDevices.SelectedOutDeviceId != -1)
	{
		err = Connect();
	}

	mMainFrame->Show(TRUE);

	if(mSplash)
	{
		mSplash->Show(FALSE);
		delete mSplash;
		mSplash = NULL;
	}

	if(!err)
	{
		err = LoadUserPresets();
		if(err) return TRUE;
		
		err = LoadInstruments();
		if(err) return TRUE;
		
		LoadROMPresets();
	}

    return TRUE;
}

int CProteusControllerApp::OnExit()
{
	Proteus.Disconnect();
	delete mInstanceChecker;
	return 0;
}

long CProteusControllerApp::Connect()
{
	long err = Proteus.Connect(mDevices.SelectedInDeviceId, mDevices.SelectedOutDeviceId,
		mDevices.SelectedThruDeviceId, mDevices.SelectedSysExId);
	if(err)
	{
		if(mSplash)
			mSplash->Show(FALSE);

		wxMessageDialog* mdgdlg = new wxMessageDialog(mMainFrame, "Error connecting to MIDI device.", 
			"Proteus Controller");
		mdgdlg->ShowModal();
		delete mdgdlg;
		mMainFrame->SetMenuState(false);
		return err;
	}
	mMainFrame->SetMenuState(true);
	mMainFrame->SetStatusText("Connected");
	return S_OK;
}

void CProteusControllerApp::GetAppPath(wxString& path)
{
	char strFileName[MAX_PATH+1];
	GetModuleFileName(NULL, strFileName, MAX_PATH);
	path = strFileName;
	path = path.BeforeLast('\\');
}

long CProteusControllerApp::LoadPreset(short ROM, short presetID, bool force)
{
	mROM = ROM;
	mPresetID = presetID;

	if(!force)
	{
		if(ROM == 0)
		{
			if(Proteus.mUserPresets[presetID])
			{
				mMainFrame->mRightFrame->LoadPreset(Proteus.mUserPresets[presetID]);
				Proteus.ProgramChange(0, presetID);
				Proteus.SetParameter(PRESET_SELECT, presetID);
				return 0;
			}
		}
		else
		{
			int romIdx = Proteus.GetSimmIdx(ROM);
			if(Proteus.mSimms[romIdx]->mPresets[presetID])
			{
				mMainFrame->mRightFrame->LoadPreset(Proteus.mSimms[romIdx]->mPresets[presetID]);
				Proteus.ProgramChange(ROM, presetID);
				return 0;
			}
		}
	}

	mProgressDlg = new CProgressDlg(mMainFrame, "Loading Preset...");

	CWorkerThread *workerThread = new CWorkerThread(this, &Proteus);
	workerThread->Set(eLoadPreset, ROM, presetID);
	workerThread->Create();
	workerThread->Run();

	mProgressDlg->ShowModal();
	
	return 0;
}

long CProteusControllerApp::ChangePreset(short ROM, short presetID)
{
	return Proteus.ProgramChange(ROM, presetID);
}

long CProteusControllerApp::RenamePreset(Preset* preset, wxString& presetName)
{
	Proteus.RenamePreset(presetName);
	mMainFrame->mLeftFrame->SetSelectedName(presetName);

	presetName.Pad(16, ' ');
	memcpy(preset->GeneralParameters.PresetName, presetName.GetData(), NAME_LENGTH);
	memcpy(Proteus.mPresetNames[preset->ID], presetName.GetData(), NAME_LENGTH);

	wxString fileName;
	GetAppPath(fileName);
	fileName.append("\\UserPresets.txt");
	Proteus.SaveUserPresetNames(fileName);

	return S_OK;
}

void CProteusControllerApp::SaveAs(int presetId)
{
	if(!mMainFrame->mRightFrame->mCurPreset)
		return;

	Proteus.SavePreset(mROM, mMainFrame->mRightFrame->mCurPreset->ID, presetId);

	char buffer[NAME_LENGTH+1];
	memcpy(buffer, mMainFrame->mRightFrame->mCurPreset->GeneralParameters.PresetName, NAME_LENGTH);
	buffer[NAME_LENGTH] = 0;
	wxString presetName(buffer);
	presetName.Trim();
	presetName.Pad(16, ' ');
	memcpy(Proteus.mPresetNames[presetId], presetName.GetData(), NAME_LENGTH);

	mMainFrame->mLeftFrame->Populate(&Proteus);

	wxString fileName;
	GetAppPath(fileName);
	fileName.append("\\UserPresets.txt");
	Proteus.SaveUserPresetNames(fileName);
}

long CProteusControllerApp::LoadUserPresets()
{
	wxString userPresets;
	GetAppPath(userPresets);
	
	userPresets.append("\\UserPresets.txt");
	long err = Proteus.LoadUserPresetNames(userPresets.GetData());

	if(err == S_OK)
	{
		mMainFrame->mLeftFrame->Populate(&Proteus);
		return err;
	}
	else
	{
		wxMessageDialog msgdlg(mMainFrame, "You have not yet loaded the preset names from your Proteus sound module.  Would you like to download the preset names now (you will only have to do this once)?",
			"Proteus Controller", wxYES_NO | wxCENTRE);
		if(msgdlg.ShowModal() == wxID_YES)
		{
			mStatus = eStatusDownloadUserPresets;
			wxCommandEvent e;
			mMainFrame->OnDownloadUserPresets(e);
			return S_OK;
		}
	}
	return E_FAIL;
}

long CProteusControllerApp::LoadInstruments()
{
	wxString instrumentNames;
	GetAppPath(instrumentNames);
	instrumentNames.append("\\InstrumentNames.txt");
	long err = Proteus.LoadInstrumentNames(instrumentNames.GetData());

	if(err == S_OK)
	{
		mMainFrame->mRightFrame->PopulateControls();
		return err;
	}
	else
	{
		wxMessageDialog msgdlg(mMainFrame, "You have not yet loaded the instrument names from your Proteus sound module.  Would you like to download the instrument names now (you will only have to do this once)?",
			"Proteus Controller", wxYES_NO | wxCENTRE);
		if(msgdlg.ShowModal() == wxID_YES)
		{
			mStatus = eStatusDownloadInstruments;
			wxCommandEvent e;
			mMainFrame->OnDownloadInstruments(e);
			return S_OK;
		}
	}
	return E_FAIL;
}

long CProteusControllerApp::LoadROMPresets()
{
	for(int i = 0; i < Proteus.mNumSimms; i++)
	{
		int romID = Proteus.mSimms[i]->mSimmID;
		wxString fileName;
		GetAppPath(fileName);
		fileName.append(wxString::Format("\\ROMPresets_%i_.txt", romID));
		Proteus.LoadROMPresetNames(	fileName, romID);
	}

	mMainFrame->mLeftFrame->Populate(&Proteus);
	return S_OK;
}

void CProteusControllerApp::OnStepEvent(wxCommandEvent& event)
{
	if(mProgressDlg)
		mProgressDlg->Step();
}

void CProteusControllerApp::OnSetStepsEvent(wxCommandEvent& event)
{
	if(mProgressDlg)
		mProgressDlg->SetTotalSteps(event.GetInt());
}

void CProteusControllerApp::OnLoadPresetComplete(wxCommandEvent& event)
{
	Preset* preset = NULL;
	if(mROM != 0)
		preset = Proteus.mSimms[Proteus.GetSimmIdx(mROM)]->mPresets[mPresetID];
	else
		preset = Proteus.mUserPresets[mPresetID];
	
	if(preset)
	{
		mMainFrame->mRightFrame->LoadPreset(preset);
	}

	if(mProgressDlg)
	{
		mProgressDlg->Show(FALSE);
		delete mProgressDlg;
	}
}

void CProteusControllerApp::OnDownloadPresetsComplete(wxCommandEvent& event)
{
	if(mProgressDlg)
	{
		mProgressDlg->Show(FALSE);
		delete mProgressDlg;
	}
	if(mStatus == eStatusDownloadUserPresets)
	{
		mStatus = eStatusNA;
		LoadInstruments();
	}
	mMainFrame->mLeftFrame->Populate(&Proteus);
}

void CProteusControllerApp::OnDownloadInstrumentsComplete(wxCommandEvent& event)
{
	if(mProgressDlg)
	{
		mProgressDlg->Show(FALSE);
		delete mProgressDlg;
	}
	if(mStatus == eStatusDownloadInstruments)
	{
		mStatus = eStatusNA;
	}
	mMainFrame->mRightFrame->PopulateControls();
	if(mMainFrame->mRightFrame->mCurPreset)
		mMainFrame->mRightFrame->LoadPreset(mMainFrame->mRightFrame->mCurPreset);
}

void CProteusControllerApp::OnMidiSetup(wxCommandEvent& event)
{
	CMidiSetupDlg* dlg = new CMidiSetupDlg(mMainFrame, this);
	
	if(dlg->ShowModal())
	{
		wxString optionsPath;
		GetAppPath(optionsPath);
		optionsPath.Append("\\options.ini");
		mDevices.Initialize(optionsPath);

		wxCommandEvent evt;
		OnDisconnect(evt);
		OnConnect(evt);
	}
}

void CProteusControllerApp::OnConnect(wxCommandEvent& event)
{
	long err = Connect();
	if(err) return;

	err = LoadUserPresets();
	if(err) return;
	
	err = LoadInstruments();
	if(err) return;
	
	LoadROMPresets();
}

void CProteusControllerApp::OnDisconnect(wxCommandEvent& event)
{
	Proteus.Disconnect();
	mMainFrame->SetMenuState(false);
	mMainFrame->mLeftFrame->DeleteAllItems();
	mMainFrame->mRightFrame->EnableControls(false);
	mMainFrame->mRightFrame->mCurPreset = NULL;
	mMainFrame->SetStatusText("Disconnected");
}

void CProteusControllerApp::OnReload(wxCommandEvent& event)
{
	if(mMainFrame->mRightFrame->mCurPreset)
		LoadPreset(mROM, mPresetID, true);
}

void CProteusControllerApp::OnRTControllerChange(wxCommandEvent& event)
{
	if(!mMainFrame->mRightFrame->mCurPreset)
		return;

	int clientData = (int) event.GetClientData();
	byte rtMsg = clientData & 0xFF;
	byte rtAmt = (clientData & 0xFF00) >> 8;
	switch(rtMsg)
	{
		case RT_MSG_A:
			mMainFrame->mRightFrame->mRTControllerA->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_B:
			mMainFrame->mRightFrame->mRTControllerB->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_C:
			mMainFrame->mRightFrame->mRTControllerC->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_D:
			mMainFrame->mRightFrame->mRTControllerD->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_E:
			mMainFrame->mRightFrame->mRTControllerE->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_F:
			mMainFrame->mRightFrame->mRTControllerF->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_G:
			mMainFrame->mRightFrame->mRTControllerG->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_H:
			mMainFrame->mRightFrame->mRTControllerH->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_I:
			mMainFrame->mRightFrame->mRTControllerI->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_J:
			mMainFrame->mRightFrame->mRTControllerJ->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_K:
			mMainFrame->mRightFrame->mRTControllerK->SetValue(wxString::Format("%i", rtAmt));
			break;
		case RT_MSG_L:
			mMainFrame->mRightFrame->mRTControllerL->SetValue(wxString::Format("%i", rtAmt));
			break;
	}
}


BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(eMenuExit, MainFrame::Exit)
	EVT_MENU(eMenuDownloadInstruments, MainFrame::OnDownloadInstruments)
	EVT_MENU(eMenuDownloadUserPresets, MainFrame::OnDownloadUserPresets)
	EVT_MENU(eMenuHelpAbout,		   MainFrame::OnAbout)
	EVT_MENU(eMenuHelpReadme,		   MainFrame::OnReadme)
	EVT_MENU_RANGE(eMenuDownloadROMPresets, eMenuDownloadROMPresetsEnd, MainFrame::OnDownloadROMPresets)
	EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(int height, int width, int sash)
       : wxFrame(NULL, -1, _T("Proteus Controller"), wxDefaultPosition, wxSize(width, height),
                 wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)
{
    CreateStatusBar(1);

	SetIcon(wxIcon("PRESET_ICON", wxBITMAP_TYPE_ICO_RESOURCE));
    
    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    
    mSplitter = new wxSplitterWindow(this, -1, wxDefaultPosition, wxDefaultSize, 
		wxSP_3D | wxSP_LIVE_UPDATE | wxCLIP_CHILDREN);
    sizer->Add( mSplitter, 1, wxGROW, 0 );

    mLeftFrame = new TreePanel(mSplitter);
    mRightFrame = new EditorPanel(mSplitter);
	
    mSplitter->SplitVertically(mLeftFrame, mRightFrame, sash);

    SetSizer( sizer );

    SetStatusText(_T("Ready"), 0);
}

MainFrame::~MainFrame()
{
}

// menu command handlers
void MainFrame::Exit(wxCommandEvent& WXUNUSED(event) )
{
    Close(TRUE);
}

void MainFrame::BuildMenus()
{
	wxMenu *proteusMenu = new wxMenu;
	wxMenu *optionsMenu = new wxMenu;
	wxMenu *helpMenu = new wxMenu;
    
	mDownloadNamesMenu = new wxMenu;

	mConnectMenuItem	= new wxMenuItem(proteusMenu, eMenuConnect, _T("Connect"), _T("Connect"), false);
	proteusMenu->Append(mConnectMenuItem);
	mDisconnectMenuItem = new wxMenuItem(proteusMenu, eMenuDisconnect, _T("Disconnect"), _T("Disconnect"), false);
	proteusMenu->Append(mDisconnectMenuItem);

	mDownloadInstrumentsMenuItem = new wxMenuItem(proteusMenu, eMenuDownloadInstruments, 
		_T("Download Instrument Names"), _T("Download Instrument Names"), false);

	mReloadMenuItem = new wxMenuItem(proteusMenu, eMenuReload, _T("Reload Current Preset"), 
		_T("Reload Current Preset"), false);

	proteusMenu->AppendSeparator();
	proteusMenu->Append(eMenuDownloadPresets, _T("Download Preset Names"), mDownloadNamesMenu, _T("Download Preset Names"));
	proteusMenu->Append(mDownloadInstrumentsMenuItem);
	proteusMenu->AppendSeparator();
	proteusMenu->Append(mReloadMenuItem);
	proteusMenu->AppendSeparator();
	proteusMenu->Append(eMenuExit, _T("E&xit"), _T("Exit"));

	optionsMenu->Append(eMenuMidiSetup, _T("Midi Setup..."), _T("Midi Setup..."));

	helpMenu->Append(eMenuHelpAbout, _T("About..."), _T("About..."));
	helpMenu->Append(eMenuHelpReadme, _T("Readme..."), _T("Readme..."));
	
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(proteusMenu, _T("&Proteus"));
	menuBar->Append(optionsMenu, _T("&Options"));
	menuBar->Append(helpMenu, _T("&Help"));
    SetMenuBar(menuBar);
}

void MainFrame::SetMenuState(bool connected)
{
	mConnectMenuItem->Enable(!connected);
	mDisconnectMenuItem->Enable(connected);
	mDownloadInstrumentsMenuItem->Enable(connected);
	mReloadMenuItem->Enable(connected);

	int i = 0;
	int numItems = mDownloadNamesMenu->GetMenuItemCount();
	for(i = 0; i < numItems; i++)
	{
		wxMenuItem* item = mDownloadNamesMenu->FindItemByPosition(0);
		if(item)
		{
			mDownloadNamesMenu->Remove(item);
			delete item;
		}
	}
	if(connected)
	{
		mDownloadNamesMenu->Append(eMenuDownloadUserPresets, "Download User Preset Names", "Download User Preset Names");
		for(i = 0; i < wxGetApp().Proteus.mNumSimms; i++)
		{
			char name[NAME_LENGTH];
			wxGetApp().Proteus.mSimms[i]->GetName(name);
			wxString menuText = wxString::Format("Download ROM Presets: %s", name);
			mDownloadNamesMenu->Append(eMenuDownloadROMPresets + i, menuText, menuText);
		}
	}
}

void MainFrame::OnDownloadUserPresets(wxCommandEvent& event)
{
	wxGetApp().mProgressDlg = new CProgressDlg(this, "Downloading Preset Names...");

	CWorkerThread *workerThread = new CWorkerThread(&wxGetApp(), &wxGetApp().Proteus);
	workerThread->Set(eDownloadPresets, 0, 0);
	workerThread->Create();
	workerThread->Run();
	
	wxGetApp().mProgressDlg->ShowModal();
}

void MainFrame::OnDownloadROMPresets(wxCommandEvent& event)
{
	wxGetApp().mProgressDlg = new CProgressDlg(this, "Downloading Preset Names...");
	
	CWorkerThread *workerThread = new CWorkerThread(&wxGetApp(), &wxGetApp().Proteus);
	short romIdx = event.GetId() - eMenuDownloadROMPresets;

	short romId = 0;
	if(romId < wxGetApp().Proteus.mNumSimms)
		romId = wxGetApp().Proteus.mSimms[romIdx]->mSimmID;

	workerThread->Set(eDownloadPresets, romId, 0);
	workerThread->Create();
	workerThread->Run();
	
	wxGetApp().mProgressDlg->ShowModal();
}

void MainFrame::OnDownloadInstruments(wxCommandEvent& event)
{
	wxGetApp().mProgressDlg = new CProgressDlg(this, "Downloading Instrument Names...");

	CWorkerThread *workerThread = new CWorkerThread(&wxGetApp(), &wxGetApp().Proteus);
	workerThread->Set(eDownloadInstruments, 0, 0);
	workerThread->Create();
	workerThread->Run();
	
	wxGetApp().mProgressDlg->ShowModal();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	HINSTANCE hinstMSHTML = LoadLibrary(TEXT("MSHTML.DLL"));
	
    if (hinstMSHTML)
    {
        SHOWHTMLDIALOGFN* pfnShowHTMLDialog;
        pfnShowHTMLDialog = (SHOWHTMLDIALOGFN*)GetProcAddress(hinstMSHTML, TEXT("ShowHTMLDialog"));
        if (pfnShowHTMLDialog)
        {
            IMoniker *pURLMoniker;
			wxString fileName;
			WCHAR path[MAX_PATH];
			wcscpy(path, L"res://");
			GetModuleFileNameW(NULL, &path[6], MAX_PATH-7);
			wcscat(path, L"/about");
			
            BSTR bstrURL = SysAllocString(path);
            CreateURLMoniker(NULL, bstrURL, &pURLMoniker);
			
            if (pURLMoniker)
            {
                (*pfnShowHTMLDialog)((HWND) GetHandle(), pURLMoniker, NULL,
					L"dialogHeight:450px; dialogWidth:350px; status:no; help:no;", NULL);
				
                pURLMoniker->Release();
            }
			
            SysFreeString(bstrURL);
        }
		
        FreeLibrary(hinstMSHTML);
    }
}

void MainFrame::OnReadme(wxCommandEvent& event)
{
	wxString fileName;
	wxGetApp().GetAppPath(fileName);
	fileName.append("\\Readme.txt\"");
	fileName.Prepend("notepad.exe \"");
	wxExecute(fileName);
}

void MainFrame::OnClose(wxCommandEvent& event)
{
	int height = 0, width = 0, sash = 0;
	GetSize(&width, &height);
	sash = mSplitter->GetSashPosition();
	
	wxString optionsPath;
	wxGetApp().GetAppPath(optionsPath);
	optionsPath.Append("\\options.ini");
	wxString val;
	
	val = wxString::Format("%i", height);
	WritePrivateProfileString("WindowMetrics", "Height", val.GetData(), optionsPath);
	val = wxString::Format("%i", width);
	WritePrivateProfileString("WindowMetrics", "Width", val.GetData(), optionsPath);
	val = wxString::Format("%i", sash);
	WritePrivateProfileString("WindowMetrics", "Sash", val.GetData(), optionsPath);

	event.Skip();
}

// ----------------------------------------------------------------------------
// TreePanel
// ----------------------------------------------------------------------------
TreePanel::TreePanel(wxWindow* parent) : wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxNO_FULL_REPAINT_ON_RESIZE)
{
	mTreeCtrl = new PresetTreeCtrl(this);

	wxImageList *images = new wxImageList(16, 16, TRUE);
	wxIcon icons[3];
	icons[0] = wxIcon("ROOT_ICON", wxBITMAP_TYPE_ICO_RESOURCE);
	icons[1] = wxIcon("SIMM_ICON", wxBITMAP_TYPE_ICO_RESOURCE);
	icons[2] = wxIcon("PRESET_ICON", wxBITMAP_TYPE_ICO_RESOURCE);

	images->Add(icons[0]);
	images->Add(icons[1]);
	images->Add(icons[2]);

	mTreeCtrl->AssignImageList(images);

	wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
	sizer->Add( mTreeCtrl, 1, wxGROW, 0 );

	SetSizer( sizer );
}

TreePanel::~TreePanel()
{
}

void TreePanel::Populate(CProteus* proteus)
{
	mTreeCtrl->DeleteAllItems();

	wxTreeItemId rootId = mTreeCtrl->AddRoot("Proteus", 0);
	wxTreeItemId userPresetsId = mTreeCtrl->AppendItem(rootId, "User Presets", 1);
	wxTreeItemId SIMMsId = mTreeCtrl->AppendItem(rootId, "SIMMs", 1);

	char buff[17];
	for(int i = 0; i < proteus->mNumUserPresets; i++)
	{
		memcpy(buff, &proteus->mPresetNames[i], NAME_LENGTH);
		buff[16] = 0;
		wxString str(buff);
		str.Trim();
		TreeData* data = new TreeData();
		data->ROM = 0;
		data->PresetID = i;
		wxTreeItemId itemId = mTreeCtrl->AppendItem(userPresetsId, str, 2, 2, data);
	}

	for(int j = 0; j < proteus->mNumSimms; j++)
	{
		if(proteus->mSimms[j] && proteus->mSimms[j]->mNumPresets)
		{
			char name[NAME_LENGTH];
			proteus->mSimms[j]->GetName(name);
			wxTreeItemId simmItemId = mTreeCtrl->AppendItem(SIMMsId, wxString(name), 1);

			if(proteus->mSimms[j]->mLoadedPresets)
			{
				for(int k = 0; k < proteus->mSimms[j]->mNumPresets; k++)
				{
					if(k > MAX_PRESETS)
						break;

					memcpy(buff, &proteus->mSimms[j]->mPresetNames[k][0], NAME_LENGTH);
					buff[16] = 0;
					wxString itemStr(buff);
					itemStr.Trim();
					TreeData* itemData = new TreeData();
					itemData->ROM = proteus->mSimms[j]->mSimmID;
					itemData->PresetID = k;
					mTreeCtrl->AppendItem(simmItemId, itemStr, 2, 2, itemData);
				}
			}
		}
	}

	mTreeCtrl->Expand(rootId);
	mTreeCtrl->Expand(userPresetsId);
	mTreeCtrl->Expand(SIMMsId);

	mTreeCtrl->ScrollTo(rootId);
}

void TreePanel::SetSelectedName(const char* name)
{
	mTreeCtrl->SetItemText(mTreeCtrl->GetSelection(), wxString(name));
}

PresetTreeCtrl::PresetTreeCtrl(wxWindow* parent) : wxTreeCtrl(parent, eTreeCtrlID)
{
}

void PresetTreeCtrl::OnTreeItemActivated(wxCommandEvent& event)
{
	TreeData* data = (TreeData*) this->GetItemData(this->GetSelection());
	if(data)
	{
		short presetID = data->PresetID;
		short ROM = data->ROM;
		wxGetApp().LoadPreset(ROM, presetID);
		
		if(mLastItem.IsOk())
			SetItemBold(mLastItem, FALSE);
		
		mLastItem = GetSelection();
		SetItemBold(mLastItem);
	}
}
void PresetTreeCtrl::OnTreeItemSelected(wxCommandEvent& event)
{
	TreeData* data = (TreeData*) this->GetItemData(this->GetSelection());
	if(data)
	{
		short presetID = data->PresetID;
		short ROM = data->ROM;
		wxGetApp().ChangePreset(ROM, presetID);
	}
}


BEGIN_EVENT_TABLE(TreePanel, wxPanel)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// EditorPanel
// ----------------------------------------------------------------------------
EditorPanel::EditorPanel(wxWindow* parent) : wxScrolledWindow(parent, eEditorPanel, wxDefaultPosition, wxDefaultSize, 
															  wxNO_FULL_REPAINT_ON_RESIZE | wxTAB_TRAVERSAL)
{
	mCurPreset = NULL;
	mCurLayer = 0;
	mInitializing = true;

	EnableScrolling(true, true);
	SetScrollbars(10, 10, 71, 100, 0, 0, 0);
	
	wxStaticText* presetNameStatic = new wxStaticText(this, -1, "Preset Name:", wxPoint(5, 9), wxSize(100, 20));
	mPresetNameText1 = new wxTextCtrl(this, ePresetNameText1, "", wxPoint(80, 5), wxSize(30, 20));
	mPresetNameText2 = new wxTextCtrl(this, ePresetNameText2, "", wxPoint(115, 5), wxSize(100, 20));

	mPresetNameText1->SetMaxLength(3);
	mPresetNameText2->SetMaxLength(12);

	wxButton* saveAs = new wxButton(this, eSaveAs, "Save As...", wxPoint(250, 5), wxSize(100, 25));

	wxSize spinnerSize(45, 20);

	mLayerTabs = new wxNotebook(this, eNotebook, wxPoint(5, 30), wxSize(505, 840));
	
	wxPanel* notebookPanels[4];

	wxStaticBox *effectsBox		= new wxStaticBox(this,	-1, "Effects",			wxPoint(515, 30),	wxSize(190, 330), wxTAB_TRAVERSAL);
	wxStaticBox *ppatchcordsBox	= new wxStaticBox(this,	-1, "Preset Patchcords",wxPoint(10, 875),	wxSize(330, 120), wxTAB_TRAVERSAL);
	wxStaticBox *rtCtrlBox		= new wxStaticBox(this,	-1, "Initial Controller Amounts",wxPoint(515, 370),	wxSize(190, 130), wxTAB_TRAVERSAL);

	for(int j = 0; j < 4; j++)
	{
		wxString layerName = wxString::Format("Layer %i", j+1);
		notebookPanels[j] = new wxPanel(mLayerTabs, -1, wxPoint(5, 30), wxSize(1, 1));
		mLayerTabs->AddPage(notebookPanels[j], layerName, false);

		wxStaticBox *crossfadeBox	= new wxStaticBox(notebookPanels[j],	-1, "Crossfade",		wxPoint(10, 150),	wxSize(255, 110), wxTAB_TRAVERSAL);
		wxStaticBox *transposeBox	= new wxStaticBox(notebookPanels[j],	-1, "Transpose",		wxPoint(345, 370),	wxSize(135, 120), wxTAB_TRAVERSAL);
		wxStaticBox *filterBox		= new wxStaticBox(notebookPanels[j],	-1, "Filter",			wxPoint(10, 45),	wxSize(155, 100), wxTAB_TRAVERSAL);
		wxStaticBox *vEnvelopeBox	= new wxStaticBox(notebookPanels[j],	-1, "Velocity Envelope",wxPoint(10, 495),	wxSize(480, 100), wxTAB_TRAVERSAL);
		wxStaticBox *fEnvelopeBox	= new wxStaticBox(notebookPanels[j],	-1, "Filter Envelope",	wxPoint(10, 600),	wxSize(480, 100), wxTAB_TRAVERSAL);
		wxStaticBox *aEnvelopeBox	= new wxStaticBox(notebookPanels[j],	-1, "Auxiliary Envelope",wxPoint(10, 705),	wxSize(480, 100), wxTAB_TRAVERSAL);
		wxStaticBox *lfo1Box		= new wxStaticBox(notebookPanels[j],	-1, "LFO 1",			wxPoint(10, 265),	wxSize(200, 100), wxTAB_TRAVERSAL);
		wxStaticBox *lfo2Box		= new wxStaticBox(notebookPanels[j],	-1, "LFO 2",			wxPoint(220, 265),	wxSize(200, 100), wxTAB_TRAVERSAL);
		wxStaticBox *patchcordsBox	= new wxStaticBox(notebookPanels[j],	-1, "Patchcords",		wxPoint(10, 370),	wxSize(330, 120), wxTAB_TRAVERSAL);
		wxStaticBox *generalBox		= new wxStaticBox(notebookPanels[j],	-1, "Layer Properties",	wxPoint(275, 45),	wxSize(200, 200), wxTAB_TRAVERSAL);

		wxStaticText *instrumentNameStatic	= new wxStaticText(notebookPanels[j], -1, "Instrument:",	wxPoint(10, 19),	wxSize(60, 20));
		wxStaticText *romStatic				= new wxStaticText(notebookPanels[j], -1, "ROM:",			wxPoint(190, 19),	wxSize(40, 20));

		mInstrument[j]	= new wxChoice(notebookPanels[j],eInstrument,		wxPoint(69, 15),  wxSize(115, 20));
		mROM[j]			= new wxChoice(notebookPanels[j], eInstrumentROM,	wxPoint(225, 15), wxSize(55, 20));

		wxStaticText *volumeStatic	= new wxStaticText(notebookPanels[j], -1, "Volume:",	wxPoint(305, 5), wxSize(50, 20));
		wxStaticText *panStatic		= new wxStaticText(notebookPanels[j], -1, "Pan:",		wxPoint(395, 5), wxSize(30, 20));

		mVolume[j]	= new wxSlider(notebookPanels[j], eVolume,	0, 0, 255, wxPoint(295, 20),	wxSize(70, 20), wxSL_HORIZONTAL | wxSL_AUTOTICKS);
		mPan[j]		= new wxSlider(notebookPanels[j], ePan,		0, 0, 255, wxPoint(375, 20),	wxSize(70, 20), wxSL_HORIZONTAL | wxSL_AUTOTICKS);

		// Crossfade box
		wxStaticText *keyStatic			= new wxStaticText(crossfadeBox, -1, "Key:",		wxPoint(5, 34), wxSize(60, 20));
		wxStaticText *velocityStatic	= new wxStaticText(crossfadeBox, -1, "Velocity:",	wxPoint(5, 59), wxSize(60, 20));
		wxStaticText *rtStatic			= new wxStaticText(crossfadeBox, -1, "RT:",			wxPoint(5, 84), wxSize(60, 20));

		wxStaticText *lowStatic		= new wxStaticText(crossfadeBox, -1, "Low",	 wxPoint(55, 10),  wxSize(30, 20));
		wxStaticText *lofFadeStatic = new wxStaticText(crossfadeBox, -1, "Fade", wxPoint(105, 10), wxSize(30, 20));
		wxStaticText *highStatic	= new wxStaticText(crossfadeBox, -1, "High", wxPoint(155, 10), wxSize(30, 20));
		wxStaticText *highFadeStatic= new wxStaticText(crossfadeBox, -1, "Fade", wxPoint(205, 10), wxSize(30, 20));

		mKeyLow[j]	= new wxChoice(crossfadeBox, eKeyLow,	wxPoint(55, 30),	spinnerSize);
		mKeyHigh[j] = new wxChoice(crossfadeBox, eKeyHigh,	wxPoint(155, 30),	spinnerSize);

		mKeyLowFade[j]			= new wxSpinCtrl(crossfadeBox, eKeyLowFade,			"", wxPoint(105, 30), spinnerSize);
		mKeyHighFade[j]			= new wxSpinCtrl(crossfadeBox, eKeyHighFade,		"", wxPoint(205, 30), spinnerSize);

		mVelocityLow[j]			= new wxSpinCtrl(crossfadeBox, eVelocityLow,		"", wxPoint(55, 55),  spinnerSize);
		mVelocityLowFade[j]		= new wxSpinCtrl(crossfadeBox, eVelocityLowFade,	"", wxPoint(105, 55), spinnerSize);
		mVelocityHigh[j]		= new wxSpinCtrl(crossfadeBox, eVelocityHigh,		"", wxPoint(155, 55), spinnerSize);
		mVelocityHighFade[j]	= new wxSpinCtrl(crossfadeBox, eVelocityHighFade,	"", wxPoint(205, 55), spinnerSize);

		mRTLow[j]				= new wxSpinCtrl(crossfadeBox, eRTLow,				"", wxPoint(55, 80),  spinnerSize);
		mRTLowFade[j]			= new wxSpinCtrl(crossfadeBox, eRTLowFade,			"", wxPoint(105, 80), spinnerSize);
		mRTHigh[j]				= new wxSpinCtrl(crossfadeBox, eRTHigh,				"", wxPoint(155, 80), spinnerSize);
		mRTHighFade[j]			= new wxSpinCtrl(crossfadeBox, eRTHighFade,			"", wxPoint(205, 80), spinnerSize);

		// Transpose box
		wxStaticText *transposeStatic		= new wxStaticText(transposeBox, -1, "Transpose:",		wxPoint(5, 34), wxSize(80, 20));
		wxStaticText *tuningCoarseStatic	= new wxStaticText(transposeBox, -1, "Tuning Coarse:",	wxPoint(5, 59), wxSize(80, 20));
		wxStaticText *tuningFineStatic		= new wxStaticText(transposeBox, -1, "Tuning Fine:",	wxPoint(5, 84), wxSize(80, 20));

		mTranspose[j]	 = new wxSpinCtrl(transposeBox, eTranspose,		"", wxPoint(80, 30), spinnerSize);
		mTuningCoarse[j] = new wxSpinCtrl(transposeBox, eTuningCoarse,	"", wxPoint(80, 55), spinnerSize);
		mTuningFine[j]	 = new wxSpinCtrl(transposeBox, eTuningFine,	"", wxPoint(80, 80), spinnerSize);

		// filter box
		wxStaticText *filterStatic		= new wxStaticText(filterBox, -1, "Filter:",		wxPoint(5, 24), wxSize(50, 20));
		wxStaticText *filterFreqStatic	= new wxStaticText(filterBox, -1, "Filter Freq:",	wxPoint(5, 49), wxSize(60, 20));
		wxStaticText *filterResStatic	= new wxStaticText(filterBox, -1, "Filter Res:",	wxPoint(5, 74), wxSize(60, 20));

		mFilter[j]		= new wxChoice(filterBox,	eFilter,		wxPoint(60, 20), wxSize(90, 20));
		mFilterFreq[j]	= new wxSpinCtrl(filterBox, eFilterFreq,"", wxPoint(60, 45), spinnerSize);
		mFilterRes[j]	= new wxSpinCtrl(filterBox, eFilterRes,	"", wxPoint(60, 70), spinnerSize);

		// LFO 1 box
		wxStaticText *lfo1ShapeStatic	= new wxStaticText(lfo1Box, -1, "Shape:",		wxPoint(5, 24),   wxSize(80, 20));
		wxStaticText *lfo1RateStatic	= new wxStaticText(lfo1Box, -1, "Rate:",		wxPoint(5, 74),   wxSize(40, 20));
		wxStaticText *lfo1SyncStatic	= new wxStaticText(lfo1Box, -1, "Sync:",		wxPoint(5, 49),	  wxSize(40, 20));
		wxStaticText *lfo1DelayStatic	= new wxStaticText(lfo1Box, -1, "Delay:",		wxPoint(115, 49), wxSize(40, 20));
		wxStaticText *lfo1VarStatic		= new wxStaticText(lfo1Box, -1, "Var:",			wxPoint(115, 74), wxSize(40, 20));

		mLFO1Shape[j]	= new wxChoice(lfo1Box,		eLFO1Shape,		wxPoint(45, 20),	wxSize(95, 20));
		mLFO1Rate[j]	= new wxChoice(lfo1Box,		eLFO1Rate,		wxPoint(45, 70),	wxSize(65, 20));
		mLFO1Sync[j]	= new wxChoice(lfo1Box,		eLFO1Sync,		wxPoint(45, 45),	wxSize(65, 20));
		mLFO1Delay[j]	= new wxSpinCtrl(lfo1Box,	eLFO1Delay, "", wxPoint(150, 45),	spinnerSize);
		mLFO1Var[j]		= new wxSpinCtrl(lfo1Box,	eLFO1Var,	"", wxPoint(150, 70),	spinnerSize);

		// LFO 2 box
		wxStaticText *lfo2ShapeStatic	= new wxStaticText(lfo2Box, -1, "Shape:",		wxPoint(5, 24),		wxSize(80, 20));
		wxStaticText *lfo2RateStatic	= new wxStaticText(lfo2Box, -1, "Rate:",		wxPoint(5, 74),		wxSize(40, 20));
		wxStaticText *lfo2SyncStatic	= new wxStaticText(lfo2Box, -1, "Sync:",		wxPoint(5, 49),		wxSize(40, 20));
		wxStaticText *lfo2DelayStatic	= new wxStaticText(lfo2Box, -1, "Delay:",		wxPoint(115, 49),	wxSize(40, 20));
		wxStaticText *lfo2VarStatic		= new wxStaticText(lfo2Box, -1, "Var:",			wxPoint(115, 74),	wxSize(40, 20));
		
		mLFO2Shape[j]	= new wxChoice(lfo2Box,		eLFO2Shape,		wxPoint(45, 20),	wxSize(95, 20));
		mLFO2Rate[j]	= new wxChoice(lfo2Box,		eLFO2Rate,		wxPoint(45, 70),	wxSize(65, 20));
		mLFO2Sync[j]	= new wxChoice(lfo2Box,		eLFO2Sync,		wxPoint(45, 45),	wxSize(65, 20));
		mLFO2Delay[j]	= new wxSpinCtrl(lfo2Box,	eLFO2Delay, "", wxPoint(150, 45),	spinnerSize);
		mLFO2Var[j]		= new wxSpinCtrl(lfo2Box,	eLFO2Var,	"", wxPoint(150, 70),	spinnerSize);

		// velocity envelope box
		wxStaticText* vEnvelopeModeStatic		= new wxStaticText(vEnvelopeBox, -1, "Mode:",			wxPoint(10, 24),	wxSize(60, 20));
		wxStaticText* vEnvelopeAttack1Static	= new wxStaticText(vEnvelopeBox, -1, "Attack 1:",		wxPoint(10, 49),	wxSize(80, 20));
		wxStaticText* vEnvelopeAttack2Static	= new wxStaticText(vEnvelopeBox, -1, "Attack 2:",		wxPoint(165, 49),	wxSize(80, 20));
		wxStaticText* vEnvelopeDecay1Static		= new wxStaticText(vEnvelopeBox, -1, "Decay 1:",		wxPoint(325, 49),	wxSize(80, 20));
		wxStaticText* vEnvelopeDecay2Static		= new wxStaticText(vEnvelopeBox, -1, "Decay 2:",		wxPoint(10, 74),	wxSize(80, 20));
		wxStaticText* vEnvelopeRelease1Static	= new wxStaticText(vEnvelopeBox, -1, "Release 1:",		wxPoint(165, 74),	wxSize(80, 20));
		wxStaticText* vEnvelopeRelease2Static	= new wxStaticText(vEnvelopeBox, -1, "Release 2:",		wxPoint(325, 74),	wxSize(80, 20));

		mVelEnvMode[j]		= new wxChoice  (vEnvelopeBox,	eVelEnvMode,			wxPoint(50, 20),	wxSize(95, 20));
		mVelEnvAttack1[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvAttack1,		"", wxPoint(60, 45),	spinnerSize);
		mVelEnvAttack2[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvAttack2,		"", wxPoint(220, 45),	spinnerSize);
		mVelEnvDecay1[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvDecay1,		"",	wxPoint(380, 45),	spinnerSize);
		mVelEnvDecay2[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvDecay2,		"",	wxPoint(60, 70),	spinnerSize);
		mVelEnvRelease1[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvRelease1,	"", wxPoint(220, 70),	spinnerSize);
		mVelEnvRelease2[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvRelease2,	"", wxPoint(380, 70),	spinnerSize);
		mVelEnvAttack1Rt[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvAttack1Rt,	"", wxPoint(110, 45),	spinnerSize);
		mVelEnvAttack2Rt[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvAttack2Rt,	"",	wxPoint(270, 45),	spinnerSize);
		mVelEnvDecay1Rt[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvDecay1Rt,	"",	wxPoint(430, 45),	spinnerSize);
		mVelEnvDecay2Rt[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvDecay2Rt,	"",	wxPoint(110, 70),	spinnerSize);
		mVelEnvRelease1Rt[j]	= new wxSpinCtrl(vEnvelopeBox,	eVelEnvRelease1Rt,	"",	wxPoint(270, 70),	spinnerSize);
		mVelEnvRelease2Rt[j]= new wxSpinCtrl(vEnvelopeBox,	eVelEnvRelease2Rt,	"",	wxPoint(430, 70),	spinnerSize);

		// filter envelope box
		wxStaticText* fEnvelopeModeStatic		= new wxStaticText(fEnvelopeBox, -1, "Mode:",			wxPoint(10, 24),	wxSize(60, 20));
		wxStaticText* fEnvelopeAttack1Static	= new wxStaticText(fEnvelopeBox, -1, "Attack 1:",		wxPoint(10, 49),	wxSize(80, 20));
		wxStaticText* fEnvelopeAttack2Static	= new wxStaticText(fEnvelopeBox, -1, "Attack 2:",		wxPoint(165, 49),	wxSize(80, 20));
		wxStaticText* fEnvelopeDecay1Static		= new wxStaticText(fEnvelopeBox, -1, "Decay 1:",		wxPoint(325, 49),	wxSize(80, 20));
		wxStaticText* fEnvelopeDecay2Static		= new wxStaticText(fEnvelopeBox, -1, "Decay 2:",		wxPoint(10, 74),	wxSize(80, 20));
		wxStaticText* fEnvelopeRelease1Static	= new wxStaticText(fEnvelopeBox, -1, "Release 1:",		wxPoint(165, 74),	wxSize(80, 20));
		wxStaticText* fEnvelopeRelease2Static	= new wxStaticText(fEnvelopeBox, -1, "Release 2:",		wxPoint(325, 74),	wxSize(80, 20));
		
		mFilEnvMode[j]		= new wxChoice  (fEnvelopeBox,	eFilEnvMode,			wxPoint(50, 20),	wxSize(95, 20));
		mFilEnvAttack1[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvAttack1,		"", wxPoint(60, 45),	spinnerSize);
		mFilEnvAttack2[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvAttack2,		"", wxPoint(220, 45),	spinnerSize);
		mFilEnvDecay1[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvDecay1,		"",	wxPoint(380, 45),	spinnerSize);
		mFilEnvDecay2[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvDecay2,		"",	wxPoint(60, 70),	spinnerSize);
		mFilEnvRelease1[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvRelease1,	"", wxPoint(220, 70),	spinnerSize);
		mFilEnvRelease2[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvRelease2,	"", wxPoint(380, 70),	spinnerSize);
		mFilEnvAttack1Rt[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvAttack1Rt,	"", wxPoint(110, 45),	spinnerSize);
		mFilEnvAttack2Rt[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvAttack2Rt,	"",	wxPoint(270, 45),	spinnerSize);
		mFilEnvDecay1Rt[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvDecay1Rt,	"",	wxPoint(430, 45),	spinnerSize);
		mFilEnvDecay2Rt[j]	= new wxSpinCtrl(fEnvelopeBox,	eFilEnvDecay2Rt,	"",	wxPoint(110, 70),	spinnerSize);
		mFilEnvRelease1Rt[j]= new wxSpinCtrl(fEnvelopeBox,	eFilEnvRelease1Rt,	"",	wxPoint(270, 70),	spinnerSize);
		mFilEnvRelease2Rt[j]= new wxSpinCtrl(fEnvelopeBox,	eFilEnvRelease2Rt,	"",	wxPoint(430, 70),	spinnerSize);

		// auxiliary envelope box
		wxStaticText* aEnvelopeModeStatic		= new wxStaticText(aEnvelopeBox, -1, "Mode:",			wxPoint(10, 24),	wxSize(60, 20));
		wxStaticText* aEnvelopeAttack1Static	= new wxStaticText(aEnvelopeBox, -1, "Attack 1:",		wxPoint(10, 49),	wxSize(80, 20));
		wxStaticText* aEnvelopeAttack2Static	= new wxStaticText(aEnvelopeBox, -1, "Attack 2:",		wxPoint(165, 49),	wxSize(80, 20));
		wxStaticText* aEnvelopeDecay1Static		= new wxStaticText(aEnvelopeBox, -1, "Decay 1:",		wxPoint(325, 49),	wxSize(80, 20));
		wxStaticText* aEnvelopeDecay2Static		= new wxStaticText(aEnvelopeBox, -1, "Decay 2:",		wxPoint(10, 74),	wxSize(80, 20));
		wxStaticText* aEnvelopeRelease1Static	= new wxStaticText(aEnvelopeBox, -1, "Release 1:",		wxPoint(165, 74),	wxSize(80, 20));
		wxStaticText* aEnvelopeRelease2Static	= new wxStaticText(aEnvelopeBox, -1, "Release 2:",		wxPoint(325, 74),	wxSize(80, 20));
		
		mAuxEnvMode[j]		= new wxChoice  (aEnvelopeBox,	eAuxEnvMode,			wxPoint(50, 20),	wxSize(95, 20));
		mAuxEnvAttack1[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvAttack1,		"", wxPoint(60, 45),	spinnerSize);
		mAuxEnvAttack2[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvAttack2,		"", wxPoint(220, 45),	spinnerSize);
		mAuxEnvDecay1[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvDecay1,		"",	wxPoint(380, 45),	spinnerSize);
		mAuxEnvDecay2[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvDecay2,		"",	wxPoint(60, 70),	spinnerSize);
		mAuxEnvRelease1[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvRelease1,	"", wxPoint(220, 70),	spinnerSize);
		mAuxEnvRelease2[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvRelease2,	"", wxPoint(380, 70),	spinnerSize);
		mAuxEnvAttack1Rt[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvAttack1Rt,	"", wxPoint(110, 45),	spinnerSize);
		mAuxEnvAttack2Rt[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvAttack2Rt,	"",	wxPoint(270, 45),	spinnerSize);
		mAuxEnvDecay1Rt[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvDecay1Rt,	"",	wxPoint(430, 45),	spinnerSize);
		mAuxEnvDecay2Rt[j]	= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvDecay2Rt,	"",	wxPoint(110, 70),	spinnerSize);
		mAuxEnvRelease1Rt[j]= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvRelease1Rt,	"",	wxPoint(270, 70),	spinnerSize);
		mAuxEnvRelease2Rt[j]= new wxSpinCtrl(aEnvelopeBox,	eAuxEnvRelease2Rt,	"",	wxPoint(430, 70),	spinnerSize);

		// general box
		wxStaticText* chorusStatic	= new wxStaticText(generalBox, -1, "Chorus:",		wxPoint(10, 19),	wxSize(50, 20));
		wxStaticText* chorusWStatic	= new wxStaticText(generalBox, -1, "Width:",		wxPoint(115, 19),	wxSize(50, 20));
		wxStaticText* startStatic	= new wxStaticText(generalBox, -1, "Start:",		wxPoint(10, 44),	wxSize(50, 20));
		wxStaticText* delayStatic	= new wxStaticText(generalBox, -1, "Delay:",		wxPoint(115, 44),	wxSize(50, 20));
		wxStaticText* ntStatic		= new wxStaticText(generalBox, -1, "Non-Transpose:", wxPoint(10, 69),	wxSize(80, 20));
		wxStaticText* smStatic		= new wxStaticText(generalBox, -1, "Solo Mode:",	wxPoint(10, 94),	wxSize(80, 20));
		wxStaticText* assGpStatic	= new wxStaticText(generalBox, -1, "Assign Group:", wxPoint(10, 119),	wxSize(80, 20));
		wxStaticText* glideStatic	= new wxStaticText(generalBox, -1, "Glide:",		wxPoint(10, 144),	wxSize(50, 20));
		wxStaticText* glidecStatic	= new wxStaticText(generalBox, -1, "Curve:",		wxPoint(10, 169),	wxSize(50, 20));

		mChorus[j]		= new wxSpinCtrl(generalBox,	eChorus,		"", wxPoint(50, 15),	spinnerSize);
		mChorusWidth[j] = new wxSpinCtrl(generalBox,	eChorusWidth,	"", wxPoint(150, 15),	spinnerSize);
		mStart[j]		= new wxSpinCtrl(generalBox,	eStart,			"", wxPoint(50, 40),	spinnerSize);
		mDelay[j]		= new wxSpinCtrl(generalBox,	eDelay,			"", wxPoint(150, 40),	spinnerSize);
		mNonTranspose[j]= new wxChoice  (generalBox,	eNonTranspose,		wxPoint(100, 65),	wxSize(65, 20));
		mSoloMode[j]	= new wxChoice  (generalBox,	eSoloMode,			wxPoint(100, 90),	wxSize(95, 20));
		mAssignGroup[j]	= new wxChoice  (generalBox,	eAssignGroup,		wxPoint(100, 115),	wxSize(85, 20));
		mGlide[j]		= new wxChoice  (generalBox,	eGlide,				wxPoint(50, 140),	wxSize(105, 20));
		mGlideCurve[j]	= new wxChoice  (generalBox,	eGlideCurve,		wxPoint(50, 165),	wxSize(55, 20));

		// Patchcord box
		wxScrolledWindow* patchcordsWindow = new wxScrolledWindow(patchcordsBox, -1, wxPoint(5, 15), wxSize(320, 100), wxTAB_TRAVERSAL);
		
		int top = 5;
		for(int i = 0; i < PATCHCORDS; i++)
		{
			wxString name = wxString::Format("PC %i:", i+1);
			wxStaticText* patchcordStatic  = new wxStaticText(patchcordsWindow, -1, name, wxPoint(5, top+4), wxSize(35, 20));
			wxStaticText* patchcordStatic1 = new wxStaticText(patchcordsWindow, -1, "->", wxPoint(130, top+4), wxSize(15, 20));
			wxStaticText* patchcordStatic2 = new wxStaticText(patchcordsWindow, -1, "Amt", wxPoint(230, top+4), wxSize(30, 20));

			int psId = ePatchcordSources + i;
			int pdId = ePatchcordDests + i;
			int paId = ePatchcordAmts + i;
			mPatchcordSources[j][i] = new wxChoice(patchcordsWindow,	psId,		wxPoint(45, top),  wxSize(80, 20));
			mPatchcordDests[j][i]	= new wxChoice(patchcordsWindow,	pdId,		wxPoint(145, top), wxSize(80, 20));
			mPatchcordAmts[j][i]	= new wxSpinCtrl(patchcordsWindow,	paId, "",	wxPoint(255, top), spinnerSize);
			
			top += 25;

			this->Connect( psId,  -1, wxEVT_COMMAND_CHOICE_SELECTED,
				(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) OnEvent );
			this->Connect( pdId,  -1, wxEVT_COMMAND_CHOICE_SELECTED,
				(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) OnEvent );
			this->Connect( paId,  -1, wxEVT_COMMAND_SPINCTRL_UPDATED,
				(wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) OnSpinEvent );
		}

		patchcordsWindow->SetScrollbars(10, 10, 26, 61, 0, 0);
		patchcordsWindow->EnableScrolling(true, true);

	}

	// Preset Patchcord box
	wxScrolledWindow* ppatchcordsWindow = new wxScrolledWindow(ppatchcordsBox, -1, wxPoint(5, 15), wxSize(320, 100), wxTAB_TRAVERSAL);
	
	int top = 5;
	for(int i = 0; i < PPATCHCORDS; i++)
	{
		wxString name = wxString::Format("PC %i:", i+1);
		wxStaticText* ppatchcordStatic  = new wxStaticText(ppatchcordsWindow, -1, name, wxPoint(5, top+4), wxSize(35, 20));
		wxStaticText* ppatchcordStatic1 = new wxStaticText(ppatchcordsWindow, -1, "->", wxPoint(115, top+4), wxSize(15, 20));
		wxStaticText* ppatchcordStatic2 = new wxStaticText(ppatchcordsWindow, -1, "Amt", wxPoint(230, top+4), wxSize(30, 20));
		
		int ppsId = ePPatchcordSources + i;
		int ppdId = ePPatchcordDests + i;
		int ppaId = ePPatchcordAmts + i;
		mPPatchcordSources[i] = new wxChoice(ppatchcordsWindow,	ppsId,		wxPoint(45, top),  wxSize(65, 20));
		mPPatchcordDests[i]	= new wxChoice(ppatchcordsWindow,	ppdId,		wxPoint(130, top), wxSize(95, 20));
		mPPatchcordAmts[i]	= new wxSpinCtrl(ppatchcordsWindow,	ppaId, "",	wxPoint(255, top), spinnerSize);
		
		top += 25;
		
		this->Connect( ppsId,  -1, wxEVT_COMMAND_CHOICE_SELECTED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) OnEvent );
		this->Connect( ppdId,  -1, wxEVT_COMMAND_CHOICE_SELECTED,
			(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) OnEvent );
		this->Connect( ppaId,  -1, wxEVT_COMMAND_SPINCTRL_UPDATED,
			(wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) OnSpinEvent );
	}
	
	ppatchcordsWindow->SetScrollbars(10, 10, 26, 31, 0, 0);
	ppatchcordsWindow->EnableScrolling(true, true);
	

	mLayerTabs->SetSelection(0);

	// effects box
	wxStaticText* fxaAlgStatic = new wxStaticText(effectsBox, -1, "FXA Algorithm:", wxPoint(5, 24), wxSize(70, 20));
	wxStaticText* fxaDecayStatic = new wxStaticText(effectsBox, -1, "FXA Decay:", wxPoint(5, 49), wxSize(70, 20));
	wxStaticText* fxaHFDampStatic = new wxStaticText(effectsBox, -1, "FXA HFDamp:", wxPoint(5, 74), wxSize(70, 20));
	wxStaticText* fxaFxbStatic = new wxStaticText(effectsBox, -1, "FXB->FXA:", wxPoint(5, 99), wxSize(70, 20));
	wxStaticText* fxaSndAmtsStatic = new wxStaticText(effectsBox, -1, "FXA Send Amounts:", wxPoint(5, 120), wxSize(100, 20));

	mFXAAlg =		new wxChoice	(effectsBox, eFXAAlgId,			wxPoint(80, 20),	wxSize(105, 20));
	mFXADecay =		new wxSpinCtrl	(effectsBox, eFXADecay,		"", wxPoint(80, 45),	spinnerSize);
	mFXAHFDamp =	new wxSpinCtrl	(effectsBox, eFXAHFDamp,	"", wxPoint(80, 70),	spinnerSize);
	mFXBFXA =		new wxSpinCtrl	(effectsBox, eFXBFXA,		"", wxPoint(80, 95),	spinnerSize);
	mFXASendAmt1 =	new wxSpinCtrl	(effectsBox, eFXASendAmt1,	"", wxPoint(5, 140),	spinnerSize);
	mFXASendAmt2 =	new wxSpinCtrl	(effectsBox, eFXASendAmt2,	"", wxPoint(50, 140),	spinnerSize);
	mFXASendAmt3 =	new wxSpinCtrl	(effectsBox, eFXASendAmt3,	"", wxPoint(95, 140),	spinnerSize);
	mFXASendAmt4 =	new wxSpinCtrl	(effectsBox, eFXASendAmt4,	"", wxPoint(140, 140),	spinnerSize);

	wxStaticLine* fxStaticLine = new wxStaticLine(effectsBox, -1, wxPoint(5, 165), wxSize(180, 2));

	wxStaticText* fxbAlgStatic = new wxStaticText(effectsBox, -1, "FXB Algorithm:", wxPoint(5, 179), wxSize(70, 20));
	wxStaticText* fxbDecayStatic = new wxStaticText(effectsBox, -1, "FXB Feedback:", wxPoint(5, 204), wxSize(80, 20));
	wxStaticText* fxbHFDampStatic = new wxStaticText(effectsBox, -1, "FXB LFO Rate:", wxPoint(5, 229), wxSize(80, 20));
	wxStaticText* fxbFxbStatic = new wxStaticText(effectsBox, -1, "FXB Delay:", wxPoint(5, 254), wxSize(70, 20));
	wxStaticText* fxbSndAmtsStatic = new wxStaticText(effectsBox, -1, "FXB Send Amounts:", wxPoint(5, 279), wxSize(100, 20));
	
	mFXBAlg =		new wxChoice	(effectsBox, eFXBAlgId,			wxPoint(80, 175),	wxSize(105, 20));
	mFXBFeedbk =	new wxSpinCtrl	(effectsBox, eFXBFeedbk,	"", wxPoint(80, 200),	spinnerSize, 0);
	mFXBLFORate =	new wxSpinCtrl	(effectsBox, eFXBLFORate,	"", wxPoint(80, 225),	spinnerSize, 0);
	mFXBDelay =		new wxChoice	(effectsBox, eFXBDelay,			wxPoint(80, 250),	wxSize(85, 20));
	mFXBSendAmt1 =	new wxSpinCtrl	(effectsBox, eFXBSendAmt1,	"", wxPoint(5, 295),	spinnerSize);
	mFXBSendAmt2 =	new wxSpinCtrl	(effectsBox, eFXBSendAmt2,	"", wxPoint(50, 295),	spinnerSize);
	mFXBSendAmt3 =	new wxSpinCtrl	(effectsBox, eFXBSendAmt3,	"", wxPoint(95, 295),	spinnerSize);
	mFXBSendAmt4 =	new wxSpinCtrl	(effectsBox, eFXBSendAmt4,	"", wxPoint(140, 295),	spinnerSize);

	// Initial controller box
	wxStaticText* rtCtrlA = new wxStaticText(rtCtrlBox, -1, "A:", wxPoint(5, 24), wxSize(15, 20));
	wxStaticText* rtCtrlB = new wxStaticText(rtCtrlBox, -1, "B:", wxPoint(50, 24), wxSize(15, 20));
	wxStaticText* rtCtrlC = new wxStaticText(rtCtrlBox, -1, "C:", wxPoint(95, 24), wxSize(15, 20));
	wxStaticText* rtCtrlD = new wxStaticText(rtCtrlBox, -1, "D:", wxPoint(140, 24), wxSize(15, 20));
	wxStaticText* rtCtrlE = new wxStaticText(rtCtrlBox, -1, "E:", wxPoint(5, 49), wxSize(15, 20));
	wxStaticText* rtCtrlF = new wxStaticText(rtCtrlBox, -1, "F:", wxPoint(50, 49), wxSize(15, 20));
	wxStaticText* rtCtrlG = new wxStaticText(rtCtrlBox, -1, "G:", wxPoint(95, 49), wxSize(15, 20));
	wxStaticText* rtCtrlH = new wxStaticText(rtCtrlBox, -1, "H:", wxPoint(140, 49), wxSize(15, 20));
	wxStaticText* rtCtrlI = new wxStaticText(rtCtrlBox, -1, "I:", wxPoint(5, 74), wxSize(15, 20));
	wxStaticText* rtCtrlJ = new wxStaticText(rtCtrlBox, -1, "J:", wxPoint(50, 74), wxSize(15, 20));
	wxStaticText* rtCtrlK = new wxStaticText(rtCtrlBox, -1, "K:", wxPoint(95, 74), wxSize(15, 20));
	wxStaticText* rtCtrlL = new wxStaticText(rtCtrlBox, -1, "L:", wxPoint(140, 74), wxSize(15, 20));

	mRTControllerA = new wxTextCtrl(rtCtrlBox, eRTControllerA, "", wxPoint(20, 20), wxSize(25, 20));
	mRTControllerB = new wxTextCtrl(rtCtrlBox, eRTControllerB, "", wxPoint(65, 20), wxSize(25, 20));
	mRTControllerC = new wxTextCtrl(rtCtrlBox, eRTControllerC, "", wxPoint(110, 20), wxSize(25, 20));
	mRTControllerD = new wxTextCtrl(rtCtrlBox, eRTControllerD, "", wxPoint(155, 20), wxSize(25, 20));
	mRTControllerE = new wxTextCtrl(rtCtrlBox, eRTControllerE, "", wxPoint(20, 45), wxSize(25, 20));
	mRTControllerF = new wxTextCtrl(rtCtrlBox, eRTControllerF, "", wxPoint(65, 45), wxSize(25, 20));
	mRTControllerG = new wxTextCtrl(rtCtrlBox, eRTControllerG, "", wxPoint(110, 45), wxSize(25, 20));
	mRTControllerH = new wxTextCtrl(rtCtrlBox, eRTControllerH, "", wxPoint(155, 45), wxSize(25, 20));
	mRTControllerI = new wxTextCtrl(rtCtrlBox, eRTControllerI, "", wxPoint(20, 70), wxSize(25, 20));
	mRTControllerJ = new wxTextCtrl(rtCtrlBox, eRTControllerJ, "", wxPoint(65, 70), wxSize(25, 20));
	mRTControllerK = new wxTextCtrl(rtCtrlBox, eRTControllerK, "", wxPoint(110, 70), wxSize(25, 20));
	mRTControllerL = new wxTextCtrl(rtCtrlBox, eRTControllerL, "", wxPoint(155, 70), wxSize(25, 20));

	mRTControllerSave = new wxButton(rtCtrlBox, eRTControllerSave, "Save", wxPoint(15, 95), wxSize(75, 25));

	mInitializing = false;
}
EditorPanel::~EditorPanel()
{
}

void EditorPanel::PopulateControls()
{
	mInitializing = true;
	int i = 0;
	int j = 0;
	int k = 0;

	for(i = 0; i < 4; i++)
	{
		mROM[i]->Clear();
		for(k = 0; k < MAX_SIMMS; k++)
		{
			if(wxGetApp().Proteus.mSimms[k] && wxGetApp().Proteus.mSimms[k]->mNumInstruments)
			{
				char name[NAME_LENGTH];
				wxGetApp().Proteus.mSimms[k]->GetName(name);
				mROM[i]->Append(name);
			}
		}
		
		mVolume[i]->SetTickFreq(15, 0);
		mVolume[i]->SetRange(-96, 10);
		
		mPan[i]->SetTickFreq(16, 0);
		mPan[i]->SetRange(-64, 63);
		
		mKeyLow[i]->Clear();
		mKeyHigh[i]->Clear();
		for(k = 0; k < sizeof(kKeys) / sizeof(char*); k++)
		{
			mKeyLow[i]->Append(kKeys[k]);
			mKeyHigh[i]->Append(kKeys[k]);
		}
		
		mFilter[i]->Clear();
		for(k = 0; k < sizeof(kFilters) / sizeof(char*); k++)
		{
			mFilter[i]->Append(kFilters[k]);
		}
		
		mLFO1Shape[i]->Clear();
		mLFO2Shape[i]->Clear();
		for(k = 0; k < sizeof(kLFOShapes) / sizeof(char*); k++)
		{
			mLFO1Shape[i]->Append(kLFOShapes[k]);
			mLFO2Shape[i]->Append(kLFOShapes[k]);
		}
		
		mLFO1Rate[i]->Clear();
		mLFO2Rate[i]->Clear();
		for(k = 0; k < sizeof(kLFORates) / sizeof(char*); k++)
		{
			mLFO1Rate[i]->Append(kLFORates[k]);
			mLFO2Rate[i]->Append(kLFORates[k]);
		}
		
		mLFO1Sync[i]->Clear();
		mLFO1Sync[i]->Append("key sync");
		mLFO1Sync[i]->Append("free run");
		
		mLFO2Sync[i]->Clear();
		mLFO2Sync[i]->Append("key sync");
		mLFO2Sync[i]->Append("free run");

		mVelEnvMode[i]->Clear();
		mVelEnvMode[i]->Append("factory");
		mVelEnvMode[i]->Append("time-based");
		mVelEnvMode[i]->Append("tempo-based");

		mFilEnvMode[i]->Clear();
		mFilEnvMode[i]->Append("time-based");
		mFilEnvMode[i]->Append("tempo-based");

		mAuxEnvMode[i]->Clear();
		mAuxEnvMode[i]->Append("time-based");
		mAuxEnvMode[i]->Append("tempo-based");
		
		for(j = 0; j < PATCHCORDS; j++)
		{
			mPatchcordSources[i][j]->Clear();
			mPatchcordDests[i][j]->Clear();
			
			for(k = 0; k < sizeof(kPatchcordSources) / sizeof(char*); k++)
			{
				mPatchcordSources[i][j]->Append(kPatchcordSources[k]);
			}
			
			for(k = 0; k < sizeof(kPatchcordDests) / sizeof(char*); k++)
			{
				mPatchcordDests[i][j]->Append(kPatchcordDests[k]);
			}
		}

		mNonTranspose[i]->Clear();
		mNonTranspose[i]->Append("off");
		mNonTranspose[i]->Append("on");

		mSoloMode[i]->Clear();
		for(k = 0; k < sizeof(kSoloModes) / sizeof(char*); k++)
		{
			mSoloMode[i]->Append(kSoloModes[k]);
		}

		mAssignGroup[i]->Clear();
		for(k = 0; k < sizeof(kLayerGroups) / sizeof(char*); k++)
		{
			mAssignGroup[i]->Append(kLayerGroups[k]);
		}

		mGlide[i]->Clear();
		char buffer[200];
		for(k = 0; k < 127; k++)
		{
			memset(buffer, 0, sizeof(buffer));
			cnv_glide_rate(k, buffer);
			mGlide[i]->Append(buffer);
		}

		mGlideCurve[i]->Clear();
		mGlideCurve[i]->Append("linear");
		for(k = 1; k < 9; k++)
		{
			wxString strGlideCurve = wxString::Format("exp%i", k);
			mGlideCurve[i]->Append(strGlideCurve);
		}
	}

	for(j = 0; j < PPATCHCORDS; j++)
	{
		mPPatchcordSources[j]->Clear();
		mPPatchcordDests[j]->Clear();

		for(k = 0; k < sizeof(kPresetPatchcordSources) / sizeof(char*); k++)
		{
			mPPatchcordSources[j]->Append(kPresetPatchcordSources[k]);
		}
		
		for(k = 0; k < sizeof(kPresetPatchcordDests) / sizeof(char*); k++)
		{
			mPPatchcordDests[j]->Append(kPresetPatchcordDests[k]);
		}
	}

	mFXAAlg->Clear();
	for(i = 0; i < sizeof(kEffectsA) / sizeof(char*); i++)
	{
		mFXAAlg->Append(kEffectsA[i]);
	}

	mFXBAlg->Clear();
	for(i = 0; i < sizeof(kEffectsB) / sizeof(char*); i++)
	{
		mFXBAlg->Append(kEffectsB[i]);
	}

	mFXBDelay->Clear();
	for(i = 0; i < 12; i++)
	{
		mFXBDelay->Append(kLFORates[13 + i]);
	}
	for(i = 0; i < 127; i++)
	{
		wxString fxBDelay = wxString::Format("%i ms", (i * 5));
		mFXBDelay->Append(fxBDelay);
	}

	mInitializing = false;
}

void EditorPanel::PopulateInstruments(short layer, short rom_id)
{
	short rom_idx = wxGetApp().Proteus.GetSimmIdx(rom_id);
	if(rom_idx < 0)
		return;
	
	mInstrument[layer]->Clear();
	char buffer[NAME_LENGTH+1];
	for(int i = 0; i < wxGetApp().Proteus.mSimms[rom_idx]->mNumInstruments; i++)
	{
		memset(buffer, 0, NAME_LENGTH+1);
		memcpy(buffer, wxGetApp().Proteus.mSimms[rom_idx]->mInstrumentNames[i], NAME_LENGTH);
		buffer[NAME_LENGTH] = 0;
		mInstrument[layer]->Append(buffer);
	}
}

void EditorPanel::LoadPreset(Preset* preset)
{
	mInitializing = true;
	mCurPreset = preset;

	char name1[4];
	char name2[13];
	memcpy(name1, preset->GeneralParameters.PresetName, 3);
	name1[3] = 0;
	memcpy(name2, &preset->GeneralParameters.PresetName[4], 12);
	name2[12] = 0;
	wxString wxName1 = name1;
	wxName1.Trim();
	wxString wxName2 = name2;
	wxName2.Trim();

	mPresetNameText1->SetValue(wxName1);
	mPresetNameText2->SetValue(wxName2);

	for(int i = 0; i < 4; i++)
	{
		short rom_id = preset->Layers[i].LayerGeneralParameters.Layer_Inst_ROM_ID;
		mROM[i]->SetSelection(wxGetApp().Proteus.GetSimmIdx(rom_id));
		PopulateInstruments(i, rom_id);

		mInstrument[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Instrument);
		
		mVolume[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Volulme);
		mPan[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Pan);

		mKeyLow[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Key_Low);
		mKeyHigh[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Key_High);
		
		mKeyLowFade[i]->SetRange(0, 127);
		mKeyHighFade[i]->SetRange(0, 127);
		mKeyLowFade[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Key_LowFade);
		mKeyHighFade[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Key_HighFade);

		mVelocityLow[i]->SetRange(0, 127);
		mVelocityLowFade[i]->SetRange(0, 127);
		mVelocityHigh[i]->SetRange(0, 127);
		mVelocityHighFade[i]->SetRange(0, 127);

		mVelocityLow[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Vel_Low);
		mVelocityLowFade[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Vel_LowFade);
		mVelocityHigh[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Vel_High);
		mVelocityHighFade[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Vel_HighFade);

		mRTLow[i]->SetRange(0, 127);
		mRTLowFade[i]->SetRange(0, 127);
		mRTHigh[i]->SetRange(0, 127);
		mRTHighFade[i]->SetRange(0, 127);

		mRTLow[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_RT_Low);
		mRTLowFade[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_RT_LowFade);
		mRTHigh[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_RT_High);
		mRTHighFade[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_RT_HighFade);

		mTranspose[i]->SetRange(-36, 36);
		mTuningCoarse[i]->SetRange(-36, 36);
		mTuningFine[i]->SetRange(-64, 64);

		mTranspose[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Transpose);
		mTuningCoarse[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_CTune);
		mTuningFine[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_FTune);

		mFilterFreq[i]->SetRange(0, 255);
		mFilterRes[i]->SetRange(0, 127);

		mFilter[i]->SetSelection(IdxFromMapValue(kFilterMap, sizeof(kFilterMap) / sizeof(short), 
											preset->Layers[i].FilterParameters.Filt_Type));

		mFilterFreq[i]->SetValue(preset->Layers[i].FilterParameters.Filt_Freq);
		mFilterRes[i]->SetValue(preset->Layers[i].FilterParameters.Filt_Q);

		mLFO1Delay[i]->SetRange(0, 100);
		mLFO1Var[i]->SetRange(-25, 127);

		mLFO1Shape[i]->SetSelection(preset->Layers[i].LFOParameters[0].LFO_Shape);
		mLFO1Rate[i]->SetSelection(preset->Layers[i].LFOParameters[0].LFO_Rate + 25);
		mLFO1Sync[i]->SetSelection(preset->Layers[i].LFOParameters[0].LFO_Sync);
		mLFO1Delay[i]->SetValue(preset->Layers[i].LFOParameters[0].LFO_Delay);
		mLFO1Var[i]->SetValue(preset->Layers[i].LFOParameters[0].LFO_Var);

		mLFO2Delay[i]->SetRange(0, 100);
		mLFO2Var[i]->SetRange(-25, 127);

		mLFO2Shape[i]->SetSelection(preset->Layers[i].LFOParameters[1].LFO_Shape);
		mLFO2Rate[i]->SetSelection(preset->Layers[i].LFOParameters[1].LFO_Rate + 25);
		mLFO2Sync[i]->SetSelection(preset->Layers[i].LFOParameters[1].LFO_Sync);
		mLFO2Delay[i]->SetValue(preset->Layers[i].LFOParameters[1].LFO_Delay);
		mLFO2Var[i]->SetValue(preset->Layers[i].LFOParameters[1].LFO_Var);

		// velocity envelope
		mVelEnvMode[i]->SetSelection(preset->Layers[i].EnvelopeParameters[0].Env_Mode);

		mVelEnvAttack1[i]->SetRange(0, 100);
		mVelEnvAttack2[i]->SetRange(0, 100);
		mVelEnvDecay1[i]->SetRange(0, 100);
		mVelEnvDecay2[i]->SetRange(0, 100);
		mVelEnvRelease1[i]->SetRange(0, 100);
		mVelEnvRelease2[i]->SetRange(0, 100);
		mVelEnvAttack1Rt[i]->SetRange(0, 127);
		mVelEnvAttack2Rt[i]->SetRange(0, 127);
		mVelEnvDecay1Rt[i]->SetRange(0, 127);
		mVelEnvDecay2Rt[i]->SetRange(0, 127);
		mVelEnvAttack1[i]->SetRange(0, 127);
		mVelEnvRelease1Rt[i]->SetRange(0, 127);

		mVelEnvAttack1[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Attack1_Level);
		mVelEnvAttack2[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Attack2_Level);
		mVelEnvDecay1[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Decay1_Level);
		mVelEnvDecay2[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Decay2_Level);
		mVelEnvRelease1[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Release1_Level);
		mVelEnvRelease2[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Release2_Level);
		mVelEnvAttack1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Attack1_Rate);
		mVelEnvAttack2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Attack2_Rate);
		mVelEnvDecay1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Decay1_Rate);
		mVelEnvDecay2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Decay2_Rate);
		mVelEnvRelease1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Release1_Rate);
		mVelEnvRelease2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[0].Env_Release2_Rate);

		EnableVelEnvCtrls(i, preset->Layers[i].EnvelopeParameters[0].Env_Mode != 0);

		// filter envelope
		mFilEnvMode[i]->SetSelection(preset->Layers[i].EnvelopeParameters[1].Env_Mode - 1);
		
		mFilEnvAttack1[i]->SetRange(0, 100);
		mFilEnvAttack2[i]->SetRange(0, 100);
		mFilEnvDecay1[i]->SetRange(0, 100);
		mFilEnvDecay2[i]->SetRange(0, 100);
		mFilEnvRelease1[i]->SetRange(0, 100);
		mFilEnvRelease2[i]->SetRange(0, 100);
		mFilEnvAttack1Rt[i]->SetRange(0, 127);
		mFilEnvAttack2Rt[i]->SetRange(0, 127);
		mFilEnvDecay1Rt[i]->SetRange(0, 127);
		mFilEnvDecay2Rt[i]->SetRange(0, 127);
		mFilEnvAttack1[i]->SetRange(0, 127);
		mFilEnvRelease1Rt[i]->SetRange(0, 127);
		
		mFilEnvAttack1[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Attack1_Level);
		mFilEnvAttack2[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Attack2_Level);
		mFilEnvDecay1[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Decay1_Level);
		mFilEnvDecay2[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Decay2_Level);
		mFilEnvRelease1[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Release1_Level);
		mFilEnvRelease2[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Release2_Level);
		mFilEnvAttack1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Attack1_Rate);
		mFilEnvAttack2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Attack2_Rate);
		mFilEnvDecay1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Decay1_Rate);
		mFilEnvDecay2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Decay2_Rate);
		mFilEnvRelease1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Release1_Rate);
		mFilEnvRelease2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[1].Env_Release2_Rate);

		// auxiliary envelope
		mAuxEnvMode[i]->SetSelection(preset->Layers[i].EnvelopeParameters[2].Env_Mode - 1);
		
		mAuxEnvAttack1[i]->SetRange(0, 100);
		mAuxEnvAttack2[i]->SetRange(0, 100);
		mAuxEnvDecay1[i]->SetRange(0, 100);
		mAuxEnvDecay2[i]->SetRange(0, 100);
		mAuxEnvRelease1[i]->SetRange(0, 100);
		mAuxEnvRelease2[i]->SetRange(0, 100);
		mAuxEnvAttack1Rt[i]->SetRange(0, 127);
		mAuxEnvAttack2Rt[i]->SetRange(0, 127);
		mAuxEnvDecay1Rt[i]->SetRange(0, 127);
		mAuxEnvDecay2Rt[i]->SetRange(0, 127);
		mAuxEnvAttack1[i]->SetRange(0, 127);
		mAuxEnvRelease1Rt[i]->SetRange(0, 127);
		
		mAuxEnvAttack1[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Attack1_Level);
		mAuxEnvAttack2[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Attack2_Level);
		mAuxEnvDecay1[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Decay1_Level);
		mAuxEnvDecay2[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Decay2_Level);
		mAuxEnvRelease1[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Release1_Level);
		mAuxEnvRelease2[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Release2_Level);
		mAuxEnvAttack1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Attack1_Rate);
		mAuxEnvAttack2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Attack2_Rate);
		mAuxEnvDecay1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Decay1_Rate);
		mAuxEnvDecay2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Decay2_Rate);
		mAuxEnvRelease1Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Release1_Rate);
		mAuxEnvRelease2Rt[i]->SetValue(preset->Layers[i].EnvelopeParameters[2].Env_Release2_Rate);

		for(int k = 0; k < PATCHCORDS; k++)
		{
			mPatchcordSources[i][k]->SetSelection(IdxFromMapValue(&kPatchcordSourceMap[0], 
													sizeof(kPatchcordSourceMap) / sizeof(short),
													preset->Layers[i].Patchcords[k].Cord_Source));
			mPatchcordDests[i][k]->SetSelection(IdxFromMapValue(&kPatchcordDestMap[0],
													sizeof(kPatchcordDestMap) / sizeof(short),
													preset->Layers[i].Patchcords[k].Cord_Dest));

			mPatchcordAmts[i][k]->SetRange(-100, 100);
			mPatchcordAmts[i][k]->SetValue(preset->Layers[i].Patchcords[k].Cord_Amount);
		}

		mChorus[i]->SetRange(0, 100);
		mChorus[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_DETune);

		mChorusWidth[i]->SetRange(0, 100);
		mChorusWidth[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_DETune_Width);

		mStart[i]->SetRange(-25, 127);
		mStart[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Start_Offset);

		mDelay[i]->SetRange(0, 127);
		mDelay[i]->SetValue(preset->Layers[i].LayerGeneralParameters.Layer_Start_Delay);

		mNonTranspose[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Non_Transpose);
		mSoloMode[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Solo);
		mAssignGroup[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Group);
		mGlide[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Glide_Rate);
		mGlideCurve[i]->SetSelection(preset->Layers[i].LayerGeneralParameters.Layer_Glide_Curve);
	}

	for(int k = 0; k < PPATCHCORDS; k++)
	{
		mPPatchcordSources[k]->SetSelection(IdxFromMapValue(&kPPatchcordSourceMap[0], 
			sizeof(kPPatchcordSourceMap) / sizeof(short),
			preset->GeneralParameters.Preset_Patchcords[k].Source));
		mPPatchcordDests[k]->SetSelection(IdxFromMapValue(&kPPatchcordDestMap[0],
			sizeof(kPPatchcordDestMap) / sizeof(short),
			preset->GeneralParameters.Preset_Patchcords[k].Dest));
		
		mPPatchcordAmts[k]->SetRange(-100, 100);
		mPPatchcordAmts[k]->SetValue(preset->GeneralParameters.Preset_Patchcords[k].Amount);
	}


	// FX A
	mFXAAlg->SetSelection(preset->EffectsParameters.FX_A_Algorithm);

	mFXADecay->SetRange(0, 90);
	mFXADecay->SetValue(preset->EffectsParameters.FX_A_Decay);

	mFXAHFDamp->SetRange(0, 127);
	mFXAHFDamp->SetValue(preset->EffectsParameters.FX_A_HFDamp);

	mFXBFXA->SetRange(0, 127);
	mFXBFXA->SetValue(preset->EffectsParameters.FX_AB);
	
	mFXASendAmt1->SetRange(0, 100);
	mFXASendAmt1->SetValue(preset->EffectsParameters.FX_A_Mix_Main);

	mFXASendAmt2->SetRange(0, 100);
	mFXASendAmt2->SetValue(preset->EffectsParameters.FX_A_Mix_Sub1);

	mFXASendAmt3->SetRange(0, 100);
	mFXASendAmt3->SetValue(preset->EffectsParameters.FX_A_Mix_Sub2);

	mFXASendAmt4->SetRange(0, 100);
	mFXASendAmt4->SetValue(preset->EffectsParameters.FX_B_Mix_Sub3);

	// FX B
	mFXBAlg->SetSelection(preset->EffectsParameters.FX_B_Algorithm);

	mFXBFeedbk->SetRange(0, 127);
	mFXBFeedbk->SetValue(preset->EffectsParameters.FX_B_Feedback);

	mFXBLFORate->SetRange(0, 127);
	mFXBLFORate->SetValue(preset->EffectsParameters.FX_B_LFO_Rate);

	mFXBDelay->SetSelection(preset->EffectsParameters.FX_B_Delay + 12);

	mFXBSendAmt1->SetRange(0, 100);
	mFXBSendAmt1->SetValue(preset->EffectsParameters.FX_B_Mix_Main);

	mFXBSendAmt2->SetRange(0, 100);
	mFXBSendAmt2->SetValue(preset->EffectsParameters.FX_B_Mix_Sub1);

	mFXBSendAmt3->SetRange(0, 100);
	mFXBSendAmt3->SetValue(preset->EffectsParameters.FX_B_Mix_Sub2);

	mFXBSendAmt4->SetRange(0, 100);
	mFXBSendAmt4->SetValue(preset->EffectsParameters.FX_B_Mix_Sub4);

	mRTControllerA->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_A));
	mRTControllerB->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_B));
	mRTControllerC->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_C));
	mRTControllerD->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_D));
	mRTControllerE->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_E));
	mRTControllerF->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_F));
	mRTControllerG->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_G));
	mRTControllerH->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_H));
	mRTControllerI->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_I));
	mRTControllerJ->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_J));
	mRTControllerK->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_K));
	mRTControllerL->SetValue(wxString::Format("%i", preset->GeneralParameters.Preset_Ctrl_L));

	wxString prefix = mPresetNameText1->GetValue();
	if(prefix.Length() < 3)
		prefix.Pad(3 - prefix.Length(), ' ');
	wxString presetName = wxString::Format("%s:%s", prefix, mPresetNameText2->GetValue());
	presetName.Trim();
	wxGetApp().mMainFrame->mLeftFrame->SetSelectedName(presetName);

	if(preset->IsEditable())
	{
		strcpy(&wxGetApp().Proteus.mPresetNames[mCurPreset->ID][0], presetName);
		
		wxString fileName;
		wxGetApp().GetAppPath(fileName);
		fileName.append("\\UserPresets.txt");
		wxGetApp().Proteus.SaveUserPresetNames(fileName);

		mCurLayer = mLayerTabs->GetSelection();
		wxGetApp().Proteus.SetParameter(LAYER_SELECT, mCurLayer);
	}

	EnableControls(preset->IsEditable());

	mInitializing = false;

}

void EditorPanel::EnableVelEnvCtrls(int layer, bool enable)
{
	mVelEnvAttack1[layer]->Enable(enable);
	mVelEnvAttack2[layer]->Enable(enable);
	mVelEnvDecay1[layer]->Enable(enable);
	mVelEnvDecay2[layer]->Enable(enable);
	mVelEnvRelease1[layer]->Enable(enable);
	mVelEnvRelease2[layer]->Enable(enable);
	mVelEnvAttack1Rt[layer]->Enable(enable);
	mVelEnvAttack2Rt[layer]->Enable(enable);
	mVelEnvDecay1Rt[layer]->Enable(enable);
	mVelEnvDecay2Rt[layer]->Enable(enable);
	mVelEnvRelease1Rt[layer]->Enable(enable);
	mVelEnvRelease2Rt[layer]->Enable(enable);
}

void EditorPanel::EnableControls(bool enable)
{
	mPresetNameText1->Enable(enable);
	mPresetNameText2->Enable(enable);

	for(int i = 0; i < 4; i++)
	{
		mInstrument[i]->Enable(enable);
		mROM[i]->Enable(enable);
		mVolume[i]->Enable(enable);
		mPan[i]->Enable(enable);

		mKeyLow[i]->Enable(enable);
		mKeyLowFade[i]->Enable(enable);
		mKeyHigh[i]->Enable(enable);
		mKeyHighFade[i]->Enable(enable);

		mVelocityLow[i]->Enable(enable);
		mVelocityLowFade[i]->Enable(enable);
		mVelocityHigh[i]->Enable(enable);
		mVelocityHighFade[i]->Enable(enable);

		mRTLow[i]->Enable(enable);
		mRTLowFade[i]->Enable(enable);
		mRTHigh[i]->Enable(enable);
		mRTHighFade[i]->Enable(enable);

		mTranspose[i]->Enable(enable);
		mTuningCoarse[i]->Enable(enable);
		mTuningFine[i]->Enable(enable);

		mFilter[i]->Enable(enable);
		mFilterFreq[i]->Enable(enable);
		mFilterRes[i]->Enable(enable);

		mLFO1Shape[i]->Enable(enable);
		mLFO1Rate[i]->Enable(enable);
		mLFO1Sync[i]->Enable(enable);
		mLFO1Delay[i]->Enable(enable);
		mLFO1Var[i]->Enable(enable);

		mLFO2Shape[i]->Enable(enable);
		mLFO2Rate[i]->Enable(enable);
		mLFO2Sync[i]->Enable(enable);
		mLFO2Delay[i]->Enable(enable);
		mLFO2Var[i]->Enable(enable);

		mVelEnvMode[i]->Enable(enable);
		mVelEnvAttack1[i]->Enable(enable);
		mVelEnvAttack2[i]->Enable(enable);
		mVelEnvDecay1[i]->Enable(enable);
		mVelEnvDecay2[i]->Enable(enable);
		mVelEnvRelease1[i]->Enable(enable);
		mVelEnvRelease2[i]->Enable(enable);
		mVelEnvAttack1Rt[i]->Enable(enable);
		mVelEnvAttack2Rt[i]->Enable(enable);
		mVelEnvDecay1Rt[i]->Enable(enable);
		mVelEnvDecay2Rt[i]->Enable(enable);
		mVelEnvRelease1Rt[i]->Enable(enable);
		mVelEnvRelease2Rt[i]->Enable(enable);

		mFilEnvMode[i]->Enable(enable);
		mFilEnvAttack1[i]->Enable(enable);
		mFilEnvAttack2[i]->Enable(enable);
		mFilEnvDecay1[i]->Enable(enable);
		mFilEnvDecay2[i]->Enable(enable);
		mFilEnvRelease1[i]->Enable(enable);
		mFilEnvRelease2[i]->Enable(enable);
		mFilEnvAttack1Rt[i]->Enable(enable);
		mFilEnvAttack2Rt[i]->Enable(enable);
		mFilEnvDecay1Rt[i]->Enable(enable);
		mFilEnvDecay2Rt[i]->Enable(enable);
		mFilEnvRelease1Rt[i]->Enable(enable);
		mFilEnvRelease2Rt[i]->Enable(enable);

		mAuxEnvMode[i]->Enable(enable);
		mAuxEnvAttack1[i]->Enable(enable);
		mAuxEnvAttack2[i]->Enable(enable);
		mAuxEnvDecay1[i]->Enable(enable);
		mAuxEnvDecay2[i]->Enable(enable);
		mAuxEnvRelease1[i]->Enable(enable);
		mAuxEnvRelease2[i]->Enable(enable);
		mAuxEnvAttack1Rt[i]->Enable(enable);
		mAuxEnvAttack2Rt[i]->Enable(enable);
		mAuxEnvDecay1Rt[i]->Enable(enable);
		mAuxEnvDecay2Rt[i]->Enable(enable);
		mAuxEnvRelease1Rt[i]->Enable(enable);
		mAuxEnvRelease2Rt[i]->Enable(enable);

		int j = 0;
		for(j = 0; j < PATCHCORDS; j++)
		{
			mPatchcordSources[i][j]->Enable(enable);
			mPatchcordDests[i][j]->Enable(enable);
			mPatchcordAmts[i][j]->Enable(enable);
		}

		mChorus[i]->Enable(enable);
		mChorusWidth[i]->Enable(enable);
		mStart[i]->Enable(enable);
		mDelay[i]->Enable(enable);
		mNonTranspose[i]->Enable(enable);
		mSoloMode[i]->Enable(enable);
		mAssignGroup[i]->Enable(enable);
		mGlide[i]->Enable(enable);
		mGlideCurve[i]->Enable(enable);
	}

	for(int k = 0; k < PPATCHCORDS; k++)
	{
		mPPatchcordSources[k]->Enable(enable);
		mPPatchcordDests[k]->Enable(enable);
		mPPatchcordAmts[k]->Enable(enable);
	}

	mFXAAlg->Enable(enable);
	mFXADecay->Enable(enable);
	mFXAHFDamp->Enable(enable);
	mFXBFXA->Enable(enable);
	mFXASendAmt1->Enable(enable);
	mFXASendAmt2->Enable(enable);
	mFXASendAmt3->Enable(enable);
	mFXASendAmt4->Enable(enable);

	mFXBAlg->Enable(enable);
	mFXBFeedbk->Enable(enable);
	mFXBLFORate->Enable(enable);
	mFXBDelay->Enable(enable);
	mFXBSendAmt1->Enable(enable);
	mFXBSendAmt2->Enable(enable);
	mFXBSendAmt3->Enable(enable);
	mFXBSendAmt4->Enable(enable);

	mRTControllerA->Enable(enable);
	mRTControllerB->Enable(enable);
	mRTControllerC->Enable(enable);
	mRTControllerD->Enable(enable);
	mRTControllerE->Enable(enable);
	mRTControllerF->Enable(enable);
	mRTControllerG->Enable(enable);
	mRTControllerH->Enable(enable);
	mRTControllerI->Enable(enable);
	mRTControllerJ->Enable(enable);
	mRTControllerK->Enable(enable);
	mRTControllerL->Enable(enable);
}

void EditorPanel::OnEvent(wxCommandEvent& event)
{
	if(!mCurPreset)
		return;

	if(mInitializing)
		return;

	switch(event.GetId())
	{
		case ePresetNameText1:
		case ePresetNameText2:
		{
			wxString presetName = wxString::Format("%s:%s", mPresetNameText1->GetValue(), 
				mPresetNameText2->GetValue());
			
			wxGetApp().RenamePreset(mCurPreset, presetName);
			break;
		}

		case eSaveAs:
		{
			CSaveAsDlg dlg(wxGetApp().mMainFrame, &wxGetApp());
			dlg.ShowModal();
			break;
		}

		case eInstrument:
			wxGetApp().Proteus.SetParameter(LAYER_INSTRUMENT, mInstrument[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Instrument = mInstrument[mCurLayer]->GetSelection();
			break;

		case eInstrumentROM:
		{
			short rom_id = wxGetApp().Proteus.mSimms[mROM[mCurLayer]->GetSelection()]->mSimmID;
			wxGetApp().Proteus.SetParameter(LAYER_INST_ROM_ID, rom_id);
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Inst_ROM_ID = rom_id;
			PopulateInstruments(mCurLayer, rom_id);
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Instrument = 0;
			mInstrument[mCurLayer]->SetSelection(0);
			break;
		}
			
		case eVolume:
			wxGetApp().Proteus.SetParameter(LAYER_VOLUME, mVolume[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Volulme = mVolume[mCurLayer]->GetValue();
			break;

		case ePan:
			wxGetApp().Proteus.SetParameter(LAYER_PAN, mPan[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Pan = mPan[mCurLayer]->GetValue();
			break;

		case eKeyLow:
			wxGetApp().Proteus.SetParameter(LAYER_KEY_LOW, mKeyLow[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Key_Low = mKeyLow[mCurLayer]->GetSelection();
			break;

		case eKeyHigh:
			wxGetApp().Proteus.SetParameter(LAYER_KEY_HIGH, mKeyHigh[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Key_High = mKeyHigh[mCurLayer]->GetSelection();
			break;

		case eFilter:
			wxGetApp().Proteus.SetParameter(LAYER_FILT_TYPE, kFilterMap[mFilter[mCurLayer]->GetSelection()], false);
			mCurPreset->Layers[mCurLayer].FilterParameters.Filt_Type = mFilter[mCurLayer]->GetSelection();
			break;

		case eLFO1Shape:
			wxGetApp().Proteus.SetParameter(LAYER_LFO1_SHAPE, mLFO1Shape[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LFOParameters[0].LFO_Shape = mLFO1Shape[mCurLayer]->GetSelection();
			break;

		case eLFO1Rate:
			wxGetApp().Proteus.SetParameter(LAYER_LFO1_RATE, mLFO1Rate[mCurLayer]->GetSelection() - 25);
			mCurPreset->Layers[mCurLayer].LFOParameters[0].LFO_Rate = mLFO1Rate[mCurLayer]->GetSelection();
			break;

		case eLFO1Sync:
			wxGetApp().Proteus.SetParameter(LAYER_LFO1_SYNC, mLFO1Sync[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LFOParameters[0].LFO_Sync = mLFO1Sync[mCurLayer]->GetSelection();
			break;

		case eLFO2Shape:
			wxGetApp().Proteus.SetParameter(LAYER_LFO2_SHAPE, mLFO2Shape[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LFOParameters[1].LFO_Shape = mLFO2Shape[mCurLayer]->GetSelection();
			break;

		case eLFO2Rate:
			wxGetApp().Proteus.SetParameter(LAYER_LFO2_RATE, mLFO2Rate[mCurLayer]->GetSelection() - 25);
			mCurPreset->Layers[mCurLayer].LFOParameters[1].LFO_Rate = mLFO2Rate[mCurLayer]->GetSelection();
			break;

		case eLFO2Sync:
			wxGetApp().Proteus.SetParameter(LAYER_LFO2_SYNC, mLFO2Sync[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LFOParameters[1].LFO_Sync = mLFO2Sync[mCurLayer]->GetSelection();
			break;

		// velocity envelope
		case eVelEnvMode:
			wxGetApp().Proteus.SetParameter(LAYER_VOL_ENV_MODE, mVelEnvMode[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Mode = mVelEnvMode[mCurLayer]->GetSelection();

			EnableVelEnvCtrls(mCurLayer, mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Mode != 0);
			break;

		// filter envelope
		case eFilEnvMode:
			wxGetApp().Proteus.SetParameter(LAYER_FILT_ENV_MODE, mFilEnvMode[mCurLayer]->GetSelection() + 1);
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Mode = mFilEnvMode[mCurLayer]->GetSelection() + 1;
			break;

		// auxiliary envelope
		case eAuxEnvMode:
			wxGetApp().Proteus.SetParameter(LAYER_AUX_ENV_MODE, mAuxEnvMode[mCurLayer]->GetSelection() + 1);
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Mode = mAuxEnvMode[mCurLayer]->GetSelection() + 1;
			break;

		// general
		case eNonTranspose:
			wxGetApp().Proteus.SetParameter(LAYER_NON_TRANSPOSE, mNonTranspose[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Non_Transpose = mNonTranspose[mCurLayer]->GetSelection();
			break;

		case eSoloMode:
			wxGetApp().Proteus.SetParameter(LAYER_SOLO, mSoloMode[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Solo = mSoloMode[mCurLayer]->GetSelection();
			break;

		case eAssignGroup:
			wxGetApp().Proteus.SetParameter(LAYER_GROUP, mAssignGroup[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Group = mAssignGroup[mCurLayer]->GetSelection();
			break;

		case eGlide:
			wxGetApp().Proteus.SetParameter(LAYER_GLIDE_RATE, mGlide[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Glide_Rate = mAssignGroup[mCurLayer]->GetSelection();
			break;

		case eGlideCurve:
			wxGetApp().Proteus.SetParameter(LAYER_GLIDE_CURVE, mGlideCurve[mCurLayer]->GetSelection());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Glide_Curve = mGlideCurve[mCurLayer]->GetSelection();
			break;

		// FX
		case eFXAAlgId:
			wxGetApp().Proteus.SetParameter(PRESET_FX_A_ALGORITHM, mFXAAlg->GetSelection());
			mCurPreset->EffectsParameters.FX_A_Algorithm = mFXAAlg->GetSelection();
			break;
	
		case eFXBAlgId:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_ALGORITHM, mFXBAlg->GetSelection());
			mCurPreset->EffectsParameters.FX_B_Algorithm = mFXBAlg->GetSelection();
			break;

		case eFXBDelay:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_DELAY, mFXBDelay->GetSelection() - 12);
			mCurPreset->EffectsParameters.FX_B_Delay = mFXBDelay->GetSelection() - 12;
			break;

		// rt controller save
		case eRTControllerSave:
		{
			SGeneralParameters params;
			unsigned long val = 0;

			mRTControllerA->GetValue().ToULong(&val);
			params.Preset_Ctrl_A = (short) val;

			mRTControllerB->GetValue().ToULong(&val);
			params.Preset_Ctrl_B = (short) val;

			mRTControllerC->GetValue().ToULong(&val);
			params.Preset_Ctrl_C = (short) val;

			mRTControllerD->GetValue().ToULong(&val);
			params.Preset_Ctrl_D = (short) val;

			mRTControllerE->GetValue().ToULong(&val);
			params.Preset_Ctrl_E = (short) val;

			mRTControllerF->GetValue().ToULong(&val);
			params.Preset_Ctrl_F = (short) val;

			mRTControllerG->GetValue().ToULong(&val);
			params.Preset_Ctrl_G = (short) val;

			mRTControllerH->GetValue().ToULong(&val);
			params.Preset_Ctrl_H = (short) val;

			mRTControllerI->GetValue().ToULong(&val);
			params.Preset_Ctrl_I = (short) val;

			mRTControllerJ->GetValue().ToULong(&val);
			params.Preset_Ctrl_J = (short) val;

			mRTControllerK->GetValue().ToULong(&val);
			params.Preset_Ctrl_K = (short) val;

			mRTControllerL->GetValue().ToULong(&val);
			params.Preset_Ctrl_L = (short) val;

			wxGetApp().Proteus.SaveInitialControllers(&params);
			break;
		}
	}

	if(event.GetId() >= ePatchcordSources && event.GetId() < ePatchcordDests)
	{
		int index = event.GetId() - ePatchcordSources;
		int cmdid = LAYER_CORD0_SRC + index * 0x300;
		short newval = kPatchcordSourceMap[mPatchcordSources[mCurLayer][index]->GetSelection()];

		wxGetApp().Proteus.SetParameter(cmdid, newval);
		mCurPreset->Layers[mCurLayer].Patchcords[index].Cord_Source = newval;
	}

	if(event.GetId() >= ePatchcordDests && event.GetId() < ePatchcordAmts)
	{
		int index = event.GetId() - ePatchcordDests;
		int cmdid = LAYER_CORD0_DST + index * 0x300;
		short newval = kPatchcordDestMap[mPatchcordDests[mCurLayer][index]->GetSelection()];
		
		wxGetApp().Proteus.SetParameter(cmdid, newval);
		mCurPreset->Layers[mCurLayer].Patchcords[index].Cord_Dest = newval;
	}

	if(event.GetId() >= ePPatchcordSources && event.GetId() < ePPatchcordDests)
	{
		int index = event.GetId() - ePPatchcordSources;
		int cmdid = PRESET_CORD_0_SOURCE + index * 0x300;
		short newval = kPPatchcordSourceMap[mPPatchcordSources[index]->GetSelection()];
		
		wxGetApp().Proteus.SetParameter(cmdid, newval);
		mCurPreset->GeneralParameters.Preset_Patchcords[index].Source = newval;
	}
	
	if(event.GetId() >= ePPatchcordDests && event.GetId() < ePPatchcordAmts)
	{
		int index = event.GetId() - ePPatchcordDests;
		int cmdid = PRESET_CORD_0_DEST + index * 0x300;
		short newval = kPPatchcordDestMap[mPPatchcordDests[index]->GetSelection()];
		
		wxGetApp().Proteus.SetParameter(cmdid, newval);
		mCurPreset->GeneralParameters.Preset_Patchcords[index].Dest = newval;
	}
}

void EditorPanel::OnSpinEvent(wxSpinEvent& event)
{
	if(!mCurPreset)
		return;

	if(mInitializing)
		return;
	
	switch(event.GetId())
	{
		// crossfade
		case eKeyLowFade:
			wxGetApp().Proteus.SetParameter(LAYER_KEY_LOWFADE, mKeyLowFade[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Key_LowFade = mKeyLowFade[mCurLayer]->GetValue();
			break;

		case eKeyHighFade:
			wxGetApp().Proteus.SetParameter(LAYER_KEY_HIGHFADE, mKeyHighFade[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Key_HighFade = mKeyHighFade[mCurLayer]->GetValue();
			break;

		case eVelocityLow:
			wxGetApp().Proteus.SetParameter(LAYER_VEL_LOW, mVelocityLow[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Vel_Low = mVelocityLow[mCurLayer]->GetValue();
			break;

		case eVelocityLowFade:
			wxGetApp().Proteus.SetParameter(LAYER_VEL_LOWFADE, mVelocityLowFade[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Vel_LowFade = mVelocityLowFade[mCurLayer]->GetValue();
			break;

		case eVelocityHigh:
			wxGetApp().Proteus.SetParameter(LAYER_VEL_HIGH, mVelocityHigh[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Vel_High = mVelocityHigh[mCurLayer]->GetValue();
			break;

		case eVelocityHighFade:
			wxGetApp().Proteus.SetParameter(LAYER_VEL_HIGHFADE, mVelocityHighFade[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Vel_HighFade = mVelocityHighFade[mCurLayer]->GetValue();
			break;

		case eRTLow:
			wxGetApp().Proteus.SetParameter(LAYER_RT_LOW, mRTLow[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_RT_Low = mRTLow[mCurLayer]->GetValue();
			break;

		case eRTLowFade:
			wxGetApp().Proteus.SetParameter(LAYER_RT_LOWFADE, mRTLowFade[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_RT_LowFade = mRTLowFade[mCurLayer]->GetValue();
			break;

		case eRTHigh:
			wxGetApp().Proteus.SetParameter(LAYER_RT_HIGH, mRTHigh[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_RT_High = mRTHigh[mCurLayer]->GetValue();
			break;

		case eRTHighFade:
			wxGetApp().Proteus.SetParameter(LAYER_RT_HIGHFADE, mRTHighFade[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_RT_HighFade = mRTHighFade[mCurLayer]->GetValue();
			break;

		case eTranspose:
			wxGetApp().Proteus.SetParameter(LAYER_TRANSPOSE, mTranspose[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Transpose = mTranspose[mCurLayer]->GetValue();
			break;

		case eTuningCoarse:
			wxGetApp().Proteus.SetParameter(LAYER_CTUNE, mTuningCoarse[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_CTune = mTuningCoarse[mCurLayer]->GetValue();
			break;

		case eTuningFine:
			wxGetApp().Proteus.SetParameter(LAYER_FTUNE, mTuningFine[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_FTune = mTuningFine[mCurLayer]->GetValue();
			break;

		// filter
		case eFilterFreq:
			wxGetApp().Proteus.SetParameter(LAYER_FILT_FREQ, mFilterFreq[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].FilterParameters.Filt_Freq = mFilterFreq[mCurLayer]->GetValue();
			break;

		case eFilterRes:
			wxGetApp().Proteus.SetParameter(LAYER_FILT_Q, mFilterRes[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].FilterParameters.Filt_Q = mFilterRes[mCurLayer]->GetValue();
			break;

		// LFOs
		case eLFO1Delay:
			wxGetApp().Proteus.SetParameter(LAYER_LFO1_DELAY, mLFO1Delay[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LFOParameters[0].LFO_Delay = mLFO1Delay[mCurLayer]->GetValue();
			break;

		case eLFO1Var:
			wxGetApp().Proteus.SetParameter(LAYER_LFO1_VAR, mLFO1Var[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LFOParameters[0].LFO_Var = mLFO1Var[mCurLayer]->GetValue();
			break;

		case eLFO2Delay:
			wxGetApp().Proteus.SetParameter(LAYER_LFO2_DELAY, mLFO2Delay[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LFOParameters[1].LFO_Delay = mLFO2Delay[mCurLayer]->GetValue();
			break;

		case eLFO2Var:
			wxGetApp().Proteus.SetParameter(LAYER_LFO2_VAR, mLFO2Var[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LFOParameters[1].LFO_Var = mLFO2Var[mCurLayer]->GetValue();
			break;

		// velocity envelope
		case eVelEnvAttack1:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_ATK1_LVL, mVelEnvAttack1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Attack1_Level = mVelEnvAttack1[mCurLayer]->GetValue();
			break;

		case eVelEnvAttack2:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_ATK2_LVL, mVelEnvAttack2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Attack2_Level = mVelEnvAttack2[mCurLayer]->GetValue();
			break;

		case eVelEnvDecay1:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_DCY1_LVL, mVelEnvDecay1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Decay1_Level = mVelEnvDecay1[mCurLayer]->GetValue();
			break;

		case eVelEnvDecay2:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_DCY2_LVL, mVelEnvDecay2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Decay2_Level = mVelEnvDecay2[mCurLayer]->GetValue();
			break;

		case eVelEnvRelease1:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_RLS1_LVL, mVelEnvRelease1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Release1_Level = mVelEnvRelease1[mCurLayer]->GetValue();
			break;

		case eVelEnvRelease2:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_RLS2_LVL, mVelEnvRelease2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Release2_Level = mVelEnvRelease2[mCurLayer]->GetValue();
			break;

		case eVelEnvAttack1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_ATK1_RATE, mVelEnvAttack1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Attack1_Rate = mVelEnvAttack1Rt[mCurLayer]->GetValue();
			break;

		case eVelEnvAttack2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_ATK2_RATE, mVelEnvAttack2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Attack2_Rate = mVelEnvAttack2Rt[mCurLayer]->GetValue();
			break;

		case eVelEnvDecay1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_DCY1_RATE, mVelEnvDecay1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Decay1_Rate = mVelEnvDecay1Rt[mCurLayer]->GetValue();
			break;

		case eVelEnvDecay2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_DCY2_RATE, mVelEnvDecay2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Decay2_Rate = mVelEnvDecay2Rt[mCurLayer]->GetValue();
			break;

		case eVelEnvRelease1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_RLS1_RATE, mVelEnvRelease1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Release1_Rate = mVelEnvRelease1Rt[mCurLayer]->GetValue();
			break;

		case eVelEnvRelease2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_VENV_RLS2_RATE, mVelEnvRelease2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[0].Env_Release2_Rate = mVelEnvRelease2Rt[mCurLayer]->GetValue();
			break;

		// filter envelope
		case eFilEnvAttack1:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_ATK1_LVL, mFilEnvAttack1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Attack1_Level = mFilEnvAttack1[mCurLayer]->GetValue();
			break;
			
		case eFilEnvAttack2:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_ATK2_LVL, mFilEnvAttack2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Attack2_Level = mFilEnvAttack2[mCurLayer]->GetValue();
			break;
			
		case eFilEnvDecay1:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_DCY1_LVL, mFilEnvDecay1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Decay1_Level = mFilEnvDecay1[mCurLayer]->GetValue();
			break;
			
		case eFilEnvDecay2:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_DCY2_LVL, mFilEnvDecay2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Decay2_Level = mFilEnvDecay2[mCurLayer]->GetValue();
			break;
			
		case eFilEnvRelease1:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_RLS1_LVL, mFilEnvRelease1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Release1_Level = mFilEnvRelease1[mCurLayer]->GetValue();
			break;
			
		case eFilEnvRelease2:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_RLS2_LVL, mFilEnvRelease2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Release2_Level = mFilEnvRelease2[mCurLayer]->GetValue();
			break;
			
		case eFilEnvAttack1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_ATK1_RATE, mFilEnvAttack1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Attack1_Rate = mFilEnvAttack1Rt[mCurLayer]->GetValue();
			break;
			
		case eFilEnvAttack2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_ATK2_RATE, mFilEnvAttack2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Attack2_Rate = mFilEnvAttack2Rt[mCurLayer]->GetValue();
			break;
			
		case eFilEnvDecay1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_DCY1_RATE, mFilEnvDecay1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Decay1_Rate = mFilEnvDecay1Rt[mCurLayer]->GetValue();
			break;
			
		case eFilEnvDecay2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_DCY2_RATE, mFilEnvDecay2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Decay2_Rate = mFilEnvDecay2Rt[mCurLayer]->GetValue();
			break;
			
		case eFilEnvRelease1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_RLS1_RATE, mFilEnvRelease1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Release1_Rate = mFilEnvRelease1Rt[mCurLayer]->GetValue();
			break;
			
		case eFilEnvRelease2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_FENV_RLS2_RATE, mFilEnvRelease2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[1].Env_Release2_Rate = mFilEnvRelease2Rt[mCurLayer]->GetValue();
			break;

		// auxiliary envelope
		case eAuxEnvAttack1:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_ATK1_LVL, mAuxEnvAttack1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Attack1_Level = mAuxEnvAttack1[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvAttack2:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_ATK2_LVL, mAuxEnvAttack2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Attack2_Level = mAuxEnvAttack2[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvDecay1:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_DCY1_LVL, mAuxEnvDecay1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Decay1_Level = mAuxEnvDecay1[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvDecay2:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_DCY2_LVL, mAuxEnvDecay2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Decay2_Level = mAuxEnvDecay2[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvRelease1:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_RLS1_LVL, mAuxEnvRelease1[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Release1_Level = mAuxEnvRelease1[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvRelease2:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_RLS2_LVL, mAuxEnvRelease2[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Release2_Level = mAuxEnvRelease2[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvAttack1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_ATK1_RATE, mAuxEnvAttack1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Attack1_Rate = mAuxEnvAttack1Rt[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvAttack2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_ATK2_RATE, mAuxEnvAttack2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Attack2_Rate = mAuxEnvAttack2Rt[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvDecay1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_DCY1_RATE, mAuxEnvDecay1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Decay1_Rate = mAuxEnvDecay1Rt[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvDecay2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_DCY2_RATE, mAuxEnvDecay2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Decay2_Rate = mAuxEnvDecay2Rt[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvRelease1Rt:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_RLS1_RATE, mAuxEnvRelease1Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Release1_Rate = mAuxEnvRelease1Rt[mCurLayer]->GetValue();
			break;
			
		case eAuxEnvRelease2Rt:
			wxGetApp().Proteus.SetParameter(LAYER_AENV_RLS2_RATE, mAuxEnvRelease2Rt[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].EnvelopeParameters[2].Env_Release2_Rate = mAuxEnvRelease2Rt[mCurLayer]->GetValue();
			break;



		// general
		case eChorus:
			wxGetApp().Proteus.SetParameter(LAYER_DBL_DETUNE, mChorus[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_DETune = mChorus[mCurLayer]->GetValue();
			break;

		case eChorusWidth:
			wxGetApp().Proteus.SetParameter(LAYER_DBL_DETUNE_WIDTH, mChorusWidth[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_DETune_Width = mChorusWidth[mCurLayer]->GetValue();
			break;

		case eStart:
			wxGetApp().Proteus.SetParameter(LAYER_START_OFFSET, mStart[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Start_Offset = mStart[mCurLayer]->GetValue();
			break;

		case eDelay:
			wxGetApp().Proteus.SetParameter(LAYER_START_DELAY, mDelay[mCurLayer]->GetValue());
			mCurPreset->Layers[mCurLayer].LayerGeneralParameters.Layer_Start_Delay = mDelay[mCurLayer]->GetValue();
			break;

		// FX
		case eFXADecay:
			wxGetApp().Proteus.SetParameter(PRESET_FX_A_DECAY, mFXADecay->GetValue());
			mCurPreset->EffectsParameters.FX_A_Decay = mFXADecay->GetValue();
			break;

		case eFXAHFDamp:
			wxGetApp().Proteus.SetParameter(PRESET_FX_A_HFDAMP, mFXAHFDamp->GetValue());
			mCurPreset->EffectsParameters.FX_A_HFDamp = mFXAHFDamp->GetValue();
			break;

		case eFXBFXA:
			wxGetApp().Proteus.SetParameter(PRESET_FX_AB, mFXBFXA->GetValue());
			mCurPreset->EffectsParameters.FX_AB = mFXBFXA->GetValue();
			break;

		case eFXASendAmt1:
			wxGetApp().Proteus.SetParameter(PRESET_FX_A_MIX_MAIN, mFXASendAmt1->GetValue());
			mCurPreset->EffectsParameters.FX_A_Mix_Main = mFXASendAmt1->GetValue();
			break;

		case eFXASendAmt2:
			wxGetApp().Proteus.SetParameter(PRESET_FX_A_MIX_SUB1, mFXASendAmt2->GetValue());
			mCurPreset->EffectsParameters.FX_A_Mix_Sub1 = mFXASendAmt2->GetValue();
			break;

		case eFXASendAmt3:
			wxGetApp().Proteus.SetParameter(PRESET_FX_A_MIX_SUB2, mFXASendAmt3->GetValue());
			mCurPreset->EffectsParameters.FX_A_Mix_Sub2 = mFXASendAmt3->GetValue();
			break;

		case eFXASendAmt4:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_MIX_SUB3, mFXASendAmt4->GetValue());
			mCurPreset->EffectsParameters.FX_B_Mix_Sub3 = mFXASendAmt4->GetValue();
			break;

		case eFXBFeedbk:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_FEEDBACK, mFXBFeedbk->GetValue());
			mCurPreset->EffectsParameters.FX_B_Feedback = mFXBFeedbk->GetValue();
			break;
			
		case eFXBLFORate:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_LFO_RATE, mFXBLFORate->GetValue());
			mCurPreset->EffectsParameters.FX_B_LFO_Rate = mFXBLFORate->GetValue();
			break;

		case eFXBSendAmt1:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_MIX_MAIN, mFXBSendAmt1->GetValue());
			mCurPreset->EffectsParameters.FX_B_Mix_Main = mFXBSendAmt1->GetValue();
			break;

		case eFXBSendAmt2:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_MIX_SUB1, mFXBSendAmt2->GetValue());
			mCurPreset->EffectsParameters.FX_B_Mix_Sub1 = mFXBSendAmt2->GetValue();
			break;

		case eFXBSendAmt3:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_MIX_SUB2, mFXBSendAmt3->GetValue());
			mCurPreset->EffectsParameters.FX_B_Mix_Sub2 = mFXBSendAmt3->GetValue();
			break;

		case eFXBSendAmt4:
			wxGetApp().Proteus.SetParameter(PRESET_FX_B_MIX_SUB4, mFXBSendAmt4->GetValue());
			mCurPreset->EffectsParameters.FX_B_Mix_Sub4 = mFXBSendAmt4->GetValue();
			break;

	}

	if(event.GetId() >= ePatchcordAmts && event.GetId() < ePPatchcordSources)
	{
		int index = event.GetId() - ePatchcordAmts;
		int cmdid = LAYER_CORD0_AMT + index * 0x300;
		short newval = mPatchcordAmts[mCurLayer][index]->GetValue();
		
		wxGetApp().Proteus.SetParameter(cmdid, newval);
		mCurPreset->Layers[mCurLayer].Patchcords[index].Cord_Amount = newval;
	}

	if(event.GetId() >= ePPatchcordAmts && event.GetId() < ePatchcordsEnd)
	{
		int index = event.GetId() - ePPatchcordAmts;
		int cmdid = PRESET_CORD_0_AMOUNT + index * 0x300;
		short newval = mPPatchcordAmts[index]->GetValue();
		
		wxGetApp().Proteus.SetParameter(cmdid, newval);
		mCurPreset->GeneralParameters.Preset_Patchcords[index].Amount = newval;
	}
}

void EditorPanel::OnNotebookEvent(wxNotebookEvent& event)
{
	if(!mCurPreset)
		return;

	if(mInitializing)
		return;

	mCurLayer = mLayerTabs->GetSelection();
	wxGetApp().Proteus.SetParameter(LAYER_SELECT, mCurLayer);
}

void EditorPanel::OnLeftDown(wxMouseEvent& event)
{
	SetFocus();
	event.Skip();
}


BEGIN_EVENT_TABLE(EditorPanel, wxPanel)
	EVT_TEXT	(ePresetNameText1,	OnEvent)
	EVT_TEXT	(ePresetNameText2,	OnEvent)
	EVT_BUTTON	(eSaveAs,			OnEvent)
	
	EVT_CHOICE	(eInstrument,		OnEvent)
	EVT_CHOICE	(eInstrumentROM,	OnEvent)
	EVT_SLIDER	(eVolume,			OnEvent)
	EVT_SLIDER	(ePan,				OnEvent)

	EVT_CHOICE	(eKeyLow,			OnEvent)
	EVT_CHOICE	(eKeyHigh,			OnEvent)
	EVT_SPINCTRL(eKeyLowFade,		OnSpinEvent)
	EVT_SPINCTRL(eKeyHighFade,		OnSpinEvent)
	EVT_SPINCTRL(eVelocityLow,		OnSpinEvent)
	EVT_SPINCTRL(eVelocityLowFade,	OnSpinEvent)
	EVT_SPINCTRL(eVelocityHigh,		OnSpinEvent)
	EVT_SPINCTRL(eVelocityHighFade,	OnSpinEvent)
	EVT_SPINCTRL(eRTLow,			OnSpinEvent)
	EVT_SPINCTRL(eRTLowFade,		OnSpinEvent)
	EVT_SPINCTRL(eRTHigh,			OnSpinEvent)
	EVT_SPINCTRL(eRTHighFade,		OnSpinEvent)

	EVT_SPINCTRL(eTranspose,		OnSpinEvent)
	EVT_SPINCTRL(eTuningCoarse,		OnSpinEvent)
	EVT_SPINCTRL(eTuningFine,		OnSpinEvent)

	EVT_CHOICE	(eFilter,			OnEvent)
	EVT_SPINCTRL(eFilterFreq,		OnSpinEvent)
	EVT_SPINCTRL(eFilterRes,		OnSpinEvent)

	EVT_CHOICE	(eLFO1Shape,		OnEvent)
	EVT_CHOICE	(eLFO1Rate,			OnEvent)
	EVT_CHOICE	(eLFO1Sync,			OnEvent)
	EVT_SPINCTRL(eLFO1Delay,		OnSpinEvent)
	EVT_SPINCTRL(eLFO1Var,			OnSpinEvent)

	EVT_CHOICE	(eLFO2Shape,		OnEvent)
	EVT_CHOICE	(eLFO2Rate,			OnEvent)
	EVT_CHOICE	(eLFO2Sync,			OnEvent)
	EVT_SPINCTRL(eLFO2Delay,		OnSpinEvent)
	EVT_SPINCTRL(eLFO2Var,			OnSpinEvent)

	EVT_SPINCTRL(eChorus,			OnSpinEvent)
	EVT_SPINCTRL(eChorusWidth,		OnSpinEvent)
	EVT_SPINCTRL(eStart,			OnSpinEvent)
	EVT_SPINCTRL(eDelay,			OnSpinEvent)
	EVT_CHOICE	(eNonTranspose,		OnEvent)
	EVT_CHOICE	(eSoloMode,			OnEvent)
	EVT_CHOICE	(eAssignGroup,		OnEvent)
	EVT_CHOICE	(eGlide,			OnEvent)
	EVT_CHOICE	(eGlideCurve,		OnEvent)

	EVT_CHOICE	(eVelEnvMode,		OnEvent)
	EVT_SPINCTRL(eVelEnvAttack1,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvAttack2,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvDecay1,		OnSpinEvent)
	EVT_SPINCTRL(eVelEnvDecay2,		OnSpinEvent)
	EVT_SPINCTRL(eVelEnvRelease1,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvRelease2,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvAttack1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvAttack2Rt,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvDecay1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvDecay2Rt,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvRelease1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eVelEnvRelease2Rt,	OnSpinEvent)

	EVT_CHOICE	(eFilEnvMode,		OnEvent)
	EVT_SPINCTRL(eFilEnvAttack1,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvAttack2,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvDecay1,		OnSpinEvent)
	EVT_SPINCTRL(eFilEnvDecay2,		OnSpinEvent)
	EVT_SPINCTRL(eFilEnvRelease1,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvRelease2,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvAttack1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvAttack2Rt,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvDecay1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvDecay2Rt,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvRelease1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eFilEnvRelease2Rt,	OnSpinEvent)

	EVT_CHOICE	(eAuxEnvMode,		OnEvent)
	EVT_SPINCTRL(eAuxEnvAttack1,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvAttack2,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvDecay1,		OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvDecay2,		OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvRelease1,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvRelease2,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvAttack1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvAttack2Rt,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvDecay1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvDecay2Rt,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvRelease1Rt,	OnSpinEvent)
	EVT_SPINCTRL(eAuxEnvRelease2Rt,	OnSpinEvent)
	
	EVT_CHOICE	(eFXAAlgId,			OnEvent)
	EVT_SPINCTRL(eFXADecay,			OnSpinEvent)
	EVT_SPINCTRL(eFXAHFDamp,		OnSpinEvent)
	EVT_SPINCTRL(eFXBFXA,			OnSpinEvent)
	EVT_SPINCTRL(eFXASendAmt1,		OnSpinEvent)
	EVT_SPINCTRL(eFXASendAmt2,		OnSpinEvent)
	EVT_SPINCTRL(eFXASendAmt3,		OnSpinEvent)
	EVT_SPINCTRL(eFXASendAmt4,		OnSpinEvent)
	
	EVT_CHOICE	(eFXBAlgId,			OnEvent)
	EVT_SPINCTRL(eFXBFeedbk,		OnSpinEvent)
	EVT_SPINCTRL(eFXBLFORate,		OnSpinEvent)
	EVT_CHOICE	(eFXBDelay,			OnEvent)
	EVT_SPINCTRL(eFXBSendAmt1,		OnSpinEvent)
	EVT_SPINCTRL(eFXBSendAmt2,		OnSpinEvent)
	EVT_SPINCTRL(eFXBSendAmt3,		OnSpinEvent)
	EVT_SPINCTRL(eFXBSendAmt4,		OnSpinEvent)

	EVT_BUTTON	(eRTControllerSave,	OnEvent)

	EVT_NOTEBOOK_PAGE_CHANGED(eNotebook, OnNotebookEvent)

	EVT_LEFT_DOWN(OnLeftDown)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(PresetTreeCtrl, wxTreeCtrl)
	EVT_TREE_ITEM_ACTIVATED	(eTreeCtrlID, OnTreeItemActivated)
	EVT_TREE_SEL_CHANGED	(eTreeCtrlID, OnTreeItemSelected)
END_EVENT_TABLE()


short IdxFromMapValue(short* pMap, int numElems, short val)
{
	for(int i = 0; i < numElems; i++)
	{
		if(pMap[i] == val)
			return i;
	}
	return -1;
}
