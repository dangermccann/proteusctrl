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

#ifndef _Commands_h_
#define _Commands_h_

#include <stdio.h>

#define MIDI_SYSEX_DELAY		0x1503
#define MASTER_TRANSPOSE		0x0302

// master parameters
#define	MASTER_CLOCK_TEMPO		0x0102

// commands
#define COMMAND_PARAM_EDIT		0x01
#define COMMAND_HARDWARE_CONFIG 0x09
#define COMMAND_PRESET_DUMP		0x10
#define COMMAND_END_OF_FILE		0x7B
#define COMMAND_LCD_DUMP		0x1A

// preset general commands
#define PRESET_SELECT			0x0107
#define LAYER_SELECT			0x0207
#define PRESET_NAME_CHAR_0		0x0307
#define PRESET_NAME_CHAR_1		0x0407
#define PRESET_NAME_CHAR_2		0x0507
#define PRESET_NAME_CHAR_3		0x0607
#define PRESET_NAME_CHAR_4		0x0707
#define PRESET_NAME_CHAR_5		0x0807
#define PRESET_NAME_CHAR_6		0x0907
#define PRESET_NAME_CHAR_7		0x0A07
#define PRESET_NAME_CHAR_8		0x0B07
#define PRESET_NAME_CHAR_9		0x0C07
#define PRESET_NAME_CHAR_10		0x0D07
#define PRESET_NAME_CHAR_11		0x0E07
#define PRESET_NAME_CHAR_12		0x0F07
#define PRESET_NAME_CHAR_13		0x1007
#define PRESET_NAME_CHAR_14		0x1107
#define PRESET_NAME_CHAR_15		0x1207
#define PRESET_CTRL_A			0x1307
#define PRESET_CTRL_B			0x1407
#define PRESET_CTRL_C			0x1507
#define PRESET_CTRL_D			0x1607
#define PRESET_CTRL_E			0x1707
#define PRESET_CTRL_F			0x1807
#define PRESET_CTRL_G			0x1907
#define PRESET_CTRL_H			0x1A07
#define PRESET_KBD_TUNE			0x1B07
#define PRESET_CTRL_I			0x1C07
#define PRESET_CTRL_J			0x1D07
#define PRESET_CTRL_K			0x1E07
#define PRESET_CTRL_L			0x1F07
#define PRESET_RIFF				0x2007
#define PRESET_RIFF_ROM_ID		0x2107
#define PRESET_TEMPO_OFFSET		0x2207
#define PRESET_CORD_0_SOURCE	0x2307
#define PRESET_CORD_0_DEST		0x2407
#define PRESET_CORD_0_AMOUNT	0x2507
#define PRESET_CORD_1_SOURCE	0x2607
#define PRESET_CORD_1_DEST		0x2707
#define PRESET_CORD_1_AMOUNT	0x2807
#define PRESET_CORD_2_SOURCE	0x2907
#define PRESET_CORD_2_DEST		0x2A07
#define PRESET_CORD_2_AMOUNT	0x2B07
#define PRESET_CORD_3_SOURCE	0x2C07
#define PRESET_CORD_3_DEST		0x2D07
#define PRESET_CORD_3_AMOUNT	0x2E07
#define PRESET_CORD_4_SOURCE	0x2F07
#define PRESET_CORD_4_DEST		0x3007
#define PRESET_CORD_4_AMOUNT	0x3107
#define PRESET_CORD_5_SOURCE	0x3207
#define PRESET_CORD_5_DEST		0x3307
#define PRESET_CORD_5_AMOUNT	0x3407
#define PRESET_CORD_6_SOURCE	0x3507
#define PRESET_CORD_6_DEST		0x3607
#define PRESET_CORD_6_AMOUNT	0x3707
#define PRESET_CORD_7_SOURCE	0x3807
#define PRESET_CORD_7_DEST		0x3907
#define PRESET_CORD_7_AMOUNT	0x3A07
#define PRESET_CORD_8_SOURCE	0x3B07
#define PRESET_CORD_8_DEST		0x3C07
#define PRESET_CORD_8_AMOUNT	0x3D07
#define PRESET_CORD_9_SOURCE	0x3E07
#define PRESET_CORD_9_DEST		0x3F07
#define PRESET_CORD_9_AMOUNT	0x4007
#define PRESET_CORD_10_SOURCE	0x4107
#define PRESET_CORD_10_DEST		0x4207
#define PRESET_CORD_10_AMOUNT	0x4307
#define PRESET_CORD_11_SOURCE	0x4407
#define PRESET_CORD_11_DEST		0x4507
#define PRESET_CORD_11_AMOUNT	0x4607

// Arpeggiator commands
#define PRESET_ARP_STATUS		0x0108
#define PRESET_ARP_MODE			0x0208
#define PRESET_ARP_PATTERN		0x0308
#define PRESET_ARP_NOTE			0x0408
#define PRESET_ARP_VEL			0x0508
#define PRESET_ARP_GATE_TIME	0x0608
#define PRESET_ARP_EXT_COUNT	0x0708
#define PRESET_ARP_EXT_INT		0x0808
#define PRESET_ARP_SYNC			0x0908
#define PRESET_ARP_PRE_DELAY	0x0A08
#define PRESET_ARP_DURATION		0x0B08
#define PRESET_ARP_RECYCLE		0x0C08
#define PRESET_ARP_KBD_THRU		0x0D08
#define PRESET_ARP_LATCH		0x0E08
#define PRESET_ARP_KR_LOW		0x0F08
#define PRESET_ARP_KR_HIGH		0x1008

// effects commands
#define PRESET_FX_A_ALGORITHM	0x0109
#define PRESET_FX_A_DECAY		0x0209
#define PRESET_FX_A_HFDAMP		0x0309
#define PRESET_FX_AB			0x0409
#define PRESET_FX_A_MIX_MAIN	0x0509
#define PRESET_FX_A_MIX_SUB1	0x0609
#define PRESET_FX_A_MIX_SUB2	0x0709
#define PRESET_FX_B_ALGORITHM	0x0809
#define PRESET_FX_B_FEEDBACK	0x0909
#define PRESET_FX_B_LFO_RATE	0x0A09
#define PRESET_FX_B_DELAY		0x0B09
#define PRESET_FX_B_MIX_MAIN	0x0C09
#define PRESET_FX_B_MIX_SUB1	0x0D09
#define PRESET_FX_B_MIX_SUB2	0x0E09
#define PRESET_FX_B_MIX_SUB3	0x0F09
#define PRESET_FX_B_MIX_SUB4	0x1009

// link commands
#define LINK_1_PRESET			0x010A
#define LINK_1_VOLUME			0x020A
#define LINK_1_PAN				0x030A
#define LINK_1_TRANSPOSE		0x040A
#define LINK_1_DELAY			0x050A
#define LINK_1_KEY_LOW			0x060A
#define LINK_1_KEY_HIGH			0x070A
#define LINK_1_VEL_LOW			0x080A
#define LINK_1_VEL_HIGH			0x090A
#define LINK_2_PRESET			0x0A0A
#define LINK_2_VOLUME			0x0B0A
#define LINK_2_PAN				0x0C0A
#define LINK_2_TRANSPOSE		0x0D0A
#define LINK_2_DELAY			0x0E0A
#define LINK_2_KEY_LOW			0x0F0A
#define LINK_2_KEY_HIGH			0x100A
#define LINK_2_VEL_LOW			0x110A
#define LINK_2_VEL_HIGH			0x120A
#define LINK_1_PRESET_ROM_ID	0x130A
#define LINK_2_PRESET_ROM_ID	0x140A

// layer commands
#define LAYER_INSTRUMENT		0x010B
#define LAYER_VOLUME			0x020B
#define LAYER_PAN				0x030B
#define LAYER_SUBMIX			0x040B
#define LAYER_KEY_LOW			0x050B
#define LAYER_KEY_LOWFADE		0x060B
#define LAYER_KEY_HIGH			0x070B
#define LAYER_KEY_HIGHFADE		0x080B
#define LAYER_VEL_LOW			0x090B
#define LAYER_VEL_LOWFADE		0x0A0B
#define LAYER_VEL_HIGH			0x0B0B
#define LAYER_VEL_HIGHFADE		0x0C0B
#define LAYER_RT_LOW			0x0D0B
#define LAYER_RT_LOWFADE		0x0E0B
#define LAYER_RT_HIGH			0x0F0B
#define LAYER_RT_HIGHFADE		0x100B
#define LAYER_CTUNE				0x110B
#define LAYER_FTUNE				0x120B
#define LAYER_DBL_DETUNE		0x130B
#define LAYER_DBL_DETUNE_WIDTH	0x140B
#define LAYER_TRANSPOSE			0x150B
#define LAYER_NON_TRANSPOSE		0x160B
#define LAYER_BEND				0x170B
#define LAYER_GLIDE_RATE		0x180B
#define LAYER_GLIDE_CURVE		0x190B
#define LAYER_LOOP				0x1A0B
#define LAYER_START_DELAY		0x1B0B
#define LAYER_START_OFFSET		0x1C0B
#define LAYER_SOLO				0x1D0B
#define LAYER_GROUP				0x1E0B
#define LAYER_INST_ROM_ID		0x1F0B

// filter commands
#define LAYER_FILT_TYPE			0x010C
#define LAYER_FILT_FREQ			0x020C
#define LAYER_FILT_Q			0x030C

// LFO commands
#define LAYER_LFO1_RATE			0x010D
#define LAYER_LFO1_SHAPE		0x020D
#define LAYER_LFO1_DELAY		0x030D
#define LAYER_LFO1_VAR			0x040D
#define LAYER_LFO1_SYNC			0x050D
#define LAYER_LFO2_RATE			0x060D
#define LAYER_LFO2_SHAPE		0x070D
#define LAYER_LFO2_DELAY		0x080D
#define LAYER_LFO2_VAR			0x090D
#define LAYER_LFO2_SYNC			0x0A0D

// envelope commands
#define LAYER_VOL_ENV_MODE		0x010E
#define LAYER_VENV_ATK1_RATE	0x020E
#define LAYER_VENV_ATK1_LVL		0x030E
#define LAYER_VENV_DCY1_RATE	0x040E
#define LAYER_VENV_DCY1_LVL		0x050E
#define LAYER_VENV_RLS1_RATE	0x060E
#define LAYER_VENV_RLS1_LVL		0x070E
#define LAYER_VENV_ATK2_RATE	0x080E
#define LAYER_VENV_ATK2_LVL		0x090E
#define LAYER_VENV_DCY2_RATE	0x0A0E
#define LAYER_VENV_DCY2_LVL		0x0B0E
#define LAYER_VENV_RLS2_RATE	0x0C0E
#define LAYER_VENV_RLS2_LVL		0x0D0E
#define LAYER_FILT_ENV_MODE		0x0E0E
#define LAYER_FENV_ATK1_RATE	0x0F0E
#define LAYER_FENV_ATK1_LVL		0x100E
#define LAYER_FENV_DCY1_RATE	0x110E
#define LAYER_FENV_DCY1_LVL		0x120E
#define LAYER_FENV_RLS1_RATE	0x130E
#define LAYER_FENV_RLS1_LVL		0x140E
#define LAYER_FENV_ATK2_RATE	0x150E
#define LAYER_FENV_ATK2_LVL		0x160E
#define LAYER_FENV_DCY2_RATE	0x170E
#define LAYER_FENV_DCY2_LVL		0x180E
#define LAYER_FENV_RLS2_RATE	0x190E
#define LAYER_FENV_RLS2_LVL		0x1A0E
#define LAYER_AUX_ENV_MODE		0x1B0E
#define LAYER_AENV_ATK1_RATE	0x1C0E
#define LAYER_AENV_ATK1_LVL		0x1D0E
#define LAYER_AENV_DCY1_RATE	0x1E0E
#define LAYER_AENV_DCY1_LVL		0x1F0E
#define LAYER_AENV_RLS1_RATE	0x200E
#define LAYER_AENV_RLS1_LVL		0x210E
#define LAYER_AENV_ATK2_RATE	0x220E
#define LAYER_AENV_ATK2_LVL		0x230E
#define LAYER_AENV_DCY2_RATE	0x240E
#define LAYER_AENV_DCY2_LVL		0x250E
#define LAYER_AENV_RLS2_RATE	0x260E
#define LAYER_AENV_RLS2_LVL		0x270E
#define LAYER_FENV_REPEAT		0x290E
#define LAYER_AUXENV_REPEAT		0x2A0E

// patchcord commands
#define LAYER_CORD0_SRC			0x010F
#define LAYER_CORD0_DST			0x020F
#define LAYER_CORD0_AMT			0x030F
#define LAYER_CORD1_SRC			0x040F
#define LAYER_CORD1_DST			0x050F
#define LAYER_CORD1_AMT			0x060F
#define LAYER_CORD2_SRC			0x070F
#define LAYER_CORD2_DST			0x080F
#define LAYER_CORD2_AMT			0x090F
#define LAYER_CORD3_SRC			0x0A0F
#define LAYER_CORD3_DST			0x0B0F
#define LAYER_CORD3_AMT			0x0C0F
#define LAYER_CORD4_SRC			0x0D0F
#define LAYER_CORD4_DST			0x0E0F
#define LAYER_CORD4_AMT			0x0F0F
#define LAYER_CORD5_SRC			0x100F
#define LAYER_CORD5_DST			0x110F
#define LAYER_CORD5_AMT			0x120F
#define LAYER_CORD6_SRC			0x130F
#define LAYER_CORD6_DST			0x140F
#define LAYER_CORD6_AMT			0x150F
#define LAYER_CORD7_SRC			0x160F
#define LAYER_CORD7_DST			0x170F
#define LAYER_CORD7_AMT			0x180F
#define LAYER_CORD8_SRC			0x190F
#define LAYER_CORD8_DST			0x1A0F
#define LAYER_CORD8_AMT			0x1B0F
#define LAYER_CORD9_SRC			0x1C0F
#define LAYER_CORD9_DST			0x1D0F
#define LAYER_CORD9_AMT			0x1E0F
#define LAYER_CORD10_SRC		0x1F0F
#define LAYER_CORD10_DST		0x200F
#define LAYER_CORD10_AMT		0x210F
#define LAYER_CORD11_SRC		0x220F
#define LAYER_CORD11_DST		0x230F
#define LAYER_CORD11_AMT		0x240F
#define LAYER_CORD12_SRC		0x250F
#define LAYER_CORD12_DST		0x260F
#define LAYER_CORD12_AMT		0x270F
#define LAYER_CORD13_SRC		0x280F
#define LAYER_CORD13_DST		0x290F
#define LAYER_CORD13_AMT		0x2A0F
#define LAYER_CORD14_SRC		0x2B0F
#define LAYER_CORD14_DST		0x2C0F
#define LAYER_CORD14_AMT		0x2D0F
#define LAYER_CORD15_SRC		0x2E0F
#define LAYER_CORD15_DST		0x2F0F
#define LAYER_CORD15_AMT		0x300F
#define LAYER_CORD16_SRC		0x310F
#define LAYER_CORD16_DST		0x320F
#define LAYER_CORD16_AMT		0x330F
#define LAYER_CORD17_SRC		0x340F
#define LAYER_CORD17_DST		0x350F
#define LAYER_CORD17_AMT		0x360F
#define LAYER_CORD18_SRC		0x370F
#define LAYER_CORD18_DST		0x380F
#define LAYER_CORD18_AMT		0x390F
#define LAYER_CORD19_SRC		0x3A0F
#define LAYER_CORD19_DST		0x3B0F
#define LAYER_CORD19_AMT		0x3C0F
#define LAYER_CORD20_SRC		0x3D0F
#define LAYER_CORD20_DST		0x3E0F
#define LAYER_CORD20_AMT		0x3F0F
#define LAYER_CORD21_SRC		0x400F
#define LAYER_CORD21_DST		0x410F
#define LAYER_CORD21_AMT		0x420F
#define LAYER_CORD22_SRC		0x430F
#define LAYER_CORD22_DST		0x440F
#define LAYER_CORD22_AMT		0x450F
#define LAYER_CORD23_SRC		0x460F
#define LAYER_CORD23_DST		0x470F
#define LAYER_CORD23_AMT		0x480F

#define EOX 0xF7

// real-time controller messages
#define RT_MSG_A				0x5F
#define RT_MSG_B				0x16
#define RT_MSG_C				0x17
#define RT_MSG_D				0x18
#define RT_MSG_E				0x19
#define RT_MSG_F				0x1A
#define RT_MSG_G				0x1B
#define RT_MSG_H				0x1C
#define RT_MSG_I				0x4E
#define RT_MSG_J				0x4F
#define RT_MSG_K				0x5B
#define RT_MSG_L				0x5D

static short kPPatchcordSourceMap [] =
{
	0, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
	32, 33, 34, 35, 36, 37, 40, 41, 42, 43, 160
};

// preset patchcord sources
static char* kPresetPatchcordSources[] = 
{
	"Off",	 //								0
	"PitWl", // (Pitch Wheel)				16
	"ModWl", // (Mod Wheel)					17
	"Press", // (Pressure)					...
	"Pedal",
	"MidiA",
	"MidiB",
	"FtSw1", //  (Foot Switch 1)
	"FtSw2", //  (Foot Switch 2)
	"Ft1FF", //  (FlipFlop FootSwitch 1)
	"Ft2FF", //  (FlipFlop FootSwitch 2)
	"MidiVl",//  (Volume controller 7)
	"MidPn", //  (Pan controller 10)		27
	"MidiC", //								32
	"MidiD", //								33
	"MidiE",
	"MidiF",
	"MidiG",
	"MidiH", //								37
	"MidiI", //								40
	"MidiJ",
	"MidiK",
	"MidiL", //								43
	"DC"	 //  (DC Offset)				160
};

static short kPPatchcordDestMap [] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8,
	96, 97, 98, 99, 100, 112, 113, 114, 115,
	116, 117, 118, 119, 120, 121, 128, 129, 131
};


// preset patchcord destinations
static char* kPresetPatchcordDests[] = 
{
	"Off",			//								0
	"FX_A_Send 1",	//								1
	"FX_A_Send 2",
	"FX_A_Send 3",
	"FX_A_Send 4",
	"FX_B_Send 1",
	"FX_B_Send 2",
	"FX_B_Send 3",
	"FX_B_Send 4",	//								8
	"ArpRate",		// (Arpeggiator Rate)			96
	"ArpExten",		// (Arpeggiator Extension)		97
	"ArpVel",		// (Arpeggiator Velocity)
	"ArpGate",		// (Arpeggiator Gate)
	"ArpIntvl",		// (Arpeggiator Interval)		100
	"BeatsVelG1",	// (Beats Velocity Group 1)		112
	"BeatsVelG2",	// (Beats Velocity Group 2)		113
	"BeatsVelG3",	// (Beats Velocity Group 3)
	"BeatsVelG4",	// (Beats Velocity Group 4)
	"BeatsXpsG1",	// (Beats Transpose Grp 1)
	"BeatsXpsG2",	// (Beats Transpose Grp 2)
	"BeatsXpsG3",	// (Beats Transpose Grp 3)
	"BeatsXpsG4",	// (Beats Transpose Grp 4)
	"BeatsBusy",	// (Beats Busy)
	"BeatsVari",	// (Beats Variation)			121
	"PLagIn",		// (Preset Lag In)				128
	"PLagAmt",		// (Preset Lag Amount)			129
	"PRampRt"		// (Preset Ramp Rate)			131
};

static char* kPresetKbdTune[] = 
{
	"equal",			// 0
	"Just C",
	"Valloti",
	"19-Tone",
	"Gamelan",
	"Just C2",
	"Just C-minor",
	"Just C3",
	"Werkmeister III",
	"Kirnberger",
	"Scarlatti",
	"Repeating Octave",	// 11
	"User"				// 12 - 23
};

static char* kTempOffsets[] = 
{
	"x1/4",		// 0
	"x1/2",		// 1
	"x1",
	"x2",
	"x4"		// 4
};

static char* kArpeggiatorModes[] =
{
	"up",						// 0
	"down",						// 1
	"up/down",
	"forward assign",
	"backward assign",
	"forward/backward assign",
	"random",
	"pattern"					// 7
};

static char* kArpeggiatorPatternSpeeds[] =
{
	"4X",		// -2
	"2X",		// -1
	"1X",		// 0
	"1/2X",		// 1
	"1/4X"		// 2
};

static short kPatchcordSourceMap[] =
{
	0, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 26, 27, 28,
	20, 21, 32, 33, 34, 35, 36, 37, 40, 41, 42, 43, 128, 129,
	22, 24, 23, 25, 
	26, 27, /* ???? */
	48, 72, 73, 74, 80, 81, 82, 88, 89, 90, 96, 97, 104, 105, 98, 99,
	95, /* ?? */
	100, 101, 106, 107, 108, 109, 150, 151, 144, 145, 146, 147, 148, 149,
	160, 161, 162, 163, 164, 165, 166, 167
};

static char* kPatchcordSources [] =
{
	"Off",											// 0
	"Key+",			// (Key 0...127)				// 8
	"Key~",			// (Key -64...+63)				// 9
	"Vel+",			// (Velocity 0...127)
	"Vel~",			// (Velocity -64...+63)
	"Vel<",			// (Velocity -127...0)			// 12
	"RlsVel",		// (Release Velocity)			// 13
	"Gate",											// 14
	"PitchWl",		// (Pitch Wheel)				// 16
	"ModWl",		// (Mod Wheel)					// 17
	"Press",		// (Pressure)					// 18
	"Pedal",		// (Pedal)						// 19
	"MidiVol",		// (Midi Volume controller 7)	// 26
	"MidPan",		// (Midi Pan controller 10)		// 27
	"MidiExpr",										// ?? 28 ??
	"MidiA",										// 20
	"MidiB",										// 21
	"MidiC",										// 32
	"MidiD",
	"MidiE",
	"MidiF",
	"MidiG",
	"MidiH",										// 37
	"MidiI",										// 40
	"MidiJ",
	"MidiK",
	"MidiL",										// 43
	"PLagOut",										// 128
	"PRampOut",										// 129
	"FootSw1",		// (Foot Switch 1)				// 22
	"Foot1FF",										// 24
	"FootSw2",		// (Foot Switch 2)				// 23
	"Foot2FF",										// 25
	"FootSw3",		// (Foot Switch 2)				// ??
	"Foot3FF",										// ??
	"KeyGlide",		// (Key Glide)					// 48
	"VEnv+",		// (Volume Envelope 0...127)	// 72
	"VEnv~",		// (Volume Envelope -64...+63)	// 73
	"VEnv<",		// (Volume Envelope -127...0)	// 74
	"FEnv+",		// (Filter Envelope 0...127)	// 80
	"FEnv~",		// (Filter Envelope -64...+63)  // 81
	"FEnv<",		// (Filter Envelope -127...0)	// 82
	"AEnv+",		// (Aux Envelope 0...127)		// 88
	"AEnv~",		// (Aux Envelope -64...+63)		// 89
	"AEnv<",		// (Aux Envelope -127...0)		// 90
	"Lfo1+",										// 96	(backwards?)
	"Lfo1~",										// 97
	"Lfo2+",										// 104
	"Lfo2~",										// 105
	"White",		// (White Noise)				// 98
	"Pink",			// (Pink Noise)					// 99
	"XfdRand",										// ??
	"KeyRand1",										// 100
	"KeyRand2",										// 101
	"Lag0sum",										// 106
	"Lag0",											// 107
	"Lag1sum",										// 108
	"Lag1",											// 109
	"ClkOctal",										// 150
	"ClkQuad",										// 151
	"ClkDwhl",										// 144
	"ClkWhole",										// 145
	"ClkHalf",										//
	"ClkQtr",
	"Clk8th",
	"Clk16th",										// 149
	"DC",											// 160
	"Sum",											// 161
	"Switch",
	"Abs",
	"Diode",
	"FlipFlop",
	"Quantize",
	"Gain4X"										// 167
};

static short kPatchcordDestMap[] =
{
	0, 8, 47, 48, 49, 50, 52, 53, 54, 56, 57, 64, 65, 66, 72, 73, 74,
	76, /* ?? */
	75, 80, 81, 82, 
	84, /* ?? */
	83, 88, 89, 90,
	92, /* ?? */
	91, 94, 96, 97,
	104, 105, 106, 108, 161, 162, 163, 164, 165, 166, 167, 168,
	169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
	181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191
};

static char* kPatchcordDests [] =
{
	"Off",
	"KeySust",													// 8
	"FinePtch",													// 47
	"Pitch",
	"Glide",
	"ChrsAmt",					// (Chorus Amount)				// 50
	"‘SStart",					// (Sample Start)				// 52
	"SLoop",					// (Sample Loop)				// 53
	"SRetrig",					// (Sample Retrigger)			// 54
	"FilFreq",					// (Filter Frequency)			// 56
	"‘FilRes",					// (Filter Resonance)			// 57
	"AmpVol",					// (Amplifier Volume)			// 64
	"AmpPan",					// (Amplifier Pan)
	"RTXfade",					// (Amplifier Crossfade)		// 66
	"VEnvRts",					// (Volume Envelope Rates)		// 72
	"VEnvAtk",					// (Volume Envelope Attack)		// 73
	"VEnvDcy",					// (Volume Envelope Decay)		// 74
	"VEnvSus",													// ?? 76 ??
	"VEnvRls",					// (Volume Envelope Release)	// 75
	"FEnvRts",					// (Filter Envelope Rates)		// 80
	"FEnvAtk",					// (Filter Envelope Attack)		// 81
	"FEnvDcy",					// (Filter Envelope Decay)		// 82
	"FEnvSus",													// ??
	"FEnvRls",					// (Filter Envelope Release)	// 83
	"AEnvRts",					// (Aux Envelope Rates)			// 88
	"AEnvAtk",					// (Aux Envelope Attack)		// 89
	"AEnvDcy",					// (Aux Envelope Decay)			// 90
	"AEnvSus",													// ??
	"AEnvRls",					// (Aux Envelope Release)		// 91
	"AEnvTrig",													// 94
	"Lfo1Rate",													// 96
	"Lfo1Trig",													// 97
	"Lfo2Rate",													// 104
	"Lfo2Trig",													// 105
	"Lag0in",													// 106
	"Lag1in",													// 108
	"Sum",						// (Summing Amp)				// 161
	"Switch",
	"Abs",						// (Absolute Value)
	"Diode",
	"FlipFlop",
	"Quantize",
	"Gain4X",
	"C01Amt",					// (Cord Amount)				// 168
	"C02Amt",
	"C03Amt",
	"C04Amt",
	"C05Amt",
	"C06Amt",
	"C07Amt",
	"C08Amt",
	"C09Amt",
	"C10Amt",
	"C11Amt",
	"C12Amt",
	"C13Amt",
	"C14Amt",
	"C15Amt",
	"C16Amt",
	"C17Amt",
	"C18Amt",
	"C19Amt",
	"C20Amt",
	"C21Amt",
	"C22Amt",
	"C23Amt",
	"C24Amt",													//  191
};

static char* kEffectsA[]  = 
{
	"Master Effect A",
	"Room 1",
	"Room 2",
	"Room 3",
	"Hall 1",
	"Hall 2",
	"Plate",
	"Delay",
	"Panning Delay",
	"Multitap 1",
	"Multitap Pan",
	"3 Tap",
	"3 Tap Pan",
	"Soft Room",
	"Warm Room",
	"Perfect Room",
	"Tiled Room",
	"Hard Plate",
	"Warm Hall",
	"Spacious Hall",
	"Bright Hall",
	"Brt Hall Pan",
	"Bright Plate",
	"BBall Court",
	"Gymnasium",
	"Cavern",
	"Concert 9",
	"Concert 10 Pan",
	"Reverse Gate",
	"Gate 2",
	"Gate Pan",
	"Concert 11",
	"MediumConcert",
	"Large Concert",
	"Lg Concert Pan",
	"Canyon",
	"DelayVerb 1",
	"DelayVerb 2",
	"DelayVerb 3",
	"DelayVerb4Pan",
	"DelayVerb5Pan",
	"DelayVerb 6",
	"DelayVerb 7",
	"DelayVerb 8",
	"DelayVerb 9"
};

static char* kEffectsB[] = 
{
	"Master Effect B",
	"Chorus 1",
	"Chorus 2",
	"Chorus 3",
	"Chorus 4",
	"Chorus 5",
	"Doubling",
	"Slapback",
	"Flange 1",
	"Flange 2",
	"Flange 3",
	"Flange 4",
	"Flange 5",
	"Flange 6",
	"Flange 7",
	"Big Chorus",
	"Symphonic",
	"Ensemble",
	"Delay",
	"Delay Stereo",
	"Delay Stereo 2",
	"Panning Delay",
	"Delay Chorus",
	"Pan Dly Chrs 1",
	"Pan Dly Chrs 2",
	"DualTap 1/3",
	"DualTap 1/4",
	"Vibrato",
	"Distortion 1",
	"Distortion 2",
	"DistortedFlange",
	"DistortedChorus",
	"DistortedDouble"
};

static short kFilterMap[] = 
{
	0x007F,
	0x0001,
	0x0000,
	0x0002,
	0x0104,
	0x0105,
	0x0106,
	0x0108,
	0x0109,
	0x0008,
	0x0009,
	0x0010,
	0x0011,
	0x0012,
	0x0020,
	0x0021,
	0x0022,
	0x0112,
	0x0103,
	0x010C,
	0x010E,
	0x0113,
	0x0114,
	0x0115,
	0x0116,
	0x0050,
	0x0051,
	0x010F,
	0x010D,
	0x0110,
	0x0117,
	0x0118,
	0x0119,
	0x0040,
	0x0041,
	0x011A,
	0x011B,
	0x0048,
	0x011C,
	0x011D,
	0x0107,
	0x010B,
	0x0111,
	0x011E,
	0x011F,
	0x0120,
	0x0121,
	0x0122,
	0x010A,
	0x0042,
	0x0123,
};

static char* kFilters[] =
{
	"Off",				// -- --- = (7Fh,00h)
	"Smooth",			// 2 LPF  = (01h,00h)
	"Classic",			// 4 LPF  = (00h,00h)
	"Steeper",			// 6 LPF  = (02h,00h)
	"MegaSweepz",		// 12 LPF = (04h,01h)
	"EarlyRizer",		// 12 LPF = (05h,01h)
	"Millennium",		// 12 LPF = (06h,01h)
	"KlubKlassi",		// 12 LPF = (08h,01h)
	"BassBox-303",		// 12 LPF = (09h,01h)
	"Shallow",			// 2 HPF  = (08h,00h)
	"Deeper",			// 4 HPF  = (09h,00h)
	"Band-pass1",		// 2 BPF  = (10h,00h)
	"Band-pass2",		// 4 BPF  = (11h,00h)
	"ContraBand",		// 6 BPF  = (12h,00h)
	"Swept1oct",		// 6 EQ+  = (20h,00h)
	"Swept2>1oct",		// 6 EQ+  = (21h,00h)
	"Swept3>1oct",		// 6 EQ+  = (22h,00h)
	"DJAlkaline",		// 12 EQ+ = (12h,01h)
	"AceOfBass",		// 12 EQ+ = (03h,01h)
	"TB-OrNot-TB",		// 12 EQ+ = (0Ch,01h)
	"BolandBass",		// 12 EQ+ = (0Eh,01h)
	"BassTracer",		// 12 EQ+ = (13h,01h)
	"RogueHertz",		// 12 EQ+ = (14h,01h)
	"RazorBlades",		// 12 EQ- = (15h,01h)
	"RadioCraze",		// 12 EQ- = (16h,01h)
	"Aah-Ay-Eeh",		// 6 VOW  = (50h,00h)
	"Ooh-To-Aah",		// 6 VOW  = (51h,00h)
	"MultiQVox",		// 12 VOW = (0Fh,01h)
	"Ooh-To-Eee",		// 12 VOW = (0Dh,01h)
	"TalkingHedz",		// 12 VOW = (10h,01h)
	"Eeh-To-Aah",		// 12 VOW = (17h,01h)
	"UbuOrator",		// 12 VOW = (18h,01h)
	"DeepBouche",		// 12 VOW = (19h,01h)
	"PhazeShift1",		// 6 PHA  = (40h,00h)
	"PhazeShift2",		// 6 PHA  = (41h,00h)
	"FreakShifta",		// 12 PHA = (1Ah,01h)
	"CruzPusher",		// 12 PHA = (1Bh,01h)
	"FlangerLite",		// 6 FLG  = (48h,00h)
	"AngelzHairz",		// 12 FLG = (1Ch,01h)
	"DreamWeava",		// 12 FLG = (1Dh,01h)
	"MeatyGizmo",		// 12 REZ = (07h,01h)
	"DeadRinger",		// 12 REZ = (0Bh,01h)
	"ZoomPeaks",		// 12 REZ = (11h,01h)
	"AcidRavage",		// 12 REZ = (1Eh,01h)
	"BassOMatic",		// 12 REZ = (1Fh,01h)
	"LucifersQ",		// 12 REZ = (20h,01h)
	"ToothComb",		// 12 REZ = (21h,01h)
	"EarBender",		// 12 WAH = (22h,01h)
	"FuzziFace",		// 12 DST = (0Ah,01h)
	"BlissBatz",		// 6  PHA = (42h,00h)
	"KlangKling"		// 12 SFX = (23h,01h)
};

static char* kKeys[] =
{
	"C-2",
	"C#-2",
	"D-2",
	"D#-2",
	"E-2",
	"F-2",
	"F#-2",
	"G-2",
	"G#-2",
	"A-2",
	"A#-2",
	"B-2",
	"C-1",
	"C#-1",
	"D-1",
	"D#-1",
	"E-1",
	"F-1",
	"F#-1",
	"G-1",
	"G#-1",
	"A-1",
	"A#-1",
	"B-1",
	"C0",
	"C#0",
	"D0",
	"D#0",
	"E0",
	"F0",
	"F#0",
	"G0",
	"G#0",
	"A0",
	"A#0",
	"B0",
	"C1",
	"C#1",
	"D1",
	"D#1",
	"E1",
	"F1",
	"F#1",
	"G1",
	"G#1",
	"A1",
	"A#1",
	"B1",
	"C2",
	"C#2",
	"D2",
	"D#2",
	"E2",
	"F2",
	"F#2",
	"G2",
	"G#2",
	"A2",
	"A#2",
	"B2",
	"C3",
	"C#3",
	"D3",
	"D#3",
	"E3",
	"F3",
	"F#3",
	"G3",
	"G#3",
	"A3",
	"A#3",
	"B3",
	"C4",
	"C#4",
	"D4",
	"D#4",
	"E4",
	"F4",
	"F#4",
	"G4",
	"G#4",
	"A4",
	"A#4",
	"B4",
	"C5",
	"C#5",
	"D5",
	"D#5",
	"E5",
	"F5",
	"F#5",
	"G5",
	"G#5",
	"A5",
	"A#5",
	"B5",
	"C6",
	"C#6",
	"D6",
	"D#6",
	"E6",
	"F6",
	"F#6",
	"G6",
	"G#6",
	"A6",
	"A#6",
	"B6",
	"C7",
	"C#7",
	"D7",
	"D#7",
	"E7",
	"F7",
	"F#7",
	"G7",
	"G#7",
	"A7",
	"A#7",
	"B7",
	"C8",
	"C#8",
	"D8",
	"D#8",
	"E8",
	"F8",
	"F#8",
	"G8",
};

static char* kLFOShapes[] = 
{
	"triangle",				// 0
	"sine",
	"sawtooth",
	"square",
	"sine",
	"33% pulse",
	"25% pulse",
	"16% pulse",
	"12% pulse",
	"pat:octaves",
	"pat:5th + oct",
	"pat sus4trip",
	"pat:neener",
	"sine 1, 2",
	"sine 1, 3, 5",
	"sine + noise",
	"hemiquaver"			// 16
};

static char* kLFORates[] = 
{
	"8/1",		// -25
	"4/1d",
	"8/1t",
	"4/1",
	"2/1d",
	"4/1t",
	"2/1",
	"1/1d",
	"2/1t",
	"1/1",
	"1/2d",
	"1/1t",		// -14
	"1/2",
	"1/4d",
	"1/2t",
	"1/4",		// -10
	"1/8d",
	"1/4t",
	"1/8",
	"1/16d",
	"1/8t",
	"1/16",
	"1/32d",
	"1/16t",
	"1/32",		// -1
	
	"0.08Hz",
	"0.11Hz",
	"0.15Hz",
	"0.18Hz",
	"0.21Hz",
	"0.25Hz",
	"0.28Hz",
	"0.32Hz",
	"0.35Hz",
	"0.39Hz",
	"0.42Hz",
	"0.46Hz",
	"0.50Hz",
	"0.54Hz",
	"0.58Hz",
	"0.63Hz",
	"0.67Hz",
	"0.71Hz",
	"0.76Hz",
	"0.80Hz",
	"0.85Hz",
	"0.90Hz",
	"0.94Hz",
	"0.99Hz",
	"1.04Hz",
	"1.10Hz",
	"1.15Hz",
	"1.20Hz",
	"1.25Hz",
	"1.31Hz",
	"1.37Hz",
	"1.42Hz",
	"1.48Hz",
	"1.54Hz",
	"1.60Hz",
	"1.67Hz",
	"1.73Hz",
	"1.79Hz",
	"1.86Hz",
	"1.93Hz",
	"2.00Hz",
	"2.07Hz",
	"2.14Hz",
	"2.21Hz",
	"2.29Hz",
	"2.36Hz",
	"2.44Hz",
	"2.52Hz",
	"2.60Hz",
	"2.68Hz",
	"2.77Hz",
	"2.85Hz",
	"2.94Hz",
	"3.03Hz",
	"3.12Hz",
	"3.21Hz",
	"3.31Hz",
	"3.40Hz",
	"3.50Hz",
	"3.60Hz",
	"3.70Hz",
	"3.81Hz",
	"3.91Hz",
	"4.02Hz",
	"4.13Hz",
	"4.25Hz",
	"4.36Hz",
	"4.48Hz",
	"4.60Hz",
	"4.72Hz",
	"4.84Hz",
	"4.97Hz",
	"5.10Hz",
	"5.23Hz",
	"5.37Hz",
	"5.51Hz",
	"5.65Hz",
	"5.79Hz",
	"5.94Hz",
	"6.08Hz",
	"6.24Hz",
	"6.39Hz",
	"6.55Hz",
	"6.71Hz",
	"6.88Hz",
	"7.04Hz",
	"7.21Hz",
	"7.39Hz",
	"7.57Hz",
	"7.75Hz",
	"7.93Hz",
	"8.12Hz",
	"8.32Hz",
	"8.51Hz",
	"8.71Hz",
	"8.92Hz",
	"9.13Hz",
	"9.34Hz",
	"9.56Hz",
	"9.78Hz",
	"10.00Hz",
	"10.23Hz",
	"10.47Hz",
	"10.71Hz",
	"10.95Hz",
	"11.20Hz",
	"11.46Hz",
	"11.71Hz",
	"11.98Hz",
	"12.25Hz",
	"12.52Hz",
	"12.80Hz",
	"13.09Hz",
	"13.38Hz",
	"13.68Hz",
	"13.99Hz",
	"14.30Hz",
	"14.61Hz",
	"14.93Hz",
	"15.26Hz",
	"15.60Hz",
	"15.94Hz",
	"16.29Hz",
	"16.65Hz",
	"17.01Hz",
	"17.38Hz",
	"17.76Hz",
	"18.14Hz",
};

static char* kSoloModes[] = 
{
	"off",					// 0
	"multiple trigger",
	"melody (last)",
	"melody (low)",
	"melody (high)",
	"synth (last)",
	"synth (low)",
	"synth (high)",
	"fingered glide"		// 8
};

static char* kLayerGroups[] =
{
	"poly all",				// 0
	"poly 16 A",
	"poly 16 B",
	"poly 8 A",
	"poly 8 B",
	"poly 8 C",
	"poly 8 D",
	"poly 4 A",
	"poly 4 B",
	"poly 4 C",
	"poly 4 D",
	"poly 2 A",
	"poly 2 B",
	"poly 2 C",
	"poly 2 D",
	"mono A",
	"mono B",
	"mono C",
	"mono D",
	"mono E",
	"mono F",
	"mono G",
	"mono H",
	"mono I"				// 23
};

static unsigned char envunits1[] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 2, 2, 2, 2,
	2, 2, 2, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 5, 5, 5, 5,
	6, 6, 7, 7, 7, 8, 8, 9,
	9, 10, 11, 11, 12, 13, 13, 14,
	15, 16, 17, 18, 19, 20, 22, 23,
	24, 26, 28, 30, 32, 34, 36, 38,
	41, 44, 47, 51, 55, 59, 64, 70,
	76, 83, 91, 100, 112, 125, 142, 163,
};

static unsigned char envunits2[] =
{
	00, 01, 02, 03, 04, 05, 06, 07,
	8,  9,  10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23,
	25, 26, 28, 29, 32, 34, 36, 38,
	41, 43, 46, 49, 52, 55, 58, 62,
	65, 70, 74, 79, 83, 88, 93, 98,
	04, 10, 17, 24, 31, 39, 47, 56,
	65, 74, 84, 95, 06, 18, 31, 44,
	59, 73, 89, 06, 23, 42, 62, 82,
	04, 28, 52, 78, 05, 34, 64, 97,
	32, 67, 06, 46, 90, 35, 83, 34,
	87, 45, 06, 70, 38, 11, 88, 70,
	56, 49, 48, 53, 65, 85, 13, 50,
	97, 54, 24, 06, 02, 15, 44, 93,
	64, 60, 84, 41, 34, 70, 56, 03,
	22, 28, 40, 87, 9,  65, 36, 69,
};

static void cnv_glide_rate(long val, char *buf)
{
	int msec = (envunits1[val] * 1000 + envunits2[val] * 10) / 5;
	sprintf(buf, "%2d.%03d sec/oct", msec / 1000, msec % 1000);
}

#endif
