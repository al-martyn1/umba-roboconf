rem echo off
call setTestConfig.bat
call copyConf.bat

del /S /Q ..\lib-cube\*.* >nul 2>&1
if exist "C:\Program Files (x86)\STMicroelectronics\STM32Cube" set CUBE_ROOT="C:\Program Files (x86)\STMicroelectronics\STM32Cube"
if exist "C:\Program Files\STMicroelectronics\STM32Cube"       set CUBE_ROOT="C:\Program Files\STMicroelectronics\STM32Cube"
if exist "D:\Program Files\STMicroelectronics\STM32Cube"       set CUBE_ROOT="D:\Program Files\STMicroelectronics\STM32Cube"
rem %CUBE2CMP% "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeMX" ..\lib-cube 2> log_import_cubes.txt 1> import_cubes.txt
rem --cmp
rem --detect

rem unconditional generate cmps
%CUBE2CMP% --cmp %CUBE_ROOT%\STM32CubeMX ..\lib-cube > log_import_cubes.txt 2>&1
rem F:\_easymcu\doc_scripts\ 
rem save PDFs to F:\_easymcu\Doc\ 

rem %CUBE2CMP% --detect --doclistdatasheet --doclistrefman --doclisterrata --doclistappnotes --doclistprogman %CUBE_ROOT%\STM32CubeMX ..\lib-cube > log_import_cubes.txt 2>&1
%CUBE2CMP% --wgetdatasheet --wgetrefman --wgeterrata --wgetappnotes --wgetprogman %CUBE_ROOT%\STM32CubeMX F:\_easymcu\doc_scripts > log_import_cubes.txt 2>&1



