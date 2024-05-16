@call scripts\detect_compiled_path.bat
@echo RBC_COMPILED_BIN_PATH: %RBC_COMPILED_BIN_PATH%

@if "%RBC_COMPILED_BIN_PATH%"=="" @goto ERR

@set RBC_ROOT=%RBC_COMPILED_BIN_PATH%\..

@call %~dp0\scripts\create_distr_folders_impl.bat
@call %~dp0\scripts\deploy_conf_impl.bat
@call %~dp0\scripts\deploy_libs_impl.bat
@call %~dp0\scripts\deploy_rules_impl.bat
@call %~dp0\scripts\deploy_binaries.bat
@goto END

:ERR
@echo Cant find target path
@goto END


:END