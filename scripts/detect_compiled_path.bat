
:TRY_MSVC_RELEASE
@if not exist %~dp0\..\out\msvc\Win32\Release\roboconf.exe goto TRY_MSVC_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\out\msvc\Win32\Release
@goto DO_COPY


:TRY_MSVC_DEBUG
@if not exist %~dp0\..\out\msvc\Win32\Debug\roboconf.exe goto TRY_GCC_RELEASE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\out\msvc\Win32\Debug
@goto DO_COPY


:TRY_GCC_RELEASE
@if not exist %~dp0\..\.build\GCC-73_32-QT\none-unknown\Release\roboconf.exe goto TRY_GCC_DEBUG
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC-73_32-QT\unknown\Release
@goto DO_COPY


:TRY_GCC_DEBUG
@if not exist %~dp0\..\.build\GCC-73_32-QT\none-unknown\Debug\roboconf.exe goto ERR_EXE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC-73_32-QT\unknown\Debug
@goto DO_COPY
@goto ERR_EXE


:DO_COPY
@rem copy /Y /B %RBC_COMPILED_BIN_PATH%\roboconf.exe %RBC_ROOT%\bin\roboconf.exe
@goto END


:ERR_EXE
@echo Binaries not exist
@goto END


:END

