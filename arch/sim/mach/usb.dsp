# Microsoft Developer Studio Project File - Name="usb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=usb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "usb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "usb.mak" CFG="usb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "usb - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "usb - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "usb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\bin\usb\release"
# PROP BASE Intermediate_Dir ".\obj\usb\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\bin\usb\release"
# PROP Intermediate_Dir ".\obj\usb\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "..\include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "usb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\bin\usb\debug"
# PROP BASE Intermediate_Dir ".\obj\usb\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\bin\usb\debug"
# PROP Intermediate_Dir ".\obj\usb\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "usb - Win32 Release"
# Name "usb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "arch"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sim_usb.c
# End Source File
# Begin Source File

SOURCE=.\usb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\usbip\usbip_dev.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\kernel\usb\ccid_none.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\ccid_spe.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\hid_kbd.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\msd_bbb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\msd_scsi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\scd_ccid.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb_dev.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb_dfu.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb_hcd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb_hid.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\scd_iccd.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb_msd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb_otg.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\usb\usb_scd.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\kernel\usb\ccid_spe.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\msd_bbb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\msd_scsi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\driver\usb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\usb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\usb_dev.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\usb_hcd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\usb_msd.h
# End Source File
# End Group
# End Target
# End Project
