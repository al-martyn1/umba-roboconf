#pragma once

//
#include "tracy_tracing.h"


/*
    (conditional (matchValue "mcu")(matchCase i|ignore|c|s|case|sensitive)(matchType exact|regex|prefix)(matchTo "STM32") )
    (conditional (matchValue "netlistType")(matchType exact|regex|prefix)(matchTo "") )
    (conditional or  (matchValue "netlistType") (conditional ...) )

    Составное условие, состояит из нескольких подусловий, объединяется по OR/AND
    (conditional and (matchValue "netlistType") 
                     (conditional 1 ...)
                     (conditional 2 ...)
                     ( подусловные выражения )    
    )

    Иерархия условий (аналогично объединению по AND)
    (conditional (matchValue "netlistType") 
                 (conditional ...
                     ( подусловные выражения )
                 )
    )

    Если matchTo начинается с символа !, то инвертируем результат проверки.

    При этом разные условия следует проверять в разные моменты:
    а) netlistType - после определения типа нетилиста, но до его разбора
    б) mcu - после того, как определили тип MCU

    По умолчанию
    matchCase: ignore
    matchType: prefix

    matchValue, matchType, matchCase - можно не задавать для дочерних условий
    matchTo - обязательный параметр, маска, префикс или полное значение
              Если первый символ строки - '!', то инвертируем результат.

    При поиске подусловных списков мы задаем условие matchValue. 
    Если оно не совпадает с тем, которое указано в conditional, то условие считается ложным.

    На входе и на выходе - вектор указателей на список

*/

namespace ListConditional
{


//-----------------------------------------------------------------------------
template< typename T>
struct SimpleOptional
{
    bool valid;
    T    value;

    SimpleOptional() : valid(false), value() {}
    SimpleOptional( const SimpleOptional & s ) : valid(s.valid), value(s.value) {}
    SimpleOptional( const T &v ) : valid(true), value(v) {}

    SimpleOptional<T> getCopy()
    {
        return SimpleOptional<T>( *this );
    }

    SimpleOptional& operator=( SimpleOptional s )
    {
        using std::swap;
        swap( *this, s);
        return *this;
    }

    friend void swap( SimpleOptional &s1, SimpleOptional &s2 )
    {
        using std::swap;
        swap(s1.valid, s2.valid);
        swap(s1.value, s2.value);
    }

    SimpleOptional& operator=( const T &t )
    {
        valid = true;
        value = t;
        return *this;
    }

}; // struct SimpleOptional


enum class MatchType
{
    prefix,
    exact,
    regex,
}; // enum class MatchType

enum class ConditionalTestResult
{
    cond_err   = -1,
    cond_false =  0,
    cond_true  =  1,

}; // enum class MatchType

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
inline
ConditionalTestResult testConditional(RoboconfOptions &rbcOpts
                    , const expression_list_t            &condExpr
                    , const std::map<std::string, std::string> &matchValues
                    //, const std::string                  &valueType
                    //, const std::string                  &valueValue
                    , SimpleOptional<std::string>  &parentMatchValueType
                    , SimpleOptional<MatchType>    &parentMatchType
                    , SimpleOptional<bool>         &parentCaseIgnore
                    )
{
    expression_list_t::const_iterator  exprIt = condExpr.begin();

    std::string expected;
    std::string found;
    std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    /*
    (conditional (matchValue "mcu")(matchCase i|ignore|c|s|case|sensitive)(matchType exact|regex|prefix)(matchTo "STM32") )
    (conditional (matchValue "netlistType")(matchType exact|regex|prefix)(matchTo "") )
    (conditional or  (matchValue "netlistType") (conditional ...) )
    (conditional and (matchValue "netlistType") (conditional ...) )
    */

    ExpressionParsingResult
    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti:conditional;V?!", exprIt, condExpr.end()
                                       , readedVals, expected, found
                                       );
    //ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_FALSE_OBJ(condExpr, "parsing 'conditional'");
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_CUSTOM_OBJ( condExpr, "parsing 'conditional'", ConditionalTestResult::cond_err );

    bool bComplex = false;
    bool bOr      = false;

    if (readedVals.size()>1 && !readedVals[1].singleValue.empty())
    {
        std::string opVal = toLower(readedVals[1].singleValue);

        if (opVal=="or" || opVal=="|" || opVal=="||")
        {
            bComplex = true;
            bOr      = true;
        }
        else if (opVal=="and" || opVal=="&" || opVal=="&&")
        {
            bComplex = true;
            bOr      = false;
        }
        else
        {
            LOG_ERR_OBJ(readedVals[0])<<"parsing 'conditional'"<<": expected "<<"'and'/'or' operator, but found '"<<readedVals[0].singleValue<<"' value\n";
            return ConditionalTestResult::cond_err;
        }
    }

    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Mi1$:-conditional,matchValue,matchCase,matchType,matchTo,-()", exprIt, condExpr.end()
                                       , readedVals, expected, found
                                       );
    //ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_FALSE_OBJ( condExpr, "parsing 'conditional'");
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_CUSTOM_OBJ( condExpr, "parsing 'conditional'", ConditionalTestResult::cond_err );

    UMBA_ASSERT(readedVals.size()>=1);

    const auto & mvv = readedVals[0].mapOfVectorsValue;

    bool conditionalFound = false;
    UMBA_USED(conditionalFound);

    SimpleOptional<std::string>  matchValueType = parentMatchValueType;
    SimpleOptional<MatchType>    matchType      = parentMatchType     ;
    SimpleOptional<bool>         fCaseIgnore    = parentCaseIgnore    ;
    SimpleOptional<std::string>  matchTo;

    for( const auto &kv : mvv )
    {
        if (kv.first=="matchvalue")
        {
            UMBA_ASSERT( kv.second.size()>0 );
            matchValueType = toLower(kv.second[0]);
        }
        else if (kv.first=="matchcase")
        {
            UMBA_ASSERT( kv.second.size()>0 );
            std::string mcStr = toLower(kv.second[0]);
            if (mcStr=="i" || mcStr=="ignore" || mcStr=="insensitive")
            {
                fCaseIgnore = true;
            }
            else if ( mcStr=="c" || mcStr=="case" || mcStr=="s" || mcStr=="sensitive")
            {
                fCaseIgnore = false;
            }
            else
            {
                LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': expression 'matchCase': expected "<<"i/ignore/insensitive/c/case/s/sensitive"<<", but found "<<kv.second[0]<<"\n";
                return ConditionalTestResult::cond_err;
            }
        }
        else if (kv.first=="matchtype")
        {
            UMBA_ASSERT( kv.second.size()>0 );
            std::string mtStr = toLower(kv.second[0]);

            if (mtStr=="e" || mtStr=="exact")
            {
                matchType = MatchType::exact;
            }
            else if (mtStr=="r" || mtStr=="regex")
            {
                matchType = MatchType::regex;
            }
            else if (mtStr=="p" || mtStr=="prefix")
            {
                matchType = MatchType::prefix;
            }
            else 
            {
                LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': expression 'matchType': expected "<<"e/exact/p/prefix/r/regex"<<", but found "<<kv.second[0]<<"\n";
                return ConditionalTestResult::cond_err;
            }
        }
        else if (kv.first=="matchto")
        {
            if (bComplex)
            {
                LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': complex '"<<(bOr?"OR":"AND")<<"' condition: expression 'matchTo' not allowed here\n";
                return ConditionalTestResult::cond_err;
            }
            matchTo = kv.second[0];
            // bComplex
        }
        else if (kv.first=="conditional")
        {
            if (!bComplex)
            {
                LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': nested 'conditional' not allowed for simple conditional expression\n";
                return ConditionalTestResult::cond_err;
            }
        }
        else if (kv.first=="()")
        {
        }
        else
        {
            LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': unknown error: "<<kv.first<<"\n";
            return ConditionalTestResult::cond_err;
        }
    }


    parentMatchValueType = matchValueType;
    parentMatchType      = matchType     ;
    parentCaseIgnore     = fCaseIgnore   ;

    //(conditional (matchValue "mcu")(matchCase i|ignore|c|s|case|sensitive)(matchType exact|regex|prefix)(matchTo "STM32") )

    if (!bComplex)
    {
        if (!matchValueType.valid)
        {
            LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': 'matchValue' expression not taken\n";
            return ConditionalTestResult::cond_err;
        }
        if (!matchType.valid)
        {
            LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': 'matchType' expression not taken\n";
            return ConditionalTestResult::cond_err;
        }
        if (!fCaseIgnore.valid)
        {
            LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': 'matchCase' expression not taken\n";
            return ConditionalTestResult::cond_err;
        }
        if (!matchTo.valid)
        {
            LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': 'matchTo' expression not taken\n";
            return ConditionalTestResult::cond_err;
        }

        
        auto it = matchValues.find( matchValueType.value );
        if (it==matchValues.end())
        {
            // value for examine not found. Simple false
            return ConditionalTestResult::cond_false;
        }

        std::string examineStr = it->second;
        std::string matchToStr = matchTo.value;
        if (fCaseIgnore.value)
        {
            examineStr = toLower(examineStr);
            matchToStr = toLower(matchToStr);
        }

        bool fNeg = false;
        if (!matchToStr.empty() && matchToStr[0]=='!')
        {
            fNeg = true;
            matchToStr.erase(0,1);
        }

        /*
            MatchRes   fNeg   Res
               0         0     0
               0         1     1
               1         0     1
               1         1     0
        */


        if (matchType.value==MatchType::exact)
        {
            return ((examineStr==matchToStr)!=fNeg) ? ConditionalTestResult::cond_true : ConditionalTestResult::cond_false;
        }
        else if (matchType.value==MatchType::regex)
        {
            try
            {
                return (regexpSimpleMatchUnsafe( examineStr, matchToStr )!=fNeg) ? ConditionalTestResult::cond_true : ConditionalTestResult::cond_false;;
            }
            catch( const std::exception &e )
            {
                LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': error in regex: "<<e.what()<<"\n";
                return ConditionalTestResult::cond_err;
            }
            catch(...)
            {
                LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': error in regex: unknown error\n";
                return ConditionalTestResult::cond_err;
            }
        }
        else // MatchType::prefix
        {
            return (startsWith( examineStr, matchToStr )!=fNeg) ? ConditionalTestResult::cond_true : ConditionalTestResult::cond_false;;
        }


    }
    else // if (bComplex)
    {
        bool summaryRes = bOr ? false : true;
        size_t totalSubExprCount = 0;
        bool hasErrors =false;

        exprIt = condExpr.begin();
        for(; exprIt!=condExpr.end(); ++exprIt)
        {
            if (!exprIt->isList())
                continue;

            expression_list_t subList = exprIt->itemList;
            if (subList.empty())
                continue;

            if (!subList[0].isText())
                continue;

            if (toLower(subList[0].itemText)!="conditional")
                continue;

            ++totalSubExprCount;

            auto parentMatchValueTypeTmp = parentMatchValueType.getCopy();
            auto parentMatchTypeTmp      = parentMatchType     .getCopy();
            auto parentCaseIgnoreTmp     = parentCaseIgnore    .getCopy();

            ConditionalTestResult stepRes = testConditional( rbcOpts, subList
                                            , matchValues
                                            , parentMatchValueTypeTmp // parentMatchValueType.getCopy()
                                            , parentMatchTypeTmp      // parentMatchType     .getCopy()
                                            , parentCaseIgnoreTmp     // parentCaseIgnore    .getCopy()
                                            );

            if (stepRes==ConditionalTestResult::cond_err)
            {
                hasErrors = true;
            }
            else
            {
                bool boolRes = (stepRes==ConditionalTestResult::cond_true);
                if (bOr)
                    summaryRes |= boolRes;
                else
                    summaryRes &= boolRes;
            }
        }

        if (hasErrors)
            return ConditionalTestResult::cond_err;

        if (!totalSubExprCount)
        {
            LOG_ERR_OBJ(condExpr)<<"parsing 'conditional': complex 'conditional': there is no 'conditional' subexpressions at all\n";
            return ConditionalTestResult::cond_err;
        }

        if (summaryRes)
            return ConditionalTestResult::cond_true;
        return ConditionalTestResult::cond_false;
    }

    // return ConditionalTestResult::cond_err;
}

//-----------------------------------------------------------------------------
inline
std:: vector< expression_list_t* >
extractConditionalLists( RoboconfOptions &rbcOpts
                       , const std:: vector< expression_list_t* > pls
                       , const std::map<std::string, std::string> &parmMatchValues
                       )
{
    std::map<std::string, std::string> matchValues;
    for( const auto &kv : parmMatchValues)
    {
        matchValues[toLower(kv.first)] = kv.second;
    }

    std:: vector< expression_list_t* > resLists; resLists.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    std::string expected;
    std::string found;
   
    std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    bool hasErrors = false;

    for( auto pList : pls )
    {
        const auto &lst = *pList;
        expression_list_t::const_iterator lstIt = lst.begin();

        ExpressionParsingResult
        ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!"
                                           , lstIt, lst.end()
                                           , readedVals, expected, found
                                           );
        std::string listUnderConditionName;
        if (!readedVals.empty())
            listUnderConditionName = readedVals[0].token;

        std:: vector< expression_list_t* > parents; parents.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
        bool parentsGood = true;
        expression_list_t *pParent = lst.getParentList();

        while(pParent)
        {
            const auto &parentLst = *pParent;
            expression_list_t::const_iterator parentLstIt = parentLst.begin();

            bool bAdd = true;
            if (parentLstIt==parentLst.end())
            {
                bAdd = false;
            }
            else if (!parentLstIt->isText())
            {
                bAdd = false;
            }
            else if (toLower(parentLstIt->itemText)!="conditional")
            {
                bAdd = false;
            }

            if (bAdd)
            {
                parents.push_back( pParent );
            }

            pParent = pParent->getParentList(); // parentLst.
            
        }


        if (!parentsGood)
            continue;

        bool conditionalRes = true;

        if (!parents.empty())
        {
            std::reverse( parents.begin(), parents.end() );

            SimpleOptional<std::string>  matchValueType = SimpleOptional<std::string>()                 ; // matchValue   
            SimpleOptional<MatchType>    matchType      = SimpleOptional<MatchType>( MatchType::prefix );
            SimpleOptional<bool>         fCaseIgnore     = SimpleOptional<bool>( true )                  ;
            
            for( auto pCondExprLst : parents )
            {

                ConditionalTestResult res = testConditional(rbcOpts, *pCondExprLst, matchValues, matchValueType, matchType, fCaseIgnore);
                if (res==ConditionalTestResult::cond_false)
                {
                    conditionalRes = false;
                    break;
                }
                else if (res==ConditionalTestResult::cond_true)
                {
                }
                else 
                {
                    hasErrors = true;
                }
            }
        }

        if (!hasErrors)
        {
            if (conditionalRes)
                resLists.push_back(pList);
        }
    }

    if (hasErrors)
        throw std::runtime_error("Error in 'conditional' items");

    return resLists;
}

//-----------------------------------------------------------------------------
inline
std:: vector< expression_list_t* >
extractConditionalLists(RoboconfOptions &rbcOpts
                       , const std:: vector< expression_list_t* > pls
                       , const std::string                  &valueType
                       , const std::string                  &valueValue
                       )
{
    std::map<std::string, std::string> matchValues;
    matchValues[valueType] = valueValue;
    return extractConditionalLists(rbcOpts, pls, matchValues );
}


} // namespace ListConditional


