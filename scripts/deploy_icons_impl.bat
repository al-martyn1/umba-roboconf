@if "%RBC_ROOT%"=="" goto ERR

@xcopy /E /Y /I %~dp0\..\icons\document.ico        %RBC_ROOT%\icons
@xcopy /E /Y /I %~dp0\..\icons\document-www.ico    %RBC_ROOT%\icons
@xcopy /E /Y /I %~dp0\..\icons\pdf.ico             %RBC_ROOT%\icons
@xcopy /E /Y /I %~dp0\..\icons\pdf-www.ico         %RBC_ROOT%\icons
@xcopy /E /Y /I %~dp0\..\icons\word.ico            %RBC_ROOT%\icons
@xcopy /E /Y /I %~dp0\..\icons\word-www.ico        %RBC_ROOT%\icons
@xcopy /E /Y /I %~dp0\..\icons\www.ico             %RBC_ROOT%\icons

@goto END

:ERR
@echo RBC_ROOT is not set

:END
