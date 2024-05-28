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
#include "datacheetUtils.h"
#include "base64.h"


//-----------------------------------------------------------------------------
std::string RoboconfOptions::getMimeTypeByFileExt(const std::string &ext) const
{
    std::unordered_map<std::string, std::string>::const_iterator mit = mimeTypes.find(toLower(ext));
    if (mit==mimeTypes.end())
        return "application/octet-stream";

    return mit->second;
}

//-----------------------------------------------------------------------------
std::string RoboconfOptions::getMimeTypeByFileName(const std::string &fileName) const
{
    return getMimeTypeByFileExt(getFileExtention(fileName));
}

//-----------------------------------------------------------------------------
bool RoboconfOptions::findIcon( const std::string &datasheetName, std::string &foundName, std::string &foundData, bool quetMode ) const
{
    bool isLocalDocument = true;
    std::string documentType = detasheetGetFileType(datasheetName, &isLocalDocument);

    const RoboconfOptions &rbcOpts = *this;

    if (!quetMode)
    {
        LOG_MSG("icons-doc")      << "Document: '" << datasheetName << "\n";
        LOG_MSG("icons-doc-type") << "Type    : '" << documentType << "\n";
    }

    std::unordered_map<std::string, IconInfo>::iterator it = icons.find(documentType);

    if (it==icons.end())
    {
        if (!quetMode)
        {
            LOG_MSG("icons-unk-doc-type") << "unknown document type, try generic document type\n";
        }

        if (!isLocalDocument)
        {
            if (documentType=="" || documentType=="-www")
            {
                // has no extention
                documentType = "www";
            }
            else
            {
                documentType = "document-www";
            }
        }
        else
        {
            documentType = "document";
        }

        if (!quetMode)
        {
            LOG_MSG("icons-doc-type") << "Type    : '" << documentType << "\n";
        }


        it = icons.find(documentType);
        if (it==icons.end())
        {
            if (!quetMode)
            {
                LOG_MSG("icons-not-found-doc-type") << "Document type info not found\n";
            }
            return false;
        }
    }

    if (!it->second.dataBase64.empty())
    {
        foundName = it->second.fullFileName;
        foundData = it->second.dataBase64;

        if (!quetMode)
        {
            LOG_MSG("icons-found") << "Found icon: '" << foundName << "', has data\n";
        }

        return true;
    }

    std::ifstream iconStream;
    std:: vector<std::string> dshCheckedLocations; dshCheckedLocations.reserve(datasheetPaths.size());
    if (!includeSearch( iconStream, it->second.fileName, foundName, iconsPaths, true /* binary */ , &dshCheckedLocations ))
    {
        if (!quetMode)
        {
            LOG_MSG("icons-icon-not-found") << "Icon not found, icon file: '" << it->second.fileName << "'\n";
        }
        //LOG_MSG("datasheet-found-local") << "Datasheet found at: '" << foundName << "'\n";
        return false;
    }

    iconStream.seekg(0, std::ios::end);
    auto length = iconStream.tellg();
    if (!length)
    {
        if (!quetMode)
        {
            LOG_MSG("icons-file-empty") << "Icon file is empty, icon file: '" << foundName << "'\n";
        }
        return false;
    }

    iconStream.seekg(0, std::ios::beg);
    auto bytes = std::vector<char>((std::size_t)length, (char)0);
    iconStream.read(&bytes[0], length);

    it->second.fullFileName = foundName;

    //data:image/jpeg;base64,
    it->second.dataBase64 = "data:"
                          + getMimeTypeByFileName(foundName) 
                          + ";base64,"
                          + base64_encode( (unsigned char*)bytes.data(), (unsigned int)bytes.size()
                                         , false // !add_filling
                                         , 0 // max_line_len
                                         // getBase64StandartChars()
                                         );
    foundData = it->second.dataBase64;

    if (!quetMode)
    {
        LOG_MSG("icons-icon-found") << "Icon found, data: '" << foundData << "'\n";
    }

    return true;

    //foundName = , std::string &foundData


//     mutable std::unordered_map<std::string, IconInfo>   icons;
//  
// struct IconInfo
// {
//     std::string  type          ;
//     std::string  fileName      ;
//     std::string  fullFileName  ;
//     std::string  dataBase64    ;
// };

// bool isDatasheetNetworkLink(std::string name)
// bool isDatacheetAbsFileName(std::string name)
//std::string detasheetGetFileType(const std::string &name, bool *pLocal=0)

// document.ico
// document-www.ico
// pdf.ico
// pdf-www.ico
// wikipedia.ico
// word.ico
// word-www.ico
// www.ico

}

//-----------------------------------------------------------------------------
bool RoboconfOptions::findDatasheet( const std::string &name, std::string &foundName, bool quetMode) const
{
    if (name.empty())
        return false;

    const RoboconfOptions &rbcOpts = *this;

    std::vector<std::string> extraIncPath;
    if (!isDatasheetNetworkLink(name))
    {
        extraIncPath = makeIncVectorFromFileName(name);
    }

    std::string lookFor = datasheetsDb.makeKeyFilename(name, true); // keep case

    std::vector<std::string> aliasList = datasheetsDb.getLocalAliases(name);
    if (!isDatasheetNetworkLink(lookFor))
    {
        aliasList.emplace_back(lookFor);
    }

    if (!quetMode)
    {
        LOG_MSG("datasheet-look-for") << "Datasheet  : '" << name << "\n";
        auto &streamRef = LOG_MSG("datasheet-look-for");
        streamRef << "Looking for: '"; // <<
        bool bFirst = true;
        for(auto alias : aliasList)
        {
            if (!bFirst)
            {
                streamRef << "', '";
            }

            bFirst = false;

            streamRef << alias;
        
        }
        streamRef << "'\n";

    }



    for(auto aliasName : aliasList) 
    {
        std::ifstream dsStream;
        std:: vector<std::string> dshCheckedLocations; dshCheckedLocations.reserve(datasheetPaths.size());
        if (includeSearch( dsStream, aliasName, foundName, extraIncPath, /* <= swap to change lookup order => */ datasheetPaths, false /*not binary*/, &dshCheckedLocations ))
        {
            LOG_MSG("datasheet-found-local") << "Datasheet found at: '" << foundName << "'\n";
            return true;
        }

        if (!quetMode)
        {
            //LOG_WARN_OPT("not-found") << "file '" << name << "' not found\n";
            LOG_MSG("datasheet-not-found") << "Datasheet '" << aliasName << "' not found\n";
            for( auto l : dshCheckedLocations)
                LOG_MSG("datasheet-location") <<"  in '"<<l<<"'\n";
        }

    }

    if (!quetMode)
    {
        //LOG_WARN_OPT("not-found") << "file '" << name << "' not found\n";
        LOG_MSG("datasheet-not-found-local") << "Datasheet '" << name << "' not found in local storages\n";
    }

    //rbcOpts.msgDetailLevels.addToDetailGroup( "datasheet", "datasheet-aliases" );



    // Not found in local paths

    if (isDatasheetNetworkLink(name))
    {
        foundName = name;
        if (!quetMode)
        {
            LOG_MSG("datasheet-found") << "Datasheet found at: '" << foundName << "'\n";
        }
        return true;
    }

    aliasList = datasheetsDb.getNetworkAliases(name);

    if (aliasList.empty())
    {
        if (!quetMode)
        {
            //LOG_WARN_OPT("not-found") << "file '" << name << "' not found\n";
            LOG_MSG("datasheet-not-found") << "Datasheet '" << name << "' not found\n";
        }

        return false;
    }

    foundName = aliasList[0];
    if (!quetMode)
    {
        LOG_MSG("datasheet-found") << "Datasheet found at: '" << foundName << "'\n";
    }

    return true;

}
//-----------------------------------------------------------------------------
std::string RoboconfOptions::macroExpandString( const std::string &text ) const
{
    return ::macroExpandString( text, globalVars );
}

//-----------------------------------------------------------------------------
umba::SimpleFormatter& RoboconfOptions::getNulStream( ) const
{
    return const_cast<umba::SimpleFormatter&>(logNul);
}

//-----------------------------------------------------------------------------
umba::SimpleFormatter& RoboconfOptions::getLogStream( LogEntryType et ) const
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

    return const_cast<umba::SimpleFormatter&>(logNul);
}

//-----------------------------------------------------------------------------
void RoboconfOptions::disableWarnings( const std::string &warnType )
{
    std:: vector<std::string> vw; vw.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

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
    std:: vector<std::string> vm; vm.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

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

    std:: vector< expression_list_t* > componentClassRules; componentClassRules.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

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
    //std:: vector< ComponentClassDetectionRule > componentsClassDetectionRules;
    ComponentClassDetectionRule rule;
    auto res = rule.extractRuleFromListItem( *this, listRule );
    if (res==ExpressionParsingResult::success)
        componentsClassDetectionRules.push_back(rule);
    return res;
}

//-----------------------------------------------------------------------------
bool RoboconfOptions::readComponentClassDetectionRules( const std::string &inputFilename
                                                      , const std:: vector<std::string> &rulesPaths
                                                      , const std::string &netlistType
                                                      , bool readQuet
                                                      )
{
    readQuet = false;

    //TODO: !!!

    /* ������ � ��� ����� ������ ������ ���� ���� � ��������� ������� ������ ����������.
       ��, ��� � ���� � ��������� .rul ��� �������, �� �� ��������, �� ����� ����.
       � ������, ���� �� ������ .rul ������, ��� ���� �����, ����������� ����������, �������������� �������.
       ��� ������ ��� ����, ����� ���������������� ������� ����������� ������ ����������������.
       ����� ������� ������ �� ����������� ���� ����� � ���������, ������� ��� �� ���� � ��� ����� (��� ����������),
       ��� � ������� NET-����, ��� ����� 'path/to/ne/file/file.NET' ��� ����� 'path/to/ne/file/file-classes.rul'.
       ������� ������� �� ����� ��������� � ���� ������ ������.

       ������� ����������� � ����� �������� RoboconfOptions - ���� ������� ��� ��������� ����� ����

     */

    //std::string getPath( const std::string &s )
    //std::string getPathName( const std::string &s )

    RoboconfOptions &rbcOpts = *this; // ��� �������� ������������

    expression_list_t allExpressionsList;

    {
        //std::string implicitClassRulesFileName = getPathName(inputFilename) + "-classes.rul";
        std::string implicitClassRulesFileName = getPathName(inputFilename) + ".classes";
        std::ifstream rulesStream;
        rulesStream.open( implicitClassRulesFileName.c_str() );
        if (!!rulesStream)
        {
            LOG_MSG("cls-rul-file-found")<< "Found component class rules file: " << implicitClassRulesFileName << "\n";
    
            FileSet::file_id_t fileNo = usedFiles.getFileId(implicitClassRulesFileName);
            size_t lineNo = 0;
            expression_list_t lst;
            if ( !readList(fileNo, lineNo, rulesStream, lst ) )
            {
                //if (!readQuet)
                {
                    LOG_ERR<<"failed to read rules file '"<<implicitClassRulesFileName<<"'\n";
                }
                return false;
            }
            rulesStream.close();
            rulesStream.clear();
    
            allExpressionsList = lst;
        }
    }

    // ��� �� ��������� ����������� ���� ����� �� �������, ������ ������ ��, ��� ������ ����� ��������� ������.
    // ��� ����� �� ����� ������ � ����� ������ ������, ����� includeSearch.

    std:: vector<std::string> rulesFiles = componentsClassDetectionRulesFiles;
    std::reverse(rulesFiles.begin(), rulesFiles.end());

    auto inputFileIncPathsVector = makeIncVectorFromFileName(inputFilename);

    std:: vector<std::string>::const_iterator rfIt = rulesFiles.begin();
    for(; rfIt!=rulesFiles.end(); rfIt++)
    {
        std::string foundName;
        std::ifstream rulesStream;
        if (!includeSearch( rulesStream, *rfIt, foundName, inputFileIncPathsVector, /* <= swap to change lookup order => */ rulesPaths ))
        {
            //if (!readQuet)
            {
                LOG_ERR_OPT<<"component class detection rules file '"<<*rfIt<<"' not found\n"; // " (detect rules: '"<<componentsClassDetectionRulesName<<"')\n";
            }
            // if (componentsClassDetectionRulesName == "<default>")
            //     componentsClassDetectionRulesName.clear();
            return false;
        }

        LOG_MSG("cls-rul-file-found")<< "Found component class rules file: " << *rfIt << "\n";


        expression_list_t lst;
        
        FileSet::file_id_t fileNo = usedFiles.getFileId(foundName);
        size_t lineNo = 0;
        if ( !readList(fileNo, lineNo, rulesStream, lst ) )
        {
            //if (!readQuet)
            {
                LOG_ERR<<"failed to read rules file '"<<foundName<<"'\n";
            }
            return false;
        }
        rulesStream.close();

        allExpressionsList.append_expression_list(lst);
    }


    removeCommentExpressionLists( allExpressionsList );


    #if 0
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
        if (!readQuet)
        {
            LOG_ERR_OPT<<"component class detection rules file '"<<lookupForName<<"' not found (detect rules: '"<<componentsClassDetectionRulesName<<"')\n";
        }
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
        if (!readQuet)
        {
            LOG_ERR<<"failed to read rules file '"<<foundName<<"'\n";
        }
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

    #endif

    componentsClassDetectionRules.clear();

    return extractComponentClassDetectionRules( allExpressionsList, netlistType );
    
}

//-----------------------------------------------------------------------------
bool 
RoboconfOptions::extractGroupingRules( expression_list_t & rulesList /* , const std::string &netlistType */  )
{
    rulesList.updateParentPtr(0);

    std:: vector< expression_list_t* > groupingRules; groupingRules.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

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
   
    std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

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
    std:: vector<std::string>    netlabels;
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






