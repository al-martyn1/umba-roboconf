#pragma once

#include "umba/umba.h"
#include "umba/parse_utils.h"

#include <algorithm>

namespace umba
{
namespace format_utils
{

enum class CaseParam
{
    lower,
    upper
};


inline
char digitToChar( unsigned d, CaseParam caseParam = CaseParam::upper )
{
    return (char)( d<10
                 ? '0' + d 
                 : (caseParam==CaseParam::lower ? 'a' : 'A') - 10 + d 
                 );
}


inline
size_t formatNumber( char* pBuf, size_t bufSize, uint64_t val, int ss, CaseParam caseParam = CaseParam::upper, size_t fmtWidth = 0, char fillChar = ' ' )
{
    UMBA_ASSERT(pBuf);
    UMBA_ASSERT(bufSize>0);

    if (!fmtWidth)
        fmtWidth = 1;

    if (fmtWidth>bufSize)
        fmtWidth = bufSize;

    //char* pBufOrg = pBuf;

    size_t placedDigitsCount = 0;

    while( placedDigitsCount < fmtWidth || val)
    {
        unsigned d = (unsigned)( val % (uint64_t)ss);

        char ch = fillChar;
        if (!val)
        {
            if (!placedDigitsCount)
                ch = '0';
        }
        else
        {
            ch = digitToChar( d, caseParam );
        }

        val /= (uint64_t)ss;

        if (placedDigitsCount<bufSize)
           *pBuf[placedDigitsCount++] = ch;
    }

    std::reverse( &pBuf[0], &pBuf[placedDigitsCount] );

    return placedDigitsCount;
}

inline
size_t formatNumber( uint8_t* pBuf, size_t bufSize, uint64_t val, int ss, CaseParam caseParam = CaseParam::upper, size_t fmtWidth = 0, char fillChar = ' ' )
{
    return formatNumber( (uint8_t*)pBuf, bufSize, val, ss, caseParam, fmtWidth, fillChar );
}

size_t formatNumber( char* pBuf, size_t bufSize, int64_t val, int ss, CaseParam caseParam = CaseParam::upper, size_t fmtWidth = 0, char fillChar = ' ' )
{
    if (ss!=10)
        return formatNumber( pBuf, bufSize, (uint64_t)val, ss, caseParam, fmtWidth, fillChar );

    UMBA_ASSERT(pBuf);

    if (val<0)
    {
        UMBA_ASSERT(bufSize>1);
        *pBuf++ = '-';
        bufSize--;
        return 1 + formatNumber( pBuf, bufSize, (uint64_t)-val, ss, caseParam, fmtWidth ? fmtWidth-1 : (size_t)0, fillChar );
    }
    else
    {
        UMBA_ASSERT(bufSize>0);
        return formatNumber( pBuf, bufSize, (uint64_t)val, ss, caseParam, fmtWidth, fillChar );
    }
}

inline
size_t formatNumber( uint8_t* pBuf, size_t bufSize, int64_t val, int ss, CaseParam caseParam = CaseParam::upper, size_t fmtWidth = 0, char fillChar = ' ' )
{
    return formatNumber( (uint8_t*)pBuf, bufSize, val, ss, caseParam, fmtWidth, fillChar );
}

inline
size_t formatDecimalFixedPoint( char * pBuf, size_t bufSize, int64_t val, size_t valPrec, size_t fmtPrec, size_t fmtWidth = 0, char sepChar = '.' )
{
    val = mulPow10( val, (int)fmtPrec - (int)valPrec );

    int64_t rest = (val<0 ? -val : val ) % pow10( (int)fmtPrec );
    val = mulPow10( val, -(int)fmtPrec );

    size_t intPartWidth = fmtWidth;
    if (intPartWidth > (fmtPrec+1))
        intPartWidth -= fmtPrec+1;

    size_t intPartLen = formatNumber( pBuf, bufSize, val, 10, CaseParam::upper, intPartWidth, fillChar );
    if (intPartLen>=bufSize)
       return intPartLen;

    pBuf[intPartLen++] = sepChar;
    if (intPartLen>=bufSize)
       return intPartLen;

    return intPartLen + formatNumber( &pBuf[intPartLen], bufSize-intPartLen, (uint64_t)rest, 10, CaseParam::upper, fmtPrec, '0' );

}


} // namespace format_utils
} // namespace umba



