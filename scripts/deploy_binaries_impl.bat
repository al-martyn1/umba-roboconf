@if "%RBC_ROOT%"=="" goto ERR_RBC_ROOT

@if not exist %~dp0\..\out\msvc\Win32\Release\roboconf.exe goto TRY_GCC_BINARIES
@set RBC_COMPILED_BIN_PATH=%~dp0\..\out\msvc\Win32\Release
@goto DO_COPY


:TRY_GCC_BINARIES
@if not exist %~dp0\..\.build\GCC-73_32-QT\unknown\Release\roboconf.exe goto ERR_EXE
@set RBC_COMPILED_BIN_PATH=%~dp0\..\.build\GCC-73_32-QT\unknown\Release
@goto DO_COPY
@goto ERR_EXE


:DO_COPY
@copy /Y /B %RBC_COMPILED_BIN_PATH%\roboconf.exe %RBC_ROOT%\bin\roboconf.exe
@goto END


:ERR_RBC_ROOT
@echo RBC_ROOT is not set
@goto END

:ERR_EXE
@echo Binaries not exist
@goto END


:END

