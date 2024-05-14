@rem CSV separator set option
@rem "Pin Designator,Net Name,Display Name" -> "Designator,Net,Name"
@rem "# Altium Pin Report File" -> "Report Generated From Altium Designer;;;;;;;;;;;;;;;;;;;;;;;;"
@rem --csv-import-option is -O

@rem ../lib/
roboconf.exe --csv-component=74HC4067DB "-O=description:16:1 Analog Multiplexer" -O=purpose:SWITCH:MUX "-O=package:*" 74HC4067DB.csv 74HC4067DB.cmp
