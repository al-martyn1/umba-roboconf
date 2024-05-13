#pragma once

#if defined(WIN32) || defined(WIN32)

#include <winsock2.h>
#include <windows.h>

inline
std::string osGetFullPathName( const std::string &fn )
{
    if (fn.empty())
        return fn;

    char buf[4096];
    DWORD res = GetFullPathNameA( fn.c_str(), sizeof(buf), &buf[0], 0 );
    if (res>=sizeof(buf))
        return fn;

    buf[res] = 0;
    return std::string(buf);
}

#else

inline
std::string getFullPathName( const std::string &fn )
{
    return fn;
}

#endif


