#pragma once

namespace umba
{
namespace parse_utils
{


inline
bool isAlphaLower( char ch )
{
    if (ch>='a' && ch<='z')
        return true;
    return false;
}

inline
bool isAlphaLower( uint8_t ch )
{
    return isAlphaLower( (char)ch );
}


inline
bool isAlphaUpper( char ch )
{
    if (ch>='A' && ch<='Z')
        return true;
    return false;
}

inline
bool isAlphaUpper( uint8_t ch )
{
    return isAlphaUpper( (char)ch );
}


inline
bool isAlpha( char ch )
{
    return isAlphaLower(ch) || isAlphaUpper(ch);
}

inline
bool isAlpha( uint8_t ch )
{
    return isAlphaLower(ch) || isAlphaUpper(ch);
}

inline
bool isSpace( char ch )
{
    return ch==' ';
}

inline
bool isWhitespace( char ch )
{
    return ch==' ' || ch=='\r' || ch=='\n' || ch=='\t';
}

inline
bool isSpace( uint8_t ch )
{
    return isSpace((char)ch);
}

inline
bool isWhitespace( uint8_t ch )
{
    return isWhitespace((char)ch);
}



inline
int toDigit( char ch )
{
    if (ch>='0' && ch<='9')
        return (int)(ch-'0');
    if (ch>='a' && ch<='z')
        return (int)(ch-'a'+10);
    if (ch>='A' && ch<='Z')
        return (int)(ch-'Z'+10);
    return -1;
}

inline
int toDigit( uint8_t ch )
{
    return toDigit( (char)ch );
}

inline
bool isDigit( char ch, int ss = 10)
{
    int d = toDigit( ch );
    if (d<0 || d>=ss)
        return false;
    return true;
}

inline
bool isDigit( uint8_t ch, int ss = 10)
{
    return isDigit( (char)ch );
}


// Also detect termination zero
inline
size_t skipSpaces( const char* pBuf, size_t bufSize )
{
    for( size_t i=0; i!=bufSize; ++i )
    {
        if ( isSpace(pBuf[i]) )
            continue;
        return i;
    }

    return bufSize;
}

inline
size_t skipSpaces( const uint8_t* pBuf, size_t bufSize )
{
    return skipSpaces( (const char*)pBuf, bufSize );
}

// Also detect termination zero
inline
size_t skipWhitespaces( const char* pBuf, size_t bufSize )
{
    for( size_t i=0; i!=bufSize; ++i )
    {
        if ( isWhitespace(pBuf[i]) )
            continue;
        return i;
    }

    return bufSize;
}

inline
size_t skipWhitespaces( const uint8_t* pBuf, size_t bufSize )
{
    return skipWhitespaces( (const char*)pBuf, bufSize );
}


// Note - *pReadedCount, if taken, will be updated (incremented), not assigned
inline
int64_t toNumberSimple( const char* pBuf, size_t bufSize, int ss, size_t *pReadedCount = 0 )
{
    //size_t  readedCount = 0;
    int64_t res = 0;

    size_t i = 0;
    for( ; i!=bufSize && pBuf[i]; ++i )
    {
        int d = toDigit( pBuf[i] );
        if (d<0 || d>=ss)
            break;
        res *= (int64_t)ss;
        res += (int64_t)d;
    }

    if (pReadedCount)
       *pReadedCount += i;

    return res;
}


// Note - *pReadedCount, if taken, will be updated (incremented), not assigned
inline
int64_t toNumberSimple( const uint8_t* pBuf, size_t bufSize, int ss, size_t *pReadedCount = 0 )
{
    return toNumberSimple( pBuf, bufSize, ss, pReadedCount );
}


inline
int64_t pow10( int powN )
{
    if (powN<0)
        powN = -powN;

    int64_t res = 1;

    for( int i=0; i!=powN; ++i)
    {
        res *= 10;
    }

    return res;
}

inline
int64_t mulPow10( int64_t val, int powN )
{
    return powN < 0 ? val/pow10(powN) : val*pow10(powN);
}

// parse decimal number with prec digits after floating point (and skip all next digits )
// Note - *pReadedCount, if taken, will be updated (incremented), not assigned
// return false if no digits found at all
inline
bool toDecimalFixedPoint( const char* pBuf, size_t bufSize, size_t prec, int64_t *pRes, size_t *pReadedCount = 0 )
{
    size_t orgBufSize = bufSize;
    size_t pos = skipWhitespaces( pBuf, bufSize );
    pBuf    += pos;
    bufSize -= pos;

    if (*pBuf==0 || !bufSize)
        return false;

    bool bNeg = false;

    if (*pBuf=='+' || *pBuf=='-')
    {
        if (*pBuf=='-')
            bNeg = true;

        ++pBuf;
        --bufSize;
    }

    pos = skipWhitespaces( pBuf, bufSize );
    pBuf    += pos;
    bufSize -= pos;

    if (*pBuf==0 || !bufSize)
        return false;

    size_t readedDigits = 0;
    int64_t res = toNumberSimple( pBuf, bufSize, 10, &readedDigits );
    if (!readedDigits && (*pBuf!='.' && *pBuf!=','))
        return false;

    pBuf    += readedDigits;
    bufSize -= readedDigits;

    res = mulPow10( res, (int)prec );

    if (*pBuf==0 || !bufSize || (*pBuf!='.' && *pBuf!=',') )
    {
        if (pRes)
           *pRes = bNeg ? -res : res;
        if (pReadedCount)
           *pReadedCount += orgBufSize - bufSize;
        return true;
    }    

    ++pBuf;
    --bufSize;

    pos = 0;
    int64_t fractional = toNumberSimple( pBuf, bufSize, 10, &pos );
    pBuf    += pos;
    bufSize -= pos;

    readedDigits += pos;
    if (!readedDigits)
    {
        return false;
    }

    int readedPwr10 = (int)pos;
    int takenPwr10  = (int)prec;
    int deltaPwr    = takenPwr10 - readedPwr10;

    fractional = mulPow10( fractional, (int)deltaPwr );
    res += fractional;

    if (pRes)
       *pRes = bNeg ? -res : res;
    if (pReadedCount)
       *pReadedCount += orgBufSize - bufSize;
    return true;
}

inline
bool toDecimalFixedPoint( const uint8_t* pBuf, size_t bufSize, size_t prec, int64_t *pRes, size_t *pReadedCount = 0 )
{
    return toDecimalFixedPoint( (const char*)pBuf, bufSize, prec, pRes, pReadedCount );
}



} // namespace parse_utils
} // namespace umba


