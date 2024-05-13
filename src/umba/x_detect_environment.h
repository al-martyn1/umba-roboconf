/*!
\file
\brief Определение сборочного окружения и целевой платформы.

Существует два способа использования: 1) для проекта задаются predefined макросы,
которые настраивают библиотеку - UMBA_USE_MCU, UMBA_USE_QT ..., и тогда библиотека 
сама подключает необходимые хидеры, и 2) когда перед подключением заголовков библиотеки
подключаются сначала системные хидеры

1) UMBA_USE_MCU, UMBA_USE_QT - для использования хидеров stm и Qt.
   UMBA_USE_WIN32 - не нужно определять, так как при компиляции под Windows всегда определяется макрос _WIN32
   UMBA_USE_FREERTOS - для подключения хидеров FreeRTOS


2) Все системные заголовки требуется подключать до использования библиотеки UMBA.
Под системными подразувеваются заголовки Windows, Qt, FreeRTOS и тп.
Если ничего не используется, то предполагается, что используется камень,
и производится подключение заголовков CMSIS для сответствующего МК.

При сборке под Windows определен макрос \b _WIN32, конфигурация определяется как
\b UMBA_WIN32_USED и подключаются необходимые заголовочные файлы.


Для проекта можно задавать макрос 
\b UMBA_USE_FREERTOS
(в настройках проекта/компилятора). При его задании автоматически 
подключаться требуемые заголовки FreeRTOS.

Для отключения всех устаревших возможностей следует задать макрос
\b UMBA_DISABLE_DEPRECATED.

Для отключения всех отладочных проверок (полный релиз) следует задать макрос
\b UMBA_NDEBUG.

В зависимости от системы автоматически определяются макросы:
- \b UMBA_WIN32_USED, 
- \b UMBA_FREERTOS_USED,
- \b UMBA_QT_USED.

Их можно использовать в своем коде для проверки окружения.
Для Windows \b UMBA_WIN32_USED определен всегда, и может быть определен одновременно с \b UMBA_QT_USED.

\b UMBA_MCU_USED - определен, когда библиотека используется для сборки под железо

В зависимости от используемого компилятора автоматически определяются макросы:
- \b UMBA_MSVC_COMPILER_USED,
- \b UMBA_KEIL_ARMCC_COMPILER_USED,
- \b UMBA_KEIL_CLANG_COMPILER_USED,
- \b UMBA_IAR_ICC_COMPILER_USED,
- \b UMBA_GCC_COMPILER_USED.

Препроцессор:
- \b UMBA_PP_HAS_COUNTER / \b UMBA_PP_NO_COUNTER - есть или нет макрос \b `__COUNTER__`.

В зависимости от версии языка C++ определяются макросы: 
- \b UMBA_CXX_HAS_STD03,
- \b UMBA_CXX_HAS_STD11,
- \b UMBA_CXX_HAS_STD14,
- \b UMBA_CXX_HAS_STD17.

*/

#pragma once

#ifdef UMBA_STRICT
    #ifndef UMBA_DISABLE_DEPRECATED
        #define UMBA_DISABLE_DEPRECATED
    #endif
#endif


#if defined(_WIN32)
    #include <winsock2.h>
    #include <windows.h>
#endif

#if defined(UMBA_USE_QT)
    #include <QObject>
#endif


#if defined(QT_VERSION) || defined(UMBA_USE_QT)
    #ifndef UMBA_QT_USED
        #define UMBA_QT_USED
    #endif
#endif


#if defined(_WIN32) || defined(WIN32)
    #define UMBA_WIN32_USED
#endif


#ifdef UMBA_USE_FREERTOS
    #include <FreeRTOS.h>
    #define  UMBA_FREERTOS_USED
#else
    #ifdef INC_FREERTOS_H
        #define UMBA_FREERTOS_USED
    #endif
#endif


//! @cond Doxygen_Suppress_Not_Documented
#if !defined(UMBA_WIN32_USED) && !defined(UMBA_QT_USED)
    #define UMBA_MCU_USED
#endif
//! @endcond

#if defined(_MSC_VER)

    #define UMBA_MSVC_COMPILER_USED

#elif defined(__CC_ARM)

    #define UMBA_KEIL_ARMCC_COMPILER_USED

#elif defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
//#elif defined (__ARMCC_VERSION) && defined(__clang__)
    /* Как правильнее? */

    #define UMBA_KEIL_CLANG_COMPILER_USED

#elif defined(__ICCARM__) || defined(__IAR_SYSTEMS_ICC__)

    #define UMBA_IAR_ICC_COMPILER_USED

#elif defined(__GNUC__)

    #define UMBA_GCC_COMPILER_USED

//#elif defined(__CWCC__)
    /* Что такое CWCC? CodeWarrior? */

#else

    #error "Unknown (unsupported) compiler"

#endif

// UMBA_PP_NO_*, UMBA_PP_HAS_* - preprocessor feature flags
// UMBA_CC_ - Plain С feature flags
// UMBA_CXX_ - С++ feature flags

//! @cond Doxygen_Suppress_Not_Documented
#if defined(__COUNTER__)
    #define UMBA_PP_HAS_COUNTER
#else
    #define UMBA_PP_NO_COUNTER
#endif
//! @endcond


//! @cond Doxygen_Suppress_Not_Documented
#if defined(_MSC_VER) && defined(_MSVC_LANG)
// Microsoft, как обычно, выделывается
// Поддерживаются только ключи - /std:c++14 and /std:c++latest
// /std:c++14  - 201402
// /std:c++latest - 201403

    #if __cplusplus >= 199711L
        #define UMBA_CXX_HAS_STD03
    #endif

    #if _MSVC_LANG >= 201103L
        #define UMBA_CXX_HAS_STD11
    #endif

    #if _MSVC_LANG >= 201402L
        #define UMBA_CXX_HAS_STD14
    #endif

    #if _MSVC_LANG >= 201703L
        #define UMBA_CXX_HAS_STD17
    #endif


#elif defined(__cplusplus)

    #if __cplusplus >= 201703L
        #define UMBA_CXX_HAS_STD17
    #endif
   
    #if __cplusplus >= 201402L
        #define UMBA_CXX_HAS_STD14
    #endif

    #if __cplusplus >= 201103L
        #define UMBA_CXX_HAS_STD11
    #endif

    #if __cplusplus >= 199711L
        #define UMBA_CXX_HAS_STD03
    #endif

#endif
//! @endcond



//! Определяется модулем UMBA для возможности автоматического определения факта использования данного модуля
#define MODULE_UMBA_USED



#if defined(UMBA_WIN32_USED)

    // Подключаем WinSock2.h перед подключением Windows.h.
    // Если так не сделать, и потом захочется использовать сокеты,
    // то будет конфликт с WinSock.h, который подключается из Windows.h
    #if !defined(_WINSOCK2API_)
        #include <WinSock2.h>
    #endif
    
    #if !defined(_WINDOWS_)
        #include <Windows.h>
    #endif

#elif defined(UMBA_QT_USED)

    // Тут можно подключать Qt

#elif defined(UMBA_MCU_USED)

    #include "stm32.h"
    #include "stm32_drivers.h"
    

    // Подключаем общие для всех STM заголовки из CMSIS
    // Не нужно, сами подключаться
    //#include <core_cmFunc.h>
    //#include <core_cmInstr.h>

    //TODO: В зависимости от типа МК нужно подключить соответствующие заголовки из CMSIS
    // Сейчас подразувевается, что они подключаются при конфигурации проекта
    // Но если что-то не находится, то нужно подключить один из этих файлов до использования UMBA
    //#include <core_cm0.h>
    //#include <core_cm0plus.h>
    //#include <cmsis/core_cm3.h>
    //#include <core_cm4.h>

    //#include "mcu_support_package/inc/stm32f10x.h"
    //#include "mcu_support_package/inc/system_stm32f10x.h"

#endif



