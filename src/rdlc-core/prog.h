#pragma once

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#ifndef STRICT
    #define STRICT
#endif

#if defined(WIN32) || defined(_WIN32)
//#if !defined(_WINDOWS_)
    #include <winsock2.h>
    #include <windows.h>
#endif

#include "fsUtils.h"

inline
std::string getProgExeName()
{
    char buf[4096];

    DWORD res = GetModuleFileNameA( 0, buf, sizeof(buf) );
    if (res==sizeof(buf))
        buf[sizeof(buf)-1] = 0;

    return std::string(buf);
}


#else

std::string getProgExeName()
{
    return std::string();
}

#endif

// detectProgLocation( exeFullName, progBinPath, progRootPath );
inline
void detectProgLocation( std::string &exeFullName, std::string &progBinPath, std::string &progRootPath )
{
    exeFullName = getProgExeName();
    if (!exeFullName.empty())
    {
        progBinPath = getPath( exeFullName );
        bool isExeFolderBin = false;

        if (!progBinPath.empty())
        {
            std::string exeFolder = toLower(getFileName( progBinPath ));
            if (exeFolder=="bin" || exeFolder=="debug" || exeFolder=="release")
                isExeFolderBin = true;
        }

        if (isExeFolderBin)
        {
            progRootPath    = getPath( progBinPath );
        }
        else
        {
            progRootPath    = progBinPath;
        }

        //progConfPath    = progRootPath + std::string("\\conf");
        //progIncludePath = progRootPath + std::string("\\include");
    }
}





