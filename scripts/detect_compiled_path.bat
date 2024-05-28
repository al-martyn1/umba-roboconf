
:TRY_MSVC_RELEASE
@if not exist %~dp0\..\.out\msvc\Win32\Release\umba-roboconf.exe goto TRY_MSVC_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc\Win32\Release
@goto DO_COPY


:TRY_MSVC_DEBUG
@if not exist %~dp0\..\.out\msvc\Win32\Debug\umba-roboconf.exe goto TRY_GCC_V1_RELEASE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc\Win32\Debug
@goto DO_COPY


:TRY_GCC_V1_RELEASE
@if not exist "%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Release\umba-roboconf.exe" goto TRY_GCC_V1_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Release
@goto DO_COPY


:TRY_GCC_V1_DEBUG
@if not exist "%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Debug\umba-roboconf.exe" goto TRY_GCC_V2_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Debug
@goto DO_COPY


:TRY_GCC_V2_RELEASE
@if not exist "%~dp0\..\.build\GCC-73_32-QT\none-unknown\Release\umba-roboconf.exe" goto TRY_GCC_V2_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC-73_32-QT\none-unknown\Release
@goto DO_COPY


:TRY_GCC_V2_DEBUG
@if not exist "%~dp0\..\.build\GCC-73_32-QT\none-unknown\Debug\umba-roboconf.exe" goto ERR_EXE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC-73_32-QT\none-unknown\Debug
@goto DO_COPY


@goto ERR_EXE


:DO_COPY
@rem copy /Y /B "%RBC_COMPILED_BIN_PATH%\umba-roboconf.exe" "%RBC_ROOT%\bin\umba-roboconf.exe"
@goto END


:ERR_EXE
@echo Binaries not exist
@goto END


:END

