#pragma once

#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include "trims.h"
#include "splits.h"
#include "textUtils.h"

bool startsWithAndStrip( std::string &str, const std::string &prefix );

#include "fsUtils.h"
//#include "prog.h"




//-----------------------------------------------------------------------------
inline
bool isSpace( char ch, const std::string& spaceChars = "\t\n\v\f\r ")
{
    if (spaceChars.find( ch )==std::string::npos)
        return false;
    return true;
}

//-----------------------------------------------------------------------------
inline
bool isDigit( char ch)
{
    if (ch>='0' && ch<='9')
        return true;
    return false;
}

inline
bool isUpper( char ch )
{
    if (ch>='A' && ch<='Z')
        return true;
    return false;
}

inline
bool isLower( char ch )
{
    if (ch>='a' && ch<='z')
        return true;
    return false;
}

//-----------------------------------------------------------------------------
inline void strAlphaStat( const std::string &s, size_t &nUppers, size_t &nLowers, size_t &nDigits, size_t &nOthers )
{
    nUppers = 0;
    nLowers = 0;
    nDigits = 0;
    nOthers = 0;
    for( auto ch : s )
    {
        if (isUpper(ch))
           ++nUppers;
        else if (isLower(ch))
           ++nLowers;
        else if (isDigit(ch))
           ++nDigits;
        else
           ++nOthers;
    }
}

inline bool isUpperAlphasOnly( const std::string &s )
{
    size_t nUppers, nLowers, nDigits, nOthers;
    strAlphaStat( s, nUppers, nLowers, nDigits, nOthers );
    //if ((nUppers+nDigits+ nOthers))
    if (!nLowers)
        return true;
    return false;
}

inline bool isLowerAlphasOnly( const std::string &s )
{
    size_t nUppers, nLowers, nDigits, nOthers;
    strAlphaStat( s, nUppers, nLowers, nDigits, nOthers );
    //if ((nUppers+nDigits+ nOthers))
    if (!nUppers)
        return true;
    return false;
}

//-----------------------------------------------------------------------------
template< typename TKey, typename TVal >
inline
bool isEqual( const std::map<TKey, TVal> &s1, const std::map<TKey, TVal> &s2 )
{
    if (s1.size()!=s2.size())
        return false;

    typename std::map<TKey, TVal>::const_iterator it1 = s1.begin();
    typename std::map<TKey, TVal>::const_iterator it2 = s2.begin();

    for(; it1!=s1.end(); ++it1, ++it2)
    {
        if (it1->first!=it2->first)
            return false;
        if (it1->second != it2->second)
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
template< typename TVal >
inline
bool isEqual( const std::set<TVal> &s1, const std::set<TVal> &s2 )
{
    if (s1.size()!=s2.size())
        return false;

    typename std::set<TVal>::const_iterator it1 = s1.begin();
    typename std::set<TVal>::const_iterator it2 = s2.begin();

    for(; it1!=s1.end(); ++it1, ++it2)
    {
        if (*it1!=*it2)
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
template< typename TVal >
inline
bool isEqual( const std::vector<TVal> &s1, const std::vector<TVal> &s2 )
{
    if (s1.size()!=s2.size())
        return false;

    typename std::vector<TVal>::const_iterator it1 = s1.begin();
    typename std::vector<TVal>::const_iterator it2 = s2.begin();

    for(; it1!=s1.end(); ++it1, ++it2)
    {
        if (*it1!=*it2)
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
inline
char contains( const std::string &str, char ch )
{
    std::string::size_type pos = str.find( ch );
    if (pos==str.npos)
        return 0;
    return ch;
}

//-----------------------------------------------------------------------------
inline
char contains( const std::string &str, const std::string &chars )
{
    std::string::size_type pos = str.find_first_of( chars );
    if (pos==str.npos)
        return 0;
    return str[pos];
}

//-----------------------------------------------------------------------------
inline
bool startsWith( const std::string &str, const std::string &prefix )
{
    if (prefix.empty())
        return true;

    if (str.size()<prefix.size())
        return false;

    if (str.find(prefix)==0)
        return true;

    return false;
}

//-----------------------------------------------------------------------------
inline
bool endsWith( const std::string &str, const std::string &suffix )
{
    if (suffix.empty())
        return true;

    if (str.size()<suffix.size())
        return false;

    std::string::size_type pos = str.size() - suffix.size();
    if (str.find(suffix, pos)==pos)
        return true;

    return false;
}

//-----------------------------------------------------------------------------
inline
bool startsWithAndStrip( std::string &str, const std::string &prefix )
{
    if (!startsWith(str, prefix))
        return false;

    str.erase( 0, prefix.size());

    return true;
}

//-----------------------------------------------------------------------------
inline
bool endsWithAndStrip( std::string &str, const std::string &suffix )
{
    if (!endsWith(str, suffix))
        return false;

    str.erase( str.size() - suffix.size() );

    return true;
}

//-----------------------------------------------------------------------------
inline
bool isAlphaChar(char ch)
{
    if (ch>='a' && ch<='z')
        return true;
    if (ch>='A' && ch<='Z')
        return true;

    return false;
}

//-----------------------------------------------------------------------------
inline
bool isDecDigitChar(char ch)
{
    if (ch>='0' && ch<='9')
        return true;

    return false;
}

//-----------------------------------------------------------------------------
inline
bool isIdentChar(char ch)
{
    return isAlphaChar(ch) || isDecDigitChar(ch) || (ch == '_');
}

//-----------------------------------------------------------------------------
inline
bool isContinuationLine( std::string line )
{
    trim(line);
    if (line.empty())
        return false;
    if (line.back()!='\\')
        return false;
    return true;
}

//-----------------------------------------------------------------------------
inline
bool stripLineContinuation( std::string &line )
{
    trim(line);
    if (line.empty())
        return false;
    if (line.back()!='\\')
        return false;
    line.erase( line.size()-1 );
    trim(line);
    return true;
}

//-----------------------------------------------------------------------------
inline
bool isComment( std::string line )
{
    trim(line);

    return startsWith( line, ";" ) || startsWith( line, "#" ) || startsWith( line, "//" );
}

//-----------------------------------------------------------------------------
inline
bool isDirective( std::string line, std::string &dirName, std::string &dirArg )
{
    trim(line);
    if (!startsWith( line, "$" ))
        return false;

    line.erase(0, 1);
    splitToPair( line, dirName, dirArg, ' ' );

    trim(dirName);
    trim(dirArg);

    return true;
}

//-----------------------------------------------------------------------------
inline
std::string::size_type countOfChar( const std::string &s, char ch )
{
    std::string::size_type cnt = 0;
    for(auto it = s.begin(); it!=s.end(); ++it)
    {
        if (*it==ch)
            cnt++;
    }

    return cnt;
}

//-----------------------------------------------------------------------------
inline
char isOnlySameCharString( const std::string &s )
{
    if (s.empty())
        return 0;

    char ch = s[0];
    auto cnt = countOfChar( s, ch );
    return cnt==s.size() ? ch : (char)0;
}

//-----------------------------------------------------------------------------
inline
bool isShortNamespace( std::string line, std::string &nsName )
{
    trim(line);

    if (line.empty())
        return false;

    if (countOfChar( line, ':')!=1)
        return false;

    if (countOfChar(line, '.')!=0)
        return false;

    if (!endsWith( line, ":" ))
        return false;

    line.erase( line.size()-1 );

    trim(line);

    if (line.empty())
        return false;

    nsName = line;

    return true;
}

//-----------------------------------------------------------------------------
inline
bool isCommandLineOption( std::string cmdLineArg, std::string &opt, std::string &optArg, bool &bShort )
{
    if (startsWithAndStrip( cmdLineArg, "--"))
    {
        bShort = false;
        bool hasEq = splitToPair(cmdLineArg, opt, optArg, '=');
        trim(opt);
        trim(optArg);

        if ( !hasEq /* optArg.empty() */ )
        {
            if (endsWithAndStrip( opt, "-"))
                optArg = "-";
            else if (endsWithAndStrip( opt, "+"))
                optArg = "+";
        }

        return true;
    }
    if (startsWithAndStrip( cmdLineArg, "-"))
    {
        bShort = true;
        bool hasEq = splitToPair(cmdLineArg, opt, optArg, '=');
        trim(opt);
        trim(optArg);

        if ( !hasEq /* optArg.empty() */ )
        {
            if (endsWithAndStrip( opt, "-"))
                optArg = "-";
            else if (endsWithAndStrip( opt, "+"))
                optArg = "+";
        }

        return true;
    }

    return false;
}

inline
bool isCommandLineOption( std::string opt, bool bShort, std::string optLongName )
{
    if (bShort==false)
    {
        if (opt==optLongName)
            return true;
        return false;
    }

    return false;
}

inline
bool isCommandLineOption( std::string opt, bool bShort, std::string optLongName, char optShort1 )
{
    if (bShort==false)
    {
        if (opt==optLongName)
            return true;
        return false;
    }

    if (opt==std::string(1,optShort1))
        return true;

    return false;
}

inline
bool isCommandLineOption( std::string opt, bool bShort, std::string optLongName, char optShort1, char optShort2 )
{
    if (bShort==false)
    {
        if (opt==optLongName)
            return true;
        return false;
    }

    if (opt==std::string(1,optShort1))
        return true;

    if (opt==std::string(1,optShort2))
        return true;

    return false;
}







