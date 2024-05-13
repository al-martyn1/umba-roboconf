echo off
call setTestConfig.bat
call copyConf.bat




rem 56-Pin HTSSOP
rem %ROBOCONF% -S=DRV8301 "-O=description:Three-Phase Gate Driver With Dual Current Shunt Amplifiers and Buck Regulator" "-O=purpose:SENSOR:SPI|PWM" -O=datasheet:drv8301.pdf  ../data/units/DRV8301.csv ../lib/


rem %ROBOCONF% -S=MAX4444 "-O=description:Ultra-High-Speed, Low-Distortion, Differentialto-Single-Ended Line Receivers with Enable" -O=purpose:SWITCH -O=datasheet:MAX4444-MAX4445.pdf  ../data/units/MAX4444.csv ../lib/



rem %ROBOCONF% -S=ADG819 "-O=description:0.5 Ohm, CMOS, 1.8 V to 5.5 V, 2:1 Mux/SPDT Switch" -O=purpose:SWITCH -O=datasheet:adg819.pdf  ../data/units/ADG819.csv ../lib/

rem opamp

rem %ROBOCONF% -S=RESISTOR_X4 "-O=description:Resistor Assembly x4"  ../data/units/RESISTOR_X4.csv ../lib/
rem %ROBOCONF% -S=RESISTOR_X8 "-O=description:Resistor Assembly x4"  ../data/units/RESISTOR_X8.csv ../lib/


rem %ROBOCONF% -S=LMZ14203 "-O=description:SIMPLE SWITCHER 6-V to 42-V, 3-A Power Module"  -O=purpose:SWITCH -O=datasheet:lmz14203.pdf        ../data/units/LMZ14203.csv ../lib/
rem %ROBOCONF% -S=LMR16006 "-O=description:SIMPLE SWITCHER 60 V 0.6 A Buck Regulators"     -O=purpose:GATE -O=datasheet:lmr16006.pdf        ../data/units/LMR16006.csv ../lib/
rem %ROBOCONF% -S=ADG1604                     "-O=description:4:1 Analog Multiplexer" -O=purpose:SWITCH:MUX -O=datasheet:ADG1604.pdf  ../data/units/ADG1604.csv ../lib/ADG1604.cmp
rem %ROBOCONF% -S=ADG1604  -O=package:TSSOP14 "-O=description:4:1 Analog Multiplexer" -O=purpose:SWITCH:MUX -O=datasheet:ADG1604.pdf  ../data/units/ADG1604.csv ../lib/ADG1604-TSSOP14.cmp


rem %ROBOCONF% -S=TS5A3159  "-O=description:SPDT Analog Switch" -O=purpose:SWITCH -O=datasheet:ts5a3159.pdf  ../data/units/TS5A3159.csv ../lib/
rem %ROBOCONF% -S=ADG160_  "-O=description:4/8-Channel Multiplexer" -O=purpose:SWITCH:MUX -O=datasheet:ADG1608_1609.pdf  ../data/units/ADG160_.csv ../lib/ADG160_.cmp


rem %ROBOCONF% -S=MCP10_ "-O=description:Reset controller" -O=purpose:RESET_CONTROLLER -O=datasheet:11187f.pdf ../data/units/MCP10_.csv ../lib/MCP10_.cmp



rem %ROBOCONF% -S=SN65HVD23_ "-O=description:CAN Bus Transceiver" -O=purpose:DRIVER:CAN -O=datasheet:SN65HVD23x.pdf  ../data/units/SN65HVD23_.csv ../lib/
rem %ROBOCONF% -S=SN65HVD23_ -O=package:SOIC8 "-O=description:CAN Bus Transceiver" -O=purpose:DRIVER:CAN -O=datasheet:SN65HVD23x.pdf  ../data/units/SN65HVD23_.csv ../lib/SN65HVD23_-SOIC8.cmp



rem %ROBOCONF% -S=SN65HVD1780,SN65HVD1781,SN65HVD1782                    "-O=description:70-V Fault-Protected RS-485 Transceiver"  -O=purpose:DRIVER:RS485 -O=datasheet:sn65hvd1781.pdf ../data/units/SN65HVD1781.csv ../lib/SN65HVD178_.cmp
rem %ROBOCONF% -S=SN65HVD1780,SN65HVD1781,SN65HVD1782 "-O=package:SOIC8" "-O=description:70-V Fault-Protected RS-485 Transceiver"  -O=purpose:DRIVER:RS485 -O=datasheet:sn65hvd1781.pdf ../data/units/SN65HVD1781.csv ../lib/SN65HVD178_-SOIC8.cmp

rem %ROBOCONF% -S=ISL3179E,ISL3180E "-O=description:High ESD Protected, Full Fail-Safe RS-485/RS-422 Transceiver"  -O=purpose:DRIVER:RS485 -O=datasheet:isl3179e-80e.pdf ../data/units/ISL3179E-80E.csv ../lib/ISL31__E.cmp
rem %ROBOCONF% -S=MAX3250 "-O=description:RS-232 Transceiver"  -O=purpose:DRIVER:RS232 -O=datasheet:MAX3250.pdf ../data/units/MAX3250.csv ../lib/
rem %ROBOCONF% -S=MAX3280E,MAX3281E,MAX3283E,MAX3284E "-O=description:RS-485/RS-422 True Fail-Safe Receivers"  -O=purpose:DRIVER:RS485 -O=datasheet:MAX3280E-MAX3284E.pdf ../data/units/MAX328_E.csv ../lib/MAX328_E.cmp

rem %ROBOCONF% -S=MAX220,MAX222,MAX232,MAX233,MAX242,MAX243 "-O=description:Multichannel RS-232 Driver/Receiver"  -O=purpose:DRIVER:RS232 -O=datasheet:Maxim_Integrated-MAX232CSE+T-datasheet.pdf ../data/units/MAX2__.csv ../lib/MAX2_2.cmp
rem copy ..\lib\MAX2_2.cmp ..\lib\MAX2_3.cmp

rem %ROBOCONF% -S=MAX3483,MAX3485,MAX3486,MAX3488,MAX3490,MAX3491 "-O=description:High-Speed, RS-485/RS-422 Transceiver"  -O=purpose:DRIVER:RS485 -O=datasheet:MAX3483-MAX3491.pdf ../data/units/MAX348_.csv ../lib/MAX348_.cmp
rem copy ..\lib\MAX348_.cmp ..\lib\MAX349_.cmp

rem %ROBOCONF% -S=MAX3362 "-O=description:High-Speed, RS-485/RS-422 Transceiver"  -O=purpose:DRIVER:RS485 -O=datasheet:MAX3362.pdf ../data/units/MAX3362.csv ../lib/
rem %ROBOCONF% -S=MAX3093E,MAX3094E "-O=description:Quad RS-422/RS-485 Receiver"  -O=purpose:DRIVER:RS485 -O=datasheet:MAX3093E-MAX3094E.pdf ../data/units/MAX309_E.csv ../lib/MAX309_E.cmp
rem %ROBOCONF% -S=ADM3485E "-O=description:RS-485/RS-422 Transceiver"  -O=purpose:DRIVER:RS485 -O=datasheet:adm3485e.pdf ../data/units/ADM3485E.csv ../lib/
rem %ROBOCONF% -S=CP2104 "-O=description:SINGLE-CHIP USB-TO-UART BRIDGE"  -O=purpose:DRIVER:USB_UART -O=datasheet:cp2104.pdf ../data/units/CP2104.csv ../lib/





rem %ROBOCONF% -S=TXB0104 "-O=description:4-Bit Bidirectional Voltage-level Translator With Automatic Direction Sensing"  -O=purpose:DRIVER -O=datasheet:txb0104.pdf ../data/units/TXB0104.csv ../lib/

rem %ROBOCONF% -S=TW283_ "-O=description:4-Channel Video and Audio Controller"  -O=purpose:VIDEO -O=datasheet:Intersil-TW2837-PB1-GE-datasheet.pdf ../data/units/TW283_.csv ../lib/TW283_.cmp


rem %ROBOCONF% -S=MT48LC2M32B2 "-O=description:Synchronous DRAM 512K x 32 x 4 banks"  -O=purpose:RAM -O=datasheet:Micron_Technologies-MT48LC2M32B2TG-6_G-datasheet.pdf ../data/units/MT48LC2M32B2.csv ../lib/




rem %ROBOCONF% "-S=TEN 5-1211" "-O=description:DC/DC Converter" -O=purpose:POWER_CONVERTER -O=datasheet:ten5.pdf  ../data/units/TEN_5-1211.csv ../lib/TEN_5-1211.cmp
rem %ROBOCONF% -S=TPS55340 "-O=description:Flyback DC-DC Regulator" -O=purpose:POWER_CONVERTER -O=datasheet:tps55340.pdf  ../data/units/TPS55340.csv ../lib/
rem %ROBOCONF% -S=MAX889 "-O=description:High-Frequency, Regulated, Inverting Charge Pump" -O=purpose:POWER_CONVERTER -O=datasheet:MAX889.pdf  ../data/units/MAX889.csv ../lib/
rem %ROBOCONF% -S=TPS6302_ "-O=description:High Efficiency Single Inductor Buck-Boost Converter With 4-A Switches" -O=purpose:POWER_CONVERTER -O=datasheet:tps63020.pdf  ../data/units/TPS6302_.csv ../lib/TPS6302_.cmp





rem %ROBOCONF% -S=TPD2S017 "-O=description:2-Channel Ultra-Low Clamp Voltage ESD (electrostatic discharge) Solution" -O=purpose:PROTECTION -O=datasheet:tpd2s017.pdf        ../data/units/TPD2S017.csv ../lib/
rem %ROBOCONF% -S=TPS249_ "-O=description:TPS249x Positive High-Voltage Power-Limiting Hot Swap Controller" -O=purpose:DRIVER -O=datasheet:tps2490.pdf  ../data/units/TPS249_.csv ../lib/
rem %ROBOCONF% -S=ADM7150 "-O=description:Ultralow Noise, High PSRR, RF Linear Regulator" -O=purpose:DRIVER -O=datasheet:adm7150.pdf  ../data/units/ADM7150.csv ../lib/
rem %ROBOCONF% -S=ADM7154 "-O=description:Ultralow Noise, High PSRR, RF Linear Regulator" -O=purpose:DRIVER -O=datasheet:ADM7154.pdf  ../data/units/ADM7154.csv ../lib/

rem %ROBOCONF% -S=TLV760,TLV760_,TLV760__  "-O=description:Fixed-Output Linear-Voltage Regulator" -O=purpose:DC_REGULATOR -O=datasheet:tlv760.pdf  ../data/units/TLV760__.csv ../lib/TLV760__.cmp
rem copy /Y ..\lib\TLV760__.cmp ..\lib\TLV760_.cmp
rem copy /Y ..\lib\TLV760__.cmp ..\lib\TLV760.cmp


rem %ROBOCONF% -S=TLV431,NCV431,SCV431 "-O=description:Low Voltage Precision Adjustable Shunt Regulator" -O=purpose:DRIVER -O=datasheet:TLV431A-D.PDF  ../data/units/TLV431ASN1T1G.csv ../lib/__V431.cmp
rem %ROBOCONF% -S=LM1117 "-O=description:Low-Dropout Linear Regulator" -O=purpose:DRIVER -O=datasheet:LM1117.PDF  ../data/units/LM1117.csv ../lib/


rem %ROBOCONF% -S=MAX9918,MAX9919,MAX9920 "-O=description:Precision Uni-/Bidirectional Current Sensor with analog output" -O=purpose:SENSOR:ADC:CURRENT -O=datasheet:MAX9918-MAX9920.pdf  ../data/units/MAX9920.csv ../lib/MAX991_.cmp
rem copy /Y ..\lib\MAX991_.cmp ..\lib\MAX9920.cmp
rem %ROBOCONF% -S=LM3414 "-O=description:1-A, 60-W Common Anode-Capable Constant Current Buck LED Driver PWM 240Hz" -O=purpose:DRIVER:PWM -O=datasheet:lm3414hv.pdf  ../data/units/LM3414.csv ../lib/
rem %ROBOCONF% -S=MC33926 "-O=description:5.0 A throttle control H-bridge" -O=purpose:DRIVER:HBRIDGE:MOTOR -O=datasheet:MC33926.pdf  ../data/units/MC33926.csv ../lib/
rem %ROBOCONF% -S=MAX9546,MAX9547 "-O=description:Differential Video Interface Chipset" -O=purpose:DRIVER -O=datasheet:MAX9546-MAX9547.pdf  ../data/units/MAX954_.csv ../lib/MAX954_.cmp
rem %ROBOCONF% -S=ADA4430                  "-O=description:Ultralow Power Video Filter with Power-Down" -O=purpose:SWITCH -O=datasheet:ADA4430-1.pdf  ../data/units/ADA4430.csv ../lib/
rem %ROBOCONF% -S=ADA4430 -O=package:SOT23 "-O=description:Ultralow Power Video Filter with Power-Down" -O=purpose:SWITCH -O=datasheet:ADA4430-1.pdf  ../data/units/ADA4430.csv ../lib/ADA4430-SOT23.cmp
rem %ROBOCONF% -S=CAT4237                   "-O=description:High Voltage CMOS Boost White LED Driver" -O=purpose:GATE -O=datasheet:CAT4237-D.PDF  ../data/units/CAT4237.csv ../lib/
rem %ROBOCONF% -S=CAT4237 -O=package:TSOT23 "-O=description:High Voltage CMOS Boost White LED Driver" -O=purpose:GATE -O=datasheet:CAT4237-D.PDF  ../data/units/CAT4237.csv ../lib/CAT4237-TSOT23.cmp


rem %ROBOCONF% -S=DRV104 "-O=description:PWM High-Side Driver for Solenoids, Coils, Valves, Heaters, and Lamps" -O=purpose:DRIVER:PWM -O=datasheet:drv104.pdf  ../data/units/DRV104.csv ../lib/

rem %ROBOCONF% -S=DRV8301 "-O=description:Three-Phase Gate Driver With Dual Current Shunt Amplifiers and Buck Regulator" -O=purpose:DRIVER:PWM:BLDC_MOTOR -O=datasheet:drv8301.pdf  ../data/units/DRV8301.csv ../lib/

rem %ROBOCONF% -S=DRV8832 "-O=description:Low-Voltage Motor Driver IC" -O=purpose:DRIVER:HBRIDGE:MOTOR -O=datasheet:drv8832.pdf  ../data/units/DRV8832.csv ../lib/


rem %ROBOCONF% -S=AD8541,AD8542,AD8544 "-O=description:CMOS Rail-to-Rail General-Purpose Amplifiers" -O=purpose:OPAMP -O=datasheet:AD8541_8542_8544.pdf  ../data/units/AD854_.csv ../lib/AD854_.cmp
rem %ROBOCONF% -S=AD8061,AD8062,AD8063 "-O=description:Rail-to-Rail Amplifier" -O=purpose:OPAMP -O=datasheet:ad8061_8062_8063.pdf  ../data/units/AD806_.csv ../lib/AD806_.cmp



rem %ROBOCONF% -S=MAX9928,MAX9929 "-O=description:Current-Sense Amplifiers, Micropower, Uni-/Bidirectional" -O=purpose:OPAMP -O=datasheet:MAX9928-MAX9929.pdf  ../data/units/MAX992_.csv ../lib/MAX992_.cmp

rem Sensors
rem %ROBOCONF% -S=LM60     "-O=description:Temperature Sensor (Analog)" -O=purpose:SENSOR:ADC:TEMPERATURE -O=datasheet:lm60.pdf  ../data/units/LM60.csv ../lib/
rem %ROBOCONF% -S=MAX31855 "-O=description:Cold-Junction Compensated Thermocouple-to-Digital Converter" -O=purpose:SENSOR:SPI:TEMPERATURE -O=datasheet:MAX31855.pdf  ../data/units/MAX31855.csv ../lib/
rem %ROBOCONF% -S=LSM9DS1  "-O=description:3D accelerometer, 3D gyroscope, 3D magnetometer (iNEMO inertial module)" "-O=purpose:SENSOR:I2C|SPI:9D_INERTIAL" -O=datasheet:lsm9ds1.pdf  ../data/units/LSM9DS1.csv ../lib/
rem %ROBOCONF% -S=AS5145  "-O=description:12-Bit Programmable Magnetic Rotary Encoder" "-O=purpose:SENSOR:SSI|PWM|QUADRATURE:ROTARY" -O=datasheet:AS5145_DS000398_1-00.pdf  ../data/units/AS5145.csv ../lib/


rem %ROBOCONF% -S=VL53L0X "-O=description:Ranging and gesture detection sensor" -O=purpose:SENSOR -O=datasheet:vl53l0x.pdf  ../data/units/VL53L0X.csv ../lib/
rem %ROBOCONF% -S=40PC150G2A "-O=description:Honeywell Pressure Port Style O-ring interface (Analog)" -O=purpose:SENSOR:ADC:PRESSURE -O=datasheet:Honeywell-40PC150G2A-datasheet.pdf  ../data/units/40PC______.csv ../lib/40PC______.cmp
rem %ROBOCONF% -S=INA21_ "-O=description:Voltage Output, Low- or High-Side Measurement, Bidirectional, Zero-Drift Series, Current-Shunt Monitor" -O=purpose:SENSOR -O=datasheet:ina215.pdf  ../data/units/INA21_.csv ../lib/INA21_.cmp
rem %ROBOCONF% -S=INA19_ "-O=description:Current-Shunt Monitor" -O=purpose:SENSOR -O=datasheet:ina198.pdf  ../data/units/INA19_.csv ../lib/INA19_.cmp
rem %ROBOCONF% -S=INA137,INA2137 "-O=description:AUDIO DIFFERENTIAL LINE RECEIVER" -O=purpose:SENSOR -O=datasheet:ina137.pdf  ../data/units/INA137.csv ../lib/
rem copy /Y ..\lib\INA137.cmp ..\lib\INA2137.cmp

rem %ROBOCONF% -S=HDC1080 "-O=description:Digital Humidity Sensor with Temperature Sensor (I2C)" -O=purpose:SENSOR:I2C:TEMPERATURE -O=datasheet:hdc1080.pdf  ../data/units/HDC1080.csv ../lib/



rem Transistors
rem %ROBOCONF% -S=NTMFS5H409NL,NTMFS5H409NLT1G "-O=description:Power MOSFET"                    -O=purpose:SWITCH -O=datasheet:NTMFS5H409NL-D.PDF  ../data/units/NTMFS5H409NL.csv ../lib/NTMFS5H409.csv

rem %ROBOCONF% -S=IRF7509      "-O=description:Power MOSFET (HEXFET)"                           -O=purpose:SWITCH -O=datasheet:irf7509pbf.pdf      ../data/units/IRF7509.csv      ../lib/
rem %ROBOCONF% -S=IRLML2244    "-O=description:Power MOSFET (HEXFET) Load/System Switch"        -O=purpose:SWITCH -O=datasheet:irlml2244pbf.pdf    ../data/units/IRLML2244.csv    ../lib/
rem %ROBOCONF% -S=IRLML0030    "-O=description:Power MOSFET (HEXFET) Load/System Switch"        -O=purpose:SWITCH -O=datasheet:irlml0030pbf.pdf    ../data/units/IRLML0030.csv    ../lib/
rem %ROBOCONF% -S=IRLML0060    "-O=description:Power MOSFET (HEXFET) Load/System Switch"        -O=purpose:SWITCH -O=datasheet:Infineon-IRLML0060-DS-v01_01-EN.pdf ../data/units/IRLML0060.csv    ../lib/
rem %ROBOCONF% -S=2N7002       "-O=description:60 V, 300 mA N-channel Trench MOSFET"            -O=purpose:SWITCH -O=datasheet:2N7002.pdf ../data/units/2N7002.csv    ../lib/
rem %ROBOCONF% -S=FDN5618P     "-O=description:60V P-Channel Logic Level PowerTrench MOSFET"    -O=purpose:SWITCH -O=datasheet:FDN5618P-D.PDF ../data/units/FDN5618P.csv    ../lib/
rem %ROBOCONF% -S=CMXDM7002A     "-O=description:Dual version of the 2N7002 enh. mode N-MOSFET" -O=purpose:SWITCH -O=datasheet:cmxdm7002a.pdf ../data/units/CMXDM7002A.csv    ../lib/
rem %ROBOCONF% -S=CSD19532Q5B    "-O=description:100 V N-Channel NexFET Power MOSFET"           -O=purpose:SWITCH -O=datasheet:csd19532q5b.pdf ../data/units/CSD19532Q5B.csv    ../lib/








rem STM32xxx
rem %ROBOCONF% -S=STM32F405RG,STM32F405VG "-O=description:ARM Cortex-M4 32b MCU+FPU, 210DMIPS"         -O=purpose:MCU -O=datasheet:STM32F405xx-407xx_datasheet.pdf -O=refman:STM32F405_415_407_417_427_437__429_439_reference_manual.pdf ../data/units/STM32F405RG.csv ../lib/STM32F405__.cmp

rem %ROBOCONF% -S=STM32F405RG,STM32F405VG "-O=package:LQFP100" "-O=description:ARM Cortex-M4 32b MCU+FPU, 210DMIPS"         -O=purpose:MCU -O=datasheet:STM32F405xx-407xx_datasheet.pdf -O=refman:STM32F405_415_407_417_427_437__429_439_reference_manual.pdf ../data/units/STM32F405__-LQFP100.csv ../lib/STM32F405__-LQFP100.cmp

rem %ROBOCONF% -S=STM32F405RG,STM32F405VG "-O=package:LQFP64" "-O=description:ARM Cortex-M4 32b MCU+FPU, 210DMIPS"         -O=purpose:MCU -O=datasheet:STM32F405xx-407xx_datasheet.pdf -O=refman:STM32F405_415_407_417_427_437__429_439_reference_manual.pdf ../data/units/STM32F405__-LQFP64.csv ../lib/STM32F405__-LQFP64.cmp


rem %ROBOCONF% -S=STM32F413_H,STM32F413_G,STM32F423_H,STM32F423_G "-O=description:ARM Cortex-M4 core with DSP and FPU, 1,5 MByte Flash, 100 MHz CPU" -O=purpose:MCU -O=datasheet:stm32f413vh.pdf -O=refman:en.DM00305666.pdf.pdf ../data/units/STM32F413_H.csv ../lib/STM32F413_H.cmp
rem copy /Y ..\lib\STM32F413_H.cmp ..\lib\STM32F413_G.cmp
rem copy /Y ..\lib\STM32F413_H.cmp ..\lib\STM32F423_G.cmp
rem copy /Y ..\lib\STM32F413_H.cmp ..\lib\STM32F423_H.cmp

rem %ROBOCONF% -S=STM32F405RGT6 "-O=description:ARM Cortex-M4 32b MCU+FPU, 210DMIPS"         -O=purpose:MCU -O=datasheet:STM32F405xx-407xx_datasheet.pdf -O=refman:STM32F405_415_407_417_427_437__429_439_reference_manual.pdf ../data/units/STM32F405RGT6.csv ../lib/

rem %ROBOCONF% -S=STM32F303_B__,STM32F303_C__ "-O=description:ARM-based Cortex-M4 32b MCU+FPU 48KB SRAM, 4 ADCs, 2 DAC" -O=purpose:MCU -O=datasheet:stm32f303cb.pdf -O=refman:en.DM00043574.pdf ../data/units/STM32F303xB_STM32F303xC.csv ../lib/STM32F303_B__.cmp
rem copy /Y ..\lib\STM32F303_B__.cmp ..\lib\STM32F303_C__.cmp







rem %ROBOCONF% -S=STM32F103_8,STM32F103_B "-O=pin-filter-chars:[]()/*-+" "-O=description:ARM Cortex-M3 MCU with 64/128 Kbytes Flash, 72 MHz CPU, motor control, USB, CAN, 7 timers, 2 ADCs, 9 com" -O=purpose:MCU -O=datasheet:STM32F101xx-102xx-103xx-105xx-107xx.pdf -O=refman:en.CD00171190.pdf ../data/units/STM32F103_8.csv ../lib/STM32F103_8.cmp
rem copy /Y ..\lib\STM32F103_8.cmp ..\lib\STM32F103_B.cmp

rem %ROBOCONF% -S=STM32F103_8,STM32F103_B "-O=pin-filter-chars:[]()/*-+" "-O=package:VFQFPN36" "-O=description:ARM Cortex-M3 MCU with 64/128 Kbytes Flash, 72 MHz CPU, motor control, USB, CAN, 7 timers, 2 ADCs, 9 com" -O=purpose:MCU -O=datasheet:STM32F101xx-102xx-103xx-105xx-107xx.pdf -O=refman:en.CD00171190.pdf ../data/units/STM32F103T8-VFQFPN36.csv ../lib/STM32F103_8-VFQFPN36.cmp
rem copy /Y ..\lib\STM32F103_8-VFQFPN36.cmp ..\lib\STM32F103_B-VFQFPN36.cmp


rem %ROBOCONF% -S=STM32F103_C,STM32F103_D,STM32F103_E "-O=pin-filter-chars:[]()/*-+" "-O=description:High-density performance line Arm-based 32-bit MCU with 256 to 512KB Flash, USB, CAN, 11 timers, 3 ADCs" -O=purpose:MCU -O=datasheet:stm32f103ve.pdf -O=refman:en.CD00171190.pdf ../data/units/STM32F103_C.csv ../lib/STM32F103_C.cmp
rem copy /Y ..\lib\STM32F103_C.cmp ..\lib\STM32F103_D.cmp
rem copy /Y ..\lib\STM32F103_C.cmp ..\lib\STM32F103_E.cmp

rem %ROBOCONF% -S=STM32F303VD "-O=pin-filter-chars:[]()/*-+" "-O=package:LQFP100" "-O=description:ARM-based Cortex-M4 32b MCU+FPU 48KB SRAM, 4 ADCs, 2 DAC" -O=purpose:MCU -O=datasheet:stm32f303cb.pdf -O=refman:en.DM00043574.pdf ../data/units/STM32F303VD-LQFP100.csv ../lib/STM32F303VD-LQFP100.cmp


rem %ROBOCONF% -S=STM32F303_6,STM32F303_8,STM32F303_B,STM32F303_C,STM32F303_D,STM32F303_E,STM32F328_8,STM32F358_C,STM32F398_E "-O=pin-filter-chars:[]()/*-+" "-O=description:ARM-based Cortex-M4 32b MCU+FPU 48KB SRAM, 4 ADCs, 2 DAC" -O=purpose:MCU -O=datasheet:stm32f303cb.pdf -O=refman:en.DM00043574.pdf ../data/units/STM32F303xB_STM32F303xC.csv ../lib/STM32F303_6.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F303_8.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F303_B.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F303_C.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F303_D.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F303_E.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F328_8.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F358_C.cmp
rem copy /Y ..\lib\STM32F303_6.cmp ..\lib\STM32F398_E.cmp

rem %ROBOCONF% -S=STM32F303_6,STM32F303_8,STM32F303_B,STM32F303_C,STM32F303_D,STM32F303_E,STM32F328_8,STM32F358_C,STM32F398_E "-O=pin-filter-chars:[]()/*-+" "-O=package:LQFP48" "-O=description:ARM-based Cortex-M4 32b MCU+FPU 48KB SRAM, 4 ADCs, 2 DAC" -O=purpose:MCU -O=datasheet:stm32f303cb.pdf -O=refman:en.DM00043574.pdf ../data/units/STM32F303xB_STM32F303xC-LQFP48.csv ../lib/STM32F303_6-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F303_8-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F303_B-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F303_C-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F303_D-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F303_E-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F328_8-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F358_C-LQFP48.cmp
rem copy /Y ..\lib\STM32F303_6-LQFP48.cmp ..\lib\STM32F398_E-LQFP48.cmp



rem %ROBOCONF% -S=CP2105  "-O=description:SINGLE-CHIP USB TO DUAL UART BRIDGE"  -O=purpose:DRIVER:USB_UART -O=datasheet:cp2105.pdf ../data/units/CP2105.csv ../lib/
rem %ROBOCONF% -S=PSD1-A  "-O=description:Single output DC/DC converter"        -O=purpose:PWR:DC -O=datasheet:peak_psd-1a.pdf     ../data/units/PSD1-A-0505E.csv ../lib/PSD1-A.cmp
rem %ROBOCONF% -S=ISO1050 "-O=description:ISO1050 Isolated CAN Transceiver"     -O=purpose:PWR:DC -O=datasheet:iso1050.pdf         ../data/units/ISO1050.csv ../lib/ISO1050.cmp
