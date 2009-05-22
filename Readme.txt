==============================
Proteus Controller Readme
Copyright (c) 2004 John McCann
==============================

The Proteus Controller is a MIDI-based patch editor for E-mu Proteus compatible sound modules.  This application displays a list of all presets (patches) available on a sound module and allows the user to view and edit many of the properties of the preset.  This document is intended to help a first time user get the application up and running.

==============
Hardware Setup
==============
The Proteus Controller requires the sound module's MIDI In be connected to your computer's MIDI out, and the sound module's MIDI out be connected to your computer's MIDI in.  You may also optionally connect a MIDI controller such as a keyboard to a second MIDI in port on your computer and the Proteus controller will forward all MIDI messages from the MIDI controller to the sound module's MIDI in.  A summary of the hardware setup is displayed below:
Computer		Sound Module
MIDI In  A	-> 	MIDI Out
MIDI Out A	->	MIDI In

(optional)
Computer		Keyboard
MIDI In B	-> 	MIDI Out

Please note that most computers have only one set of MIDI In/Out ports, therefore a MIDI hub is required to use a MIDI controller with the Proteus Controller.  The M-Audio MIDISport is one such device.

To change which MIDI ports your sound module and MIDI controller are connected to, select MIDI Setup from the Options menu.  

========
SysEx ID
========
The Proteus Controller uses MIDI System Exclusive messages to communicate with E-mu sound modules.  The SysEx ID is a number that the Proteus Controller uses to identify your sound module.  You can change your E-mu sound module's SysEx ID in the Master menu.  Make sure the same ID is set in the MIDI Setup dialog.

============
Preset Names
============
For performance reasons, the first time you run the Proteus Controller you will be prompted to download the user preset and instrument names from your sound module.  After they are downloaded, the names are saved on your hard drive and will be automatically loaded the next time you launch the Proteus Controller.  You can also download the names off of any installed ROMs by selecting the desired item from the Proteus menu. 

====================
Realtime Controllers
====================
When editing your preset it is convenient to set the initial controller amounts by turning the knobs on the front of your sound module.  The Proteus Controller listens for the MIDI messages corresponding to the realtime controller events, and allows you save the initial controller amounts via the Save button in the Realtime Controller box.  However, the message IDs that the sound module sends can be adjusted in the Master menu.  For the Proteus Controller to properly receive these messages, you must set your Realtime Controller Numbers to the following values:
A 95
B 22
C 23
D 24
E 25
F 26
G 27
H 28
I 78
J 79
K 91
L 93

===========
Other Notes
===========
E-mu Proteus sound modules allow you to set a delay between SysEx messages in the Master menu.  Decreasing this value to around 50ms will improve performance significantly.

