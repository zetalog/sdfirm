# Microsoft Developer Studio Project File - Name="winacpi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=winacpi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "winacpi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "winacpi.mak" CFG="winacpi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "winacpi - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "winacpi - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "winacpi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../bin/release/utils"
# PROP BASE Intermediate_Dir "../../../obj/release/winacpi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../bin/release/utils"
# PROP Intermediate_Dir "../../../obj/release/winacpi"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\include\\" /I "..\..\..\include\host\missing" /D "NDEBUG" /D "ACPI_MISALIGNMENT_NOT_SUPPORTED" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D CONFIG_ACPI_TESTS=1 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 setupapi.lib comctl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "winacpi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../bin/debug/utils"
# PROP BASE Intermediate_Dir "../../../obj/debug/winacpi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../bin/debug/utils"
# PROP Intermediate_Dir "../../../obj/debug/winacpi"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Od /I "..\..\..\include\\" /I "..\..\..\include\host\missing" /D "_DEBUG" /D "ACPI_MISALIGNMENT_NOT_SUPPORTED" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D CONFIG_ACPI_TESTS=1 /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 setupapi.lib comctl32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "winacpi - Win32 Release"
# Name "winacpi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_core.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_error.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_event.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_fadt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_fixed.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_generic.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_interp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_name.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_opcode.c
# End Source File
# Begin Source File

SOURCE=.\acpi_ospm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_rsdp.c
# End Source File
# Begin Source File

SOURCE=.\acpi_simul.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_space.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_table.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_utils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_xsdt.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\win\dialogs.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\cmn\dirent.c
# End Source File
# Begin Source File

SOURCE=..\..\lib\win\layout.c
# End Source File
# Begin Source File

SOURCE=.\winacpi.c
# End Source File
# Begin Source File

SOURCE=.\winacpi.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\host\acpi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\target\acpi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_aml.h
# End Source File
# Begin Source File

SOURCE=..\..\..\kernel\acpi\acpi_int.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\winacpi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\host\winlayout.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\dfumode4bit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\dnload.ico
# End Source File
# Begin Source File

SOURCE=.\res\header.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tabletype4bit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar4bit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upload.ico
# End Source File
# Begin Source File

SOURCE=.\res\winacpi.ico
# End Source File
# Begin Source File

SOURCE=.\res\wizard.bmp
# End Source File
# End Group
# End Target
# End Project
