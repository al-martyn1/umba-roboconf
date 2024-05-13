/*!
\file
\brief Различные общие макросы препроцессора

Можно задавать поведение:
\b UMBA_STRICT  - если включить, то полезет всякое гавно, на которое следует обратить внимание, задает также UMBA_DISABLE_DEPRECATED
\b UMBA_DISABLE_DEPRECATED - отключить deprecated функции
*/

#pragma once

#include "umba/x_detect_environment.h"


//---------------------------------------------------------
#ifndef UMBA_KEEP_MINMAX

    #ifdef min
        #undef min
    #endif

    #ifdef max
        #undef max
    #endif

#endif  /* UMBA_KEEP_MINMAX */ 


//---------------------------------------------------------
//! Вычисление количества элементов массива
#define UMBA_COUNT_OF(a)               (sizeof (a) / sizeof (a[0]))
//! Legacy версия UMBA_COUNT_OF
#ifndef NUM_ELEM
    #define NUM_ELEM(a)                    UMBA_COUNT_OF(a)
#endif


//---------------------------------------------------------
//! Стрингификация - промежуточный макрос
#define UMBA_STRINGIFY_HELPER(x)       #x

//! Стрингификация
#define UMBA_STRINGIFY(x)              UMBA_STRINGIFY_HELPER(x)


//---------------------------------------------------------
//! Подавление варнинга о неиспользованном аргументе
#define UMBA_USED(x)                   (void)(x)



//---------------------------------------------------------
/*!
\def UMBA_DEPRECATED
\brief Компиляторо-зависимый макрос для пометки функций как устаревшие
*/

/*!
\def UMBA_DEPRECATED_MSG(msg)
\brief Компиляторо-зависимый макрос для пометки функций как устаревшие, с выдачей сообщения
*/

#if defined(UMBA_MSVC_COMPILER_USED)

    #if !defined(_MSVC_LANG) || _MSVC_LANG < 201402
        #define UMBA_DEPRECATED            __declspec(deprecated)
        #define UMBA_DEPRECATED_MSG(msg)   __declspec(deprecated(msg))
    #else
        #define UMBA_DEPRECATED            [[deprecated]]
        #define UMBA_DEPRECATED_MSG(msg)   [[deprecated(msg)]]
    #endif

#elif defined(UMBA_KEIL_ARMCC_COMPILER_USED)

    #define UMBA_DEPRECATED
    #define UMBA_DEPRECATED_MSG(msg)

#elif defined(UMBA_KEIL_CLANG_COMPILER_USED) 

    #define UMBA_DEPRECATED                __attribute__((deprecated))
    #define UMBA_DEPRECATED_MSG(msg)       __attribute__((deprecated(msg)))

#elif defined(__GNUC__)

    #define UMBA_DEPRECATED                __attribute__((deprecated))
    #define UMBA_DEPRECATED_MSG(msg)       __attribute__((deprecated(msg)))

#else

    /* UMBA_IAR_ICC_COMPILER_USED и другие */

    #define UMBA_DEPRECATED
    #define UMBA_DEPRECATED_MSG(msg)

#endif


//! Синтаксический сахар для объявления некопируемых классов
#define UMBA_NON_COPYABLE_CLASS(className)                                   \
                                private:                                     \
                                    className(const className &);            \
                                    className& operator=(const className &);


//! Объявление функции-обработчика прерывания. Можно использовать как для собственно
//! объявления функции, так и для реализации
#define UMBA_IRQ_HANDLER(procName)  extern "C" void procName()


//---------------------------------------------------------
/*!
\def UMBA_DECLARE_DEPRECATED_FN(...)
\brief Макрос для пометки функций как устаревшие.
По возможности выдает предупреждение, если задан макрос UMBA_DISABLE_DEPRECATED, то не определяет функцию
*/

/*!
\def UMBA_DECLARE_DEPRECATED_FN_MSG( msg, ... )
\brief Макрос для пометки функций как устаревшие.
По возможности выдает предупреждение (с выдачей сообщения), если задан макрос UMBA_DISABLE_DEPRECATED, то ломает компиляцию.
*/

#if defined(UMBA_DISABLE_DEPRECATED)
    //#error "UMBA_DISABLE_DEPRECATED defined"
    #define UMBA_DECLARE_DEPRECATED_FN( ... )
    #define UMBA_DECLARE_DEPRECATED_FN_MSG( msg, ... )
#else
    //#error "UMBA_DISABLE_DEPRECATED - NOT defined!!!"
    #define UMBA_DECLARE_DEPRECATED_FN( ... )             UMBA_DEPRECATED __VA_ARGS__
    #define UMBA_DECLARE_DEPRECATED_FN_MSG( msg, ... )    UMBA_DEPRECATED_MSG(msg) __VA_ARGS__
#endif

#ifndef UMBA_FORCE_INLINE

    #if defined(UMBA_KEIL_ARMCC_COMPILER_USED)

        #define UMBA_FORCE_INLINE( funcFullSignature )   __forceinline funcFullSignature

    #elif defined(UMBA_GCC_COMPILER_USED) || defined(UMBA_KEIL_CLANG_COMPILER_USED)

        #define UMBA_FORCE_INLINE( funcFullSignature )   inline __attribute__ ((always_inline)) funcFullSignature

    #else

        #define UMBA_FORCE_INLINE( funcFullSignature )   inline funcFullSignature

    #endif

#endif

#ifndef UMBA_FORCE_INLINE_MEMBER

    #if defined(UMBA_KEIL_ARMCC_COMPILER_USED)

        #define UMBA_FORCE_INLINE_MEMBER( funcFullSignature )   __forceinline funcFullSignature

    #elif defined(UMBA_GCC_COMPILER_USED) || defined(UMBA_KEIL_CLANG_COMPILER_USED)

        #define UMBA_FORCE_INLINE_MEMBER( funcFullSignature )   __attribute__ ((always_inline)) funcFullSignature

    #else

        #define UMBA_FORCE_INLINE_MEMBER( funcFullSignature )   funcFullSignature

    #endif

#endif

/*
#ifndef UMBA_FORCE_INLINE_DECLARATION

    #if defined(UMBA_KEIL_ARMCC_COMPILER_USED)

        #define UMBA_FORCE_INLINE_DECLARATION( funcFullSignature )   __forceinline funcFullSignature

    #elif defined(UMBA_GCC_COMPILER_USED) || defined(UMBA_KEIL_CLANG_COMPILER_USED)

        #define UMBA_FORCE_INLINE_DECLARATION( funcFullSignature )   inline __attribute__ ((always_inline)) funcFullSignature

    #else

        #define UMBA_FORCE_INLINE_DECLARATION( funcFullSignature )   funcFullSignature

    #endif

#endif
*/



#ifndef UMBA_DECLARE_WEAK_FUNCTION

    #if defined(UMBA_KEIL_ARMCC_COMPILER_USED)

        #define UMBA_DECLARE_WEAK_FUNCTION( funcFullSignature )   __weak funcFullSignature

    #elif defined(UMBA_GCC_COMPILER_USED) || defined(UMBA_KEIL_CLANG_COMPILER_USED)

        #define UMBA_DECLARE_WEAK_FUNCTION( funcFullSignature )   __attribute__ ((weakref)) funcFullSignature

    #endif

#endif

#ifndef UMBA_DECLARE_WEAK_IRQ_HANDLER
    #define UMBA_DECLARE_WEAK_IRQ_HANDLER( funcName )     extern "C" UMBA_DECLARE_WEAK_FUNCTION( void funcName() )
#endif

#ifndef UMBA_WEAK_IRQ_HANDLER
    #define UMBA_WEAK_IRQ_HANDLER( funcName )             UMBA_DECLARE_WEAK_IRQ_HANDLER(funcName)
#endif



