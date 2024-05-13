echo off

if exist distr rd /Q /S distr 

if exist distr goto failed_remove_old

mkdir distr
mkdir distr\umba-roboconf
mkdir distr\umba-roboconf\bin
mkdir distr\umba-roboconf\samples
mkdir distr\umba-roboconf\samples\netlists
mkdir distr\umba-roboconf\doc
mkdir distr\umba-roboconf\conf
mkdir distr\umba-roboconf\lib
mkdir distr\umba-roboconf\lib-user
mkdir distr\umba-roboconf\rules

xcopy /E /Y /I lib\*.*      distr\umba-roboconf\lib
xcopy /E /Y /I lib-user\*.* distr\umba-roboconf\lib-user
xcopy /E /Y /I lib-cube\*.* distr\umba-roboconf\lib-cube
xcopy /E /Y /I rules\*.*    distr\umba-roboconf\rules
xcopy /E /Y /I conf\*.*     distr\umba-roboconf\conf


copy /Y data\nets\main_board_master_MCU.NET distr\umba-roboconf\samples\netlists\vtx2_master.NET
copy /Y data\nets\main_board_slave.NET      distr\umba-roboconf\samples\netlists\vtx2_slave.NET
copy /Y data\nets\Hot-End_MCU.NET           distr\umba-roboconf\samples\netlists\vtx2_hotend.NET
copy /Y data\nets\TVKO_MCU.NET              distr\umba-roboconf\samples\netlists\vtx2_tvko.NET

copy /Y data\nets-vertel\vtl-MSensor.NET    distr\umba-roboconf\samples\netlists\vtl_msensor.NET
copy /Y data\nets-vertel\vtl-main.NET       distr\umba-roboconf\samples\netlists\vtl_main.NET   
copy /Y data\nets-vertel\vtl-periph.NET     distr\umba-roboconf\samples\netlists\vtl_periph.NET 

mkdir distr\umba-roboconf\samples\rules
copy /Y rules\*.* distr\umba-roboconf\samples\rules

mkdir distr\umba-roboconf\samples\data


echo rem Stub> distr\umba-roboconf\samples\copyConf.bat
echo set ROBOCONF=bin\roboconf.exe> distr\umba-roboconf\samples\setTestConfig.bat
echo set DATAROOT=data>> distr\umba-roboconf\samples\setTestConfig.bat

copy /Y tests\teleshina.bat     distr\umba-roboconf\samples\teleshina.bat
copy /Y tests\teleshina_log.bat distr\umba-roboconf\samples\teleshina_log.bat

rem mkdir distr\umba-roboconf\samples\data
copy /Y data\Teleshina_Foot_Board.NET distr\umba-roboconf\samples\data\Teleshina_Foot_Board.NET
copy /Y data\Teleshina_Hot_End.NET    distr\umba-roboconf\samples\data\Teleshina_Hot_End.NET   
copy /Y data\Teleshina_Main_Board.NET distr\umba-roboconf\samples\data\Teleshina_Main_Board.NET



echo rem Samples > distr\umba-roboconf\samples\samples.bat
echo rem                                                                                                                                      >> distr\umba-roboconf\samples\samples.bat

echo rem Vertex2 master                                                                                                                       >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_master.NET     vtx2_master_summary.html             >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_master.NET     vtx2_master_mcu.html                 >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_master.rul   netlists\vtx2_master.NET     vtx2_master_periph.html              >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=h_conf   --rules=rules\vtx2_master.rul   netlists\vtx2_master.NET     vtx2_master_conf.h                   >> distr\umba-roboconf\samples\samples.bat

echo rem Vertex2 slave                                                                                                                        >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_slave.NET      vtx2_slave_summary.html              >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_slave.NET      vtx2_slave_mcu.html                  >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_slave.rul    netlists\vtx2_slave.NET      vtx2_slave_periph.html               >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=h_conf   --rules=rules\vtx2_slave.rul    netlists\vtx2_slave.NET      vtx2_slave_conf.h                    >> distr\umba-roboconf\samples\samples.bat

echo rem Vertex2 hot end                                                                                                                      >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_hotend.NET     vtx2_hotend_summary.html             >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_hotend.NET     vtx2_hotend_mcu.html                 >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_hotend.rul   netlists\vtx2_hotend.NET     vtx2_hotend_periph.html              >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=h_conf   --rules=rules\vtx2_hotend.rul   netlists\vtx2_hotend.NET     vtx2_hotend_conf.h                   >> distr\umba-roboconf\samples\samples.bat

echo rem Vertex2 TVKO                                                                                                                         >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_tvko.NET       vtx2_tvko_summary.html               >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_tvko.NET       vtx2_tvko_mcu.html                   >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_tvko.rul     netlists\vtx2_tvko.NET       vtx2_tvko_periph.html                >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=h_conf   --rules=rules\vtx2_tvko.rul     netlists\vtx2_tvko.NET       vtx2_tvko_conf.h                     >> distr\umba-roboconf\samples\samples.bat


echo rem Vertex2 Teleshev main                                                                                                                >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtl_main.NET        vtl_main_summary.html                >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtl_main.NET        vtl_main_mcu.html                    >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtl_main.rul      netlists\vtl_main.NET        vtl_main_periph.html                 >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=h_conf   --rules=rules\vtl_main.rul      netlists\vtl_main.NET        vtl_main_conf.h                      >> distr\umba-roboconf\samples\samples.bat

echo rem Vertex2 Teleshev periph                                                                                                              >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtl_periph.NET      vtl_periph_summary.html              >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtl_periph.NET      vtl_periph_mcu.html                  >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtl_periph.rul    netlists\vtl_periph.NET      vtl_periph_periph.html               >> distr\umba-roboconf\samples\samples.bat
echo ..\bin\roboconf.exe  --report=h_conf   --rules=rules\vtl_periph.rul    netlists\vtl_periph.NET      vtl_periph_conf.h                    >> distr\umba-roboconf\samples\samples.bat



copy out\msvc\Win32\Release\roboconf.exe distr\umba-roboconf\bin\roboconf.exe

distr\umba-roboconf\bin\roboconf.exe --help >distr\umba-roboconf\doc\readme.txt
copy /Y doc\*.png distr\umba-roboconf\doc\


rem xcopy /E /Y /I ..\distr s:\rdlc
rem xcopy /E /Y /I ..\distr e:\rdlc


@set ZIP_TARGET_FOLDER=distr
@set DISTR_NAME=umba-roboconf
@set PLATFORM=x86
@set VERSION=0.9
@set CONFIGURATION=release
@rem if exist distr rd /Q /S distr 
@set ZIPDISTRNAME=%DISTR_NAME%_windows_%PLATFORM%_%CONFIGURATION%_%VERSION%.zip

@cd %ZIP_TARGET_FOLDER%
@echo Calling ZIP: zip %ZIPDISTRNAME% -r %ZIPDISTRNAME% %DISTR_NAME%
@zip %ZIPDISTRNAME% -r %ZIPDISTRNAME% %DISTR_NAME%
@rem move %ZIPDISTRNAME% ..\..
@rem cd ..\..\..
@cd ..
@rem echo RD_ROOT = %RD_ROOT%
@rem rd /S /Q %RD_ROOT%



goto end

:failed_remove_old
echo Failed to remove previous distributive
goto end



:end


