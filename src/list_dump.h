#pragma once

#include "parse.h"
#include "logerr.h"


//-----------------------------------------------------------------------------
//template<typename StreamType>
inline
void dumpExpressionListSimpleShort( RoboconfOptions &rbcOpts, const char* msgId, const expression_list_t &lst )
{
    auto s = & LOG_MSG(msgId);

    bool useLines = lst.size() > 10;

    bool hasHeader = !lst.empty() && lst.front().isText();

    if (hasHeader)
        (*s)<<"(";
    for( auto it = lst.begin(); it != lst.end(); ++it )
    {
        if (it->isText())
        {

            (*s)<<it->itemText<<" ";
        }
        else
        {
            std::string head = it->getHeadText();
            if (head.empty())
            {
                (*s)<<"() ";
            }
            else
            {
                (*s)<<"("<<head<<" ...) ";
            }
        }
    }

    if (hasHeader)
        (*s)<<")";
    (*s)<<"\n";
}

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
                (*psa)<<"\n"; // "\n1"
                psa = & LOG_MSG(msgId);
                (*psa)<<curLevel;
            }

            if (!lvl)
            {
                (*psa)<<"\n"; // "\n2"
                psa = & LOG_MSG(msgId);
                (*psa)<<curLevel;
            }

            (*psa)<<"(";

            if (lstLvl>limitIndentLvl && i>0)
            {
                (*psa)<<"\n"; // "\n3"
                psa = & LOG_MSG(msgId);
                (*psa)<<curLevel<<levelIndent;
            }

            dumpExpressionList( rbcOpts, lvl+1, psa, msgId, it->itemList, levelIndent, curLevel + levelIndent /* lvl ? curLevel + levelIndent : curLevel */ );
            
            if (lstLvl>limitIndentLvl)
            {
                (*psa)<<"\n"; // "\n4"
                psa = & LOG_MSG(msgId);
                (*psa)<<curLevel;
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

//-----------------------------------------------------------------------------
inline
void dumpExpressionListSimpleShort( RoboconfOptions &rbcOpts, const char* msgId, const std::vector<expression_list_t*> &pLsts )
{
    for( auto p: pLsts )
    {
        dumpExpressionListSimpleShort( rbcOpts, msgId, *p );
    }
}

//-----------------------------------------------------------------------------
inline
void dumpExpressionList( RoboconfOptions &rbcOpts, const char* msgId, const char* msg, const std::vector<expression_list_t*> &pLsts)
{
    bool bFirst = true;
    for( auto p: pLsts )
    {
        auto ps = &LOG_MSG(msgId);

        if (bFirst)
        {
           (*ps)<<msg<<"\n";
           ps = &LOG_MSG(msgId);
           bFirst = false;
        }
        else
        {
            //ps = &LOG_MSG(msgId);
        }

        dumpExpressionList( rbcOpts, 0, *ps, msgId, *p );

        /*
        auto &sv = LOG_MSG(msgId); // <<msg; // "\n0"
        if (bFirst)
        {
            sv<<msg; // <<"\n";
            bFirst = false;
        }
        dumpExpressionList( rbcOpts, 0, sv, msgId, *p );
        */
    }
}




