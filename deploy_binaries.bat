@if "%RBC_ROOT%"=="" goto ERR
@if not exist out\msvc\Win32\Release\roboconf.exe goto EXE_ERR

xcopy /E /Y /I lib\*.*      %RBC_ROOT%\lib
xcopy /E /Y /I lib-user\*.* %RBC_ROOT%\lib-user
xcopy /E /Y /I lib-cube\*.* %RBC_ROOT%\lib-cube
xcopy /E /Y /I rules\*.*    %RBC_ROOT%\rules
xcopy /E /Y /I conf\*.*     %RBC_ROOT%\conf

goto END

:ERR
@echo RBC_ROOT is not set
goto END

:EXE_ERR
@echo Binaries not exist
goto END


:END

