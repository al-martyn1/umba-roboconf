@if "%RBC_ROOT%"=="" goto ERR

@if not exist "%RBC_ROOT%"                  @mkdir "%RBC_ROOT%"
@if not exist "%RBC_ROOT%\bin"              @mkdir "%RBC_ROOT%\bin"
@if not exist "%RBC_ROOT%\samples"          @mkdir "%RBC_ROOT%\samples"
@rem if not exist "%RBC_ROOT%\samples\netlists" @mkdir "%RBC_ROOT%\samples\netlists"
@if not exist "%RBC_ROOT%\doc"              @mkdir "%RBC_ROOT%\doc"
@if not exist "%RBC_ROOT%\conf"             @mkdir "%RBC_ROOT%\conf"
@if not exist "%RBC_ROOT%\lib"              @mkdir "%RBC_ROOT%\lib"
@if not exist "%RBC_ROOT%\lib-user"         @mkdir "%RBC_ROOT%\lib-user"
@if not exist "%RBC_ROOT%\rules"            @mkdir "%RBC_ROOT%\rules"
@if not exist "%RBC_ROOT%\icons"            @mkdir "%RBC_ROOT%\icons"


goto END

:ERR
@echo RBC_ROOT is not set

:END
