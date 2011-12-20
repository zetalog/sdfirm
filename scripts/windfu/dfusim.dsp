# Microsoft Developer Studio Project File - Name="dfusim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dfusim - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dfusim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dfusim.mak" CFG="dfusim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dfusim - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dfusim - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dfusim - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "../../obj/release/dfusim"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "../../obj/release/dfusim"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\include\" /I "..\..\include\host\missing" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0500 /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib advapi32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "dfusim - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "../../obj/debug/dfusim"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "../../obj/debug/dfusim"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\..\include\" /I "..\..\include\host\missing" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D _WIN32_WINNT=0x0500 /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "dfusim - Win32 Release"
# Name "dfusim - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\cmn\bitops.c
# End Source File
# Begin Source File

SOURCE=.\dfusim.c
# End Source File
# Begin Source File

SOURCE=..\cmn\wdmsim.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\host\bitops.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host\circbf.h
# End Source File
# Begin Source File

SOURCE=.\dfusim.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host\usbd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host\usbdfu.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host\wdm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host\wdmsim.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
