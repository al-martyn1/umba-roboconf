@call make_distr.bat
@xcopy /E /Y /I ..\distr \\dep111\Releases\roboconf_manual_build
@call copyDatasheetsToShared.bat
