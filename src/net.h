#pragma once

#include "config.h"

//

#include "component.h"

#include "netlist_types.h"
//
#include "connections_types.h"
//
#include "pinUtils.h"

//
#include "tracy_tracing.h"



struct NetNode
{
    std::string    targetDesignator;
    std::string    targetPin;
    std::string    targetDesignatorFull; // Designator.Pin

    NetNode() : targetDesignator(), targetPin() {}
    NetNode( const std::string &dsg, std::string p, const std::string &dsgFull ) : targetDesignator(dsg), targetPin(p), targetDesignatorFull(dsgFull) {}

}; // struct NetNode


struct NetInfo
{

    #if defined(ROBOCONF_NET_CLASSES_USE_UNORDERED_MAP)
        using nodes_map_type = std::unordered_map< std::string, NetNode >;
    #else
        using nodes_map_type = std:: map< std::string, NetNode >;
    #endif


    std::string                        name;
    nodes_map_type                     nodes;
    bool                               visited;
    std::string                        payload;

    void unvisit()
    {
        visited = false;
    }

    bool valid() const
    {
        return !(name.empty() || nodes.size()<2);
    }

}; // struct NetInfo

/*
template < typename StreamType >
StreamType& operator<<( StreamType &s, const NetInfo &ni )
{
    s<<"Net: "<<ni.name;
    for( auto nd : ni.nodes )
    {
        s << " " << nd.first;
    }
    s << "\n";
    return s;
}
*/



struct NetlistInfo;

void findStartConnectionsDesignators( const NetlistInfo &netlist, const std::string &purpose, std:: vector< std::string > &designators );



struct McuConnectionsInfo
{
    std::string                           mcuDesignator;
    std::vector<Connection>               connectionList;
    std::vector< ConnectionsGroup >       connectionGroups;

    void buildConnectionList( RoboconfOptions &rbcOpts
                            , const ConnectionBuildingOptions &buildingOptions
                            , const NetlistInfo &netlist
                            , const std::string &mcuDesignator_
                            );
    // {
    //     mcuDesignator = mcuDesignator_;
    //     connectionList.clear();
    //     connectionsListBuild( rbcOpts, buildingOptions, netlist, mcuDesignator, connectionList);
    // }

    bool processConnectionModifyRules( RoboconfOptions &rbcOpts
                                     , const expression_list_t &processingRules
                                     );
    // {
    //     bool res = processConnectionModifyRules(rbcOpts, connectionList, processingRules);
    //     if (res)
    //     {
    //         connectionsListRemoveMcuDuplicates( connectionList );
    //     }
    //  
    //     return res;
    // }

    void splitConnectionsToGroups( RoboconfOptions &rbcOpts );
    // {
    //     connectionGroups.clear();
    //     splitConnectionsToGroupsByTarget( rbcOpts, connectionGroups, connectionList );
    // }

    bool detectConnectionsInterfaces( RoboconfOptions &rbcOpts
                                    , const expression_list_t &processingRules
                                    , bool operateVerbose
                                    //, std::string targetPurpose = std::string()
                                    );
    // {
    //     return connectionsDetectInterfaces( rbcOpts, connectionGroups, processingRules, operateVerbose, targetPurpose );
    // }


}; // struct McuConnectionsInfo



struct NetlistInfo
{
    #if defined(ROBOCONF_NET_CLASSES_USE_UNORDERED_MAP)
        using string_set_type           = std::unordered_set<std::string>;
        using components_map_type       = std::unordered_map< std::string, ComponentInfo >;
        using nets_map_type             = std::unordered_map< std::string, NetInfo >;
        using designators_map_type      = std::unordered_map< std::string, string_set_type >;
        using mcu_connections_map_type  = std::unordered_map< std::string, McuConnectionsInfo >;
    #else
        using string_set_type           = std:: set<std::string>;
        using components_map_type       = std:: map< std::string, ComponentInfo >;
        using nets_map_type             = std:: map< std::string, NetInfo >;
        using designators_map_type      = std:: map< std::string, string_set_type >;
        using mcu_connections_map_type  = std:: map< std::string, McuConnectionsInfo >;
    #endif


    std::string                                     name;
    std::string                                     projectName;
    components_map_type                             components;
    nets_map_type                                   nets;
    designators_map_type                            designators; // to nets


    std::vector< std::string >                      mcuDesignators;
    mcu_connections_map_type                        mcuConnectionsInfoMap;

    void findStartConnectionsDesignators()
    {
        mcuDesignators.clear();
        ::findStartConnectionsDesignators( *this, "MCU" /* purpose */ , mcuDesignators );
        std::sort(mcuDesignators.begin(), mcuDesignators.end(), designatorPinNamesLess );
    }

    bool buildMcuConnectionLists( RoboconfOptions &rbcOpts
                                , const ConnectionBuildingOptions &buildingOptions
                                , const expression_list_t &processingRules
                                , bool operateVerbose
                                , std::size_t *pProcessedMcus=0
                                , std::string targetPurpose = std::string()
                                )
    {
        if (pProcessedMcus)
        {
            *pProcessedMcus = 0;
        }
        // std::size_t processedMcus = 0;

        for( auto curMcuDesignator : mcuDesignators )
        {
            auto componentKV =  /* netlistInfo. */ components.find(curMcuDesignator);
            if (componentKV ==  /* netlistInfo. */ components.end())
                continue;

            if (pProcessedMcus)
            {
                std::size_t &processedMcus = *pProcessedMcus;
                ++processedMcus;
            }

            McuConnectionsInfo mcuConnectionsInfo;
            mcuConnectionsInfo.buildConnectionList(rbcOpts, buildingOptions, *this, curMcuDesignator);
            if (!mcuConnectionsInfo.processConnectionModifyRules(rbcOpts, processingRules))
            {
                return false;
            }

            mcuConnectionsInfo.splitConnectionsToGroups( rbcOpts );

            if (!mcuConnectionsInfo.detectConnectionsInterfaces(rbcOpts, processingRules, operateVerbose/*, targetPurpose*/))
            {
                return false;
            }

            mcuConnectionsInfoMap[curMcuDesignator] = mcuConnectionsInfo;
        }

        return true;
    }
    

                // RoboconfOptions &rbcOpts
                       // , const expression_list_t &processingRules
                       // , const ConnectionBuildingOptions &opts

                // std::vector<Connection> connectionList;
                // connectionsListBuild( rbcOpts, opts, netlistInfo, curMcuD, connectionList );
                //  
                // if (!processConnectionModifyRules( rbcOpts, connectionList, processingRules ))
                //     return false;
                //  
                // connectionsListRemoveMcuDuplicates( connectionList );
                //  
                //  
                // std::vector< ConnectionsGroup > connGroups;
                // splitConnectionsToGroupsByTarget( rbcOpts, connGroups, connectionList );
                //  
                //  
                // if (!connectionsDetectInterfaces( rbcOpts, connGroups, processingRules, operateVerbose ))
                //     return false;


    void clearDesignatorsMap()
    {
        designators.clear();
    }

    void makeDesignatorsMap()
    {
        for( const auto &kvNet : nets )
        {
            // kvNet.second - NetInfo
            // NetInfo.nodes map, key is designator
            for( const auto & kvNode : kvNet.second.nodes )
            {
                designators[kvNode.first].insert(kvNet.first);
            }
        }
    }

    void unvisit()
    {
        for( auto & kv : nets )
        {
            kv.second.unvisit();
        }
    }

    bool valid() const
    {
        return !name.empty();
    }

    template < typename THanler >
    void traverseComponents( const THanler &handler, const std::unordered_set<ComponentClass> &classes, bool allowAssemblies = true )
    {
        for( auto & c : components )
        {
            bool allowProcessComponent = true;
            if (c.second.assembly && !allowAssemblies)
            {
                 allowProcessComponent = false;
            }

            if (!allowProcessComponent)
                continue;

            if (classes.empty())
            {
                handler( c.second, *this );
            }
            else if (classes.find(c.second.componentClass)!=classes.end())
            {
                handler( c.second, *this);
            }
        }
    }

    template < typename THanler >
    void traverseComponents( const THanler &handler, const std::unordered_set<ComponentClass> &classes, bool allowAssemblies = true ) const
    {
        for( const auto & c : components )
        {
            bool allowProcessComponent = true;
            if (c.second.assembly && !allowAssemblies)
            {
                 allowProcessComponent = false;
            }

            if (!allowProcessComponent)
                continue;

            if (classes.empty())
            {
                handler( c.second, *this );
            }
            else if (classes.find(c.second.componentClass)!=classes.end())
            {
                handler( c.second, *this);
            }
        }
    }



}; // struct NetlistInfo

/*
template < typename StreamType >
StreamType& operator<<( StreamType &s, const NetlistInfo &nl )
{
    s<<"--- Netlist: "<< nl.name << "\n";

    s<<"--- Components\n";
    for(auto ci : nl.components)
        s<<ci.second;

    s<<"--- Nets\n";
    for(auto nt : nl.nets)
        s<<nt.second;

    return s;
}
*/

template < typename THanler >
void traverseComponents( all_nets_map_type &nets, const THanler &handler, const std::unordered_set<ComponentClass> &classes, bool allowAssemblies = true )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    for( auto & p : nets )
        p.second.traverseComponents( handler, classes, allowAssemblies );
}


struct ComponentTypesCollector
{
    std:: vector< ComponentTypePackage > &types;

    ComponentTypesCollector(std:: vector< ComponentTypePackage > &t ) : types( t ) {}

    void operator()( ComponentInfo &ci, NetlistInfo &netlistInfo ) const
    {
        if (ci.componentClass==ComponentClass::cc_RESISTOR && ci.assembly)
        {
            std::string typeName = "RESISTOR_X";
            typeName.append(std::to_string(ci.assembly));
            types.emplace_back(ComponentTypePackage(typeName));
        }
        else
        {
            types.emplace_back(ComponentTypePackage(ci.typeName, ci.package));
        }
    }
}; // struct ComponentTypesCollector

inline
void collectComponentTypes( all_nets_map_type &nets, std:: vector<ComponentTypePackage> &types, const std::unordered_set<ComponentClass> &classes, bool allowAssemblies = true )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    traverseComponents( nets, ComponentTypesCollector(types), classes, allowAssemblies );
    makeUniqueVector(types);
}

inline
void makeDesignatorsMap( all_nets_map_type &nets )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif
    
    for( auto &netlistKV : nets )
    {
        netlistKV.second.makeDesignatorsMap();
    }
}



