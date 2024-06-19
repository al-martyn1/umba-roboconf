#pragma once

#include <string>
#include <vector>
#include <iostream>

//
#include "umba/parse_utils.h"
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

    std:: vector<std::string> res; res.reserve(8);
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
            res.emplace_back(buf);
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
        res.emplace_back(buf);
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
int compareDesignatorPinNames(const std:: vector<std::string> &vecName1, const std:: vector<std::string> &vecName2)
{
    std:: vector<std::string>::const_iterator it1 = vecName1.begin();
    std:: vector<std::string>::const_iterator it2 = vecName2.begin();

    for(; it1!=vecName1.end() && it2!=vecName2.end(); ++it1, ++it2)
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

    if (it1==vecName1.end() && it2==vecName2.end()) // оба закончились - равны
        return 0;

    // Который короче, тот и меньше

    if (it1==vecName1.end()) // первый закончился, а второй - ещё нет
        return -1;

    return 1;

}

//-----------------------------------------------------------------------------
inline
int compareDesignatorPinNames(const std::string &name1, const std::string &name2)
{
    return compareDesignatorPinNames(splitDesignatorPinName(name1), splitDesignatorPinName(name2));
}

//-----------------------------------------------------------------------------
inline
bool designatorPinNamesLess(const std::string &name1, const std::string &name2)
{
    auto res = compareDesignatorPinNames(name1, name2);
    
    // std::cout << "Comparing '" << name1 << "' and '" << name2 << "': " << res << "\n";
     
    return res < 0;
}
 
//-----------------------------------------------------------------------------
//! Порты GPIO щбозначаются обычно как PNXX, или PYXX, где N - буква номера порта, Y - цифра (или две) номера порта, XX - одна или две цифры номера разряда порта. Начинается обычно с буквы P
template<typename IterType> inline
bool isGpioPinFunction(IterType b, IterType e)
{
    if (b==e)
        return false;

    if (*b!='P')
        return false;

    ++b;
    if (b==e)
        return false;

    bool secondIsDigit = umba::parse_utils::isDigit(*b, 10);
    // Не цифра, и не заглавная буква (hex цифры в верхнем регистре тоже подходят)
    if ( !secondIsDigit && *b<'A' && *b>'Z' )
        return false;

    ++b;
    if (b==e)
    {
        // Строка закончилась
        if (secondIsDigit)
        {
            // Строка вида PX - это единственный порт, с несколькими пинами, обозначенными цифрами
            // Других разумных вариантов для такой фукции пина нет
            return true;
        }
        
        // Строка вида PN- это единственный порт, с несколькими пинами, обозначенными заглавными буквами
        // Других разумных вариантов для такой фукции пина нет
        // PA/PB/PC/PD/PE/PF - это может быть либо hex цифра единственного 16ти-разрядного порта
        // Или это может быть обозначение пина латинской буквой A-Z единственного порта
        return true;
    }

    for(; b!=e; ++b)
    {
        if (!umba::parse_utils::isDigit(*b, 10)) // Если на позиции 2+ не цифра, то это какое-то странное обозначение для GPIO-пина, и мы его за таковой не считаем
            return false;
    }

    return true;

}

inline
bool isGpioPinFunction(const std::string &pinFn)
{
    return isGpioPinFunction(pinFn.begin(), pinFn.end());
}

//-----------------------------------------------------------------------------
//! Переносит GPIO-пин функцию на первую позицию вектора, остальное сортирует по алфавиту. Если обосрёмся и не найдём GPIO имя, или ложно найдём - ну, ничего и страшного, но в большинстве случаев должно прокатывать
template<typename IterType> inline
void sortPinFunctionsVector(IterType b, IterType e)
{
    for(IterType it = b; it!=e; ++it)
    {
        if(isGpioPinFunction(*it))
        {
            std::swap(*b, *it);
            ++b; // сортировка начинается не сначала - первой идёт GPIO-пин функция
            break;
        }
    }

    std::sort(b, e);
}

inline
void sortPinFunctionsVector(std::vector<std::string> &pinFunctionsVector)
{
    return sortPinFunctionsVector(pinFunctionsVector.begin(), pinFunctionsVector.end());
}



