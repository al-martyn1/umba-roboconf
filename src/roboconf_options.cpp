#include "roboconf_options.h"
#include "logerr.h"

#include "component.h"

#include "rdlc-core/macros.h"
#include "regexp.h"
#include "csv.h"
#include "incsearch.h"
#include "list_dump.h"
#include "list_conditional.h"
#include "rdlc-core/textUtils.h"



//-----------------------------------------------------------------------------
std::string RoboconfOptions::macroExpandString( const std::string &text ) const
{
    return ::macroExpandString( text, globalVars );
}

//-----------------------------------------------------------------------------
umba::SimpleFormatter& RoboconfOptions::getNulStream( )
{
    return logNul;
}

//-----------------------------------------------------------------------------
umba::SimpleFormatter& RoboconfOptions::getLogStream( LogEntryType et )
{
    switch(et)
    {
        case LogEntryType::err:
        case LogEntryType::warn:
             if (pErrLog)
                 return *pErrLog;
             break;

        case LogEntryType::msg:
             if (pMsgLog)
                 return *pMsgLog;
    }

    return logNul;
}

//-----------------------------------------------------------------------------
void RoboconfOptions::disableWarnings( const std::string &warnType )
{
    std::vector<std::string> vw;

    char splitCh = contains( warnType, ",;" );
    if ( splitCh )
    {
        splitToVector( warnType, vw, splitCh );
    }
    else
        vw.push_back(warnType);

    for( const auto & w : vw)
        warnControl.disable(w);
}

//-----------------------------------------------------------------------------
bool RoboconfOptions::isWarningAllowed(const std::string &warnType) const
{
    return warnControl.isAllowed(warnType);
}

//-----------------------------------------------------------------------------
void RoboconfOptions::enableMessages( const std::string &msgType )
{
    std::vector<std::string> vm;

    char splitCh = contains( msgType, ",;" );
    if ( splitCh )
    {
        splitToVector( msgType, vm, splitCh );
    }
    else
        vm.push_back(msgType);

    for( const auto & m : vm)

    msgDetailLevels.enableDetails(m);
}

//-----------------------------------------------------------------------------
bool RoboconfOptions::isMessageAllowed(const std::string &msgType) const
{
    return msgDetailLevels.isDetailEnabled(msgType);
}

//-----------------------------------------------------------------------------
void RoboconfOptions::resetEnabledMessages()
{
    msgDetailLevels.resetEnabledDetails();
}

//-----------------------------------------------------------------------------
RoboconfOptions::file_id_t RoboconfOptions::getFileId( std::string fileName )
{
    return usedFiles.getFileId( fileName );
}

//-----------------------------------------------------------------------------
std::string RoboconfOptions::getFileName(file_id_t id) const
{
    return usedFiles.getFileName( id );
}

//-----------------------------------------------------------------------------
bool RoboconfOptions::extractComponentClassDetectionRules(expression_list_t & rulesList, const std::string &netlistType )
{
    rulesList.updateParentPtr(0);

    std::vector< expression_list_t* > componentClassRules;

    ListSimpleXPath::executeQuery( rulesList, "//componentClass", &componentClassRules, caseIgnore );

    componentClassRules = ListConditional::extractConditionalLists( *this, componentClassRules, "netlistType", netlistType );

    RoboconfOptions &rbcOpts = *this;

    LOG_MSG("conf-dump-short-rules-cls")<<"--- Short component class detection rules (after parsing conditions)\n";
    dumpExpressionListSimpleShort( rbcOpts, "conf-dump-short-rules-cls", componentClassRules );

    //LOG_MSG("conf-dump-rules-cls")<<"--- Component class detection rules (before parsing conditions)"; // "\n0"
    //auto &sv = LOG_MSG("conf-dump-rules-cls")<<"--- Rules dump (Component class detection)"; // "\n0"
    //dumpExpressionList( rbcOpts, 0, sv, "conf-dump-rules-cls", lst );
    dumpExpressionList( rbcOpts, "conf-dump-rules-cls", "--- Component class detection rules (after parsing conditions)", componentClassRules );


    bool totalRes = true;

    for ( auto pLst : componentClassRules )
    {
        auto res = extractComponentClassDetectionRule( *pLst );
        if (res==ExpressionParsingResult::skip)
        {
            //++i;
        }
        else
        {
            /*
            if (bErase)
                lst.erase(lst.begin()+i);
            else
                ++i;
            */
            if (res!=ExpressionParsingResult::success)
                totalRes = false;
        }
    }

    return totalRes;

    /*
    return processExpressionList( rulesList, true // erase
                                , [this]( const ExpressionItem & rule )
                                  {
                                      return this->extractComponentClassDetectionRule( rule );
                                  }
                                );
    */
}

//-----------------------------------------------------------------------------
ExpressionParsingResult RoboconfOptions::extractComponentClassDetectionRule( const expression_list_t & listRule )
//ExpressionParsingResult RoboconfOptions::extractComponentClassDetectionRule( const ExpressionItem & listRule )
{
    //std::vector< ComponentClassDetectionRule > componentsClassDetectionRules;
    ComponentClassDetectionRule rule;
    auto res = rule.extractRuleFromListItem( *this, listRule );
    if (res==ExpressionParsingResult::success)
        componentsClassDetectionRules.push_back(rule);
    return res;
}

//-----------------------------------------------------------------------------
bool RoboconfOptions::readComponentClassDetectionRules( const std::string &inputFilename, const std::vector<std::string> &rulesPaths, const std::string &netlistType  )
{
    std::string lookupForName = "default_classes.rul";

    if (componentsClassDetectionRulesName.empty())
        componentsClassDetectionRulesName = "<default>";
    else
    {
        lookupForName = componentsClassDetectionRulesName;
        std::string ext = getFileExtention( lookupForName );
        if (ext==".")
            ext.clear();

        if (ext.empty())
        {
            lookupForName.append(".rul");
        }

        std::string p = getPath(lookupForName);
        if (p.empty())
            lookupForName = appendPath( "classes", lookupForName );
    }

	RoboconfOptions &rbcOpts = *this; // for log

    std::string foundName;
    std::ifstream rulesStream;
    if ( !includeSearch( rulesStream, lookupForName, foundName, makeIncVectorFromFileName(inputFilename), /* <= swap to change lookup order => */ rulesPaths ) )
    {
        LOG_ERR_OPT<<"component class detection rules file '"<<lookupForName<<"' not found (detect rules: '"<<componentsClassDetectionRulesName<<"')\n";
        if (componentsClassDetectionRulesName == "<default>")
            componentsClassDetectionRulesName.clear();
        return false;
    }

    if (componentsClassDetectionRulesName == "<default>")
        componentsClassDetectionRulesName.clear();

    expression_list_t lst;
    
	FileSet::file_id_t fileNo = usedFiles.getFileId(foundName);
	size_t lineNo = 0;
    if ( !readList(fileNo, lineNo, rulesStream, lst ) )
    {
        LOG_ERR<<"failed to read rules file '"<<foundName<<"'\n";
        return false;
    }
    rulesStream.close();

    removeCommentExpressionLists( lst );

    //----------

    /*
    //UNDONE: need to dump rules after processing conditions
    LOG_MSG("conf-dump-short-rules-cls")<<"--- Short component class detection rules (before parsing conditions)\n";
    dumpExpressionListSimpleShort( rbcOpts, "conf-dump-short-rules-cls", lst );

    LOG_MSG("conf-dump-rules-cls")<<"--- Component class detection rules (before parsing conditions)\n"; // "\n0"
    auto &sv = LOG_MSG("conf-dump-rules-cls")<<"--- Rules dump (Component class detection)"; // "\n0"
    dumpExpressionList( rbcOpts, 0, sv, "conf-dump-rules-cls", lst );
    */

    //----------

    return extractComponentClassDetectionRules( lst, netlistType );
}

//-----------------------------------------------------------------------------
bool 
RoboconfOptions::extractGroupingRules( expression_list_t & rulesList /* , const std::string &netlistType */  )
{
    rulesList.updateParentPtr(0);

    std::vector< expression_list_t* > groupingRules;

    ListSimpleXPath::executeQuery( rulesList, "//forceGroup|forceUngroup|keepGroup", &groupingRules, caseIgnore );
/*
    static const int forceGroup   = 1;  // (forceGroup   "GroupName" (names) )
    static const int forceUngroup = 2;  // (forceUngroup "GroupName" (names) )
    static const int keepGroup    = 3;  // (keepGroup    "GroupName" (names) )
*/

    groupingRules = ListConditional::extractConditionalLists( *this, groupingRules, std::map<std::string, std::string>() /* , "netlistType", netlistType */ );

    RoboconfOptions &rbcOpts = *this;
    UMBA_USED(rbcOpts);

    //LOG_MSG("conf-dump-short-rules-cls")<<"--- Short component class detection rules (after parsing conditions)\n";
    //dumpExpressionListSimpleShort( rbcOpts, "conf-dump-short-rules-cls", componentClassRules );

    //LOG_MSG("conf-dump-rules-cls")<<"--- Component class detection rules (before parsing conditions)"; // "\n0"
    //auto &sv = LOG_MSG("conf-dump-rules-cls")<<"--- Rules dump (Component class detection)"; // "\n0"
    //dumpExpressionList( rbcOpts, 0, sv, "conf-dump-rules-cls", lst );
    //dumpExpressionList( rbcOpts, "conf-dump-rules-cls", "--- Component class detection rules (after parsing conditions)", componentClassRules );


    bool totalRes = true;

    for ( auto pLst : groupingRules )
    {
        auto res = extractGroupingRule( *pLst );
        if (res==ExpressionParsingResult::skip)
        {
            //++i;
        }
        else
        {
            /*
            if (bErase)
                lst.erase(lst.begin()+i);
            else
                ++i;
            */
            if (res!=ExpressionParsingResult::success)
                totalRes = false;
        }
    }

    return totalRes;

}

//-----------------------------------------------------------------------------
ExpressionParsingResult 
RoboconfOptions::extractGroupingRule( const ExpressionItem & rule )
{
    if (rule.isText())
        return ExpressionParsingResult::skip; // simple skip unknow text item
    return extractGroupingRule( rule.itemList );
}

ExpressionParsingResult 
RoboconfOptions::extractGroupingRule( const expression_list_t & listRule )
{
    expression_list_t::const_iterator lstIt = listRule.begin();

    RoboconfOptions &rbcOpts = *this;
   
    std::string expected;
    std::string found;
   
    std::vector< ExpressionParsingResultItem > readedVals;

    ExpressionParsingResult
    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:keepGroup,forceGroup,forceUngroup;V_?;I"
                                           , lstIt, listRule.end()
                                           , readedVals, expected, found
                                           );
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ(listRule, "processing 'grouping' rule");
    ROBOCONF_CHECK_PARSE_READ_RESULT_RET_SKIP();

    FileSet::file_id_t  fileNo = listRule.fileNo;
    size_t              lineNo = listRule.lineNo;

    if (readedVals.empty())
    {
        LOG_ERR<<"Expected 'keepGroup', 'forceGroup' or 'forceUngroup'\n";
        return ExpressionParsingResult::failed;
    }

    if (readedVals.empty())
    {
        LOG_ERR<<"Expected 'keepGroup', 'forceGroup' or 'forceUngroup'\n";
        return ExpressionParsingResult::failed;
    }

    ForceGroupingRule rule;
    rule.fileNo = listRule.fileNo;
    rule.lineNo = listRule.lineNo;

    rule.ruleType = (int)readedVals[0].tokenId;

    if (readedVals.size()>1)
    {
        rule.groupName = readedVals[1].singleValue;
    }

    if (readedVals.size()>2)
    {
        rule.netlabels = readedVals[2].vectorValue;
    }

    rule.prepareRegex();
    groupingRules.push_back(rule);

    return ExpressionParsingResult::success;

}

//-----------------------------------------------------------------------------
/*

struct ForceGroupingRule
{
    static const int keepGroup    = 0;  // (keepGroup    "GroupName" (names) )
    static const int forceGroup   = 1;  // (forceGroup   "GroupName" (names) )
    static const int forceUngroup = 2;  // (forceUngroup "GroupName" (names) )

    int                         ruleType;
    std::string                 groupName;
    std::vector<std::string>    netlabels;
};
*/    
int RoboconfOptions::checkGroupingRules( const std::string &grpName, const std::string &netName, std::string *pNewGrpName )
{
    for( const auto &rule : groupingRules )
    {
        auto res = rule.check( *this, grpName, netName, pNewGrpName);
        if (res==ForceGroupingRule::noGroupingRules)
            continue;
        return res;
    }

    return ForceGroupingRule::noGroupingRules;
}

//-----------------------------------------------------------------------------
int ForceGroupingRule::check( RoboconfOptions &rbcOpts, const std::string &grpName, const std::string &netName, std::string *pNewGrpName ) const
{
    bool bCheckGroup = false;

    if (ruleType==keepGroup)
        bCheckGroup = true;
    else if (ruleType==forceGroup)
        bCheckGroup = false; // name is taken for new group, not for check
    else if (ruleType==forceUngroup)
        bCheckGroup = true;

    if (this->groupName.empty())
        bCheckGroup = false;

    if (bCheckGroup)
    {
        if (!checkMatch( rbcOpts, grpName, this->groupName )) // group must match if taken
            return ForceGroupingRule::noGroupingRules;
    }

    if (netlabels.empty())
    {
        return ruleType;
    }

    for ( const auto &n : netlabels )
    {
        if (n.empty())
            continue;

        if (checkMatch( rbcOpts, netName, n ))
        {
            if (ruleType==forceGroup && pNewGrpName)
                *pNewGrpName = this->groupName;
            return ruleType;
        }
    }

    return ForceGroupingRule::noGroupingRules;
}

//-----------------------------------------------------------------------------
bool ForceGroupingRule::checkMatch( RoboconfOptions &rbcOpts, const std::string &str, const std::string &regexStr ) const
{
    try
    {
        if (regexStr.empty())
        {
            LOG_ERR_OBJ(*this) << "Invalid grouping rule - empty expression value\n";
            return false;
        }

        return regexpSimpleMatchUnsafe( str, regexStr );
    }
    catch(const std::exception &e)
    {
        LOG_ERR_OBJ(*this) << "Invalid grouping rule - bad regex: "<<e.what()<<"\n";
        return false;
    }
    catch(...)
    {
        LOG_ERR_OBJ(*this) << "Invalid grouping rule - bad regex\n";
        return false;
    }
}





/*

    std::string regexStr;
    while(readListTextItem( regexList, regexIt, regexStr, true ))
    {
        try
        {
            if (regexStr.empty())
            {
                LOG_ERR_OBJ(*expressionListIt) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - empty expression value\n";
                return false;
            }

            if (regexStr.front()!='^')
                regexStr = std::string("^") + regexStr;
            if (regexStr.back()!='$')
                regexStr = regexStr + std::string("$");

            bool matchRes = regexpSimpleMatchUnsafe( tokens, regexStr );
            if (bMatchAll)
            {
                matchResult = matchResult && matchRes;
                if (!matchResult)
                    return true;
            }
            else
            {
                matchResult = matchResult || matchRes;
                if (matchResult)
                {
                    connection.interfaceType      = setTypeName;
                    connection.interfaceLineType  = setLineName;
                    connection.interfaceDetected  = true;
                    return true;
                }
            }
        }
        catch(const std::exception &e)
        {
            LOG_ERR_OBJ(*expressionListIt) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - bad regex: "<<e.what()<<"\n";
            return false;
        }
        catch(...)
        {
            LOG_ERR_OBJ(*expressionListIt) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - bad regex\n";
            return false;
        }
    }
*/






