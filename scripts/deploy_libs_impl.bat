@if "%RBC_ROOT%"=="" goto ERR

@xcopy /E /Y /I %~dp0\..\lib\*.*      %RBC_ROOT%\lib
@xcopy /E /Y /I %~dp0\..\lib-user\*.* %RBC_ROOT%\lib-user
@xcopy /E /Y /I %~dp0\..\lib-cube\*.* %RBC_ROOT%\lib-cube

@goto END

:ERR
@echo RBC_ROOT is not set

:END
