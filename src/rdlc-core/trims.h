#pragma once

#include <vector>
#include <string>
/*
inline void ltrim(std::string &s)
{
    s.erase( s.begin()
           , std::find_if( s.begin(), s.end(), [](int ch)
                         {
                             return !std::isspace(ch);
                         }
                         )
           );
}

// trim from end (in place)
inline void rtrim(std::string &s)
{
    s.erase( std::find_if( s.rbegin(), s.rend(), [](int ch)
                         {
                             return !std::isspace(ch);
                         }
                         ).base(), s.end()
           );
}
*/

//-----------------------------------------------------------------------------
inline
void ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
}
 
inline
void rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
}

inline
void trim(std::string &s, const std::string &chars = "\t\n\v\f\r ")
{
    ltrim(s, chars);
    rtrim(s, chars);
}

//-----------------------------------------------------------------------------
inline
void ltrim( std::vector<std::string> &v, const std::string& chars = "\t\n\v\f\r ")
{
    for( std::string& s : v )
        ltrim(s, chars);
}
 
inline
void rtrim(std::vector<std::string> &v, const std::string& chars = "\t\n\v\f\r ")
{
    for( std::string& s : v )
        rtrim(s, chars);
}

inline
void trim( std::vector<std::string> &v, const std::string &chars = "\t\n\v\f\r ")
{
    for( std::string& s : v )
        trim(s, chars);
}

//-----------------------------------------------------------------------------
inline
std::string ltrim_copy(std::string s, const std::string &chars = "\t\n\v\f\r ")
{
    ltrim(s, chars);
    return s;
}

inline
std::string rtrim_copy(std::string s, const std::string &chars = "\t\n\v\f\r ")
{
    rtrim(s, chars);
    return s;
}

inline
std::string trim_copy(std::string s, const std::string &chars = "\t\n\v\f\r ")
{
    trim(s, chars);
    return s;
}

//-----------------------------------------------------------------------------
inline
std::vector<std::string> ltrim_copy(std::vector<std::string> &v, const std::string &chars = "\t\n\v\f\r ")
{
    ltrim(v, chars);
    return v;
}

inline
std::vector<std::string> rtrim_copy(std::vector<std::string> &v, const std::string &chars = "\t\n\v\f\r ")
{
    rtrim(v, chars);
    return v;
}

inline
std::vector<std::string> trim_copy(std::vector<std::string> &v, const std::string &chars = "\t\n\v\f\r ")
{
    trim(v, chars);
    return v;
}

//-----------------------------------------------------------------------------
inline
bool unquote( std::string &s, char chStart, char chEnd )
{
    if (s.size()<2)
        return false;

    if (s[0]==chStart && s[s.size()-1]==chEnd)
    {
        s.erase( s.size()-1 );
        s.erase( 0, 1 );
        trim(s);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
inline
bool unquoteInclude( std::string &s )
{
    if (unquote( s, '\"', '\"' ))
       return false;
    if (unquote( s, '\'', '\'' ))
       return false;
    if (unquote( s, '<', '>' ))
       return true;
    return false;
}

//-----------------------------------------------------------------------------
inline
bool unquoteString( std::string &s )
{
    if (unquote( s, '\"', '\"' ))
       return true;
    if (unquote( s, '\'', '\'' ))
       return true;
    return false;
}

inline
bool unquoteTrimString( std::string &s )
{
    trim(s);
    bool res = unquoteString(s);
    trim(s);
    return res;
}


inline
char unquoteStringGetQuotChar( std::string &s )
{
    if (unquote( s, '\"', '\"' ))
       return '\"';

    if (unquote( s, '\'', '\'' ))
       return '\'';

    if (unquote( s, '<', '>' ))
       return '<';

    return 0;
}


