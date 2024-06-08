#pragma once

#include <string>
#include <vector>
#include <iostream>

//
#include "tracy_tracing.h"



//-----------------------------------------------------------------------------
// Аналогично splitComponentName, но splitAlsoDigits всегда true, keepSeps - false, и разделитель - точка
// bool splitAlsoDigits = true
inline
std:: vector<std::string> splitDesignatorPinName(const std::string &name)
{
    const bool splitAlsoDigits = true ;
    const bool keepSeps        = false;

    enum CharClass{
        charClassInitial = 0,
        charClassAlpha   = 1,
        charClassDigit   = 2,
        charClassSep     = 3
    };

    CharClass prevCharClass = charClassInitial;

    std:: vector<std::string> res; res.reserve(4);
    std::string buf;
    char prevSep = 0;

    size_t i = 0, size = name.size();

    for(; i!=size; ++i)
    {
        char ch = name[i];

        CharClass  charClass = charClassInitial;

        if (ch>='a' && ch<='z')
            charClass = charClassAlpha;
        else if (ch>='A' && ch<='Z')
            charClass = charClassAlpha;
        else if (ch>='0' && ch<='9')
        {
            if (splitAlsoDigits)
                charClass = charClassDigit;
            else
                charClass = charClassAlpha;
        }
        else 
        {
            charClass = charClassSep;
            prevSep   = ch;
        }

        if (prevCharClass!=charClassSep && prevCharClass!=charClass && !buf.empty())
        {
            res.push_back(buf);
            buf.clear();
        }

        if (charClass==charClassSep)
        {
            if (!prevSep)
                prevSep = '.';
            if (keepSeps)
                buf.append(1,prevSep);
            //charClass = charClassAlpha;
        }
        else
        {
            buf.append(1,ch);
        }

        prevCharClass = charClass;

    }

    if (!buf.empty())
    {
        res.push_back(buf);
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
int compareDesignatorPinNames(const std::string &name1, const std::string &name2)
{
    std:: vector<std::string> v1 = splitDesignatorPinName(name1);
    std:: vector<std::string> v2 = splitDesignatorPinName(name2);

    std:: vector<std::string>::const_iterator it1 = v1.begin();
    std:: vector<std::string>::const_iterator it2 = v2.begin();

    for(; it1!=v1.end() && it2!=v2.end(); ++it1, ++it2)
    {
        try
        {
            auto ll1 = std::stoll(*it1);
            auto ll2 = std::stoll(*it2);

            if (ll1<ll2)
                return -1;

            if (ll1>ll2)
                return 1;

        }
        catch(...)
        {
            if (*it1<*it2)
                return -1;

            if (*it1>*it2)
                return 1;
        
        }
    }

    if (it1==v1.end() && it2==v2.end()) // оба закончились - равны
        return 0;

    // Который короче, тот и меньше

    if (it1==v1.end()) // первый закончился, а второй - ещё нет
        return -1;

    return 1;

}

//-----------------------------------------------------------------------------
inline
bool designatorPinNamesLess(const std::string &name1, const std::string &name2)
{
    auto res = compareDesignatorPinNames(name1, name2);
    
    // std::cout << "Comparing '" << name1 << "' and '" << name2 << "': " << res << "\n";
     
    return res < 0;
}
 


