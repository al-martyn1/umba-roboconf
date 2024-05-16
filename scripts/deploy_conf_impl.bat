@if "%RBC_ROOT%"=="" goto ERR

xcopy /E /Y /I %~dp0\..\conf\*.*     %RBC_ROOT%\conf

goto END

:ERR
@echo RBC_ROOT is not set

:END
