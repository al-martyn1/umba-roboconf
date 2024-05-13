#pragma once

#include "parse.h"
#include "logerr.h"


//-----------------------------------------------------------------------------
template<typename StreamType>
inline
void dumpExpressionListItemText( RoboconfOptions &rbcOpts, StreamType & s, std::string text, bool forceNoQuot = false )
{
    //unquoteTrimString(text);

    //char unquoteStringGetQuotChar( std::string &s )

    s<<text<<" ";
}

/*
template<typename StreamType>
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType & s, const expression_list_t &lst )
{
    //s<<"(";
    for( auto it = lst.begin(); it != lst.end(); ++it )
    {
        if (it->isText())
        {
            dumpExpressionListItemText(rbcOpts, s, it->itemText);
        }
        else
        {
            s<<"(";
            dumpExpressionList(rbcOpts, lvl, s, it->itemList);
            s<<")";
            s<<" ";
        }
    }
    //s<<")";
}
*/

//-----------------------------------------------------------------------------
/*
1) Список всегда начинается с открывающей скобки
2) После перевода строки всегда нужно вывести info(id)
3) Пока есть только текстовые ноды и односложные списки, выводим в одну строку
4)
*/
template<typename StreamType>
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType *psa, const char* msgId, const expression_list_t &lst, const std::string & levelIndent, const std::string &curLevel )
{
    const size_t limitIndentLvl = lvl ? 0 : 1;

    //auto &sv = sa;
    size_t i = 0;
    for( auto it = lst.begin(); it != lst.end(); ++it )
    {
        if (it->isText())
        {
            dumpExpressionListItemText(rbcOpts, *psa, it->itemText);
        }
        else
        {
            auto lstLvl = it->itemList.getMaxNestedListLevelEx();

            if (lstLvl>limitIndentLvl && lvl)
            {
                (*psa)<<"\n1"<<curLevel;
            }

            if (!lvl)
            {
                (*psa)<<"\n2"<<curLevel;
            }

            (*psa)<<"(";

            if (lstLvl>limitIndentLvl && i>0)
            {
                (*psa)<<"\n3"<<curLevel<<levelIndent;
            }

            dumpExpressionList( rbcOpts, lvl+1, psa, msgId, it->itemList, levelIndent, curLevel + levelIndent /* lvl ? curLevel + levelIndent : curLevel */ );
            
            if (lstLvl>limitIndentLvl)
            {
                (*psa)<<"\n4"<<curLevel;
            }

            (*psa)<<") ";
        }

        if (lvl)
            ++i;

    }
}

//-----------------------------------------------------------------------------
template<typename StreamType>
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType & sa, const char* msgId, const expression_list_t &lst)
{
    dumpExpressionList( rbcOpts, lvl, &sa, msgId, lst, std::string("    "), std::string() );
    sa<<"\n";
}
