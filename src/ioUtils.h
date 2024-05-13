#pragma once

#include "umba/umba.h"
#include "umba/assert.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "encoding.h"

#include "rdlc-core/fsUtils.h"

/*
readFile, readFileSafe - чем отличаются, не понятно. Видимо когда-то отличались
UPD: Переделал - теперь readFile позволяет прочитать пустой файл
*/
//-----------------------------------------------------------------------------
inline
std::string readFile( std::istream &in, bool bSafe )
{
    UMBA_ASSERT(!!in);

    constexpr size_t sz = 16384;
    char buf[sz];
    std::string res;

    while(in.read( &buf[0], sz))
    {
        res.append( buf, sz );
    }

    auto szLast = in.gcount();
    if (szLast)
       res.append( buf, (std::string::size_type)szLast );

    if (res.empty() && bSafe)
        throw std::runtime_error( std::string("failed to read file")  /* + canonFileName */  );

    return res;
}

//-----------------------------------------------------------------------------
inline
std::string readFile( const std::string &fn )
{
    std::string res;
    std::string canonFileName = makeSimpleCanonicalFilename(fn);
    std::ifstream infile( canonFileName.c_str(), std::ios::in|std::ios::binary /* |std::ios::ate */ );

    if (!infile)
        throw std::runtime_error( std::string("failed to open file - ") + canonFileName );

    return readFile( infile, false );
/*
    constexpr size_t sz = 16384;
    char buf[sz];

    while(infile.read( &buf[0], sz))
    {
        res.append( buf, sz );
    }

    auto szLast = infile.gcount();
    if (szLast)
       res.append( buf, (std::string::size_type)szLast );

    //if (res.empty())
    //    throw std::runtime_error( std::string("failed to read file - ") + canonFileName );

    return res;
*/
}

//-----------------------------------------------------------------------------
inline
std::string readFileSafe( const std::string &fn )
{
    std::string res;
    std::string canonFileName = makeSimpleCanonicalFilename(fn);
    std::ifstream infile( canonFileName.c_str(), std::ios::in|std::ios::binary /* |std::ios::ate */ ); // какая свинья в примерах этот флаг использует?

    if (!infile)
        throw std::runtime_error( std::string("failed to open file - ") + canonFileName );

    return readFile( infile, false );
/*
    constexpr size_t sz = 16384;
    char buf[sz];

    while(infile.read( &buf[0], sz))
    {
        res.append( buf, sz );
    }

    auto szLast = infile.gcount();
    if (szLast)
       res.append( buf, (std::string::size_type)szLast );

    if (res.empty())
        throw std::runtime_error( std::string("failed to read file - ") + canonFileName );

    return res;
*/
}

//-----------------------------------------------------------------------------
inline
std::string readFileEncoded( std::istream &in, std::string srcEnc = std::string(), std::string targetEnc = "UTF-8", std::string httpHint = std::string(), std::string metaHint = std::string() )
{
    std::string text = readFile( in, false );

    if (targetEnc.empty())
        targetEnc = "UTF-8";

    EncodingsApi* pEncApi = getEncodingsApi();

    if (srcEnc.empty())
    {
        size_t bomSize = 0;
        srcEnc = pEncApi->detect( text, bomSize, httpHint, metaHint );
    }

    if (srcEnc.empty())
        throw std::runtime_error( std::string("Invalid file encoding") );

    if (pEncApi->isEqualEncodingNames(srcEnc, targetEnc))
        return text; // nothing to convert

    UINT cpSrc = pEncApi->getCodePageByName( srcEnc );
    if (!cpSrc)
        throw std::runtime_error( std::string("Unknown or unsupported file encoding - ") + srcEnc );

    UINT cpDst = pEncApi->getCodePageByName( targetEnc );
    if (!cpDst)
        throw std::runtime_error( std::string("Unknown or unsupported target encoding - ") + targetEnc );

    return pEncApi->convert(text, cpSrc, cpDst );
}

//-----------------------------------------------------------------------------
inline
std::string readFileEncoded( const std::string &fn, std::string srcEnc = std::string(), std::string targetEnc = "UTF-8", std::string httpHint = std::string(), std::string metaHint = std::string() )
{
    std::string res;
    std::string canonFileName = makeSimpleCanonicalFilename(fn);
    std::ifstream infile( canonFileName.c_str(), std::ios::in|std::ios::binary /* |std::ios::ate */ );

    if (!infile)
        throw std::runtime_error( std::string("failed to open file - ") + canonFileName );

    return readFileEncoded( infile, srcEnc, targetEnc, httpHint, metaHint );

/*
    std::string text = readFile( fn );

    if (targetEnc.empty())
        targetEnc = "UTF-8";

    EncodingsApi* pEncApi = getEncodingsApi();

    if (srcEnc.empty())
    {
        size_t bomSize = 0;
        srcEnc = pEncApi->detect( text, bomSize, httpHint, metaHint );
    }

    if (srcEnc.empty())
        throw std::runtime_error( std::string("Invalid file encoding") );

    if (pEncApi->isEqualEncodingNames(srcEnc, targetEnc))
        return text; // nothing to convert

    UINT cpSrc = pEncApi->getCodePageByName( srcEnc );
    if (!cpSrc)
        throw std::runtime_error( std::string("Unknown or unsupported file encoding - ") + srcEnc );

    UINT cpDst = pEncApi->getCodePageByName( targetEnc );
    if (!cpDst)
        throw std::runtime_error( std::string("Unknown or unsupported target encoding - ") + targetEnc );

    return pEncApi->convert(text, cpSrc, cpDst );
*/
}

//-----------------------------------------------------------------------------
inline
std::string readFileEncodedSafe( std::istream &in, std::string srcEnc = std::string(), std::string targetEnc = "UTF-8", std::string httpHint = std::string(), std::string metaHint = std::string() )
{
    std::string text = readFile( in, true );

    if (targetEnc.empty())
        targetEnc = "UTF-8";

    EncodingsApi* pEncApi = getEncodingsApi();

    if (srcEnc.empty())
    {
        size_t bomSize = 0;
        srcEnc = pEncApi->detect( text, bomSize, httpHint, metaHint );
    }

    if (srcEnc.empty())
        throw std::runtime_error( std::string("Invalid file encoding") );

    if (pEncApi->isEqualEncodingNames(srcEnc, targetEnc))
        return text; // nothing to convert

    UINT cpSrc = pEncApi->getCodePageByName( srcEnc );
    if (!cpSrc)
        throw std::runtime_error( std::string("Unknown or unsupported file encoding - ") + srcEnc );

    UINT cpDst = pEncApi->getCodePageByName( targetEnc );
    if (!cpDst)
        throw std::runtime_error( std::string("Unknown or unsupported target encoding - ") + targetEnc );

    return pEncApi->convert(text, cpSrc, cpDst );
}

//-----------------------------------------------------------------------------
inline
std::string readFileEncodedSafe( const std::string &fn, std::string srcEnc = std::string(), std::string targetEnc = "UTF-8", std::string httpHint = std::string(), std::string metaHint = std::string() )
{
    std::string res;
    std::string canonFileName = makeSimpleCanonicalFilename(fn);
    std::ifstream infile( canonFileName.c_str(), std::ios::in|std::ios::binary /* |std::ios::ate */ );

    if (!infile)
        throw std::runtime_error( std::string("failed to open file - ") + canonFileName );

    return readFileEncodedSafe( infile, srcEnc, targetEnc, httpHint, metaHint );

/*
    std::string text = readFileSafe( fn );

    if (targetEnc.empty())
        targetEnc = "UTF-8";

    EncodingsApi* pEncApi = getEncodingsApi();

    if (srcEnc.empty())
    {
        size_t bomSize = 0;
        srcEnc = pEncApi->detect( text, bomSize, httpHint, metaHint );
    }

    if (srcEnc.empty())
        throw std::runtime_error( std::string("Invalid file encoding") );

    if (pEncApi->isEqualEncodingNames(srcEnc, targetEnc))
        return text; // nothing to convert

    UINT cpSrc = pEncApi->getCodePageByName( srcEnc );
    if (!cpSrc)
        throw std::runtime_error( std::string("Unknown or unsupported file encoding - ") + srcEnc );

    UINT cpDst = pEncApi->getCodePageByName( targetEnc );
    if (!cpDst)
        throw std::runtime_error( std::string("Unknown or unsupported target encoding - ") + targetEnc );

    return pEncApi->convert(text, cpSrc, cpDst );
*/
}

