@if exist %~dp0\scripts\setup_local.bat @call %~dp0\scripts\setup_local.bat

@set RBC_ROOT=%LOCAL_RBC_ROOT%

@call %~dp0\scripts\create_distr_folders_impl.bat
@call %~dp0\scripts\deploy_conf_impl.bat
@call %~dp0\scripts\deploy_libs_impl.bat
@call %~dp0\scripts\deploy_rules_impl.bat
@call %~dp0\scripts\deploy_binaries_impl.bat
