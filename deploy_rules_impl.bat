@if "%RBC_ROOT%"=="" goto ERR

xcopy /E /Y /I rules\*.*    %RBC_ROOT%\rules

goto END

:ERR
@echo RBC_ROOT is not set

:END
