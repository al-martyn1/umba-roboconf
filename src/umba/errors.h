/*! \file
\brief Ошибки, и связанные с ними определения
*/

#pragma once

#include "umba/preprocessor.h"
#include "umba/stl.h"

#if defined(UMBA_WIN32_USED)
    #include <stdio.h>
#endif

namespace umba{

typedef uint32_t  ErrorCode;


namespace errors
{
    const ErrorCode errorFlag              = 0x80000000;
    const ErrorCode invertFlag             = 0x40000000;

    const ErrorCode errorSubsystemMask     = 0x0F000000;

    const ErrorCode errorSubsystemUmba     = 0x00000000;
    const ErrorCode errorSubsystemWindows  = 0x01000000;
    const ErrorCode errorSubsystemPosix    = 0x02000000;
    const ErrorCode errorSubsystemQt       = 0x03000000;


    inline ErrorCode makeFromWindows( ErrorCode errorCode )
    {
     return errorFlag | errorSubsystemWindows | (errorCode & ~(errorSubsystemMask | errorFlag));
    }

    inline ErrorCode makeFromPosix( ErrorCode errorCode )
    {
     return errorFlag | errorSubsystemPosix | (errorCode & ~(errorSubsystemMask | errorFlag));
    }

    inline ErrorCode makeFromQt( ErrorCode errorCode )
    {
     return errorFlag | errorSubsystemQt | (errorCode & ~(errorSubsystemMask | errorFlag));
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    #ifdef UMBA_DECLARE_ERROR_CODE
        #undef UMBA_DECLARE_ERROR_CODE
    #endif

    #ifdef UMBA_DECLARE_ERROR_CODE_ALIAS
        #undef UMBA_DECLARE_ERROR_CODE_ALIAS
    #endif

    /* info */
    #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                 const ErrorCode name = code; 
    #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info) \
                 const ErrorCode name = code; 

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    #include "x_error_codes.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    #undef UMBA_DECLARE_ERROR_CODE
    #undef UMBA_DECLARE_ERROR_CODE_ALIAS

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


} // namespace errors



template <typename TValue>
struct Result;





//! Плейс-холдер для кода ошибки
class Error
{

public:

    mutable ErrorCode    result;

    Error()               : result(errors::ok) {}
    Error(ErrorCode ec)   : result(ec) {}
    Error(const Error &e) : result(e.result) {}

    Error& operator=(const ErrorCode &e) { result = e;        return *this; }
    Error& operator=(const Error &e)     { result = e.result; return *this; }

    template <typename TValue>
    Error( const Result<TValue> &r );

    //! Проверка на сбой
    bool isFail() const
    {
        return (result & errors::errorFlag) ? true : false;
    }

    //! Проверка на успех
    bool isSuccess() const
    {
        return !isFail();
    }

    //! Проверка на полный успех
    bool isClean() const
    {
        return result == 0;
    }

    //! Использование в операторе if и сравнение с bool
    /*! Проверка на успешность вызова:
        if (auto a = someFunction()) { Ветка по успеху }
     */
    explicit operator bool() const
    {
        if ((result&errors::invertFlag)==0) // invert flag not set
            return isSuccess();

        result &= ~ errors::invertFlag;
        return isFail();
    }

    //! Использование в операторе if с инверсией
    /*! Проверка на фейл вызова:
        if (auto a = !someFunction()) { Ветка по неудаче }
     */
    Error operator!() const
    {
        return Error(result | errors::invertFlag);
    }

    #ifdef UMBA_MCU_USED

      #ifndef DOXYGEN_SHOULD_SKIP_THIS

        // errors:: 
        #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                     case name : return UMBA_STRINGIFY(name);
        #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info)

      #endif /* DOXYGEN_SHOULD_SKIP_THIS */


        //! Возвращает строку с сообщением об ошибке
        const char* what() const
        {
            switch(result & ~errors::invertFlag)
            {
                using namespace errors;
                #include "x_error_codes.h"
                #undef UMBA_DECLARE_ERROR_CODE
                #undef UMBA_DECLARE_ERROR_CODE_ALIAS
                default:
                {
                     return "Unknown error";
                }
            };
        }

        //! Возвращает Unicode строку сообщением об ошибке
        const wchar_t* whatWide() const
        {
            return L"Some error";
        }

    #else

      #ifndef DOXYGEN_SHOULD_SKIP_THIS

        // errors:: 
        #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                     case name : return info;
        #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info)

      #endif /* DOXYGEN_SHOULD_SKIP_THIS */

        //! Возвращает строку с сообщением об ошибке
        const char* what() const
        {
            switch(result & ~errors::invertFlag)
            {
                using namespace errors;
                #include "x_error_codes.h"
                #undef UMBA_DECLARE_ERROR_CODE
                #undef UMBA_DECLARE_ERROR_CODE_ALIAS
                default:
                {
                    #if defined(_WIN32) || defined(WIN32)
                        ErrorCode cleanCode = result & ~(errorSubsystemMask|errorFlag|invertFlag);
                        const char* fmt = "Unknown error %d (0x%08X)";
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     fmt = "Umba error %d (0x%08X)";    break;
                            case errorSubsystemWindows:  fmt = "Windows error %d (0x%08X)"; break;
                            case errorSubsystemPosix:    fmt = "Posix error %d (0x%08X)";   break;
                            case errorSubsystemQt:       fmt = "Qt error %d (0x%08X)";      break;
                            //default:                     return "Unknown error";
                        };
                        thread_local static char buf[64];
                        sprintf(buf, fmt, cleanCode, result);
                        return buf;
                    #else
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     return "Umba error";
                            case errorSubsystemWindows:  return "Windows error";
                            case errorSubsystemPosix:    return "Posix error";
                            case errorSubsystemQt:       return "Qt error";
                            default:                     return "Unknown error";
                        };
                    #endif
                }
            };
        }

      #ifndef DOXYGEN_SHOULD_SKIP_THIS

        //errors::
        #define UMBA_DECLARE_ERROR_CODE(name, code, info) \
                     case  name : return L##info;
        #define UMBA_DECLARE_ERROR_CODE_ALIAS(name, code, info)

      #endif /* DOXYGEN_SHOULD_SKIP_THIS */

        //! Возвращает Unicode строку сообщением об ошибке
        const wchar_t* whatWide() const
        {
            switch(result & ~errors::invertFlag)
            {
                using namespace errors;
                #include "x_error_codes.h"
                #undef UMBA_DECLARE_ERROR_CODE
                #undef UMBA_DECLARE_ERROR_CODE_ALIAS
                default:
                {
                    #if defined(_WIN32) || defined(WIN32)
                        ErrorCode cleanCode = result & ~(errorSubsystemMask|errorFlag|invertFlag);
                        const wchar_t* fmt = L"Unknown error %d (0x%08X)";
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     fmt = L"Umba error %d (0x%08X)";    break;
                            case errorSubsystemWindows:  fmt = L"Windows error %d (0x%08X)"; break;
                            case errorSubsystemPosix:    fmt = L"Posix error %d (0x%08X)";   break;
                            case errorSubsystemQt:       fmt = L"Qt error %d (0x%08X)";      break;
                            //default:                     return "Unknown error";
                        };
                        thread_local static wchar_t buf[64];
                        swprintf(buf, fmt, cleanCode, result);
                        return buf;
                    #else
                        switch(result&errorSubsystemMask)
                        {
                            case errorSubsystemUmba:     return L"Umba error";
                            case errorSubsystemWindows:  return L"Windows error";
                            case errorSubsystemPosix:    return L"Posix error";
                            case errorSubsystemQt:       return L"Qt error";
                            default:                     return L"Unknown error";
                        };
                    #endif
                }
            };
        }

    #endif


}; // struct Error


// Compare with the same
inline
bool operator==(Error e1, Error e2)
{
    return e1.result == e2.result;
}

inline
bool operator!=(Error e1, Error e2)
{
    return e1.result != e2.result;
}

// Compare with ErrorCode
inline
bool operator==(Error e1, ErrorCode e2)
{
    return e1.result == e2;
}

inline
bool operator!=(Error e1, ErrorCode e2)
{
    return e1.result != e2;
}

inline
bool operator==(ErrorCode e1, Error e2)
{
    return e1 == e2.result;
}

inline
bool operator!=(ErrorCode e1, Error e2)
{
    return e1 != e2.result;
}

// Compare with bool
inline
bool operator==(Error e1, bool e2)
{
    return e1.isSuccess() == e2;
}

inline
bool operator!=(Error e1, bool e2)
{
    return e1.isSuccess() != e2;
}

inline
bool operator==(bool e1, Error e2)
{
    return e1 == e2.isSuccess();
}

inline
bool operator!=(bool e1, Error e2)
{
    return e1 != e2.isSuccess();
}





//! Обобщенный результат
/*! Данный класс позволяет возвращать код результата (ошибки)
    одновременно с полезной нагрузкой. Когда полезная нагрузка не нужна,
    следует использовать класс Error

 */
template <typename TValue>
struct Result : public Error
{

public:

    TValue       value;

    Result( )
    : Error()
    , value()
    {}

    Result( const Error &e )
    : Error(e.result)
    , value()
    {}

    Result( ErrorCode ec, const TValue &tv )
    : Error(ec)
    , value(tv)
    {}

    Result( const Result &r )
    : Error(r.result)
    , value(r.value)
    {}

    Result(Result &&r)
    : Error(r.result)
    , value()
    {
        value = std::move(r.value);
    }

    Result( const TValue &tv )
    : Error(0)
    , value(tv)
    {}

/*
    explicit operator Error() const
    {
        return Error(result);
    }
*/

    Result& operator=(const Result &r)
    {
        if (&r==this) return *this;
        result = r.result;
        value  = r.value;
        return *this;
    }

    Result& operator=(Result &&r)
    {
        if (&r==this) return *this;
        result = std::move(r.result);
        value  = std::move(r.value);
        return *this;
    }


    //! Использование в операторе if и сравнение с bool
    /*! Проверка на успешность вызова:
        if (auto a = someFunction()) { Ветка по успеху }
        if (auto a = !someFunction()) { Ветка по неудаче }
     */
    explicit operator bool() const
    {
        if ((result&errors::invertFlag)==0) // invert flag not set
            return isSuccess();

        result &= ~ errors::invertFlag;
        return isFail();
    }

    //! Использование в операторе if с инверсией
    /*! Проверка на фейл вызова:
        if (auto a = !someFunction()) { Ветка по неудаче }
     */
    Result operator!() const
    {
        return Result( result | errors::invertFlag, value );
    }

}; // struct Result




template <typename TValue>
inline
Error::Error( const Result<TValue> &r ) : result(r.result)
{
}




} // namespace umba


