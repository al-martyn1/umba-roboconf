@if "%RBC_ROOT%"=="" goto ERR

@xcopy /E /Y /I %~dp0\..\rules\*.*    %RBC_ROOT%\rules

@goto END

:ERR
@echo RBC_ROOT is not set

:END
