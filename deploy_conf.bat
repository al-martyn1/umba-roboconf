@if exist %~dp0\setup_local.bat @call %~dp0\setup_local.bat

@set RBC_ROOT=%LOCAL_RBC_ROOT%

@call %~dp0\deploy_conf_impl.bat


