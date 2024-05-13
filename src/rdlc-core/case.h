#pragma once

#include "isa.h"
#include "splits.h"


//bool isSpace( char ch, const std::string& spaceChars = "\t\n\v\f\r ");
bool isDigit( char ch);
bool isUpper( char ch );
bool isLower( char ch );
void strAlphaStat( const std::string &s, size_t &nUppers, size_t &nLowers, size_t &nDigits, size_t &nOthers );
bool isUpperAlphasOnly( const std::string &s );
bool isLowerAlphasOnly( const std::string &s );


//-----------------------------------------------------------------------------

const bool caseSensitive  = false;
const bool caseInensitive = true;
const bool caseIgnore     = true;



//-----------------------------------------------------------------------------

inline
char toUpper( char ch )
{
    if (ch>='a' && ch<='z')
        return ch - 'a' + 'A';
    return ch;
}

inline
char toLower( char ch )
{
    if (ch>='A' && ch<='Z')
        return ch - 'A' + 'a';
    return ch;
}

//-----------------------------------------------------------------------------
inline
std::string toUpper( std::string s )
{
    for (char& ch : s) 
    {
        ch = toUpper(ch);
    }

    return s;
}

inline
std::string toLower( std::string s )
{
    for (char& ch : s) 
    {
        ch = toLower(ch);
    }

    return s;
}

//-----------------------------------------------------------------------------
inline
std::vector<std::string> toUpper( std::vector<std::string> v )
{
    for (std::string& s : v) 
    {
        s = toUpper(s);
    }

    return v;
}

inline
std::vector<std::string> toLower( std::vector<std::string> v )
{
    for (std::string& s : v) 
    {
        s = toLower(s);
    }

    return v;
}

//-----------------------------------------------------------------------------
inline
std::set<std::string> toUpper( const std::set<std::string> &s )
{
    std::set<std::string> res;
    for (std::string str : s) 
    {
        res.insert(toUpper(str));
    }

    return res;
}

inline
std::set<std::string> toLower( const std::set<std::string> &s )
{
    std::set<std::string> res;
    for (std::string str : s) 
    {
        res.insert(toLower(str));
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
std::vector<std::string> splitByUpperCase( const std::string &s )
{
    //bool prevUpper = false;
    std::vector<std::string> resVec;
    if (isUpperAlphasOnly(s) || isLowerAlphasOnly(s))
    {
        resVec.push_back(s);
        return resVec;
    }
    
    std::string curStr;

    std::string::size_type i = 0, size = s.size();
    for(; i!=size; ++i)
    {
        char ch = s[i];
        if (isUpper(ch)) // new word starts?
        {
            if (!curStr.empty())
            {
                resVec.push_back(curStr);
                curStr.clear();
            }
        }

        curStr.append(1, ch);
    }

    if (!curStr.empty())
    {
        resVec.push_back(curStr);
        curStr.clear();
    }

    std::vector<std::string> resVec2;

    for (const auto& w : resVec)
    {
        if (w.empty())
            continue;

        if (w.size()>1)
        {
            resVec2.push_back(w);
            continue;
        }

        if (resVec2.empty())
            resVec2.push_back(std::string());

        resVec2[resVec2.size()-1].append(w);
    }

    return resVec2;
}

inline
std::vector<std::string> splitToWords( const std::string &s )
{
    std::vector<std::string> words;
    splitToVector( s, words, '_' );
    //if (words.size()>1)
    //    return words;
    //return splitByUpperCase( s );
    std::vector<std::string> res;
    for( auto w : words )
    {
        auto tmp = splitByUpperCase( w );
        res.insert( res.end(), tmp.begin(), tmp.end() );
    }
    
    return res;
}

//-----------------------------------------------------------------------------
inline
std::string makeCamelCaseEx( const std::string s, bool butFirst = false)
{
     std::vector<std::string> words = toLower(splitToWords(s));
     for( auto &w : words )
     {
         if (!w.empty())
             w[0] = toUpper(w[0]);
     }

     if (butFirst && !words.empty())
         words[0] = toLower(words[0]);

     return mergeStrings( words, std::string() );
}

//-----------------------------------------------------------------------------
inline
std::string makePascalCaseEx( const std::string s, size_t shortUppperLen = 2)
{
     std::vector<std::string> words = toLower(splitToWords(s));
     for( auto &w : words )
     {
         if (w.size()<=shortUppperLen)
             w = toUpper(w);
         else
             w[0] = toUpper(w[0]);
     }
     return mergeStrings( words, std::string() );
}


//-----------------------------------------------------------------------------
struct ICaseConverter
{
    virtual std::string getDescription() const = 0;
    virtual std::string convertCase( const std::string &s ) const = 0;

}; // struct ICaseConverter


struct CaseConverter_PascalCase : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("PascalCase");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        //return makeCamelCaseEx( s, false );
        return makePascalCaseEx( s, 2 );
    }

}; // struct CaseConverter_PascalCase

struct CaseConverter_camelCase : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("camelCase");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        return makeCamelCaseEx( s, true );
    }

}; // struct CaseConverter_camelCase

struct CaseConverter_BoobS_CasE : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("BoobS_CasE");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        std::vector<std::string> words = toLower(splitToWords(s));
        for( auto &w : words )
        {
            if (!w.empty())
            {
                w[0] = toUpper(w[0]);
                w[w.size()-1] = toUpper(w[w.size()-1]);
            }
        }
        return mergeStrings( words, std::string("_") );
    }

}; // struct CaseConverter_BoobS_CasE

struct CaseConverter_baLls_caSe : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("baLls_caSe");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        std::vector<std::string> words = toLower(splitToWords(s));
        for( auto &w : words )
        {
            auto s = w.size();
            if (s>1)
            {
                w[s/2] = toUpper(w[s/2]);
            }
        }
        return mergeStrings( words, std::string("_") );
    }

}; // struct CaseConverter_baLls_caSe

struct CaseConverter_std_case : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("std_case");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        return mergeStrings( toLower(splitToWords(s)), std::string("_") );
    }

}; // struct CaseConverter_std_case

struct CaseConverter_DEFINES_CASE : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("DEFINES_CASE");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        return mergeStrings( toUpper(splitToWords(s)), std::string("_") );
    }

}; // struct CaseConverter_DEFINES_CASE

struct CaseConverter_lowercase : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("allin_lowercase");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        return toLower(s);
    }

}; // CaseConverter_lowercase

struct CaseConverter_UPPERCASE : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("ALLIN_UPPERCASE");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        return toUpper(s);
    }

}; // CaseConverter_UPPERCASE

struct CaseConverter_Keep_case : public ICaseConverter
{
    virtual std::string getDescription() const override
    {
        return std::string("Keep_case");
    }

    virtual std::string convertCase( const std::string &s ) const override
    {
        return s;
    }

}; // CaseConverter_Keep_case

/*
struct CaseConverter_PascalCase
struct CaseConverter_camelCase 
struct CaseConverter_BoobS_CasE
struct CaseConverter_baLls_caSe
struct CaseConverter_std_case  
struct CaseConverter_DEFINES_CASE
struct CaseConverter_lowercase 
struct CaseConverter_UPPERCASE 
struct CaseConverter_Keep_case 
*/

