call setTestConfig.bat
call copyConf.bat

if not exist lib-cache mkdir lib-cache
if not exist net-cache mkdir net-cache
if not exist summary   mkdir summary
if not exist mcu       mkdir mcu
if not exist periph    mkdir periph
if not exist h_conf    mkdir h_conf

rem set VERBOSE=-V=9 
set VERBOSE=%VERBOSE% --log-source-code-pos
set VERBOSE=%VERBOSE% -V=-cls-chk

rem if not exist net-cache\wiz2_BLDC_debug_board.rbc_net 
%ROBOCONF% %VERBOSE% --make-cache ../data/nets-wiz2/wiz2_BLDC_debug_board.NET  net-cache\wiz2_BLDC_debug_board.rbc_net
%ROBOCONF% %VERBOSE% --report=summary --local-lib-cache=lib-cache\wiz2_BLDC_debug_board.rbc_libs --rules=rules\wiz2_BLDC_debug_board.rul net-cache\wiz2_BLDC_debug_board.rbc_net  summary\wiz2_BLDC_debug_board   
%ROBOCONF% %VERBOSE% --report=mcu     --local-lib-cache=lib-cache\wiz2_BLDC_debug_board.rbc_libs --rules=rules\wiz2_BLDC_debug_board.rul net-cache\wiz2_BLDC_debug_board.rbc_net  mcu\wiz2_BLDC_debug_board
%ROBOCONF% %VERBOSE% --report=periph  --local-lib-cache=lib-cache\wiz2_BLDC_debug_board.rbc_libs --rules=rules\wiz2_BLDC_debug_board.rul net-cache\wiz2_BLDC_debug_board.rbc_net  periph\wiz2_BLDC_debug_board
%ROBOCONF% %VERBOSE% --report=h_conf  --local-lib-cache=lib-cache\wiz2_BLDC_debug_board.rbc_libs --rules=rules\wiz2_BLDC_debug_board.rul net-cache\wiz2_BLDC_debug_board.rbc_net  h_conf\wiz2_BLDC_debug_board

%ROBOCONF% %VERBOSE% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Motor_Controller.NET  net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net
%ROBOCONF% %VERBOSE% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller.rbc_libs --rules=rules\wiz2_PLATFORM_Motor_Controller.rul net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net  summary\wiz2_PLATFORM_Motor_Controller   
%ROBOCONF% %VERBOSE% --report=mcu     --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller.rbc_libs --rules=rules\wiz2_PLATFORM_Motor_Controller.rul net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net  mcu\wiz2_PLATFORM_Motor_Controller
%ROBOCONF% %VERBOSE% --report=periph  --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller.rbc_libs --rules=rules\wiz2_PLATFORM_Motor_Controller.rul net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net  periph\wiz2_PLATFORM_Motor_Controller
%ROBOCONF% %VERBOSE% --report=h_conf  --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller.rbc_libs --rules=rules\wiz2_PLATFORM_Motor_Controller.rul net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net  h_conf\wiz2_PLATFORM_Motor_Controller

rem PLATFORM_Motor_Controller_MCU.NET
rem wiz2_PLATFORM_Motor_Controller.NET


rem   --make-dump -- --report=summary --local-lib-cache=lib-cache\wiz2_BLDC_debug_board.rbc_libs --rules=rules\wiz2_BLDC_debug_board.rul net-cache\wiz2_BLDC_debug_board.rbc_net  summary\wiz2_BLDC_debug_board
