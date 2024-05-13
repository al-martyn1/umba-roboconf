@set TEST_CONFIG=Release
@rem set TEST_CONFIG=Debug

@set OUT_BASE=..\out\msvc\Win32

@set ROBOCONF=%OUT_BASE%\%TEST_CONFIG%\roboconf.exe
@set PAGEN=%OUT_BASE%\%TEST_CONFIG%\sm32_pa_gen.exe
@set CUBE2CMP=%OUT_BASE%\%TEST_CONFIG%\cube2cmp.exe

@set DATAROOT=..\data