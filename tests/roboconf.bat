call setTestConfig.bat
call copyConf.bat

set VERBOSE=-V=9
rem set VERBOSE=%VERBOSE% -V=-search-lib-log -V=-search-lib-summary
set VERBOSE=%VERBOSE% -V=-net-dump-pins -V=-net-dump-nets -V=-net-dump-cmn -V=-net-dump-cmps
set VERBOSE=%VERBOSE% -V=-cls-chk-not-matched -V=-cls-chk-matched
rem set VERBOSE=%VERBOSE% -V=-pkg-known
rem set VERBOSE=-V=0 --log-source-code-pos


call mk_out_dirs.bat


rem goto summary
rem goto nets_cached

%ROBOCONF% --help > roboconf_help.txt

call roboconf_mk_netcache.bat


:nets_cached
:summary
rem goto mcu


rem --verbose-lib-search
%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\samples.rbc_libs      --rules=rules\samples.rul       net-cache\samples.rbc_net         summary\samples
                                                                                                                                                
%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\vtx2_master.rbc_libs  --rules=rules\vtx2_master.rul   net-cache\vtx2_master.rbc_net     summary\vtx2_master 
%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\vtx2_slave.rbc_libs   --rules=rules\vtx2_slave.rul    net-cache\vtx2_slave.rbc_net      summary\vtx2_slave  
%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\vtx2_hotend.rbc_libs  --rules=rules\vtx2_hotend.rul   net-cache\vtx2_hotend.rbc_net     summary\vtx2_hotend 
%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\vtx2_tvko.rbc_libs    --rules=rules\vtx2_tvko.rul     net-cache\vtx2_tvko.rbc_net       summary\vtx2_tvko   
                                                                                                                                                
%ROBOCONF% %VERBOSE%             --report=summary --local-lib-cache=lib-cache\vtl-MSensor.rbc_libs                                  net-cache\vtl-MSensor.rbc_net     summary\vtl-MSensor
%ROBOCONF% %VERBOSE%             --report=summary --local-lib-cache=lib-cache\vtl-main.rbc_libs     --rules=rules\vtl_main.rul      net-cache\vtl-main.rbc_net        summary\vtl-main
%ROBOCONF% %VERBOSE%             --report=summary --local-lib-cache=lib-cache\vtl-periph.rbc_libs   --rules=rules\vtl_periph.rul    net-cache\vtl-periph.rbc_net      summary\vtl-periph

rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\vlk-flying-Distance_Sensor.rbc_libs       net-cache\vlk-flying-Distance_Sensor.rbc_net       summary\vlk-flying-Distance_Sensor
%ROBOCONF% %VERBOSE%             --report=summary --local-lib-cache=lib-cache\vlk-fl-main.rbc_libs  --rules=rules\vlk-fl-main.rul   net-cache\vlk-fl-main.rbc_net     summary\vlk-fl-main
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\vlk-jumper-EMCO_power.rbc_libs            net-cache\vlk-jumper-EMCO_power.rbc_net            summary\vlk-jumper-EMCO_power
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\vlk-jumper-main_board.rbc_libs            net-cache\vlk-jumper-main_board.rbc_net            summary\vlk-jumper-main_board
rem %ROBOCONF% %VERBOSE%             --report=summary --local-lib-cache=lib-cache\vlk-jp-psensor.rbc_libs                               net-cache\vlk-jp-psensor.rbc_net  summary\vlk-jp-psensor

rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Led_Driver.rbc_libs                         net-cache\wiz2_PLATFORM_Led_Driver.rbc_net                        summary\log_wiz2_PLATFORM_Led_Driver                           
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Link_Module.rbc_libs                        net-cache\wiz2_PLATFORM_Link_Module.rbc_net                       summary\log_wiz2_PLATFORM_Link_Module                          
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Mother_Board.rbc_libs                       net-cache\wiz2_PLATFORM_Mother_Board.rbc_net                      summary\log_wiz2_PLATFORM_Mother_Board                         
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller.rbc_libs                   net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net                  summary\log_wiz2_PLATFORM_Motor_Controller                     
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board.rbc_libs      net-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board.rbc_net     summary\log_wiz2_PLATFORM_Motor_Controller_Mother_Board        
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board_top.rbc_libs  net-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board_top.rbc_net summary\log_wiz2_PLATFORM_Motor_Controller_Mother_Board_top    
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller_Periph_Board.rbc_libs      net-cache\wiz2_PLATFORM_Motor_Controller_Periph_Board.rbc_net     summary\log_wiz2_PLATFORM_Motor_Controller_Periph_Board        
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_OSD_PIP.rbc_libs                            net-cache\wiz2_PLATFORM_OSD_PIP.rbc_net                           summary\log_wiz2_PLATFORM_OSD_PIP                              
rem %ROBOCONF% --report=summary --local-lib-cache=lib-cache\wiz2_PLATFORM_State_Sensor.rbc_libs                       net-cache\wiz2_PLATFORM_State_Sensor.rbc_net                      summary\log_wiz2_PLATFORM_State_Sensor                         

%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\USB-CAN.rbc_libs      --rules=rules\USB-CAN.rul      net-cache\USB-CAN.rbc_net       summary\USB-CAN   

%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\wiz2_av_ctrl.rbc_libs --rules=rules\wiz2_av_ctrl.rul net-cache\wiz2_av_ctrl.rbc_net  summary\wiz2_av_ctrl   

%ROBOCONF% %VERBOSE% --make-dump --report=summary --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs --rules=rules\jud_TriggersControl_v2_3_Driver.rul net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  summary\jud_TriggersControl_v2_3_Driver

rem --datasheet-path

:mcu
rem goto periph


%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vtx2_master.rbc_libs  --rules=rules\vtx2_master.rul   net-cache\vtx2_master.rbc_net  mcu\vtx2_master     
%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vtx2_slave.rbc_libs   --rules=rules\vtx2_slave.rul    net-cache\vtx2_slave.rbc_net   mcu\vtx2_slave      
%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vtx2_hotend.rbc_libs  --rules=rules\vtx2_hotend.rul   net-cache\vtx2_hotend.rbc_net  mcu\vtx2_hotend     
%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vtx2_tvko.rbc_libs    --rules=rules\vtx2_tvko.rul     net-cache\vtx2_tvko.rbc_net    mcu\vtx2_tvko       

%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vtl-main.rbc_libs     --rules=rules\vtl_main.rul      net-cache\vtl-main.rbc_net     mcu\vtl-main        
%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vtl-periph.rbc_libs   --rules=rules\vtl_periph.rul    net-cache\vtl-periph.rbc_net   mcu\vtl-periph      

%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vlk-fl-main.rbc_libs  --rules=rules\vlk-fl-main.rul   net-cache\vlk-fl-main.rbc_net  mcu\vlk-fl-main     
rem %ROBOCONF% --report=mcu --local-lib-cache=lib-cache\vlk-jumper-main_board.rbc_libs            net-cache\vlk-jumper-main_board.rbc_net            mcu\vlk-jumper-main_board

rem No MCU
rem %ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\vlk-jp-psensor.rbc_libs       net-cache\vlk-jp-psensor.rbc_net       mcu\vlk-jp-psensor                         

rem %ROBOCONF% --report=mcu --local-lib-cache=lib-cache\wiz2_PLATFORM_Led_Driver.rbc_libs                         net-cache\wiz2_PLATFORM_Led_Driver.rbc_net                        mcu\wiz2_PLATFORM_Led_Driver                           
rem %ROBOCONF% --report=mcu --local-lib-cache=lib-cache\wiz2_PLATFORM_Mother_Board.rbc_libs                       net-cache\wiz2_PLATFORM_Mother_Board.rbc_net                      mcu\wiz2_PLATFORM_Mother_Board                         
rem %ROBOCONF% --report=mcu --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller.rbc_libs                   net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net                  mcu\wiz2_PLATFORM_Motor_Controller                     
rem %ROBOCONF% --report=mcu --local-lib-cache=lib-cache\wiz2_PLATFORM_Motor_Controller_Periph_Board.rbc_libs      net-cache\wiz2_PLATFORM_Motor_Controller_Periph_Board.rbc_net     mcu\wiz2_PLATFORM_Motor_Controller_Periph_Board        
rem %ROBOCONF% --report=mcu --local-lib-cache=lib-cache\wiz2_PLATFORM_OSD_PIP.rbc_libs                            net-cache\wiz2_PLATFORM_OSD_PIP.rbc_net                           mcu\wiz2_PLATFORM_OSD_PIP                              

%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\USB-CAN.rbc_libs       net-cache\USB-CAN.rbc_net       mcu\USB-CAN

%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\wiz2_av_ctrl.rbc_libs  net-cache\wiz2_av_ctrl.rbc_net  mcu\wiz2_av_ctrl

%ROBOCONF% %VERBOSE% --make-dump --report=mcu --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs  net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  mcu\jud_TriggersControl_v2_3_Driver


:periph

rem goto h_conf

%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\vtx2_slave.rbc_libs   --rules=rules\vtx2_slave.rul    net-cache\vtx2_slave.rbc_net   periph\vtx2_slave      


%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\vtx2_tvko.rbc_libs    --rules=rules\vtx2_tvko.rul     net-cache\vtx2_tvko.rbc_net    periph\vtx2_tvko       



%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\vtx2_hotend.rbc_libs  --rules=rules\vtx2_hotend.rul   net-cache\vtx2_hotend.rbc_net  periph\vtx2_hotend     

%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\vtx2_master.rbc_libs  --rules=rules\vtx2_master.rul   net-cache\vtx2_master.rbc_net  periph\vtx2_master     

%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\vtl-main.rbc_libs     --rules=rules\vtl_main.rul      net-cache\vtl-main.rbc_net     periph\vtl-main        
%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\vtl-periph.rbc_libs   --rules=rules\vtl_periph.rul    net-cache\vtl-periph.rbc_net   periph\vtl-periph      

%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\vlk-fl-main.rbc_libs  --rules=rules\vlk-fl-main.rul   net-cache\vlk-fl-main.rbc_net  periph\vlk-fl-main     
rem %ROBOCONF% --report=periph --local-lib-cache=lib-cache\vlk-jumper-main_board.rbc_libs            net-cache\vlk-jumper-main_board.rbc_net            periph\vlk-jumper-main_board

%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\USB-CAN.rbc_libs      --rules=rules\USB-CAN.rul       net-cache\USB-CAN.rbc_net  periph\USB-CAN     

%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\wiz2_av_ctrl.rbc_libs --rules=rules\wiz2_av_ctrl.rul  net-cache\wiz2_av_ctrl.rbc_net  periph\wiz2_av_ctrl

%ROBOCONF% %VERBOSE% --make-dump --report=periph --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs --rules=rules\jud_TriggersControl_v2_3_Driver.rul  net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  periph\jud_TriggersControl_v2_3_Driver


rem goto end


:h_conf


%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\vtx2_slave.rbc_libs   --rules=rules\vtx2_slave.rul    net-cache\vtx2_slave.rbc_net   h_conf\vtx2_slave      

rem goto end

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\vtx2_tvko.rbc_libs    --rules=rules\vtx2_tvko.rul     net-cache\vtx2_tvko.rbc_net    h_conf\vtx2_tvko       

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\vtx2_hotend.rbc_libs  --rules=rules\vtx2_hotend.rul   net-cache\vtx2_hotend.rbc_net  h_conf\vtx2_hotend     

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\vtx2_master.rbc_libs  --rules=rules\vtx2_master.rul   net-cache\vtx2_master.rbc_net  h_conf\vtx2_master     

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\vtl-main.rbc_libs     --rules=rules\vtl_main.rul      net-cache\vtl-main.rbc_net     h_conf\vtl-main        
%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\vtl-periph.rbc_libs   --rules=rules\vtl_periph.rul    net-cache\vtl-periph.rbc_net   h_conf\vtl-periph      

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\vlk-fl-main.rbc_libs  --rules=rules\vlk-fl-main.rul   net-cache\vlk-fl-main.rbc_net  h_conf\vlk-fl-main     

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\USB-CAN.rbc_libs      --rules=rules\USB-CAN.rul       net-cache\USB-CAN.rbc_net  h_conf\USB-CAN     

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\wiz2_av_ctrl.rbc_libs --rules=rules\wiz2_av_ctrl.rul  net-cache\wiz2_av_ctrl.rbc_net  h_conf\wiz2_av_ctrl

%ROBOCONF% %VERBOSE% --make-dump --report=h_conf --local-lib-cache=lib-cache\jud_TriggersControl_v2_3_Driver.rbc_libs --rules=rules\jud_TriggersControl_v2_3_Driver.rul  net-cache\jud_TriggersControl_v2_3_Driver.rbc_net  h_conf\jud_TriggersControl_v2_3_Driver



:end

