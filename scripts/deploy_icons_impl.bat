@if "%RBC_ROOT%"=="" goto ERR

@xcopy /E /Y /I %~dp0\..\icons\document.ico        "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\document-www.ico    "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\pdf.ico             "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\pdf-www.ico         "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\word.ico            "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\word-www.ico        "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\www.ico             "%RBC_ROOT%\icons"

@xcopy /E /Y /I %~dp0\..\icons\document-16.png        "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\document-32.png        "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\document-www-16.png    "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\document-www-32.png    "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\pdf-16.png             "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\pdf-32.png             "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\pdf-www-16.png         "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\pdf-www-32.png         "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\wikipedia-16.png       "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\wikipedia-32.png       "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\word-16.png            "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\word-32.png            "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\word-www-16.png        "%RBC_ROOT%\icons"
@xcopy /E /Y /I %~dp0\..\icons\word-www-32.png        "%RBC_ROOT%\icons"

@goto END

:ERR
@echo RBC_ROOT is not set

:END
