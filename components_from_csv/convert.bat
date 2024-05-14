@rem CSV separator set option
@rem "Pin Designator,Net Name,Display Name" -> "Designator,Net,Name"
@rem "# Altium Pin Report File" -> "Report Generated From Altium Designer;;;;;;;;;;;;;;;;;;;;;;;;"
@rem --csv-import-option is -O

@rem Altium 20 - "-O=designator-field-title:Pin Designator" "-O=name-field-title:Display Name"
@rem Altium 17 - "-O=designator-field-title:Designator"     "-O=name-field-title:Name"
@set CSV_FORMAT_OPTS=-O=csv-separator:comma "-O=designator-field-title:Pin Designator" "-O=name-field-title:Display Name"


@rem ../lib/
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=FLASH_NAND_2GB_MT29F2G08ABAEAWP-IT "-O=description:Nand Flash" -O=purpose:GENERIC FLASH_NAND_2GB_MT29F2G08ABAEAWP-IT.csv FLASH_NAND_2GB_MT29F2G08ABAEAWP-IT.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=74HC4067DB   "-O=description:16:1 Analog Multiplexer" -O=purpose:SWITCH:MUX 74HC4067DB.csv 74HC4067DB.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=AD8656ARM    "-O=description:AD8656ARM"    -O=purpose:GENERIC AD8656ARM.csv AD8656ARM.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=ADS8867IDGS  "-O=description:ADS8867IDGS"  -O=purpose:GENERIC ADS8867IDGS.csv ADS8867IDGS.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=INA152       "-O=description:INA152"       -O=purpose:GENERIC INA152.csv INA152.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=INA168       "-O=description:INA168"       -O=purpose:GENERIC INA168.csv INA168.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=INA818IDR    "-O=description:INA818IDR"    -O=purpose:GENERIC INA818IDR.csv 74HC4067DB.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=LM4040AIM3   "-O=description:LM4040AIM3"   -O=purpose:GENERIC LM4040AIM3.csv LM4040AIM3.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=M24512W      "-O=description:M24512W"      -O=purpose:GENERIC M24512W.csv M24512W.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=MAX17502GXX  "-O=description:MAX17502GXX"  -O=purpose:GENERIC MAX17502GXX.csv MAX17502GXX.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=TPS62172DSG  "-O=description:TPS62172DSG"  -O=purpose:GENERIC TPS62172DSG.csv TPS62172DSG.cmp
roboconf.exe %CSV_FORMAT_OPTS% "-O=package:*" --csv-component=TPS71533DCKR "-O=description:TPS71533DCKR" -O=purpose:GENERIC TPS71533DCKR.csv TPS71533DCKR.cmp
