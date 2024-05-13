#pragma once

#include "rdlc-core/cpp.h"

#if 0
inline
std::string makeCppDefineName( const std::string &str, bool allowConvertMinus )
{
    std::string res; res.reserve(str.size());

    std::string::size_type i = 0, size = str.size();
    for(; i!=size; ++i)
       {
        char ch = str[i];

        if ( ch>='A' && ch<='Z' )
           {
            res.append(1, ch );
            continue;
           }

        if ( ch>='a' && ch<='z' )
           {
            res.append(1, ch - 'a' + 'A');
            continue;
           }

        if ( ch>='0' && ch<='9' )
           {
            if (res.empty())
               res.append(1, '_');
            res.append(1, ch);
            continue;
           }

        if ( ch=='+' )
           {
            res.append("PLUS");
            continue;
           }

        if ( ch=='-' )
           {
            if (allowConvertMinus)
                res.append("MINUS");
            else
                res.append("_");
            continue;
           }
           
        if ( ch=='*' )
           {
            res.append("MUL");
            continue;
           }
           
        res.append(1, '_');
       }

    return res;
}

inline
std::string makeCppDefineName( const std::string &str )
{
    return makeCppDefineName( str, false /* allowConvertMinus */  );
}
/*
inline
std::string makeCppDefineName( const std::string &str )
{
    std::string res; res.reserve(str.size());

    char prevCh = 0;
    std::string::size_type i = 0, size = str.size();
    for(; i!=size; ++i)
       {
        char ch = str[i];

        if ( ch=='n' )
           {
            res.append(1, ch );
           }
        else if ( ch>='A' && ch<='Z')
           {
            res.append(1, ch );
           }
        else if ( ch>='a' && ch<='z' )
           {
            res.append(1, ch - 'a' + 'A');
           }
        else if ( ch>='0' && ch<='9' )
           {
            if (res.empty())
               res.append(1, '_');
            res.append(1, ch);
           }
        else if ( ch=='+' )
           {
            if (prevCh!=0 && prevCh!='_')
                res.append(1, '_');
            res.append("PLUS");
           }
        else if ( ch=='-' )
           {
            if (prevCh!=0 && prevCh!='_')
                res.append(1, '_');
            res.append("PLUS");
           }
        else if ( ch=='*' )
           {
            if (prevCh!=0 && prevCh!='_')
                res.append(1, '_');
            res.append("MUL");
           }
        else
           {
            ch = '_';
            res.append(1, ch);
           }
        prevCh = ch;
       }

    return res;
}
*/


inline
std::string makeCppName( const std::string &str )
{
    std::string res; res.reserve(str.size());

    std::string::size_type i = 0, size = str.size();
    for(; i!=size; ++i)
       {
        char ch = str[i];

        if ( (ch>='A' && ch<='Z') || (ch>='a' && ch<='z') )
           {
            res.append(1, ch );
           }
        else if ( ch>='0' && ch<='9' )
           {
            if (res.empty())
               res.append(1, '_');
            res.append(1, ch);
            continue;
           }
        else if ( ch=='+' )
           {
            res.append("PLUS");
            continue;
           }
        else if ( ch=='~' )
           {
            res.append("n");
            continue;
           }
        else if ( ch=='-' )
           {
            res.append("MINUS");
            continue;
           }
        else if ( ch=='*' )
           {
            res.append("MUL");
            continue;
           }
        else
           {
            res.append(1, '_');
           }
        
       }

    return res;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2 )
{
    return s1 + std::string("_") + s2;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2, const std::string &s3 )
{
    return s1 + std::string("_") + s2 + std::string("_") + s3;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4 )
{
    return s1 + std::string("_") + s2 + std::string("_") + s3 + std::string("_") + s4;
}

inline
std::string cppConcat( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5 )
{
    return s1 + std::string("_") + s2 + std::string("_") + s3 + std::string("_") + s4 + std::string("_") + s5;
}

#endif

