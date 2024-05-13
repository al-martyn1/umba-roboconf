/*!
\file
\brief Подсистема time_service
*/

#pragma once

#include "umba/x_detect_environment.h"
#include "umba/preprocessor.h"
#include <stdint.h>


#if defined(UMBA_WIN32_USED)

    // Ничего дополнительно подключать не требуется
#elif defined(UMBA_QT_USED)
    // Ничего дополнительно подключать не требуется
#elif defined(UMBA_FREERTOS_USED)
    // Ничего дополнительно подключать не требуется
#else
#endif



namespace umba{
namespace time_service{


    #ifdef DOXYGEN

        //! Platform specific type for timer ticks representation, Usually it's defined as uint32_t, DWORD etc
        typedef PlatformSpecific   TimeTick;
        typedef uint64_t           HiresTimeTick; // mucroseconds

    #else

        typedef uint64_t           HiresTimeTick; // mucroseconds

        #if defined(UMBA_WIN32_USED)
            typedef DWORD      TimeTick;
        #elif defined(UMBA_FREERTOS_USED)
            typedef TickType_t TimeTick;
        #else
            typedef uint32_t   TimeTick;
        #endif

    #endif



    //! Инициализация сервиса времени (и его запуск). 
    /*! Требуется только на голой системе. Для FreeRTOS инициализация происходит при старте шедулера */
    void init();

    //! Запуск сервиса времени. Реализовано только на голом железе
    void start();

    //! Остановка сервиса времени. Реализовано только на голом железе
    void stop();
    
    //! Перевод системных тиков в милисекунды
    TimeTick ticksToMs(TimeTick ticks);
    //! Перевод милисекунд в системных тики
    TimeTick msToTicks(TimeTick ms);

    //! Текущее время (тик), приведенное к милисекундам
    TimeTick getCurTimeMs();

    //! Текущее время (тик) в микросекундах
    HiresTimeTick getCurTimeHires();
    void getCurTimeHiresRaw( TimeTick* pHigh, TimeTick *pLow, TimeTick *pModule);
    HiresTimeTick calcCurTimeHires( TimeTick hPart, TimeTick lPart, TimeTick module );

    //! Текущее время (тик) приведенный к милисекундам - для совместимости со старым кодом
    UMBA_DECLARE_DEPRECATED_FN( TimeTick getCurTime() );

    //! Производит задержку выполнения
    /*! Не совместима с прерываниями и не должна вызываться в них */
    void  delayMs(TimeTick deltaMs);

    //! Производит задержку выполнения (устаревшее имя), см также delayMs
    UMBA_DECLARE_DEPRECATED_FN( void delay_ms(TimeTick deltaMs) );


    // DWT->CYCCNT


} // namespace time_service
} // namespace umba


