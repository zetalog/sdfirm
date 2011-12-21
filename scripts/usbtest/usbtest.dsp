# Microsoft Developer Studio Project File - Name="usbtest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=usbtest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "usbtest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "usbtest.mak" CFG="usbtest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "usbtest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "usbtest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "usbtest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "../../obj/release/usbtest"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "../../obj/release/usbtest"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../include" /I "../../include/host/missing" /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_CONSOLE" /D "LIBUSB_1_0" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 winscard.lib kernel32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "usbtest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "../../obj/debug/usbtest"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "../../obj/debug/usbtest"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../include/host/missing" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "LIBUSB_1_0" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winscard.lib kernel32.lib advapi32.lib wsock32.lib /nologo /subsystem:console /pdb:none /debug /machine:I386

!ENDIF 

# Begin Target

# Name "usbtest - Win32 Release"
# Name "usbtest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\cmn\cunit.c
# End Source File
# Begin Source File

SOURCE=.\ut_ccid.c
# End Source File
# Begin Source File

SOURCE=.\ut_main.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ut_priv.h
# End Source File
# End Group
# Begin Group "Missing Files"

# PROP Default_Filter "h;c"
# Begin Source File

SOURCE=..\..\include\host\missing.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host\missing\stdint.h
# End Source File
# End Group
# Begin Group "USB Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\host\libusb.h
# End Source File
# Begin Source File

SOURCE=.\usb.c
# End Source File
# Begin Source File

SOURCE=.\usb_priv.h
# End Source File
# Begin Source File

SOURCE=..\..\include\host\usbapi.h
# End Source File
# End Group
# Begin Group "PCSC Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pcsc.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host\pcsc.h
# End Source File
# Begin Source File

SOURCE=.\pcsc_acos5.c
# End Source File
# Begin Source File

SOURCE=.\pcsc_priv.h
# End Source File
# Begin Source File

SOURCE=.\pcsc_tcos.c
# End Source File
# End Group
# Begin Group "CCID Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ccid.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host\ccid.h
# End Source File
# Begin Source File

SOURCE=.\ccid_priv.h
# End Source File
# End Group
# Begin Group "LOG Files"

# PROP Default_Filter "h;c"
# Begin Source File

SOURCE=..\cmn\getopt.c
# End Source File
# Begin Source File

SOURCE=.\logger.c
# End Source File
# Begin Source File

SOURCE=..\..\include\host\logger.h
# End Source File
# End Group
# End Target
# End Project
