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

	if not exist %ROOT%\bin mkdir %ROOT%\bin > nul

	if not "%1"=="" set prj=%1
							rem echo %prj%

	makecert -r -pe -n "CN=Driver %prj% Test Certificate" -ss CA -sr CurrentUser -a sha1 -sky signature ^
-sv %prj%.pvk %prj%.cer
	
	pvk2pfx -pvk %prj%.pvk -spc %prj%.cer -pfx %prj%.pfx
	
	move %prj%.pvk %ROOT%\bin
	move %prj%.cer %ROOT%\bin
	move %prj%.pfx %ROOT%\bin

	echo gen selfsign complete
