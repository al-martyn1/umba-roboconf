/*! \file
\brief Интерфейс для использования в форматном выводе
*/

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cstring>

#include "umba.h"
#include "interface.h"

#if !defined(UMBA_MCU_USED)
    #include <iostream>
    #include <string>

    #if defined(WIN32) || defined(_WIN32)
        #include "winconhelpers.h"
    #endif

#endif


//-----------------------------------------------------------------------------

#define UMBA_TERM_COLORS_MAKE_COMPOSITE( fgColor, bgColor, fBright, fInvert, fBlink ) \
                  ( ((fgColor) & umba::term::colors::color_mask)                      \
                  | (((bgColor) & umba::term::colors::color_mask)<<8)                 \
                  | ((fBright)?umba::term::colors::bright:umba::term::colors::black)  \
                  | ((fInvert)?umba::term::colors::invert:umba::term::colors::black)  \
                  | ((fBlink)?umba::term::colors::blink:umba::term::colors::black)    \
                  )


// umba::term::colors::

namespace umba
{

// https://ru.wikipedia.org/wiki/%D0%A3%D0%BF%D1%80%D0%B0%D0%B2%D0%BB%D1%8F%D1%8E%D1%89%D0%B8%D0%B5_%D0%BF%D0%BE%D1%81%D0%BB%D0%B5%D0%B4%D0%BE%D0%B2%D0%B0%D1%82%D0%B5%D0%BB%D1%8C%D0%BD%D0%BE%D1%81%D1%82%D0%B8_ANSI

namespace term 
{

enum class ConsoleType
{
    file,
    windows_console,
    ansi_terminal
};


namespace colors 
{

    typedef unsigned SgrColor; // Select Graphic Rendition used for terminals

    // foreground
    static const unsigned black          = 0x00000000;
    static const unsigned red            = 0x00000001;
    static const unsigned green          = 0x00000002;
    static const unsigned yellow         = 0x00000003;
    static const unsigned blue           = 0x00000004;
    static const unsigned magenta        = 0x00000005;
    static const unsigned cyan           = 0x00000006;
    static const unsigned white          = 0x00000007;

    static const unsigned raw_mask       = 0x00000007;
    static const unsigned color_default  = 0x00000008;
    static const unsigned color_mask     = 0x0000000F;

    // background colors can be obtained as FG << 8

    static const unsigned bright         = 0x00010000;
    static const unsigned blink          = 0x00020000;
    static const unsigned invert         = 0x00040000;


    inline
    SgrColor makeComposite( SgrColor fgColor, SgrColor bgColor, bool fBright = false, bool fInvert = false, bool fBlink = false )
    {
        return UMBA_TERM_COLORS_MAKE_COMPOSITE(fgColor, bgColor, fBright, fInvert, fBlink);
        // (fgColor & color_mask) | ((bgColor & color_mask)<<8) | (fBright?bright:black) | (fBlink?blink:black);
    }

    inline
    void splitToOptions( SgrColor clrComposite, SgrColor &clrFg, SgrColor &clrBg, bool &fBright, bool &fInvert, bool &fBlink )
    {
        clrFg = clrComposite & color_mask;
        clrBg = (clrComposite>>8) & color_mask;

        fBright = (clrComposite & bright)  ? true : false;
        fBlink  = (clrComposite & blink )  ? true : false;
        fInvert = (clrComposite & invert ) ? true : false;
    }

} // namespace colors


namespace utils
{

inline
char* formatUnsignedDec( char *pBuf, unsigned u)
{
    char *pBufOrg = pBuf;
    while(u)
    {
        unsigned d = u % 10;
        *pBuf++ = '0' + d;
        u /= 10;
    }

    if (pBufOrg == pBuf)
    {
        *pBuf++ = '0';
        return pBuf;
    }

    //*pBuf = 0;
    //std::strrev(pBufOrg);
    std::reverse(pBufOrg, pBuf);
    return pBuf;
}


}; // namespace utils

// 0 Reset/Normal
// 1 Жирный или увеличить яркость
// 5 Мигание: Медленно
// 7 Отображение: Негатив
// 25 Мигание: выкл.

// 30–37 Установить цвет текста (на переднем плане)
// 40–47 Установить цвет фона

inline
char* formatAnsiTermColor( char* pBuf, colors::SgrColor clr, bool fastBlink = false)
{
    //#define UMBA_TERM_DEBUG_TERM_ESC_CSI

    #if defined(UMBA_TERM_DEBUG_TERM_ESC_CSI)
    *pBuf++ = '\\';
    *pBuf++ = 'e';
    #else
    *pBuf++ = '\x1B';
    #endif
    *pBuf++ = '[';
    *pBuf++ = '0'; // reset/default colors  - 3 bytes written

    colors::SgrColor clrFg = 0;
    colors::SgrColor clrBg = 0;
    bool fBright   = false;
    bool fInvert   = false;
    bool fBlink    = false;

    colors::splitToOptions( clr, clrFg, clrBg, fBright, fInvert, fBlink );

    if ((clrFg&colors::color_default)==0) // +3 bytes
    {
        *pBuf++ = ';';
        pBuf = utils::formatUnsignedDec( pBuf, 30+clrFg);
    }
    if ((clrBg&colors::color_default)==0) // +3 bytes
    {
        *pBuf++ = ';';
        pBuf = utils::formatUnsignedDec( pBuf, 40+clrBg);
    }

    if (fBright) // +2 bytes
    {
        *pBuf++ = ';';
        *pBuf++ = '1';
    }

    if (fInvert) // +2 bytes
    {
        *pBuf++ = ';';
        *pBuf++ = '7';
    }

    if (fBlink) // +2 bytes
    {
        *pBuf++ = ';';
        if (fastBlink)
            *pBuf++ = '6';
        else
            *pBuf++ = '5';
    }

    *pBuf++ = 'm'; // +1
    #if defined(UMBA_TERM_DEBUG_TERM_ESC_CSI)
    *pBuf++ = ' '; // +1
    #endif
    *pBuf++ = 0;

    return pBuf; // max - 16 bytes len
}


#if defined(WIN32) || defined(_WIN32)

namespace win32
{

   inline
   WORD getConsoleTextAttribute( HANDLE hCon )
   {
       CONSOLE_SCREEN_BUFFER_INFO csb;
       ::GetConsoleScreenBufferInfo(hCon,&csb);
       return csb.wAttributes;
   }

   inline
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
   {
       CONSOLE_SCREEN_BUFFER_INFO csb;
       ::GetConsoleScreenBufferInfo(hCon,&csb);
       ::SetConsoleTextAttribute( hCon, newAttr );
       return csb.wAttributes;
   }

   inline
   colors::SgrColor convertColorFromConsoleHelper( WORD conColor )
   {
       colors::SgrColor clr = 0;

       if (conColor&FOREGROUND_INTENSITY)
       {
           conColor &= ~FOREGROUND_INTENSITY;
           clr = colors::bright;
       }

       switch(conColor)
       {
           case 0                                   : clr |= colors::black;
           case FOREGROUND_BLUE                     : clr |= colors::blue;
           case FOREGROUND_GREEN                    : clr |= colors::green;
           case FOREGROUND_RED                      : clr |= colors::red;
           case (FOREGROUND_GREEN|FOREGROUND_RED  ) : clr |= colors::yellow;
           case (FOREGROUND_BLUE |FOREGROUND_RED  ) : clr |= colors::magenta;
           case (FOREGROUND_GREEN|FOREGROUND_BLUE ) : clr |= colors::cyan;
           case (FOREGROUND_RED  |FOREGROUND_GREEN|FOREGROUND_BLUE) : clr |= colors::white;
           default: clr |= colors::black;
       }

       return clr;
   }
       
   inline
   colors::SgrColor convertColorFromConsole( WORD conColor )
   {
       return convertColorFromConsoleHelper( conColor&0x0F )
            | convertColorFromConsoleHelper( (conColor>>4)&0x0F );
   }

   inline
   WORD convertColorToConsoleHelper( colors::SgrColor clr, WORD defAttr )
   {

       switch(clr&colors::color_mask)
          {
           case colors::red    : return FOREGROUND_RED;
           case colors::green  : return FOREGROUND_GREEN;
           case colors::yellow : return FOREGROUND_GREEN|FOREGROUND_RED;
           case colors::blue   : return FOREGROUND_BLUE;
           case colors::magenta: return FOREGROUND_RED|FOREGROUND_BLUE;
           case colors::cyan   : return FOREGROUND_GREEN|FOREGROUND_BLUE;
           case colors::white  : return FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
           case colors::color_default: return defAttr & 0x0F;
           default: return 0;
          }
   }

   inline
   WORD convertColorToConsole( colors::SgrColor clr, WORD defAttr )
   {
       colors::SgrColor fgClr = clr      & colors::color_mask;
       colors::SgrColor bgClr = (clr>>8) & colors::color_mask;

       WORD fg = convertColorToConsoleHelper(fgClr, defAttr);
       if (fgClr!=colors::color_default)
       {
           if (clr&colors::bright)
               fg |= FOREGROUND_INTENSITY;
       }

       WORD bg = convertColorToConsoleHelper(bgClr, defAttr>>4);
       if (bgClr!=colors::color_default)
       {
           if (clr&colors::bright)
               bg |= FOREGROUND_INTENSITY;
       }

       if (clr&colors::invert)
           std::swap(fg,bg);

       return fg | (bg<<4); //  | bright;
   }

/*
   WORD getConsoleTextAttribute( HANDLE hCon )
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
   colors::SgrColor convertColorFromConsole( WORD conColor )
   WORD convertColorToConsole( colors::SgrColor clr )
*/


} // namespace win32    

#endif


} // namespace term




//-----------------------------------------------------------------------------
interface ICharWriter
{

    virtual
    void writeChar( char ch ) = 0;

    virtual
    void writeBuf( const uint8_t* pBuf, size_t len )
    {
        for(size_t i = 0; i!=len; ++i, ++pBuf)
           writeChar(*pBuf);
    }

    virtual
    void writeString( const char* str )
    {
        for(; *str; ++str)
           writeChar(*str);
    }

    virtual
    void flush() = 0;

    virtual
    void waitFlushDone()
    {}


    virtual
    void putDefEndl() // end line - LF or CR LF
    {
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
        writeString( "\r\n" ); // Под Windows или при использовании MCU манипулятор новой строки использует последовательность "\r\n"
        #else
        writeChar( '\n' ); // Под Linux принято для перевода строки использовать только \n
        #endif
    }

    virtual
    void putEndl() // end line - LF or CR LF
    {
        //if (isTextMode())
        //   putDefEndl();
        //   writeChar( '\n' );
        //else
        writeChar( '\n' );   
    }

    virtual
    void putCR() // carriage return
    {
        //writeChar( '\r' );
    }

    virtual
    void putFF() // form feed
    {
        //writeChar( '\f' );
    }

    virtual
    bool isTextMode()
    {
        return false;
    }

    virtual
    bool isTerminal() 
    {
        return false;
    }

    virtual
    bool isAnsiTerminal() 
    {
        return false;
    }

    // Возвращает максимальное число байт, которое может быть записано без блокировки
    virtual
    size_t getNonBlockMax()
    {
        return 1024*1024;
    }

    virtual void setTermColors(term::colors::SgrColor clr) {}
    virtual void setDefaultTermColors() {}

    void setAnsiTermColorsImpl(term::colors::SgrColor clr, bool fFastBlink = false)
    {
        char buf[20];
        term::formatAnsiTermColor( buf, clr, fFastBlink );
        writeString( buf );
    }

    void setAnsiTermDefaultColorsImpl()
    {
        setAnsiTermColorsImpl( term::colors::makeComposite( term::colors::white, term::colors::black,false, false, false ), false );
    }



}; // interface ICharWriter


interface NulCharWriter : public ICharWriter
{
    virtual
    void writeChar( char ch ) override
    {
    }

    virtual
    void flush() override {}

};


//-----------------------------------------------------------------------------
template< bool ZeroTerm = true>
class MemCharWriter : public ICharWriter
{

public:

    MemCharWriter(void* pMemBuf, size_t bufSize) : m_pMemBuf((uint8_t*)pMemBuf), m_pMemBufCur((uint8_t*)pMemBuf), m_pMemEnd( ((uint8_t*)pMemBuf)+bufSize-1 ) {}

    virtual
    void writeChar( char ch ) override
    {
        if (ZeroTerm)
        {
            if (m_pMemBufCur==(m_pMemEnd-1))
                return;
            *m_pMemBufCur++ = ch;
            *m_pMemBufCur = 0; // keep string zero terminated
        }
        else // !ZeroTerm
        {
            if (m_pMemBufCur==m_pMemEnd)
                return;
            *m_pMemBufCur++ = ch;
        }
    }

    virtual
    void flush() override {}

    const char* c_str() const
    {
        return (const char*)m_pMemBuf;
    }

    const uint8_t* data() const
    {
        return (const uint8_t*)m_pMemBuf;
    }

    size_t size() const
    {
        return m_pMemBufCur - m_pMemBuf;
    }

    bool empty() const
    {
        return m_pMemBufCur == m_pMemBuf;
    }

protected:

    uint8_t *m_pMemBuf;
    uint8_t *m_pMemBufCur;
    uint8_t *m_pMemEnd;

}; // class MemCharWriter




//-----------------------------------------------------------------------------
template<size_t bufSize>
class SelfBufCharWriter : public MemCharWriter<true>
{

public:

    SelfBufCharWriter() : MemCharWriter( (void*)m_buf, bufSize ) {}

protected:
    
    uint8_t m_buf[bufSize];
}; // class SelfBufWriter







//-----------------------------------------------------------------------------
#if !defined(UMBA_MCU_USED)



//-----------------------------------------------------------------------------
class StringRefCharWriter : public ICharWriter
{

public:

    StringRefCharWriter(std::string &buf) : m_buf(buf) {}

    //-------

    virtual
    void writeChar( char ch ) override
    {
        m_buf.append(1,ch);
    }

    virtual
    void writeBuf( const uint8_t* pBuf, size_t len ) override
    {
        m_buf.append( (const std::string::value_type*)pBuf, len );
    }

    virtual
    void writeString( const char* str ) override
    {
        m_buf.append( (const std::string::value_type*)str );
    }

    virtual
    void flush() override {}

    //-------

    const std::string& str() const
    {
        return m_buf;
    }

    const std::string::value_type* c_str() const
    {
        return m_buf.c_str();
    }

    const std::string::value_type* data() const
    {
        return m_buf.data();
    }

    std::string::size_type size() const
    {
        return m_buf.size();
    }

    bool empty() const
    {
        return m_buf.empty();
    }

protected:

    std::string &m_buf;

}; // class StringRefCharWriter




//-----------------------------------------------------------------------------
class StringCharWriter : public ICharWriter
{

public:

    StringCharWriter() : m_buf() {}

    //-------

    virtual
    void writeChar( char ch ) override
    {
        m_buf.append(1,ch);
    }

    virtual
    void writeBuf( const uint8_t* pBuf, size_t len ) override
    {
        m_buf.append( (const std::string::value_type*)pBuf, len );
    }

    virtual
    void writeString( const char* str ) override
    {
        m_buf.append( (const std::string::value_type*)str );
    }

    virtual
    void flush() override {}

    //-------

    const std::string& str() const
    {
        return m_buf;
    }

    const std::string::value_type* c_str() const
    {
        return m_buf.c_str();
    }

    const std::string::value_type* data() const
    {
        return m_buf.data();
    }

    std::string::size_type size() const
    {
        return m_buf.size();
    }

    bool empty() const
    {
        return m_buf.empty();
    }

protected:

    std::string m_buf;

}; // class StringCharWriter





//-----------------------------------------------------------------------------
/*
   WORD getConsoleTextAttribute( HANDLE hCon )
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
   colors::SgrColor convertColorFromConsole( WORD conColor )
   WORD convertColorToConsole( colors::SgrColor clr )

STD_INPUT_HANDLE (DWORD)-10	
STD_OUTPUT_HANDLE (DWORD)-11	
STD_ERROR_HANDLE (DWORD)-12
HANDLE WINAPI GetStdHandle(
  _In_ DWORD nStdHandle
);
*/

class StdStreamCharWriter : public ICharWriter
{

//public:
/*
    // https://stackoverflow.com/questions/18081392/discrimination-between-file-and-console-streams
    bool isAttachedToConsole() const
    {
    }
*/

    void detectConsoleTypeHelper()
    {
        #if defined(WIN32) || defined(_WIN32)
        m_hCon = INVALID_HANDLE_VALUE;
        #endif

        switch(m_consoleType)
        {
            case term::ConsoleType::file:
                 break;

            case term::ConsoleType::windows_console:
                 #if defined(WIN32) || defined(_WIN32)
                 if (&m_stream==&std::cout || &m_stream==&std::cerr)
                 {
                     m_hCon = ::GetStdHandle( &m_stream==&std::cout ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
                 }
                 #endif
                 break;

            case term::ConsoleType::ansi_terminal:
                 break;
        }
    }



public:

    StdStreamCharWriter(std::ostream &s)
    : m_stream(s)
    , m_consoleType(term::ConsoleType::file)
    #if defined(WIN32) || defined(_WIN32)
    , m_hCon(INVALID_HANDLE_VALUE)
    , m_defColor(0)
    #endif
    {
        #if defined(WIN32) || defined(_WIN32)

            if (&m_stream==&std::cout || &m_stream==&std::cerr)
            {
                winhelpers::ConsoleType ct = winhelpers::detectConsoleType( &m_stream==&std::cout ? _fileno(stdout) : _fileno(stderr) );
                switch(ct)
                {
                    case winhelpers::ConsoleType::file           : m_consoleType = term::ConsoleType::file; break;
                    case winhelpers::ConsoleType::windows_console: m_consoleType = term::ConsoleType::windows_console;break;
                    case winhelpers::ConsoleType::ansi_terminal  : m_consoleType = term::ConsoleType::ansi_terminal; break;
                }
                detectConsoleTypeHelper();
                if (m_hCon != INVALID_HANDLE_VALUE)
                    m_defColor = term::win32::getConsoleTextAttribute( m_hCon );
            }

        #else

            //return m_stream.tellp() == std::ostream::pos_type(-1);

            // Under Linux we assume that all terminals are ANSI
            m_consoleType = (m_stream.tellp() == std::ostream::pos_type(-1))
                          ? term::ConsoleType::ansi_terminal
                          : term::ConsoleType::file
                          ;

        #endif
    }

    void forceSetConsoleType(term::ConsoleType consoleType)
    {
        bool bChanged = m_consoleType != consoleType;
        m_consoleType = consoleType;
        detectConsoleTypeHelper();
        if (bChanged && m_hCon != INVALID_HANDLE_VALUE)
            m_defColor = term::win32::getConsoleTextAttribute( m_hCon );
    }
    

    virtual
    void writeChar( char ch ) override
    {
        //if (ch=='\n' && !isAttachedToConsole())
        //    m_stream.put('\r'); // expand single \n to \r\n sequence when stream is regular file
        m_stream.put(ch);
        char tmp[] = { ch, 0 };

        #if defined(WIN32) || defined(_WIN32)
        if (&m_stream==&std::cout || &m_stream==&std::cerr)
        {
            OutputDebugStringA( tmp );
        }
        #endif
    }
/*
    void writeBuf( const uint8_t* pBuf, size_t len ) override
    {
        m_stream.write( (const std::ostream::char_type*)pBuf, len );
    }
*/

    virtual
    void flush() override
    {
        m_stream.flush();
    }

    virtual
    bool isTextMode() override
    {
        /*
        //m_stream.openmode & 
        //std::ios_base::openmode
        if (isAttachedToConsole())
            return true;
        return false;
        */
        #if defined(WIN32) || defined(_WIN32)
            if (m_consoleType == term::ConsoleType::file)
                return false;
            return true;
        #else
            return false;
        #endif
    }

    virtual
    bool isTerminal() override
    {
        return m_consoleType != term::ConsoleType::file;

        //if (isAttachedToConsole())
        //    return true;
        return false;
    }

    virtual
    bool isAnsiTerminal() override
    {
        return m_consoleType != term::ConsoleType::file;
        //return false; //UNDONE: detect parent process is ansi terminal compatible
    }

    virtual
    void putEndl() // end line - LF or CR LF
    {
        if (isTextMode())
           writeChar( '\n' );
        else
           putDefEndl();
    }

    virtual
    void putCR() override // carriage return
    {
        if (isTerminal())
            writeChar( '\r' );
    }

    virtual
    void putFF() override // form feed
    {
        if (isTerminal())
            writeChar( '\f' );
    }

    virtual void setTermColors(term::colors::SgrColor clr) override
    {
        if (m_consoleType==term::ConsoleType::ansi_terminal)
        {
            setAnsiTermColorsImpl( clr );
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::ConsoleType::windows_console)
        {
            if (m_hCon!=INVALID_HANDLE_VALUE)
            {
                term::win32::setConsoleTextAttribute( m_hCon, term::win32::convertColorToConsole(clr, m_defColor) );
            }
        }
        #endif
    }

    virtual void setDefaultTermColors() override
    {
        if (m_consoleType==term::ConsoleType::ansi_terminal)
        {
            setAnsiTermDefaultColorsImpl( );
        }
        #if defined(WIN32) || defined(_WIN32)
        else if (m_consoleType==term::ConsoleType::windows_console)
        {
            if (m_hCon!=INVALID_HANDLE_VALUE)
            {
                if (!m_defColor)
                    term::win32::setConsoleTextAttribute( m_hCon, FOREGROUND_RED  |FOREGROUND_GREEN|FOREGROUND_BLUE );
                else
                    term::win32::setConsoleTextAttribute( m_hCon, m_defColor );
            }
        }
        #endif
    }


/*
   WORD getConsoleTextAttribute( HANDLE hCon )
   WORD setConsoleTextAttribute( HANDLE hCon, WORD newAttr )
m_defColor
   colors::SgrColor convertColorFromConsole( WORD conColor )
   WORD convertColorToConsole( colors::SgrColor clr )
*/

protected:

    std::ostream      &m_stream;
    term::ConsoleType  m_consoleType;
    #if defined(WIN32) || defined(_WIN32)
    HANDLE             m_hCon;
    WORD               m_defColor;
    #endif

}; // class StdStreamCharWriter

//-----------------------------------------------------------------------------

#else /* UMBA_MCU_USED defined */


} // namespace umba

#if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

    #include <stdio.h>
    #include <rt_sys.h>
    #include <rt_misc.h>

#endif

/*

    #if ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )

        asm(".global __use_no_semihosting_swi\n");
        
    #elif __CC_ARM
    
        #pragma import(__use_no_semihosting_swi)
        
    #endif

    extern "C"
    { 
        int fputc(int c, FILE *f)
        {
            return ITM_SendChar(c);
        }

        int fgetc(FILE *f)
        {
            char ch = 0;

            return((int)ch);
        }

        int ferror(FILE *f)
        {
            return EOF;
        }

        void _ttywrch(int ch)
        {
            ITM_SendChar(ch);            
        }
        
        char *_sys_command_string(char *cmd, int len)
        {
            return NULL;
        }
        
        // вызывается после main
        void _sys_exit(int return_code) 
        {
            while(1);
        }        
    }
*/

namespace umba
{


// Serial Wire Viewer - SWV
// http://blog.atollic.com/cortex-m-debugging-introduction-to-serial-wire-viewer-swv-event-and-data-tracing
// http://blog.atollic.com/cortex-m-debugging-printf-redirection-to-a-debugger-console-using-swv/itm-part-1
// http://blog.atollic.com/cortex-m-debugging-printf-redirection-to-a-debugger-console-using-swv/itm-part-2
// http://www.keil.com/appnotes/files/apnt_297_v102.pdf
class SwvCharWritter : implements ICharWriter
{

    virtual
    void writeChar( char ch ) override
    {
#if __CC_ARM || ( (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) )
        ITM_SendChar(ch);
#endif
    }

    virtual
    void putEndl()
    {
        writeString( "\r\n" );
    }

    virtual
    void flush() override
    {}



}; // interface SwdCharWritter





#ifdef LUART_UART_HANDLE_H
// Заголовок legacy uart подключен перед использованием райтера, 
// поэтому мы можем сделать реализацию райтера, пихающую всё в uart

} // namespace umba

// На самом деле нам нужен <algorithm>, но он подключается в umba/stl.h
#include "umba/stl.h"

// std::strlen
#include <cstring>

#include "critical_section.h"

// UMBA_IS_IN_ISR()

#define UMBA_LEGACY_UART_CHART_WRITTER_LOCK()    UMBA_CRITICAL_SECTION(globalCriticalSection)

namespace umba
{

template< size_t BufSize=512 >
class LegacyUartCharWriter : public ICharWriter
{

public:

    LegacyUartCharWriter( uart::Handle &uart )
    : m_uart(uart)
    , m_endIdx(0)
    , m_textMode(false)
    , m_term(false)
    , m_ansiTerm(false)
    , m_fastBlink(false)
    {
    }

    virtual
    void writeChar( char ch ) override
    {
        /*
        if (m_textMode && ch=='\n')
        {
            writeChar( '\r' );
        }

        blockingWrite((const uint8_t *)(&ch), sizeof(ch));
        */

        writeBuf( (const uint8_t*)&ch, 1 );
    }

    virtual
    void writeString( const char* str ) override
    {
        writeBuf( (const uint8_t*)str, std::strlen(str) );
    }

    virtual
    void writeBuf( const uint8_t* pBuf, size_t len ) override
    {
        //пытаемся сбросить свой буфер в UART
        trySendOwnBuf();

        // А ничего отсылать и не надо
        if (!pBuf || !len)
            return;

        /*
        // Если в ничего нет в буфере, режим двоичный
        // записываем прямо тот буфер, который нам передан
	    // остаток не ждем и кладем в свой буфер
        if (!m_endIdx && !m_textMode)
        {
            // отсылаем данные из переданного буфера до тех пор, пока остаток не можем положить в свой буфер
            while( len > BufSize);
            {
                size_t bytesWritten = writeToUartNonBlock( pBuf, len );
                len  -= bytesWritten;
                pBuf += bytesWritten;
                // Блокировки объекта не нужно, так как мы не меняем содержимое своего буфера

                if (isIrqProcessed())
                {
                    // Так как мы в обработчике прерывания, то мы можем сделать только одну отсылку
                    // и остаток просто отбросим
                    if (len > BufSize)
                        len = BufSize;
                    break;
                }
            } 
            
            if (len) // У нас осталось только такой кусок, который гарантированно влезает в буфер
            {
                // кладем остаток себе в буфер
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHART_WRITTER_LOCK();
                std::memcpy( m_buf, pBuf, len );
                m_endIdx = len;
            }

            return;
        }
        */

        if (isIrqProcessed())
        {
            size_t bufAvailSize = 0;

            {
                UMBA_LEGACY_UART_CHART_WRITTER_LOCK();
                bufAvailSize = BufSize - m_endIdx;
                if (len>bufAvailSize)
                    len = bufAvailSize;
            }

            if (m_textMode)
            {
                size_t requiredLen = 0;
                size_t i = 0;
                for( ; i!=len; ++i)
                {
                    size_t nextRequiredLen = requiredLen + getTextModeByteRequiredBufSpace( pBuf[i] );
                    if (nextRequiredLen>bufAvailSize)
                    {
                        break;
                    }

                    requiredLen = nextRequiredLen;
                }

                len = i;
            }
        }

        //---
        //size_t getUartMaxLen() const
        tampBuffer(pBuf, len);
        //--
    }

    virtual
    void flush() override
    {
        if (isIrqProcessed())
            return; // single time try from IRQ handlers

        size_t rest = trySendOwnBuf();

        while(rest)
        {
            rest = trySendOwnBuf();
        }
    }

    virtual
    void waitFlushDone()
    {
        while (!m_uart.isTransmitComplete()) {}
    }

    virtual
    bool isTextMode() override
    {
        return m_textMode;
    }

    virtual
    bool isTerminal() override
    {
        //if (!isTextMode())
        //    return false;

        return m_term; // 
    }

    virtual
    bool isAnsiTerminal() override
    {
        if (!isTerminal())
            return false;
        return m_ansiTerm;
    }

    virtual
    void putCR() override // carriage return
    {
        if (isTerminal())
            writeChar( '\r' );
        else
            putEndl();

        flush();
    }

    virtual
    void putFF() override // form feed
    {
        if (isTerminal())
        {
            writeString( "\n----------------\n" ); // Frame separator - для удобства разделения фреймов в логе
            writeChar( '\f' );
        }
        else
            putEndl();

        flush();
    }

    virtual void setTermColors(term::colors::SgrColor clr) override
    {
        if (isAnsiTerminal())
            setAnsiTermColorsImpl(clr, m_fastBlink);
    }

    LegacyUartCharWriter& setTextMode( bool ftm = true )
    {
        m_textMode = ftm;
        return *this;
    }

    LegacyUartCharWriter& setTerminalMode( bool fTerm = true )
    {
        m_term = fTerm;
        return *this;
    }

    LegacyUartCharWriter& setAnsiTerminalMode( bool fTerm = true )
    {
        if (fTerm)
            m_term = true;
        m_ansiTerm = fTerm;
        return *this;
    }

    LegacyUartCharWriter& setAtFastBlink( bool fFast = true )
    {
        m_fastBlink = fFast;
        return *this;
    }

    virtual
    size_t getNonBlockMax()
    {
        return BufSize - m_endIdx;  
    }


protected:

    uart::Handle     &m_uart;
    uint8_t          m_buf[BufSize];
    volatile size_t  m_endIdx;
    bool             m_textMode;
    bool             m_term;
    bool             m_ansiTerm;
    bool             m_fastBlink;

    size_t getTextModeByteRequiredBufSpace( uint8_t b )
    {
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
        if (b=='\n')
            return 2;
        #endif
        return 1;
    }

    bool putLinefeedToBufInTextMode()
    {
        #if defined(UMBA_MCU_USED) || defined(_WIN32)
            if ( m_endIdx>=(BufSize-1) )
                return false;
            m_buf[m_endIdx++] = '\r';
            m_buf[m_endIdx++] = '\n';
        #else
            m_buf[m_endIdx++] = '\n';
        #endif

        return true;
    }


    void tampBuffer(const uint8_t* &pBuf, size_t &len)
    {
        while(len)
        {
            // пытаемся отправить кусочек буфера
            trySendOwnBuf();

            // пытаемся засунуть хоть что-нибудь в буфер
            if (!m_textMode)
            {
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHART_WRITTER_LOCK();
                
                // в двоичном режиме тупо пересылаем массивами
                size_t bufAvailSize = BufSize - m_endIdx;
                size_t numBytesToCopy = bufAvailSize > len ? len : bufAvailSize;

                if (numBytesToCopy)
                {
                    std::memcpy( &m_buf[m_endIdx], pBuf, numBytesToCopy );
                    len      -= numBytesToCopy;
                    pBuf     += numBytesToCopy;
                    m_endIdx += numBytesToCopy;
                }
            }
            else
            {
                // Защищаем момент изменения буфера и индекса
                UMBA_LEGACY_UART_CHART_WRITTER_LOCK();
                while( m_endIdx!=BufSize && len )
                {
                    if (*pBuf=='\n')
                    {
                        if (!putLinefeedToBufInTextMode())
                            break; // нет места для двух символов
                        pBuf++;
                        len--;
                    }
                    else
                    {
                        m_buf[m_endIdx++] = *pBuf;
                        pBuf++;
                        len--;
                    }

                } // while( m_endIdx!=BufSize && len )
            }

            if (isIrqProcessed())
               break;


        } // while(len)
    }

    size_t getUartMaxLen() const
    {
        return UART_UART_TX_BUF_SIZE;
    }

    void blockingWriteToUart( const uint8_t* pBuf, size_t len )
    {
        while(!m_uart.sendLocalArray( (const uint8_t *)pBuf, len ) )
        {}
    }

    bool isIrqProcessed() const
    {
        if (UMBA_IS_IN_ISR()) 
            return true;
        return false;
    }

    size_t trySendOwnBuf()
    {
        UMBA_LEGACY_UART_CHART_WRITTER_LOCK();

        if (!m_endIdx)
            return m_endIdx;

        size_t bytesWritten = writeToUartNonBlock( m_buf, m_endIdx );
        if (bytesWritten)
        {
            size_t restSize = m_endIdx - bytesWritten;
            if (restSize)
                std::memmove( m_buf, &m_buf[bytesWritten], restSize );
            m_endIdx = restSize;
        }

        return m_endIdx;
    }


    // возврат - сколько байт записано
    size_t writeToUartNonBlock( const uint8_t* pBuf, size_t len )
    {
        if (len > getUartMaxLen())
            len = getUartMaxLen();

        //UMBA_LEGACY_UART_CHART_WRITTER_LOCK();
        if (!m_uart.isTransmitComplete())
            return 0;

        if (!m_uart.sendLocalArray( pBuf, len ) )
            return 0;

        return len;
    }



}; // class LegacyUartCharWriter


#endif

#endif /* #if !defined(UMBA_MCU_USED) */




}; // namespace umba

