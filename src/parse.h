#pragma once

#include <string>
#include <vector>
#include <list>
#include <stack>
#include <set>
#include <stdexcept>

//
#include "config.h"
//

#include "rdlc-core/isa.h"
#include "rdlc-core/splits.h"
#include "rdlc-core/case.h"


#include "file_set.h"

#include "ioUtils.h"

#include "string_set_type.h"
#include "string_string_map_type.h"
#include "string_string_set_map_type.h"
#include "string_string_vector_map_type.h"


enum class ExpressionParsingResult
{
    skip    = -1,
    failed  =  0,
    success = 1

};


//-----------------------------------------------------------------------------
struct ExpressionItem
{
    //typedef std:: vector<ExpressionItem> expression_list_t;
    struct expression_list_t
    {
        std:: vector<ExpressionItem> items;
        FileSet::file_id_t          fileNo;
        size_t                      lineNo;

        ExpressionItem              *pParentItem = 0;



        typedef std:: vector<ExpressionItem>::iterator        iterator;
        typedef std:: vector<ExpressionItem>::const_iterator  const_iterator;
        typedef std:: vector<ExpressionItem>::reference       reference;
        typedef std:: vector<ExpressionItem>::const_reference const_reference;
        typedef std:: vector<ExpressionItem>::size_type       size_type;
        
        expression_list_t() : items(), fileNo((FileSet::file_id_t)-1), lineNo(0), pParentItem(0)
        {
            #if defined(ROBOCONF_EXPRESSION_LIST_RESERVE)
            items.reserve(16);
            #else
            #endif
        }

        expression_list_t(const expression_list_t &l) : items(l.items), fileNo(l.fileNo), lineNo(l.lineNo), pParentItem(l.pParentItem)
        {
        }

        expression_list_t& operator=( expression_list_t l )
        {
            l.swap(*this);
            return *this;
        }

        void clear() { items.clear(); }
        size_type size() const { return items.size(); }
        bool empty() const { return items.empty(); }

        #if defined(ROBOCONF_EXPRESSION_LIST_RESERVE)
        void reserve(std::size_t sz) { items.reserve(sz); }
        #else
        void reserve(std::size_t sz) { items.reserve(sz); }
        #endif

        iterator begin() { return items.begin(); }
        iterator end()   { return items.end(); }
        const_iterator begin() const { return items.begin(); }
        const_iterator end()   const { return items.end(); }

        reference front() { return items.front(); }
        reference back()  { return items.back(); }
        const_reference front() const { return items.front(); }
        const_reference back()  const { return items.back(); }

        void push_back( const ExpressionItem &i ) { items.push_back(i); }

        reference operator[]( size_type idx ) { return items.operator[](idx); }
        const_reference operator[]( size_type idx ) const { return items.operator[](idx); }

        void erase( iterator it ) { items.erase(it); }
        void swap(expression_list_t &l) throw() { items.swap(l.items); std::swap(fileNo, l.fileNo); std::swap(lineNo, l.lineNo); std::swap(pParentItem, l.pParentItem); }

        template<typename Iter>
        void insert( iterator pos, Iter b, Iter e ) { items.insert(pos, b, e); }

        void append_expression_list(const expression_list_t &exprList)
        {
            insert(items.end(), exprList.items.begin(), exprList.items.end());
        }
        


        size_t getMaxNestedListLevel() const
        {
            size_t nestedLevel = 0;

            for (auto it = begin(); it!=end(); ++it)
            {
                if (it->isText())
                    continue;

                size_t childLevel = it->getMaxNestedListLevel();
                if (nestedLevel<childLevel)
                    nestedLevel = childLevel;
            }

            return nestedLevel;
        }

        bool isCommentList() const
        {
            if (empty())
                return false;

            if (!front().isCommentTextItem())
                return false;

            return true;
        }

        bool isSimpleList() const
        {
            if (isCommentList())
                return true;
            return getMaxNestedListLevel()<2;
        }

        size_t getMaxNestedListLevelEx() const
        {
            if (isCommentList())
                return 0;
            return getMaxNestedListLevel();
        }

        void updateParentPtr( ExpressionItem *ppi )
        {
            pParentItem = ppi;

            for( auto &i : items)
            {
                i.updateParentPtr( this );
            }
        }

        void clearParentPtr( )
        {
            pParentItem = 0;

            for( auto &i : items)
            {
                i.clearParentPtr( );
            }
        }

        expression_list_t* getParentList() const
        {
            if (!pParentItem)
                return 0;

            return pParentItem->pParentList;
        }

/*
    void updateParentPtr( expression_list_t *ppl )
    {
        pParentList = ppl;
        if (isText())
            return;

        itemList.updateParentPtr( ppl ? this : (ExpressionItem*)0 );
    }

*/

    }; // struct expression_list_t



    std::string                 itemText;
    expression_list_t           itemList;

    FileSet::file_id_t          fileNo;
    size_t                      lineNo;

    expression_list_t           *pParentList = 0;


    bool isText() const
    {
        return !itemText.empty();
    }

    bool isList() const
    {
        return itemText.empty();
    }

    void updateParentPtr( expression_list_t *ppl )
    {
        pParentList = ppl;
        if (isText())
            return;

        itemList.updateParentPtr( this );
    }

    void clearParentPtr( )
    {
        pParentList = 0;
        if (isText())
            return;

        itemList.clearParentPtr( );
    }


    size_t getMaxNestedListLevel() const
    {
        if (isText())
            return 0;
        return itemList.getMaxNestedListLevel() + 1;
    }

    std::string getHeadText() const
    {
        if (isText())
            return itemText;
        if (itemList.empty())
            return std::string();
        return itemList.front().itemText; // getHeadText();
    }

    static
    bool isCommentPrefixed( const std::string &str )
    {
        if (startsWith( str, "--" ))
            return true;
        if (startsWith( str, "//" ))
            return true;
        return false;
    }

    bool isCommentTextItem() const
    {
        if (!isText())
            return false;

        return isCommentPrefixed(itemText);
    }

    bool isCommentList() const
    {
        if (isText())
            return false;

        if (itemList.empty())
            return false;

        return itemList.front().isCommentTextItem();
    }

    void removeCommentLists();


    ExpressionItem() : itemText(), itemList(), fileNo(0), lineNo(0) {}
    ExpressionItem( const std::string &str, FileSet::file_id_t fn, size_t ln ) : itemText(str), itemList(), fileNo(fn), lineNo(ln) {}
    ExpressionItem( const expression_list_t &lst, FileSet::file_id_t fn, size_t ln) : itemText(), itemList(lst), fileNo(fn), lineNo(ln) {}

};

typedef ExpressionItem::expression_list_t   expression_list_t;


#if defined(ROBOCONF_PARSE_READ_LIST_EXPRESSION_LIST_STACK_USE_UNDERLYING_VECTOR)
    using expression_list_stack_type = std:: stack< expression_list_t, std:: vector<expression_list_t> >;
#else
    using expression_list_stack_type = std:: stack< expression_list_t >;
#endif





//-----------------------------------------------------------------------------
template <typename Handler>
bool processExpressionList( expression_list_t &lst, bool bErase, const Handler &handler )
{
    bool totalRes = true;
    for( size_t i = 0; i!=lst.size(); )
    {
        ExpressionParsingResult res = handler( lst[i] );
        if (res==ExpressionParsingResult::skip)
        {
            ++i;
        }
        else
        {
            if (bErase)
                lst.erase(lst.begin()+i);
            else
                ++i;
            if (res!=ExpressionParsingResult::success)
                totalRes = false;
        }
    }

    return totalRes;
}


//std::stack< expression_list_t >
/*
template<
    class T,
    class Container = std::deque<T>
> class stack;
*/


//-----------------------------------------------------------------------------
inline
bool isCommentExpressionList( const expression_list_t &lst )
{
    if (lst.empty())
        return false;
    return lst[0].isCommentTextItem();
}

//-----------------------------------------------------------------------------
inline
void removeCommentExpressionLists( expression_list_t &lst )
{
    expression_list_t::size_type i = 0;
    for(; i < lst.size(); )
    {
        if (!lst[i].isList())
        {
            ++i;
            continue;
        }

        if ( isCommentExpressionList( lst[i].itemList ) )
        {
            lst.erase( lst.begin() + i );
        }
        else
        {
            lst[i].removeCommentLists();
            ++i;
        }
    }
}

//-----------------------------------------------------------------------------
inline
void ExpressionItem::removeCommentLists()
{
    if (!isList())
        return;
    removeCommentExpressionLists( itemList );
}

//-----------------------------------------------------------------------------
inline
bool readList(FileSet::file_id_t fileNo, size_t &lineNo, const std::string &txt, std::string::size_type &pos, expression_list_t &lst, expression_list_stack_type &listStack )
{
    char quotSign = 0;
    std::string expr;
    bool bReadComment = false;
    int  braceLevel = 0; // used to read comments
    //size_t 
    lineNo = 1;

    while( pos!=txt.size() )
    {
        char ch = txt[pos];
        if (ch == '\n')
            ++lineNo;

        if (bReadComment)
        {
            if (ch=='(')
            {
                braceLevel++;
                expr.append(1,ch);
            }
            else if (ch==')')
            {
                braceLevel--;
                if (braceLevel!=0)
                {
                    expr.append(1,ch);
                }
                else
                {
                    // comment end found

                    if (!expr.empty())
                    {
                        lst.push_back( ExpressionItem(expr, fileNo, lineNo) );
                        expr.clear();
                    }
                    
                    expression_list_t tmpLst = lst;
                    
                    if (!listStack.empty())
                    {
                        lst.swap(listStack.top());
                        listStack.pop();
                    }
                    else
                    {
                        lst.clear();
                    }
                   
                    lst.push_back(ExpressionItem(tmpLst, fileNo, lineNo));

                    bReadComment = false;
                }
            }
            else // generic char
            {
                expr.append(1,ch);
            }

            pos++;
            continue;
        }

        if (isSpace( ch ))
        {
            if (quotSign!=0)
            {
                expr.append(1,ch);
                if (quotSign==ch)
                    quotSign = 0; // reset quoting
            }
            else
            {
                // end of unquoted expr
                if (!expr.empty())
                {
                    if (lst.empty() && ExpressionItem::isCommentPrefixed(expr) )
                    {
                        bReadComment = true;
                        braceLevel = 1;
                        lst.push_back( ExpressionItem(expr, fileNo, lineNo) );
                        expr.clear();
                        //expr.append(1, ch);
                    }
                    else
                    {
                        lst.push_back( ExpressionItem(expr, fileNo, lineNo) );
                        expr.clear();
                    }
                }
            }
            pos++;
            continue;
        }

        if (ch=='\'' || ch=='\"')
        {
            
            if (quotSign==ch)
            {
                // in quot
                quotSign = 0; // end of quoted string
            }
            else
            {
                // start quoting
                if (!expr.empty())
                {
                    lst.push_back( ExpressionItem(expr, fileNo, lineNo) );
                    expr.clear();
                }

                quotSign = ch;

            }

            expr.append(1,ch);
            pos++;
            continue;
        }

        if (ch=='(' && quotSign==0)
        {
            if (!expr.empty())
            {
                lst.push_back( ExpressionItem(expr, fileNo, lineNo) );
                expr.clear();
            }

            listStack.push(lst);
            lst.clear();
            lst.fileNo = fileNo;
            lst.lineNo = lineNo;

            pos++;
            continue;

        }

        if (ch==')' && quotSign==0)
        {
            if (!expr.empty())
            {
                lst.push_back( ExpressionItem(expr, fileNo, lineNo) );
                expr.clear();
            }

            expression_list_t tmpLst = lst;
            
            if (!listStack.empty())
            {
                lst.swap(listStack.top());
                listStack.pop();
            }
            else
            {
                lst.clear();
                lst.fileNo = fileNo;
                lst.lineNo = lineNo;
            }

            lst.push_back(ExpressionItem(tmpLst, fileNo, lineNo));

            pos++;
            continue;

        }

        expr.append(1,ch);
        pos++;
        continue;

    } // while( pos!=txt.size() )


    if (!expr.empty())
    {
        lst.push_back( ExpressionItem(expr, fileNo, lineNo) );
        expr.clear();
    }

    // close all opened lists (closing bracket forgotten)
    while(!listStack.empty())
    {
        expression_list_t tmpLst = lst;
        lst.swap(listStack.top());
        listStack.pop();
        lst.push_back(ExpressionItem(tmpLst, fileNo, lineNo));
    }

    return true;
}

//-----------------------------------------------------------------------------
inline
bool readList(FileSet::file_id_t fileNo, size_t &lineNo, const std::string &txt, expression_list_t &lst )
{
    expression_list_stack_type listStack;
    std::string::size_type pos = 0;
    return readList(fileNo, lineNo, txt, pos, lst, listStack );
}

//-----------------------------------------------------------------------------
inline
bool readList(FileSet::file_id_t fileNo, size_t &lineNo, std::istream &in, expression_list_t &lst )
{
    std::string allText = readFileEncoded( in );
/*
    allText.reserve( 10000 );
    std::string str;
    while( std::getline( in, str) )
    {
        if (!allText.empty())
            allText.append("\r\n");                
        allText.append(str);
    }
*/
    return readList(fileNo, lineNo, allText, lst );

}

//-----------------------------------------------------------------------------
inline
bool
readListTextItem( const expression_list_t &lst, expression_list_t::const_iterator &it, std::string &rdTo, bool bUnquote = false /* for compatibility */ )
{
    if (it==lst.end())
        return false;
    if (!it->isText())
        return false;
    rdTo = it->itemText;
    if (bUnquote)
        unquoteTrimString( rdTo );
    ++it;
    return true;
}

//-----------------------------------------------------------------------------
inline
size_t readListToVector( const expression_list_t &lst, std:: vector< std::string > &v, bool bUnquote = true )
{
    expression_list_t::const_iterator it = lst.begin();
    std::string str;
    while( readListTextItem(lst, it, str) )
    {
        if (bUnquote)
            unquoteTrimString( str );
        v.push_back(str);
    }
    return v.size();
}

/*
Шаблон - строка:
L - list, пропускаем, ошибка, если не список
T - token, что-то без кавычек
V - value, какое-то значение, в кавычках или без. Если список, то ошибка
S - что-то, список или строка

{} - группа (или круглые скобки?)
Группы пока не обрабатываем.

Для каждого элемента T надо задать список возможных значений
T:a,b,c;
Ti:a,b,c; - игнорируем регистр (ignore case)
Ts:a,b,c; - сравнение с учетом регистра (case sensitive)
V0 (V-zero) - допустимо пустое значение

На выходе - получаем вектор строк:
вместо L - пустая строка (или "<list>"?)
вместо S - или значение само, или значение первого элемента списка
S'  - не надо сделать unquote

Возвращаем bool как признак успеха.
При ошибке 1) expected - что ожидалось
           2) found - что найдено
*/

//-----------------------------------------------------------------------------
inline
std::string makeReadableStringFromSet( const std::set< std::string > &keywords, const std::string &appendStr, const std::string &defStr = std::string() )
{
    
    if (keywords.empty())
    {
        if (defStr.empty())
            return appendStr;
        return defStr;
    }

    std::string res;
    size_t i = 0, s = keywords.size();
    for( auto k : keywords )
    {
        if (i)
        {
            if ((i+1)==s)
                res.append(" or ");
            else
                res.append(", ");
        }

        res.append("'");
        res.append(k);
        res.append("'");
        ++i;
    }

    return res + std::string(" ") + appendStr;
}

//-----------------------------------------------------------------------------
inline
std::string makeReadableStringFromSet( const std::map< std::string, int > &keywords, const std::string &appendStr, const std::string &defStr = std::string() )
{
    
    if (keywords.empty())
    {
        if (defStr.empty())
            return appendStr;
        return defStr;
    }

    std::string res;
    size_t i = 0, s = keywords.size();
    for( auto kp : keywords )
    {
        if (i)
        {
            if ((i+1)==s)
                res.append(" or ");
            else
                res.append(", ");
        }

        res.append("'");
        res.append(kp.first);
        res.append("'");
        ++i;
    }

    return res + std::string(" ") + appendStr;
}

//-----------------------------------------------------------------------------
//template< typename InsertIter >
inline
bool readListByTemplate( const std::string &tpl
                       , expression_list_t::const_iterator &it
                       , const expression_list_t::const_iterator E
                       , std:: vector< std::string > &readTo
                       , std::string &expected
                       , std::string &found
                       )
{
    size_t pos = 0, size = tpl.size();

    enum EntryType
    {
        EntryType_Unknown,
        EntryType_List,
        EntryType_Token,
        EntryType_Value,
        EntryType_ValueOrValueList,
        EntryType_Something
    };


    while(pos!=size)
    {
        EntryType entryType     = EntryType_Unknown;
        bool caseSensitive      = false;
        std::set< std::string > keywords;
        bool                    zeroFlag    = false;
        bool                    dontUnquoteFlag = false;
        bool                    goodStopOnUnknownKeyword = false;
        bool                    itemSeparator = false;

        switch(tpl[pos])
        {
            case 'L':
                 entryType   = EntryType_List;
                 break;
            case 'T':
                 entryType   = EntryType_Token;
                 break;
            case 'V':
                 entryType   = EntryType_Value;
                 break;
            case 'S':
                 entryType   = EntryType_Something;
                 break;
            case 'A':
                 entryType   = EntryType_ValueOrValueList;
                 break;
            case ';':
                 itemSeparator = true;
                 break;

            default:
                 throw std::runtime_error("List Parser: Invalid token in template string");
        }

        ++pos;
        if (itemSeparator)
            continue;

        /*
        T:a,b,c;
        T!:a,b,c;
        Ti:a,b,c; - игнорируем регистр (ignore case)
        Ts:a,b,c; - сравнение с учетом регистра (case sensitive)
        */
        
        if (entryType == EntryType_Token)
        {
            bool readTokens = false;
            while(pos!=size && (tpl[pos]==':' || tpl[pos]=='i' || tpl[pos]=='s' || tpl[pos]=='!'))
            {
                if (tpl[pos]==':')
                {
                    readTokens = true;
                    ++pos;
                    break;
                }
                else if (tpl[pos]=='s')
                    caseSensitive = true;
                else if (tpl[pos]=='i')
                    caseSensitive = false;
                else if (tpl[pos]=='!')
                    goodStopOnUnknownKeyword = true;
                ++pos;
            }

            std::string tokenBuf;
            while(pos!=size && readTokens)
            {
                if (tpl[pos]==',' || tpl[pos]==';')
                {
                    if (!tokenBuf.empty())
                    {
                        if (caseSensitive)
                            keywords.insert(tokenBuf);
                        else
                            keywords.insert(toLower(tokenBuf));

                        tokenBuf.clear();
                    }

                    if (tpl[pos]==';')
                        readTokens = false;
                }
                else
                {
                    tokenBuf.append(1, tpl[pos]);
                }

                ++pos;
            }

            if (!tokenBuf.empty())
            {
                if (caseSensitive)
                    keywords.insert(tokenBuf);
                else
                    keywords.insert(toLower(tokenBuf));

                tokenBuf.clear();
            }

            if (it==E)
            {
                expected = makeReadableStringFromSet( keywords, "keyword" );
                found    = "end of list";
                return false;
            }

            if (it->isList())
            {
                expected = makeReadableStringFromSet( keywords, "keyword" );
                found    = "nested list";
                return false;
            }

            std::string txt = caseSensitive ? it->itemText : toLower(it->itemText);
            if (!keywords.empty())
            {
                if (keywords.find(txt)==keywords.end())
                {
                    if (goodStopOnUnknownKeyword)
                    {
                        readTo.emplace_back(txt);
                        return true;
                    }
                    expected = makeReadableStringFromSet( keywords, "keyword" );
                    found    = std::string("'") + it->itemText + std::string("'");
                    return false;
                }
            }
            
            readTo.emplace_back(txt);

        } // if (entryType == EntryType_Token)

        // L - list, пропускаем, ошибка, если не список

        else if (entryType == EntryType_List)
        {
            if (it==E)
            {
                expected = "nested list";
                found    = "end of list";
                return false;
            }

            if (!it->isList())
            {
                expected = "nested list";
                found    = std::string("'") + it->itemText + std::string("'");
                return false;
            }

            readTo.emplace_back("<list>");
        
        }

        // V - value, какое-то значение, в кавычках или без. Если список, то ошибка
        // V0 (V-zero) - допустимо пустое значение
        else if (entryType == EntryType_Value)
        {
            if (pos!=size && tpl[pos]=='0')
            {
                ++pos;
                zeroFlag = true;
            }

            if (it==E)
            {
                expected = "value";
                found    = "end of list";
                return false;
            }

            if (it->isList())
            {
                expected = "value";
                found    = "nested list";
                return false;
            }

            std::string str = it->itemText;
            unquoteTrimString( str );
            if (str.empty())
            {
                if (!zeroFlag)
                {
                    expected = "value";
                    found    = "empty value";
                    return false;
                }
            }

            readTo.emplace_back(str);

        }

        // S - что-то, список или строка
        // S'  - не надо сделать unquote
        else if (entryType == EntryType_Something)
        {
            if (pos!=size && tpl[pos]=='\'')
            {
                ++pos;
                dontUnquoteFlag = true;
            }

            if (it==E)
            {
                expected = "value or nested list";
                found    = "end of list";
                return false;
            }

            std::string str = it->itemText;
            if (it->isList())
            {
                const expression_list_t &subLst = it->itemList;
                if (subLst.empty())
                    str.clear();
                else if (subLst[0].isText())
                    str = subLst[0].itemText;
                else
                {
                    expected = "value or nested list";
                    found    = "nested nested list";
                    return false;
                }

            }

            if (!dontUnquoteFlag)
               unquoteTrimString( str );

            readTo.emplace_back(str);

        }

        // A - прочитать value или список value
        else if (entryType == EntryType_ValueOrValueList)
        {
            bool allowNoValuesAtAll = false;
            while(pos!=size && (tpl[pos]=='0' || tpl[pos]=='\'' || tpl[pos]=='*'))
            {
                if (tpl[pos]=='0')
                    zeroFlag = true;
                else if (tpl[pos]=='*')
                    allowNoValuesAtAll = true;
                else
                    dontUnquoteFlag = true;
                ++pos;
            }

            if (it==E)
            {
                if (allowNoValuesAtAll)
                    return true;

                expected = "value or nested list";
                found    = "end of list";
                return false;
            }

            std:: vector< std::string  > values;

            if (it->isList())
            {
                const expression_list_t &subLst = it->itemList;
                expression_list_t::const_iterator subIt = subLst.begin();
                for(; subIt != subLst.end(); ++subIt)
                {
                    if (!subIt->isText())
                    {
                        expected = "value";
                        found    = "nested list";
                        return false;
                    }

                    std::string str = subIt->itemText;
                    unquoteTrimString( str );
                    if (str.empty())
                    {
                        if (!zeroFlag)
                        {
                            expected = "value";
                            found    = "empty value";
                            return false;
                        }
                    }
                   
                    values.emplace_back(str);
                }
            }
            else
            {
                std::string str = it->itemText;
                unquoteTrimString( str );
                if (str.empty())
                {
                    if (!zeroFlag)
                    {
                        expected = "value";
                        found    = "empty value";
                        return false;
                    }
                }

                values.emplace_back(str);
            }

            if (values.empty())
            {
                if (!allowNoValuesAtAll)
                    return false;
            }
            
            readTo.insert( readTo.end(), values.begin(), values.end() );

            ++it;

            return true;
        }

        ++it;
    
    }

    return true;
}


//-----------------------------------------------------------------------------
//const char* ExpressionParsingRule_parseFlags( const char *pFlags, unsigned &flags, const char *pFlagsAllowed = 0 );
// ExpressionParsingRuleFlags::icase_flag
struct ExpressionParsingRuleFlags
{
    static const unsigned number_mask            = 0x0000FFFF;
    static const unsigned number_flag            = 0x00010000;
    static const unsigned one_more_item_flag     = 0x00020000;
    static const unsigned optional_item_flag     = 0x00040000;
    static const unsigned any_number_items_flag  = 0x00080000;
    static const unsigned empty_allowed_flag     = 0x00100000;
    static const unsigned icase_flag             = 0x00200000;
    static const unsigned good_stop_flag         = 0x00400000; // stop on wrong token without error
    static const unsigned stops_on_eol_only      = 0x00800000; // stop on End-Of-List only, not on any other events
    //static const unsigned number_flag = 0x00010000;
};

//-----------------------------------------------------------------------------
enum class ExpressionParsingRuleType
{
    token,             // T
    value,             // V
    valueOrValueList,  // A
    valueList,         // L
    innerList,         // I
    valueMap,          // M
    vectorVector       // E
};    

//-----------------------------------------------------------------------------
struct ExpressionParsingResultItem //-V730
{
    FileSet::file_id_t             fileNo = (FileSet::file_id_t)-1;
    size_t                         lineNo = 0;

    ExpressionParsingRuleType      ruleType;

    unsigned                       ruleFlags;

    // T
    std::map< std::string, int >   tokenIds;
    std::map< int, std::string >   tokenNames;
    int                            tokenId;
    std::string                    token;
    std::set<std::string>          uncheckedTokens;

    // V
    std::string                                 singleValue;

    // A, L, I
    std:: vector<std::string>                   vectorValue;

    // E
    std:: vector< std:: vector<std::string> >   vectorOfVectorsValue;    // vector of vectors
    
    // M
    string_string_vector_map_type               mapOfVectorsValue;    // map of vectors

};

//-----------------------------------------------------------------------------
inline
bool ExpressionParsingRule_parseFlagsCheckAllowed( char ch, const char *pFlagsAllowed = 0 )
{
    if (!pFlagsAllowed)
        return true; // if no explicitly taken allowed list, allow all

    while(*pFlagsAllowed)
    {
        if (*pFlagsAllowed==ch)
            return true;
        ++pFlagsAllowed;
    }

    return false;
}

//-----------------------------------------------------------------------------
inline
const char* ExpressionParsingRule_parseFlags( char chOption, const char *pFlags, unsigned &flags, const char *pFlagsAllowed, const char* srcFile, int srcLine )
{
    flags = 0;

    unsigned readedNumItemsVal = 0;
    bool readingNumber = false;
    bool readedNumber  = false;

    //if (*pFlags == ':')
    //    pFlags++;

    while(*pFlags)
    {
        char charToCheck = *pFlags;
        if (charToCheck>='0' && charToCheck<='9')
            charToCheck = '#';

        if (*pFlags!=';' && *pFlags!=':' && !ExpressionParsingRule_parseFlagsCheckAllowed( charToCheck, pFlagsAllowed ))
        {
            std::ostringstream ss;
            ss<<"List parsing template: failed at "<<srcFile<<":"<<srcLine<<": found wrong compile-time option '"<<chOption<<"' flag: '"<<std::string(1, *pFlags)<<"'"; // <<umba::omanip::endl;
            //throw std::runtime_error("Expression parsing template - found not explicitly allowed option");
            throw std::runtime_error(ss.str().c_str());
        }

        switch(*pFlags)
        {
            case ':':
            case ';':

                      if (readingNumber || readedNumber)
                      {
                          flags |= (readedNumItemsVal&ExpressionParsingRuleFlags::number_mask) | ExpressionParsingRuleFlags::number_flag;
                      }

                      return pFlags;

            case '+': flags |= ExpressionParsingRuleFlags::one_more_item_flag;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case '?': flags |= ExpressionParsingRuleFlags::optional_item_flag;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case '*': flags |= ExpressionParsingRuleFlags::any_number_items_flag;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case '_': flags |= ExpressionParsingRuleFlags::empty_allowed_flag;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case 'i': flags |= ExpressionParsingRuleFlags::icase_flag;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case 's': flags &= ~ExpressionParsingRuleFlags::icase_flag;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case '!': flags |= ExpressionParsingRuleFlags::good_stop_flag;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case '$': flags |= ExpressionParsingRuleFlags::stops_on_eol_only;
                      if (readingNumber)
                      {
                           readingNumber = false;
                           readedNumber  = true;
                      }
                      break;

            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                      if (readedNumber)
                          throw std::runtime_error("Expression parsing template - number taken multiple times");
                      readingNumber = true;

                      readedNumItemsVal *= 10;
                      readedNumItemsVal += (unsigned)(*pFlags-'0');
                      break;

            default:
                      throw std::runtime_error("Expression parsing template - unknown option char");
        }

        ++pFlags;
    }

    // flags = 0; // схуяли я тут обнуляю?
    if (readingNumber || readedNumber)
    {
        flags |= (readedNumItemsVal&ExpressionParsingRuleFlags::number_mask) | ExpressionParsingRuleFlags::number_flag;
    }

    return pFlags;
}

//-----------------------------------------------------------------------------
#define ROBOCONF_PARSE_HELPER_PARSE_RULE_PARSE_FLAGS(chOpt, pFlags, flags, pFlagsAllowed) \
                       ExpressionParsingRule_parseFlags( chOpt, pFlags, flags, pFlagsAllowed, __FILE__, __LINE__ )

#define ROBOCONF_PARSE_HELPER_PARSE_RULE_READ_TOKENS( pTpl, item ) \
                       ExpressionParsingRule_readTokens( pTpl, item, __FILE__, __LINE__ )

//-----------------------------------------------------------------------------
inline
const char * ExpressionParsingRule_readTokens( const char *pTpl, ExpressionParsingResultItem &item, const char* srcFile, int srcLine )
{
    std::string tokenText;
    int         tokenId = 0;

    if (!pTpl)
        return pTpl;

    if (*pTpl==':')
        pTpl++;

    while(*pTpl && *pTpl!=';')
    {
        if (*pTpl==',' || *pTpl==':')
        {
            trim(tokenText);

            if (tokenText.empty())
               continue;

            if (item.ruleFlags&ExpressionParsingRuleFlags::icase_flag)
                tokenText = toLower(tokenText);

            if (tokenText[0]=='-')
            {
                tokenText.erase(0,1);
                if (tokenText.empty())
                    continue;
                item.uncheckedTokens.insert(tokenText);
            }

            if (tokenText.empty())
               continue;

            item.tokenIds[tokenText] = tokenId;
            item.tokenNames[tokenId] = tokenText;
            tokenText.clear();
            ++tokenId;
        }
        else
        {
            tokenText.append(1, *pTpl);
        }
        pTpl++;
    }

    trim(tokenText);
    if (tokenText.empty())
    {
        return pTpl;
    }

    if (item.ruleFlags&ExpressionParsingRuleFlags::icase_flag)
        tokenText = toLower(tokenText);

    if (tokenText[0]=='-')
    {
        tokenText.erase(0,1);
        if (tokenText.empty())
            return pTpl;
        item.uncheckedTokens.insert(tokenText);
    }

    if (tokenText.empty())
    {
        return pTpl;
    }

    item.tokenIds[tokenText] = tokenId;
    item.tokenNames[tokenId] = tokenText;

    return pTpl;
}

/*
struct ExpressionParsingResultItem
{
    ExpressionParsingRuleType  ruleType;

    // T
    std::map< std::string, unsigned > tokenIds;
    std::map< unsigned, std::string > tokenNames;
    unsigned                          tokenId;
*/

//ExpressionParsingResultItem

//-----------------------------------------------------------------------------
//#define ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE_DCLARE_RESULT()  ExpressionParsingResult

#define ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( pTpl, it, E, readTo, expected, found ) \
                       readListRes = readListByTemplate( pTpl, it, E, readTo, expected, found, __FILE__, __LINE__ )


#define ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET( msg )                            \
    do {                                                                              \
    if (readListRes==ExpressionParsingResult::failed)                                 \
    {                                                                                 \
        LOG_ERR<<msg<<": expected "<<expected<<", but found "<<found<<"\n";           \
        return readListRes;                                                           \
    }                                                                                 \
    } while(false)

#define ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ( obj, msg )                   \
    do {                                                                              \
    if (readListRes==ExpressionParsingResult::failed)                                 \
    {                                                                                 \
        LOG_ERR_OBJ(obj)<<msg<<": expected "<<expected<<", but found "<<found<<"\n";  \
        return readListRes;                                                           \
    }                                                                                 \
    } while(false)




#define ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_CUSTOM( msg, resVal )             \
    do {                                                                              \
    if (readListRes==ExpressionParsingResult::failed)                                 \
    {                                                                                 \
        LOG_ERR<<msg<<": expected "<<expected<<", but found "<<found<<"\n";           \
        return resVal;                                                                \
    }                                                                                 \
    } while(false)

#define ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_CUSTOM_OBJ( obj, msg, resVal )    \
    do {                                                                              \
    if (readListRes==ExpressionParsingResult::failed)                                 \
    {                                                                                 \
        LOG_ERR_OBJ(obj)<<msg<<": expected "<<expected<<", but found "<<found<<"\n";  \
        return resVal;                                                                \
    }                                                                                 \
    } while(false)

#define ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_FALSE( msg )                      \
                 ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_CUSTOM( msg, false)

#define ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_FALSE_OBJ( obj, msg )             \
                 ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_CUSTOM_OBJ( obj, msg, false)



#define ROBOCONF_CHECK_PARSE_READ_RESULT_RET_SKIP()  \
    do {                                             \
    if (readListRes==ExpressionParsingResult::skip)  \
        return readListRes;                          \
    } while(false)


#define ROBOCONF_CHECK_PARSE_READ_LIST_NOT_SUCCESS_CONTINUE( msg )                    \
    if (readListRes==ExpressionParsingResult::failed)                                 \
        LOG_ERR<<msg<<": expected "<<expected<<", but found "<<found<<"\n";           \
    if (readListRes!=ExpressionParsingResult::success)                                \
        continue

#define ROBOCONF_CHECK_PARSE_READ_LIST_NOT_SUCCESS_CONTINUE_OBJ( obj, msg )           \
    if (readListRes==ExpressionParsingResult::failed)                                 \
        LOG_ERR_OBJ(obj)<<msg<<": expected "<<expected<<", but found "<<found<<"\n";  \
    if (readListRes!=ExpressionParsingResult::success)                                \
        continue



//-----------------------------------------------------------------------------
inline
ExpressionParsingResult readListByTemplate( const char *pTpl
                       , expression_list_t::const_iterator &it
                       , const expression_list_t::const_iterator E
                       , std:: vector< ExpressionParsingResultItem > &readTo
                       , std::string &expected
                       , std::string &found
                       , const char* file
                       , int line
                       )
{
    readTo.clear();

    while(*pTpl)
    {
        ExpressionParsingResultItem resultItem;
        bool                        tplItemSeparator = false;
        UMBA_USED(tplItemSeparator);

        if (*pTpl==';')
            ++pTpl;

        if (it!=E)
        {
            resultItem.fileNo = it->fileNo;
            resultItem.lineNo = it->lineNo;
        }

        bool skipIncrement = false;

        switch(*pTpl++)
        {
            case 'T': resultItem.ruleType = ExpressionParsingRuleType::token; // !!!
                      {
                          pTpl = ExpressionParsingRule_parseFlags( 'T', pTpl, resultItem.ruleFlags, "si!", file, line );
                          pTpl = ExpressionParsingRule_readTokens( pTpl, resultItem, file, line);

                          if (it==E)
                          {
                              expected = makeReadableStringFromSet( resultItem.tokenIds, "keyword" );
                              found    = "end of list";
                              return ExpressionParsingResult::failed;
                          }
                         
                          if (it->isList())
                          {
                              expected = makeReadableStringFromSet( resultItem.tokenIds, "keyword" );
                              found    = "nested list";
                              return ExpressionParsingResult::failed;
                          }
                         
                          resultItem.token   = (resultItem.ruleFlags&ExpressionParsingRuleFlags::icase_flag) ? toLower(it->itemText) : it->itemText;
                          resultItem.tokenId = -1;

                          if (resultItem.tokenIds.empty())
                          {
                              readTo.push_back(resultItem);
                              // allowed tokens set not found
                              //return true;
                          }
                          else
                          {
                              std::map< std::string, int >::const_iterator tit = resultItem.tokenIds.find(resultItem.token);
                              
                              if (tit == resultItem.tokenIds.end())
                              {
                                  // token not found
                                  readTo.push_back(resultItem);
                                  if (resultItem.ruleFlags&ExpressionParsingRuleFlags::good_stop_flag)
                                  {
                                      ++it;
                                      return ExpressionParsingResult::skip;
                                  }
                                  expected = makeReadableStringFromSet( resultItem.tokenIds, "keyword" );
                                  found    = std::string("'") + it->itemText + std::string("'");
                                  return ExpressionParsingResult::failed;
                              }
                              else
                              {
                                  resultItem.tokenId = tit->second;
                                  readTo.push_back(resultItem);
                              }
                          }
                      }
                      break;

            case 'V': resultItem.ruleType = ExpressionParsingRuleType::value; // !!!
                      {
                          pTpl = ExpressionParsingRule_parseFlags( 'V', pTpl, resultItem.ruleFlags, "_?$!", file, line );
                          if (it->isList())
                          {
                              if ((resultItem.ruleFlags&ExpressionParsingRuleFlags::optional_item_flag) == 0)
                              {
                                  expected = "value";
                                  found    = "nested list";
                                  return ExpressionParsingResult::failed;
                              }

                              if (resultItem.ruleFlags&ExpressionParsingRuleFlags::stops_on_eol_only)
                              {
                                  expected = "value or end of list";
                                  found    = "nested list";
                                  return ExpressionParsingResult::failed;
                              }

                              if (resultItem.ruleFlags&ExpressionParsingRuleFlags::good_stop_flag)
                              {
                                  return ExpressionParsingResult::success;
                              }

                              readTo.push_back(resultItem);

                              skipIncrement = true;
                          }
                          else if (it==E)
                          {
                              if (resultItem.ruleFlags&ExpressionParsingRuleFlags::optional_item_flag)
                              {
                                  readTo.push_back(resultItem);
                                  skipIncrement = true;
                              }
                              else
                              {
                                  expected = "value";
                                  found    = "end of list";
                                  return ExpressionParsingResult::failed;
                              }
                          }
                          else
                          {
                              std::string str = it->itemText;
                              unquoteTrimString( str );
                              if (str.empty() && (resultItem.ruleFlags&ExpressionParsingRuleFlags::empty_allowed_flag)==0)
                              {
                                  expected = "value";
                                  found    = "empty value";
                                  return ExpressionParsingResult::failed;
                              }
                             
                              resultItem.singleValue = str;
                              readTo.push_back(resultItem);
                          }
                      }
                      break;

            case 'A': resultItem.ruleType = ExpressionParsingRuleType::valueOrValueList; // !!!
                      {
                          // A_+*
                          // + - 1 или более, one_more_item,
                          // _ - допускаются пустые значения
                          // optional_item_flag ???

                          pTpl = ExpressionParsingRule_parseFlags( 'A', pTpl, resultItem.ruleFlags, "_+$", file, line );
                          if (it==E)
                          {
                              if (resultItem.ruleFlags&ExpressionParsingRuleFlags::one_more_item_flag)
                              {
                                  expected = "value or nested list";
                                  found    = "end of list";
                                  return ExpressionParsingResult::failed;
                              }

                              readTo.push_back(resultItem); // push with empty vector
                              return ExpressionParsingResult::success;
                          }
                          else if (!it->isList())
                          {
                              std::string str = it->itemText;
                              unquoteTrimString( str );
                              if (str.empty() && (resultItem.ruleFlags&ExpressionParsingRuleFlags::empty_allowed_flag)==0)
                              {
                                  expected = "value";
                                  found    = "empty value";
                                  return ExpressionParsingResult::failed;
                              }

                              resultItem.vectorValue.push_back(str);
                              readTo.push_back(resultItem);
                          }
                          else // sublist
                          {
                              const expression_list_t &subLst = it->itemList;
                              expression_list_t::const_iterator subIt = subLst.begin();
                              for(; subIt != subLst.end(); ++subIt)
                              {
                                  if (!subIt->isText())
                                  {
                                      expected = "value in nested list";
                                      found    = "nested nested list";
                                      return ExpressionParsingResult::failed;
                                  }
                             
                                  std::string str = subIt->itemText;
                                  unquoteTrimString( str );
                                  if (str.empty())
                                  {
                                      if ((resultItem.ruleFlags&ExpressionParsingRuleFlags::empty_allowed_flag)==0)
                                      {
                                          expected = "value in nested list";
                                          found    = "empty value in nested list";
                                          return ExpressionParsingResult::failed;
                                      }
                                  }
                                 
                                  resultItem.vectorValue.push_back(str);
                              }

                              if (resultItem.vectorValue.empty() && (resultItem.ruleFlags&ExpressionParsingRuleFlags::one_more_item_flag)==0)
                              {
                                  expected = "nested list";
                                  found    = "empty nested list";
                                  return ExpressionParsingResult::failed;
                              }

                              readTo.push_back(resultItem);
                          }

                          if (resultItem.ruleFlags&ExpressionParsingRuleFlags::stops_on_eol_only)
                          {
                              ++it;
                              if (it!=E)
                              {
                                  expected = "end of list";
                                  found    = it->isList() ? "nested list" : "value";
                                  return ExpressionParsingResult::failed;
                              }

                              --it;
                          }
                      }
                      break;

                      // L   - ������, ��������� �������� ���������� � ������ (�� �������� ���������).
                      // ������� �� ����� �������� ������ ��� ���� ����������� �������� ������.
            case 'L': resultItem.ruleType = ExpressionParsingRuleType::valueList;
                      {
                      }
                      break;


                      // I   - ������, ��������� �������� ���������� � ������ (������ ��������� ���������).
            case 'I': resultItem.ruleType = ExpressionParsingRuleType::innerList;
                      {
                          pTpl = ExpressionParsingRule_parseFlags( 'I', pTpl, resultItem.ruleFlags, "_+", file, line );
                          if (it==E)
                          {
                              if (resultItem.ruleFlags&ExpressionParsingRuleFlags::one_more_item_flag)
                              {
                                  expected = "nested list";
                                  found    = "end of list";
                                  return ExpressionParsingResult::failed;
                              }

                              readTo.push_back(resultItem); // push with empty vector
                              return ExpressionParsingResult::success;
                          }
                          else if (!it->isList())
                          {
                              std::string str = it->itemText;
                              unquoteTrimString( str );
                              //if (str.empty() && (resultItem.ruleFlags&ExpressionParsingRuleFlags::empty_allowed_flag)==0)
                              expected = "nested list";
                              found    = "empty value";
                              return ExpressionParsingResult::failed;
                          }
                          else // sublist
                          {
                              const expression_list_t &subLst = it->itemList;
                              expression_list_t::const_iterator subIt = subLst.begin();
                              for(; subIt != subLst.end(); ++subIt)
                              {
                                  if (!subIt->isText())
                                  {
                                      expected = "value in nested list";
                                      found    = "nested nested list";
                                      return ExpressionParsingResult::failed;
                                  }
                             
                                  std::string str = subIt->itemText;
                                  unquoteTrimString( str );
                                  if (str.empty())
                                  {
                                      if ((resultItem.ruleFlags&ExpressionParsingRuleFlags::empty_allowed_flag)==0)
                                      {
                                          expected = "value in nested list";
                                          found    = "empty value in nested list";
                                          return ExpressionParsingResult::failed;
                                      }
                                  }
                                 
                                  resultItem.vectorValue.push_back(str);
                              }

                              if (resultItem.vectorValue.empty() && (resultItem.ruleFlags&ExpressionParsingRuleFlags::one_more_item_flag)==0)
                              {
                                  expected = "nested list";
                                  found    = "empty nested list";
                                  return ExpressionParsingResult::failed;
                              }

                              readTo.push_back(resultItem);
                          }

                      }
                      break;

            case 'M': resultItem.ruleType = ExpressionParsingRuleType::valueMap;
                      {
                          pTpl = ExpressionParsingRule_parseFlags( 'M', pTpl, resultItem.ruleFlags, "si#$+", file, line );
                          pTpl = ExpressionParsingRule_readTokens( pTpl, resultItem, file, line);

                          while(it!=E && it->isList())
                          {
                              const expression_list_t &subLst = it->itemList;
                              expression_list_t::const_iterator subIt = subLst.begin();
                              std:: vector<std::string> vec; vec.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

                              for(; subIt != subLst.end(); ++subIt)
                              {
                                  if (!subIt->isText())
                                  {
                                      if (resultItem.uncheckedTokens.find("()")!=resultItem.uncheckedTokens.end())
                                      {
                                          vec.push_back("()");
                                      }
                                      else
                                      {
                                          expected = "value in nested list";
                                          found    = "nested nested list";
                                          return ExpressionParsingResult::failed;
                                      }

                                      /*
                                      std::string key;
                                      if (!vec.empty())
                                          key = (resultItem.ruleFlags&ExpressionParsingRuleFlags::icase_flag) ? toLower(vec[0]) : vec[0];
                                      else
                                          key = "()";

                                      if (resultItem.uncheckedTokens.find(key)!=resultItem.uncheckedTokens.end())
                                      {
                                      
                                      }
                                          //break; // unchecked token

                                      expected = "value in nested list";
                                      found    = "nested nested list";
                                      return ExpressionParsingResult::failed;
                                      */
                                  }
                                  else
                                  {
                                      std::string str = subIt->itemText;
                                      unquoteTrimString( str );
                                      if (str.empty())
                                      {
                                          if ((resultItem.ruleFlags&ExpressionParsingRuleFlags::empty_allowed_flag)==0)
                                          {
                                              expected = "value in nested list";
                                              found    = "empty value in nested list";
                                              return ExpressionParsingResult::failed;
                                          }
                                      }
                                     
                                      vec.push_back(str);
                                  }
                             
                              }

                              unsigned itemsMinCount = 1;
                              if (resultItem.ruleFlags&ExpressionParsingRuleFlags::number_flag)
                              {
                                  itemsMinCount = resultItem.ruleFlags&ExpressionParsingRuleFlags::number_mask;
                              }

                              //itemsMinCount++;

                              if (vec.empty())
                              {
                                  expected = "nested list";
                                  found    = "empty nested list";
                                  return ExpressionParsingResult::failed;
                              }

                              //std::string key = vec[0];
                              std::string key = (resultItem.ruleFlags&ExpressionParsingRuleFlags::icase_flag) ? toLower(vec[0]) : vec[0];
                              vec.erase( vec.begin(), ++vec.begin() );


                              if (key.empty())
                              {
                                  expected = makeReadableStringFromSet( resultItem.tokenIds, "keyword" ) + std::string(" in the first position of nested list");
                                  found = "empty item in the first position of nested list";
                                  return ExpressionParsingResult::failed;
                              }

                              std::map< std::string, int >::const_iterator tit = resultItem.tokenIds.find(key);
                              
                              if (tit == resultItem.tokenIds.end())
                              {
                                  // token not found
                                  expected = makeReadableStringFromSet( resultItem.tokenIds, "keyword" ) + std::string(" in the first position of nested list");
                                  found    = std::string("'") + key + std::string("'");
                                  return ExpressionParsingResult::failed;
                              }

                              if (resultItem.uncheckedTokens.find(key)==resultItem.uncheckedTokens.end())
                              {
                                  // ckeck items number
                                  if (resultItem.ruleFlags&ExpressionParsingRuleFlags::one_more_item_flag)
                                  {
                                      if (vec.size() < (size_t)itemsMinCount ) // >= - allowed
                                      {
                                          std::stringstream ssExp;
                                          ssExp<<"nested list with at least "<<itemsMinCount<<" items";
                                          expected = ssExp.str();
                                 
                                          std::stringstream ssFnd;
                                          ssFnd<<"nested list with "<<vec.size()<<" items";
                                          found = ssFnd.str();
                                 
                                          return ExpressionParsingResult::failed;
                                      }
                                  }
                                  else // 
                                  {
                                      if (vec.size() != (size_t)itemsMinCount )
                                      {
                                          std::stringstream ssExp;
                                          ssExp<<"nested list with exact "<<itemsMinCount<<" items";
                                          expected = ssExp.str();
                                 
                                          std::stringstream ssFnd;
                                          ssFnd<<"nested list with "<<vec.size()<<" items";
                                          found = ssFnd.str();
                                 
                                          return ExpressionParsingResult::failed;
                                      }
                                  }
                              }

                              resultItem.mapOfVectorsValue[key] = vec;

                              ++it;
                          }

                          if (it!=E && (resultItem.ruleFlags&ExpressionParsingRuleFlags::stops_on_eol_only))
                          {
                              expected = "end of list";
                              found    = it->isList() ? std::string("nested list") : std::string("value");
                              return ExpressionParsingResult::failed;
                          }

                          readTo.push_back(resultItem);

                          //while(it!=E && it->isList())
                          --it;

                      }
                      break;

            case 'E': resultItem.ruleType = ExpressionParsingRuleType::vectorVector;
                      {
                      }
                      break;

            //case ';': tplItemSeparator = true; break;
            //case '': resultItem.ruleType = ExpressionParsingRuleType::; break;
            default:
                 throw std::runtime_error("Expression parsing template - token type in template string");
        }

        if (!skipIncrement)
           ++it;
        //++pTpl;
        //if (tplItemSeparator)
        //{
        //    continue;
        //}

    }

    return ExpressionParsingResult::success;

}

//-----------------------------------------------------------------------------







//-----------------------------------------------------------------------------

namespace ListSimpleXPath
{

//-----------------------------------------------------------------------------
struct PathValues
{
    std::string               name;
    std:: vector<std::string>  values;

}; // struct PathValues


enum class AxisType
{
    child                  = 0,
    descendant_or_child    = 1,
};


struct XPathEntry
{
    AxisType                  axisType; //     = AxisType::child;
    bool                      fAny;
    bool                      fCaseIgnore;
    std::set<std::string>     nameAlters;

    XPathEntry() : axisType(AxisType::child), fAny(false), fCaseIgnore(true), nameAlters() {}
    XPathEntry( AxisType at ) : axisType(at), fAny(false), fCaseIgnore(caseIgnore), nameAlters() {}
    XPathEntry( AxisType at, std::string s, bool ci ) : axisType(at), fAny(false), fCaseIgnore(ci), nameAlters()
    {
        if (fCaseIgnore)
            s = toLower(s);
        splitToSet( s, nameAlters, "|" );

        if (nameAlters.find("*")!=nameAlters.end())
        {
            fAny = true;
            nameAlters.clear(); // not used
        }
    }

    bool isCollectChildsRecurse() const
    {
        return axisType==AxisType::descendant_or_child;
    }

    bool isNameMatched( const std::string &name ) const
    {
        if (fAny)
            return true;
        return fCaseIgnore ? (nameAlters.find(toLower(name)) != nameAlters.end())
                           : (nameAlters.find(name)          != nameAlters.end())
                           ;
    }

}; // struct XPathEntry

//-----------------------------------------------------------------------------

/*

Относительный путь - начинается с точки или не имеет ведущего слеша.
/  - разделяет непосредственных потомков (детей)
// - разделяет потомков - детей, внуков и тд
|  - символ разделения алтернатив

Примеры:
a/b    - все 'b', являющиеся детьми 'a', являющиеся детьми текущего узла
a//b   - все 'b', являющиеся детьми или потомками 'a', являющиеся детьми текущего узла
/a//b  - все 'b', являющиеся детьми или потомками 'a', являющиеся детьми корневого узла
//a/b  - все 'b', являющиеся детьми 'a', являющиеся детьми или потомками корневого узла
.//a/b - все 'b', являющиеся детьми 'a', являющиеся детьми или потомками текущего узла

*/

//-----------------------------------------------------------------------------
// return true if start from root 
inline
bool parseQuery( std::string q, std:: vector<XPathEntry> &xpathExpr, bool ci )
{
    if (q.empty())
        return false;

    bool fromRoot = false;

    if (q[0]=='.')
    {
        fromRoot = false;
        q.erase(0, 1);
    }
    else if (q[0]=='/')
    {
        fromRoot = true;
        q.erase(0, 1);
    }

    std:: vector<std::string> pathParts;
    splitToVector( q, pathParts, '/' );

    bool fDescendant = false;
    for( auto p : pathParts )
    {
        if (p.empty())
        {
            fDescendant = true;
            continue;
        }
        else
        {
            unquoteTrimString(p);
            AxisType at = fDescendant ? AxisType::descendant_or_child : AxisType::child;
            xpathExpr.push_back( XPathEntry(at, p, ci) );
            fDescendant = false;
        }
    }

    return fromRoot;
}

//-----------------------------------------------------------------------------
inline
void collectCandidates( expression_list_t &l, std:: vector< expression_list_t* > &candyList, bool bRecurse )
{
    for( auto &listEntry : l )
    {
        if (listEntry.isText())
            continue;

        candyList.push_back( &listEntry.itemList );

        if (bRecurse)
           collectCandidates( listEntry.itemList, candyList, true );
    }
}

/*
    [/]path/to|*|alter|/list
*/

//-----------------------------------------------------------------------------
template< typename XPathEntryIterator>
inline
size_t executeQuery( std:: vector< expression_list_t* >   srcLists
                 , XPathEntryIterator b, XPathEntryIterator e
                 , std:: vector< expression_list_t* >   *pRes  = 0
                 , bool                                caseIgnore = true
                 )
{
    std:: vector< expression_list_t* > resLists; resLists.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    srcLists.swap(resLists);

    //b->axisType   AxisType::child    AxisType::descendant_or_child

    for(; b!=e; ++b)
    {
        srcLists.clear();
        srcLists.swap(resLists);

        std:: vector< expression_list_t* > candyList; candyList.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

        for( auto pLst : srcLists )
        {
            collectCandidates( *pLst, candyList, b->isCollectChildsRecurse() );
        }

        size_t candyNo = 0;
        for( auto pLst : candyList )
        {
            expression_list_t &lst = *pLst;
            if (lst.empty())
                continue;

            // new code
            std::string lstName;
            if (lst[0].isText())
                lstName = lst[0].itemText;

            if (b->isNameMatched(lstName))
                resLists.push_back(pLst); // &subLst

            /*
            expression_list_t::iterator it = lst.begin();
            for( ; it!=lst.end(); ++it)
            {
                if (it->isText())
                    continue;

                expression_list_t &subLst = it->itemList;
                if (subLst.empty())
                    continue;

                std::string lstName;
                //if (!subLst[0].isText())
                //    continue;
                if (subLst[0].isText())
                    lstName = subLst[0].itemText;

                if (b->isNameMatched(lstName))
                    resLists.push_back(&subLst);
            }
            */
            ++candyNo;

        }

    }

    if (pRes)
        *pRes = resLists;

    return resLists.size();

}

//-----------------------------------------------------------------------------
inline
size_t executeQuery( const expression_list_t             &doc
                 , const std::string                   &query
                 , std:: vector< expression_list_t* >   *pRes  = 0
                 , bool                                 ci = true
                 )
{
    std:: vector<XPathEntry> xpathExpr; xpathExpr.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    bool fromRoot = parseQuery( query, xpathExpr, ci );

    expression_list_t *pInputList = (expression_list_t*)&doc;
    if (fromRoot)
    {
        expression_list_t *pParent = pInputList->getParentList();
        while(pParent)
        {
            pInputList = pParent;
            pParent = pInputList->getParentList();
        }
    }

    std:: vector< expression_list_t* > srcLists; srcLists.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    srcLists.push_back(pInputList);

    return executeQuery( srcLists, xpathExpr.begin(), xpathExpr.end(), pRes, ci );
}


}; // namespace ListSimpleXPath

//-----------------------------------------------------------------------------

