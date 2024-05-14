@if "%RBC_ROOT%"=="" goto ERR
@if not exist %~dp0\out\msvc\Win32\Release\roboconf.exe goto EXE_ERR

@copy /Y /B %~dp0\out\msvc\Win32\Release\roboconf.exe %RBC_ROOT%\bin\roboconf.exe

goto END

:ERR
@echo RBC_ROOT is not set
goto END

:EXE_ERR
@echo Binaries not exist
goto END


:END

