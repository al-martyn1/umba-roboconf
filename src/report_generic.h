#pragma once

#include <stdexcept>
#include "reports.h"
#include "rdlc-core/case.h"
#include "cpp.h"
//#include "datasheet.h"

#include "string_set_type.h"
#include "string_string_map_type.h"

enum ConnectionReportHandlerCallType
{
      ConnectionReportHandlerCallType_Values
    //, ConnectionReportHandlerCallType_EndOfValues

};
//-----------------------------------------------------------------------------
inline
std::map< std::string, ICaseConverter* >&
getCaseConverters()
{
    static std::map< std::string, ICaseConverter* > caseConverters;

    if (caseConverters.empty())
    {
        caseConverters["pascal"]     = new CaseConverter_PascalCase();
        caseConverters["camel"]      = new CaseConverter_camelCase ();
        caseConverters["boobs"]      = new CaseConverter_BoobS_CasE();
        caseConverters["balls"]      = new CaseConverter_baLls_caSe();
        caseConverters["std"]        = new CaseConverter_std_case  ();
        caseConverters["cpp"]        = new CaseConverter_std_case  ();
        caseConverters["defs"]       = new CaseConverter_DEFINES_CASE();
        caseConverters["lower"]      = new CaseConverter_lowercase ();
        caseConverters["upper"]      = new CaseConverter_UPPERCASE ();
        caseConverters["keep"]       = new CaseConverter_Keep_case ();
    }

    return caseConverters;
}

//-----------------------------------------------------------------------------
inline
std::string convertCase( const std::string &cvtName, const std::string &str )
{
    std::map< std::string, ICaseConverter* > &caseConverters = getCaseConverters();
    std::map< std::string, ICaseConverter* >::const_iterator it = caseConverters.find(cvtName);
    if (it == caseConverters.end())
    {
        throw std::runtime_error("Unknown case converter");
    }

    ICaseConverter *pConverter = it->second;
    if (!pConverter)
    {
        throw std::runtime_error("Invalid (null) case converter pointer");
    }

    return pConverter->convertCase( str );

}
  
//-----------------------------------------------------------------------------

template <typename THandler>
bool processGeneratorRules( const expression_list_t & generatorRulesList
                          , const std::string &generatorName
                          , const Connection  &conn
                          , string_string_map_type vars
                          , const THandler &handler
                          );
template <typename THandler>
bool processGeneratorRule( const ExpressionItem & generatorRule
                          , const std::string &generatorName
                          , Connection  conn
                          , string_string_map_type vars
                          , const THandler &handler
                          );






struct GenericReportGeneratorInfo
{
    std::string name;
    std::string description;
    std::string defExtention;
    std::string commentPrefix;
    std::string commentStart;
    std::string commentEnd;
    std::string lineFormat;
    size_t      nameWidth = 0; // 42
    bool        ignoreNamesCase = false;
    bool        condencedOutput = false;
};





struct GenericReportGenerator : public IReportGenerator
{
    GenericReportGeneratorInfo generatorInfo;

    GenericReportGenerator( const GenericReportGeneratorInfo &gi ) : generatorInfo(gi) {}    

    virtual
    std::string getDefaultFileExt() override
    {
        return generatorInfo.defExtention;
    }

    virtual
    std::string getReportNameDescription() override
    {
        return generatorInfo.description;
    }

    virtual
    std::vector<std::string> getReportCommandLineNames() override
    {
        std::vector<std::string> names;
        names.emplace_back( generatorInfo.name );
        return names;
    }

    bool shortCommentStart( std::ostream &os )
    {
        if (!generatorInfo.commentPrefix.empty())
        {
            os<<generatorInfo.commentPrefix<<" ";
            return true;
        }
        if (!generatorInfo.commentStart.empty())
        {
            os<<generatorInfo.commentStart<<" ";
            return true;
        }
        return false;
    }

    bool shortCommentEnd( std::ostream &os )
    {
        if (!generatorInfo.commentPrefix.empty())
        {
            os<<"\n";
            return true;
        }
        if (!generatorInfo.commentStart.empty())
        {
            os<<" "<<generatorInfo.commentEnd<<"\n";
            return true;
        }
        return false;
    }

    bool longCommentStart( std::ostream &os )
    {
        if (!generatorInfo.commentStart.empty())
        {
            os<<generatorInfo.commentStart<<" ";
            return true;
        }
        if (!generatorInfo.commentPrefix.empty())
        {
            os<<generatorInfo.commentPrefix<<" ";
            return true;
        }
        return false;
    }

    bool longCommentEnd( std::ostream &os )
    {
        if (!generatorInfo.commentStart.empty())
        {
            os<<" "<<generatorInfo.commentEnd; // <<"\n";
            return true;
        }
        if (!generatorInfo.commentPrefix.empty())
        {
            os<<"\n";
            return true;
        }
        return false;
    }

    bool longCommentLineStart( std::ostream &os )
    {
        if (!generatorInfo.commentStart.empty())
        {
            return true;
        }
        if (!generatorInfo.commentPrefix.empty())
        {
            os<<generatorInfo.commentPrefix<<" ";
            return true;
        }
        return false;
    }


    bool isNameUsedInReport( const string_set_type &usedNames, const std::string &name ) const
    {
        std::string valToFind = generatorInfo.ignoreNamesCase ? toLower(name) : name;
        return usedNames.find(valToFind)!=usedNames.end();
    }

    void setNameUsedInReport( string_set_type &usedNames, const std::string &name ) const
    {
        std::string valToSet = generatorInfo.ignoreNamesCase ? toLower(name) : name;
        usedNames.insert(valToSet);
    }

    // https://www.st.com/en/microcontrollers/stm32f303vd.html#design-scroll
    virtual
    //bool generateReport( std::ostream &os, std:: map<std::string, NetlistInfo> &nets, std::map<std::string, std::vector< ComponentInfo > > libComponents, const expression_list_t &processingRules, const ConnectionBuildingOptions &opts ) override
    bool generateReport( RoboconfOptions &rbcOpts
                       , const std::string &reportName
                       , std::ostream &os
                       , std::map<std::string, NetlistInfo> &nets
                       , std::vector< ComponentInfo > libComponents
                       , const expression_list_t &processingRules
                       , const ConnectionBuildingOptions &opts
                       , size_t &processedMcus
                       ) override
    {
        processedMcus = 1; // Ќам в данном отчете MCU без надобности, мы их тут не обрабатываем, но чтобы наверху не ругались, говорим, что нашли

        string_set_type reportUsedNames;

        std::string docTitle = std::string( "Generator: ") + generatorInfo.name;

        if (!nets.empty())
        {
            auto first = nets.begin();

            if (!first->second.projectName.empty())
                docTitle = first->second.projectName + std::string(" - ") + docTitle;
        }

        if (shortCommentStart(os))
        {
            os<<docTitle;
            shortCommentEnd(os);
            os<<"\n";
        }

        if (!rbcOpts.outputProlog.empty())
            os<<"\n"<<rbcOpts.outputProlog<<"\n\n";

        for( const auto& nlIt : nets )
        {
            NetlistInfo netlistInfo = nlIt.second; 
            if (shortCommentStart(os))
            {
                os<<netlistInfo.projectName<<" - "<<netlistInfo.name;
                shortCommentEnd(os);
                os<<"\n";
            }


            std::vector< std::string > mcuDesignators;

            findStartConnectionsDesignators( netlistInfo, "MCU" /* purpose */ , mcuDesignators );

            size_t processedMcus = 0;

            for( auto curMcuD : mcuDesignators )
            {
                auto componentKV = netlistInfo.components.find(curMcuD);
                if (componentKV == netlistInfo.components.end())
                    continue;

                processedMcus++;

                if (shortCommentStart(os))
                {
                    os<<"MCU "<<curMcuD<<" "<<componentKV->second.typeName<<" peripherals";
                    shortCommentEnd(os);
                    os<<"\n";
                }


                std::vector<Connection> connectionList;
                connectionsListBuild( rbcOpts, opts, netlistInfo, curMcuD, connectionList );

                if (!processConnectionModifyRules( rbcOpts, connectionList, processingRules ))
                    return false;

                connectionsListRemoveMcuDuplicates( connectionList );


                std::vector< ConnectionsGroup > connGroups;
                splitConnectionsToGroupsByTarget( rbcOpts, connGroups, connectionList );


                if (!connectionsDetectInterfaces( rbcOpts, connGroups, processingRules, operateVerbose ))
                    return false;

                //groupConnectionsBySheet(connectionList);


                for( auto& connGrp : connGroups )
                {
                    if (shortCommentStart(os))
                    {
                        os<<(connGrp.groupTitle.empty() ? connGrp.groupDesignator : connGrp.groupTitle);
                        shortCommentEnd(os);
                        os<<"\n";
                    }

                    size_t grpRulesCount = 0;

                    for( auto& conn : connGrp.connections )
                    {
                        string_string_map_type vars;
                        auto self = this;

                        size_t connRulesCount = 0;
                        

                        if (!processGeneratorRules( processingRules, generatorInfo.name, conn, vars, 
                                                    [&]( ConnectionReportHandlerCallType callType, std::string name, const std::string &value, const std::string &comment )
                                                    {
                                                        //if (callType==ConnectionReportHandlerCallType_EndOfValues)
                                                        //{
                                                        //    os<<"\n";
                                                        //    return
                                                        //}

                                                        if ( self->isNameUsedInReport( reportUsedNames, name ))
                                                            return;

                                                        grpRulesCount++;
                                                        connRulesCount++;

                                                        self->setNameUsedInReport( reportUsedNames, name );

                                                        if (name.size() < generatorInfo.nameWidth)
                                                            name.append( generatorInfo.nameWidth - name.size(), ' ');

                                                        std::map< std::string, std::string> fmtVars;
                                                        fmtVars["name"] = name;
                                                        fmtVars["NAME"] = name;
                                                        fmtVars["Name"] = name;
                                                        fmtVars["value"] = value;
                                                        fmtVars["VALUE"] = value;
                                                        fmtVars["Value"] = value;

                                                        os << macroExpandString( generatorInfo.lineFormat, fmtVars, RegexMacroNameSymbolsValidator() );

                                                        if (!comment.empty())
                                                        {
                                                            os<<"    ";
                                                            if (self->longCommentStart( os ))
                                                            {
                                                                os<<comment;
                                                                self->longCommentEnd(os);
                                                            }
                                                        }

                                                        os<<"\n";
                                                        
                                                        //os<<name<<" = "<<value<<"\n";
                                                    }
                                                  )
                           )
                           return false;

                           //if (connRulesCount>1 && generatorInfo.condencedOutput)
                           if (!generatorInfo.condencedOutput)
                              os<<"\n";

                    } // for( auto& conn : connectionList )

                    if (!grpRulesCount)
                       os<<"\n";
                    else if (generatorInfo.condencedOutput)
                       os<<"\n";
                    else
                       os<<"\n\n\n";

                } // for( auto& connGrp : connGroups )

            } // for( auto curMcuD : mcuDesignators )

            if (!processedMcus)
            {
                //os<<"<div class=\"warning\">No MCU found in this netlist</div>\n";
            }

        } // for( const auto& nlIt : nets )


        if (!rbcOpts.outputEpilog.empty())
            os<<"\n\n"<<rbcOpts.outputEpilog<<"\n";

        //os<<"<body>\n";
        //os<<"</body>\n</html>\n";

        return true;

    }

}; // struct GenericReportGenerator

//-----------------------------------------------------------------------------







//-----------------------------------------------------------------------------
inline
bool extractCustomGenerator( const ExpressionItem & generatorInfoRule, std::map< std::string, IReportGenerator* > &reportGenerators )
{
    if (generatorInfoRule.isText())
        return true; // simple skip unknow text item

    const expression_list_t &lst = generatorInfoRule.itemList;
    expression_list_t::const_iterator it = lst.begin();

#if 0
(generatorInfo (name "h_conf")
               (description "C-header config generator")
               (defaultFileExtention "h")
               (lineComment "//")
               (commentStart "/*")
               (commentEnd "*/")
)

#endif

    std::vector< std::string > readedVals;
    std::string expected;
    std::string found;
    bool readFieldsRes = readListByTemplate( "Ti!:generatorinfo;L", it, lst.end()
                                           , readedVals, expected, found
                                           );
    if (!readFieldsRes)
    {
        LOG_ERR_OBJ(generatorInfoRule)<<"'generatorinfo' processing rule: expected "<<expected<<", but found "<<found<<"\n";
        return false;
    }

    if (readedVals[0]!="generatorinfo")
        return true;

    --it;

    GenericReportGeneratorInfo generatorInfo;

    bool condencedOutputTaken = false;
    bool ignorecaseTaken      = false;

    while( it!=lst.end()
         && ( generatorInfo.name.empty()
           || generatorInfo.description.empty()
           || generatorInfo.defExtention.empty()
           || generatorInfo.commentPrefix.empty()
           || generatorInfo.commentStart.empty()
           || generatorInfo.commentEnd.empty()
           || generatorInfo.lineFormat.empty()
           || (generatorInfo.nameWidth==0)
           || !condencedOutputTaken
           || !ignorecaseTaken
           //|| generatorInfo..empty()
            )
         )
    {
    
        readedVals.clear();
        if (!it->isList())
        {
            ++it;
            continue;
        }


        const expression_list_t &infoLst         = it->itemList;
        expression_list_t::const_iterator infoIt = infoLst.begin();
        ++it;

        readFieldsRes = readListByTemplate( "Ti:name,description,defaultFileExtention,lineComment,commentStart,commentEnd,lineFormat,nameWidth,condenced,ignoreCase;V", infoIt, infoLst.end()
                                           , readedVals, expected, found
                                           );
        if (!readFieldsRes)
        {
            LOG_ERR_OBJ(generatorInfoRule)<<"'generatorinfo' processing rule: expected "<<expected<<", but found "<<found<<"\n";
            return false;
        }

        if (readedVals[0]=="name")
            generatorInfo.name = readedVals[1];
        else if (readedVals[0]=="description")
            generatorInfo.description = readedVals[1];
        else if (readedVals[0]=="defaultfileextention")
            generatorInfo.defExtention = readedVals[1];
        else if (readedVals[0]=="linecomment")
            generatorInfo.commentPrefix = readedVals[1];
        else if (readedVals[0]=="commentstart")
            generatorInfo.commentStart = readedVals[1];
        else if (readedVals[0]=="commentend")
            generatorInfo.commentEnd = readedVals[1];
        else if (readedVals[0]=="lineformat")
            generatorInfo.lineFormat = readedVals[1];
        else if (readedVals[0]=="namewidth")
            generatorInfo.nameWidth = (size_t)parseIntegerTolerant( readedVals[1], 10 );
        else if (readedVals[0]=="condenced")
        {
            std::string cmp = toLower(readedVals[1]);
            if (cmp=="true" || cmp=="1" || cmp=="yes" || cmp=="t" || cmp=="y" || cmp=="")
                generatorInfo.condencedOutput = true;
            else if (cmp=="false" || cmp=="0" || cmp=="no" || cmp=="f" || cmp=="n")
                generatorInfo.condencedOutput = false;
            else
                {
                    LOG_ERR_OBJ(generatorInfoRule)<<"'generatorinfo' processing rule: 'condenced' - expected TRUE/FALSE/1/0, but found '"<<readedVals[1]<<"'\n";
                    return false;
                }
            condencedOutputTaken = true;
        }
        else if (readedVals[0]=="ignorecase")
        {
            std::string cmp = toLower(readedVals[1]);
            if (cmp=="true" || cmp=="1" || cmp=="yes" || cmp=="t" || cmp=="y")
                generatorInfo.ignoreNamesCase = true;
            else if (cmp=="false" || cmp=="0" || cmp=="no" || cmp=="f" || cmp=="n")
                generatorInfo.ignoreNamesCase = false;
            else
                {
                    LOG_ERR_OBJ(generatorInfoRule)<<"'generatorinfo' processing rule: 'ignorecase' - expected TRUE/FALSE/1/0, but found '"<<readedVals[1]<<"'\n";
                    return false;
                }
            ignorecaseTaken      = true;
        }

    }

    if (generatorInfo.nameWidth < 12)
        generatorInfo.nameWidth = 12;


    if (generatorInfo.name.empty())
    {
        LOG_ERR_OBJ(generatorInfoRule)<<"'generatorinfo' processing rule: generator name not taken\n";
        return false;
    }

    if (generatorInfo.lineFormat.empty())
    {
        LOG_ERR_OBJ(generatorInfoRule)<<"'generatorinfo' processing rule: generator line format ('lineFormat') not taken\n";
        return false;
    }

    if (generatorInfo.defExtention.empty())
    {
        LOG_ERR_OBJ(generatorInfoRule)<<"'generatorinfo' processing rule: generator default target file extention ('defaultFileExtention') not taken\n";
        return false;
    }

/*
struct GenericReportGeneratorInfo
{
    std::string name;
    std::string description;
    std::string defExtention;
    std::string commentPrefix;
    std::string commentStart;
    std::string commentEnd;
};

*/

    addReportGenerator( reportGenerators, new GenericReportGenerator(generatorInfo) );

    return true;
}

//-----------------------------------------------------------------------------
inline
bool extractCustomGenerators( const expression_list_t & generatorInfoRulesList, std::map< std::string, IReportGenerator* > &reportGenerators )
{
    for( const auto & rule : generatorInfoRulesList )
    {
        if (!extractCustomGenerator( rule, reportGenerators ))
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
template <typename THandler>
inline
bool processGeneratorRules( const expression_list_t & generatorRulesList
                          , const std::string &generatorName
                          , const Connection  &conn
                          , string_string_map_type vars
                          , const THandler &handler
                          )
{
    for( const auto & rule : generatorRulesList )
    {
        if (!processGeneratorRule( rule, generatorName, conn, vars, handler ))
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
template <typename THandler>
inline
bool processGeneratorRule( const ExpressionItem & generatorRule
                         , const std::string &generatorName
                         , Connection  conn
                         , string_string_map_type vars
                         , const THandler &handler
                         )
{
    if (generatorRule.isText())
        return true;

    const expression_list_t &lst = generatorRule.itemList;
    expression_list_t::const_iterator it = lst.begin();

#if 0
(generator "h_conf" interface "GPIO:OUT"
    (match "function" to "P(A|B|C|D|E|F|G|H|I|J)(\d{1,2})"  set ( ("$(NET)_GPIO"               to "GPIO$({1})")
                                                                  ("$(NET)_GPIO_PIN"           to "GPIO_Pin_$({2})")
                                                                  ("$(NET)_GPIO_PIN_DIRECTION" to "GPIO_DIRECTION_OUT" comment "only for info")
                                                                )
    )
)
#endif

    std::vector< std::string > readedVals;
    std::string expected;
    std::string found;
    bool readFieldsRes = readListByTemplate( "Ti!:generator;V;Ti:interface;V;L", it, lst.end()
                                           , readedVals, expected, found
                                           );
    if (!readFieldsRes)
    {
        LOG_ERR_OBJ(generatorRule)<<"'generator' processing rule: expected "<<expected<<", but found "<<found<<"\n";
        return false;
    }

    if (readedVals[0]!="generator")
        return true;

    if (readedVals[1]!=generatorName)
        return true;

    --it; // back to match list

    std::string interfaceName  = conn.interfaceType + std::string(":") + conn.interfaceLineType;

    std::string interfaceNameMatchRegex = readedVals[3];

    std::string regexCheckError;
    if (!regexCheck( interfaceNameMatchRegex, regexCheckError ))
    {
        LOG_ERR_OBJ(generatorRule)<< "invalid 'generator processing rule: bad regular expression: "<<regexCheckError<<"\n";
        return false;
    }

    interfaceNameMatchRegex = regexpAddAssertionBOL( interfaceNameMatchRegex );
    interfaceNameMatchRegex = regexpAddAssertionEOL( interfaceNameMatchRegex );

    if (!regexpSimpleMatch( interfaceName, interfaceNameMatchRegex ))
        return true;

#if 0
(generator "h_conf" interface "GPIO:OUT"
    (match "function" to "P(A|B|C|D|E|F|G|H|I|J)(\d{1,2})"  set ( ("$(NET_NAME)_GPIO"               to "GPIO$({1})")
                                                                  ("$(NET_NAME)_GPIO_PIN"           to "GPIO_Pin_$({2})")
                                                                  ("$(NET_NAME)_GPIO_PIN_DIRECTION" to "GPIO_DIRECTION_OUT" comment "only for info")
                                                                )
    )
)
#endif

    while( it!=lst.end() )
    {
        if (!it->isList())
            continue;

        readedVals.clear();

        const expression_list_t &matchList         = it->itemList;
        expression_list_t::const_iterator matchIt  = matchList.begin();

        ++it;

        readFieldsRes = readListByTemplate( "Ti:match;V;Ti:to;V;L", matchIt, matchList.end()
                                           , readedVals, expected, found
                                           );
        if (!readFieldsRes)
        {
            LOG_ERR_OBJ(matchList)<<"'generator' processing rule: match: expected "<<expected<<", but found "<<found<<"\n";
            return false;
        }

        --matchIt;
        // back to list


        enum TokenType
        {
             tokenType_Unknown,
             tokenType_netToken,
             tokenType_pinFunction,
             tokenType_Purpose
        };
       
        TokenType tokenType = tokenType_Unknown;

        const std::string &tokenTypeStr = readedVals[1];
       
        if (tokenTypeStr=="token" || tokenTypeStr=="nettoken")
        {
            tokenType = tokenType_netToken;
        }
        else if (tokenTypeStr=="function" || tokenTypeStr=="pinfunction")
        {
            tokenType = tokenType_pinFunction;
        }
        //else if (readedVals[1]=="purpose" || readedVals[1]=="unitpurpose")
        //{
        //    tokenType = tokenType_Purpose;
        //}
        else
        {
            LOG_ERR_OBJ(matchList)<< "Invalid 'generator processing rule: match: unknown token '"<<readedVals[1]<<"'\n";
            return false;
        }

        string_set_type tokens;
        if (tokenType == tokenType_netToken)
        {
            tokens = conn.getTokens();
        }
        else if (tokenType == tokenType_pinFunction)
        {
            tokens = conn.getSourceFunctions();
        }
        //else if (tokenType == tokenType_Purpose)
        //{
        //    tokens.insert(targetPurposeType);
        //}

        std::string regex = readedVals[3];
        //std::string regexCheckError;
        if (!regexCheck( regex, regexCheckError ))
        {
            LOG_ERR_OBJ(matchList)<< "Invalid 'generator processing rule: match: bad regular expression: "<<regexCheckError<<"\n";
            return false;
        }
        
        regex = regexpAddAssertionBOL( regex );
        regex = regexpAddAssertionEOL( regex );

        auto varsCopy = vars;

        if (!regexpMatchFillVars( regex, tokens, varsCopy ))
            continue;

#if 0
(generator "h_conf" interface "GPIO:OUT"
    (match "function" to "P(A|B|C|D|E|F|G|H|I|J)(\d{1,2})"  set ( ("$(NET_NAME)_GPIO"               to "GPIO$({1})")
                                                                  ("$(NET_NAME)_GPIO_PIN"           to "GPIO_Pin_$({2})")
                                                                  ("$(NET_NAME)_GPIO_PIN_DIRECTION" to "GPIO_DIRECTION_OUT" comment "only for info")
                                                                )
    )
)
#endif

        //std::string orgNetName     = conn.srcNetName;

        Connection connCopy = conn;

        expression_list_t setList; // = matchIt->itemList;

        for(; matchIt!=matchList.end(); ++matchIt)
        {
            if (!matchIt->isList())
                continue;

            const expression_list_t           &subList = matchIt->itemList;
            expression_list_t::const_iterator    subIt = subList.begin();

            readedVals.clear();
            readFieldsRes = readListByTemplate( "Ti:modify,set", subIt, subList.end()
                                               , readedVals, expected, found
                                               );
            if (!readFieldsRes)
            {
                LOG_ERR_OBJ(matchList)<<"'generator' processing rule: match: expected "<<expected<<", but found "<<found<<"\n";
                return false;
            }
        
            if (readedVals[0]=="set")
            {
                setList = subIt->itemList;
                break;
            }

            if (readedVals[0]=="modify")
            {
                if (!processConnectionModifyRules( rbcOpts, connCopy, *matchIt /* modifyRule */ ))
                {
                    return false;;
                }
            }
        
        }

        if (matchIt==matchList.end())
        {
            LOG_ERR_OBJ(matchList)<<"'generator' processing rule: match: not found expected 'set' expression\n";
            return false;
        }


        std::string netName        = connCopy.processedStrings["MCUNET"];
        std::string unitType       = connCopy.processedStrings["UNITTYPE"];
        std::string unitPurpose    = connCopy.processedStrings["UNITPURPOSE"];
        std::string unitClass      = connCopy.processedStrings["UNITCLASS"];

        

        //bool processConnectionModifyRules( Connection &conn, const ExpressionItem & rule /* modifyRule */ )

        std::map< std::string, std::string > varsArgs;
        
        varsArgs["INTERFACE_NAME"]  = interfaceName;
        varsArgs["InterfaceName"]   = convertCase( "pascal" , interfaceName);
        varsArgs["interfaceName"]   = convertCase( "camel"  , interfaceName);
        varsArgs["InterfacE_NamE"]  = convertCase( "boobs"  , interfaceName);
        varsArgs["interFace_naMe"]  = convertCase( "balls"  , interfaceName);
        varsArgs["interface_name"]  = convertCase( "cpp"    , interfaceName);
        varsArgs["interfacename"]   = convertCase( "lower"   , interfaceName);
        varsArgs["INTERFACENAME"]   = convertCase( "upper"   , interfaceName);
        
        varsArgs["NET_NAME"]        = netName;
        varsArgs["NetName"]         = convertCase( "pascal"  , netName);
        varsArgs["netName"]         = convertCase( "camel"   , netName);
        varsArgs["NeT_NamE"]        = convertCase( "boobs"   , netName);
        varsArgs["nEt_naMe"]        = convertCase( "balls"   , netName);
        varsArgs["net_name"]        = convertCase( "cpp"     , netName);
        varsArgs["netname"]         = convertCase( "lower"   , netName);
        varsArgs["NETNAME"]         = convertCase( "upper"   , netName);

        varsArgs["MCU_NET"]         = netName;
        varsArgs["McuNet"]          = convertCase( "pascal"  , netName);
        varsArgs["mcuNet"]          = convertCase( "camel"   , netName);
        varsArgs["McU_NeT"]         = convertCase( "boobs"   , netName);
        varsArgs["mCu_nEt"]         = convertCase( "balls"   , netName);
        varsArgs["mcu_net"]         = convertCase( "cpp"     , netName);
        varsArgs["mcunet"]          = convertCase( "lower"   , netName);
        varsArgs["MCUNET"]          = convertCase( "upper"   , netName);
        
        varsArgs["UNIT_TYPE"]       = unitType;
        varsArgs["UnitType"]        = convertCase( "pascal"  , unitType);
        varsArgs["unitType"]        = convertCase( "camel"   , unitType);
        varsArgs["UniT_TypE"]       = convertCase( "boobs"   , unitType);
        varsArgs["unIt_tyPe"]       = convertCase( "balls"   , unitType);
        varsArgs["unit_type"]       = convertCase( "cpp"     , unitType);
        varsArgs["unittype"]        = convertCase( "lower"   , unitType);
        varsArgs["UNITTYPE"]        = convertCase( "upper"   , unitType);
        
        varsArgs["UNIT_PURPOSE"]    = unitPurpose;
        varsArgs["UnitPurpose"]     = convertCase( "pascal"  , unitPurpose);
        varsArgs["unitPurpose"]     = convertCase( "camel"   , unitPurpose);
        varsArgs["UniT_PurposE"]    = convertCase( "boobs"   , unitPurpose);
        varsArgs["unIt_purPose"]    = convertCase( "balls"   , unitPurpose);
        varsArgs["unit_purpose"]    = convertCase( "cpp"     , unitPurpose);
        varsArgs["unitpurpose"]     = convertCase( "lower"   , unitPurpose);
        varsArgs["UNITPURPOSE"]     = convertCase( "upper"   , unitPurpose);
        
        varsArgs["UNIT_CLASS"]      = unitClass;
        varsArgs["UnitClass"]       = convertCase( "pascal"  , unitClass);
        varsArgs["unitClass"]       = convertCase( "camel"   , unitClass);
        varsArgs["UniT_ClasS"]      = convertCase( "boobs"   , unitClass);
        varsArgs["unIt_clAss"]      = convertCase( "balls"   , unitClass);
        varsArgs["unit_class"]      = convertCase( "cpp"     , unitClass);
        varsArgs["unitclass"]       = convertCase( "lower"   , unitClass);
        varsArgs["UNITCLASS"]       = convertCase( "upper"   , unitClass);
        
        for( auto & varKV : varsArgs )
        {
            varsCopy[varKV.first] = makeCppName( varKV.second );
            //varKV.second = makeCppName( varKV.second );
        }


        //const expression_list_t &setList         = matchIt->itemList;
        expression_list_t::const_iterator setIt  = setList.begin();

        for(; setIt!=setList.end(); ++setIt)
        {
            if (!setIt->isList())
                continue;

            const expression_list_t &setItemList        = setIt->itemList;
            expression_list_t::const_iterator setItemIt = setItemList.begin();

            std::vector< std::string > readedVals;

            // "V;Ti:to;V;Ti:comment;V"
            readFieldsRes = readListByTemplate( "V;Ti:to;V", setItemIt, setItemList.end()
                                           , readedVals, expected, found
                                           );

            if (!readFieldsRes)
            {
                LOG_ERR_OBJ(setList)<<"'generator' processing rule: set: expected "<<expected<<", but found "<<found<<"\n";
                return false;
            }

            if (setItemIt!=setItemList.end())
            {
                readFieldsRes = readListByTemplate( "Ti:comment;V", setItemIt, setItemList.end()
                                           , readedVals, expected, found
                                           );
                if (!readFieldsRes)
                {
                    LOG_ERR_OBJ(setList)<<"'generator' processing rule: set: expected "<<expected<<", but found "<<found<<"\n";
                    return false;
                }
            }


            //std::vector<std::string> readedVec = readedVals2;
            std::string comment;
            if (readedVals.size()>4)
            {
                comment = macroExpandString( readedVals[4], varsCopy, RegexMacroNameSymbolsValidator() );
            }

            handler( ConnectionReportHandlerCallType_Values
                   , parseEscapes( macroExpandString( readedVals[0], varsCopy, RegexMacroNameSymbolsValidator() ) )
                   , parseEscapes( macroExpandString( readedVals[2], varsCopy, RegexMacroNameSymbolsValidator() ) )
                   , comment
                   );

        }


    }

    return true;

}

