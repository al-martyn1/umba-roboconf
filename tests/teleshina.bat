call setTestConfig.bat
call copyConf.bat

set VERBOSE=-V=grp-dump
rem set VERBOSE=-V=9
rem set VERBOSE=%VERBOSE% -V=-search-lib-log -V=-search-lib-summary
rem set VERBOSE=%VERBOSE% -V=-net-dump-pins -V=-net-dump-nets -V=-net-dump-cmn -V=-net-dump-cmps
rem set VERBOSE=%VERBOSE% -V=-cls-chk-not-matched -V=-cls-chk-matched
rem set VERBOSE=%VERBOSE% -V=-pkg-known
rem set VERBOSE=-V=0 --log-source-code-pos

if not exist reports mkdir reports
if not exist reports\Teleshina mkdir reports\Teleshina

if not exist reports\Teleshina\Foot_Board.rbc_net   %ROBOCONF% %VERBOSE% --make-cache %DATAROOT%/Teleshina_Foot_Board.NET   reports\Teleshina\Foot_Board.rbc_net
if not exist reports\Teleshina\Hot_End.rbc_net      %ROBOCONF% %VERBOSE% --make-cache %DATAROOT%/Teleshina_Hot_End.NET      reports\Teleshina\Hot_End.rbc_net
if not exist reports\Teleshina\Main_Board.rbc_net   %ROBOCONF% %VERBOSE% --make-cache %DATAROOT%/Teleshina_Main_Board.NET   reports\Teleshina\Main_Board.rbc_net

rem --local-lib-cache=Teleshina\Foot_Board.rbc_libs
rem --local-lib-cache=Teleshina\Hot_End.rbc_libs   
rem --local-lib-cache=Teleshina\Main_Board.rbc_libs

%ROBOCONF% %VERBOSE% --make-dump --report=summary                 --rules=rules\Teleshina_Foot_Board.rul   reports\Teleshina\Foot_Board.rbc_net   reports\Teleshina\Foot_Board_summary
%ROBOCONF% %VERBOSE% --make-dump --report=mcu                     --rules=rules\Teleshina_Foot_Board.rul   reports\Teleshina\Foot_Board.rbc_net   reports\Teleshina\Foot_Board_mcu
%ROBOCONF% %VERBOSE% --make-dump --report=periph                  --rules=rules\Teleshina_Foot_Board.rul   reports\Teleshina\Foot_Board.rbc_net   reports\Teleshina\Foot_Board_periph
%ROBOCONF% %VERBOSE% --make-dump --report=h_conf                  "--prolog=inline:#include ""periph/gpio.h"""     --rules=rules\Teleshina_Foot_Board.rul   reports\Teleshina\Foot_Board.rbc_net   reports\Teleshina\Foot_Board_conf
%ROBOCONF% %VERBOSE% --make-dump --report=cpp_pins_snake          "--prolog=inline:#include ""Foot_Board_conf.h""" --rules=rules\Teleshina_Foot_Board.rul   reports\Teleshina\Foot_Board.rbc_net   reports\Teleshina\Foot_Board_pins
%ROBOCONF% %VERBOSE% --make-dump --report=extern_cpp_pins_snake   "--prolog=inline:#include ""Foot_Board_conf.h""" --rules=rules\Teleshina_Foot_Board.rul   reports\Teleshina\Foot_Board.rbc_net   reports\Teleshina\Foot_Board_pins

%ROBOCONF% %VERBOSE% --make-dump --report=summary                 --rules=rules\Teleshina_Hot_End.rul      reports\Teleshina\Hot_End.rbc_net      reports\Teleshina\Hot_End_summary
%ROBOCONF% %VERBOSE% --make-dump --report=mcu                     --rules=rules\Teleshina_Hot_End.rul      reports\Teleshina\Hot_End.rbc_net      reports\Teleshina\Hot_End_mcu
%ROBOCONF% %VERBOSE% --make-dump --report=periph                  --rules=rules\Teleshina_Hot_End.rul      reports\Teleshina\Hot_End.rbc_net      reports\Teleshina\Hot_End_periph
%ROBOCONF% %VERBOSE% --make-dump --report=h_conf                  "--prolog=inline:#include ""periph/gpio.h"""  --rules=rules\Teleshina_Hot_End.rul      reports\Teleshina\Hot_End.rbc_net      reports\Teleshina\Hot_End_conf
%ROBOCONF% %VERBOSE% --make-dump --report=cpp_pins_snake          "--prolog=inline:#include ""Hot_End_conf.h""" --rules=rules\Teleshina_Hot_End.rul      reports\Teleshina\Hot_End.rbc_net      reports\Teleshina\Hot_End_pins
%ROBOCONF% %VERBOSE% --make-dump --report=extern_cpp_pins_snake   "--prolog=inline:#include ""Hot_End_conf.h""" --rules=rules\Teleshina_Hot_End.rul      reports\Teleshina\Hot_End.rbc_net      reports\Teleshina\Hot_End_pins

%ROBOCONF% %VERBOSE% --make-dump --report=summary                 --rules=rules\Teleshina_Main_Board.rul   reports\Teleshina\Main_Board.rbc_net   reports\Teleshina\Main_Board_summary
%ROBOCONF% %VERBOSE% --make-dump --report=mcu                     --rules=rules\Teleshina_Main_Board.rul   reports\Teleshina\Main_Board.rbc_net   reports\Teleshina\Main_Board_mcu
%ROBOCONF% %VERBOSE% --make-dump --report=periph                  --rules=rules\Teleshina_Main_Board.rul   reports\Teleshina\Main_Board.rbc_net   reports\Teleshina\Main_Board_periph
%ROBOCONF% %VERBOSE% --make-dump --report=h_conf                "--prolog=inline:#include ""periph/gpio.h"""      --rules=rules\Teleshina_Main_Board.rul   reports\Teleshina\Main_Board.rbc_net   reports\Teleshina\Main_Board_conf
%ROBOCONF% %VERBOSE% --make-dump --report=cpp_pins_snake        "--prolog=inline:#include ""Main_Board_conf.h"""  --rules=rules\Teleshina_Main_Board.rul   reports\Teleshina\Main_Board.rbc_net   reports\Teleshina\Main_Board_pins
%ROBOCONF% %VERBOSE% --make-dump --report=extern_cpp_pins_snake "--prolog=inline:#include ""Main_Board_conf.h"""  --rules=rules\Teleshina_Main_Board.rul   reports\Teleshina\Main_Board.rbc_net   reports\Teleshina\Main_Board_pins


if exist reports\Teleshina\Foot_Board_summary.htm      copy /Y reports\Teleshina\Foot_Board_summary.htm        reports\Teleshina\summary_Foot_Board.htm 
if exist reports\Teleshina\Hot_End_summary.htm         copy /Y reports\Teleshina\Hot_End_summary.htm           reports\Teleshina\summary_Hot_End.htm    
if exist reports\Teleshina\Main_Board_summary.htm      copy /Y reports\Teleshina\Main_Board_summary.htm        reports\Teleshina\summary_Main_Board.htm 
                                                                                                                         
if exist reports\Teleshina\Foot_Board_mcu.htm          copy /Y reports\Teleshina\Foot_Board_mcu.htm            reports\Teleshina\mcu_Foot_Board.htm     
if exist reports\Teleshina\Hot_End_mcu.htm             copy /Y reports\Teleshina\Hot_End_mcu.htm               reports\Teleshina\mcu_Hot_End.htm        
if exist reports\Teleshina\Main_Board_mcu.htm          copy /Y reports\Teleshina\Main_Board_mcu.htm            reports\Teleshina\mcu_Main_Board.htm     
                                                                                                                         
if exist reports\Teleshina\Foot_Board_periph.htm       copy /Y reports\Teleshina\Foot_Board_periph.htm         reports\Teleshina\periph_Foot_Board.htm  
if exist reports\Teleshina\Hot_End_periph.htm          copy /Y reports\Teleshina\Hot_End_periph.htm            reports\Teleshina\periph_Hot_End.htm     
if exist reports\Teleshina\Main_Board_periph.htm       copy /Y reports\Teleshina\Main_Board_periph.htm         reports\Teleshina\periph_Main_Board.htm  
                                                                                                                         
if exist reports\Teleshina\Foot_Board_h_conf.htm       copy /Y reports\Teleshina\Foot_Board_h_conf.htm         reports\Teleshina\conf_Foot_Board_h.htm  
if exist reports\Teleshina\Hot_End_h_conf.htm          copy /Y reports\Teleshina\Hot_End_h_conf.htm            reports\Teleshina\conf_Hot_End_h.htm     
if exist reports\Teleshina\Main_Board_h_conf.htm       copy /Y reports\Teleshina\Main_Board_h_conf.htm         reports\Teleshina\conf_Main_Board_h.htm  
