call setTestConfig.bat
call copyConf.bat

if not exist lib-cache mkdir lib-cache
if not exist net-cache mkdir net-cache
if not exist summary   mkdir summary
if not exist mcu       mkdir mcu
if not exist periph    mkdir periph
if not exist h_conf    mkdir h_conf

set VERBOSE=%VERBOSE% --log-source-code-pos
set VERBOSE=-V=9 %VERBOSE% -V=-cls-chk

rem if not exist net-cache\main_board_master_MCU_Calay.rbc_net 
%ROBOCONF% %VERBOSE% --make-cache ../data/nets/main_board_master_MCU_Calay.NET.EDF  net-cache\main_board_master_MCU_Calay.rbc_net
%ROBOCONF% %VERBOSE% --report=summary --local-lib-cache=lib-cache\main_board_master_MCU_Calay.rbc_libs --rules=rules\main_board_master_MCU_Calay.rul net-cache\main_board_master_MCU_Calay.rbc_net  summary\main_board_master_MCU_Calay   
rem %ROBOCONF% -V=9 -- --report=mcu     --local-lib-cache=lib-cache\main_board_master_MCU_Calay.rbc_libs --rules=rules\main_board_master_MCU_Calay.rul net-cache\main_board_master_MCU_Calay.rbc_net  mcu\main_board_master_MCU_Calay
rem %ROBOCONF% -V=9 -- --report=periph  --local-lib-cache=lib-cache\main_board_master_MCU_Calay.rbc_libs --rules=rules\main_board_master_MCU_Calay.rul net-cache\main_board_master_MCU_Calay.rbc_net  periph\main_board_master_MCU_Calay
rem %ROBOCONF% -V=9 -- --report=h_conf  --local-lib-cache=lib-cache\main_board_master_MCU_Calay.rbc_libs --rules=rules\main_board_master_MCU_Calay.rul net-cache\main_board_master_MCU_Calay.rbc_net  h_conf\main_board_master_MCU_Calay

rem   --make-dump -- --report=summary --local-lib-cache=lib-cache\main_board_master_MCU_Calay.rbc_libs --rules=rules\main_board_master_MCU_Calay.rul net-cache\main_board_master_MCU_Calay.rbc_net  summary\main_board_master_MCU_Calay

