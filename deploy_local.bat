@if exist setup_local.bat @call setup_local.bat

@set RBC_ROOT=%LOCAL_RBC_ROOT%

@call create_distr_folderes.bat
@call deploy_conf_libs_rules.bat
