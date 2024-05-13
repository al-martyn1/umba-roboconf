//#include "project_config.h"
#include "time_service.h"
#include "assert.h"
#include "critical_section.h"


#ifdef UMBA_FREERTOS_USED
    #include "freertos/freertos_wrapper.h"
#endif

#if defined(UMBA_MCU_USED)
    #include "x_mcu_low_level.h"
#endif



namespace umba{
namespace time_service{

    #ifdef UMBA_FREERTOS_USED

        void init() {}
        void start() {}
        void stop() {}

        TimeTick ticksToMs(TimeTick ticks)
        {
            return ticks * portTICK_PERIOD_MS;
        }
        
        TimeTick msToTicks(TimeTick ms)
        {
            return ms / portTICK_PERIOD_MS;
        }

        TimeTick getCurTime()
        {
            return ticksToMs(xTaskGetTickCount());
        }
        
        TimeTick getCurTimeMs()
        {
            if (IS_IN_ISR())
                return ticksToMs(osTaskGetTickCountFromISR());
            else
                return ticksToMs(osTaskGetTickCount());
        }

        HiresTimeTick getCurTimeHires()
        {
            return (HiresTimeTick)1000*(HiresTimeTick)getCurTimeMs();
        }
        
        void   delayMs(TimeTick deltaMs)
        {
            UMBA_ASSERT( ! IS_IN_ISR() );
            osTaskDelay(msToTicks(deltaMs));
        }
        
        void   delay_ms(TimeTick deltaMs)
        {
            UMBA_ASSERT( ! IS_IN_ISR() );
            osTaskDelay(msToTicks(deltaMs));
        }

    #elif defined(UMBA_WIN32_USED)

        void init() {}
        void start() {}
        void stop() {}

        TimeTick ticksToMs(TimeTick ticks)
        {
            return ticks;
        }
        
        TimeTick msToTicks(TimeTick ms)
        {
            return ms;
        }

        TimeTick getCurTime()
        {
            return GetTickCount();
        }

        TimeTick getCurTimeMs()
        {
            return GetTickCount();
        }

        HiresTimeTick getCurTimeHires()
        {
            LARGE_INTEGER freq;
            LARGE_INTEGER curTime;
            if (!QueryPerformanceFrequency(&freq) || !QueryPerformanceCounter(&curTime))
                return (HiresTimeTick)getCurTimeMs() * 1000ull;

            double microsecondsPerCount = 1.0e9 / static_cast<double>(freq.QuadPart);
            
            return (HiresTimeTick)(curTime.QuadPart*microsecondsPerCount);
        }

        void   delayMs(TimeTick deltaMs)
        {
            Sleep(deltaMs);
        }

        void delay_ms(TimeTick delta)
        {
            Sleep(delta);
        }


    #else

        static volatile TimeTick      systemTime = 0;
        static volatile HiresTimeTick systemTimeHires = 0;


        void init()
        {
            if( SysTick_Config(SystemCoreClock / 1000) )
            {
                UMBA_ASSERT_FAIL();
            }
        }

        void   delayMs(TimeTick deltaMs);
    
        void start(void)
        {
            SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
            #ifndef UMBA_TIMESERVICE_NO_TACTIC_DELAY
            delayMs( 250 );
            #endif
        }
    
        void stop(void)
        {
            SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
        }

        TimeTick ticksToMs(TimeTick ticks)
        {
            return ticks;
        }
        
        TimeTick msToTicks(TimeTick ms)
        {
            return ms;
        }

        TimeTick getCurTime()
        {
            return systemTime;
        }

        void getCurTimeHiresRaw( TimeTick* pHigh, TimeTick *pLow, TimeTick *pModule)
        {
            TimeTick   tickVal      = 0;
            TimeTick   hrMsecModule = 0;
            TimeTick   msTick       = 0;

            {
                UMBA_CRITICAL_SECTION_EX(umba::CriticalSection, umba::globalCriticalSection);
                tickVal = SysTick->VAL;          
                hrMsecModule = SysTick->LOAD + 1;
                msTick = systemTime;
            }

            if (pModule)
                *pModule = hrMsecModule;

            if (pLow)
                *pLow = hrMsecModule - 1 - tickVal;

            if (pHigh)
                *pHigh = msTick;

        }

        HiresTimeTick calcCurTimeHires( TimeTick hPart, TimeTick lPart, TimeTick module )
        {
            return 1000ull*(HiresTimeTick)hPart + 1000ull * (HiresTimeTick)lPart / (HiresTimeTick)module;
        }
   
        HiresTimeTick getCurTimeHires()
        {
            TimeTick tickHi, tickLo, tickMod;
            getCurTimeHiresRaw( &tickHi, &tickLo, &tickMod );
            if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
                getCurTimeHiresRaw( &tickHi, &tickLo, &tickMod );
            //HiresTimeTick tmp = umba::time_service::calcCurTimeHires( tickHi, tickLo, tickMod );
            //HiresTimeTick res = tmp > systemTimeHires ? tmp : systemTimeHires;
            //systemTimeHires = tmp;
            //return res;
            return umba::time_service::calcCurTimeHires( tickHi, tickLo, tickMod );

/*
            HiresTimeTick hrTickHigh    = 0;
            HiresTimeTick hrTickLow     = 0;
            HiresTimeTick hrMsecModule  = 0;
            {
                UMBA_CRITICAL_SECTION_EX(umba::CriticalSection, umba::globalCriticalSection);
                hrMsecModule = SysTick->LOAD + 1;
                hrTickHigh   = systemTime;
                hrTickLow    = SysTick->VAL;
            }

            hrTickLow = hrMsecModule - hrTickLow;

            return hrTickHigh*1000ull + (hrMsecModule-hrTickLow) * 1000ull / hrMsecModule;
*/
        }

        TimeTick getCurTimeMs()
        {
            return systemTime;
        }

        void   delayMs(TimeTick deltaMs)
        {
            UMBA_ASSERT( ! UMBA_IS_IN_ISR() );
            TimeTick start = getCurTimeMs();
            while( getCurTimeMs() - start <= deltaMs) {}
        }

        void delay_ms(TimeTick delta)
        {
            UMBA_ASSERT( ! UMBA_IS_IN_ISR() );
            TimeTick start = getCurTimeMs();
            while( getCurTimeMs() - start <= delta) {}
        }

    #endif
    // #ifdef UMBA_FREERTOS_USED
        
} // namespace time_service
} // namespace umba


#if !defined(UMBA_FREERTOS_USED)  && !defined(UMBA_WIN32_USED)
    extern "C"
    {
        void SysTick_Handler( void )
        {
            umba::time_service::systemTime++;
        }
    }
#endif


