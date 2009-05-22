# Microsoft Developer Studio Project File - Name="ProteusController" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ProteusController - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProteusController.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProteusController.mak" CFG="ProteusController - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProteusController - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ProteusController - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProteusController - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "wxWindows-2.4.2/include" /I "wxWindows-2.4.2\lib\mswd" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Winmm.lib kernel32.lib urlmon.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxWindows-2.4.2\lib\wxmsw.lib /nologo /subsystem:windows /machine:I386 /libpath:"wxWindows-2.4.2\lib" /OPT:REF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ProteusController - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "wxWindows-2.4.2/include" /I "wxWindows-2.4.2\lib\mswd" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Winmm.lib kernel32.lib urlmon.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib wxWindows-2.4.2\lib\wxmswd.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"wxWindows-2.4.2\lib"

!ENDIF 

# Begin Target

# Name "ProteusController - Win32 Release"
# Name "ProteusController - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CMidiSetupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CProteus.cpp
# End Source File
# Begin Source File

SOURCE=.\CProteusController.cpp
# End Source File
# Begin Source File

SOURCE=.\CSaveAsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CWorkerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\ProteusController.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CMidiSetupDlg.h
# End Source File
# Begin Source File

SOURCE=.\Commands.h
# End Source File
# Begin Source File

SOURCE=.\CProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\CProteus.h
# End Source File
# Begin Source File

SOURCE=.\CProteusController.h
# End Source File
# Begin Source File

SOURCE=.\CSaveAsDlg.h
# End Source File
# Begin Source File

SOURCE=.\CWorkerThread.h
# End Source File
# Begin Source File

SOURCE=.\Events.h
# End Source File
# Begin Source File

SOURCE=.\Interfaces.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\preset.ico
# End Source File
# Begin Source File

SOURCE=.\root.ico
# End Source File
# Begin Source File

SOURCE=.\simm.ico
# End Source File
# Begin Source File

SOURCE=.\splash_b.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\GPL.txt

!IF  "$(CFG)" == "ProteusController - Win32 Release"

# Begin Custom Build
OutDir=.\Release
InputPath=.\GPL.txt

"$(OutDir)\CustomBuild1.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xcopy $(InputPath) $(OutDir) /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "ProteusController - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\GPL.txt

"$(OutDir)\CustomBuild1.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xcopy $(InputPath) $(OutDir) /Y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Readme.txt

!IF  "$(CFG)" == "ProteusController - Win32 Release"

# Begin Custom Build
OutDir=.\Release
InputPath=.\Readme.txt

"$(OutDir)\CustomBuild2.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xcopy $(InputPath) $(OutDir) /Y

# End Custom Build

!ELSEIF  "$(CFG)" == "ProteusController - Win32 Debug"

# Begin Custom Build
OutDir=.\Debug
InputPath=.\Readme.txt

"$(OutDir)\CutsomBuild2txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xcopy $(InputPath) $(OutDir) /Y

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
