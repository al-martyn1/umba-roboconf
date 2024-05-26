echo off

if exist distr rd /Q /S distr 

if exist distr goto failed_remove_old

@set RBC_ROOT=distr\umba-roboconf


@echo Creating folders
@call scripts\create_distr_folders_impl.bat

@echo Deploy binaries
@rem copy .out\msvc\Win32\Release\umba-roboconf.exe distr\umba-roboconf\bin\umba-roboconf.exe
@call scripts\deploy_binaries_impl.bat

@echo Deploy conf
@call scripts\deploy_conf_impl.bat

@echo Deploy libs
@call scripts\deploy_libs_impl.bat

@echo Deploy rules
@call scripts\deploy_rules_impl.bat


@distr\umba-roboconf\bin\umba-roboconf.exe --help >distr\umba-roboconf\doc\readme.txt
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

@if exist %RBC_ROOT%\conf\component-alias-db.custom.txt                 @del %RBC_ROOT%\conf\component-alias-db.custom.txt
@if exist %RBC_ROOT%\conf\roboconf.MARTINOV-PC_martinov.options         @del %RBC_ROOT%\conf\roboconf.MARTINOV-PC_martinov.options
@if exist %RBC_ROOT%\conf\component-alias-db.MARTINOV-PC_martinov.txt   @del %RBC_ROOT%\conf\component-alias-db.MARTINOV-PC_martinov.txt
@if exist %RBC_ROOT%\conf\datasheet-alias-db.MARTINOV-PC_martinov.txt   @del %RBC_ROOT%\conf\datasheet-alias-db.MARTINOV-PC_martinov.txt


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
@rem echo Calling ZIP: zip %ZIPDISTRNAME% -r %ZIPDISTRNAME% %DISTR_NAME%
@echo Calling ZIP: zip -r %ZIPDISTRNAME% %DISTR_NAME%
@rem zip %ZIPDISTRNAME% -r %ZIPDISTRNAME% %DISTR_NAME%
@zip -r %ZIPDISTRNAME% %DISTR_NAME%
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


