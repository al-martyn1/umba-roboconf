call setTestConfig.bat
call copyConf.bat


%ROBOCONF% -S=STM32F103RC "-O=description:ARM Cortex MCU" -O=purpose:MCU ../data/units/STM32F103RC.csv ../lib/STM32F103RC.cmp
