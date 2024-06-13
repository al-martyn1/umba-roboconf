#pragma once

#include <algorithm>

#include "incsearch.h"
#include "designator.h"

//
#include "string_set_type.h"
#include "string_string_map_type.h"

//
#include "connections_types.h"
//
#include "component_handlers.h"
//
#include "tracy_tracing.h"





//-----------------------------------------------------------------------------
struct CompareConnectionsByDstComponentWeightLess
{
    bool operator()( const Connection &c1, const Connection &c2 ) const
    {
        return getComponentClassWeight(c1.dstComponentInfo.componentClass) < getComponentClassWeight(c2.dstComponentInfo.componentClass);
    }
};

struct CompareConnectionsByDstComponentWeightGreater
{
    bool operator()( const Connection &c1, const Connection &c2 ) const
    {
        return getComponentClassWeight(c1.dstComponentInfo.componentClass) > getComponentClassWeight(c2.dstComponentInfo.componentClass);
    }
};

//int getComponentClassWeight( ComponentClass cc )


struct CompareConnectionsBySrcNetNameWithIntsLess
{
    bool compareStrings;

    CompareConnectionsBySrcNetNameWithIntsLess( bool cmpAsStr = false ) : compareStrings(cmpAsStr) {}

    bool operator()( const Connection &c1, const Connection &c2 ) const
    {
        std:: vector<unsigned> v1 = readNumericParts( c1.srcNetName );
        std:: vector<unsigned> v2 = readNumericParts( c2.srcNetName );
        if (compareStrings)
        {
            if (v1.empty() || v2.empty())
                return c1.srcNetName < c2.srcNetName;
        }
        int ires = compareUnsignedVectors( v1, v2 );
        if (compareStrings)
        {
            if (ires==0) 
                return c1.srcNetName < c2.srcNetName;
        }
        return ires < 0;
    }
};

struct CompareConnectionsBySrcNetNameWithIntsGreater
{
    bool compareStrings;

    CompareConnectionsBySrcNetNameWithIntsGreater( bool cmpAsStr = false ) : compareStrings(cmpAsStr) {}

    bool operator()( const Connection &c1, const Connection &c2 ) const
    {
        std:: vector<unsigned> v1 = readNumericParts( c1.srcNetName );
        std:: vector<unsigned> v2 = readNumericParts( c2.srcNetName );
        if (compareStrings)
        {
            if (v1.empty() || v2.empty())
                return c1.srcNetName > c2.srcNetName;
        }
        int ires = compareUnsignedVectors( v1, v2 );
        if (compareStrings)
        {
            if (ires==0)
                return c1.srcNetName > c2.srcNetName;
        }
        return ires > 0;
    }
};

//srcNetName




//-----------------------------------------------------------------------------









//-----------------------------------------------------------------------------
inline
bool ConnectionGroupLessByTitle( const ConnectionsGroup &g1, const ConnectionsGroup &g2 )
{
    return g1.groupTitle < g2.groupTitle;
}

//-----------------------------------------------------------------------------
inline
bool ConnectionGroupGreaterByTitle( const ConnectionsGroup &g1, const ConnectionsGroup &g2 )
{
    return g1.groupTitle > g2.groupTitle;
}

//-----------------------------------------------------------------------------







//-----------------------------------------------------------------------------
template< typename StreamType, typename SetType >
inline
StreamType& printSetConnHelper( StreamType &os, const SetType &sss )
{
    for( auto it = sss.begin(); it!=sss.end(); ++it)
    {
        if (it != sss.begin())
            os<<", ";
        os<<*it;
    }

    return os;
}

//-----------------------------------------------------------------------------
template< typename StreamType, typename SetItemType, typename SetItemTypePrinter >
inline
StreamType& printSetConnHelperEx( StreamType &os, const std::set< SetItemType > &sss, const SetItemTypePrinter &printer )
{
    for( auto it = sss.begin(); it!=sss.end(); ++it)
    {
        if (it != sss.begin())
            os<<", ";
        printer(os, *it);
    }

    return os;
}


//inline
//size_t getMinStringSizeFromSet

//-----------------------------------------------------------------------------
inline
void splitConnectionGroupByClasters( RoboconfOptions &rbcOpts, const ConnectionsGroup &connGrp, std:: vector< ConnectionsGroup > &connGroupsPushTo, ConnectionsGroup &ungrouppedConns )
{
    #if defined(ROBOCONF_STRING_STRING_MAP_USE_UNORDERED_MAP)
        using size_t_set_type   = std::unordered_set<size_t>;
        using intersections_map = std::unordered_map< string_set_type, size_t_set_type >;
    #else
        using size_t_set_type   = std:: set<size_t>;
        using intersections_map = std:: map< string_set_type, size_t_set_type >;
    #endif


    ConnectionsGroup newGroupTemplate;
    newGroupTemplate.groupDesignator  = connGrp.groupDesignator;
    newGroupTemplate.dstComponentInfo = connGrp.dstComponentInfo;

    // grp-dump-
    //LOG_MSG("net-dump-cmn")<<"--- Netlist: "<<netlist.second.name<<"\n";

    string_set_type excludes;
    size_t minSizeLimit = 3;
    string_set_type commonCore = connGrp.getCommonNameParts( excludes, minSizeLimit );

    //std:: map< std::set<std::string>, std::set<size_t> > 
    intersections_map allIntersections;

    size_t i = 0, size = connGrp.connections.size();

    // ищем пересечения в именах цепей и пытаемся по ним сделать группы
    for( ; i!=size; ++i)
    {
        for( size_t j = 0; j!=size; ++j)
        {
            if (i==j)
                continue; // сами с собой пересечение не ищем - оно и так максимальное
            auto clasterI = connGrp.connections[i].getClasterNames(excludes, minSizeLimit);
            auto clasterJ = connGrp.connections[j].getClasterNames(excludes, minSizeLimit);
            string_set_type intrsc = makeStringSetIntersection(clasterI, clasterJ);
            allIntersections[intrsc].insert(i);
            allIntersections[intrsc].insert(j);
        }
    }

    {
        //std:: map< std::set<std::string>, std::set<size_t> > 
        intersections_map tmp;
        for( auto intrscKV : allIntersections ) 
        {
            // Почему-то я решил, что если все netlabel'ы входящие в данный компонент
            // имеют общее ядро, то отдельные соединения просто обязаны иметь общее ядро
            // большего размера, иначе их нельзя поместить в одну группу
            // с учетом того, что по смыслу мы пытаемся объеденить контакты разъемов,
            // остальное у нас сгруппировано по таргет десигнаторам

            //if (intrscKV.first.size()<=commonCore.size())
            //   continue;
            if (intrscKV.second.size()<2)
               continue;
            tmp[intrscKV.first] = intrscKV.second;
        }

        tmp.swap(allIntersections);
    }

    size_t_set_type  usedConns;
    for( auto intrscKV : allIntersections ) 
    {
        ConnectionsGroup newGroup = newGroupTemplate;
        for(auto no : intrscKV.second)
        {
            if (usedConns.find(no)!=usedConns.end())
                continue;
            usedConns.insert(no);
            newGroup.connections.emplace_back(connGrp.connections[no]);
        }

        newGroup.groupTitle = newGroup.tryGenerateTitle();
        if (!newGroup.connections.empty())
        {
            connGroupsPushTo.emplace_back(newGroup);
        }
    }

    for( i = 0; i!=size; ++i)
    {
        if (usedConns.find(i)!=usedConns.end())
            continue;
        //if ()
        ungrouppedConns.connections.emplace_back(connGrp.connections[i]);
    }

}

//-----------------------------------------------------------------------------
inline
std:: vector< Connection >::iterator connectionsListFindConnectionBySrcPinDesignator( std:: vector< Connection > &connList, const std::string &srcPinDesignator )
{
    std:: vector< Connection >::iterator it = connList.begin();
    for(; it != connList.end(); ++it)
    {
        if (it->srcPinDesignator == srcPinDesignator)
            return it;
    }

    return connList.end();
}

//-----------------------------------------------------------------------------
inline
void moveConnectionDuplicatesToExtra( std:: vector< Connection > &connList )
{
    std:: vector< Connection > tmp; tmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    
    for( const auto & conn : connList )
    {
        auto it = connectionsListFindConnectionBySrcPinDesignator(tmp, conn.srcPinDesignator);
        if (it==tmp.end())
        {
            tmp.emplace_back(conn);
        }
        else
        {
            it->extraDestinations.emplace_back(conn);
        }
    }

    tmp.swap(connList);

}

//-----------------------------------------------------------------------------








//-----------------------------------------------------------------------------

// struct Connection //-V730
// {
//  
//     std::string        interfaceType;      // need to find by rules
//     std::string        interfaceLineType;  // need to find by rules
//     bool               interfaceDetected = false;
//  
//  
//     ComponentInfo      srcComponentInfo;
//     std::string        srcPinDesignator;
//     ComponentPinInfo   srcPinInfo;
//     std::string        srcNetName;
//  
//  
//     bool               netStop = false;
//     int                netGroundType = 0;
//     std::string        dstPinDesignator;
//  
//     ComponentInfo      dstComponentInfo;
//     ComponentPinInfo   dstPinInfo;
//     //std::string        dstComponentSheet;
//     //std::string        dstComponentType;
//     //std::string        dstComponentPurpose;
//     //unsigned           dstComponentClass;
//     //std::string        dstComponentClassStr;
//     //std::string        dstComponentDescription;
//  
//     std:: vector< std::string> payloads;
//  
//     std:: vector<Connection>   extraDestinations;
//  
// /*
//     conn.processedStrings["MCUNET"]      = conn.srcNetName;
//     conn.processedStrings["UNITTYPE"]    = conn.dstComponentInfo.typeName;
//     conn.processedStrings["UNITPURPOSE"] = conn.dstComponentInfo.purpose;
//     conn.processedStrings["UNITCLASS"]   = conn.dstComponentInfo.getComponentClassString();
// */
//     string_string_map_type    processedStrings;
//  
//     string_set_type           mcuNetTokens;
//     string_set_type           mcuNetClasterNames;
//  
//     int                       groupingRuleType;
//     std::string               forceGroupName;


inline
void splitConnectionsToGroupsByTarget_mcuNetMakeTokensAndPutToGroupMap( const std:: vector< Connection > &conns, std::unordered_map< std::string , std:: vector<Connection> > &connMap )
{
    UmbaTracyTraceScope();
    for( auto conn : conns )
    {
        conn.splitMcuNetMakeTokens(); // Тут мы делаем разбиения имён цепей на части, для последующих кластеризаций
        connMap[ conn.dstComponentInfo.designator ].emplace_back( conn );
    }
}

inline
ConnectionsGroup* splitConnectionsToGroupsByTarget_splitToGroupsSimple(std:: vector< ConnectionsGroup > &connGroups, const std::unordered_map< std::string , std:: vector<Connection> > &connMap)
{
    UmbaTracyTraceScope();

    // В негруппированное помещаем соединения, которые в одиночестве идут на таргет десигнатор
    {
        ConnectionsGroup ungrouppedConns;
        ungrouppedConns.groupTitle = "Unclassified";
        ungrouppedConns.ungroupped = true;
        connGroups.emplace_back(ungrouppedConns);
    }

    ConnectionsGroup *pUngrouppedConns = &connGroups[0];

    for( const auto& connKV : connMap )
    {
        if (connKV.second.size() <= 1)
        {
            // Если по целевому десигнатору у нас найдено одно соединение, то это без классификации
            pUngrouppedConns->connections.insert( pUngrouppedConns->connections.end(), connKV.second.begin(), connKV.second.end() );
        }
        else
        {
            ConnectionsGroup grp;
            grp.groupDesignator  = connKV.second[0].dstComponentInfo.designator;
            grp.dstComponentInfo = connKV.second[0].dstComponentInfo;
            grp.connections      = connKV.second;
            grp.groupTitle       = grp.tryGenerateTitle( );

            connGroups.emplace_back(grp);
            pUngrouppedConns = &connGroups[0];
        }
    }

    return pUngrouppedConns;
}

inline
void splitConnectionsToGroupsByTarget_logGrpDumpBydsg(RoboconfOptions &rbcOpts, const std:: vector< ConnectionsGroup > &connGroups)
{
    UmbaTracyTraceScope();

    using namespace umba::omanip;

    LOG_MSG("grp-dump-bydsg")<<"Connections by target designator - initial (1)\n";
    for( const auto &logGrp : connGroups )
    {
        // logGrp.dstComponentInfo - ComponentInfo

        std::string emptyStr = "";
        if (logGrp.connections.empty())
            emptyStr = " - empty";

        LOG_MSG("grp-dump-bydsg")<<"Group: "<<logGrp.groupDesignator<<" - "<<logGrp.groupTitle<<emptyStr<<"\n";

        for( const auto &conn : logGrp.connections )
        {
            LOG_MSG("grp-dump-bydsg")<<"    "<<conn.dstPinDesignator<<" / "<<conn.srcNetName<<" / "<<conn.dstComponentInfo.typeName<<" (src designator: "<<conn.srcPinDesignator<<")\n";
        }
    }

    /*
    if (!ungrouppedConns.connections.empty())
    {
        LOG_MSG("grp-dump-bydsg")<<"Group: Ungrouped\n";

        for( const auto &conn : ungrouppedConns.connections )
        {
            LOG_MSG("grp-dump-bydsg")<<"    "<<conn.dstPinDesignator<<" / "<<conn.srcNetName<<" / "<<conn.dstComponentInfo.typeName<<" (src designator: "<<conn.srcPinDesignator<<")\n";
        }
    }
    */

    LOG_MSG("grp-dump-bydsg")<<"--------------------------------"<<endl;

}


inline
void splitConnectionsToGroupsByTarget( RoboconfOptions &rbcOpts, std:: vector< ConnectionsGroup > &connGroups, const std:: vector< Connection > &conns )
{
    UmbaTracyTraceScope();
    
    //using std::endl;
    using namespace umba::omanip;

    // Тут мы делаем разбиения имён цепей на части, для последующих кластеризаций
    // Таки нельзя тут, вектор по константной ссылке получаем, его нельзя модифицировать
    // for( auto &conn : conns )
    // {
    //     conn.splitMcuNetMakeTokens();
    // }


    // Группируем соединения по десигнатору назначения, складываем в map
    // Тут мы кладём по основному десигнатору, но могут быть и дополнительные, не так ли?
    // Особенно, когда мы подключаемся через разъём

    // Вызывается из
    // report_generic.h
    // report_periph.h

    // Там примерно такая последовательность
    // connectionsListBuild( rbcOpts, opts, netlistInfo, curMcuD, connectionList );
    // if (!processConnectionModifyRules( rbcOpts, connectionList, processingRules )) return;
    // connectionsListRemoveMcuDuplicates( connectionList );
    // std::vector< ConnectionsGroup > connGroups;
    // splitConnectionsToGroupsByTarget( rbcOpts, connGroups, connectionList );

    // У нас есть "важные" классы компонентов
    // std::unordered_set<ComponentClass> importantComponentClasses;
    // importantComponentClasses.insert(ComponentClass::cc_DD         );
    // importantComponentClasses.insert(ComponentClass::cc_DA         );
    // importantComponentClasses.insert(ComponentClass::cc_AA         );
    // importantComponentClasses.insert(ComponentClass::cc_TRANSISTOR );

    // Или завести мапу важности, где 0 - важно, 1 - не очень важно, 2 - неважно, 3 совсем не важно
    // Если у нас есть extraDestinations...
    // То что? Надо посмотреть в отладчике, что и как
    


    std::unordered_map< std::string , std:: vector<Connection> > connMap;
    splitConnectionsToGroupsByTarget_mcuNetMakeTokensAndPutToGroupMap(conns, connMap);

    ConnectionsGroup *pUngrouppedConns = splitConnectionsToGroupsByTarget_splitToGroupsSimple(connGroups, connMap);
    
    connMap.clear();

    splitConnectionsToGroupsByTarget_logGrpDumpBydsg(rbcOpts, connGroups);


/*
struct Connection //-V730
{
    std::string        interfaceType;      // need to find by rules
    std::string        interfaceLineType;  // need to find by rules
    bool               interfaceDetected = false;


    ComponentInfo      srcComponentInfo;
    std::string        srcPinDesignator;
    ComponentPinInfo   srcPinInfo;
    std::string        srcNetName;


    bool               netStop = false;
    int                netGroundType = 0;
    std::string        dstPinDesignator;

    ComponentInfo      dstComponentInfo;
    ComponentPinInfo   dstPinInfo;
*/

    // grp-dump-
    //LOG_MSG("net-dump-cmn")<<"--- Netlist: "<<netlist.second.name<<"\n";



    // Раскидываем по группам, одновременно выделяя кластеры в отдельные группы 
    // (помимо группировки по таргет десигнаторам)
    {
        std:: vector< ConnectionsGroup > connGroupsTmp;
    
        pUngrouppedConns = &connGroups[0];
        connGroupsTmp.emplace_back(*pUngrouppedConns);
        pUngrouppedConns = &connGroupsTmp[0];
    
        bool bFirstGrp = true;
        for( auto connGrp : connGroups )
        {
            if (bFirstGrp)
            {
                bFirstGrp = false;
                continue;
            }
    
            if (connGrp.connections.empty())
            {
                connGroupsTmp.emplace_back(connGrp);
                pUngrouppedConns = &connGroupsTmp[0];
                continue;
            }
    
            if (connGrp.dstComponentInfo.componentClass==ComponentClass::cc_QUARTZ)
            {
                // Кварц пропускаем
                continue;
            }
    
            if ( connGrp.dstComponentInfo.componentClass==ComponentClass::cc_TESTPOINT
              || connGrp.dstComponentInfo.componentClass==ComponentClass::cc_MOUNT
               )
            {
                // Тест точки и маунт холы пропускаем
                continue;
            }
    
            if (connGrp.dstComponentInfo.componentClass==ComponentClass::cc_HEADER)
            {
                // Кластеризуем хидеры
                splitConnectionGroupByClasters( rbcOpts, connGrp, connGroupsTmp, *pUngrouppedConns );
                continue;
            }
    
    
            // if ( connGrp.dstComponentInfo.componentClass!=ComponentClass::cc_TESTPOINT
            //   // && connGrp.dstComponentInfo.componentClass!=ComponentClass::cc_HEADER // А зачем я решил, что хидеры не надо кластерить по именам?
            //   && connGrp.dstComponentInfo.componentClass!=ComponentClass::cc_MOUNT
            //    )
            // {
            //     connGroupsTmp.emplace_back(connGrp);
            //     pUngrouppedConns = &connGroupsTmp[0];
            //     continue;
            // }
    
            // Остальное - кладём как есть
    
            connGroupsTmp.emplace_back(connGrp);
            pUngrouppedConns = &connGroupsTmp[0];
        }
    
        connGroupsTmp.swap(connGroups);
    
    }

    //connGroupsTmp.clear();
    pUngrouppedConns = &connGroups[0];

    LOG_MSG("grp-dump-clust")<<"Connections by target designator - clastered (2)\n";
    for( const auto &logGrp : connGroups )
    {
        std::string emptyStr = "";
        if (logGrp.connections.empty())
            emptyStr = " - empty";

        LOG_MSG("grp-dump-clust")<<"Group: "<<logGrp.groupDesignator<<" - "<<logGrp.groupTitle<<emptyStr<<"\n";
        // logGrp.dstComponentInfo - ComponentInfo

        for( const auto &conn : logGrp.connections )
        {
            LOG_MSG("grp-dump-clust")<<"    "<<conn.dstPinDesignator<<" / "<<conn.srcNetName<<" / "<<conn.dstComponentInfo.typeName<<" (src designator: "<<conn.srcPinDesignator<<")\n";
        }
    }

    

    /*
    if (!ungrouppedConns.connections.empty())
    {
        LOG_MSG("grp-dump-clust")<<"Group: Ungrouped\n";

        for( const auto &conn : ungrouppedConns.connections )
        {
            LOG_MSG("grp-dump-clust")<<"    "<<conn.dstPinDesignator<<" / "<<conn.srcNetName<<" / "<<conn.dstComponentInfo.typeName<<" (src designator: "<<conn.srcPinDesignator<<")\n";
        }
    }
    */
    LOG_MSG("grp-dump-clust")<<"--------------------------------"<<endl;


    //pUngrouppedConns->updateConnectionsGroupingOptions( rbcOpts );
    for( auto &connGrp : connGroups )
    {
        connGrp.updateConnectionsGroupingOptions( rbcOpts );
    }



    //ConnectionsGroup ungrouppedConnsTmp;

    // 1) Нужно пробежаться по именованым группам, и выцепить пины, которые нужно ungroup
    // 2) Нужно пробежаться по безымянной группе и сгрупировать пины, которые нужно group

    //std::map< std::string , std:: vector<Connection> > connMap;


    // Ungroup grouped
    LOG_MSG("grp-log-fungrp-start")<<"--------------------------------"<<endl;
    LOG_MSG("grp-log-fungrp-start")<<"Start force ungroupping"<<endl;

    {
        bool bFirstGrp = true;
        for( auto &connGrp : connGroups )
        {
            if (bFirstGrp)
            {
                bFirstGrp = false;
                continue;
            }
    
            for( size_t connIdx = 0; connIdx!=connGrp.connections.size(); )
            {
                auto conn = connGrp.connections[connIdx];
                if (conn.groupingRuleType==ForceGroupingRule::forceUngroup)
                {
                    pUngrouppedConns->connections.push_back(conn);
                    connGrp.connections.erase(pUngrouppedConns->connections.begin()+connIdx);
                }
                else
                {
                    ++connIdx;
                }
            }
        }
    }

// struct ConnectionsGroup //-V730
// {
//     std::string                  groupDesignator; // target designator for all of group items
//     std::string                  groupTitle;      // target designator for all of group items
//     ComponentInfo                dstComponentInfo;
//     std:: vector< Connection >   connections;
//     bool                         ungroupped = false;
//     bool                         forceNamed = false;;
    
    // Group ungrouped
    LOG_MSG("grp-log-fgrp-start")<<"--------------------------------"<<endl;
    LOG_MSG("grp-log-fgrp-start")<<"Start force groupping"<<endl;
    for( size_t connIdx = 0; connIdx!=pUngrouppedConns->connections.size(); )
    {
        auto conn = pUngrouppedConns->connections[connIdx];
        if (conn.groupingRuleType==ForceGroupingRule::forceGroup)
        {
            std::string grpName = conn.forceGroupName;
            //if (grpName.empty())
            //    grpName = "GROUP_AUTO";
            connMap[grpName].push_back( conn );
            pUngrouppedConns->connections.erase(pUngrouppedConns->connections.begin()+connIdx);
        }
        else
        {
            ++connIdx;
        }
    }

    // Create new groups
    for( const auto& connKV : connMap )
    {
        if (connKV.second.size() < 1)
        {
            continue;
        }
        else
        {
            bool foundExistingGroup = false;
            for( auto &grp : connGroups )
            {
                if (grp.groupTitle==connKV.first)
                {
                    grp.connections.insert( grp.connections.end(), connKV.second.begin(), connKV.second.end() );
                    foundExistingGroup = true;
                    break;
                }
            }

            if (!foundExistingGroup)
            {
                ConnectionsGroup grp;
                grp.groupDesignator  = connKV.second[0].dstComponentInfo.designator;
                grp.dstComponentInfo = connKV.second[0].dstComponentInfo;
                grp.connections      = connKV.second;
                grp.groupTitle       = connKV.first; // grp.tryGenerateTitle( );
                if (grp.groupTitle.empty())
                    grp.groupTitle = grp.tryGenerateTitle();
                grp.forceNamed       = true;
                connGroups.emplace_back(grp);
                pUngrouppedConns = &connGroups[0];
            }

        }
    }

    LOG_MSG("grp-dump-fgrp")<<"Connections by target designator - force grouped/ungrouped\n";
    for( const auto &logGrp : connGroups )
    {
        std::string emptyStr = "";
        if (logGrp.connections.empty())
            emptyStr = " - empty";

        LOG_MSG("grp-dump-fgrp")<<"Group: "<<logGrp.groupDesignator<<" - "<<logGrp.groupTitle<<emptyStr<<"\n";
        // logGrp.dstComponentInfo - ComponentInfo

        for( const auto &conn : logGrp.connections )
        {
            LOG_MSG("grp-dump-fgrp")<<"    "<<conn.dstPinDesignator<<" / "<<conn.srcNetName<<" / "<<conn.dstComponentInfo.typeName<<" (src designator: "<<conn.srcPinDesignator<<")\n";
        }
    }

    /*
    if (!ungrouppedConns.connections.empty())
    {
        LOG_MSG("grp-dump-fgrp")<<"Group: Ungrouped\n";

        for( const auto &conn : ungrouppedConns.connections )
        {
            LOG_MSG("grp-dump-fgrp")<<"    "<<conn.dstPinDesignator<<" / "<<conn.srcNetName<<" / "<<conn.dstComponentInfo.typeName<<" (src designator: "<<conn.srcPinDesignator<<")\n";
        }
    }
    */
    LOG_MSG("grp-dump-fgrp")<<"--------------------------------"<<endl;



/*
            ConnectionsGroup grp;
            grp.groupDesignator  = connKV.second[0].dstComponentInfo.designator;
            grp.dstComponentInfo = connKV.second[0].dstComponentInfo;
            grp.connections      = connKV.second;
            grp.groupTitle       = grp.tryGenerateTitle( );

            connGroups.emplace_back(grp);

*/

    //int                                  groupingRuleType;
    //std::string                          forceGroupName;

/*

    std::string                  groupTitle;      // target designator for all of group items
    std:: vector< Connection >    connections;
    //int                                  groupingRuleType;
    //std::string                          forceGroupName;

*/


    {
        struct GroupsConnections
        {
            std::unordered_set< size_t >   groupNums;
            std:: vector<Connection>       connections;
            //ConnectionsGroup            group;
        };

        // Группируем по пину проца - хотя такое не часто бывает
        std::unordered_map< std::string, GroupsConnections >  bySourceDesignators;
       
        for( size_t grpNo = 0; grpNo != connGroups.size(); ++grpNo )
        {
            const ConnectionsGroup &grp = connGroups[grpNo];
       
            for( const auto & conn : grp.connections )
            {
                bySourceDesignators[conn.srcPinDesignator].groupNums.insert(grpNo);
                //bySourceDesignators[conn.srcPinDesignator].group.connections.push_back(conn);
                bySourceDesignators[conn.srcPinDesignator].connections.push_back(conn);
            }
        }
        // Бага - не классифицированные пины не обрабатываются

        //std::map< std::set<size_t>, ConnectionsGroup > newGroups;

        bool hasMultipleSourceDesignators = false;


        // Проверяем, есть ли src десигнаторы с более чем одним подключенным таргетом
        std::unordered_map< std::string, GroupsConnections >::iterator bsdIt = bySourceDesignators.begin();
        for(; bsdIt != bySourceDesignators.end(); ++bsdIt)
        {
            if (bsdIt->second.connections.size()<2) //!!! Was bag - groupNums instead of connections
                continue;
            hasMultipleSourceDesignators = true;
            break;
        }

        // Надо вывести в лог
        if (hasMultipleSourceDesignators)
        {
            LOG_MSG("grp-dump-bysrc")<<"Multiple connected source designators\n";
            bsdIt = bySourceDesignators.begin();
            for(; bsdIt != bySourceDesignators.end(); ++bsdIt)
            {
                //if (bsdIt->second.groupNums.size()<2)
                if (bsdIt->second.connections.size()<2)
                    continue;

                //LOG_MSG("grp-dump-bysrc")<<"Multiple connected source designators\n";
                LOG_MSG("grp-dump-bysrc")<<"From groups:\n";

                //std::ostringstream oss;
                for( auto gi : bsdIt->second.groupNums )
                {
                    const ConnectionsGroup &grp = connGroups[gi];
                    LOG_MSG("grp-dump-bysrc")<<"  Group: "<<grp.groupDesignator<<" - "<<grp.groupTitle<<"\n";
                }

                for( const auto &conn : bsdIt->second.connections )
                {
                    LOG_MSG("grp-dump-bysrc")<<"    "<<conn.dstPinDesignator<<" / "<<conn.srcNetName<<" / "<<conn.dstComponentInfo.typeName<<" (src designator: "<<conn.srcPinDesignator<<")\n";
                }
            }
        }

        // Логика интересная - если на одном пине проца висит несколько периферий, то мы из каждой группы удаляем 
        // такое соединение, и цепляем к нему все добавочные соединения
        // И, у нас если есть несколько таких периферий, и они входили ранее в одинаковые группы, то мы их сводим в одну новую
        // Нафига, спрашивается?
        //----------------------
        // Переделаем
        // Сделаем вот как. Если соединение входит в несколько групп, то перевещаем его из группы меньшего размера в группу большего размера

        bsdIt = bySourceDesignators.begin();
        for(; bsdIt != bySourceDesignators.end(); ++bsdIt)
        {
            //if (bsdIt->second.groupNums.size()<2)
            if (bsdIt->second.connections.size()<2)
                continue;

            if (bsdIt->second.groupNums.size()<2)
                continue;


            size_t primaryGrpNo   = (size_t)-1;
            size_t primaryGrpSize = 0;

            for( auto grpNo : bsdIt->second.groupNums )
            {
                if (grpNo==0)
                    continue; // unsorted group can't be  primary

                size_t testSz = connGroups[grpNo].connections.size();
                if (primaryGrpNo == (size_t)-1)
                {
                    primaryGrpNo   = grpNo;
                    primaryGrpSize = testSz;
                }
                else if (primaryGrpSize < testSz )
                {
                    primaryGrpNo   = grpNo;
                    primaryGrpSize = testSz;
                }
            }

            // Соединения нужно удалить из всех групп, кроме primary группы
            // Но только если primary группа найдена
            if (primaryGrpNo != (size_t)-1)
            {
                for( auto grpNo : bsdIt->second.groupNums )
                {
                    if (primaryGrpNo==grpNo)
                        continue;

                    for( const auto & conn : bsdIt->second.connections )
                    {
                        if (connGroups[grpNo].removeConnectionByDstPinDesignator( conn.dstPinDesignator ))
                            connGroups[primaryGrpNo].connections.push_back(conn);
                    }
                }
            }

            //removeConnectionByDstPinDesignator( const std::string &pd )

            /*
            //hasMultipleSourceDesignators = true;

            std::map< std::set<size_t>, ConnectionsGroup >::iterator ngIt = newGroups.find(bsdIt->second.groupNums);
            if (ngIt == newGroups.end())
            {
                ConnectionsGroup newGroupTemplate;
                if (!bsdIt->second.connections.empty())
                {
                    newGroupTemplate.groupDesignator  = bsdIt->second.connections[0].dstComponentInfo.designator;
                    newGroupTemplate.dstComponentInfo = bsdIt->second.connections[0].dstComponentInfo;
                    //newGroups[bsdIt->second.groupNums] = newGroupTemplate;
                }

                newGroups[bsdIt->second.groupNums] = newGroupTemplate;
            } // if (ngIt == newGroups.end())


            std::string rmDesignator;

            ngIt = newGroups.find(bsdIt->second.groupNums);
            auto connIt = bsdIt->second.connections.begin();
            if (connIt != bsdIt->second.connections.end())
            {
                auto connIt2 = connIt;
                connIt2++;
                connIt->extraDestinations.insert( connIt->extraDestinations.end(), connIt2, bsdIt->second.connections.end() );

                ngIt->second.connections.emplace_back(*connIt);

                rmDesignator = connIt->srcPinDesignator;
            }

            if (!rmDesignator.empty())
            {
                auto grpNumsIt = bsdIt->second.groupNums.begin();
                for(; grpNumsIt != bsdIt->second.groupNums.end(); ++grpNumsIt )
                {
                    size_t grpNo = *grpNumsIt;
                    connGroups[grpNo].removeConnectionBySrcPinDesignator( rmDesignator );
                }
            }
            */

        } // for(; bsdIt != bySourceDesignators.end(); ++bsdIt)

        //std::map< std::set<size_t>, ConnectionsGroup > newGroups;
        /*
        for( auto newGrpKV : newGroups )
        {
            connGroups.emplace_back(newGrpKV.second);
        }

        //connGroupsTmp

        for( auto &grp : connGroups )
        {
            if (grp.connections.size()<2) 
            {
                if (!grp.connections.empty())
                    ungrouppedConns.connections.emplace_back(grp.connections[0]);
            }
            else
            {
                grp.groupTitle = grp.tryGenerateTitle( );
                connGroupsTmp.emplace_back(grp);
            }
        }

        connGroupsTmp.swap(connGroups);
        */

    }

    // extraDestinations

    for( auto & grp : connGroups )
    {
        std::stable_sort( grp.connections.begin(), grp.connections.end(), CompareConnectionsByDstComponentWeightGreater() );
        moveConnectionDuplicatesToExtra( grp.connections );
    }

    //CompareConnectionsByDstComponentWeightGreater

    if (!connGroups.empty())
        std::stable_sort( connGroups.begin()+1, connGroups.end(), ConnectionGroupLessByTitle );
    //ConnectionGroupGreaterByTitle

    // connGroupsTmp.clear();

    // Merge groups with same names
    {
        std:: vector< ConnectionsGroup > connGroupsTmp;
        for( const auto & grp : connGroups )
        {
            if (grp.connections.empty()) // also remove empty groups
                continue;
    
            //const ConnectionsGroup &grp = connGroups[grpNo];
            bool bAppended = false;
            for( auto & grpTmp : connGroupsTmp )
            {
                if (grpTmp.groupTitle == grp.groupTitle)
                {
                    bAppended = true;
                    grpTmp.connections.insert( grpTmp.connections.end(), grp.connections.begin(), grp.connections.end() );
                    break;
                }
            }
    
            if (!bAppended)
            {
                connGroupsTmp.push_back(grp);
            }
        }
    
        connGroupsTmp.swap(connGroups);

    }
/*

struct ConnectionsGroup
{
    std::string                  groupDesignator; // target designator for all of group items
    std::string                  groupTitle;      // target designator for all of group items
    ComponentInfo                dstComponentInfo;
    std:: vector< Connection >    connections;
    bool                         ungroupped = false;

*/

    //moveConnectionDuplicatesToExtra( ungrouppedConns.connections );

    //connGroups.emplace_back(ungrouppedConns);

    for( auto &grp : connGroups )
    {
        stable_sort( grp.connections.begin(), grp.connections.end(), CompareConnectionsBySrcNetNameWithIntsLess() );
    }

}

//-----------------------------------------------------------------------------
















//-----------------------------------------------------------------------------
inline
bool ConnectionLessBySheet( const Connection& c1, const Connection& c2 )
{
    return c1.dstComponentInfo.sheetName < c2.dstComponentInfo.sheetName;
}

//-----------------------------------------------------------------------------
inline
bool ConnectionGreaterBySheet( const Connection& c1, const Connection& c2 )
{
    return c1.dstComponentInfo.sheetName > c2.dstComponentInfo.sheetName;
}

//-----------------------------------------------------------------------------
inline
void groupConnectionsBySheet( std:: vector< Connection > &conns )
{
    std::stable_sort( conns.begin(), conns.end(), ConnectionLessBySheet );
}



//-----------------------------------------------------------------------------
inline
void findStartConnectionsDesignators( const NetlistInfo &netlist, const std::string &purpose, std:: vector< std::string > &designators )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    std::unordered_set<ComponentClass> componentClasses;
    componentClasses.insert(ComponentClass::cc_DD);
    netlist.traverseComponents( ComponentPurposeDesignatorsCollector(designators,purpose), componentClasses, false );
}

//-----------------------------------------------------------------------------
inline
void connectionsListBuild_WalkTroughNets( RoboconfOptions &rbcOpts
                                        , const ConnectionBuildingOptions &buildingOptions
                                        , const NetlistInfo               &netlist
                                        , std::unordered_set<std::string> usedNets
                                        , std::unordered_set<std::string> usedDsgs
                                        , const std::string &dsgFrom
                                        , Connection conn
                                        , std:: vector<Connection> &connectionList
                                        )
{
    UmbaTracyTraceScope();
    
    auto netsDsgFromIt = netlist.designators.find(dsgFrom);

    if (netsDsgFromIt==netlist.designators.end())
        return; // no entry for designator - not connected?

    size_t dsgProcessedNetCount = 0;

    for( const auto & netName : netsDsgFromIt->second )
    {
        auto netKV = netlist.nets.find(netName);
        if (netKV==netlist.nets.end())
            continue; // unknown net

        if (buildingOptions.isStopNet( rbcOpts, netName))
        {
            conn.dstPinDesignator        = netName;
            conn.dstComponentInfo.description = buildingOptions.getStopNetDescription(netName);
            //conn.dstComponentDescription = buildingOptions.getStopNetDescription(netName);
            conn.netStop                 = true;
            conn.netGroundType           = buildingOptions.getStopNetGroundOption(netName);
            connectionList.emplace_back(conn);
            return;
        }

        if (usedNets.find(netName)!=usedNets.end())
            continue; // visited
        usedNets.insert(netName);

        dsgProcessedNetCount++;

        auto &netInfo = netKV->second;

        if (conn.srcNetName.empty())
            conn.srcNetName = netInfo.name; // first found net

        if (!netInfo.payload.empty())
           conn.payloads.emplace_back(netInfo.payload);

        for( const auto & nodeKV : netInfo.nodes )
        {
            if (nodeKV.second.targetDesignatorFull == dsgFrom)
                continue;
            connectionsListBuild_WalkTroughNets( rbcOpts, buildingOptions, netlist, usedNets, usedDsgs
                                               , nodeKV.second.targetDesignatorFull
                                               , conn, connectionList
                                               );
        }
    }

    if (!dsgProcessedNetCount)
    {
        // end point
        conn.dstPinDesignator = dsgFrom; // if equals to srcDesignator - this means that src not connected
        std::string dsgComponent, dsgPinNo;
        splitToPairFromEnd( dsgFrom, dsgComponent, dsgPinNo, "." );

        typename NetlistInfo::components_map_type::const_iterator cit = netlist.components.find(dsgComponent);
        if (cit == netlist.components.end())
        {
            // component not found
            conn.dstPinDesignator = std::string("<UNKNOWN> ") + conn.dstPinDesignator;
        }
        else
        {
            //unsigned dsgPinNo = (unsigned)parseIntegerTolerant( dsgPinNoStr, 10 );

            conn.dstComponentInfo = cit->second;

            //conn.dstComponentSheet    =  cit->second.sheetName;
            //conn.dstComponentType     =  cit->second.typeName;
            //conn.dstComponentPurpose  =  cit->second.purpose;
            //conn.dstComponentClass    =  cit->second.componentClass;
            //conn.dstComponentClassStr =  cit->second.getComponentClassString();
            //conn.dstComponentDescription =  cit->second.description;

            //std::map< std::string, ComponentPinInfo >
            typename ComponentInfo::pins_map_type::const_iterator pit = cit->second.pins.find(dsgPinNo);

            if (pit != cit->second.pins.end())
                conn.dstPinInfo = pit->second;

            connectionList.emplace_back(conn);
        }

    }

}

//-----------------------------------------------------------------------------

inline
void connectionsListBuild( RoboconfOptions &rbcOpts
                         , const ConnectionBuildingOptions &buildingOptions
                         , const NetlistInfo &netlist
                         , const std::string &startDesignator
                         , std:: vector<Connection> &connectionList
                         )
{
    UmbaTracyTraceScope();
    
    auto componentKV = netlist.components.find( startDesignator );
    if (componentKV==netlist.components.end())
        return;

    const ComponentInfo &componentInfo = componentKV->second;
    for( const auto & componentPinKV : componentInfo.pins ) // std::map< unsigned, ComponentPinInfo > pins;
    {
        const auto &pinInfo = componentPinKV.second;

        if (pinInfo.pinFunctions.find("DONT_ANALYSE")!=pinInfo.pinFunctions.end())
            continue; // skip analyzing by key pin function

        Connection conn;
        conn.srcComponentInfo = componentInfo;

        {
            std::stringstream tmp;
            tmp<<startDesignator<<"."<<pinInfo.pinNo;
            conn.srcPinDesignator = tmp.str();
        }
        
        conn.srcPinInfo    = pinInfo;

        connectionsListBuild_WalkTroughNets( rbcOpts, buildingOptions, netlist, std::unordered_set<std::string>(), std::unordered_set<std::string>()
                                           , conn.srcPinDesignator, conn, connectionList
                                           );

    }

    //return;

    std:: vector<Connection> tmpTargets; tmpTargets.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    std:: vector<Connection> tmpNets; tmpNets.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    for( const auto& conn : connectionList )
    {
        if (!conn.netStop)
        {
            if (conn.dstComponentInfo.componentClass== ComponentClass::cc_CAPACITOR)
                continue;                              // skip filter CAPACITORs
            if (conn.dstComponentInfo.componentClass== ComponentClass::cc_QUARTZ)
                continue;                              // skip filter QUARTZs

            tmpTargets.emplace_back(conn);
        }
        else
            tmpNets.emplace_back(conn);
    }

    connectionList.clear();

    for( const auto& netConn : tmpNets )
    {
        if (netConn.netGroundType!=CommonNetInfo::NET_GROUND_OPTION_GND && netConn.netGroundType!=CommonNetInfo::NET_GROUND_OPTION_VCC_PLUS)
        {
            connectionList.emplace_back(netConn); // connection to unrecognized common
            continue;
        }

        
        for( auto& tgtConn : tmpTargets )
        {
            if (tgtConn.srcPinDesignator!=netConn.srcPinDesignator)
                continue;

            if (netConn.netGroundType==CommonNetInfo::NET_GROUND_OPTION_GND)
            {
                if (netConn.payloads.empty())
                    tgtConn.srcPinInfo.pinFunctions.insert("__SHORTGND");
                else
                    tgtConn.srcPinInfo.pinFunctions.insert("__PULLDOWN");
            }
            else // CommonNetInfo::NET_GROUND_OPTION_VCC_PLUS
            {
                if (netConn.payloads.empty())
                    tgtConn.srcPinInfo.pinFunctions.insert("__SHORTVCC");
                else
                    tgtConn.srcPinInfo.pinFunctions.insert("__PULLUP");
            }
        }
    }

    connectionList.insert(connectionList.end(), tmpTargets.begin(), tmpTargets.end());

    for( auto & conn : connectionList )
    {
        if ( conn.dstComponentInfo.purpose.empty()
           && ( conn.dstComponentInfo.componentClass==ComponentClass::cc_HEADER
             || conn.dstComponentInfo.componentClass==ComponentClass::cc_TESTPOINT
             || conn.dstComponentInfo.componentClass==ComponentClass::cc_MOUNT
             // || conn.dstComponentInfo.componentClass == ComponentClass::cc_LED //NOTE: !!! А почему это у нас LEDу ничего не задаётся?
              )
           )
        {
            conn.dstComponentInfo.purpose = conn.dstComponentInfo.getComponentClassString();
        }

    }

}

//-----------------------------------------------------------------------------
inline
void connectionsListRemoveSameTargetDesignatorDuplicates( std:: vector<Connection> &connectionList )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    std::unordered_set< std::string > removedPinFunctions;

    // keep dst with same pin functions
    if (connectionList.size()>1)
    {
        //std::unordered_set< std::string > rpfn; // local removedPinFunctions

        std:: vector<Connection> tmp; tmp.reserve(connectionList.size());  // tmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

        for( auto conn : connectionList )
        {
            if ( !pinFunctionIsContain( conn.processedStrings["MCUNET"], conn.dstPinInfo.pinFunctions ) )
            {
                //rpfn.insert( conn.dstPinInfo.pinFunctions.begin(), conn.dstPinInfo.pinFunctions.end() );
                removedPinFunctions.insert( conn.dstPinInfo.pinFunctions.begin(), conn.dstPinInfo.pinFunctions.end() );
                continue;
            }

            tmp.push_back(conn);
        }

        if (!tmp.empty())
        {
            connectionList.swap(tmp);
            //removedPinFunctions.insert( rpfn.begin(), rpfn.end() );
        }
    }

    // remove dups, keep first
    if (connectionList.size()>1)
    {
        std:: vector<Connection>::iterator it = connectionList.begin();
        ++it;
        for(; it != connectionList.end(); ++it)
        {
            removedPinFunctions.insert( it->dstPinInfo.pinFunctions.begin(), it->dstPinInfo.pinFunctions.end() );
        }

        connectionList.erase( connectionList.begin()+1, connectionList.end() );
    }


    for( auto & conn : connectionList )
    {
        conn.dstPinInfo.pinFunctions.insert( removedPinFunctions.begin(), removedPinFunctions.end() );
    }

}

//-----------------------------------------------------------------------------
inline
void connectionsListRemoveSameSourceDesignatorDuplicates( std:: vector<Connection> &connectionList )
{
    UmbaTracyTraceScope();

    std::set< std::string > removedPinFunctions;

    // Remove headers
    if (connectionList.size()>1)
    {
        std:: vector<Connection> tmp; tmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

        std::set< std::string > rpfn; // local removedPinFunctions

        for( auto conn : connectionList )
        {
            if ( conn.dstComponentInfo.componentClass==ComponentClass::cc_HEADER
              || conn.dstComponentInfo.componentClass==ComponentClass::cc_TESTPOINT
              || conn.dstComponentInfo.componentClass==ComponentClass::cc_MOUNT
               )
            {
                rpfn.insert( conn.dstPinInfo.pinFunctions.begin(), conn.dstPinInfo.pinFunctions.end() );
                continue;
            }

            tmp.push_back(conn);
        }

        if (!tmp.empty())
        {
            connectionList.swap(tmp);
            removedPinFunctions.insert( rpfn.begin(), rpfn.end() );
        }
    }

    // Remove ignored pins
    if (connectionList.size()>1)
    {
        std:: vector<Connection> tmp; tmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

        for( auto conn : connectionList )
        {
            if ( conn.dstPinInfo.isPinFunction("PINTYPE_IGNORE") )
                continue;

            tmp.push_back(conn);
        }

        if (!tmp.empty())
            connectionList.swap(tmp);
    }

    // Remove too much payloaded pins
    if (connectionList.size()>1)
    {
        std:: vector<Connection> tmp; tmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

        for( auto conn : connectionList )
        {
            if ( conn.payloads.size()>1 )
                continue;

            tmp.push_back(conn);
        }

        if (!tmp.empty())
            connectionList.swap(tmp);
    }


    if (connectionList.size()>1)
    {
        std::map< std::string, std:: vector<Connection> > byTargetDessignator;
        for( auto conn : connectionList )
        {
            byTargetDessignator[conn.dstComponentInfo.designator].emplace_back(conn);
        }

        std:: vector<Connection> tmp; tmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

        for( auto &connKV : byTargetDessignator)
        {
            connectionsListRemoveSameTargetDesignatorDuplicates(connKV.second);
            tmp.insert( tmp.end(), connKV.second.begin(), connKV.second.end() );
        }

        /*
        for( auto conn : connectionList )
        {
            std:: vector<Connection> &v = byTargetDessignator[conn.dstComponentInfo.designator];
            tmp.insert( tmp.end(), v.begin(), v.end() );
        }
        */
        if (!tmp.empty())
        {
            connectionList.swap(tmp);
            //removedPinFunctions.insert( rpfn.begin(), rpfn.end() );
        }
    }


    // update dstPinFunctions with removed values
    for( auto & conn : connectionList )
    {
        conn.dstPinInfo.pinFunctions.insert( removedPinFunctions.begin(), removedPinFunctions.end() );
    }

    size_t headersCount = 0;
    for( const auto & conn : connectionList )
    {
        if ( conn.dstComponentInfo.componentClass==ComponentClass::cc_HEADER 
          || conn.dstComponentInfo.componentClass==ComponentClass::cc_TESTPOINT
          || conn.dstComponentInfo.componentClass==ComponentClass::cc_MOUNT
           )
           {
            headersCount++;
           }
    }

    if (headersCount==connectionList.size())
    {
        connectionList.erase( connectionList.begin()+1, connectionList.end() );
    }

}

/*
    ComponentInfo      srcComponentInfo;
    std::string        srcPinDesignator;
    ComponentPinInfo   srcPinInfo;
    std::string        srcNetName;


    bool               netStop = false;
    int                netGroundType = 0;
    std::string        dstPinDesignator;

    ComponentInfo      dstComponentInfo;
    ComponentPinInfo   dstPinInfo;
    std::string        dstComponentSheet;
    std::string        dstComponentType;
    std::string        dstComponentPurpose;
    unsigned           dstComponentClass;
    std::string        dstComponentClassStr;
    std::string        dstComponentDescription;

    std:: vector< std::string> payloads;

    std::map< std::string, std::string > processedStrings;

*/

//-----------------------------------------------------------------------------
inline
void connectionsListRemoveMcuDuplicates( std:: vector<Connection> &connectionList )
{
    UmbaTracyTraceScope();

    std::unordered_map< std::string, std:: vector<Connection> > connMap;

    for( auto conn : connectionList )
    {
        connMap[conn.srcPinDesignator].emplace_back(conn);
    }

    for( auto &connKV : connMap )
    {
        connectionsListRemoveSameSourceDesignatorDuplicates( connKV.second );
    }

    std:: vector<Connection> res; res.reserve(connectionList.size()); // res.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    std::set<std::string>   usedDesignators;
    for( auto conn : connectionList )
    {
        if ( usedDesignators.find(conn.srcPinDesignator)!=usedDesignators.end() )
            continue;
        usedDesignators.insert(conn.srcPinDesignator);

        //if (!connMap[conn.srcPinDesignator].empty())
        //    res.emplace_back( connMap[conn.srcPinDesignator][0] );
        const auto &dsgVec = connMap[conn.srcPinDesignator];
        res.insert( res.end(), dsgVec.begin(), dsgVec.end() );
    }

    connectionList.swap(res);
}

//-----------------------------------------------------------------------------
bool processConnectionModifyRules( RoboconfOptions &rbcOpts, Connection &conn, const ExpressionItem & rule );
bool processConnectionModifyRules( RoboconfOptions &rbcOpts, Connection &conn, const expression_list_t &processingRules );

//-----------------------------------------------------------------------------
inline
bool processConnectionModifyRules( RoboconfOptions &rbcOpts, std:: vector<Connection> &connectionList, const expression_list_t &processingRules )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    for( auto & conn : connectionList )
    {
        if (!processConnectionModifyRules( rbcOpts, conn, processingRules ))
            return false;
    }
    return true;
}

//-----------------------------------------------------------------------------
inline
bool processConnectionModifyRules( RoboconfOptions &rbcOpts, Connection &conn, const expression_list_t &processingRules )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    conn.processedStrings["MCUNET"]      = conn.srcNetName;
    conn.processedStrings["UNITTYPE"]    = conn.dstComponentInfo.typeName;
    conn.processedStrings["UNITPURPOSE"] = conn.dstComponentInfo.purpose;
    conn.processedStrings["UNITCLASS"]   = conn.dstComponentInfo.getComponentClassString();
    //conn.processedStrings[""] = conn.;

    for( const auto& rule: processingRules )
    {
        if (!processConnectionModifyRules( rbcOpts, conn, rule ))
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
inline
bool processConnectionModifyRules(RoboconfOptions &rbcOpts, Connection &conn, const ExpressionItem & rule /* modifyRule */ )
{
    UmbaTracyTraceScope();

    if (rule.isText())
        return true;

    const expression_list_t &lst = rule.itemList;
    expression_list_t::const_iterator it = lst.begin();

/*
(modify "mcuNet" replace "(.+)_(\d+)(.*)" "$({1})$({2})$({3})")

(modify "mcuNet" toUpper)
(modify "mcuNet" toLower)

(// expand MOTX abbr to full MOTORX term)
(modify "mcuNet" replaceSingle "(.*)MOT(\d+)(.*)" "$({1})MOTOR$({2})$({3})")

*/

    std:: vector< std::string > readedVals; readedVals.reserve(16);
    std::string expected;
    std::string found;
    bool readFieldsRes = readListByTemplate( "Ti!:modify;V;Ti:replace,replaceSingle,toUpper,toLower"
                                           , it, lst.end()
                                           , readedVals
                                           , expected
                                           , found
                                           );
    if (!readFieldsRes)
    {
        LOG_ERR_OBJ(rule)<<"'modify' processing rule: expected "<<expected<<", but found "<<found<<"\n";
        return false;
    }

    const std::string verb = readedVals[0];

    if (verb!="modify")
        return true;

    std::string whatModify = toUpper(readedVals[1]);
    if (whatModify=="NETNAME")
        whatModify = "MCUNET";

    auto whatIt = conn.processedStrings.find( whatModify );
    if (whatIt==conn.processedStrings.end())
    {
       LOG_ERR_OBJ(rule)<<"'modify' processing rule: variable '"<<readedVals[1]<<"' not found\n";
       return false;
    }


    std::string &valWhichToModify = whatIt->second;

    const std::string whatToDo = readedVals[2];
    //std::string modifyHow
    if (whatToDo=="toupper")
    {
        whatIt->second = toUpper(valWhichToModify);
        return true;
    }
    else if (whatToDo=="tolower")
    {
        whatIt->second = toLower(valWhichToModify);
        return true;
    }

    // Processing here replace,replaceSingle

    readFieldsRes = readListByTemplate( "V;V;"
                                      , it, lst.end()
                                      , readedVals
                                      , expected
                                      , found
                                      );
    if (!readFieldsRes)
    {
        LOG_ERR_OBJ(rule)<<"'modify' processing rule: expected "<<expected<<", but found "<<found<<"\n";
        return false;
    }

    const std::string &matchExpression = readedVals[3];
    const std::string &replaceText     = readedVals[4];

    // replace, replaceSingle
    size_t i = 0, maxIterations = valWhichToModify.size();
    if (whatToDo=="replacesingle")
        maxIterations = 1;

    for( ; i!=maxIterations; ++i)
    {
        std::string strRes;
        if (!regexpEvalString( strRes, matchExpression, replaceText, valWhichToModify, string_string_map_type() ) )
            break;
        whatIt->second = strRes;
    }


    return true;
}

//-----------------------------------------------------------------------------







//-----------------------------------------------------------------------------
struct ExternalDevicePinConnectInfo
{
    // input
    std::string    devicePinName;
    std::string    connectTargetPinDesignator; // in full form DD.N
    // extracted from devicePinName and ComponentPinInfo from preloaded ComponentInfo
    std::string    devicePinNo;


    // device designator extracted from connectTargetPinDesignator
    std::string    connectTargetDeviceDesignator;
    std::string    connectTargetPin;
    // extracted from connectTargetPin and ComponentPinInfo from ComponentInfo from netlist
    std::string    connectTargetPinNo;


    ExternalDevicePinConnectInfo( const std::string &dpn = std::string(), const std::string &ctp = std::string())
    : devicePinName(dpn)
    , connectTargetPinDesignator(ctp)
    {
        if (!connectTargetPinDesignator.empty())
            splitToPairFromEnd( connectTargetPinDesignator, connectTargetDeviceDesignator, connectTargetPin, "." );

        devicePinNo        = devicePinName; // (unsigned)parseIntegerTolerant( devicePinName, 10 ); // try to parse pin no
        connectTargetPinNo = connectTargetPin; // (unsigned)parseIntegerTolerant( connectTargetPin, 10 ); // try to parse pin no
    }

    bool isDevicePinTakenByName() const
    {
        uint64_t u = 0;
        return !parseInteger( devicePinName, u, 10 );
    }

    bool isConnectTargetPinTakenByName() const
    {
        uint64_t u = 0;
        return !parseInteger( connectTargetPin, u, 10 );
    }

}; // struct ExternalDevicePinConnectInfo

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
struct ExternalDeviceConnectInfo
{
    std::string deviceName;
    std::string devicePackage;
    std::string designatorType;

    std:: vector<ExternalDevicePinConnectInfo> connectionPins;

    std::string netlistName;

    FileSet::file_id_t fileNo = (FileSet::file_id_t)-1;
    size_t             lineNo = 0;


    bool isComponentDefinitionRequired() const
    {
        for( auto pin : connectionPins)
        {
            if (pin.isDevicePinTakenByName())
                return true;
        }

        return false;
    }

    bool updateDevicePinNumbers( const ComponentInfo &ci, std::string &failedPinFn )
    {
        for( auto& dcPin : connectionPins )
        {
            if (!dcPin.isDevicePinTakenByName())
                continue;

            if ( !ci.findPinNoByFunction( dcPin.devicePinName, dcPin.devicePinNo ) )
            {
                failedPinFn = dcPin.devicePinName;
                return false;
            }
        }
/*
        for( auto& dcPin : connectionPins )
        {
            connectionPinsByTarget[dcPin.connectTargetDeviceDesignator].emplace_back(dcPin);
        }
*/        
        return true;

    }

    bool findAllTargetsInNetlist( const NetlistInfo &netlist )
    {
        //std::map< std::string, std:: vector<ExternalDevicePinConnectInfo> >::const_iterator btIt = connectionPinsByTarget.begin();
        //for(; btIt != connectionPinsByTarget.begin();)

        std:: vector<ExternalDevicePinConnectInfo>::iterator pit = connectionPins.begin();
        for(; pit != connectionPins.end(); ++pit )
        {
            typename NetlistInfo::components_map_type::const_iterator netComponentIt = netlist.components.find(pit->connectTargetDeviceDesignator);
            if (netComponentIt == netlist.components.end())
                return false;

            if (!pit->isConnectTargetPinTakenByName())
                continue; // to next pin

            const ComponentInfo &componentInfo = netComponentIt->second;
            if (!componentInfo.findPinNoByFunction( pit->connectTargetPin, pit->connectTargetPinNo ))
                return false; // no such pin in this device
        }

        return true; // all pins found
    }

}; // struct ExternalDeviceConnectInfo



//-----------------------------------------------------------------------------
/*
(connect device "HSCSSNN001PD2A3" package "SIP4" as "DA"
    (-- "3" to "X12.3")
    ("SDA" to "X12.SDA") (-- нужно заранее прочитать описание устройства )
    (-- "4" to "X12.4")
    ("SCL" to "X12.SCL")

    (-- HSCSSNN001PD2A3 - Honeywell Pressure sensor
        (pin "3" "" ("SDA" ) )
        (pin "4" "" ("SCL" ) )
    )
)
*/
inline
bool parseExternalConnectionsRule( RoboconfOptions &rbcOpts, const ExpressionItem & rule, ExternalDeviceConnectInfo &externalDeviceConnectInfo, bool &bReaded )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    bReaded = false;

    if (rule.isText())
        return true;

    const expression_list_t &lst = rule.itemList;
    expression_list_t::const_iterator it = lst.begin();

    std:: vector< std::string > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    std::string expected;
    std::string found;
    bool readFieldsRes = readListByTemplate( "Ti!:connect;Ti!:device;V", it, lst.end()
                                           , readedVals, expected, found
                                           );
    if (!readFieldsRes)
    {
        LOG_ERR_OBJ(rule)<<"'connect' processing rule: expected "<<expected<<", but found "<<found<<"\n";
        return false;
    }

    if (readedVals[0]!="connect")
        return true;

    if (readedVals[1]!="device")
        return true;

    externalDeviceConnectInfo.fileNo = rule.fileNo;
    externalDeviceConnectInfo.lineNo = rule.lineNo;
    externalDeviceConnectInfo.deviceName = readedVals[2];
    readedVals.clear();

    while( it!=lst.end() && !it->isList()
           && (  externalDeviceConnectInfo.devicePackage.empty()
              || externalDeviceConnectInfo.designatorType.empty()
              )
         )
    {
        readFieldsRes = readListByTemplate( "Ti:package,as;V", it, lst.end()
                                           , readedVals, expected, found
                                           );
        if (!readFieldsRes)
        {
            LOG_ERR_OBJ(rule)<<"'connect' processing rule: expected "<<expected<<", but found "<<found<<"\n";
            return false;
        }

        if (readedVals[0]=="as")
            externalDeviceConnectInfo.designatorType = readedVals[1];
        else
            externalDeviceConnectInfo.devicePackage  = readedVals[1];
    }

    if (externalDeviceConnectInfo.designatorType.empty())
        externalDeviceConnectInfo.designatorType = "DA";

    // Read pins
    // ("SCL" to "X12.SCL")
    for( ; it != lst.end(); ++it )
    {
        if (it->isText())
        {
            continue;
        }

        const expression_list_t &pinLst = it->itemList;
        expression_list_t::const_iterator pinit = pinLst.begin();

        readedVals.clear();

        readFieldsRes = readListByTemplate( "V;Ti:to,->;V", pinit, pinLst.end()
                                           , readedVals, expected, found
                                           );
        if (!readFieldsRes)
        {
            LOG_ERR_OBJ(rule)<<"'connect' processing rule pin connection: expected "<<expected<<", but found "<<found<<"\n";
            return false;
        }

        externalDeviceConnectInfo.connectionPins.emplace_back( ExternalDevicePinConnectInfo( readedVals[0] /* devicePinName */ , readedVals[2] /* connectTargetPinDesignator */ ) );

    }

    if (externalDeviceConnectInfo.connectionPins.empty())
    {
        LOG_ERR_OBJ(rule)<<"'connect device \""<<externalDeviceConnectInfo.deviceName<<"\"' processing rule - no pins connected to net\n";
        return false;
    }


    bReaded = true;

    return true;

}

//-----------------------------------------------------------------------------
inline
bool parseExternalConnectionsRules( RoboconfOptions &rbcOpts
                                  , const expression_list_t &processingRules
                                  , std:: vector< ExternalDeviceConnectInfo > &externalDeviceConnections
                                  , const std:: vector< std::string > &localSearchPaths
                                  , const std:: vector< std::string > &libSearchPaths
                                  , const all_nets_map_type &allNets
                                  )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    expression_list_t::const_iterator rulit = processingRules.begin();
    for( ; rulit != processingRules.end(); ++rulit)
    {
        if (rulit->isText())
            continue;

        ExternalDeviceConnectInfo externalDeviceConnectInfo;
        bool bReaded = false;

        if (!parseExternalConnectionsRule( rbcOpts, *rulit, externalDeviceConnectInfo, bReaded ))
            return false;

        if (bReaded)
            externalDeviceConnections.emplace_back(externalDeviceConnectInfo);
    }

    //----------
    // find devices pin numbers

    std:: vector< ExternalDeviceConnectInfo >::iterator dcit = externalDeviceConnections.begin();
    for(; dcit != externalDeviceConnections.end(); ++dcit)
    {
        if (!dcit->isComponentDefinitionRequired())
            continue;

        std:: vector<std::string> lookupNames = generateComponentNames( rbcOpts, dcit->deviceName, dcit->devicePackage,  "user" ) ;
        //std::map<std::string, std:: vector< ComponentInfo > > deviceComponents;
        std:: vector< ComponentInfo > deviceComponents;

        for( auto lookupName : lookupNames )
        {
            std::string fullName = lookupName + std::string(".cmp");
            std::string foundName;
            std::ifstream libStream;
            if ( !includeSearch( libStream, fullName, foundName, localSearchPaths, libSearchPaths ) )
            {
                continue;
            }
       
            //readedNames.insert(libFileName);
            componentReadCmp(rbcOpts, rbcOpts.usedFiles.getFileId(foundName), libStream, deviceComponents );
            libStream.close();
        }

        ComponentInfo componentInfo;
        componentInfo.typeName = dcit->deviceName;
        componentInfo.package  = dcit->devicePackage;

        auto updater = ComponentTypesUpdater(rbcOpts, deviceComponents);
        updater( componentInfo, NetlistInfo() /* not required for this handler */ );

        std::string failedPinFn;
        if (!dcit->updateDevicePinNumbers( componentInfo, failedPinFn ))
        {
            LOG_ERR_OBJ(*dcit)<<"externally connected device '"<<dcit->deviceName<<"' - pin '"<<failedPinFn<<"' not found by it's function\n";
            return false;
        }

        bool devTargetsGood = false;
        //const std:: map<std::string, NetlistInfo> &allNets
        for( const auto& netlistKV : allNets )
        {
            ExternalDeviceConnectInfo tmpExtDevice = *dcit;
            if (tmpExtDevice.findAllTargetsInNetlist( netlistKV.second ) )
            {
                devTargetsGood = true;
                *dcit = tmpExtDevice;
                dcit->netlistName = netlistKV.second.name;
                break;
            }
            //NetlistInfo
        }

        if (!devTargetsGood)
        {
            LOG_ERR_OBJ(*dcit)<<"externally connected device '"<<dcit->deviceName<<"' - some targets not found\n";
            return false;
        }
        // dcit->netlistName

    }

    return true;

}

//-----------------------------------------------------------------------------
inline
bool connectExternalDevices( RoboconfOptions &rbcOpts, const std:: vector< ExternalDeviceConnectInfo > &externalDeviceConnections
                           , all_nets_map_type &netsConnectTo
                           )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif
    
    for ( auto exconn : externalDeviceConnections )
    {
        if (exconn.netlistName.empty())
        {
            LOG_ERR_OBJ(exconn)<<"externally connected device '"<<exconn.deviceName<<"' - empty netlist\n";
            return false;
        }

        auto netIt = netsConnectTo.find(exconn.netlistName);
        if (netIt==netsConnectTo.end())
        {
            LOG_ERR_OBJ(exconn)<<"externally connected device '"<<exconn.deviceName<<"' - netlist '"<<exconn.netlistName<<"' not found\n";
            return false;
        }

        NetlistInfo &foundNetlist = netIt->second;

        std::string newDesignatorCandidate = exconn.designatorType;

        if ( !regexpSimpleMatch( newDesignatorCandidate, ".*(\\d+)" ) )
        {
            newDesignatorCandidate += "1"; // taken designator do not have the number, add it
        }

        ComponentInfo componentInfo;
        componentInfo.designator = generateNewName( foundNetlist.components, newDesignatorCandidate );
        componentInfo.typeName   = exconn.deviceName;
        componentInfo.package    = exconn.devicePackage;
        componentInfo.sheetName   = "connected_device";

        //componentInfo.detectComponentClassFromDesignator();
        componentInfo.detectComponentClass( rbcOpts );

        foundNetlist.components[componentInfo.designator] = componentInfo;

        /* for each device connection
               1) get the target designator, e.g. X5.2
               2) for this designator - find the net, to which the designator connected to
               3) for the found net, add new nodes
        */

        // Нужно уметь находить имя цепи по десигнатору пина

        foundNetlist.makeDesignatorsMap();

        for( auto exconnPin : exconn.connectionPins)
        {
            std::string newDevicePinDesignator = makeFullDesignator( componentInfo.designator               , exconnPin.devicePinNo );
            std::string targetPinDesignator    = makeFullDesignator( exconnPin.connectTargetDeviceDesignator, exconnPin.connectTargetPinNo );

            typename NetlistInfo::designators_map_type::iterator designatorNetsIt = foundNetlist.designators.find(targetPinDesignator);
            if (designatorNetsIt==foundNetlist.designators.end())
                continue;

            typename NetlistInfo::string_set_type::iterator allDesignatorNetsIterator = designatorNetsIt->second.begin();
            for(; allDesignatorNetsIterator != designatorNetsIt->second.end(); ++allDesignatorNetsIterator)
            {
                const std::string &netConnected = *allDesignatorNetsIterator;
                typename NetlistInfo::nets_map_type::iterator netIt = foundNetlist.nets.find(netConnected);
                if (netIt == foundNetlist.nets.end())
                    continue;

                NetInfo &net = netIt->second;

                NetNode netNode;
                netNode.targetDesignator       = componentInfo.designator;           // R36
                netNode.targetPin              = exconnPin.devicePinNo ;  // 1
                netNode.targetDesignatorFull   = newDevicePinDesignator;  // R36.1

                net.nodes[newDevicePinDesignator] = netNode;
            }
            
        }

        foundNetlist.clearDesignatorsMap(); // will be rebuilded

    }

    return true;

}

//-----------------------------------------------------------------------------
//! Обновляет данные в connection только если возвращает true
inline
bool connectionsDetectInterfacesProcessSingleMatchOnConnection( RoboconfOptions         &rbcOpts
                                                              , bool                    &matchResult
                                                              , const std::string       &detectingType
                                                              , const std::string       &targetPurposeType
                                                              , const Connection        &connection
                                                              , ConnectionInterfaceInfo &foundConnectionInterfaceInfo
                                                              , const expression_list_t &matchRules
                                                              , bool operateVerbose
                                                              )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

/*
and (match "token" (any "TXD{0,1}") set ("TX") )
    (match "token" (all "RXD{0,1}") set ("RX") )

*/
    expression_list_t::const_iterator rulit = matchRules /* .itemList */ .begin();

    std:: vector< std::string > readedVals;
    std::string expected;
    std::string found;
    bool readFieldsRes = readListByTemplate( "Ti:match;V;L;Ti:set;S", rulit, matchRules.end()
                                           , readedVals, expected, found
                                           );
    if (!readFieldsRes)
    {
        LOG_ERR_OBJ(matchRules)<< "Invalid 'detect \""<<detectingType<<"\"' processing rule: expected "<<expected<<", but found "<<found<<"\n";
        return false;
    }

    enum TokenType
    {
         tokenType_Unknown,
         tokenType_netToken,
         tokenType_pinFunction,
         tokenType_Purpose
    };

    TokenType tokenType = tokenType_Unknown;

    // readedVals[1] - toLower(matchWhatStr)
    if (readedVals[1]=="token" || readedVals[1]=="nettoken")
    {
        tokenType = tokenType_netToken;
    }
    else if (readedVals[1]=="function" || readedVals[1]=="pinfunction")
    {
        tokenType = tokenType_pinFunction;
    }
    else if (readedVals[1]=="purpose" || readedVals[1]=="unitpurpose")
    {
        tokenType = tokenType_Purpose;
    }
    else
    {
        LOG_ERR_OBJ(matchRules)<< "Invalid 'detect \""<<detectingType<<"\"' processing rule - unknown token type in match expression - '"<<readedVals[1]<<"'\n";
        return false;
    }


    std::string setWhat = readedVals[4];

    //rulit = matchRules /* .itemList */ .begin();
    expression_list_t::const_iterator expressionListIt = matchRules.begin();
    std::advance(expressionListIt, 2);

    const expression_list_t &regexList = expressionListIt->itemList;
    expression_list_t::const_iterator regexIt = regexList.begin();
    if (regexIt==regexList.end())
    {
        LOG_ERR_OBJ(*expressionListIt) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - expression list is empty\n";
        return false;
    }

    std::string allAnyStr;
    if (!readListTextItem( regexList, regexIt, allAnyStr, false /* dont unquote */  ))
    {
        LOG_ERR_OBJ(*expressionListIt) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - expression list is empty\n";
        return false;
    }

    bool bMatchAll = false;
    if (toLower(allAnyStr)=="any" || toLower(allAnyStr)=="all")
    {
        if (toLower(allAnyStr)=="all")
            bMatchAll = true;

        if (regexIt==regexList.end())
        {
            LOG_ERR_OBJ(*expressionListIt) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - expression list is empty\n";
            return false;
        }
    }
    else
    {
        regexIt--;
    }


    std::string setTypeName, setLineName;
    if (!splitToPair(setWhat, setTypeName, setLineName, ':'))
    {
        setLineName = setWhat;
        setTypeName = detectingType;
    }
    else if (setLineName.empty())
    {
        string_set_type tpfSet = connection.getTargetFunctions();
        if (tpfSet.find("IN")!=tpfSet.end())
           setLineName = "OUT";
        else if (tpfSet.find("OUT")!=tpfSet.end())
           setLineName = "IN";
        else
           setLineName = setTypeName;
    }


    matchResult = bMatchAll;

    string_set_type tokens;
    if (tokenType == tokenType_netToken)
    {
        tokens = connection.getTokens();
    }
    else if (tokenType == tokenType_pinFunction)
    {
        tokens = connection.getTargetFunctions();
    }
    else if (tokenType == tokenType_Purpose)
    {
        tokens.insert(targetPurposeType);
    }


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
                    return true; // тут всё правильно
            }
            else
            {
                matchResult = matchResult || matchRes;
                if (matchResult)
                {
                    foundConnectionInterfaceInfo.interfaceType      = setTypeName;
                    foundConnectionInterfaceInfo.interfaceLineType  = setLineName;
                    foundConnectionInterfaceInfo.interfaceDetected  = true;
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

    if (matchResult)
    {
        foundConnectionInterfaceInfo.interfaceType      = setTypeName;
        foundConnectionInterfaceInfo.interfaceLineType  = setLineName;
        foundConnectionInterfaceInfo.interfaceDetected  = true;
    }

    return true;

}

//-----------------------------------------------------------------------------
inline
bool isAllConnectionsDetected( const std:: vector< Connection > &connections )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    size_t processedCount = 0;
    for( auto &conn : connections )
    {
        if (conn.interfaceDetected)
            continue;
        processedCount++;
    }

    return processedCount==0;
}

//-----------------------------------------------------------------------------
    // void updateInterfaceInfo(const ConnectionInterfaceInfo& info)
    // {
    //     interfaceType     = info.interfaceType    ;
    //     interfaceLineType = info.interfaceLineType;
    //     interfaceDetected = info.interfaceDetected;
    // }

// ConnectionInterfaceInfo &foundConnectionInterfaceInfo

inline
bool connectionsDetectInterfacesProcessMatches( RoboconfOptions            &rbcOpts
                                              , bool                       &matchResult
                                              , const std::string          detectingType
                                              , const std::string          targetPurposeType
                                              , std:: vector< Connection > &connectionsMut    // mutable vector
                                              , const expression_list_t    &matchesList
                                              , bool                        operateVerbose
                                              )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    constexpr std::size_t maxNumConnectionsCanBeProcessed = 512;
    ConnectionInterfaceInfo   connectionsInterfaceInfo[maxNumConnectionsCanBeProcessed]; // Ни в одной группе не может быть столько соединений, мало у каких процов вообще есть столько пинов

    auto updateMutConnections = [&]()
    {
        for(std::size_t idx=0; idx!=connectionsMut.size(); ++idx)
        {
            if (!connectionsInterfaceInfo[idx].interfaceDetected)
                continue;
            connectionsMut[idx].updateInterfaceInfo(connectionsInterfaceInfo[idx]);
        }
    };
    // constexpr std::size_t maxNumConnectionsCanBeProcessed = 512;
    // ConnectionInterfaceInfo   сonnectionsInterfaceInfo[maxNumConnectionsCanBeProcessed]; // Ни в одной группе не может быть столько соединений, мало у каких процов вообще есть столько пинов


/*
and (match "token" (any "TXD{0,1}") set ("TX") )
    (match "token" (all "RXD{0,1}") set ("RX") )

*/
    expression_list_t::const_iterator matchIt = matchesList.begin();

    std::string ruleName;
    {
        // UmbaTracyTraceScope(); // не нужно - тут время практически не тратится

        if (connectionsMut.size() >= maxNumConnectionsCanBeProcessed)
        {
            LOG_ERR_OBJ(matchesList) << "Too much namber of connections\n";
            return false;
        }

        if (!readListTextItem( matchesList, matchIt, ruleName, false /* dont unquote */  ))
        {
            LOG_ERR_OBJ(matchesList) << "Invalid 'detect \""<<detectingType<<"\"' processing rule\n";
            return false;
        }
    
        if (toLower(ruleName)!="and" && toLower(ruleName)!="&&" && toLower(ruleName)!="or" && toLower(ruleName)!="||")
        {
            LOG_ERR_OBJ(matchesList) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - invalid operation (nor AND nor OR not found)\n";
            return false;
        }
    
        // if (isAllConnectionsDetected(connections))
        // {
        //     matchResult = true;
        //     return true;
        // }
    }


    bool bMatchAll = false;
    if (toLower(ruleName)=="and" || toLower(ruleName)=="&&")
        bMatchAll = true;

    matchResult = bMatchAll;


    // std:: vector< Connection > connectionsTmp;
    // {
    //     UmbaTracyTraceScope();
    //     // Сделали отдельный скоуп для приравнивания, хотим замерять, сколько тут времени тратится
    //     connectionsTmp = connections; //connections.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    //  
    //     //LOG_MSG("detect-conn")
    //     std::cout << "Number of connections: " << connections.size() << "\n";
    // }

    const std:: vector< Connection > &connections = connectionsMut; // Делаем константную ссылку, чтобы компилятор нам дал по рукам, если захотим модифицировать

    bool allConnectionsAlreadyDetected = true;

    for(std::size_t idx=0; idx!=connections.size(); ++idx)
    {
        allConnectionsAlreadyDetected = allConnectionsAlreadyDetected && connectionsMut[idx].interfaceDetected;
        connectionsInterfaceInfo[idx] = connectionsMut[idx].getInterfaceInfo();
    }

    if (allConnectionsAlreadyDetected)
    {
        matchResult = true;
        return true;
    }


    //ConnectionInterfaceInfo getInterfaceInfo()
     

    for( ; matchIt != matchesList.end(); ++matchIt)
    {
        // UmbaTracyTraceScope(); // не нужно - тут время практически не тратится
        if (matchIt->isText())
        {
            LOG_ERR_OBJ(matchesList) << "Invalid 'detect \""<<detectingType<<"\"' processing rule - match expression list expected, found text item: '"<<matchIt->itemText<<"'\n";
            return false;
        }

        bool ruleMatched = false;

        //std::size_t idx = (std::size_t)-1;
        std::size_t processedCount = 0;
        for(std::size_t idx=0; idx!=connections.size(); ++idx)
        //for( auto &conn : connectionsTmp )
        {
            // UmbaTracyTraceScope(); // не нужно - тут время практически не тратится
            if (connectionsInterfaceInfo[idx].interfaceDetected)
                continue;

            const auto &conn = connections[idx];
            //++idx;

            processedCount++;


            // Тут conn обновляется, но только если матч прошел без ошибок и ruleMatched также равно true
            // Обновляются поля
            // connection.interfaceType      = setTypeName;
            // connection.interfaceLineType  = setLineName;
            // connection.interfaceDetected  = true;

            if (!connectionsDetectInterfacesProcessSingleMatchOnConnection( rbcOpts, ruleMatched, detectingType, targetPurposeType, conn, connectionsInterfaceInfo[idx], matchIt->itemList, operateVerbose ))
            {
                return false;
            }

            // if (ruleMatched && conn.interfaceDetected)
            // {
            //     //std::cout << "  Updated connection[" << idx << "]\n";
            // }

            if (ruleMatched)
                break;

        } // for( auto &conn : connectionsTmp )


        if (bMatchAll)
        {
            matchResult = matchResult && ruleMatched;
            if (!matchResult)
            {
                //std::cout << "  Rule not matched\n";
                return true; // тут всё правильно
            }
        }
        else
        {
            matchResult = matchResult || ruleMatched;
            if (matchResult)
            {
                //connections = connectionsTmp;
                //connections.swap(connectionsTmp);
                //std::cout << "  Rule matched\n";
                updateMutConnections();
                return true;
            }
        }

        if (!processedCount)
            break;

    } // for( ; matchIt != matchesList.end(); ++matchIt)

    if (matchResult)
    {
        //connections = connectionsTmp;
        //connections.swap(connectionsTmp);
        //std::cout << "  Rule matched\n";
        updateMutConnections();
    }
    else
    {
        //std::cout << "  Rule not matched\n";
    }

    return true;
}

//-----------------------------------------------------------------------------
inline
bool connectionsDetectInterfaces( RoboconfOptions &rbcOpts
                                , std:: vector< Connection > &connections
                                , const expression_list_t    &processingRules
                                , bool operateVerbose
                                , std::string targetPurpose = std::string()
                                )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    std::string targetPurposeType;
    std::string targetInterfaces;
    std::string targetValueType;

    if (targetPurpose.empty())
    {
        if (!connections.empty())
        {
            //targetPurpose = connections[0].dstComponentInfo.purpose;
            targetPurpose = connections[0].processedStrings["UNITPURPOSE"];
        }
    }

    splitToPair( targetPurpose, targetPurposeType, targetInterfaces, ":" );

    if (!targetInterfaces.empty())
       splitToPair( targetInterfaces, targetInterfaces, targetValueType, ":" );
    trim(targetInterfaces);

    std:: vector< std::string > targetInterfacesVec; // targetInterfacesVec.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    if (!targetInterfaces.empty())
        splitToVector( targetInterfaces, targetInterfacesVec, '|' );
    
    if (!targetInterfacesVec.empty() && std::find(targetInterfacesVec.begin(), targetInterfacesVec.end(), std::string("GPIO") )==targetInterfacesVec.end() )
        targetInterfacesVec.emplace_back("GPIO");

    if (targetInterfacesVec.empty())
        targetInterfacesVec.emplace_back("*");

    {
        bool bf = true;
        auto &ss = LOG_MSG("detect-ifs")<<"detect interfaces: ";
        for( auto tiv : targetInterfacesVec )
        {
            if (bf)
                bf = false;
            else
               ss<<", ";
            ss<<tiv;
        }
        ss<<"\n";
    }

    std::string defGpioDirection = (toUpper(targetPurposeType )=="SENSOR") ? "IN" : "OUT";

    /*
    Если заданы интерфейсы, то проверяем только их

    */



/*
(detect "UART"
    (and (match "token" (any "TXD{0,1}") set ("TX") )
         (match "token" (all "RXD{0,1}") set ("RX") )
    )
)
*/
    for( auto targetInterface : targetInterfacesVec)
    {
        expression_list_t::const_iterator rulit = processingRules.begin();
        for( ; rulit != processingRules.end(); ++rulit)
        {
            if (rulit->isText())
                continue;
       
            const expression_list_t &lst = rulit->itemList;
            expression_list_t::const_iterator it = lst.begin();
       
            std::string ruleName;
            if (!readListTextItem( lst, it, ruleName, false /* dont unquote */  ))
            {
                LOG_ERR_OBJ(*rulit) << "Invalid processing rule\n";
                return false;
            }
       
            if (toLower(ruleName)!="detect")
                continue;
       
            std::string detectingType;
            if (!readListTextItem( lst, it, detectingType, true ))
            {
                LOG_ERR_OBJ(*rulit) << "Invalid 'detect' processing rule, missing expected interface type\n";
                return false;
            }
       
            if (it == lst.end())
            {
                LOG_ERR_OBJ(*rulit) << "Invalid 'detect' processing rule, missing expected matches list\n";
                return false;
            }
       
            if (it->isText() || it->itemList.size()<2)
            {
                LOG_ERR_OBJ(*rulit) << "Invalid 'detect' processing rule, missing expected matches list\n";
                return false;
            }

            if (detectingType!=targetInterface && targetInterface!="*")
               continue;
       
            LOG_MSG("detect-iface")<<"detect "<<detectingType<<"\n";
       
            //std::find(targetInterfacesVec.begin(), targetInterfacesVec.end(), std::string("GPIO") )==targetInterfacesVec.end()
            bool matchResult = false;
            if (!connectionsDetectInterfacesProcessMatches(rbcOpts, matchResult, detectingType, targetPurposeType, connections, it->itemList, operateVerbose ))
                return false;

            if (isAllConnectionsDetected(connections))
                break;
        } // for( ; rulit != processingRules.end(); ++rulit)

    } // for( auto targetInterface : targetInterfacesVec)

    for( auto & conn : connections )
    {
        if (conn.interfaceDetected)
            continue;

        conn.interfaceDetected = true;
        conn.interfaceType      = "GPIO";

        string_set_type tpfSet = conn.getTargetFunctions();
        if (tpfSet.find("IN")!=tpfSet.end())
           conn.interfaceLineType = "OUT";
        else if (tpfSet.find("OUT")!=tpfSet.end())
           conn.interfaceLineType = "IN";
        else
           conn.interfaceLineType = defGpioDirection;
    }

    return true;

}

//-----------------------------------------------------------------------------
inline
bool connectionsDetectInterfaces(RoboconfOptions &rbcOpts, std:: vector< ConnectionsGroup > &connGroups, const expression_list_t &processingRules, bool operateVerbose )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif
    
    for( auto &connGrp : connGroups )
    {
        if (!connGrp.ungroupped)
        {
            LOG_MSG("detect-grp")<<"---\n";
            LOG_MSG("detect-grp")<<"Process detecting rules for '"<<connGrp.groupTitle<<"' connection group\n";

            bool matchResult = false;

            if (!connectionsDetectInterfaces(rbcOpts, connGrp.connections, processingRules, operateVerbose ))
                return false;
        }
        else
        {
            LOG_MSG("detect-grp")<<"---\n";
            LOG_MSG("detect-grp")<<"Process detecting rules for unclassified items\n";

            std:: vector< Connection >::iterator connEndIt  = connGrp.connections.end();
            std:: vector< Connection >::iterator connIt     = connGrp.connections.begin();
            if (connIt==connEndIt)
                continue;

            std:: vector< Connection >::iterator connNextIt = connIt;
            ++connNextIt;

            for(; connIt!=connEndIt; ++connIt)
            {
                //bool matchResult = false;

                LOG_MSG("detect-conn")<<"---\n";
                LOG_MSG("detect-conn")<<"Process connection: "<< connIt->srcNetName<<"\n";

                std:: vector< Connection > tmp = std:: vector< Connection >(connIt, connNextIt);
                if (!connectionsDetectInterfaces(rbcOpts, tmp, processingRules, operateVerbose ))
                    return false;
                *connIt = tmp[0];

                if (connNextIt!=connEndIt)
                    ++connNextIt;

            }
        }
    }

    return true;
}



