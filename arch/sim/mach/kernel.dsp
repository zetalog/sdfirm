# Microsoft Developer Studio Project File - Name="kernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=kernel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kernel.mak" CFG="kernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kernel - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "kernel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kernel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\bin\kernel\release"
# PROP BASE Intermediate_Dir ".\obj\kernel\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\bin\kernel\release"
# PROP Intermediate_Dir ".\obj\kernel\release"
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

!ELSEIF  "$(CFG)" == "kernel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\bin\kernel\debug"
# PROP BASE Intermediate_Dir ".\obj\kernel\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\bin\kernel\debug"
# PROP Intermediate_Dir ".\obj\kernel\debug"
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

# Name "kernel - Win32 Release"
# Name "kernel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\lib\atomic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bitops.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\bulk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\crc16_ccitt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\delay.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\heap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\heap_dlmalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\init\init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\irq.c
# End Source File
# Begin Source File

SOURCE=..\..\..\init\main.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\muldiv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\panic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\state.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\string.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\task.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\timer.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\target\arch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\atomic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\bitops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\bulk.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\circbf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\compiler.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\const.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\crc16_ccitt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\dbg_event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\delay.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\devid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\generic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\driver\gpt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\gpt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\irq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\jiffies.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\muldiv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\panic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\state.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\task.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\driver\tsc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\tsc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\types.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\driver\vic.h
# End Source File
# End Group
# End Target
# End Project
