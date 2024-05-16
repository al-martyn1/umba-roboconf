#pragma once

#include "umba/umba.h"
#include "umba/simple_formatter.h"

#include <map>
#include <set>
#include <vector>
#include <string>

#include "packages.h"
#include "file_set.h"
#include "msg_control.h"
#include "log_options.h"
#include "detail_levels.h"

#include "rdlc-core/macros.h"
#include "rdlc-core/textUtils.h"


#include "rdlc-core/log.h"

#include "component_class.h"


class RoboconfOptions;

struct ForceGroupingRule
{
    static const int noGroupingRules = -1;
    static const int keepGroup    = 0;  // (keepGroup    "GroupName" (names) )
    static const int forceGroup   = 1;  // (forceGroup   "GroupName" (names) )
    static const int forceUngroup = 2;  // (forceUngroup "GroupName" (names) )

    int                         ruleType;
    std::string                 groupName;
    std::vector<std::string>    netlabels;

    FileSet::file_id_t          fileNo = (FileSet::file_id_t)-1;
    size_t                      lineNo = (size_t)-1;



    bool checkMatch( RoboconfOptions &rbcOpts, const std::string &str, const std::string &regexStr ) const;

    std::string prepareRegex( std::string str )
    {
        if (str.empty())
            return str;

        //if (str.front()!='^')
        //    str = std::string("^") + str;
        //if (str.back()!='$')
        //    str = str + std::string("$");

        return str;
    }

    void prepareRegex( )
    {
        groupName = prepareRegex(groupName);
        for( auto & n : netlabels )
        {
            n = prepareRegex(n);
        }
    }

    int check( RoboconfOptions &rbcOpts, const std::string &grpName, const std::string &netName, std::string *pNewGrpName = 0 ) const;

};


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


class RoboconfOptions
{

    umba::SimpleFormatter     *pMsgLog;
    umba::SimpleFormatter     *pErrLog;

    umba::NulCharWriter       nulWriter;
    umba::SimpleFormatter     logNul; // (&nulWriter);

public:

    typedef  FileSet::file_id_t  file_id_t;

    std::vector<std::string>                   includePaths;
    std::vector<std::string>                   libPaths;
    std::vector<std::string>                   datasheetPaths;
    std::vector<std::string>                   rulesPaths;
                                              
    std::map< std::string, std::string >       globalVars;
    FileSet                                    usedFiles;
    MsgControl                                 warnControl;
    PackagesDb                                 packagesDb;
    LogOptions                                 logOptions;
    DetailLevels                               msgDetailLevels;
    std::string                                componentsClassDetectionRulesName;
    std::vector< ComponentClassDetectionRule > componentsClassDetectionRules;
    std::vector<ForceGroupingRule>             groupingRules;

    std::string outputProlog;
    std::string outputEpilog;


    RoboconfOptions() : pMsgLog(0), pErrLog(0), nulWriter(), logNul(&nulWriter) {}
    RoboconfOptions(umba::SimpleFormatter *pml, umba::SimpleFormatter *pel ) : pMsgLog(pml), pErrLog(pel), nulWriter(), logNul(&nulWriter) {}

    void setMsgStream( umba::SimpleFormatter *ps ) { pMsgLog = ps; }
    void setErrStream( umba::SimpleFormatter *ps ) { pErrLog = ps; }

    std::string macroExpandString( const std::string &text ) const;
    umba::SimpleFormatter& getNulStream( );
    umba::SimpleFormatter& getLogStream( LogEntryType et );

    //--- Warnings control

    void disableWarnings( const std::string &warnType );
    bool isWarningAllowed(const std::string &warnType) const;

    //--- Messages control

    void enableMessages( const std::string &msgType );
    bool isMessageAllowed(const std::string &msgType) const;
    void resetEnabledMessages();

    //--- Files control

    file_id_t getFileId( std::string fileName );
    std::string getFileName(file_id_t id) const;

    //--- Component class detection

    bool 
    extractComponentClassDetectionRules(expression_list_t & rulesList, const std::string &netlistType );

    ExpressionParsingResult 
    //extractComponentClassDetectionRule( const ExpressionItem & listRule );
    extractComponentClassDetectionRule( const expression_list_t & listRule );

    bool 
    readComponentClassDetectionRules( const std::string &inputFilename, const std::vector<std::string> &rulesPaths, const std::string &netlistType );


    //--- Grouping rules parsing

    bool 
    extractGroupingRules(expression_list_t & rulesList /* , const std::string &netlistType */  );

    ExpressionParsingResult 
    extractGroupingRule( const ExpressionItem & rule );

    ExpressionParsingResult 
    extractGroupingRule( const expression_list_t & listRule );

    //bool 
    //readGroupingRules( const std::string &inputFilename, const std::vector<std::string> &rulesPaths, const std::string &netlistType );

    /*
    ForceGroupingRule::noGroupingRules
    ForceGroupingRule::keepGroup      
    ForceGroupingRule::forceGroup     
    ForceGroupingRule::forceUngroup   
    */

    int checkGroupingRules( const std::string &grpName, const std::string &netName, std::string *pNewGrpName = 0 );

    void readPrologEpilog( std::string fileName, std::string &readTo )
    {
        if (startsWithAndStrip( fileName, "inline:" ))
        {
            readTo = parseEscapes(fileName);
            //return true;
        }
        else
        {
            readTo = readFileEncodedSafe( fileName );
            //return readFile( fileName, readTo );
        }
    }
    
    void readProlog( std::string fileName )
    {
        readPrologEpilog( fileName, outputProlog );
    }

    void readEpilog( std::string fileName )
    {
        readPrologEpilog( fileName, outputEpilog );
    }
    

}; // class RoboconfOptions


