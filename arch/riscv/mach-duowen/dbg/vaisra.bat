@echo OFF
for /F "tokens=1-3 delims=: " %%a in ('netsh.exe interface ip show address name^="vEthernet (WSL)" ^|find "IP Address"') do set BINDTO=%%c
@echo ===========================================================================
@echo WSL Address=%BINDTO%
@echo ===========================================================================

.\openocd-win32\bin\openocd.exe -f jlink.cfg -f vaisra.cfg -c "bindto %BINDTO%" -c init -c halt
