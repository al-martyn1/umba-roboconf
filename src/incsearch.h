#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include "rdlc-core/fsUtils.h"

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
bool includeSearch( std::ifstream &ifs, const std::string &includeFile, std::string &foundName, const std::vector<std::string> &paths, std::vector<std::string> *pCheckedFiles = 0)
{
    for( const auto &p : paths)
    {
        //if (p.empty())
        //    continue;

        std::string fileName = p.empty() ? includeFile : p + std::string("\\") + includeFile;
        if (pCheckedFiles)
            pCheckedFiles->push_back(fileName);

        //using std::ios_base::iostate;
        //ifs.clear(eofbit|failbit|badbit);
        ifs.close();
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
bool includeSearch( std::ifstream &ifs, const std::string &includeFile, std::string &foundName, const std::vector<std::string> &pathsFirst, const std::vector<std::string> &pathsSecond, std::vector<std::string> *pCheckedFiles = 0 )
{
    if (includeSearch( ifs, includeFile, foundName, pathsFirst, pCheckedFiles))
        return true;
    if (includeSearch( ifs, includeFile, foundName, pathsSecond, pCheckedFiles))
        return true;
    return false;
}


