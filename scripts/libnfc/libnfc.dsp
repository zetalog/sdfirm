# Microsoft Developer Studio Project File - Name="libnfc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libnfc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libnfc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libnfc.mak" CFG="libnfc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libnfc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libnfc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libnfc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "../../obj/release/libusb"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "../../obj/release/libusb"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\include" /I "..\..\include\host\missing" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libnfc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "../../obj/release/libusb"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "../../obj/release/libusb"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\include\host\missing" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "libnfc - Win32 Release"
# Name "libnfc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\drivers\acr122.c
# End Source File
# Begin Source File

SOURCE=".\iso14443-subr.c"
# End Source File
# Begin Source File

SOURCE=".\mirror-subr.c"
# End Source File
# Begin Source File

SOURCE=".\nfc-device.c"
# End Source File
# Begin Source File

SOURCE=".\nfc-emulation.c"
# End Source File
# Begin Source File

SOURCE=".\nfc-internal.c"
# End Source File
# Begin Source File

SOURCE=.\nfc.c
# End Source File
# Begin Source File

SOURCE=.\drivers\pn532_uart.c
# End Source File
# Begin Source File

SOURCE=.\chips\pn53x.c
# End Source File
# Begin Source File

SOURCE=.\drivers\pn53x_usb.c
# End Source File
# Begin Source File

SOURCE=.\buses\uart_win32.c
# End Source File
# Begin Source File

SOURCE=.\buses\usb_libusb.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\drivers\acr122.h
# End Source File
# Begin Source File

SOURCE=.\drivers.h
# End Source File
# Begin Source File

SOURCE=.\iso7816.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=".\mirror-subr.h"
# End Source File
# Begin Source File

SOURCE="..\..\include\host\nfc-emulation.h"
# End Source File
# Begin Source File

SOURCE=".\nfc-internal.h"
# End Source File
# Begin Source File

SOURCE="..\..\include\host\nfc-types.h"
# End Source File
# Begin Source File

SOURCE=..\..\include\host\nfc.h
# End Source File
# Begin Source File

SOURCE=.\drivers\pn532_uart.h
# End Source File
# Begin Source File

SOURCE=".\chips\pn53x-internal.h"
# End Source File
# Begin Source File

SOURCE=.\chips\pn53x.h
# End Source File
# Begin Source File

SOURCE=.\drivers\pn53x_usb.h
# End Source File
# Begin Source File

SOURCE=.\buses\uart.h
# End Source File
# Begin Source File

SOURCE=.\buses\usb.h
# End Source File
# End Group
# End Target
# End Project
