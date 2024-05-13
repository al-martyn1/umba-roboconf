#pragma once

#include "parse.h"
#include "logerr.h"


//-----------------------------------------------------------------------------
template<typename StreamType>
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType & s, const expression_list_t &lst )
{
    //s<<"(";
    for( auto it = lst.begin(); it != lst.end(); ++it )
    {
        if (it->isText())
        {
            auto t = it->itemText;
            unquoteTrimString(t);
            s<<"\""<<t<<"\" ";
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

//    std::string                 itemText;
//    expression_list_t           itemList;

//-----------------------------------------------------------------------------
/*
1) Список всегда начинается с открывающей скобки
2) После перевода строки всегда нужно вывести info(id)
*/
template<typename StreamType>
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType & sa, const char* msgId, const expression_list_t &lst, const std::string & levelIndent, const std::string &curLevel )
{
    size_t lstNestedLevel = lst.getMaxNestedListLevel();
    if (lstNestedLevel>1 && !lst.isCommentList())
    {
        //auto &sv = lvl ? LOG_MSG(msgId) : sa;
        auto &sv = sa;
        //sv<<curLevel<<"(";
        //sa<<"(";
        for( auto it = lst.begin(); it != lst.end(); ++it )
        {
            if (it->isText())
            {
                auto t = it->itemText;
                unquoteTrimString(t);
                sa<<"\""<<t<<"\" ";
            }
            else
            {
                //sv<<curLevel<<"(\n";
                //sv<<"\n";
                //auto &sa = LOG_MSG(msgId)<<curLevel<<"(";

                size_t subLvl = it->itemList.getMaxNestedListLevel();

                if (subLvl>1 && !it->itemList.isCommentList())
                    sa<<"\n"<<curLevel;
                sa<<"(";

                dumpExpressionList( rbcOpts, lvl+1, sa, msgId, it->itemList, levelIndent, curLevel + levelIndent /* lvl ? curLevel + levelIndent : curLevel */ );
                
                if (subLvl>1 && !it->itemList.isCommentList())
                    sa<<"\n"<<curLevel;
                sa<<")";

                //sa<<"\n";
                //LOG_MSG(msgId);
            }
        }
        //sa<<curLevel<<")\n";
        //LOG_MSG(msgId);
    }
    else
    {
        //auto &sv = LOG_MSG(msgId);
        auto &sv = sa;
        for( auto it = lst.begin(); it != lst.end(); ++it )
        {
            if (it->isText())
            {
                auto t = it->itemText;
                unquoteTrimString(t);
                sv<<"\""<<t<<"\" ";
            }
            else
            {
                sv<<"(";
                dumpExpressionList( rbcOpts, lvl+1, sv, it->itemList );
                sv<<")";
                sv<<" ";
            }
        }
        //s<<")\n";
    }
}

//-----------------------------------------------------------------------------
template<typename StreamType>
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, size_t lvl, StreamType & sa, const char* msgId, const expression_list_t &lst)
{
    dumpExpressionList( rbcOpts, lvl, sa, msgId, lst, std::string("    "), std::string() );
}
