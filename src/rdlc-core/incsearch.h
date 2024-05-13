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
bool includeSearch( std::ifstream &ifs, const std::string &includeFile, std::string &foundName, const std::vector<std::string> &paths)
{
    for( const auto &p : paths)
    {
        //if (p.empty())
        //    continue;

        std::string fileName = p.empty() ? includeFile : p + std::string("\\") + includeFile;

        //using std::ios_base::iostate;
        //ifs.clear(eofbit|failbit|badbit);
        ifs.clear();
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
bool includeSearch( std::ifstream &ifs, const std::string &includeFile, std::string &foundName, const std::vector<std::string> &pathsFirst, const std::vector<std::string> &pathsSecond )
{
    if (includeSearch( ifs, includeFile, foundName, pathsFirst ))
        return true;
    if (includeSearch( ifs, includeFile, foundName, pathsSecond ))
        return true;
    return false;
}


