@if "%RBC_ROOT%"=="" goto ERR

xcopy /E /Y /I lib\*.*      %RBC_ROOT%\lib
xcopy /E /Y /I lib-user\*.* %RBC_ROOT%\lib-user
xcopy /E /Y /I lib-cube\*.* %RBC_ROOT%\lib-cube

goto END

:ERR
@echo RBC_ROOT is not set

:END
