
@if exist ..\.out\msvc\Win32\conf       del /S /Q ..\.out\msvc\Win32\conf      >nul 2>&1
@if exist ..\.out\msvc\Win32\lib        del /S /Q ..\.out\msvc\Win32\lib       >nul 2>&1
@if exist ..\.out\msvc\Win32\lib-user   del /S /Q ..\.out\msvc\Win32\lib-user  >nul 2>&1
@if exist ..\.out\msvc\Win32\lib-cube   del /S /Q ..\.out\msvc\Win32\lib-cube  >nul 2>&1
@if exist ..\.out\msvc\Win32\rules      del /S /Q ..\.out\msvc\Win32\rules     >nul 2>&1

@if exist ..\.out\msvc\x64\conf         del /S /Q ..\.out\msvc\x64\conf        >nul 2>&1
@if exist ..\.out\msvc\x64\lib          del /S /Q ..\.out\msvc\x64\lib         >nul 2>&1
@if exist ..\.out\msvc\x64\lib-user     del /S /Q ..\.out\msvc\x64\lib-user    >nul 2>&1
@if exist ..\.out\msvc\x64\lib-cube     del /S /Q ..\.out\msvc\x64\lib-cube    >nul 2>&1
@if exist ..\.out\msvc\x64\rules        del /S /Q ..\.out\msvc\x64\rules       >nul 2>&1


@if exist c:\roboconf                  del /S /Q c:\roboconf                >nul 2>&1

@rem echo off
@rem /Q - Запрет вывода имен копируемых файлов.
@rem /E - Копирование каталогов с подкаталогами, включая пустые.
@rem /Y Подавление запроса подтверждения на перезапись существующего
@rem /I Если целевой объект не существует и копируется несколько файлов, считается, что целевой объект задает каталог.

@xcopy /Q /E /Y /I ..\conf       ..\.out\msvc\Win32\conf
@xcopy /Q /E /Y /I ..\lib        ..\.out\msvc\Win32\lib
@xcopy /Q /E /Y /I ..\lib-user   ..\.out\msvc\Win32\lib-user
@xcopy /Q /E /Y /I ..\lib-cube   ..\.out\msvc\Win32\lib-cube
@xcopy /Q /E /Y /I ..\rules      ..\.out\msvc\Win32\rules

@xcopy /Q /E /Y /I ..\conf       ..\.out\msvc\x64\conf
@xcopy /Q /E /Y /I ..\lib        ..\.out\msvc\x64\lib
@xcopy /Q /E /Y /I ..\lib-user   ..\.out\msvc\x64\lib-user
@xcopy /Q /E /Y /I ..\lib-cube   ..\.out\msvc\x64\lib-cube
@xcopy /Q /E /Y /I ..\rules      ..\.out\msvc\x64\rules


@rem if not exist c:\roboconf mkdir c:\roboconf
@rem xcopy /Q /E /Y /I ..\lib       c:\roboconf\lib
@rem xcopy /Q /E /Y /I ..\lib-user  c:\roboconf\lib-user
