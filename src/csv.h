#pragma once


#include <iostream>
#include <string>
#include <vector>
#include "rdlc-core/splits.h"

//
#include "tracy_tracing.h"



typedef std:: vector< std::string > csv_line_t;

inline
bool readCsv( std::istream &in, std:: vector< csv_line_t > &csvLines, char sep = ';' )
{
    std::string line;
    while( std::getline( in, line ) )
    {
        csv_line_t csvLine;
        splitToVector( line, csvLine, sep );
        csvLines.emplace_back(csvLine);
    }

    return true;
}

inline
size_t csvLineGetNonEmptyCount( const csv_line_t &l )
{
    size_t cnt = 0;
    for(size_t i = 0; i!=l.size(); ++i)
    {
        if (!l[i].empty())
            cnt++;
    }
    return cnt;
}

inline
size_t csvLineGetNextNonEmpty( const csv_line_t &l, size_t startFrom = 0 )
{
    for(size_t i = startFrom; i<l.size(); ++i)
    {
        if (!l[i].empty())
            return i;
    }
    return (size_t)-1;
}

inline
size_t csvLineFindContaining( const csv_line_t &l, const std::string &s, size_t startFrom = 0 )
{
    for(size_t i = startFrom; i<l.size(); ++i)
    {
        if (l[i].find(s)!=std::string::npos)
            return i;
    }
    return (size_t)-1;
}

inline
size_t csvLineFindEqual( const csv_line_t &l, const std::string &s, size_t startFrom = 0 )
{
    for(size_t i = startFrom; i<l.size(); ++i)
    {
        if (l[i] == s )
            return i;
    }
    return (size_t)-1;
}




