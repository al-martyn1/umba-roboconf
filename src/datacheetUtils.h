#pragma once

#include <string>
#include <vector>

#include "rdlc-core/case.h"
#include "rdlc-core/isa.h"



//----------------------------------------------------------------------------
inline
bool isDatasheetNetworkLink(std::string name)
{
    name = toLower(name);

    if (startsWith( name, "http://" ) || startsWith( name, "ftp://" ))
        return true;
    
    return false;
}

//----------------------------------------------------------------------------
inline
bool isDatacheetAbsFileName(std::string name)
{
    if (name.empty())
        return false;

    #if defined(WIN32) || defined(_WIN32)

        if (name[0]=='/' || name[0]=='\\') // abs path from cur drive
            return true;

        if (name.size()<2)
            return false;

        if (startsWith( name, "\\\\" )) // network path
            return true;

        if (name[1]==':') // abs - DRIVE:\
            return true;

        return false;

    #else

        if (name[0]=='/')
            return true;

        return false;

    #endif
}


