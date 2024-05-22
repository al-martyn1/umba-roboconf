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

//----------------------------------------------------------------------------
inline
int compareDatasheetFilenamesNetworkOrLocal(const std::string &name1, const std::string &name2)
{
    bool n1 = isDatasheetNetworkLink(name1);
    bool n2 = isDatasheetNetworkLink(name2);

    if (n1==n2)
        return 0;

    if (!n1) // не сетевой - он меньше
        return -1;

    return 1;
}

//----------------------------------------------------------------------------
inline
bool datasheetFilenamesNetworkOrLocalLess(const std::string &name1, const std::string &name2)
{
    int cmp = compareDatasheetFilenamesNetworkOrLocal(name1, name2);
    return cmp < 0;
}


// //----------------------------------------------------------------------------
// template< typename T, typename Pred >
// typename std::vector<T>::iterator
//     insert_sorted( std::vector<T> & vec, T const& item, Pred pred )
// {
//     return vec.insert
//         ( 
//            std::upper_bound( vec.begin(), vec.end(), item, pred ),
//            item 
//         );
// }


