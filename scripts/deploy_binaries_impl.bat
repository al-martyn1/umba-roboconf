@if "%RBC_ROOT%"=="" goto ERR_RBC_ROOT

@call %~dp0detect_compiled_path.bat


:DO_COPY
copy /Y /B "%RBC_COMPILED_BIN_PATH%\umba-roboconf.exe" "%RBC_ROOT%\bin\umba-roboconf.exe"

if not exist "%~dp0\..\.build\GCC*" goto GCC_RUNTIME_SKIPPED
if exist %~dp0\setup_path_qt5_compiler_32.bat call %~dp0\setup_path_qt5_compiler_32.bat
copy /Y /B "%QT_MINGW32_BIN_PATH%\libgcc_s_dw2-1.dll"  "%RBC_ROOT%\bin\libgcc_s_dw2-1.dll"
copy /Y /B "%QT_MINGW32_BIN_PATH%\libwinpthread-1.dll" "%RBC_ROOT%\bin\libwinpthread-1.dll"
copy /Y /B "%QT_MINGW32_BIN_PATH%\libstdc++-6.dll" "%RBC_ROOT%\bin\libstdc++-6.dll"

:GCC_RUNTIME_SKIPPED

@goto END


:ERR_RBC_ROOT
@echo RBC_ROOT is not set
@goto END

:ERR_EXE
@echo Binaries not exist
@goto END


:END


