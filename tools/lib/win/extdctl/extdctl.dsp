# Microsoft Developer Studio Project File - Name="extdctl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=extdctl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "extdctl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "extdctl.mak" CFG="extdctl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "extdctl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "extdctl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "extdctl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\..\..\bin\release\utils"
# PROP BASE Intermediate_Dir "..\..\..\..\obj\release\extdctl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\bin\release\utils"
# PROP Intermediate_Dir "..\..\..\..\obj\release\extdctl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "extdctl_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "..\..\..\..\include" /D "NDEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D CONFIG_ACPI_TESTS=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /dll /map /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "extdctl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\..\..\bin\debug\utils"
# PROP BASE Intermediate_Dir "..\..\..\..\obj\debug\extdctl"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\bin\debug\utils"
# PROP Intermediate_Dir "..\..\..\..\obj\debug\extdctl"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "extdctl_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Od /I "..\..\..\..\include" /D "_DEBUG" /D "_USRDLL" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D CONFIG_ACPI_TESTS=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib comctl32.lib advapi32.lib shell32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "extdctl - Win32 Release"
# Name "extdctl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ECDebug.c
# End Source File
# Begin Source File

SOURCE=.\ECDraw.c
# End Source File
# Begin Source File

SOURCE=.\extdctl.def
# End Source File
# Begin Source File

SOURCE=.\extdctrl.c
# End Source File
# Begin Source File

SOURCE=.\extdctrl.rc
# End Source File
# Begin Source File

SOURCE=.\SPLAttrib.c
# End Source File
# Begin Source File

SOURCE=.\SPLDraw.c
# End Source File
# Begin Source File

SOURCE=.\Splitter.c
# End Source File
# Begin Source File

SOURCE=.\SPLMsgProc.c
# End Source File
# Begin Source File

SOURCE=.\SPLTrack.c
# End Source File
# Begin Source File

SOURCE=.\TLMisc.c
# End Source File
# Begin Source File

SOURCE=.\TLMouse.c
# End Source File
# Begin Source File

SOURCE=.\TLMsgPrc.c
# End Source File
# Begin Source File

SOURCE=.\TLPaint.c
# End Source File
# Begin Source File

SOURCE=.\TLTree.c
# End Source File
# Begin Source File

SOURCE=.\TreeList.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ECIntern.h
# End Source File
# Begin Source File

SOURCE=.\ECRes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\host\extdctrl.h
# End Source File
# Begin Source File

SOURCE=.\SPLIntern.h
# End Source File
# Begin Source File

SOURCE=.\TLintern.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\sarrows.cur
# End Source File
# Begin Source File

SOURCE=.\res\splith.cur
# End Source File
# Begin Source File

SOURCE=.\SPLITH.CUR
# End Source File
# Begin Source File

SOURCE=.\res\splitv.cur
# End Source File
# End Group
# End Target
# End Project
