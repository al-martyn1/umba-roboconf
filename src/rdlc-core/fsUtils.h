#pragma once

#include "splits.h"
#include "isa.h"

#if defined(WIN32) || defined(_WIN32)
    #include <winsock2.h>
    #include <windows.h>
#endif



//-----------------------------------------------------------------------------
inline
bool isPathSep( char ch )
{
    return ch=='/' || ch=='\\';
}

//-----------------------------------------------------------------------------
inline
char getPathSep( )
{
    #if defined(WIN32) || defined(_WIN32)
    return '\\';
    #else
    return '/';
    #endif
}

//-----------------------------------------------------------------------------
inline
void stripPathSep( std::string &p )
{
    trim( p, "/\\" );
/*
    if (p.empty())
        return;

    if (!isPathSep(p.back()))
        return;

    p.erase( p.size()-1, 1 );
*/
}

//-----------------------------------------------------------------------------
inline
std::string makeSimpleCanonicalFilename( const std::string &fileName )
{
    std::string res;
    res.reserve(fileName.size());

    for( auto ch : fileName )
    {
        if (ch>='A' && ch<='Z')
        {
            ch = ch -'A' + 'a';
        }
        else if (isPathSep(ch)) // (ch=='\\' || ch=='/')
        {
            ch = getPathSep();
            /*
            #if defined(WIN32) || defined(_WIN32)
            ch = '\\';
            #else
            ch = '/';
            #endif
            */
        }

        res.append(1, ch);
    }

    /*
    #if defined(WIN32) || defined(_WIN32)
    while(startsWithAndStrip( res, ".\\" )) {}
    #else
    while(startsWithAndStrip( res, "./" )) {}
    #endif
    */
    std::string rmPart = ".";
    rmPart.append(1, getPathSep());
    while(startsWithAndStrip( res, rmPart )) {}

    return res;

}

//-----------------------------------------------------------------------------
inline
std::string appendPath( const std::string &p, std::string f )
{
    if (p.empty())
        return f;

    if (f.empty())
        return p;

    if (isPathSep(p.back())) // (p.back()=='/' || p.back()=='\\')
    {
        if (isPathSep(f.front())) // (f.front()=='/' || f.front()=='\\')
            f.erase(0,1);
        return p + f;
    }

    if (isPathSep(f.front()))
        return p + f;
    else
        return p + std::string(1, getPathSep()) + f;

    /*
    #if defined(WIN32) || defined(_WIN32)
    if (f.front()=='/' || f.front()=='\\')
        return p + f;
    else
        return p + std::string("\\") + f;
    #else
    if (f.front()=='/' || f.front()=='\\')
        return p + f;
    else
        return p + std::string("/") + f;
    #endif
    */
}

//-----------------------------------------------------------------------------
inline
std::string getPath( const std::string &s )
{
    if (s.empty())
        return s;

    std::string::size_type pos = s.size();
    for( ; pos; --pos)
    {
        char ch = s[pos-1];
        if (isPathSep(ch)) // (ch=='\\' || ch=='/')
        {
            return std::string( s, 0, pos-1 );
        }
    }

    return std::string();
}

//-----------------------------------------------------------------------------
inline
std::string getPathName( const std::string &s )
{
    if (s.empty())
        return s;

    std::string::size_type pos = s.size();
    for( ; pos; --pos)
    {
        char ch = s[pos-1];
        if (ch=='.')
        {
            return std::string( s, 0, pos-1 ); // cut ext
        }
        if (isPathSep(ch)) // (ch=='\\' || ch=='/') // found path sep before ext sep
        {
            return s;
        }
    }

    return s;
}

//-----------------------------------------------------------------------------
inline
std::string getNameFromFull( const std::string &s )
{
    if (s.empty())
        return s;

    std::string::size_type pos = s.size();
    for( ; pos; --pos)
    {
        char ch = s[pos-1];
        if (isPathSep(ch)) // (ch=='\\' || ch=='/')
        {
            return std::string( s, pos );
        }
    }

    return s;
}

//-----------------------------------------------------------------------------
inline
std::string getFileName( const std::string &name )
{
    std::string f, s;
    splitToPair( getNameFromFull(name), f, s, '.' );
    return f;
}

//-----------------------------------------------------------------------------
inline
std::string getFileExtention( const std::string &name )
{
    std::string f, s;
    splitToPair( getNameFromFull(name), f, s, '.' );
    return s;
}

inline
std::string appendExtention( std::string name, std::string ext )
{
    if (!ext.empty() && ext.front()=='.')
        ext.erase(0,1);
    if (name.empty() || name.back()!='.')
        name.append(".");
    return name+ext;
}

//-----------------------------------------------------------------------------
inline
std::string generateOutputFilename( const std::string &inputFilename, const std::string &outputFilename, const std::string &defName, const std::string &defExt)
{
    std::string outputFinalName = outputFilename;
    std::string ext  = getFileExtention( outputFinalName );
    std::string name = getFileName( outputFinalName );
    std::string path = getPath( outputFinalName );

    std::string inputPath = getPath(inputFilename);

    
    if (outputFinalName.empty())
        path = getPath(inputFilename);
    
    if (name.empty())
    {
        outputFinalName = defName;
        if (!path.empty())
            outputFinalName = path + std::string(1, getPathSep()) /* std::string("\\") */  + outputFinalName;
    }
    
    //if (ext.empty())
    //    ext = defExt;
    
    if (ext.empty())
    {
        if (!defExt.empty())
            outputFinalName.append(".");
        outputFinalName.append(defExt);
    }

    return outputFinalName;

}

//-----------------------------------------------------------------------------
inline
bool setFileReadOnlyAttr( const std::string &file, bool bSet = true )
{
    #if defined(WIN32) || defined(_WIN32)
    DWORD attrs = GetFileAttributesA( file.c_str() );
    if (attrs==INVALID_FILE_ATTRIBUTES)
       {
        return false; // GetLastError();
       }

    if (bSet)
       attrs |= FILE_ATTRIBUTE_READONLY;
    else
       attrs &= ~FILE_ATTRIBUTE_READONLY;

    BOOL bRes = SetFileAttributesA( file.c_str(), attrs );
    if (!bRes)
       {
        return false; //GetLastError();
       }
    return true;
    #else
    return false; //UNDONE: not implemented
    #endif
}

//-----------------------------------------------------------------------------
/*
inline
RCODE setFileReadOnlyAttr( const std::wstring &file, bool bSet = true )
{
    DWORD attrs = GetFileAttributesW( file.c_str() );
    if (attrs==INVALID_FILE_ATTRIBUTES)
       {
        return WIN2RC(GetLastError());
       }

    if (bSet)
       attrs |= FILE_ATTRIBUTE_READONLY;
    else
       attrs &= ~FILE_ATTRIBUTE_READONLY;

    BOOL bRes = SetFileAttributesW( file.c_str(), attrs );
    if (!bRes)
       {
        return WIN2RC(GetLastError());
       }
    return EC_OK;
}
*/
