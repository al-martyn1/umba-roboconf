call setTestConfig.bat
call copyConf.bat

set VERBOSE=%VERBOSE%

if exist net-cache goto mk_cache
mkdir net-cache
:mk_cache

if not exist net-cache\samples.rbc_net         %ROBOCONF% %VERBOSE% --make-cache ../data/nets/samples.net   net-cache\samples.rbc_net
if not exist net-cache\samples_copy.rbc_net    %ROBOCONF% %VERBOSE% --make-cache net-cache\samples.rbc_net  net-cache\samples_copy.rbc_net

if not exist net-cache\vtx2_master.rbc_net     %ROBOCONF% %VERBOSE% --make-cache ../data/nets/main_board_master_MCU.NET  net-cache\vtx2_master.rbc_net
if not exist net-cache\vtx2_slave.rbc_net      %ROBOCONF% %VERBOSE% --make-cache ../data/nets/main_board_slave.NET       net-cache\vtx2_slave.rbc_net
if not exist net-cache\vtx2_hotend.rbc_net     %ROBOCONF% %VERBOSE% --make-cache ../data/nets/Hot-End_MCU.NET            net-cache\vtx2_hotend.rbc_net
if not exist net-cache\vtx2_tvko.rbc_net       %ROBOCONF% %VERBOSE% --make-cache ../data/nets/TVKO_MCU.NET               net-cache\vtx2_tvko.rbc_net

if not exist net-cache\vtl-MSensor.rbc_net     %ROBOCONF% %VERBOSE% --make-cache ../data/nets-vertel/vtl-MSensor.NET     net-cache\vtl-MSensor.rbc_net
if not exist net-cache\vtl-main.rbc_net        %ROBOCONF% %VERBOSE% --make-cache ../data/nets-vertel/vtl-main.NET        net-cache\vtl-main.rbc_net
if not exist net-cache\vtl-periph.rbc_net      %ROBOCONF% %VERBOSE% --make-cache ../data/nets-vertel/vtl-periph.NET      net-cache\vtl-periph.rbc_net

if not exist net-cache\USB-CAN.rbc_net         %ROBOCONF% %VERBOSE% --make-cache ../data/nets/USB-CAN.NET                net-cache\USB-CAN.rbc_net

if not exist net-cache\wiz2_av_ctrl.rbc_net    %ROBOCONF% %VERBOSE% --make-cache ../data/nets-wiz2/wiz2_av_ctrl.NET      net-cache\wiz2_av_ctrl.rbc_net


if not exist net-cache\wiz2_PLATFORM_Led_Driver.rbc_net                        %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Led_Driver.NET                            net-cache\wiz2_PLATFORM_Led_Driver.rbc_net
if not exist net-cache\wiz2_PLATFORM_Link_Module.rbc_net                       %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Link_Module.NET                           net-cache\wiz2_PLATFORM_Link_Module.rbc_net
if not exist net-cache\wiz2_PLATFORM_Mother_Board.rbc_net                      %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Mother_Board.NET                          net-cache\wiz2_PLATFORM_Mother_Board.rbc_net
if not exist net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net                  %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Motor_Controller.NET                      net-cache\wiz2_PLATFORM_Motor_Controller.rbc_net
if not exist net-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board.rbc_net     %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Motor_Controller_Mother_Board.NET         net-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board.rbc_net
if not exist net-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board_top.rbc_net %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Motor_Controller_Mother_Board_top.NET     net-cache\wiz2_PLATFORM_Motor_Controller_Mother_Board_top.rbc_net
if not exist net-cache\wiz2_PLATFORM_Motor_Controller_Periph_Board.rbc_net     %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_Motor_Controller_Periph_Board.NET         net-cache\wiz2_PLATFORM_Motor_Controller_Periph_Board.rbc_net
if not exist net-cache\wiz2_PLATFORM_OSD_PIP.rbc_net                           %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_OSD_PIP.NET                               net-cache\wiz2_PLATFORM_OSD_PIP.rbc_net
if not exist net-cache\wiz2_PLATFORM_State_Sensor.rbc_net                      %ROBOCONF% --make-cache ../data/nets-wiz2/wiz2_PLATFORM_State_Sensor.NET                          net-cache\wiz2_PLATFORM_State_Sensor.rbc_net

if not exist net-cache\vlk-flying-Distance_Sensor.rbc_net                      %ROBOCONF% --make-cache ../data/nets-vlk/vlk-flying-Distance_Sensor.NET                           net-cache\vlk-flying-Distance_Sensor.rbc_net
if not exist net-cache\vlk-fl-main.rbc_net                                     %ROBOCONF% %VERBOSE% --make-cache ../data/nets-vlk/vlk-fl-main.NET                                net-cache\vlk-fl-main.rbc_net
if not exist net-cache\vlk-jumper-EMCO_power.rbc_net                           %ROBOCONF% --make-cache ../data/nets-vlk/vlk-jumper-EMCO_power.NET                                net-cache\vlk-jumper-EMCO_power.rbc_net
if not exist net-cache\vlk-jumper-main_board.rbc_net                           %ROBOCONF% --make-cache ../data/nets-vlk/vlk-jumper-main_board.NET                                net-cache\vlk-jumper-main_board.rbc_net
if not exist net-cache\vlk-jp-psensor.rbc_net                                  %ROBOCONF% --make-cache ../data/nets-vlk/vlk-jp-psensor.NET                                       net-cache\vlk-jp-psensor.rbc_net

if not exist net-cache\jud_TriggersControl_v2_3_Driver.rbc_net                 %ROBOCONF% --make-cache ../data/nets-jud/jud_TriggersControl_v2_3_Driver.NET                      net-cache\jud_TriggersControl_v2_3_Driver.rbc_net

