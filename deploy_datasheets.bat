@if exist %~dp0\setup_local_datasheets.bat @call %~dp0\setup_local_datasheets.bat

@rem set RBC_ROOT=%LOCAL_RBC_ROOT%

@call %~dp0\deploy_datasheets_impl.bat


