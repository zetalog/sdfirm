# Microsoft Developer Studio Project File - Name="libusb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libusb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libusb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libusb.mak" CFG="libusb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libusb - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libusb - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libusb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../../bin/release/utils"
# PROP BASE Intermediate_Dir "../../../../obj/release/libusb"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../bin/release/utils"
# PROP Intermediate_Dir "../../../../obj/release/libusb"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../../../include" /I "../../../../include/host/missing" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libusb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../../bin/debug/utils"
# PROP BASE Intermediate_Dir "../../../../obj/debug/libusb"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../bin/debug/utils"
# PROP Intermediate_Dir "../../../../obj/debug/libusb"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "../../../../include/host/missing" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libusb - Win32 Release"
# Name "libusb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\usb_core.c
# End Source File
# Begin Source File

SOURCE=.\usb_desc.c
# End Source File
# Begin Source File

SOURCE=.\usb_io.c
# End Source File
# Begin Source File

SOURCE=.\usb_linux.c
# End Source File
# Begin Source File

SOURCE=.\usb_reg.c
# End Source File
# Begin Source File

SOURCE=.\usb_sync.c
# End Source File
# Begin Source File

SOURCE=.\usb_win32.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\include\host\libusb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\host\usb_err.h
# End Source File
# Begin Source File

SOURCE=.\usb_int.h
# End Source File
# Begin Source File

SOURCE=.\usb_linux.h
# End Source File
# Begin Source File

SOURCE=.\usb_win32.h
# End Source File
# End Group
# Begin Group "Missing Files"

# PROP Default_Filter "h;c"
# Begin Source File

SOURCE=..\..\..\lib\cmn\daytime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cmn\dirent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\host\missing\dirent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cmn\poll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\host\missing\sys\poll.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\host\missing\stdint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\include\host\missing\sys\time.h
# End Source File
# End Group
# End Target
# End Project
