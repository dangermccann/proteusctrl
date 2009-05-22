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

#ifndef _CEvents_h_
#define _CEvents_h_

enum
{
	eMenuExit,
	eMenuDownloadPresets,
	eMenuDownloadInstruments,
	eMenuMidiSetup,
	eMenuConnect,
	eMenuDisconnect,
	eMenuHelpAbout,
	eMenuHelpReadme,
	eMenuReload,
	eStepEvent,
	eSetStepsEvent,
	eLoadPresetComplete,
	eDownloadPresetsComplete,
	eDownloadInstrumentsComplete,
	eRTControllerChange,

	eMenuDownloadUserPresets,

	eMenuDownloadROMPresets		= 1000,
	eMenuDownloadROMPresetsEnd	= 2000,
};

enum
{
	eEditorPanel,
	eTreeCtrlID = 1,

	eNotebook,
	eSaveAs,
	
	ePresetNameText1,
	ePresetNameText2,
	eInstrument,
	eInstrumentROM,
	eVolume,
	ePan,

	eKeyLow,
	eKeyLowFade,
	eKeyHigh,
	eKeyHighFade,
	eVelocityLow,
	eVelocityLowFade,
	eVelocityHigh,
	eVelocityHighFade,
	eRTLow,
	eRTLowFade,
	eRTHigh,
	eRTHighFade,

	eTranspose,
	eTuningCoarse,
	eTuningFine,

	eFilter,
	eFilterFreq,
	eFilterRes,

	eLFO1Shape,
	eLFO1Rate,
	eLFO1Sync,
	eLFO1Delay,
	eLFO1Var,

	eLFO2Shape,
	eLFO2Rate,
	eLFO2Sync,
	eLFO2Delay,
	eLFO2Var,

	eVelEnvMode,
	eVelEnvAttack1,
	eVelEnvAttack2,
	eVelEnvDecay1,
	eVelEnvDecay2,
	eVelEnvRelease1,
	eVelEnvRelease2,
	eVelEnvAttack1Rt,
	eVelEnvAttack2Rt,
	eVelEnvDecay1Rt,
	eVelEnvDecay2Rt,
	eVelEnvRelease1Rt,
	eVelEnvRelease2Rt,

	eFilEnvMode,
	eFilEnvAttack1,
	eFilEnvAttack2,
	eFilEnvDecay1,
	eFilEnvDecay2,
	eFilEnvRelease1,
	eFilEnvRelease2,
	eFilEnvAttack1Rt,
	eFilEnvAttack2Rt,
	eFilEnvDecay1Rt,
	eFilEnvDecay2Rt,
	eFilEnvRelease1Rt,
	eFilEnvRelease2Rt,

	eAuxEnvMode,
	eAuxEnvAttack1,
	eAuxEnvAttack2,
	eAuxEnvDecay1,
	eAuxEnvDecay2,
	eAuxEnvRelease1,
	eAuxEnvRelease2,
	eAuxEnvAttack1Rt,
	eAuxEnvAttack2Rt,
	eAuxEnvDecay1Rt,
	eAuxEnvDecay2Rt,
	eAuxEnvRelease1Rt,
	eAuxEnvRelease2Rt,

	eChorus,
	eChorusWidth,
	eStart,
	eDelay,
	eNonTranspose,
	eSoloMode,
	eAssignGroup,
	eGlide,
	eGlideCurve,

	eFXAAlgId,
	eFXADecay,
	eFXAHFDamp,
	eFXBFXA,
	eFXASendAmt1,
	eFXASendAmt2,
	eFXASendAmt3,
	eFXASendAmt4,

	eFXBAlgId,
	eFXBFeedbk,
	eFXBLFORate,
	eFXBDelay,
	eFXBSendAmt1,
	eFXBSendAmt2,
	eFXBSendAmt3,
	eFXBSendAmt4,

	eRTControllerA,
	eRTControllerB,
	eRTControllerC,
	eRTControllerD,
	eRTControllerE,
	eRTControllerF,
	eRTControllerG,
	eRTControllerH,
	eRTControllerI,
	eRTControllerJ,
	eRTControllerK,
	eRTControllerL,
	eRTControllerSave,

	ePatchcordSources		= 1000,
	ePatchcordDests			= 1100,
	ePatchcordAmts			= 1200,
	ePPatchcordSources		= 2000,
	ePPatchcordDests		= 2100,
	ePPatchcordAmts			= 2200,
	ePatchcordsEnd			= 2300,
};

enum
{
	eStatusNA,
	eStatusDownloadUserPresets,
	eStatusDownloadInstruments,
};

#endif // _CEvents_h_