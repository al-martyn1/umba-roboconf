@rem if not exist doc.generated mkdir doc.generated
@rem if not exist src           mkdir src

@rem --dont-warn=cls-chk-not-matched --dont-warn=conf-dump-short-rules-cls
@rem cls-chk-not-at-all



@call setTestConfig.bat
@call copyConf.bat


@rem roboconf.exe  --make-dump -V=9 --log-source-code-pos -V=-conf-dump-short-rules-cls -V=-conf-dump-rules-cls -V=-data-dump-short -V=-net-chkstp-not-matched --report=periph   --rules=es.rul  doc\schematic\ES-4007-BMCL-CPU_R.1.0.NET  doc.generated\ES-4007-BMCL-CPU_R_1_0_periph.html


@set VERBOSE=-V=9 
@rem set VERBOSE=%VERBOSE% -V=-search-lib-log -V=-search-lib-summary
@rem set VERBOSE=%VERBOSE% -V=-net-dump-pins -V=-net-dump-nets -V=-net-dump-cmn -V=-net-dump-cmps
@rem set VERBOSE=%VERBOSE% -V=-cls-chk-not-matched -V=-cls-chk-matched
@rem set VERBOSE=%VERBOSE% -V=-pkg-known
@set VERBOSE=%VERBOSE% --log-source-code-pos --make-dump

@call mk_out_dirs.bat
@if not exist es @mkdir es


%ROBOCONF% %VERBOSE% --make-cache ../data/nets/es.NET  net-cache\es.rbc_net

%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\es.rbc_libs --rules=rules\es.rul  net-cache\es.rbc_net  es\summary.html
@rem summary\jud_TriggersControl_v2_3_Driver
%ROBOCONF% %VERBOSE% --make-dump --report=mcu     --local-lib-cache=lib-cache\es.rbc_libs                       net-cache\es.rbc_net  es\mcu.html
@rem mcu\jud_TriggersControl_v2_3_Driver
%ROBOCONF% %VERBOSE% --make-dump --report=periph  --local-lib-cache=lib-cache\es.rbc_libs --rules=rules\es.rul  net-cache\es.rbc_net  es\periph.html
@rem periph\jud_TriggersControl_v2_3_Driver
%ROBOCONF% %VERBOSE% --make-dump --report=h_conf  --local-lib-cache=lib-cache\es.rbc_libs --rules=rules\es.rul  net-cache\es.rbc_net  es\conf.h
@rem h_conf\jud_TriggersControl_v2_3_Driver



@rem roboconf.exe  --help > roboconf_help.txt
@rem roboconf.exe  %LOGLVL% --report=summary                                  doc\schematic\es.NET     doc.generated\ES-4007-BMCL-CPU_R_1_0_summary.html > summary.log 2>&1
@rem roboconf.exe  %LOGLVL% --report=mcu                                      doc\schematic\es.NET     doc.generated\ES-4007-BMCL-CPU_R_1_0_mcu.html > mcu.log 2>&1

@rem rem --rules=rules\vtx2_master.rul
@rem set RULES_IN=
@rem roboconf.exe  %LOGLVL% --report=periph  %RULES_IN%    doc\schematic\ES-4007-BMCL-CPU_R.1.0.NET     doc.generated\ES-4007-BMCL-CPU_R_1_0_periph.html > periph.log 2>&1
@rem roboconf.exe  %LOGLVL% --report=h_conf  %RULES_IN%    doc\schematic\ES-4007-BMCL-CPU_R.1.0.NET     src\periph_config.h > periph_config.log 2>&1
