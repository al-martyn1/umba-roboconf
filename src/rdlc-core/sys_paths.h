#pragma once

#if defined(WIN32) || defined(_WIN32)
    #include <Shlobj.h>
#endif


inline
std::string getUserFolderPath()
{
    #if defined(WIN32) || defined(_WIN32)

    char buf[4096];

    // SHGetFolderPathA
    if (SHGetSpecialFolderPathA( 0, buf, CSIDL_PROFILE, false))
    //if (SHGetFolderPathA( 0, buf, CSIDL_PROFILE, false))
       return buf;

    return std::string();

    #else

    return "~";

    #endif
}

