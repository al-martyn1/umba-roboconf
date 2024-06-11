
:TRY_MSVC2022_RELEASE
@if not exist %~dp0\..\.out\msvc2022\Win32\Release\umba-roboconf.exe goto TRY_MSVC2022_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc2022\Win32\Release
@goto DO_COPY


:TRY_MSVC2022_DEBUG
@if not exist %~dp0\..\.out\msvc2022\Win32\Debug\umba-roboconf.exe goto TRY_MSVC2019_RELEASE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc2022\Win32\Debug
@goto DO_COPY



:TRY_MSVC2019_RELEASE
@if not exist %~dp0\..\.out\msvc2019\Win32\Release\umba-roboconf.exe goto TRY_MSVC2019_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc2019\Win32\Release
@goto DO_COPY


:TRY_MSVC2019_DEBUG
@if not exist %~dp0\..\.out\msvc2019\Win32\Debug\umba-roboconf.exe goto TRY_MSVC2017_RELEASE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc2019\Win32\Debug
@goto DO_COPY



:TRY_MSVC2017_RELEASE
@if not exist %~dp0\..\.out\msvc2017\Win32\Release\umba-roboconf.exe goto TRY_MSVC2017_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc2017\Win32\Release
@goto DO_COPY


:TRY_MSVC2017_DEBUG
@if not exist %~dp0\..\.out\msvc2017\Win32\Debug\umba-roboconf.exe goto TRY_GCC_V7_3_V1_RELEASE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.out\msvc2017\Win32\Debug
@goto DO_COPY




:TRY_GCC_V7_3_V1_RELEASE
@if not exist "%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Release\umba-roboconf.exe" goto TRY_GCC_V7_3_V1_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Release
@goto DO_COPY


:TRY_GCC_V7_3_V1_DEBUG
@if not exist "%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Debug\umba-roboconf.exe" goto TRY_GCC_V7_3_V2_RELEASE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC 7.3.0 i686-w64-mingw32\win32-x86\Debug
@goto DO_COPY


:TRY_GCC_V7_3_V2_RELEASE
@if not exist "%~dp0\..\.build\GCC-73_32-QT\none-unknown\Release\umba-roboconf.exe" goto TRY_GCC_V7_3_V2_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC-73_32-QT\none-unknown\Release
@goto DO_COPY


:TRY_GCC_V7_3_V2_DEBUG
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

