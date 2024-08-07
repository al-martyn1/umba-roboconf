#pragma once

#include "isa.h"
#include "splits.h"
#include "case.h"

#include <array>
#include <algorithm>
#include <utility>

//bool isSpace( char ch, const std::string& spaceChars = "\t\n\v\f\r ");
// bool isDigit( char ch);
// bool isUpper( char ch );
// bool isLower( char ch );
// void strAlphaStat( const std::string &s, size_t &nUppers, size_t &nLowers, size_t &nDigits, size_t &nOthers );
// bool isUpperAlphasOnly( const std::string &s );
// bool isLowerAlphasOnly( const std::string &s );

// char toUpper( char ch );
// char toLower( char ch );
// std::string toUpper( std::string s );
// std::string toLower( std::string s );
// std::vector<std::string> toUpper( std::vector<std::string> v );
// std::vector<std::string> toLower( std::vector<std::string> v );
// std::set<std::string> toUpper( const std::set<std::string> &s );
// std::set<std::string> toLower( const std::set<std::string> &s );




//-----------------------------------------------------------------------------
inline
std::string textAddIndent(const std::string &text, const std::string &indent)
{
    std::string res;

    // (text.size()/40) - кол-во строк навскидку
    res.reserve( text.size() + (text.size()/40) * indent.size());

    res = indent;

    for( char ch : text)
    {
        res.append(1,ch);
        if (ch=='\n')
            res.append(indent);
    }

    return res;
/*
    std::vector< std::string > lines;
    splitToVector( text, lines, '\n' );
    for( auto &ln : lines )
    {
        ln = indent + ln;
    }
*/
}

//-----------------------------------------------------------------------------
inline
std::string compareStringsCasePrepare( const std::string &s, bool ci )
{
    if (ci) return toLower(s);
    return s;
}

//-----------------------------------------------------------------------------
//inline
//int compareStringsCaseNoCase( const std::string &s1, const std::string &s2, bool ci )

//-----------------------------------------------------------------------------
struct StringsLess
{
    bool bCaseIgnore;
    StringsLess( bool bCI = true ) : bCaseIgnore(bCI) {}
    bool operator()( const std::string &s1, const std::string &s2 ) const
    {
        return compareStringsCasePrepare(s1,bCaseIgnore) < compareStringsCasePrepare(s2,bCaseIgnore);
    }
};

struct StringsGreater
{
    bool bCaseIgnore;
    StringsGreater( bool bCI = true ) : bCaseIgnore(bCI) {}
    bool operator()( const std::string &s1, const std::string &s2 ) const
    {
        return compareStringsCasePrepare(s1,bCaseIgnore) > compareStringsCasePrepare(s2,bCaseIgnore);
    }
};

//-----------------------------------------------------------------------------
struct StringsLessFirstBySize
{
    bool bCaseIgnore;
    StringsLessFirstBySize( bool bCI = true ) : bCaseIgnore(bCI) {}
    bool operator()( const std::string &s1, const std::string &s2 ) const
    {
        if (s1.size()<s2.size())
            return true;
        if (s1.size()>s2.size())
            return false;
        return compareStringsCasePrepare(s1,bCaseIgnore) < compareStringsCasePrepare(s2,bCaseIgnore);
    }
};

struct StringsGreaterFirstBySize
{
    bool bCaseIgnore;
    StringsGreaterFirstBySize( bool bCI = true ) : bCaseIgnore(bCI) {}
    bool operator()( const std::string &s1, const std::string &s2 ) const
    {
        if (s1.size()>s2.size())
            return true;
        if (s1.size()<s2.size())
            return false;
        return compareStringsCasePrepare(s1,bCaseIgnore) > compareStringsCasePrepare(s2,bCaseIgnore);
    }
};

//-----------------------------------------------------------------------------
inline
std::string expandStringWidth( std::string str, std::string::size_type width )
{
    while(str.size() < width)
    {
        std::string::size_type i = 0;
        for( ; i!=str.size() && str.size()<width; ++i)
        {
            if (str[i]==' ')
            {
                str.insert( i, 1, ' ');
                while( i!=str.size() && str[i]==' ') ++i;
                --i;
            }
        }
    }

    return str;
}

//-----------------------------------------------------------------------------
inline
std::vector<std::string> prepareTextParaMakeLines( const std::string &para, std::string::size_type paraWidth )
{
    std::vector<std::string> words;
    splitToVector( para, words, ' ' );
    trim(words);

    std::vector< std::vector<std::string> > paraLinesWords;
    std::vector<std::string> curLineWords;
    std::string::size_type   curLineLen = 0;

    for( const auto &w : words )
    {
        if ( (curLineLen+1+w.size()) > paraWidth)
        {
            paraLinesWords.push_back(curLineWords);
            curLineLen = 0;
            curLineWords.clear();
        }

        curLineLen += 1 + w.size();
        curLineWords.push_back( w );
    }

    if (!curLineWords.empty())
    {
        paraLinesWords.push_back(curLineWords);
    }

    std::vector<std::string> res;
    std::vector< std::vector<std::string> >::size_type lineIdx = 0;

    for( const auto &lw : paraLinesWords )
    {
        if (lineIdx!=(paraLinesWords.size()-1))
            res.push_back( expandStringWidth(mergeStrings( lw, " ", true  /* skipEmpty */ ), paraWidth ));
        else
            res.push_back( mergeStrings( lw, " ", true  /* skipEmpty */ ));
        lineIdx++;
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
std::string prepareTextParaMakeString( const std::string &para, std::string::size_type paraWidth )
{
    std::vector<std::string> v = prepareTextParaMakeLines( para, paraWidth );
    auto res = mergeStrings( v, "\n", true  /* skipEmpty */ );
    if (!res.empty() && res.back()!='.' && res.back()!='!' && res.back()!='?' && res.back()!=':' && res.back()!=';')
        res.push_back('.');
    return res;
}

//-----------------------------------------------------------------------------
inline
std::string formatTextParas( std::string text, std::string::size_type paraWidth )
{
    std::vector<std::string> paras;
    splitToVector( text, paras, '\n' );

    text.clear();
    //trim(paras);
    for( auto &p : paras )
    {
        if (!p.empty() && p[0]==' ')
        {
            if (!text.empty())
                text.append("\n");
            trim(p);
            text.append("  ");
            text.append(p);
        }
        else
        {
            if (!text.empty())
                text.append("\n\n");
            text.append(prepareTextParaMakeString(p, paraWidth));
        }
    }

    return text;
    //return mergeStrings( paras, "\n\n", true /* skipEmpty */ );
}

//-----------------------------------------------------------------------------
inline
std::string parseEscapes( const std::string &text )
{
    std::string res;
    res.reserve( text.size() );

    bool prevEscape = false;

    for( char ch : text)
    {
        if (!prevEscape)
        {
            if (ch=='\\')
            {
                prevEscape = true;
            }
            else
            {
                res.append(1,ch);
            }
        }
        else
        {
            switch(ch)
            {
                case 'n':
                     res.append(1,'\n');
                     break;
                case 'r':
                     res.append(1,'\r');
                     break;
                case 't':
                     res.append(1,'\t');
                     break;
                case '\'':
                     res.append(1,'\'');
                     break;
                case '\"':
                     res.append(1,'\"');
                     break;
                case '\\':
                     res.append(1,'\\');
                     break;
                default:
                     res.append(1,ch);
            }

            prevEscape = false;
        }
    }

    return res;
}

//void splitToVector( std::string str, std::vector<std::string> &vec, char ch )

inline
std::string textAppendDot( std::string text )
{
    rtrim(text);
    if (!text.empty() && text.back()!='.')
        text.append(1, '.');
    return text;
}

inline
std::string textCompress( const std::string &text, const std::string &compressChars )
{
    std::string res;

    std::string::size_type i = 0, sz = text.size();

    char prevWs = 0;

    for(; i!=sz; ++i)
    {
        char ch = text[i];
        std::string::size_type chPos = compressChars.find(ch);
        //if (ch==' ' || ch=='\t')
        if (chPos!=compressChars.npos)
        {
            if (ch==prevWs)
            {
                // simple skip
            }
            else
            {
                prevWs = ch;
                res.append(1,ch);
            }
        }
        else
        {
            prevWs = 0;
            res.append(1,ch);
        }
    }

    return res;

}

inline
std::string textDetectLinefeed( const std::string &text )
{
    std::string::size_type crlfPos = text.find("\r\n");
    std::string::size_type lfPos   = text.find('\n');
    std::string::size_type crPos   = text.find('\r');

    struct PosAndLinefeed
    {
        std::string::size_type  pos;
        std::string             lfStr;
    };

    std::array<PosAndLinefeed, 3> positions = { PosAndLinefeed{crlfPos, "\r\n"}, PosAndLinefeed{lfPos, "\n"}, PosAndLinefeed{crPos, "\r"} };
    std::sort( positions.begin(), positions.end()
             , [](const PosAndLinefeed &p1, const PosAndLinefeed &p2)
               {
                   return p1.pos<p2.pos;
               }
             );

    if (positions[0].pos==std::string::npos)
    {
        return "\r\n";
    }

    return positions[0].lfStr;

    // // CRLF most used
    // std::string::size_type pos = text.find('\n');
    // if (pos == text.npos)
    // {
    //     pos = text.find('\r');
    //     if (pos != text.npos)
    //         return "\r";
    //     // no linefeeds found at all
    //     return "\r\n";
    // }
    //  
    // if (pos==0)
    // {
    //     if ((pos+1) != text.size() && text[pos+1]=='\r')
    //         return "\n\r"; // MAC style
    //     return "\n";
    // }
    //  
    // if (text[pos-1]=='\r')
    //     return "\r\n";
    //  
    // if ((pos+1) != text.size() && text[pos+1]=='\r')
    //     return "\n\r"; // MAC style
    //  
    // return "\n";
}

inline
std::string detectLinefeed( const std::string &text )
{
    return textDetectLinefeed(text);
}



inline
void textSplitToLines( const std::string &text, std::vector<std::string> &lines, std::string &detectedLinefeed )
{
    detectedLinefeed = textDetectLinefeed(text);
    // if (detectedLinefeed.empty())
    //     throw std::runtime_error("BEBEBEBEBE");

    splitToVector( text, lines, detectedLinefeed.back() );
    rtrim(lines, "\r\n");
}



