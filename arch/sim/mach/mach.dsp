# Microsoft Developer Studio Project File - Name="mach" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=mach - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mach.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mach.mak" CFG="mach - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mach - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mach - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mach - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\bin\release"
# PROP BASE Intermediate_Dir ".\obj\mach\release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\bin\release"
# PROP Intermediate_Dir ".\obj\mach\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "..\include" /I "..\..\..\include" /I "..\..\..\include\host\missing" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib shell32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "mach - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\bin\debug"
# PROP BASE Intermediate_Dir ".\obj\win33\debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\bin\debug"
# PROP Intermediate_Dir ".\obj\win33\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\..\include" /I "..\..\..\include\host\missing" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdi32.lib kernel32.lib advapi32.lib shell32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mach - Win32 Release"
# Name "mach - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\entry.c
# End Source File
# Begin Source File

SOURCE=..\common\head.c
# End Source File
# End Group
# Begin Group "dbg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_dfu.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_hid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_io.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_msd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_pn53x.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_scd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_scs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_scsi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\dbg_usb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\uartlog\uartdbg.c
# End Source File
# End Group
# Begin Group "sim"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sim_core.c
# End Source File
# Begin Source File

SOURCE=.\sim_cpu.c
# End Source File
# Begin Source File

SOURCE=.\sim_gpt.c
# End Source File
# Begin Source File

SOURCE=.\sim_irq.c
# End Source File
# Begin Source File

SOURCE=.\sim_mem.c
# End Source File
# Begin Source File

SOURCE=.\sim_uart.c
# End Source File
# End Group
# Begin Group "cmn"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\scripts\cmn\dfastm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\cmn\eloop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\scripts\cmn\missing.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\board.c
# End Source File
# Begin Source File

SOURCE=.\gpt.c
# End Source File
# Begin Source File

SOURCE=.\irq.c
# End Source File
# Begin Source File

SOURCE=.\spi.c
# End Source File
# Begin Source File

SOURCE=.\tsc.c
# End Source File
# Begin Source File

SOURCE=.\uart.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "h(asm)"

# PROP Default_Filter ""
# Begin Group "h(mach)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\asm\mach\arch.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\mach\gpt.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\mach\irq.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\mach\spi.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\mach\tsc.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\mach\uart.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\mach\usb.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\include\asm\arch.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\delay.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\irq.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\muldiv.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\task.h
# End Source File
# Begin Source File

SOURCE=..\include\asm\types.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\target\autoconf.h
# End Source File
# Begin Source File

SOURCE=.\mach.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\host\usbip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
