/*!
\file
\brief Подсистема hr_counter - счетчики высокого разрешения

    Максимальные величины, которые можно получать на 32х битном типе на STM32:

    0x7FFFFFFF*2/ 72000000 = 59.65 сек
    0x7FFFFFFF*2/168000000 = 25.56 сек

    STM32F7 - 216 МГц
    STM32H7 - 400 МГц

    0x7FFFFFFF*2/400000000 = 10 сек

    Минимальная задержка, которую можно получить на STM32F1 72MHz
    функцией delayNanosec - 1740 нс


*/

#pragma once

#include "umba/x_detect_environment.h"
#include "umba/preprocessor.h"
#include "umba/user_literals.h"

#include <stdint.h>


#if defined(UMBA_WIN32_USED)

    // Ничего дополнительно подключать не требуется
#elif defined(UMBA_QT_USED)
    // Ничего дополнительно подключать не требуется
#elif defined(UMBA_FREERTOS_USED)
    // Ничего дополнительно подключать не требуется
#else
#endif


// umba::hr_counter::

namespace umba{
namespace hr_counter{



#if defined(UMBA_WIN32_USED)



    typedef uint64_t  HiResTick;
    typedef HiResTick NanosecInterval;
    typedef uint64_t  HiResTickLong;

    inline
    bool isCounterAvailable()
    {
        LARGE_INTEGER li;
        if (!QueryPerformanceCounter(&li))
            return false;
        if (!QueryPerformanceFrequency(&li))
            return false;
        return true;
    }

    inline
    HiResTick getTickFreqKHz()
    {
        LARGE_INTEGER li;
        if (!QueryPerformanceFrequency(&li))
            li.QuadPart = 1000*1000*1000; // assign 1GHz
        HiResTick freqKHz = ((HiResTick)li.QuadPart) / 1000;
        return freqKHz;
    }

    inline
    HiResTick getTick()
    {
        LARGE_INTEGER li;
        li.QuadPart = 0;
        if (!QueryPerformanceCounter(&li))
           return (HiResTick)0;
        return (HiResTick)li.QuadPart;
    }

    inline
    NanosecInterval convertTickToNanosec( HiResTick tick )
    {
        return (tick*1000*1000) / getTickFreqKHz();
    }

    // Not required - types are the same
    // HiResTickLong convertTickToNanosec( HiResTickLong tick )

    inline
    HiResTick convertNanosecToTick( NanosecInterval ns )
    {
        return ns*getTickFreqKHz() / (1000*1000);
    }

    inline
    void delayNanosec( NanosecInterval deltaNanosec )
    {
        HiResTick tickStart = getTick();
        if (!isCounterAvailable())
            return;
    
        HiResTick deltaTick = convertNanosecToTick( deltaNanosec );
    
        while( (getTick() - tickStart) <= deltaTick) {}
    }

    inline
    void delayNanosecHard( NanosecInterval deltaNanosec )
    {
        delayNanosec( deltaNanosec );
    }

    

#elif defined(UMBA_MCU_USED)

    typedef uint32_t  HiResTick;
    typedef HiResTick NanosecInterval;
    typedef uint64_t  HiResTickLong;

    #include "optimize_speed.h"
    UMBA_FORCE_INLINE( bool isCounterAvailable() )
    {
        #if !defined(DWT)
             return false;
        #else
             // Some magic found in ARMv8 man
             if ((DWT->CTRL & 1)==0)
             {
                 __OM uint32_t *DWT_LAR = (uint32_t*)0xE0001FB0;
                 *DWT_LAR = 0xC5ACCE55; // Unlock magic constant
                 CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
                 DWT->CTRL |= 1;
                 *DWT_LAR   = 0; // lock again
                 //*DWT_CYCCNT = 0;
             }

             HiResTick tck1 = DWT->CYCCNT;
             if (tck1==0) // min chanse that it is real zero
             {
                 HiResTick tck2 = DWT->CYCCNT;
                 if (tck2==0)
                     return false; // not implemented
             }

             if (tck1==DWT->CYCCNT)
                 return false;

             return true;
        #endif
    }

    UMBA_FORCE_INLINE( HiResTick getTickFreqKHz() )
    {
        return SystemCoreClock / 1000;
    }

    UMBA_FORCE_INLINE( HiResTick getTick() )
    {
        #if !defined(DWT)
            return 0;
        #else
            return DWT->CYCCNT;
        #endif
    }

    UMBA_FORCE_INLINE( NanosecInterval convertTickToNanosec( HiResTick tick ) )
    {
        if (tick<1000)
            return (tick*1000*1000) / getTickFreqKHz();
        else
            return (tick*1000) / (getTickFreqKHz()/1000);
    }

    UMBA_FORCE_INLINE( HiResTick convertNanosecToTick( NanosecInterval ns ) )
    {
        if (ns<5000)
            return ns*getTickFreqKHz() / (1000*1000);
        else
            return ns*(getTickFreqKHz()/1000) / (1000);
    }

    inline
    HiResTickLong convertTickToNanosec( HiResTickLong tick )
    {
        return (tick*1000*1000) / (HiResTickLong)getTickFreqKHz();
    }

    inline
    HiResTickLong convertNanosecToTick( HiResTickLong ns )
    {
        return ns*(HiResTickLong)getTickFreqKHz() / (1000*1000);
    }

    inline
    void delayNanosec( NanosecInterval deltaNanosec )
    {
        HiResTick tickStart = getTick();
        if (!isCounterAvailable())
            return;


        // Эмпирически подобранная коррекция

        #if defined(STM32F1_SERIES)

            deltaNanosec *= 10;
            deltaNanosec /= 16;

            if (deltaNanosec>1000)
                deltaNanosec-=1000;
            else
                deltaNanosec = 0;

        #elif defined(STM32F3_SERIES)

            deltaNanosec *= 10;
            deltaNanosec /= 18;

            if (deltaNanosec>800)
                deltaNanosec-=800;
            else
                deltaNanosec = 0;

        #elif defined(STM32F4_SERIES)

            if (deltaNanosec>200)
                deltaNanosec-=200;
            else
                deltaNanosec = 0;

        #else
            
        #endif

        HiResTick deltaTick = convertNanosecToTick( deltaNanosec );
        while( (getTick() - tickStart) <= deltaTick)
        {
            #if defined(STM32F1_SERIES)
            //UMBA_MEMORY_BARRIER();
            #endif
        }
    }

    inline
    void delayNanosecHard( NanosecInterval deltaNanosec )
    {
        uint32_t isMasked = UMBA_INTERRUPTS_DISABLED(); 
        UMBA_DISABLE_IRQ();
        delayNanosec(deltaNanosec);
        if (!isMasked)
           UMBA_ENABLE_IRQ();
    }


    #include "optimize_pop.h"

#endif


UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( NanosecInterval, nanosec )
{
    return value;
}
UMBA_DEFINE_USER_LITERAL_TYPE_END( )

UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( NanosecInterval, microsec )
{
    return value*1000;
}
UMBA_DEFINE_USER_LITERAL_TYPE_END( )

UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( NanosecInterval, millisec )
{
    return value*1000*1000;
}
UMBA_DEFINE_USER_LITERAL_TYPE_END( )




} // namespace hr_counter
} // namespace umba


