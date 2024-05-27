#pragma once

#include "fsUtils.h"

inline
std::vector<std::string> makeIncVectorFromFileName( const std::string &pathFile )
{
    //return makeStringVector( getPath(pathFile) );
    std::string p = getPath(pathFile);
    if (p.empty())
        return makeStringVector( "." );
    else
        return makeStringVector( p, "." );

}

inline
bool includeSearch( std::ifstream &ifs, const std::string &includeFile, std::string &foundName, const std::vector<std::string> &paths, bool binaryMode = false)
{
    for( const auto &p : paths)
    {
        //if (p.empty())
        //    continue;

        std::string fileName = appendPath(p, includeFile);

        //using std::ios_base::iostate;
        //ifs.clear(eofbit|failbit|badbit);
        ifs.clear();
        if (binaryMode)
            ifs.open( fileName.c_str(), std::ios_base::in | std::ios_base::binary );
        else
            ifs.open( fileName.c_str() );
        if (!!ifs)
        {
            foundName = fileName;
            return true;
        }
    }

    return false;
}

inline
bool includeSearch( std::ifstream &ifs, const std::string &includeFile, std::string &foundName, const std::vector<std::string> &pathsFirst, const std::vector<std::string> &pathsSecond, bool binaryMode = false )
{
    if (includeSearch( ifs, includeFile, foundName, pathsFirst, binaryMode))
        return true;
    if (includeSearch( ifs, includeFile, foundName, pathsSecond, binaryMode ))
        return true;
    return false;
}


