#pragma once

#include <algorithm>

#include "incsearch.h"
#include "designator.h"
//#include "component_handlers.h"

//
#include "string_set_type.h"
#include "string_string_map_type.h"

//
#include "tracy_tracing.h"


//-----------------------------------------------------------------------------
struct ConnectionInterfaceInfo
{
    std::string        interfaceType;      // need to find by rules
    std::string        interfaceLineType;  // need to find by rules
    bool               interfaceDetected = false;

}; // struct ConnectionInterfaceInfo



struct Connection //-V730
{
    std::string        dstPinDesignator;

    std::string        srcPinDesignator;
    std::string        srcNetName;
    ComponentInfo      srcComponentInfo;
    ComponentPinInfo   srcPinInfo;

    ComponentInfo      dstComponentInfo;
    ComponentPinInfo   dstPinInfo;

    std::string        interfaceType;      // need to find by rules
    std::string        interfaceLineType;  // need to find by rules
    bool               interfaceDetected = false;


    bool               netStop = false;
    int                netGroundType = 0;

    //std::string        dstComponentSheet;
    //std::string        dstComponentType;
    //std::string        dstComponentPurpose;
    //unsigned           dstComponentClass;
    //std::string        dstComponentClassStr;
    //std::string        dstComponentDescription;

    std:: vector< std::string> payloads;

    std:: vector<Connection>   extraDestinations;

/*
    conn.processedStrings["MCUNET"]      = conn.srcNetName;
    conn.processedStrings["UNITTYPE"]    = conn.dstComponentInfo.typeName;
    conn.processedStrings["UNITPURPOSE"] = conn.dstComponentInfo.purpose;
    conn.processedStrings["UNITCLASS"]   = conn.dstComponentInfo.getComponentClassString();
*/
    string_string_map_type    processedStrings;

    string_set_type           mcuNetTokens;
    string_set_type           mcuNetClasterNames;

    int                       groupingRuleType;
    std::string               forceGroupName;


    std::string getProcessedString(const std::string &id) const
    {
        typename string_string_map_type::const_iterator it = processedStrings.find(id);
        if (it!=processedStrings.end())
            return it->second;
        return std::string();
    }


    void updateInterfaceInfo(const ConnectionInterfaceInfo& info)
    {
        interfaceType     = info.interfaceType    ;
        interfaceLineType = info.interfaceLineType;
        interfaceDetected = info.interfaceDetected;
    }

    ConnectionInterfaceInfo getInterfaceInfo() const
    {
        ConnectionInterfaceInfo info;
        info.interfaceType     = interfaceType    ;
        info.interfaceLineType = interfaceLineType;
        info.interfaceDetected = interfaceDetected;
        return info;
    }


    string_set_type getTokens() const
    {
        return mcuNetTokens;
    }

    string_set_type getSourceFunctions() const
    {
        string_set_type res;
        for( auto fn : srcPinInfo.pinFunctions )
        {
            trim(fn, "_-~");
            res.insert(fn);
        }
        return res;
    }

    string_set_type getTargetFunctions() const
    {
        string_set_type res;
        for( auto fn : dstPinInfo.pinFunctions )
        {
            trim(fn, "_-~");
            res.insert(fn);
        }
        return res;
    }


    string_set_type getClasterNames( const string_set_type &excludes = string_set_type(), size_t minSizeLimit = 0 ) const
    {
        string_set_type resSet;
        for( auto n : mcuNetClasterNames )
        {
            if (n.size() < minSizeLimit)
                continue;

            if (excludes.find(n)!=excludes.end())
               continue;

            resSet.insert(n);

        }

        return resSet;
    }


    //! Возвращает компоненты для заголовка. Для этого делит имя цепи на части, адаляяя разделители '-', но оставляя цифры прикрепленными к своим меткам
    std:: vector<std::string> getTitleParts()
    {
        return splitComponentName( processedStrings["MCUNET"], 0, false /* keepSeps */, false /* splitAlsoDigits */ );
    }

    static
    string_set_type makeSetFromNamePartsSkipNumbers(const std:: vector<std::string> &nameParts /* , bool skipNumbers */ )
    {
        UmbaTracyTraceScope();

        string_set_type s;
        for( auto np : nameParts )
        {
            uint64_t u;
            trim( np, "_-~");

            if (np.empty())
                continue;

            if (parseInteger( np, u, 10 ))
                continue; // skip numbers

            s.insert(toUpper(np));
        }

        return s;
    }

    //! Формирует токены имени сети - с отделением номеров от имён (SPI1->SPI), и токены для заголовка, без отделения цифр (SPI1->SPI1)
    void splitMcuNetMakeTokens( )
    {
        UmbaTracyTraceScope();
        //TODO: !!! Добавил умолчательные аргументы. Надо разобраться, нужно ли отделять цифры от имён
        mcuNetTokens       = makeSetFromNamePartsSkipNumbers( splitComponentName( processedStrings["MCUNET"], 0, true /* keepSeps */, true /* splitAlsoDigits */ ) );

        // Делаем то же самое, но только в этот раз не отделяем приклеенные к именам цифры, удаляем только отдельно стоящие
        mcuNetClasterNames = makeSetFromNamePartsSkipNumbers( getTitleParts() );
    }

    //! Генерирует заголовок из всех частей, сгенерированных ф-ей getTitleParts(), или только из частей, которые ещё содержатся в наборе cmn
    std::string generateTitle( const string_set_type &cmn, bool bCommon = true )
    {
        UmbaTracyTraceScope();
        std::string res;
        std:: vector<std::string> titleParts = getTitleParts();
        for( const auto& tp : titleParts )
        {
            // Тут, похоже, было неправильно - bCommon отвечал, добавлять ли часть в зависимости от того, нйдена или не найдена
            // А надо - если bCommon==true, то добавльять только если найдена, иначе добавлять всегда

            // if ( ( cmn.find(tp)!=cmn.end()) == bCommon )
            // {
            //     if (!res.empty())
            //         res.append(1,'_');
            //     res.append(tp);
            // }

            if (cmn.find(tp)==cmn.end())
            {
                // Не найдена, проверяем, обязательно ли добавлять только то, что есть в наборе
                if (bCommon)
                    continue;
                // Разрешено добалять всё
            }

            if (!res.empty())
                res.append(1,'_');
            res.append(tp);
        }

        return res;
    }
    

    bool isConnected() const
    {
        return srcPinDesignator!=dstPinDesignator;
    }


}; // struct Connection

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
inline
string_set_type makeStringSetIntersection( const string_set_type &s1, const string_set_type &s2)
{
    string_set_type res;
    for( const auto& str : s1 )
    {
        if (s2.find(str)==s2.end())
            continue;
        res.insert(str);
    }

    return res;
}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
struct ConnectionsGroup //-V730
{
    std::string                  groupDesignator; // target designator for all of group items
    std::string                  groupTitle;      // target designator for all of group items
    ComponentInfo                dstComponentInfo;
    std:: vector< Connection >   connections;
    bool                         ungroupped = false;
    bool                         forceNamed = false;;


    bool isSrcPinDesignatorPresent( const std::string &pd ) const
    {
        for( const auto& conn : connections )
        {
            if (conn.srcPinDesignator==pd)
                return true;
        }
        return false;
    }

    bool removeConnectionBySrcPinDesignator( const std::string &pd )
    {
        std:: vector< Connection >::iterator it = connections.begin();
        for( ; it != connections.end(); ++it)
        {
            if (it->srcPinDesignator == pd)
            {
                connections.erase(it);
                return true;
            }
        }
        return false;
    }

    bool removeConnectionByDstPinDesignator( const std::string &pd )
    {
        std:: vector< Connection >::iterator it = connections.begin();
        for( ; it != connections.end(); ++it)
        {
            if (it->dstPinDesignator == pd)
            {
                connections.erase(it);
                return true;
            }
        }
        return false;
    }

    

    string_set_type getCommonNameParts( const string_set_type &excludes = string_set_type(), size_t minSizeLimit = 0 ) const
    {
        if (connections.empty())
            return string_set_type();

        string_set_type intersection;
        bool bFirst = true;

        for( const auto& conn : connections )
        {
            if (bFirst)
            {
                intersection = conn.getClasterNames( excludes, minSizeLimit );
                bFirst = false;
            }
            else
            {
                intersection = makeStringSetIntersection(intersection, conn.getClasterNames( excludes, minSizeLimit ) );
            }
        }

        return intersection;
    }

    std::string tryGenerateTitle( )
    {
        UmbaTracyTraceScope();
        if (connections.empty())
            return std::string();

        return connections[0].generateTitle( getCommonNameParts(), true /* bCommon */  );

    }

    void updateConnectionsGroupingOptions( RoboconfOptions &rbcOpts )
    {
        for( auto &c : connections )
        {
            c.groupingRuleType = rbcOpts.checkGroupingRules( groupTitle, c.processedStrings["MCUNET"] /* c.srcNetName */ , &c.forceGroupName );
            //if (c.groupingRuleType==)
        }
    }

}; // struct ConnectionsGroup

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
struct CommonNetInfo
{

    static const int NET_GROUND_OPTION_UNKNOWN     = 0;
    static const int NET_GROUND_OPTION_GND         = 1;
    static const int NET_GROUND_OPTION_VCC_PLUS    = 2;
    static const int NET_GROUND_OPTION_VCC_MINUS   = 3;

    std::string mask;
    std::string description;
    int         netGroundOption;

    FileSet::file_id_t fileNo = (FileSet::file_id_t)-1;
    size_t             lineNo = 0;


    CommonNetInfo( const std::string &msk = std::string(), const std::string &d = std::string(), int o = 0, FileSet::file_id_t fNo = (FileSet::file_id_t)-1, size_t lNo = 0 ) : mask(msk), description(d), netGroundOption(o), fileNo(fNo), lineNo(lNo) {}

}; // struct CommonNetInfo

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
struct ConnectionBuildingOptions
{
    //mutable std::map< std::string, CommonNetInfo > stopNets;
    mutable std:: vector<CommonNetInfo> stopNets;

/*
    void addStopNet( const std::string &netName, const std::string &netDescription, int netGroundOption )
    {
        stopNets[netName] = CommonNetInfo(netDescription, netGroundOption);
    }

    bool isStopNet(const std::string &netName) const
    {
        if (netName.empty())
           return true;

        if (stopNets.find(netName) != stopNets.end())
            return true;

        if (netName[0]=='+' || netName[0]=='-')
        {
            if (netName[0]=='+')
                stopNets[netName] = CommonNetInfo( std::string("Generic ") + netName, CommonNetInfo::NET_GROUND_OPTION_VCC_PLUS );
            else
                stopNets[netName] = CommonNetInfo( std::string("Generic ") + netName, CommonNetInfo::NET_GROUND_OPTION_VCC_MINUS );
            return true;
        }

        if ( startsWith(netName, "GND") || startsWith(netName, "AGND") || startsWith(netName, "SGND") || startsWith(netName, "PGND") )
        {
            stopNets[netName] = CommonNetInfo( std::string("Generic ") + netName, CommonNetInfo::NET_GROUND_OPTION_GND );
            return true;
        }

        return false;
    }

    std::string getStopNetDescription(const std::string &netName) const
    {
        if (netName.empty())
           return std::string("<EMPTY>");

        auto it = stopNets.find(netName);
        if (it!=stopNets.end())
            return it->second.description;

        if (netName[0]=='+' || netName[0]=='-')
            return std::string("Generic ") + netName;

        if ( startsWith(netName, "GND") || startsWith(netName, "AGND") || startsWith(netName, "SGND") || startsWith(netName, "PGND") )
           return std::string("Generic ") + netName;

        return std::string("<UNKNOWN>");
    }

    int getStopNetGroundOption(const std::string &netName) const
    {
        auto it = stopNets.find(netName);
        if (it!=stopNets.end())
            return it->second.netGroundOption;
        return CommonNetInfo::NET_GROUND_OPTION_UNKNOWN;
    }
*/

    bool isStopNet(RoboconfOptions &rbcOpts, const std::string &netName) const
    {
        UmbaTracyTraceScope();

        if (netName.empty())
           return true;

        for( const auto &netInfo : stopNets )
        {
            try
            {
                bool checkRes = regexpSimpleMatchUnsafe( netName, netInfo.mask );
                if (checkRes)
                {
                    LOG_MSG("net-chkstp-matched")<<"stop net: "<<netName<<" matched to "<<netInfo.mask<<"\n";
                    return true;
                }
                else
                {
                    LOG_MSG("net-chkstp-not-matched")<<"stop net: "<<netName<<" not matched to "<<netInfo.mask<<"\n";
                }
            }
            catch(const std::regex_error &e)
            {
                LOG_ERR_OBJ(netInfo)<<"regex error: "<<e.what()<<"\n";
            }
            catch(...)
            {
                LOG_ERR_OBJ(netInfo)<<"regex unknown error\n";
            }
        }

        return false;

    }

    std::string getStopNetDescription(const std::string &netName) const
    {
        #if defined(ROBOCONF_TRACY_TRACE_ALL)
            UmbaTracyTraceScope();
        #endif

        if (netName.empty())
           return std::string("<EMPTY>");

        for( const auto &netInfo : stopNets )
        {
            try
            {
                bool checkRes = regexpSimpleMatchUnsafe( netName, netInfo.mask );
                if (checkRes)
                {
                    return netInfo.description + netName;
                }
            }
            catch(...)
            {
            }
        }

        return std::string("<UNKNOWN>");
    }

    int getStopNetGroundOption(const std::string &netName) const
    {
        #if defined(ROBOCONF_TRACY_TRACE_ALL)
            UmbaTracyTraceScope();
        #endif

        for( const auto &netInfo : stopNets )
        {
            try
            {
                bool checkRes = regexpSimpleMatchUnsafe( netName, netInfo.mask );
                if (checkRes)
                {
                    return netInfo.netGroundOption;
                }
            }
            catch(...)
            {
            }
        }

        return CommonNetInfo::NET_GROUND_OPTION_UNKNOWN;
    }


    bool extractStopNets(RoboconfOptions &rbcOpts, expression_list_t & rulesList )
    {
        #if defined(ROBOCONF_TRACY_TRACE_ALL)
            UmbaTracyTraceScope();
        #endif

        return processExpressionList(rulesList, true /* erase */
            , [this, &rbcOpts](const ExpressionItem & rule)
        {
            return this->extractStopNet( rbcOpts, rule);
        }
        );
    }

    ExpressionParsingResult extractStopNet( RoboconfOptions &rbcOpts, const ExpressionItem & rule )
    {
        #if defined(ROBOCONF_TRACY_TRACE_ALL)
            UmbaTracyTraceScope();
        #endif

        if (rule.isText())
            return ExpressionParsingResult::skip; // simple skip unknow text item
     
        const expression_list_t &lst = rule.itemList;
        expression_list_t::const_iterator it = lst.begin();
     
        std::string expected;
        std::string found;
     
        std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
     
        //(powerNet ground "SGND.*" "PGND.*" "AGND.*" "GND.*")
        //(powerNet plus  "+\d+.*" "+VCC.*")
        //(powerNet minus "-\d+.*" "-VCC.*")
     
        ExpressionParsingResult
        ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:powerNet;Ti:ground,plus,minus;A+", it, lst.end()
                                               , readedVals, expected, found
                                               );
        ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ(rule, "processing rule 'powerNet'");
        ROBOCONF_CHECK_PARSE_READ_RESULT_RET_SKIP();
/*
struct CommonNetInfo
{

    std::string description;
    int         netGroundOption;

    CommonNetInfo( const std::string &d = std::string(), int o = 0 ) : description(d), netGroundOption(o) {}

*/
        int groundOption = CommonNetInfo::NET_GROUND_OPTION_UNKNOWN;
        std::string descriptionPrefix;

        if (readedVals[1].tokenId==0) // ground
        {
            groundOption = CommonNetInfo::NET_GROUND_OPTION_GND;
            descriptionPrefix = "Ground ";
        }
        else if (readedVals[1].tokenId==1) // plus
        {
            groundOption = CommonNetInfo::NET_GROUND_OPTION_VCC_PLUS;
            descriptionPrefix = "Power (+) ";
        }
        else if (readedVals[1].tokenId==2) // minus
        {
            groundOption = CommonNetInfo::NET_GROUND_OPTION_VCC_MINUS;
            descriptionPrefix = "Power (-) ";
        }

        std:: vector<std::string>::const_iterator vit = readedVals[2].vectorValue.begin();
        for( ; vit != readedVals[2].vectorValue.end(); ++vit)
        {
            stopNets.push_back( CommonNetInfo(*vit, descriptionPrefix, groundOption, readedVals[2].fileNo, readedVals[2].lineNo ) );
        }
     
     
        return ExpressionParsingResult::success;
    }

    





}; // struct ConnectionBuildingOptions

//-----------------------------------------------------------------------------
