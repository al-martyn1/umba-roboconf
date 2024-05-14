@if "%RBC_ROOT%"=="" goto ERR

xcopy /E /Y /I conf\*.*     %RBC_ROOT%\conf

goto END

:ERR
@echo RBC_ROOT is not set

:END
