@if exist %~dp0\scripts\setup_local_datasheets.bat @call %~dp0\scripts\setup_local_datasheets.bat

@rem set RBC_ROOT=%LOCAL_RBC_ROOT%

@call %~dp0\scripts\deploy_datasheets_impl.bat


