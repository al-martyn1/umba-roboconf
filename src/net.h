#pragma once

#include "config.h"

//

#include "component.h"

#include "netlist_types.h"


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
    std::string                        name;
    std::map<std::string, NetNode>     nodes;
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

struct NetlistInfo
{
    std::string                                     name;
    std::string                                     projectName;
    std::map< std::string, ComponentInfo >          components;
    std::map< std::string, NetInfo >                nets;
    std::map< std::string, std::set<std::string> >  designators; // to nets

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
    void traverseComponents( const THanler &handler, const std::set<ComponentClass> &classes, bool allowAssemblies = true )
    {
        for( auto & c : components )
        {
            if (classes.empty())
                handler( c.second, *this );
            else if (classes.find(c.second.componentClass)!=classes.end())
                handler( c.second, *this);
            else if (allowAssemblies && c.second.assembly )
                handler( c.second, *this);
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
void traverseComponents( all_nets_map_type &nets, const THanler &handler, const std::set<ComponentClass> &classes, bool allowAssemblies = true )
{
    for( auto & p : nets )
        p.second.traverseComponents( handler, classes, allowAssemblies );
}


struct ComponentTypesCollector
{
    std::vector< ComponentTypePackage > &types;

    ComponentTypesCollector(std::vector< ComponentTypePackage > &t ) : types( t ) {}

    void operator()( ComponentInfo &ci, NetlistInfo &netlistInfo ) const
    {
        if (ci.componentClass==ComponentClass::cc_RESISTOR && ci.assembly)
        {
            std::ostringstream oss;
            oss<<"RESISTOR_X"<<ci.assembly;
            types.push_back(ComponentTypePackage(oss.str()));
        }
        else
        {
            types.push_back(ComponentTypePackage(ci.typeName, ci.package));
        }
    }
}; // struct ComponentTypesCollector

inline
void collectComponentTypes( all_nets_map_type &nets, std::vector<ComponentTypePackage> &types, const std::set<ComponentClass> &classes, bool allowAssemblies = true )
{
    traverseComponents( nets, ComponentTypesCollector(types), classes, allowAssemblies );
    makeUniqueVector(types);
}

inline
void makeDesignatorsMap( all_nets_map_type &nets )
{
    for( auto &netlistKV : nets )
    {
        netlistKV.second.makeDesignatorsMap();
    }
}



