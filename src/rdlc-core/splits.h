#pragma once

#include <vector>
#include <set>
//#include "utils.h"
#include "trims.h"


inline
int charToDigit( char ch );

//-----------------------------------------------------------------------------
inline
bool splitToPair( std::string str, std::string &first, std::string &second, char ch, std::string::size_type pos = 0 )
{
    //std::string::size_type 
    pos = str.find( ch, pos );
    if (pos==std::string::npos)
    {
        first.assign(str, 0);
        second.clear();
        return false;
    }

    first.assign( str, 0, pos );
    second.assign( str, pos+1 );
    return true;
}

//-----------------------------------------------------------------------------
inline
void splitToVector( std::string str, std::vector<std::string> &vec, char ch )
{
    std::string f, s;
    while( splitToPair(str, f, s, ch) )
    {
        vec.push_back(f);
        str = s;
    }
    if (!str.empty())
       vec.push_back(f);
}


void textSplitToLines( const std::string &text, std::vector<std::string> &lines, std::string &detectedLinefeed );

inline
void splitTextToLines( const std::string &text, std::vector<std::string> &lines, std::string &detectedLinefeed )
{
    textSplitToLines( text, lines, detectedLinefeed);
}


//-----------------------------------------------------------------------------
inline
bool splitToPair( std::string str, std::string &first, std::string &second, const std::string &sep, std::string::size_type pos = 0 )
{
    //std::string::size_type 
    pos = str.find( sep, pos );
    if (pos==std::string::npos)
    {
        first.assign(str, 0);
        second.clear();
        return false;
    }

    first.assign( str, 0, pos );
    second.assign( str, pos+sep.size() );
    return true;
}

//-----------------------------------------------------------------------------
inline
bool splitToPairFromEnd( std::string str, std::string &first, std::string &second, const std::string &sep, std::string::size_type pos = std::string::npos )
{
    //std::string::size_type 
    pos = str.rfind( sep, pos );
    if (pos==std::string::npos)
    {
        first.assign(str, 0);
        second.clear();
        return false;
    }

    first.assign( str, 0, pos );
    second.assign( str, pos+sep.size() );
    return true;
}

//-----------------------------------------------------------------------------
inline
void splitToVector( std::string str, std::vector<std::string> &vec, const std::string &sep )
{
    std::string f, s;
    //while( splitToPair(str, f, s, sep) )
    while( splitToPair(str, f, str, sep) )
    {
        vec.push_back(f);
        //str = s;
    }
    vec.push_back(f);
}

//-----------------------------------------------------------------------------
inline
void splitToMap( std::string str, std::map< std::string, std::string > &mapa, const std::string &listSep = ",", const std::string &pairSep = ":")
{
    std::vector<std::string> optList;
    splitToVector( str, optList, listSep );
    for( const auto &optPair : optList )
    {
        std::string optName, optVal;
        splitToPair( optPair, optName, optVal, pairSep );

        std::map< std::string, std::string >::iterator optIt = mapa.find(optName);
        if (optIt == mapa.end())
        {
            mapa[optName] = optVal;
        }
        else // not an empty
        {
            if (optIt->second.empty())
                optIt->second = optVal;
            else
            {
                optIt->second.append(listSep);
                optIt->second.append(optVal);
            }
        }
    }
}

//-----------------------------------------------------------------------------
inline
void splitToSet( std::string str, std::set< std::string > &seta, const std::string &listSep)
{
    std::vector<std::string> optList;
    splitToVector( str, optList, listSep );
    seta.insert( optList.begin(), optList.end() );
}

//-----------------------------------------------------------------------------
inline
std::string mergeStrings( const std::vector<std::string> &v, const std::string &sep, bool skipEmpty = true)
{
    bool lastAddOk = false;
    std::string res;
    for(auto it = v.begin(); it!=v.end(); ++it)
    {
        if (lastAddOk)
            res.append(sep);
        if (skipEmpty)
        {
            if (it->empty())
            {
                lastAddOk = false;
            }
            else
            {
                res.append(*it);
                lastAddOk = true;
            }
        }
        else
        {
            res.append(*it);
            lastAddOk = true;
        }
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
bool splitToPairNotDecimalMinus( std::string str, std::string &first, std::string &second, char ch, std::string::size_type pos = 0 )
{
    std::vector<std::string> vec;
    splitToVector( str, vec, ch );

    if (vec.empty())
    {
        first.clear();
        second.clear();
        return false;
    }

    if (vec.size()<2)
    {
        first.assign(vec[0], 0);
        second.clear();
        return false;
    }

    std::vector<std::string>::size_type i = 1, size = vec.size();
    for(; i!=size; ++i )
    {
        std::string tmp = trim_copy(vec[i]);
        if (tmp.empty())
            break;
            //continue; // break;

        std::string::size_type badCharPos = tmp.find_first_not_of( "0123456789.+-Ee" );
        if (badCharPos==str.npos)
           continue; // integer or floating point detected

        break;
    }

/*
    if (i == size)
    {
        first.assign(vec[0], 0);
        second.assign( mergeStrings( std::vector<std::string>( vec.begin()+1, vec.end() ), std::string(1,ch) ) );
        return true;
    }
*/
    first.assign( mergeStrings( std::vector<std::string>( vec.begin(), vec.begin()+i ), std::string(1,ch), false ) );
    second.assign( mergeStrings( std::vector<std::string>( vec.begin()+i, vec.end() ), std::string(1,ch), false ) );

    return true;
}

//-----------------------------------------------------------------------------
static const int open_brace_flag = 0x10000000;

inline
int getBraceType( char ch )
{
    switch(ch)
       {
        case '{':
             return 0 | open_brace_flag;

        case '}':
             return 0;             

        case '[':
             return 1 | open_brace_flag;

        case ']':
             return 1;             

        case '(':
             return 2 | open_brace_flag;

        case ')':
             return 2;             

        case '<':
             return 3 | open_brace_flag;

        case '>':
             return 3;             

        default: return -1;
       }
}

//-----------------------------------------------------------------------------
inline
bool checkBraces( const int *braceCounters, size_t nCounters )
{
    for( size_t i=0; i!=nCounters; ++i)
    {
        if (braceCounters[i]!=0)
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
inline
void splitToVectorWithRestectForBraces( const std::string &str, std::vector<std::string> &vec, char sep)
{
    std::string buf;
    bool lastAdd = false;

    int braceCounters[4] = { 0, 0, 0, 0 };

    for( auto ch : str )
    {
        if (ch==sep && checkBraces(braceCounters, sizeof(braceCounters)/sizeof(braceCounters[0])))
        {
            vec.push_back(buf);
            buf.clear();
            lastAdd = true;
            continue;
        }

        buf.append(1, ch);
        lastAdd = false;

        int braceType = getBraceType(ch);
        if (braceType<0)
            continue;

        bool isOpeningBrace = (braceType & open_brace_flag)!=0;
        int braceIdx = braceType & ~open_brace_flag;
        if (isOpeningBrace)
            ++braceCounters[braceIdx]; //-V557
        else
            --braceCounters[braceIdx]; //-V557
    }

    if (!lastAdd && !buf.empty())
    {
        vec.push_back(buf);
    }
}

//-----------------------------------------------------------------------------
inline
bool splitToPairWithRestectForBraces( std::string str, std::string &first, std::string &second, char ch )
{
    std::vector<std::string> vec;
    splitToVectorWithRestectForBraces( str, vec, ch );

    first.clear();
    second.clear();

    if (vec.empty())
        return false;

    if (vec.size()==1)
    {
        first.assign(vec[0]);
        return false;
    }

    first.assign(vec[0]);
    second.assign( mergeStrings( std::vector<std::string>( vec.begin()+1, vec.end() ), std::string(1,ch) ) );
    return true;
}
