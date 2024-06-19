echo off

if exist distr rd /Q /S distr 

if exist distr goto failed_remove_old

@set RBC_ROOT=distr\umba-roboconf
@set RBC_BUILD_DISTR_MODE=1


@echo Creating folders
@call %~dp0\scripts\create_distr_folders_impl.bat

@echo Deploy binaries
@rem copy .out\msvc\Win32\Release\umba-roboconf.exe distr\umba-roboconf\bin\umba-roboconf.exe
@call %~dp0\scripts\deploy_binaries_impl.bat

@echo Deploy conf
@call %~dp0\scripts\deploy_conf_impl.bat

@echo Deploy libs
@call %~dp0\scripts\deploy_libs_impl.bat

@echo Deploy rules
@call %~dp0\scripts\deploy_rules_impl.bat

@echo Deploy icons
@call %~dp0\scripts\deploy_icons_impl.bat


@%~dp0\distr\umba-roboconf\bin\umba-roboconf.exe --help >distr\umba-roboconf\doc\readme.txt
@copy /Y doc\*.png distr\umba-roboconf\doc\



@rem copy /Y data\nets\main_board_master_MCU.NET distr\umba-roboconf\samples\netlists\vtx2_master.NET
@rem copy /Y data\nets\main_board_slave.NET      distr\umba-roboconf\samples\netlists\vtx2_slave.NET
@rem copy /Y data\nets\Hot-End_MCU.NET           distr\umba-roboconf\samples\netlists\vtx2_hotend.NET
@rem copy /Y data\nets\TVKO_MCU.NET              distr\umba-roboconf\samples\netlists\vtx2_tvko.NET

@rem copy /Y data\nets-vertel\vtl-MSensor.NET    distr\umba-roboconf\samples\netlists\vtl_msensor.NET
@rem copy /Y data\nets-vertel\vtl-main.NET       distr\umba-roboconf\samples\netlists\vtl_main.NET   
@rem copy /Y data\nets-vertel\vtl-periph.NET     distr\umba-roboconf\samples\netlists\vtl_periph.NET 

@rem mkdir distr\umba-roboconf\samples\rules
@rem copy /Y rules\*.* distr\umba-roboconf\samples\rules

@rem mkdir distr\umba-roboconf\samples\data


@rem echo rem Stub> distr\umba-roboconf\samples\copyConf.bat
@rem echo set ROBOCONF=bin\umba-roboconf.exe> distr\umba-roboconf\samples\setTestConfig.bat
@rem echo set DATAROOT=data>> distr\umba-roboconf\samples\setTestConfig.bat

@rem copy /Y tests\teleshina.bat     distr\umba-roboconf\samples\teleshina.bat
@rem copy /Y tests\teleshina_log.bat distr\umba-roboconf\samples\teleshina_log.bat

@rem mkdir distr\umba-roboconf\samples\data
@rem copy /Y data\Teleshina_Foot_Board.NET distr\umba-roboconf\samples\data\Teleshina_Foot_Board.NET
@rem copy /Y data\Teleshina_Hot_End.NET    distr\umba-roboconf\samples\data\Teleshina_Hot_End.NET   
@rem copy /Y data\Teleshina_Main_Board.NET distr\umba-roboconf\samples\data\Teleshina_Main_Board.NET



echo rem Samples > distr\umba-roboconf\samples\samples.bat
echo rem                                                                                                                                      >> distr\umba-roboconf\samples\samples.bat

@if exist %RBC_ROOT%\conf\umba-roboconf.custom.options                       @del %RBC_ROOT%\conf\umba-roboconf.custom.options
@if exist %RBC_ROOT%\conf\umba-roboconf.MARTINOV-PC_martinov.options         @del %RBC_ROOT%\conf\umba-roboconf.MARTINOV-PC_martinov.options
@if exist %RBC_ROOT%\conf\component-alias-db.custom.txt                      @del %RBC_ROOT%\conf\component-alias-db.custom.txt
@if exist %RBC_ROOT%\conf\component-alias-db.MARTINOV-PC_martinov.txt        @del %RBC_ROOT%\conf\component-alias-db.MARTINOV-PC_martinov.txt
@if exist %RBC_ROOT%\conf\datasheet-alias-db.MARTINOV-PC_martinov.txt        @del %RBC_ROOT%\conf\datasheet-alias-db.MARTINOV-PC_martinov.txt



@set ZIP_TARGET_FOLDER=distr
@set DISTR_NAME=umba-roboconf
@set PLATFORM=x86
@set VERSION=0.9
@set CONFIGURATION=release
@rem if exist distr rd /Q /S distr 
@set ZIPDISTRNAME=%DISTR_NAME%_windows_%PLATFORM%_%CONFIGURATION%_%VERSION%.zip

@rem set ZIP=zip
@set ZIP=7z

@cd %ZIP_TARGET_FOLDER%
@rem echo Calling ZIP: %ZIP% %ZIPDISTRNAME% -r %ZIPDISTRNAME% %DISTR_NAME%
@echo Calling ZIP: %ZIP% a -r %ZIPDISTRNAME% %DISTR_NAME%
@rem %ZIP% a -r %ZIPDISTRNAME% %DISTR_NAME%
@%ZIP% a -r %ZIPDISTRNAME% %DISTR_NAME%
@rem move %ZIPDISTRNAME% ..\..
@rem cd ..\..\..
@cd ..
@rem echo RD_ROOT = %RD_ROOT%
@rem rd /S /Q %RD_ROOT%
if exist ..\releases copy /Y /B distr\%ZIPDISTRNAME% ..\releases\%ZIPDISTRNAME%



goto end

:failed_remove_old
echo Failed to remove previous distributive
goto end



:end


