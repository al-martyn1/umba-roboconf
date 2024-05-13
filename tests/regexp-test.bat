call setTestConfig.bat

%ROBOCONF% "--regexp-test=P(A|B|C|D|E|F|G|H)(\d{1,2})" "--regexp-template=GPIO$({1}),GPIO_PIN_$({2})"   PC14

%ROBOCONF% "--regexp-test=A(\d+)"  A_GND2
%ROBOCONF% "--regexp-test=A(\d+)"  A2
%ROBOCONF% "--regexp-test=.*(USART).*" "--regexp-template=$({PREFIX})UART$({SUFFIX})" UART4_TX
%ROBOCONF% "--regexp-test=.*(USART).*" "--regexp-template=$({PREFIX})" USART2_CTS
%ROBOCONF% "--regexp-test=.*(USART).*" "--regexp-template=$({SUFFIX})" USART2_CTS
%ROBOCONF% "--regexp-test=.*(USART).*" "--regexp-template=$({MATCH})"  USART2_CTS
%ROBOCONF% "--regexp-test=.*(USART).*" "--regexp-template=$({1})" USART2_CTS

%ROBOCONF% "--regexp-test=(.*)USART(.*)" "--regexp-template=$({PREFIX})$({1})UART$({2})$({SUFFIX})" USART2_CTS

%ROBOCONF% "--regexp-test=(.*_)(n)(.*)" "--regexp-template=$({P})___$({M})___$({S})___$({1})" MCU_Motor_2_nD2
%ROBOCONF% "--regexp-test=MCU_(.*)" "--regexp-template=$({1})" MCU_Motor_2_nD2
%ROBOCONF% "--regexp-test=(.*_)(n)([A-Z].*)" "--regexp-template=$({1})~$({3})" MCU_Motor_2_nD2
%ROBOCONF% "--regexp-test=(.+)_(\d+)(.*)" "--regexp-template=$({1})$({2})$({3})" MCU_Motor_2_nD2
%ROBOCONF% "--regexp-test=(.+)_(\d+)(.*)" "--regexp-template=$({1})$({2})$({3})" SWITCH_1
%ROBOCONF% "--regexp-test=(.*)MOT(\d*)(.*)" "--regexp-template=$({1})MOTOR$({2})$({3})" MCU_MOT1_SPEED

%ROBOCONF% "--regexp-test=(.*)_PWM_(.*)" "--regexp-template=$({1})_$({2})_PWM" MCU_PWM_ESC
%ROBOCONF% "--regexp-test=(.*)_PWM_(.*)" "--regexp-template=$({1})_$({2})_PWM" PWM_ESC
%ROBOCONF% "--regexp-test=PWM_(.*)"      "--regexp-template=$({1})_PWM"        MCU_PWM_ESC
%ROBOCONF% "--regexp-test=PWM_(.*)"      "--regexp-template=$({1})_PWM"        PWM_ESC

%ROBOCONF% "--regexp-test=([[:alpha:]]+\d+)_([[:alpha:]]+)(\d+)" "--regexp-template=$({P})$({1})UNIT$({3})_$({2})$({S})"        AS5040_PWM3
%ROBOCONF% "--regexp-test=(.*)RE/DE(.*)" "--regexp-template=$({P})$({1})DE$({2})$({S})"        RS485UNIT1_USART_RE/DE
%ROBOCONF% "--regexp-test=(.*)DE(.*)" "--regexp-template=$({P})$({1})DE$({2})$({S})"        RS485UNIT1_USART_RE/DE
%ROBOCONF% "--regexp-test=(.*)RE(.*)" "--regexp-template=$({P})$({1})DE$({2})$({S})"        RS485UNIT1_USART_RE/DE

%ROBOCONF% "--regexp-test=SWITCH(\d+)" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})$({1})"        SWITCH_1
%ROBOCONF% "--regexp-test=SWITCH(\d+)" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})$({1})"        SWITCH1

%ROBOCONF% "--regexp-test=.*(\d+)" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})___#1_$({1})"       DA
%ROBOCONF% "--regexp-test=.*(\d+)" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})___#1_$({1})"       DA3
%ROBOCONF% "--regexp-test=.*(\d+)" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})___#1_$({1})"       DA3B

%ROBOCONF% "--regexp-test=.*(\d+).*" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})___#1_$({1})"     DA
%ROBOCONF% "--regexp-test=.*(\d+).*" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})___#1_$({1})"     DA3
%ROBOCONF% "--regexp-test=.*(\d+).*" "--regexp-template=prefix_$({P})___match_$({M})___suffix_$({S})___LIMIT_$({P})___#1_$({1})"     DA3B





