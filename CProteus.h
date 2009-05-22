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

#ifndef _CProteus_h_
#define _CProteus_h_


#include <Windows.h>
#include <Mmsystem.h>
#include <stdio.h>

#include "Interfaces.h"

#define BUFFER_SIZE 2048

enum
{
	eStatusNone,
	eStatusGetParam,
	eStatusSetParam,
	eStatusReceiving,
	eStatusHardwareConfig,
	eStatusNameRequest,
	eStatusSimpleDump,
	eStatusArpeggiatorDump,
	eStatusEffectsDump,
	eStatusLinkDump,
	eStatusLayerDump,
	eStatusFilterDump,
	eStatusLFODump,
	eStatusEnvelopeDump,
	eStatusPatchcordDump,
	eStatusLCDDump,
};

enum
{
	eObjectTypePreset		= 1,
	eObjectTypeInstrument	= 2,
	eObjectTypeArp			= 3,
	eObjectTypeSetup		= 4,
	eObjectTypeDemo			= 5,
	eObjectTypeRiff			= 6
};

static unsigned char dataHeader[] =								{ 0xF0, 0x18, 0x0F, 0x00, 0x55 };
static unsigned char dataParamRequest[] =						{ 0x02, 0x01, 0x00, 0x00 };
static unsigned char dataParamEdit[] =							{ 0x01, 0x02, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataHardwareConfigRequest[] =				{ 0x0A };
static unsigned char dataNameRequest[] =						{ 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataPresetCommonParametersDump[] =			{ 0x11, 0x11, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataPresetArpeggiatorParametersDump[] =	{ 0x11, 0x12, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataPresetEffectsParametersDump[] =		{ 0x11, 0x13, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataPresetLinkParametersDump[] =			{ 0x11, 0x14, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataLayerParametersDump[] =				{ 0x11, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataLayerFilterParametersDump[] =			{ 0x11, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataLayerLFOParametersDump[] =				{ 0x11, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataLayerEnvelopeParametersDump[] =		{ 0x11, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataLayerPatchcordParametersDump[] =		{ 0x11, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char dataLCDScreenDumpRequest[] =				{ 0x1B, 0x01 };
static unsigned char dataLCDScreenDump[61];
static unsigned char dataCopyPreset[] =							{ 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


#define OFFSET_DEVICE_ID		3
#define OFFSET_PARAM_EDIT_ID	2
#define OFFSET_PARAM_EDIT_DATA	4
#define OFFSET_DUMP				11


#define TIMEOUT			1000
#define POLL_INTERVAL	100
#define MAX_PRESETS		2048
#define MAX_INSTRUMENTS 2048
#define MAX_SIMMS		8
#define NAME_LENGTH		16
#define PATCHCORDS		24
#define PPATCHCORDS		12

static char* kNotInstalled = "(not installed)";
static char* kSSSSSSSSSSSSSSSS = "SSSSSSSSSSSSSSSS";

struct SPresetPatchcord
{
	short Source;
	short Dest;
	short Amount;
};

struct SGeneralParameters
{
	// all shorts LSB first
	char PresetName[16];
	short Preset_Ctrl_A;
	short Preset_Ctrl_B;
	short Preset_Ctrl_C;
	short Preset_Ctrl_D;
	short Preset_Ctrl_E;
	short Preset_Ctrl_F;
	short Preset_Ctrl_G;
	short Preset_Ctrl_H;
	short Preset_Kbd_Tune;
	short Preset_Ctrl_I;
	short Preset_Ctrl_J;
	short Preset_Ctrl_K;
	short Preset_Ctrl_L;
	short Preset_Riff;
	short Preset_Riff_ROM_ID;
	short Preset_Tempo_Offset;
	SPresetPatchcord Preset_Patchcords[12];
};

struct SArpeggiatorParameters
{
	short Preset_Arp_Status;
	short Preset_Arp_Mode;
	short Preset_Arp_Pattern;
	short Preset_Arp_Node;
	short Preset_Arp_Vel;
	short Preset_Arp_Gate_Time;
	short Preset_Arp_Exit_Count;
	short Preset_Arp_Int;
	short Preset_Arp_Sync;
	short Preset_Arp_Pre_Delay;
	short Preset_Arp_Duration;
	short Preset_Arp_Recycle;
	short Preset_Arp_KBD_Thru;
	short Preset_Arp_Latch;
	short Preset_Arp_KR_Low;
	short Preset_Arp_KR_High;
	short Preset_Arp_Pattern_Speed;
	short Preset_Arp_Pattern_ROM_ID;
	short Arp_PostDelay;
};

struct SEffectsParameters
{
	short FX_A_Algorithm;
	short FX_A_Decay;
	short FX_A_HFDamp;
	short FX_AB;
	short FX_A_Mix_Main;
	short FX_A_Mix_Sub1;
	short FX_A_Mix_Sub2;
	short FX_B_Algorithm;
	short FX_B_Feedback;
	short FX_B_LFO_Rate;
	short FX_B_Delay;
	short FX_B_Mix_Main;
	short FX_B_Mix_Sub1;
	short FX_B_Mix_Sub2;
	short FX_B_Mix_Sub3;
	short FX_B_Mix_Sub4;
};

struct SLink
{
	short Link_Preset;
	short Link_Volume;
	short Link_Pan;
	short Link_Transpose;
	short Link_Delay;
	short Link_Key_Low;
	short Link_Key_High;
	short Link_Vel_Low;
	short Link_Vel_High;
};

struct SLinkParameters
{
	SLink Links[2];
	short Link_1_Preset_ROM_ID;
	short Link_2_Preset_ROM_ID;
};

// layer structs
struct SLayerGeneralParameters
{
	short Layer_Instrument;
	short Layer_Volulme;
	short Layer_Pan;
	short Layer_Submix;
	short Layer_Key_Low;
	short Layer_Key_LowFade;
	short Layer_Key_High;
	short Layer_Key_HighFade;
	short Layer_Vel_Low;
	short Layer_Vel_LowFade;
	short Layer_Vel_High;
	short Layer_Vel_HighFade;
	short Layer_RT_Low;
	short Layer_RT_LowFade;
	short Layer_RT_High;
	short Layer_RT_HighFade;
	short Layer_CTune;
	short Layer_FTune;
	short Layer_DETune;
	short Layer_DETune_Width;
	short Layer_Transpose;
	short Layer_Non_Transpose;
	short Layer_Blend;
	short Layer_Glide_Rate;
	short Layer_Glide_Curve;
	short Layer_Loop;
	short Layer_Start_Delay;
	short Layer_Start_Offset;
	short Layer_Solo;
	short Layer_Group;
	short Layer_Inst_ROM_ID;
};

struct SFilterParameters
{
	short Filt_Type;
	short Filt_Freq;
	short Filt_Q;
};

struct SLFOParameters
{
	short LFO_Rate;
	short LFO_Shape;
	short LFO_Delay;
	short LFO_Var;
	short LFO_Sync;
};

struct SEnvelopeParameters
{
	short Env_Mode;
	short Env_Attack1_Rate;
	short Env_Attack1_Level;
	short Env_Decay1_Rate;
	short Env_Decay1_Level;
	short Env_Release1_Rate;
	short Env_Release1_Level;
	short Env_Attack2_Rate;
	short Env_Attack2_Level;
	short Env_Decay2_Rate;
	short Env_Decay2_Level;
	short Env_Release2_Rate;
	short Env_Release2_Level;
};

struct SPatchcord
{
	short Cord_Source;
	short Cord_Dest;
	short Cord_Amount;
};

struct SLayer
{
	SLayerGeneralParameters LayerGeneralParameters;
	SFilterParameters		FilterParameters;
	SLFOParameters			LFOParameters[2];
	SEnvelopeParameters		EnvelopeParameters[3];
	SPatchcord				Patchcords[24];
};

struct SMasterParameters
{ 
	short MasterClockTempo;
};

struct Preset
{
	short ID;
	short ROM_ID;

	SGeneralParameters		GeneralParameters;
	SArpeggiatorParameters	ArpeggiatorParameters;
	SEffectsParameters		EffectsParameters;
	SLinkParameters			LinkParameters;
	SLayer					Layers[4];

	bool IsEditable() { return ROM_ID == 0; }
};

struct Simm
{
	short mSimmID;
	short mNumPresets;
	short mNumInstruments;
	Preset* mPresets[MAX_PRESETS];
	char mPresetNames[MAX_PRESETS][NAME_LENGTH];
	char mInstrumentNames[MAX_INSTRUMENTS][NAME_LENGTH];

	bool mLoadedPresets;

	Simm()
	{
		mLoadedPresets		= false;
		mSimmID				= -1;
		mNumPresets			= 0;
		mNumInstruments		= 0;
	}

	void GetName(char* name)
	{
		switch(mSimmID)
		{
			case 6:
				strcpy(name, "B-3");
				break;

			case 14:
				strcpy(name, "XROM");
				break;

			default:
				sprintf(name, "ROM %i", mSimmID);
				break;
		}
	}
};

class CProteusControllerApp;

class CProteus
{
public:
	CProteus();
	~CProteus();

	long Connect					(UINT inDeviceID, UINT outDeviceID, int thruDeviceID, UINT proteusID);
	long Disconnect					();
	void DeleteDataStructures		();
	long SetParameter				(short paramID, short value, bool nibbleize = true);
	long GetParameter				(short paramID, short* value);
	long HardwareConfigRequest		();
	long NameRequest				(short ROM, short objectNum, byte type, char* buff);
	long ProgramChange				(short ROM, short programID);
	
	long CommonParmetersDump		(short ROM, short presetNumber);
	long ArpeggiatorParmetersDump	(short ROM, short presetNumber);
	long EffectsParmetersDump		(short ROM, short presetNumber);
	long LinkParmetersDump			(short ROM, short presetNumber);
	long LayerParmetersDump			(short ROM, short presetNumber, short layer);
	long LayerFiltersDump			(short ROM, short presetNumber, short layer);
	long LayerLFODump				(short ROM, short presetNumber, short layer);
	long LayerEnvelopeDump			(short ROM, short presetNumber, short layer);
	long LayerPatchcordDump			(short ROM, short presetNumber, short layer);
	long LCDDump					();
	long LCDSet						();
	long SavePreset					(short srcROM, short srcPreset, short destPreset);
	long RenamePreset				(const char* name);
	long SaveInitialControllers		(SGeneralParameters* params);

	long PrepareData(unsigned char* data, int length);
	long PrepareDumpData(unsigned char* data, short ROM, short presetNumber);
	long PrepareLayerDumpData(unsigned char* data, short ROM, short presetNumber, short layer);
	long Send();
	long SendAndWait(int status = eStatusNone);
	bool IsValidData(unsigned char* data, byte command);
	Preset* GetOrCreatePreset();

	void MidiOutProc	(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwParam1, DWORD dwParam2);
	void MidiInProc		(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2);
	void MidiThruProc	(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2);

	void OutputMessageString(unsigned char* ptr, DWORD bytes);

	long LoadPreset(short ROM, short presetNumber, IProgressCallback* progress);
	long DownloadUserPresetNames(IProgressCallback* progress);
	long DownloadROMPresetNames(short ROM, IProgressCallback* progress);
	long DownloadInstrumentNames(IProgressCallback* progress);

	long LoadUserPresetNames(const char* fileName);
	long LoadROMPresetNames(const char* fileName, short ROM);
	long LoadInstrumentNames(const char* fileName);
	long SaveUserPresetNames(const char* fileName);
	long SaveROMPresetNames(const char* fileName, short ROM);
	long SaveInstrumentNames(const char* fileName);

	long LoadMasterParameters();

	short GetSimmIdx(short simmID);

	short mNumUserPresets;
	short mNumSimms;
	Simm* mSimms[MAX_SIMMS];
	Preset* mUserPresets[MAX_PRESETS];
	char mPresetNames[MAX_PRESETS][NAME_LENGTH];
	SMasterParameters mMasterParameters;


	HMIDIIN mMidiIn;
	HMIDIIN mMidiThru;
	HMIDIOUT mMidiOut;
	UINT mProteusID;

	MIDIHDR mOutHeader;
	MIDIHDR mInHeader;
	unsigned char mInBuffer[BUFFER_SIZE];
	unsigned char mOutBuffer[BUFFER_SIZE];

	short mParamResult;

	short mDumpPresetID;
	short mDumpROM;
	short mDumpLayer;

	short mCurPresetID;
	short mCurROMID;

	unsigned char mLCDDump[59];

	bool mConnected;
	bool mTerminating;
	int mStatus;

	bool mTransmitClock;
	double mClockTempo;
	DWORD mLastClockTime;
	double mTempoHistory[24];
	short mTempoHistoryLength;

	CProteusControllerApp* mApp;
};

void CALLBACK MidiOutProcCB(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void CALLBACK MidiInProcCB(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void CALLBACK MidiThruProcCB(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);


#define MAX_MIDI_DEVICES 10

class CMidiDeviceInfo
{
public:
	unsigned int NumInDevices;
	unsigned int NumOutDevices;
	char InDevices[MAX_MIDI_DEVICES][MAXPNAMELEN];
	char OutDevices[MAX_MIDI_DEVICES][MAXPNAMELEN];

	int SelectedInDeviceId;
	int SelectedOutDeviceId;
	int SelectedThruDeviceId;
	int SelectedSysExId;
	
	void Initialize(const char* optionsPath);
};

void Nibbleize(unsigned char* buffer, short val);
void UnNibbleize(short& val);

class CScopeTimer
{
public:
	CScopeTimer(const char* scopeName);
	~CScopeTimer();

	const char* mScopeName;
	DWORD mStartTime;
};

#endif