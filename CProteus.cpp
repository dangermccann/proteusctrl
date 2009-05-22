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

#include "CProteus.h"
#include "Commands.h"
#include "Events.h"
#include "CProteusController.h"

#include "wx/wxprec.h"
#include "wx/debug.h"

#include <stdio.h>

#define chkstep(progress) \
	if(progress->IsCancelled()) return -1;	\
	progress->Step();

CProteus::CProteus()
{
	mConnected = false;
	mTransmitClock = true;
	mProteusID = 0;
	mMidiIn = NULL;
	mMidiOut = NULL;
	mMidiThru = NULL;
	mStatus = eStatusNone;
	mApp = NULL;
	mClockTempo = 0;
	mLastClockTime = 0;
	mTempoHistoryLength = 0;

	for(int i = 0; i < MAX_SIMMS; i++)
	{
		mSimms[i] = NULL;
	}

	for(int k = 0; k < MAX_PRESETS; k++)
	{
		mUserPresets[k] = NULL;
	}
}

CProteus::~CProteus()
{
	DeleteDataStructures();
}

long CProteus::Connect(UINT inDeviceID, UINT outDeviceID, int thruDeviceID, UINT proteusID)
{
	mTerminating = false;

	MMRESULT err = midiInOpen(&mMidiIn, inDeviceID, (DWORD) MidiInProcCB, (DWORD) this, CALLBACK_FUNCTION);
	if(err)  { wxASSERT(0); return err; };
	
	memset(mInBuffer, 0, BUFFER_SIZE);
	memset(&mInHeader, 0, sizeof(MIDIHDR));
	mInHeader.dwBufferLength = BUFFER_SIZE;
	mInHeader.dwFlags = 0;
	mInHeader.lpData = (char*) mInBuffer;
	err = midiInPrepareHeader(mMidiIn, &mInHeader, sizeof(mInHeader));
	if(err)  { wxASSERT(0); return err; };

	err = midiInAddBuffer(mMidiIn, &mInHeader, sizeof(mInHeader));
	if(err)  { wxASSERT(0); return err; };

	err = midiInStart(mMidiIn);
	if(err)  { wxASSERT(0); return err; };

	err = midiOutOpen(&mMidiOut, outDeviceID, (DWORD) MidiOutProcCB, (DWORD) this, CALLBACK_FUNCTION);
	if(err)  { wxASSERT(0); return err; };

	mProteusID = proteusID;

	if(thruDeviceID != -1)
	{
		err = midiInOpen(&mMidiThru, thruDeviceID, (DWORD) MidiThruProcCB, (DWORD) this, CALLBACK_FUNCTION);
		if(err)  { wxASSERT(0); return err; };

		err = midiInStart(mMidiThru);
		if(err)  { wxASSERT(0); return err; };
	}
	else
		mMidiThru = NULL;

	err = HardwareConfigRequest();
	if(err) return err;

	mLastClockTime = 0;
	mClockTempo = 0;
	mTempoHistoryLength = 0;
	
	return S_OK;
}

#pragma warning( push )
#pragma warning( disable : 4390 )

long CProteus::Disconnect()
{
	MMRESULT err = 0;

	if(mConnected)
	{
		mTerminating = true;

		if(mMidiIn)
		{
			err = midiInReset(mMidiIn);
			if(err) wxASSERT(0);

			err = midiInStop(mMidiIn);
			if(err) wxASSERT(0);

			err = midiInUnprepareHeader(mMidiIn, &mInHeader, sizeof(mInHeader));
			if(err) wxASSERT(0);

			err = midiInClose(mMidiIn);
			if(err) wxASSERT(0);
		}

		if(mMidiThru)
		{
			err = midiInReset(mMidiThru);
			if(err) wxASSERT(0);
			
			err = midiInStop(mMidiThru);
			if(err) wxASSERT(0);

			err = midiInClose(mMidiThru);
			if(err && err != 0x41) wxASSERT(0);
		}

		if(mMidiOut)
		{
			err = midiOutUnprepareHeader(mMidiOut, &mOutHeader, sizeof(mOutHeader));
			if(err) wxASSERT(0);

			err = midiOutClose(mMidiOut);
			if(err) wxASSERT(0);
		}

		mConnected = false;
		DeleteDataStructures();
	}
	return S_OK;
}

void CProteus::DeleteDataStructures()
{
	for(int i = 0; i < MAX_SIMMS; i++)
	{
		if(mSimms[i])
		{
			for(int j = 0; j < MAX_PRESETS; j++)
			{
				if(mSimms[i]->mPresets[j])
				{
					delete mSimms[i]->mPresets[j];
					mSimms[i]->mPresets[j] = NULL;
				}
			}
			
			delete mSimms[i];
			mSimms[i] = NULL;
		}
	}
	
	for(int k = 0; k < MAX_PRESETS; k++)
	{
		if(mUserPresets[k])
		{
			delete mUserPresets[k];
			mUserPresets[k] = NULL;
		}
	}
}

#pragma warning( pop ) 

long CProteus::SetParameter(short paramID, short value, bool nibbleize)
{
	dataParamEdit[OFFSET_PARAM_EDIT_ID+1] = (unsigned char) paramID;
	dataParamEdit[OFFSET_PARAM_EDIT_ID] = (paramID >> 8);

	if(nibbleize)
	{
		Nibbleize(&dataParamEdit[OFFSET_PARAM_EDIT_DATA], value);
	}
	else
	{
		dataParamEdit[OFFSET_PARAM_EDIT_DATA] = (unsigned char) value;
		dataParamEdit[OFFSET_PARAM_EDIT_DATA+1] = (value >> 8);
	}

	PrepareData(dataParamEdit, sizeof(dataParamEdit));
	
	MMRESULT err = Send();
	if(err) return err;
	
	return S_OK;
}

long CProteus::GetParameter(short paramID, short* value)
{
	mParamResult = 0;

	dataParamRequest[OFFSET_PARAM_EDIT_ID+1] =  (unsigned char) paramID;
	dataParamRequest[OFFSET_PARAM_EDIT_ID] = (paramID >> 8);
	PrepareData(dataParamRequest, sizeof(dataParamRequest));
	mStatus = eStatusGetParam;

	MMRESULT err = SendAndWait();
	if(err) return err;

	*value = mParamResult;

	wxString str;
	str = wxString::Format("GetParameter: %x - %x\n", paramID, mParamResult);
	OutputDebugString(str);
	
	return S_OK;
}

long CProteus::HardwareConfigRequest()
{
	PrepareData(dataHardwareConfigRequest, sizeof(dataHardwareConfigRequest));
	mStatus = eStatusHardwareConfig;	
	return SendAndWait();
}

long CProteus::NameRequest(short ROM, short objectNum, byte type, char* buff)
{
	dataNameRequest[1] = type;
	PrepareDumpData(dataNameRequest, ROM, objectNum);
	
	mStatus = eStatusNameRequest;
	PrepareData(dataNameRequest, sizeof(dataNameRequest));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	memcpy(buff, mOutBuffer, 16);
	
	return S_OK;	
}

long CProteus::ProgramChange(short ROM, short programID)
{
	DWORD pc = 0x000000C0;
	DWORD bc = 0x000000B0;

	DWORD dwBank = ROM;
	dwBank = dwBank << 16;
	bc |= dwBank;

	MMRESULT err = midiOutShortMsg(mMidiOut, bc);
	if(err) return err;

	dwBank = programID / 0x80;
	bc = 0x000020B0;
	dwBank = dwBank << 16;
	bc |= dwBank;

	err = midiOutShortMsg(mMidiOut, bc);
	if(err) return err;

	DWORD dwProgram = programID % 0x80;
	dwProgram = dwProgram << 8;
	pc |= dwProgram;

	err = midiOutShortMsg(mMidiOut, pc);

	return err;
}

long CProteus::CommonParmetersDump(short ROM, short presetNumber)
{
	mStatus = eStatusSimpleDump;

	PrepareDumpData(dataPresetCommonParametersDump, ROM, presetNumber);
	PrepareData(dataPresetCommonParametersDump, sizeof(dataPresetCommonParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;

	return S_OK;
}

long CProteus::ArpeggiatorParmetersDump(short ROM, short presetNumber)
{
	mStatus = eStatusArpeggiatorDump;
	
	PrepareDumpData(dataPresetArpeggiatorParametersDump, ROM, presetNumber);	
	PrepareData(dataPresetArpeggiatorParametersDump, sizeof(dataPresetArpeggiatorParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::EffectsParmetersDump(short ROM, short presetNumber)
{
	mStatus = eStatusEffectsDump;

	PrepareDumpData(dataPresetEffectsParametersDump, ROM, presetNumber);	
	PrepareData(dataPresetEffectsParametersDump, sizeof(dataPresetEffectsParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LinkParmetersDump(short ROM, short presetNumber)
{
	mStatus = eStatusLinkDump;
	
	PrepareDumpData(dataPresetLinkParametersDump, ROM, presetNumber);	
	PrepareData(dataPresetLinkParametersDump, sizeof(dataPresetLinkParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LayerParmetersDump(short ROM, short presetNumber, short layer)
{
	mStatus = eStatusLayerDump;
	
	PrepareLayerDumpData(dataLayerParametersDump, ROM, presetNumber, layer);	
	PrepareData(dataLayerParametersDump, sizeof(dataLayerParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LayerFiltersDump(short ROM, short presetNumber, short layer)
{
	mStatus = eStatusFilterDump;
	
	PrepareLayerDumpData(dataLayerFilterParametersDump, ROM, presetNumber, layer);	
	PrepareData(dataLayerFilterParametersDump, sizeof(dataLayerFilterParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LayerLFODump(short ROM, short presetNumber, short layer)
{
	mStatus = eStatusLFODump;
	
	PrepareLayerDumpData(dataLayerLFOParametersDump, ROM, presetNumber, layer);	
	PrepareData(dataLayerLFOParametersDump, sizeof(dataLayerLFOParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LayerEnvelopeDump(short ROM, short presetNumber, short layer)
{
	mStatus = eStatusEnvelopeDump;
	
	PrepareLayerDumpData(dataLayerEnvelopeParametersDump, ROM, presetNumber, layer);	
	PrepareData(dataLayerEnvelopeParametersDump, sizeof(dataLayerEnvelopeParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LayerPatchcordDump(short ROM, short presetNumber, short layer)
{
	mStatus = eStatusPatchcordDump;
	
	PrepareLayerDumpData(dataLayerPatchcordParametersDump, ROM, presetNumber, layer);	
	PrepareData(dataLayerPatchcordParametersDump, sizeof(dataLayerPatchcordParametersDump));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LCDDump()
{
	mStatus = eStatusLCDDump;

	PrepareData(dataLCDScreenDumpRequest, sizeof(dataLCDScreenDumpRequest));
	MMRESULT err = SendAndWait();
	if(err) return err;
	
	return S_OK;
}

long CProteus::LCDSet()
{
	LCDDump();

	memset(dataLCDScreenDump, 0, sizeof(dataLCDScreenDump));

	dataLCDScreenDump[0] = COMMAND_LCD_DUMP;
	dataLCDScreenDump[1] = 0x01;
	dataLCDScreenDump[2] = 0x02;
	dataLCDScreenDump[3] = 0x18;
	dataLCDScreenDump[4] = 0x08;

	for(int i = 13; i < 61; i+=2)
	{
		dataLCDScreenDump[i] = 'A';
		dataLCDScreenDump[i+1] = 'B';
	}
	
	PrepareData(dataLCDScreenDump, sizeof(dataLCDScreenDump));
	MMRESULT err = Send();
	if(err) return err;

	Sleep(1000);

	memcpy(&dataLCDScreenDump[2], mLCDDump, sizeof(mLCDDump));

	PrepareData(dataLCDScreenDump, sizeof(dataLCDScreenDump));
	err = Send();
	if(err) return err;
	
	return S_OK;
}

long CProteus::SavePreset(short srcROM, short srcPreset, short destPreset)
{
	Nibbleize(&dataCopyPreset[1], srcPreset);
	Nibbleize(&dataCopyPreset[3], destPreset);
	Nibbleize(&dataCopyPreset[5], srcROM);

	PrepareData(dataCopyPreset, sizeof(dataCopyPreset));
	MMRESULT err = Send();
	if(err) return err;

	return S_OK;
}

long CProteus::RenamePreset(const char* name)
{
	char paddedName[NAME_LENGTH+1];
	int len = strlen(name);
	strcpy(paddedName, name);
	for(int j = 0; j < NAME_LENGTH - len; j++)
	{
		paddedName[len + j] = 0x20;
	}
	paddedName[NAME_LENGTH] = 0;
	
	unsigned char buffer[66];
	buffer[0] = 0x01;
	buffer[1] = 0x20;	// 32 byte pairs
	for(int i = 0; i < NAME_LENGTH; i++)
	{
		short paramID = PRESET_NAME_CHAR_0 + 0x100 * i;
		buffer[3 + i*4] = (unsigned char) paramID;
		buffer[2 + i*4] = (paramID >> 8);

		buffer[4 + i*4] = paddedName[i];
		buffer[5 + i*4] = 0;
	}

	PrepareData(buffer, sizeof(buffer));
	
	MMRESULT err = Send();
	if(err) return err;

	return S_OK;
}

long CProteus::SaveInitialControllers(SGeneralParameters* params)
{
	byte buffer[50];
	buffer[0] = 0x01;
	buffer[1] = 0x18;	// 24 byte pairs

	for(int i = 0; i < 12; i++)
	{
		short paramID = PRESET_CTRL_A + 0x100 * i;
		if(i > 7)
			paramID += 0x100;

		buffer[3 + i*4] = (unsigned char) paramID;
		buffer[2 + i*4] = (paramID >> 8);
		
		Nibbleize(&buffer[4 + i*4], (&params->Preset_Ctrl_A)[i > 7 ? i+1 : i]);
	}
	
	PrepareData(buffer, sizeof(buffer));
	
	MMRESULT err = Send();
	if(err) return err;
	
	return S_OK;
}

long CProteus::PrepareData(unsigned char* data, int length)
{
	memset(mOutBuffer, 0, BUFFER_SIZE);
	int i = 0;
	for( ; i < sizeof(dataHeader); i++)
	{
		mOutBuffer[i] = dataHeader[i];
	}
	mOutBuffer[OFFSET_DEVICE_ID] = mProteusID;
	for(int j = 0; j < length; j++)
	{
		mOutBuffer[i] = data[j];
		i++;
	}
	mOutBuffer[i++] = EOX;

	mOutHeader.dwBufferLength = i;
	mOutHeader.dwBytesRecorded = i;
	mOutHeader.lpData = (char*) mOutBuffer;
	mOutHeader.dwFlags = 0;
	MMRESULT err = midiOutPrepareHeader(mMidiOut, &mOutHeader, sizeof(mOutHeader));
	if(err)  { wxASSERT(0); return err; };

	return S_OK;
}

long CProteus::PrepareDumpData(unsigned char* data, short ROM, short presetNumber)
{
	Nibbleize(&data[2], presetNumber);
	Nibbleize(&data[4], ROM);

	mDumpROM = ROM;
	mDumpPresetID = presetNumber;

	return S_OK;
}

long CProteus::PrepareLayerDumpData(unsigned char* data, short ROM, short presetNumber, short layer)
{
	Nibbleize(&data[2], presetNumber);
	Nibbleize(&data[4], layer);
	Nibbleize(&data[6], ROM);
	
	mDumpROM = ROM;
	mDumpPresetID = presetNumber;
	mDumpLayer = layer;
	
	return S_OK;
}

long CProteus::Send()
{
	if(mMidiThru)
		midiInReset(mMidiThru);

	MMRESULT err = midiOutLongMsg(mMidiOut, &mOutHeader, sizeof(mOutHeader));

	if(mMidiThru)
		midiInStart(mMidiThru);

	return err;
}

long CProteus::SendAndWait(int status)
{
	MMRESULT err = Send();
	if(err) { wxASSERT(0); return err; }

	int count = 0;
	while(mStatus != status && (count * POLL_INTERVAL) < TIMEOUT)
	{
		Sleep(POLL_INTERVAL);
		count++;
	}

	if(mStatus != status && (count * POLL_INTERVAL) >= TIMEOUT)
	{
		OutputDebugString("Operation timed out\r\n");
		return E_FAIL;
	}

	return S_OK;
}

bool CProteus::IsValidData(unsigned char* data, byte command)
{

	for(int i = 0; i < sizeof(dataHeader); i++)
	{
		if(i != OFFSET_DEVICE_ID && *(data+i) != dataHeader[i])
			return false;
	}
	if(*(data+i) != command)
		return false;
	return true;
}

Preset* CProteus::GetOrCreatePreset()
{
	Preset* preset = NULL;
	bool isNew = false;
	if(mDumpROM)
	{
		short romIdx = GetSimmIdx(mDumpROM);
		if(romIdx == -1)
			return NULL;

		if(mSimms[romIdx]->mPresets[mDumpPresetID] == NULL)
		{
			mSimms[romIdx]->mPresets[mDumpPresetID] = new Preset();
			isNew = true;
		}
		preset = mSimms[romIdx]->mPresets[mDumpPresetID];
	}
	else
	{
		if(mUserPresets[mDumpPresetID] == NULL)
		{
			mUserPresets[mDumpPresetID] = new Preset();
			isNew = true;
		}
		preset = mUserPresets[mDumpPresetID];
	}
	if(isNew)
	{
		preset->ID = mDumpPresetID;
		preset->ROM_ID = mDumpROM;
		memset(&preset->GeneralParameters, 0, sizeof(SGeneralParameters));
		memset(&preset->EffectsParameters, 0, sizeof(SEffectsParameters));
	}
	return preset;
}

void CProteus::MidiOutProc(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
{
	switch(wMsg)
	{
	case MOM_DONE:
		midiOutUnprepareHeader(mMidiOut, &mOutHeader, sizeof(mOutHeader));
		break;
	}
}

void CProteus::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
{
	int newStatus = mStatus;
	switch(wMsg)
	{
		case MIM_LONGDATA:
		{
			LPMIDIHDR header = (LPMIDIHDR) dwParam1;
			//OutputMessageString((unsigned char*) header->lpData, header->dwBytesRecorded);
			switch(mStatus)
			{
				case eStatusGetParam:
				{
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PARAM_EDIT))
					{
						memcpy(&mParamResult, &header->lpData[9], 2);
						UnNibbleize(mParamResult);
					}

					newStatus = eStatusNone;
					break;
				}

				case eStatusHardwareConfig:
				{
					if(IsValidData((unsigned char*) header->lpData, COMMAND_HARDWARE_CONFIG))
					{
						memcpy(&mNumUserPresets, &header->lpData[7], 2);
						mNumSimms = header->lpData[9];
						int offset = 11;
						for(int i = 0; i < mNumSimms; i++)
						{
							if(!mSimms[i])
							{
								mSimms[i] = new Simm();

								for(int j = 0; j < MAX_PRESETS; j++)
								{
									mSimms[i]->mPresets[j] = NULL;
								}
							}

							memcpy(&mSimms[i]->mSimmID, &header->lpData[offset], 2);
							offset+=2;
							memcpy(&mSimms[i]->mNumPresets, &header->lpData[offset], 2);
							offset+=2;
							memcpy(&mSimms[i]->mNumInstruments, &header->lpData[offset], 2);
							offset+=2;

							UnNibbleize(mSimms[i]->mNumPresets);
							UnNibbleize(mSimms[i]->mNumInstruments);
						}
					}
					
					newStatus = eStatusNone;
					break;
				}

				case eStatusNameRequest:
					memset(mOutBuffer, 0, BUFFER_SIZE);
					memcpy(mOutBuffer, &header->lpData[OFFSET_DUMP], NAME_LENGTH);
					newStatus = eStatusNone;
					break;

				case eStatusSimpleDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->GeneralParameters, &header->lpData[OFFSET_DUMP], sizeof(SGeneralParameters));

							for(int i = 0; i < PPATCHCORDS; i++)
							{
								UnNibbleize(preset->GeneralParameters.Preset_Patchcords[i].Source);
								UnNibbleize(preset->GeneralParameters.Preset_Patchcords[i].Dest);
								UnNibbleize(preset->GeneralParameters.Preset_Patchcords[i].Amount);
							}
						}

						char buff[19];
						memcpy(buff, preset->GeneralParameters.PresetName, NAME_LENGTH);
						buff[16] = '\n';
						buff[17] = 0;
						OutputDebugString(buff);
					}
					newStatus = eStatusNone;
					break;

				case eStatusArpeggiatorDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->ArpeggiatorParameters, &header->lpData[OFFSET_DUMP], sizeof(SArpeggiatorParameters));
						}
					}
					newStatus = eStatusNone;
					break;

				case eStatusEffectsDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->EffectsParameters, &header->lpData[OFFSET_DUMP], sizeof(SEffectsParameters));
						}
					}
					newStatus = eStatusNone;
					break;

				case eStatusLinkDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->LinkParameters, &header->lpData[OFFSET_DUMP], sizeof(SLinkParameters));
						}
					}
					newStatus = eStatusNone;
					break;
					
				case eStatusLayerDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->Layers[mDumpLayer].LayerGeneralParameters, &header->lpData[OFFSET_DUMP+2], 
								sizeof(SLayerGeneralParameters));

							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_Instrument);
							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_Volulme);
							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_Pan);
							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_CTune);
							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_FTune);
							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_Transpose);
							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_Blend);
							UnNibbleize(preset->Layers[mDumpLayer].LayerGeneralParameters.Layer_Start_Delay);
						}
					}
					newStatus = eStatusNone;
					break;

				case eStatusFilterDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->Layers[mDumpLayer].FilterParameters, &header->lpData[OFFSET_DUMP+2], 
								sizeof(SFilterParameters));

							UnNibbleize(preset->Layers[mDumpLayer].FilterParameters.Filt_Freq);
							UnNibbleize(preset->Layers[mDumpLayer].FilterParameters.Filt_Type);
							UnNibbleize(preset->Layers[mDumpLayer].FilterParameters.Filt_Q);
						}
					}
					newStatus = eStatusNone;
					break;
				
				case eStatusLFODump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->Layers[mDumpLayer].LFOParameters[0], &header->lpData[OFFSET_DUMP+2], 
								sizeof(SLFOParameters) * 2);

							UnNibbleize(preset->Layers[mDumpLayer].LFOParameters[0].LFO_Rate);
							UnNibbleize(preset->Layers[mDumpLayer].LFOParameters[0].LFO_Delay);
							UnNibbleize(preset->Layers[mDumpLayer].LFOParameters[1].LFO_Rate);
							UnNibbleize(preset->Layers[mDumpLayer].LFOParameters[1].LFO_Delay);
						}
					}
					newStatus = eStatusNone;
					break;

				case eStatusEnvelopeDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->Layers[mDumpLayer].EnvelopeParameters[0], &header->lpData[OFFSET_DUMP+2], 
								sizeof(SEnvelopeParameters) * 3);
						}
					}
					newStatus = eStatusNone;
					break;

				case eStatusPatchcordDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_PRESET_DUMP))
					{
						Preset* preset = GetOrCreatePreset();
						
						if(preset)
						{
							memcpy(&preset->Layers[mDumpLayer].Patchcords[0], &header->lpData[OFFSET_DUMP+2], 
								sizeof(SPatchcord) * PATCHCORDS);

							for(int i = 0; i < PATCHCORDS; i++)
							{
								UnNibbleize(preset->Layers[mDumpLayer].Patchcords[i].Cord_Source);
								UnNibbleize(preset->Layers[mDumpLayer].Patchcords[i].Cord_Dest);
								UnNibbleize(preset->Layers[mDumpLayer].Patchcords[i].Cord_Amount);
							}
						}
					}
					newStatus = eStatusNone;
					break;

				case eStatusLCDDump:
					if(IsValidData((unsigned char*) header->lpData, COMMAND_LCD_DUMP))
					{
						memcpy(mLCDDump, &header->lpData[7], 59);
					}
					newStatus = eStatusNone;
					break;

				default:
					newStatus = eStatusNone;
					//wxASSERT(0);
					break;
			}

			if(!mTerminating)
			{
				memset(mInBuffer, 0, BUFFER_SIZE);
				MMRESULT err = midiInAddBuffer(mMidiIn, &mInHeader, sizeof(mInHeader));
				if(err)  { wxASSERT(0); }
			}
			mStatus = newStatus;
		}
		break;

		case MIM_DATA:
			{
				byte rtMsg = (dwParam1 & 0x0000FF00) >> 8;
				byte rtAmt = (dwParam1 & 0x00FF0000) >> 16;
				if(	   rtMsg == RT_MSG_A || rtMsg == RT_MSG_B || rtMsg == RT_MSG_C || rtMsg == RT_MSG_D
					|| rtMsg == RT_MSG_E || rtMsg == RT_MSG_F || rtMsg == RT_MSG_G || rtMsg == RT_MSG_H
					|| rtMsg == RT_MSG_I || rtMsg == RT_MSG_J || rtMsg == RT_MSG_K || rtMsg == RT_MSG_L)
				{
					if(!mTerminating)
					{
						wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, eRTControllerChange );
						int clientData = rtAmt;
						clientData = clientData << 8;
						clientData |= rtMsg;
						event.SetClientData((void*) clientData);
						wxPostEvent( mApp, event );
					}
				}
			}
			break;

		case MIM_ERROR:
		case MIM_LONGERROR:
			mStatus = eStatusNone;
			wxASSERT(0);
			break;

		case MIM_OPEN:
			mConnected = true;
			break;

		case MIM_CLOSE:
			mStatus = eStatusNone;
			mConnected = false;
			break;
	}
}

void CProteus::MidiThruProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwParam1, DWORD dwParam2)
{
	if(wMsg == MIM_DATA)
	{
		if(dwParam1 >= 0xF8 && dwParam1 <= 0xFF)
		{
			if(dwParam1 == 0xF8)
			{
				DWORD curTime = dwParam2;
				if(curTime > mLastClockTime)
				{
					double dif = curTime - mLastClockTime;
					double seconds = dif / 1000.0;
					double minutes = seconds / 60.0;
					// there should be six messages per sixteenth note
					minutes = minutes * 4.0 * 6.0;
					double newTempo = 1.0 / minutes;
					mTempoHistory[mTempoHistoryLength] = newTempo;
					mTempoHistoryLength++;

					if(mTempoHistoryLength == 24)
					{
						if(!mTerminating)
						{
							double totalTempos = 0;
							for(int i = 0; i < 24; i++)
							{
								totalTempos += mTempoHistory[i];
							}
							mClockTempo = totalTempos / 24.0;
							wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, eTempoChange );
							wxPostEvent( mApp, evt );
							
						}

						mTempoHistoryLength = 0;
					}
				}
				mLastClockTime = curTime;
			}
			
			if(!mTransmitClock)
				return;
		}

		midiOutShortMsg(mMidiOut, dwParam1);
	}
}

void CProteus::OutputMessageString(unsigned char* ptr, DWORD bytes)
{
	TCHAR buffer[80];

	if(mStatus != eStatusReceiving)
		OutputDebugString("*************** System Exclusive **************\r\n");

	short i = 16;

	while(bytes--)
	{
		if (!(--i))
		{
			sprintf(&buffer[0], "0x%02X\r\n", *(ptr)++);
			i = 16;
		}
		else
			sprintf(&buffer[0], "0x%02X ", *(ptr)++);

		OutputDebugString(&buffer[0]);
	}

	if(mStatus != eStatusReceiving)
		OutputDebugString("\r\n***********************************************\r\n");
}


long CProteus::LoadPreset(short ROM, short presetNumber, IProgressCallback* progress)
{
	MMRESULT err = S_OK;

	progress->SetTotalSteps(25);

	err = ProgramChange(ROM, presetNumber);
	if(err) return err;
	
	err = CommonParmetersDump(ROM, presetNumber);
	if(err) return err;
	chkstep(progress)
	
	OutputDebugString("Loading Arpeggiator Parameters\n");
	err = ArpeggiatorParmetersDump(ROM, presetNumber);
	if(err) return err;
	chkstep(progress)
	
	OutputDebugString("Loading Effects Parameters\n");
	err = EffectsParmetersDump(ROM, presetNumber);
	if(err) return err;
	chkstep(progress)
	
	OutputDebugString("Loading Link Parameters\n");
	err = LinkParmetersDump(ROM, presetNumber);
	if(err) return err;
	chkstep(progress)
	
	for(int i = 0; i < 4; i++)
	{
		char buff[50];
		sprintf(buff, "Loading Layer %i Parameters\n", i+1);
		OutputDebugString(buff);

		err = LayerParmetersDump(ROM, presetNumber, i);
		if(err) return err;
		chkstep(progress)
		
		err = LayerFiltersDump(ROM, presetNumber, i);
		if(err) return err;
		chkstep(progress)
		
		err = LayerLFODump(ROM, presetNumber, i);
		if(err) return err;
		chkstep(progress)
		
		err = LayerEnvelopeDump(ROM, presetNumber, i);
		if(err) return err;
		chkstep(progress)
		
		err = LayerPatchcordDump(ROM, presetNumber, i);
		if(err) return err;
		chkstep(progress)
	}

	err = SetParameter(PRESET_SELECT, presetNumber);
	if(err) return err;

	err = SetParameter(LAYER_SELECT, 0);
	if(err) return err;
	
	return err;
}

long CProteus::DownloadUserPresetNames(IProgressCallback* progress)
{
	MMRESULT err = S_OK;

	progress->SetTotalSteps(mNumUserPresets);

	for(int i = 0; i < mNumUserPresets; i++)
	{
		if(i > MAX_PRESETS)
			break;

		err = NameRequest(0, i, eObjectTypePreset, &mPresetNames[i][0]);
		if(err) return err;

		chkstep(progress)

#ifdef _DEBUG
		char buff[NAME_LENGTH+2];
		memcpy(buff, &mPresetNames[i][0], NAME_LENGTH);
		buff[NAME_LENGTH] = '\n';
		buff[NAME_LENGTH+1] = '\0';
		OutputDebugString(buff);
#endif

		if(memcmp(kNotInstalled, &mPresetNames[i][0], strlen(kNotInstalled)) == 0)
		{
			mNumUserPresets = i;
			break;
		}
	}
	
	return err;
}

long CProteus::DownloadROMPresetNames(short ROM, IProgressCallback* progress)
{
	short ROMIdx = GetSimmIdx(ROM);
	if(ROMIdx == -1)
		return E_FAIL;
	
	MMRESULT err = S_OK;

	progress->SetTotalSteps(mSimms[ROMIdx]->mNumPresets);
	
	for(int i = 0; i < mSimms[ROMIdx]->mNumPresets; i++)
	{
		if(i > MAX_PRESETS)
			break;

		err = NameRequest(ROM, i, eObjectTypePreset, &mSimms[ROMIdx]->mPresetNames[i][0]);
		if(err) return err;

		chkstep(progress)

#ifdef _DEBUG
		char buff[NAME_LENGTH+2];
		memcpy(buff, &mSimms[ROMIdx]->mPresetNames[i][0], NAME_LENGTH);
		buff[NAME_LENGTH] = '\n';
		buff[NAME_LENGTH+1] = '\0';
		OutputDebugString(buff);
#endif

		if(memcmp(kNotInstalled, &mSimms[ROMIdx]->mPresetNames[i][0], strlen(kNotInstalled)) == 0 ||
			memcmp(kSSSSSSSSSSSSSSSS, &mSimms[ROMIdx]->mPresetNames[i][0], strlen(kSSSSSSSSSSSSSSSS)) == 0)
		{
			mSimms[ROMIdx]->mNumPresets = i;
			break;
		}
	}

	mSimms[ROMIdx]->mLoadedPresets = true;
	
	return err;
}

long CProteus::DownloadInstrumentNames(IProgressCallback* progress)
{
	MMRESULT err = S_OK;

	int totalInstruments = 0;
	for(int j = 0; j < MAX_SIMMS; j++)
	{
		if(!mSimms[j])
			continue;
		totalInstruments += mSimms[j]->mNumInstruments;
	}

	progress->SetTotalSteps(totalInstruments);
	
	for(int ROM = 0; ROM < MAX_SIMMS; ROM++)
	{
		if(!mSimms[ROM])
			continue;

		for(int i = 0; i < mSimms[ROM]->mNumInstruments; i++)
		{
			if(i > MAX_INSTRUMENTS)
				break;
			
			err = NameRequest(mSimms[ROM]->mSimmID, i, eObjectTypeInstrument, &mSimms[ROM]->mInstrumentNames[i][0]);
			if(err) return err;
			
			chkstep(progress)
				
#ifdef _DEBUG
			char buff[NAME_LENGTH+2];
			memcpy(buff, &mSimms[ROM]->mInstrumentNames[i][0], NAME_LENGTH);
			buff[NAME_LENGTH] = '\n';
			buff[NAME_LENGTH+1] = '\0';
			OutputDebugString(buff);
#endif
			
			if(memcmp(kNotInstalled, &mSimms[ROM]->mInstrumentNames[i][0], strlen(kNotInstalled)) == 0 ||
				memcmp(kSSSSSSSSSSSSSSSS, &mSimms[ROM]->mInstrumentNames[i][0], strlen(kSSSSSSSSSSSSSSSS)) == 0)
			{
				mSimms[ROM]->mNumInstruments = i;
				break;
			}
		}
	}	
	
	return err;
}

long CProteus::LoadUserPresetNames(const char* fileName)
{
	HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!file || file == INVALID_HANDLE_VALUE)
		return E_FAIL;

	int i = 0;
	char buffer[NAME_LENGTH+2];
	DWORD bytesRead = 0;
	while(ReadFile(file, buffer, NAME_LENGTH+2, &bytesRead, NULL) && bytesRead)
	{
		if(i >= MAX_PRESETS)
			break;

		memcpy(&mPresetNames[i][0], buffer, NAME_LENGTH);
		i++;
	}

	mNumUserPresets = i;

	CloseHandle(file);

	return S_OK;
}

long CProteus::LoadROMPresetNames(const char* fileName, short ROM)
{
	short ROMIdx = GetSimmIdx(ROM);
	if(ROMIdx == -1)
		return E_FAIL;
	
	HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!file || file == INVALID_HANDLE_VALUE)
	{
		mSimms[ROMIdx]->mLoadedPresets = false;
		return E_FAIL;
	}
	
	int i = 0;
	char buffer[NAME_LENGTH+2];
	DWORD bytesRead = 0;
	while(ReadFile(file, buffer, NAME_LENGTH+2, &bytesRead, NULL) && bytesRead)
	{
		if(i >= MAX_PRESETS)
			break;

		memcpy(&mSimms[ROMIdx]->mPresetNames[i][0], buffer, NAME_LENGTH);
		i++;
	}

	mSimms[ROMIdx]->mNumPresets = i;
	mSimms[ROMIdx]->mLoadedPresets = true;
	
	CloseHandle(file);

	return S_OK;
}

long CProteus::LoadInstrumentNames(const char* fileName)
{
	HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!file || file == INVALID_HANDLE_VALUE)
		return E_FAIL;
	
	int i = 0;
	char buffer[NAME_LENGTH+2];
	DWORD bytesRead = 0;
	int ROM = 0;

	while(ReadFile(file, buffer, NAME_LENGTH+2, &bytesRead, NULL) && bytesRead)
	{
		if(i >= MAX_PRESETS)
			break;
		
		if(memcmp(buffer, "****NEXTROM*****", NAME_LENGTH) == 0)
		{
			mSimms[ROM]->mNumInstruments = i;
			i = 0;
			ROM++;
			if(!mSimms[ROM])
			{
				CloseHandle(file);
				return S_OK;
			}
		}
		else
		{
			memcpy(&mSimms[ROM]->mInstrumentNames[i][0], buffer, NAME_LENGTH);
			i++;
		}
	}
	
	CloseHandle(file);
	
	return S_OK;
}

long CProteus::SaveUserPresetNames(const char* fileName)
{
	HANDLE file = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!file || file == INVALID_HANDLE_VALUE)
		return E_FAIL;

	DWORD written = 0;
	char buffer[NAME_LENGTH + 2];

	for(int i = 0; i < mNumUserPresets; i++)
	{
		memcpy(buffer, &mPresetNames[i][0], NAME_LENGTH);
		buffer[NAME_LENGTH] = '\r';
		buffer[NAME_LENGTH+1] = '\n';
		WriteFile(file, buffer, NAME_LENGTH+2, &written, NULL);
	}

	CloseHandle(file);
	
	return S_OK;
}

long CProteus::SaveROMPresetNames(const char* fileName, short ROM)
{
	short ROMIdx = GetSimmIdx(ROM);
	if(ROMIdx == -1)
		return E_FAIL;

	HANDLE file = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!file || file == INVALID_HANDLE_VALUE)
		return E_FAIL;
	
	DWORD written = 0;
	char buffer[NAME_LENGTH + 2];
	
	for(int i = 0; i < mSimms[ROMIdx]->mNumPresets; i++)
	{
		memcpy(buffer, &mSimms[ROMIdx]->mPresetNames[i][0], NAME_LENGTH);
		buffer[NAME_LENGTH] = '\r';
		buffer[NAME_LENGTH+1] = '\n';
		WriteFile(file, buffer, NAME_LENGTH+2, &written, NULL);
	}
	
	CloseHandle(file);

	return S_OK;
}

long CProteus::SaveInstrumentNames(const char* fileName)
{
	HANDLE file = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!file || file == INVALID_HANDLE_VALUE)
		return E_FAIL;
	
	DWORD written = 0;
	char buffer[NAME_LENGTH + 2];

	for(int ROM = 0; ROM < MAX_SIMMS; ROM++)
	{
		if(!mSimms[ROM])
			continue;

		for(int i = 0; i < mSimms[ROM]->mNumInstruments; i++)
		{
			memcpy(buffer, &mSimms[ROM]->mInstrumentNames[i][0], NAME_LENGTH);
			buffer[NAME_LENGTH] = '\r';
			buffer[NAME_LENGTH+1] = '\n';
			WriteFile(file, buffer, NAME_LENGTH+2, &written, NULL);
		}

		WriteFile(file, "****NEXTROM*****\r\n", NAME_LENGTH+2, &written, NULL);
	}
	
	CloseHandle(file);

	return S_OK;
}

long CProteus::LoadMasterParameters()
{
	return GetParameter(MASTER_CLOCK_TEMPO, &mMasterParameters.MasterClockTempo);
}

short CProteus::GetSimmIdx(short simmID)
{
	for(int i = 0; i < MAX_SIMMS; i++)
	{
		if(mSimms[i])
		{
			if(mSimms[i]->mSimmID == simmID)
				return i;
		}
	}
	return -1;
}


void CALLBACK MidiOutProcCB(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if(dwInstance)
	{
		CProteus* thisPtr = (CProteus*) dwInstance;
		thisPtr->MidiOutProc(hMidiOut, wMsg, dwParam1, dwParam2);
	}
}
void CALLBACK MidiInProcCB(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if(dwInstance)
	{
		CProteus* thisPtr = (CProteus*) dwInstance;
		thisPtr->MidiInProc(hMidiIn, wMsg, dwParam1, dwParam2);
	}
}

void CALLBACK MidiThruProcCB(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if(dwInstance)
	{
		CProteus* thisPtr = (CProteus*) dwInstance;
		thisPtr->MidiThruProc(hMidiIn, wMsg, dwParam1, dwParam2);
	}
}

void CMidiDeviceInfo::Initialize(const char* optionsPath)
{
	NumInDevices = midiInGetNumDevs();
	NumOutDevices = midiOutGetNumDevs();

	SelectedInDeviceId = -1;
	SelectedOutDeviceId = -1;
	SelectedThruDeviceId = -1;
	SelectedSysExId = 0;

	char savedInName[MAXPNAMELEN+1];
	char savedOutName[MAXPNAMELEN+1];
	char savedThruName[MAXPNAMELEN+1];
	GetPrivateProfileString("MidiSetup", "InDevice", "", savedInName, MAXPNAMELEN, optionsPath);
	GetPrivateProfileString("MidiSetup", "OutDevice", "", savedOutName, MAXPNAMELEN, optionsPath);
	GetPrivateProfileString("MidiSetup", "ThruDevice", "", savedThruName, MAXPNAMELEN, optionsPath);

	MMRESULT err = 0;
	
	OutputDebugString("*** In devices ***\n");
	for(UINT i = 0; i < NumInDevices; i++)
	{
		MIDIINCAPS incaps;
		err = midiInGetDevCaps(i, &incaps, sizeof(incaps));
		if(err) { wxASSERT(0); continue; }

		OutputDebugString(incaps.szPname);
		OutputDebugString("\n");

		memset(&InDevices[i][0], 0, MAXPNAMELEN);
		memcpy(&InDevices[i][0], incaps.szPname, strlen(incaps.szPname));

		if(strcmp(incaps.szPname, savedInName) == 0)
		{
			SelectedInDeviceId = i;
		}
		if(strcmp(incaps.szPname, savedThruName) == 0)
		{
			SelectedThruDeviceId = i;
		}
	}
	
	OutputDebugString("*** Out devices ***\n");
	for(UINT j = 0; j < NumOutDevices; j++)
	{
		MIDIOUTCAPS outcaps;
		err = midiOutGetDevCaps(j, &outcaps, sizeof(outcaps));
		if(err) { wxASSERT(0); continue; }

		OutputDebugString(outcaps.szPname);
		OutputDebugString("\n");

		memset(&OutDevices[j][0], 0, MAXPNAMELEN);
		memcpy(&OutDevices[j][0], outcaps.szPname, strlen(outcaps.szPname));

		if(strcmp(outcaps.szPname, savedOutName) == 0)
		{
			SelectedOutDeviceId = j;
		}
	}

	char sysExStr[10];
	GetPrivateProfileString("MidiSetup", "SysExId", "0", sysExStr, 9, optionsPath);
	SelectedSysExId = atoi(sysExStr);
}

void Nibbleize(unsigned char* buffer, short val)
{
	if(val < 0)
		val += 0x4000;
	
	buffer[0] = val % 0x80;
	buffer[1] = val / 0x80;
}

void UnNibbleize(short& val)
{
	short newVal = 0;
	newVal += (val >> 8) * 128;
	newVal += (val & 0xFF);
	if(newVal > 0x2000)
		newVal -= 0x4000;
	val = newVal;
}

CScopeTimer::CScopeTimer(const char* scopeName)
{
	mStartTime = GetTickCount();
	mScopeName = scopeName;
}

CScopeTimer::~CScopeTimer()
{
	DWORD totalTime = GetTickCount() - mStartTime;
	char buffer[200];
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "Scope Timer: %s\n%i milliseconds\n", mScopeName, totalTime);
	OutputDebugString(buffer);
}
