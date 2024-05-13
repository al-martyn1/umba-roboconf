@call setTestConfig.bat
@call copyConf.bat

@set VERBOSE=-V=9
@rem set VERBOSE=%VERBOSE% -V=-search-lib-log -V=-search-lib-summary
@rem set VERBOSE=%VERBOSE% -V=-net-dump-pins -V=-net-dump-nets -V=-net-dump-cmn -V=-net-dump-cmps
@rem set VERBOSE=%VERBOSE% -V=-cls-chk-not-matched -V=-cls-chk-matched
@rem set VERBOSE=%VERBOSE% -V=-pkg-known
@set VERBOSE=%VERBOSE% --log-source-code-pos

@call mk_out_dirs.bat

%ROBOCONF% %VERBOSE% --make-cache ../data/nets-jud/jud_TriggersControl_v2_3_Driver.NET  net-cache\jud_TriggersControl_v2_3_Driver.rbc_net

%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs --rules=rules\jud_TriggersControl_v2_3_Driver.rul net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  summary\jud_TriggersControl_v2_3_Driver
%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs  net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  mcu\jud_TriggersControl_v2_3_Driver
%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs --rules=rules\jud_TriggersControl_v2_3_Driver.rul  net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  periph\jud_TriggersControl_v2_3_Driver
%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs --rules=rules\jud_TriggersControl_v2_3_Driver.rul  net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  h_conf\jud_TriggersControl_v2_3_Driver



