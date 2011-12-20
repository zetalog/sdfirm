# Microsoft Developer Studio Project File - Name="kconfig" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=kconfig - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kconfig.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kconfig.mak" CFG="kconfig - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kconfig - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "kconfig - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kconfig - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\bin\kconfig\release"
# PROP BASE Intermediate_Dir ".\obj\kconfig\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\bin\kconfig\release"
# PROP Intermediate_Dir ".\obj\kconfig\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "..\include" /I "..\..\..\include" /I "..\..\..\scripts\kconfig" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "KBUILD_NO_NLS" /D "YY_NO_UNISTD_H" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "kconfig - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\bin\kconfig\debug"
# PROP BASE Intermediate_Dir ".\obj\kconfig\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\bin\kconfig\debug"
# PROP Intermediate_Dir ".\obj\kconfig\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\..\include" /I "..\..\..\scripts\kconfig" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "KBUILD_NO_NLS" /D "YY_NO_UNISTD_H" /YX /FD /GZ /c
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

# Name "kconfig - Win32 Release"
# Name "kconfig - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\confdata.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\expr.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\lex.zconf.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\menu.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\regex\regex.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\symbol.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\util.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\zconf.hash.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\zconf.tab.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\expr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\lkc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\lkc_port.h
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\lkc_proto.h
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\kconfig\prog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\regex\regex_internal.h
# End Source File
# End Group
# End Target
# End Project
