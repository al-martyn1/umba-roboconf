# Как собирать под VSCode с CMake


## CMake

Чтобы не дергать админов, проще поставить из архивов. Все версии тут - https://cmake.org/files/.

Последняя 3.29 (3.29.3) тут - https://cmake.org/files/v3.29/

Версия 3.17 (3.17.5), которая последняя без баги тут - https://cmake.org/files/v3.17/.

Хотя, вроде заработало и на 3.29.

Распаковываем из архива как есть в корень диска C, и получаем такие каталоги:
```
cmake-3.17.5-win64-x64
cmake-3.29.3-windows-x86_64
```


## Настраиваем CMakeTools Kits

При сканировании все ручные изменения перезатираются, так что если вы там что-то меняли, то сделайте бэкап этого файла, 
а после сканирования ручками верните свои изменения.

> [!TIP]
> Подробнее про настройку CMake Tools можно почитать тут - https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/kits.md

В каталоге `scripts` копируем setup_kits_sample.bat в setup_kits.bat:
```
copy /Y setup_kits_sample.bat setup_kits.bat
```

Редактируем setup_kits.bat, настраивая пути к своим компиляторам, добавля свои и удаляя ненужные.

Запускаем `_start_code.bat`

Через VS Code Command Palette (Ctrl+Shift+P) набираем `CMake: Scan for Kits`. Выбираем предложенное.

Закрываем VS Code. Удаляем `scripts/setup_kits.bat`.


## Toolset configuration file

Набор хелперов для STM32/CMake тут - https://github.com/ObKo/stm32-cmake
Можно сразу сделать (куда-нибудь в локальную common папку, не в проект), например, в `C:\work\github_3dp`:
```
git clone https://github.com/ObKo/stm32-cmake.git
```

Эти скрипты будут использоваться в любом проекте CMake/VSCode.



## Настраиваем CMake Tools

### Общая информация по CMake Tools

CMake Tools какая-то нервная. И вроде её конфигурационный файл корректный, VSCode никаких ошибок там не подсвечивает,
но почему-то после правок тулчейна в конфиге CMake Tools запуск CMake потом отваливается - то вообще CMake не может найти,
то находит ошибку в CMakeLists.txt в позиции 1,1.

В общем, когда мы начинаем править сгенерированный сканером тулчейнов конфиг, то добавляем по одному параметру, десять раз 
всё проверяем-перезапускаем, убеждаемся, что всё работает, и только потом начинаем добавлять что-то дальше.
И да, перед каждым изменением копируем рабочий конфиг в безопасное место.

Может, я что-то не так делал, но это была большая боль.


### Настраиваем ARM CMakeTools Kit для STM32

Если для проектов под STMку потребуется CMake версии, отличной от той, что установлена в системе, то его можно задать 
для конкретного кита.

Также в настройках кита можно задать переменные окружения для генерации билд-скриптов, сборки, и отладки целевой программы (если кит 
используется для создания хостовых программ).

Также в настройках надо задать путь к конфигурационному файлу тулчейна.

Через VS Code Command Palette (Ctrl+Shift+P) набираем `CMake: Edit User-Local CMake Kits`. Выбираем предложенное.

В открывшемся файле находим наш армовский кит:
```json
{
    "name": "GCC 13.2.1 arm-none-eabi",
    "compilers":{
      "C": "C:\\Program Files (x86)\\Arm GNU Toolchain arm-none-eabi\\13.2 Rel1\\bin\\arm-none-eabi-gcc.exe",
      "CXX": "C:\\Program Files (x86)\\Arm GNU Toolchain arm-none-eabi\\13.2 Rel1\\bin\\arm-none-eabi-g++.exe"
    },
    "cmakePath": "C:\\cmake-3.17.5-win64-x64\\bin\\cmake.exe",
    "toolchainFile": "C:\\work\\github_3dp\\stm32-cmake\\cmake\\stm32_gcc.cmake",
    "cmakeSettings": {
      "buildDirectory": "${workspaceFolder}/.build/${buildKit}/stm32-arm/${buildType}",
      "loggingLevel": "trace",
      "environment": {
        "STM32_TOOLCHAIN_PATH": "C:\\Program Files (x86)\\Arm GNU Toolchain arm-none-eabi\\13.2 Rel1",
        "OPENOCD_ROOT": "C:\\OpenOCD",
        "OPENOCD_SHARE": "C:\\OpenOCD\\share"
      },
      "isTrusted": true,
      "preferredGenerator": {
        "name": "MinGW Make"
      }
    }
  },
```

В опции `cmakePath` задаем имя с полным путем к нужному CMake.

В опции `toolchainFile` задаем путь к тулчейн-файлу, который склонировали с гитхаба.

В опции `buildDirectory` можно задать путь для генерации, но, похоже, опция тулчейна имеет меньший приоритет, чем 
аналогичная опция в конфиге VSCode.

Переменная окружения `STM32_TOOLCHAIN_PATH` в общем-то не нужна, тулчейн-файл сам находит его по пути к сишному компилятору тулчейна.


У меня это не получилось, но вдруг у кого-то получится.
Или кто-то найдёт у меня баги в моих действиях, что я делаю не так.

К счастью, всё внезапно заработало, и пока обратно не сломалось. И работает. И CMake 3.29, прописанный в путях, работает.


### Настраиваем CMake для использованияемую по умолчанию в VSCode

Если CMake не прописан в путях, его можно прописать в VSCode.

Через VS Code Command Palette (Ctrl+Shift+P) набираем `Settings.json` - предлагает выбор из нескольких вариантов:

 - Preferences: Open User Setting (JSON)
 - Preferences: Open Default Setting (JSON)
 - Preferences: Open Workspace Setting (JSON)

Выбираем `User Setting` - глобальные настройки VSCode для текущего пользователя:
```json
{
    "workbench.colorTheme": "Default Light Modern",
    "explorer.confirmDelete": false,
    "cmake.showOptionsMovedNotification": false,
    "cortex-debug.armToolchainPath": "C:\\Program Files (x86)\\Arm GNU Toolchain arm-none-eabi\\13.2 Rel1\\bin",
    "cortex-debug.openocdPath": "C:\\OpenOCD\\bin\\openocd.exe",
    "cortex-debug.gdbPath": "C:\\Program Files (x86)\\Arm GNU Toolchain arm-none-eabi\\13.2 Rel1\\bin\\arm-none-eabi-gdb.exe",
    "cmake.cmakePath": "C:\\cmake-3.29.3-windows-x86_64\\bin\\cmake.exe",
    "cmake.configureSettings": {},
    "cmake.configureOnOpen": true,
    "cmake.buildDirectory": "${workspaceFolder}/.build/${buildKit}/${buildKitTargetOs}-${buildKitTargetArch}/${buildType}",
    "cmake.debugConfig": {
      "all": {
        "cwd": "${workspaceRoot}"
      }
    },
    "git.openRepositoryInParentFolders": "always",
    "security.workspace.trust.untrustedFiles": "open",
}
```

Интересует опция `cmake.cmakePath` - тут указываем имя EXE-шника CMake'а с полным путем.

В опции `buildDirectory` можно задать путь для генерации сборочных скриптов и помещения туда выходных файлов. Это значение 
будет работать для всех китов.


## Настройка через переменные окружения


### Настраиваем CMake не через VSCode

Для того, чтобы CMake был доступен, можно просто прописать его `bin` каталог в системную переменную `PATH`.

Для того, чтобы в VSCode использовалась версия CMake, отличная от той, которая прописана в PATH, воспользуемся
запускающим bat-файлом `_start_code.bat`, который дергает bat-файлы из подкаталога `scripts`.

Для настройки CMake см. `scripts/setup_cmake_sample.bat`.



### Настраиваем пути к тулчейну и OPENOCD

Идем в папку `scripts`.
Копируем `*_sample.bat` в файлы без суффикса `_sample`
```
copy setup_gcc_arm_none_eabi_toolchain_sample.bat setup_gcc_arm_none_eabi_toolchain.bat
copy setup_openocd_sample.bat                     setup_openocd.bat
```

В файле `setup_gcc_arm_none_eabi_toolchain.bat` надо настроить путь к тулчейн конфигу, 
который лежит в ранее склонированном репозитории: `%CLONES_COMMON_ROOT%\stm32-cmake\cmake\stm32_gcc.cmake`

Тут же настраиваем путь к самому тулчейну. Каталог `bin` не включаем в путь, указываем путь до корня установки.

В файле `setup_openocd.bat` настраиваем путь к OPEN OCD.



## Резюмируя

Я перепробовал несколько способов, изначально у меня заработал простой способ через переменные окружения,
но были высказаны предложения обойтись без них.

Сейчас всё вроде получилось, и тулчейн-файл указан в настройках CMake Tools, и вроде как всё работает, но 
в процессе настройки у меня переставали работать все вышеперечисленные способы с очень странными сообщениями.

> [!NOTE]
> Если настраивать тулчейн в CMAke Tools и указывать путь к тулчейн-файлу там, то возможно не получится собирать 
одним тулчейном под различные микроконтроллеры. Подход с настройкой переменной окружения в bat-файле гибче. Но в 
этом случае надо ручками импортировать переменную окружения в CMake-переменную командой 
`set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})`.



