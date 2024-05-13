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
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType & sa, const char* msgId, const expression_list_t &lst, const std::string & levelIndent, const std::string &curLevel )
{
    auto &sv = sa;
    for( auto it = lst.begin(); it != lst.end(); ++it )
    {
        if (it->isText())
        {
            dumpExpressionListItemText(rbcOpts, sv, it->itemText);
        }
        else
        {
            auto lstLvl = it->itemList.getMaxNestedListLevelEx();

            if (lstLvl>0)
                sa<<"\n"<<curLevel;

            sa<<"(";

            if (lstLvl>0)
                sa<<"\n"<<curLevel<<levelIndent;

            dumpExpressionList( rbcOpts, lvl+1, sa, msgId, it->itemList, levelIndent, curLevel + levelIndent /* lvl ? curLevel + levelIndent : curLevel */ );
            
            if (!it->itemList.isSimpleList())
                sa<<"\n"<<curLevel;
            sa<<")";
        }
    }
}

//-----------------------------------------------------------------------------
template<typename StreamType>
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType & sa, const char* msgId, const expression_list_t &lst)
{
    dumpExpressionList( rbcOpts, lvl, sa, msgId, lst, std::string("    "), std::string() );
}
