@rem CSV separator set option
@rem "Pin Designator,Net Name,Display Name" -> "Designator,Net,Name"
@rem "# Altium Pin Report File" -> "Report Generated From Altium Designer;;;;;;;;;;;;;;;;;;;;;;;;"
@rem --csv-import-option is -O

@rem Altium 20 - "-O=designator-field-title:Pin Designator" "-O=name-field-title:Display Name"
@rem Altium 17 - "-O=designator-field-title:Designator"     "-O=name-field-title:Name"
@set CSV_FORMAT_OPTS=-O=csv-separator:comma "-O=designator-field-title:Pin Designator" "-O=name-field-title:Display Name"


@rem Надо сделать возможность задания нескольких мест поиска даташитов

@rem [X] Done
@rem ../lib/
@rem any of: MT29F2G08ABAEAWP-IT-chipdip-DOC011454851.pdf
@rem alter name FLASH_NAND_2GB_MT29F2G08ABAEAWP-IT 
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=MT29F2G08ABAEAWP-IT,FLASH_NAND_2GB_MT29F2G08ABAEAWP-IT  "-O=description:NAND Flash Memory" -O=purpose:FLASH "-O=datasheet:MT29F2G08ABAEAWP-IT-chipdip-DOC011454851.pdf" FLASH_NAND_2GB_MT29F2G08ABAEAWP-IT.csv MT29F2G08ABAEAWP-IT.cmp


@rem [X] Done
@rem 74HC_HCT4067.pdf
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=74HC4067DB   "-O=description:16-channel analog multiplexer/demultiplexer" -O=purpose:SWITCH:MUX "-O=datasheet:74HC_HCT4067.pdf" 74HC4067DB.csv 74HC4067DB.cmp


@rem [X] Done
@rem L:\Dev\Техническая документация\Усилители\Малошумящие\AD8655_8656.pdf
@copy /Y /B "L:\Dev\Техническая документация\Усилители\Малошумящие\AD8655_8656.pdf" ..\datasheets\AD8655_8656.pdf
@rem alter name AMP_OP_AD8656ARM 
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=AD8656ARM,AMP_OP_AD8656ARM "-O=description:Low Noise Precision CMOS Amplifier"    -O=purpose:SENSOR:ADC "-O=datasheet:AD8655_8656.pdf" AD8656ARM.csv AD8656ARM.cmp


@rem SENSOR:SSI|PWM|STEP_DIR|QUADRATURE|BLDC:ROTARY
@rem SENSOR:SSI|PWM|QUADRATURE:ROTARY
@rem SENSOR:SPI|PWM
@rem SENSOR:I2C:PRESSURE
@rem SENSOR:I2C|SPI:9D_INERTIAL
@rem SENSOR:SPI:TEMPERATURE
@rem [X] Done
@rem L:\Dev\Техническая документация\ADC\16bit\ads8867_100kSPS_Diff_DaisyChain.pdf
@rem copy /Y /B "L:\Dev\Техническая документация\ADC\16bit\ads8867_100kSPS_Diff_DaisyChain.pdf" ..\datasheets\ads8867_100kSPS_Diff_DaisyChain.pdf
@rem alter name ADC_16B_100KSPS_DIF_ADS8867IDGS
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=ADS8867IDGS,ADC_16B_100KSPS_DIF_ADS8867IDGS  "-O=description:16-Bit, 100-kSPS, Serial Interface, microPower, Miniature, True-Differential Input, SAR Analog-to-Digital Converter"  -O=purpose:SENSOR:SPI "-O=datasheet:ads8867_100kSPS_Diff_DaisyChain.pdf" ADS8867IDGS.csv ADS8867IDGS.cmp


@rem [ ] Не нужно
@rem L:\Dev\Техническая документация\Усилители\Difference\ina152.pdf
@rem copy /Y /B "L:\Dev\Техническая документация\Усилители\Difference\ina152.pdf" ..\datasheets\ina152.pdf
@rem alter name AMP_DIFFC_INA152
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=INA152,AMP_DIFFC_INA152       "-O=description:Single-Supply DIFFERENCE AMPLIFIER"       -O=purpose:GENERIC INA152.csv INA152.cmp


@rem [ ] Не нужно
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=INA168       "-O=description:INA1x8 High-Side Measurement Current Shunt Monitor"       -O=purpose:GENERIC "-O=datasheet:INA138-168-chipdip-DOC001550995.pdf"  INA168.csv INA168.cmp


@rem [ ] Не нужно
@rem alter name AMP_INSTR_INA818IDR
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=INA818IDR,AMP_INSTR_INA818IDR   "-O=description:Low-Power Precision Instrumentation Amplifier"    -O=purpose:SENSOR:ADC "-O=datasheet:INA818.PDF"  INA818IDR.csv INA818IDR.cmp


@rem [ ] Не нужно
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=LM4040AIM3   "-O=description:Precision Micropower Shunt Voltage Reference"   -O=purpose:GENERIC "-O=datasheet:LM4040AIM3-chipdip-DOC009395451.pdf"  LM4040AIM3.csv LM4040AIM3.cmp


@rem [X] Done
@rem L:\Dev\Техническая документация\Память\EEPROM\M24512W.pdf
@rem copy /Y /B "L:\Dev\Техническая документация\Память\EEPROM\M24512W.pdf" ..\datasheets\M24512W.pdf
@rem alter name EEPROM_M24512W(R,F)DW
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=M24512W,EEPROM_M24512W(R,F)DW  "-O=description:512-Kbit serial I2C bus EEPROM"      -O=purpose:FLASH:I2C "-O=datasheet:M24512W.pdf"  M24512W.csv M24512W.cmp


@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=MAX17502GXX  "-O=description:MAX17502GXX"  -O=purpose:GENERIC "-O=datasheet:"  MAX17502GXX.csv MAX17502GXX.cmp
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=TPS62172DSG  "-O=description:TPS62172DSG"  -O=purpose:GENERIC "-O=datasheet:"  TPS62172DSG.csv TPS62172DSG.cmp
@rem roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=TPS71533DCKR "-O=description:TPS71533DCKR" -O=purpose:GENERIC "-O=datasheet:"  TPS71533DCKR.csv TPS71533DCKR.cmp
