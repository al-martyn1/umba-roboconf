@if "%RBC_ROOT%"=="" goto ERR

@if "%RBC_BUILD_DISTR_MODE%"=="1" @goto BUILD_DISTR_CONF

@rem Copy all files to local folder
@xcopy /E /Y /I %~dp0\..\conf\*.*     "%RBC_ROOT%\conf"
@goto END

:BUILD_DISTR_CONF
@copy /B /Y %~dp0\..\conf\umba-roboconf.options     "%RBC_ROOT%\conf\umba-roboconf.options"
@copy /B /Y %~dp0\..\conf\component-alias-db.txt    "%RBC_ROOT%\conf\component-alias-db.txt"
@copy /B /Y %~dp0\..\conf\datasheet-alias-db.txt    "%RBC_ROOT%\conf\datasheet-alias-db.txt"
@goto END

:ERR
@echo RBC_ROOT is not set

:END
