@if exist %~dp0\scripts\setup_local.bat @call %~dp0\scripts\setup_local.bat

@set RBC_ROOT=%LOCAL_RBC_ROOT%

@call %~dp0\scripts\deploy_libs_impl.bat

