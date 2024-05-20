@if "%LOCAL_DATASHEETS_PATH%"=="" goto ERR

@xcopy /E /Y /I %~dp0\..\datasheets\*.*     %LOCAL_DATASHEETS_PATH%\

@goto END

:ERR
@echo LOCAL_DATASHEETS_PATH is not set

:END
