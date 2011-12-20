@echo Off
	Setlocal EnableDelayedExpansion
							rem echo print local
	set bpath=%~dp0
	set cpath=%cd%
							rem echo %bpath%
							rem echo %cpath%
	cd /D %bpath%
	cd ..\
							rem echo %cd%
	set ROOT=%cd%
							rem echo %ROOT%
	
	for /f "delims=" %%i in ("%cd%") do set prj=%%~ni
						
							rem echo %prj%
	cd %cpath%

	if {%1}=={} goto no_driver
	if {%DDK_TARGET_OS%}=={} goto no_ddk_env
	if {%BUILD_ALT_DIR%}=={} goto no_ddk_env
	if {%BUILD_DEFAULT_TARGETS%}=={} goto no_ddk_env
	if {%NTMAKEENV%}=={} goto no_ddk_env
	if {%_BUILDARCH%}=={} goto no_dkk_env
	
	set platform=%_BUILDARCH%
	if {%_BUILDARCH%}=={amd64} set platform=x64
							rem echo %platform%

	set dpath=%ROOT%\bin\drivers\obj%BUILD_ALT_DIR:~0,3%\%DDK_TARGET_OS%\%_BUILDARCH%
	
							rem echo %dpath%
	if {%1}=={/c} goto clean
	
	set dname=%1
							rem echo %dname%
	set a0=%ROOT%\bin
	set a1=%ROOT%\bin\drivers
	set a2=%ROOT%\bin\drivers\obj%BUILD_ALT_DIR:~0,3%
	set a3=%ROOT%\bin\drivers\obj%BUILD_ALT_DIR:~0,3%\%DDK_TARGET_OS%
	set a4=%ROOT%\bin\drivers\obj%BUILD_ALT_DIR:~0,3%\%DDK_TARGET_OS%\%_BUILDARCH%

	for %%i in (%a0% %a1% %a2% %a3% %a4%) do if not exist %%i mkdir %%i > nul

	set spath=%bpath%%dname%\obj%BUILD_ALT_DIR%
	if {%BUILD_DEFAULT_TARGETS:~1%}=={386} goto i386
							
	set spath=%bpath%%dname%\obj%BUILD_ALT_DIR%\%BUILD_DEFAULT_TARGETS:~1%
	goto not_i386
							rem echo %spath%
:i386
	set spath=%spath%\%BUILD_DEFAULT_TARGETS:*-=i%
							rem echo %dname%
:not_i386
	set dinf=%spath%\%dname%.inf
							rem echo %dinf%
	if not exist %dinf% goto no_inf

	%NTMAKEENV%\%_BUILDARCH%\stampinf -f %dinf% -d 09/08/2011 -v 6.13.0322.0201 -c %dname%.cat
							
							rem echo %NTMAKEENV%\%_BUILDARCH%
	set dsys=%spath%\%dname%.sys
							rem echo %dsys%
	if not exist %dsys% goto no_sys

	set dpdb=%spath%\%dname%.pdb
							rem echo %dpdb%
	if not exist %dpdb% goto no_pdb	

	set dbmf=%spath%\%dname%.bmf
							rem echo %dbmf%
	if not exist %dbmf% goto no_bmf

	set cert=%2
	
	if {%cert%}=={} goto initcert	

	set cert=%ROOT%\bin\%cert:.pfx=%
							rem echo %cert%
	if not exist %cert%.pfx goto initcert

	goto sign

:initcert
	if not exist %ROOT%\bin\*.cer goto gencert

	if not exist %ROOT%\bin\*.pfx goto gencert

	if not exist %ROOT%\bin\*.pvk goto gencert
	
	echo.
	
	echo Current Exist Certification:	

	echo.

	set count=1
	for /f %%i IN ('dir /b %ROOT%\bin\*.pfx') do (call :subroutine %%i)
	set /a count-=1
	goto :aga

:subroutine
 	echo %count%. %1
 	set arr%count%=%1
	set /a count+=1
	goto :eof

:aga	
	echo.
							rem echo %count%
	set /p cert=Select a pfx file (1~~%count%)?
	
	echo.
							rem echo !arr%cert%!
	set cert=%ROOT%\bin\!arr%cert%!
	set cert=%cert:~0,-4%
							rem echo %cert%
	if not exist %cert%.pfx goto aga

	goto sign

:gencert
							rem echo generating self certification...
	call %bpath%wdm-gencert.bat %prj%

	set cert=%ROOT%\bin\%prj%
							rem echo %cert%
:sign	
	%NTMAKEENV%\SelfSign\signtool sign /f %cert%.pfx /t http://timestamp.verisign.com/scripts/timestamp.dll ^
%spath%\%dname%.sys	
	
	if {%DDK_TARGET_OS%}=={Win2K} set os_type=2000

	if {%DDK_TARGET_OS%}=={WinNET} set os_type=Server2003_%platform%

	if {%DDK_TARGET_OS%}=={WinXP} set os_type=XP_%platform%

	if {%DDK_TARGET_OS%}=={WinLH} set os_type=VISTA_%platform%,Server2008_%platform%

	%NTMAKEENV%\SelfSign\Inf2Cat.exe /driver:%spath% /OS:%os_type%
	
	set dcat=%spath%\%dname%.cat
							rem echo %dcat%
	if not exist %dcat% goto no_cat

	%NTMAKEENV%\SelfSign\signtool sign /f %cert%.pfx /t http://timestamp.verisign.com/scripts/timestamp.dll ^
%spath%\%dname%.cat

	if not exist %dpath% goto no_path

	copy /Y %dinf% %dpath%\
	copy /Y %dcat% %dpath%\
	copy /Y %dsys% %dpath%\
	copy /Y %dpdb% %dpath%\
	copy /Y %dbmf% %dpath%\

	rem echo %dpath%\%dname%.sys >list.txt
	rem echo %dpath%\%dname%.inf >>list.txt
	rem echo %dpath%\%dname%.cat >>list.txt		
							rem type list.txt
	
	rem makecab /f list.txt /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 ^
	rem /d diskdirectorytemplate=tmpmakecab /d cabinetnametemplate=%dname%.cab
	rem move tmpmakecab\%dname%.cab %ROOT%\bin\

	rem del /s /q list.txt 
	rem rd /s /q tmpmakecab
	
	goto end

:no_ddk_env
	echo Run DDK Build Environment batch first.
	goto end
:no_path
	echo Check Destination Package Folder first.
	goto end
:no_inf
	echo %dname%.inf not found.
	goto end
:no_cat 
	echo %dname%.cat generated fail.
	goto end
:no_sys
	echo %dname%.sys not found.
	goto end
:no_pdb
	echo %dname%.pdb not found.
	goto end
:no_bmf 
	echo %dbmf%.bmf not found.
	goto end
:no_driver
	echo Usage: sign-win-dfu.bat [/c] DRIVER_NAME [/z]
	echo		/c: clean driver package
	echo		/z: specify certification name
	goto end
:clean
	rem echo clean driver package...
							rem echo %dpath%
	set cpath=%cd%
	cd /D %dpath%
	del /s /q *.inf *.cat *.sys *.pdb *.bmf
	cd /D %cpath%
	echo clean up complete!
:end
							rem echo %cpath%
	cd /D %cpath%
	
