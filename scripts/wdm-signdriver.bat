	@echo Off
	Setlocal EnableDelayedExpansion
	set bpath=%~dp0
	set cpath=%cd%
	cd /D %bpath%
	cd ..\
	set ROOT=%cd%
	for /f "delims=" %%i in ("%cd%") do set prj=%%~ni
	cd %cpath%
	if {%1}=={/?} goto usage
	if {%1}=={} (goto no_driver) else if not exist %bpath%%1 goto no_driver
	if {%DDK_TARGET_OS%}=={} goto no_ddk_env
	if {%BUILD_ALT_DIR%}=={} goto no_ddk_env
	if {%BUILD_DEFAULT_TARGETS%}=={} goto no_ddk_env
	if {%NTMAKEENV%}=={} goto no_ddk_env
	if {%_BUILDARCH%}=={} goto no_dkk_env
	set platform=%_BUILDARCH%
	if {%_BUILDARCH%}=={amd64} set platform=x64
	if {%BUILD_ALT_DIR:~0,3%}=={chk} goto _debug
	if {%BUILD_ALT_DIR:~0,3%}=={fre} goto _free
:_debug
	set dpath=%ROOT%\bin\debug\drivers\%DDK_TARGET_OS%\%_BUILDARCH%
	set conf=debug
	goto _skip
:_free
	set dpath=%ROOT%\bin\release\drivers\%DDK_TARGET_OS%\%_BUILDARCH%
	set conf=release
:_skip
	if {%2}=={/c} goto clean
	set dname=%1
	set a0=%ROOT%\bin
	set a1=%a0%\%conf%
	set a2=%a1%\drivers
	set a3=%a2%\%DDK_TARGET_OS%
	set a4=%a3%\%_BUILDARCH%
	for %%i in (%a0% %a1% %a2% %a3% %a4%) do if not exist %%i mkdir %%i > nul
	set spath=%bpath%%dname%\obj%BUILD_ALT_DIR%
	if {%BUILD_DEFAULT_TARGETS:~1%}=={386} goto i386
	set spath=%bpath%%dname%\obj%BUILD_ALT_DIR%\%BUILD_DEFAULT_TARGETS:~1%
	goto not_i386
:i386
	set spath=%spath%\%BUILD_DEFAULT_TARGETS:*-=i%
:not_i386
	set dinf=%spath%\%dname%.inf
	if not exist %dinf% goto no_inf
	%NTMAKEENV%\%_BUILDARCH%\stampinf -f %dinf% -d 09/08/2011 -v 6.13.0322.0201 -c %dname%.cat
	set dsys=%spath%\%dname%.sys
	if not exist %dsys% goto no_sys
	set dpdb=%spath%\%dname%.pdb
	if not exist %dpdb% goto no_pdb
	set dmof=%bpath%%dname%\%dname%.mof
	if not exist %dmof% goto skip_bmf
	set dbmf=%spath%\%dname%.bmf
	if not exist %dbmf% goto no_bmf
:skip_bmf
	set cert=%2
	if {%cert%}=={} goto initcert
	if not exist %ROOT%\bin\%cert%.pfx goto initcert
	set cert=%ROOT%\bin\%cert%.pfx
	goto sign
:initcert
	if not exist %ROOT%\bin\*.cer goto gencert
	if not exist %ROOT%\bin\*.pfx goto gencert
	if not exist %ROOT%\bin\*.pvk goto gencert
	echo.
	echo Certificate Store (q to exit):
	echo.
	if {%cert%}=={} goto prj_cert
	echo 0. create new cert (%cert%)
	goto list_cert
:prj_cert
	echo 0. create new cert (%prj%)
:list_cert
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
	set /p cert=Select a pfx file (0~~%count%)?
	echo.
	if {%cert%}=={0} goto gencert
	if {%cert%}=={q} goto end
	set cert=%ROOT%\bin\!arr%cert%!
	set cert=%cert:~0,-4%.pfx
	if not exist %cert% goto aga
	goto sign
:gencert
	if {%2}=={} goto def_cert
	call %bpath%wdm-gencert.bat %2
	set  cert=%ROOT%\bin\%2.pfx
	goto sign
:def_cert
	call %bpath%wdm-gencert.bat %prj%
	set cert=%ROOT%\bin\%prj%.pfx
:sign
	%NTMAKEENV%\SelfSign\signtool sign /f %cert% /t http://timestamp.verisign.com/scripts/timestamp.dll ^
%spath%\%dname%.sys
	if {%DDK_TARGET_OS%}=={Win2K} set os_type=2000
	if {%DDK_TARGET_OS%}=={WinNET} set os_type=Server2003_%platform%
	if {%DDK_TARGET_OS%}=={WinXP} set os_type=XP_%platform%
	if {%DDK_TARGET_OS%}=={WinLH} set os_type=VISTA_%platform%,Server2008_%platform%
	%NTMAKEENV%\SelfSign\Inf2Cat.exe /driver:%spath% /OS:%os_type%
	set dcat=%spath%\%dname%.cat
	if not exist %dcat% goto no_cat
	%NTMAKEENV%\SelfSign\signtool sign /f %cert% /t http://timestamp.verisign.com/scripts/timestamp.dll ^
%spath%\%dname%.cat
	if not exist %dpath% goto no_path
	copy /Y %dinf% %dpath%\
	copy /Y %dcat% %dpath%\
	copy /Y %dsys% %dpath%\
	copy /Y %dpdb% %dpath%\
	if exist %dbmf% copy /Y %dbmf% %dpath%\
	goto end
:no_ddk_env
	echo Err: Run DDK Build Environment batch first.
	goto end
:no_path
	echo Err: Check Destination Package Folder first.
	goto end
:no_inf
	echo Err: %dname%.inf not found.
	set dinf=%bpath%%dname%\%dname%
	if exist %dinf%.inf (goto no_inf2) else if exist %dinf%.inx goto no_inf2
	echo no %dname%.inf found in driver package.
	goto end
:no_inf2
	echo find %dname%.inf/inx in %bpath%%dname%.
	goto end
:no_cat
	echo Err: %dname%.cat generated fail.
	goto end
:no_sys
	echo Err: %dname%.sys not found.
	goto end
:no_pdb
	echo Err: %dname%.pdb not found.
	goto end
:no_bmf
	echo Err: %dname%.bmf not found.
	goto end
:no_driver
	echo Err: specified driver not found.
	goto end
:usage
	echo Usage:
	echo wdm-signdriver.bat DRIVER_NAME  [/c] / [cert]
	echo    [/c]    Clean a specified driver package.
	echo    [cert]  Specify certificate name
	echo            if no cert specified, existed cert which located in
	echo            "sdfirm\bin" folder will be shown to you, choose one index,
	echo            use index "0" to create a new one, q to exit.
	echo.
	echo    Sample:    How to sign usbdfu
	echo    Step1: Run wdk Checked/Free Build Environment
	echo    Step2: Using existed certificate
	echo      Type "wdm-signdriver.bat usbdfu"
	echo      If no cert existed, a pop-up dialog will automatically run and you
	echo      will be asked to create a new cert.
	echo      If any cert existed, certificate store will be shown on the screen,
	echo      choose one of the cert which you want to use.
	echo    Step2: Using specified certificate
	echo      Type "wdm-signdriver.bat usbdfu sdfirm"
	echo      This indicates usbdfu using the cert which named sdfirm.
	echo      NOTE: If sdfirm cert is not existed, use index "0" to create a new one.
	echo    Kernel driver package will be generated into following folder:
	echo      sdfirm\bin\(debug/release)\drivers\PLATFORM\ARCH
	echo      NOTE: According to your build environment,PLATFORM\ARCH may differs.
	echo.
	goto end
:clean
	echo clean driver package...
	set cpath=%cd%
	cd /D %dpath%
	del /s /q %1.inf %1.cat %1.sys %1.pdb %1.bmf
	cd /D %cpath%
	echo clean up complete!
:end
	cd /D %cpath%
