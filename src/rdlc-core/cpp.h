#pragma once

inline
const char* cppHelpersGetNumbersAlphabet( bool upper )
{
    static const char lowers[] = "0123456789abcdef";
    static const char uppers[] = "0123456789ABCDEF";

    return upper ? uppers : lowers;
}

inline
std::string cppHelpersFormatUnsigned( unsigned u, unsigned ss, bool upper, size_t width, char fillChar = 0 )
{
    if (fillChar==0)
    {
        if (ss==10)
           fillChar = ' ';
        else
           fillChar = '0';
    }

    std::string res;
    while( u )
    {
        unsigned d = u % ss;
        u /= ss;
        if (d>=16)
        {
            res.append(1, '#');
        }
        else
        {
            res.append(1, cppHelpersGetNumbersAlphabet(upper)[d] );
        }
    }

    if (res.empty())
        res = "0";

    if (res.size()<width)
    {
        res.append( width-res.size(), fillChar );
    }

    std::reverse( res.begin(), res.end() );

    return res;
}

inline
std::string cppHelpersFormatInt( int i )
{
    return i < 0
         ? std::string("-") + cppHelpersFormatUnsigned( (unsigned)-i, 10, false, 1, ' ' )
         :                    cppHelpersFormatUnsigned( (unsigned)i , 10, false, 1, ' ' )
         ;
}


inline
std::string cEscape( char ch )
{
    unsigned char uch = ch;
    switch(uch)
    {
        case '\r': return "\\r";
        case '\n': return "\\n";
        case '\t': return "\\t";
        case '\'': return "\\'";
        case '\"': return "\\\"";
        case '\\': return "\\\\";
        default:
            if (uch<(unsigned char)' ' || uch>0x7E)
            {
                return std::string("\\x") + cppHelpersFormatUnsigned( (unsigned)uch, 16, true, 2, '0' );
            }
            else
            {
                return std::string(1, ch);
            }
    }
}

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
            res.append("_PLUS");
            continue;
           }

        if ( ch=='-' )
           {
            if (allowConvertMinus)
                res.append("_MINUS");
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
    return makeCppDefineName( str, false );
#if 0
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
           
        if ( ch=='*' )
           {
            res.append("MUL");
            continue;
           }
           
        res.append(1, '_');
       }

    return res;
#endif

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

