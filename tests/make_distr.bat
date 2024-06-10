echo off

if exist ..\distr rd /Q /S ..\distr 

if exist ..\distr goto failed_remove_old

mkdir ..\distr
mkdir ..\distr\bin
mkdir ..\distr\samples
mkdir ..\distr\samples\netlists
mkdir ..\distr\doc
mkdir ..\distr\conf
mkdir ..\distr\lib
mkdir ..\distr\lib-user
mkdir ..\distr\rules

xcopy /E /Y /I ..\lib\*.* ..\distr\lib
xcopy /E /Y /I ..\lib-user\*.* ..\distr\lib-user
xcopy /E /Y /I ..\lib-cube\*.* ..\distr\lib-cube
xcopy /E /Y /I ..\rules\*.* ..\distr\rules
xcopy /E /Y /I ..\conf\*.* ..\distr\conf


copy /Y ..\data\nets\main_board_master_MCU.NET ..\distr\samples\netlists\vtx2_master.NET
copy /Y ..\data\nets\main_board_slave.NET      ..\distr\samples\netlists\vtx2_slave.NET
copy /Y ..\data\nets\Hot-End_MCU.NET           ..\distr\samples\netlists\vtx2_hotend.NET
copy /Y ..\data\nets\TVKO_MCU.NET              ..\distr\samples\netlists\vtx2_tvko.NET

copy /Y ..\data\nets-vertel\vtl-MSensor.NET              ..\distr\samples\netlists\vtl_msensor.NET
copy /Y ..\data\nets-vertel\vtl-main.NET                 ..\distr\samples\netlists\vtl_main.NET   
copy /Y ..\data\nets-vertel\vtl-periph.NET               ..\distr\samples\netlists\vtl_periph.NET 

mkdir ..\distr\samples\rules
copy /Y rules\*.* ..\distr\samples\rules

mkdir ..\distr\samples\data


echo rem Stub> ..\distr\samples\copyConf.bat
echo set ROBOCONF=..\bin\roboconf.exe> ..\distr\samples\setTestConfig.bat
echo set DATAROOT=data>> ..\distr\samples\setTestConfig.bat

copy /Y teleshina.bat     ..\distr\samples\teleshina.bat
copy /Y teleshina_log.bat ..\distr\samples\teleshina_log.bat

mkdir ..\distr\samples\data
copy /Y ..\data\Teleshina_Foot_Board.NET ..\distr\samples\data\Teleshina_Foot_Board.NET
copy /Y ..\data\Teleshina_Hot_End.NET    ..\distr\samples\data\Teleshina_Hot_End.NET   
copy /Y ..\data\Teleshina_Main_Board.NET ..\distr\samples\data\Teleshina_Main_Board.NET



echo rem Samples > ..\distr\samples\samples.bat
echo rem                                                                                                                                      >> ..\distr\samples\samples.bat

echo rem Vertex2 master                                                                                                                       >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_master.NET     vtx2_master_summary.html             >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_master.NET     vtx2_master_mcu.html                 >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_master.rul   netlists\vtx2_master.NET     vtx2_master_periph.html              >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=umba_periph_config   --rules=rules\vtx2_master.rul   netlists\vtx2_master.NET     vtx2_master_conf.h                   >> ..\distr\samples\samples.bat

echo rem Vertex2 slave                                                                                                                        >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_slave.NET      vtx2_slave_summary.html              >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_slave.NET      vtx2_slave_mcu.html                  >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_slave.rul    netlists\vtx2_slave.NET      vtx2_slave_periph.html               >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=umba_periph_config   --rules=rules\vtx2_slave.rul    netlists\vtx2_slave.NET      vtx2_slave_conf.h                    >> ..\distr\samples\samples.bat

echo rem Vertex2 hot end                                                                                                                      >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_hotend.NET     vtx2_hotend_summary.html             >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_hotend.NET     vtx2_hotend_mcu.html                 >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_hotend.rul   netlists\vtx2_hotend.NET     vtx2_hotend_periph.html              >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=umba_periph_config   --rules=rules\vtx2_hotend.rul   netlists\vtx2_hotend.NET     vtx2_hotend_conf.h                   >> ..\distr\samples\samples.bat

echo rem Vertex2 TVKO                                                                                                                         >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtx2_tvko.NET       vtx2_tvko_summary.html               >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtx2_tvko.NET       vtx2_tvko_mcu.html                   >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtx2_tvko.rul     netlists\vtx2_tvko.NET       vtx2_tvko_periph.html                >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=umba_periph_config   --rules=rules\vtx2_tvko.rul     netlists\vtx2_tvko.NET       vtx2_tvko_conf.h                     >> ..\distr\samples\samples.bat


echo rem Vertex2 Teleshev main                                                                                                                >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtl_main.NET        vtl_main_summary.html                >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtl_main.NET        vtl_main_mcu.html                    >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtl_main.rul      netlists\vtl_main.NET        vtl_main_periph.html                 >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=umba_periph_config   --rules=rules\vtl_main.rul      netlists\vtl_main.NET        vtl_main_conf.h                      >> ..\distr\samples\samples.bat

echo rem Vertex2 Teleshev periph                                                                                                              >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=summary                                  netlists\vtl_periph.NET      vtl_periph_summary.html              >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=mcu                                      netlists\vtl_periph.NET      vtl_periph_mcu.html                  >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=periph   --rules=rules\vtl_periph.rul    netlists\vtl_periph.NET      vtl_periph_periph.html               >> ..\distr\samples\samples.bat
echo ..\bin\roboconf.exe  --report=umba_periph_config   --rules=rules\vtl_periph.rul    netlists\vtl_periph.NET      vtl_periph_conf.h                    >> ..\distr\samples\samples.bat



copy ..\.out\msvc\Win32\Release\roboconf.exe ..\distr\bin\roboconf.exe

..\distr\bin\roboconf.exe --help >..\distr\doc\readme.txt
copy /Y ..\doc\*.png ..\distr\doc\


rem xcopy /E /Y /I ..\distr s:\rdlc
rem xcopy /E /Y /I ..\distr e:\rdlc

goto end

:failed_remove_old
echo Failed to remove previous distributive
goto end





:end


