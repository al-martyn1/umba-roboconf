@if exist setup_local.bat @call setup_local.bat

@set RBC_ROOT=%LOCAL_RBC_ROOT%

@call create_distr_folderes.bat
@call deploy_conf_impl.bat
@call deploy_libs_impl.bat
@call deploy_rules_impl.bat
@call deploy_binaries.bat
