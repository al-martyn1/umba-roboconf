#pragma once

#include <set>

#include "splits.h"
#include "isa.h"
#include "trims.h"

inline
int charToDigit( char ch )
{
    if (ch>='0' && ch<='9')
        return ch - '0';
    if (ch>='a' && ch<='z')
        return ch - 'a' + 10;
    if (ch>='A' && ch<='Z')
        return ch - 'A' + 10;
    return -1;
}

inline
bool readNumericPart( const std::string &str, std::string::size_type &pos, unsigned &i, int ss = 10 )
{
    for(; pos!=str.size() && ((charToDigit(str[pos])<0) || (charToDigit(str[pos])>=ss)); ++pos )
    {
    }

    if (pos==str.size())
        return false;

    i = 0;

    for(; pos!=str.size(); ++pos)
    {
        int d = charToDigit(str[pos]);
        if (d<0 || d>=ss)
            return true;

        i *= (unsigned)ss;
        i += (unsigned)d;
    }

    return true;
}

inline
bool readNumericPart( const std::string &str, std::string::size_type &pos, uint64_t &i, int ss = 10 )
{
    for(; pos!=str.size() && ((charToDigit(str[pos])<0) || (charToDigit(str[pos])>=ss)); ++pos )
    {
    }

    if (pos==str.size())
        return false;

    i = 0;

    for(; pos!=str.size(); ++pos)
    {
        int d = charToDigit(str[pos]);
        if (d<0 || d>=ss)
            return true;

        i *= (uint64_t)ss;
        i += (uint64_t)d;
    }

    return true;
}

inline
std::vector<unsigned> readNumericParts( const std::string &str, int ss = 10 )
{
    std::vector<unsigned> res;

    std::string::size_type pos = 0;

    unsigned i = 0;

    while(readNumericPart( str, pos, i, ss ))
         res.push_back( i );

    return res;
}

inline
int compareUnsignedVectors( std::vector< unsigned > v1, std::vector< unsigned > v2 )
{
    while( v2.size() < v1.size())
        v2.insert( v2.begin(), unsigned(0) );

    while( v1.size() < v2.size())
        v1.insert( v1.begin(), unsigned(0) );

    std::vector< unsigned >::const_iterator it1 = v1.begin(), it2 = v2.begin();

    for( ; it1!=v1.end(); ++it1, ++it2)
    {
        if (*it1==*it2)
            continue;

        if (*it1 < *it2)
            return -1;

        return 1;
    }

    return 0;
}


// restricted chars
inline
std::string replaceForbiddenChars( std::string str, const std::string &forbiddenChars, char replaceWithCh = '_')
{
    for( auto &ch : str )
    {
        if (forbiddenChars.find( ch )!=forbiddenChars.npos)
            ch = replaceWithCh;
    }
    return str;
}

inline
bool parseInteger ( const std::string &str, uint64_t &u, int ss )
{
    if (str.empty())
        return false;

    // -9223372036854775808 - число, для которого нет положительной пары
    u = 0;
    //auto i = str.begin()
    for(auto ch : str)
    {
        int d = charToDigit(ch);
        if (d<0 || d>=ss)
            return false;

        u *= (uint64_t)ss;
        u += (uint64_t)d;
    }

    return true;
}

inline
uint64_t parseIntegerSimple( const std::string &str, int ss )
{
    uint64_t res = 0;
    if (!parseInteger(str, res, ss))
        return 0;
    return res;
}


inline
uint64_t parseIntegerTolerant( const std::string &str, int ss = 10 )
{
    uint64_t u = 0;
    if (str.empty())
        return false;

    // -9223372036854775808 - число, для которого нет положительной пары
    //auto i = str.begin()
    for(auto ch : str)
    {
        int d = charToDigit(ch);
        if (d<0 || d>=ss)
            continue;

        u *= (uint64_t)ss;
        u += (uint64_t)d;
    }

    return u;
}

inline
bool parseInteger ( std::string str, uint64_t &u )
{
    trim(str);

    if (unquote( str, '\'', '\'' ))
    {
        u = 0;
        for( auto c : str)
        {
            u<<=8;
            u |= (uint64_t)(uint8_t)c;
        }
        return true;
    }

    if (startsWithAndStrip( str, "0x" ) || startsWithAndStrip( str, "0X" ))
    {
        trim(str);
        return parseInteger ( str, u, 16 );
    }

    if (startsWithAndStrip( str, "0b" ) || startsWithAndStrip( str, "0B" ))
    {
        trim(str);
        return parseInteger ( str, u, 2 );
    }

    if (startsWithAndStrip( str, "-" ))
    {
        trim(str);
        //UNDONE: нужно попытаться найти символьную константу
        bool res = parseInteger ( str, u, 10 );
        if (!res)
            return res;

        u = (uint64_t) (- (int64_t)u);
        return true;
    }

    if (startsWithAndStrip( str, "+" ))
    {
        trim(str);
        //UNDONE: нужно попытаться найти символьную константу
        return parseInteger ( str, u, 10 );
    }

    //UNDONE: нужно попытаться найти символьную константу
    return parseInteger ( str, u, 10 );

}


template<typename TMap>
size_t findMapKeyMaxLen( const TMap &m )
{
    size_t len = 0;
    for(auto it = m.begin(); it!=m.end(); ++it)
    {
        if (it->first.size() > len)
            len = it->first.size();
    }

    return len;
}

template<typename T>
size_t findContainerValsMaxLen( const T &t )
{
    size_t len = 0;
    for(auto it = t.begin(); it!=t.end(); ++it)
    {
        if (it->size() > len)
            len = it->size();
    }

    return len;
}


template< typename TVal >
void makeUniqueVector( std::vector< TVal > &v )
{
    std::set<TVal> s;
    std::vector< TVal > res;

    res.reserve(v.size());

    for( auto u : v )
    {
        if (s.find(u)!=s.end())
            continue;
        s.insert(u);
        res.push_back(u);
    }

    v.swap(res);

}


template< typename TMap >
inline
std::string generateNewName( const TMap &m, const std::string &name )
{
    if (m.find(name)==m.end())
        return name;

    std::string namePrefix = name;
    //std::string::size_type i = name.size();

    auto lastNonDigit = namePrefix.find_last_not_of( "0123456789", std::string::npos );
    uint64_t index = 0;

    if (lastNonDigit==std::string::npos)
    {
        index = parseIntegerTolerant( namePrefix );
        namePrefix.clear();
    }
    else if (lastNonDigit==(namePrefix.size()-1))
    {
        index = 0;
        namePrefix.append(1, '_');
    }
    else
    {
        index = parseIntegerTolerant( std::string(namePrefix, lastNonDigit + 1) );
        namePrefix.erase(lastNonDigit + 1 );
    }

    do
    {
        std::ostringstream oss;
        oss<<namePrefix<<index;
        if (m.find(oss.str())==m.end())
            return oss.str();
        index++;
    } while(true);

    return name;
}

template<typename TCharValidator>
inline
std::string filterForValidName( const std::string &name, const TCharValidator& validator )
{
    std::string res;
    for( auto ch : name )
    {
        if (validator(ch))
            res.append(1,ch);
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
std::string::size_type searchAndReplace( std::string &str, const std::string &lookFor, const std::string &replaceWith, bool bSingleShot = false )
{
    //size_t cnt = 0;
    std::string::size_type prevPos = std::string::npos;
    std::string::size_type pos = str.find(lookFor);
    while( pos!=std::string::npos )
    {
        str.replace( pos, lookFor.size(), replaceWith );
        pos += replaceWith.size();
        if (bSingleShot)
            return pos;

        prevPos = pos;
        pos = str.find(lookFor,pos);
        //cnt++;
    }

    //return cnt>0;
    return prevPos;
}

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
//template< typename TItem >
inline
std::vector<std::string> makeStringVector( )
{
    return std::vector<std::string>(); 
}

//-----------------------------------------------------------------------------
inline
std::vector<std::string> makeStringVector( const std::string &s1 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    res.emplace_back(s4);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    res.emplace_back(s4);
    res.emplace_back(s5);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    res.emplace_back(s4);
    res.emplace_back(s5);
    res.emplace_back(s6);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6, const std::string &s7 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    res.emplace_back(s4);
    res.emplace_back(s5);
    res.emplace_back(s6);
    res.emplace_back(s7);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6, const std::string &s7, const std::string &s8 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    res.emplace_back(s4);
    res.emplace_back(s5);
    res.emplace_back(s6);
    res.emplace_back(s7);
    res.emplace_back(s8);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6, const std::string &s7, const std::string &s8, const std::string &s9 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    res.emplace_back(s4);
    res.emplace_back(s5);
    res.emplace_back(s6);
    res.emplace_back(s7);
    res.emplace_back(s8);
    res.emplace_back(s9);
    return res;
}

inline
std::vector<std::string> makeStringVector( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6, const std::string &s7, const std::string &s8, const std::string &s9, const std::string &s10 )
{
    std::vector<std::string> res; 
    res.emplace_back(s1);
    res.emplace_back(s2);
    res.emplace_back(s3);
    res.emplace_back(s4);
    res.emplace_back(s5);
    res.emplace_back(s6);
    res.emplace_back(s7);
    res.emplace_back(s8);
    res.emplace_back(s9);
    res.emplace_back(s10);
    return res;
}


//-----------------------------------------------------------------------------
inline
std::set<std::string> makeStringSet( )
{
    return std::set<std::string>(); 
}

//-----------------------------------------------------------------------------
inline
std::set<std::string> makeStringSet( const std::string &s1 )
{
    std::set<std::string> res; 
    res.insert(s1);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2, const std::string &s3 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    res.insert(s3);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    res.insert(s3);
    res.insert(s4);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    res.insert(s3);
    res.insert(s4);
    res.insert(s5);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    res.insert(s3);
    res.insert(s4);
    res.insert(s5);
    res.insert(s6);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6, const std::string &s7 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    res.insert(s3);
    res.insert(s4);
    res.insert(s5);
    res.insert(s6);
    res.insert(s7);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6, const std::string &s7, const std::string &s8 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    res.insert(s3);
    res.insert(s4);
    res.insert(s5);
    res.insert(s6);
    res.insert(s7);
    res.insert(s8);
    return res;
}

inline
std::set<std::string> makeStringSet( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5, const std::string &s6, const std::string &s7, const std::string &s8, const std::string &s9 )
{
    std::set<std::string> res; 
    res.insert(s1);
    res.insert(s2);
    res.insert(s3);
    res.insert(s4);
    res.insert(s5);
    res.insert(s6);
    res.insert(s7);
    res.insert(s8);
    res.insert(s9);
    return res;
}

//-----------------------------------------------------------------------------

