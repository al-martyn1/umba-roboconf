@if "%RBC_ROOT%"=="" goto ERR_RBC_ROOT

@call %~dp0detect_compiled_path.bat

@rem if not exist %~dp0\..\out\msvc\Win32\Release\roboconf.exe goto TRY_GCC_BINARIES
@rem set RBC_COMPILED_BIN_PATH=%~dp0\..\out\msvc\Win32\Release
@rem goto DO_COPY


:TRY_GCC_BINARIES
@rem if not exist %~dp0\..\.build\GCC-73_32-QT\none-unknown\Release\roboconf.exe goto ERR_EXE
@rem set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC-73_32-QT\none-unknown\Release
@rem goto DO_COPY
@rem goto ERR_EXE


:DO_COPY
@copy /Y /B %RBC_COMPILED_BIN_PATH%\roboconf.exe %RBC_ROOT%\bin\roboconf.exe

if not exist %~dp0\..\.build\GCC*QT goto GCC_RUNTIME_SKIPPED
if exist %~dp0\setup_path_qt5_compiler_32.bat call %~dp0\setup_path_qt5_compiler_32.bat
copy /Y /B %QT_MINGW32_BIN_PATH%\libgcc_s_dw2-1.dll %RBC_ROOT%\bin\libgcc_s_dw2-1.dll
copy /Y /B %QT_MINGW32_BIN_PATH%\libwinpthread-1.dll %RBC_ROOT%\bin\libwinpthread-1.dll
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


