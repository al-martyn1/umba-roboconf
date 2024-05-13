#pragma once

#include <sstream>
#include "component.h"
#include "net.h"


struct ComponentDesignatorsCollector
{
    std::vector< std::string > &designators;

    ComponentDesignatorsCollector(std::vector< std::string > &d ) : designators( d ) {}

    void operator()( ComponentInfo &ci, const NetlistInfo &netlistInfo ) const
    {
        if (ci.componentClass== ComponentClass::cc_RESISTOR && ci.assembly)
        {
            designators.push_back( ci.designator );
        }
        else
        { //-V523
            designators.push_back( ci.designator );
        }
    }
}; // struct ComponentTypesCollector


struct ComponentPurposeDesignatorsCollector
{
    std::vector< std::string > &designators;
    std::string                purpose;

    ComponentPurposeDesignatorsCollector(std::vector< std::string > &d, const std::string &prp ) : designators( d ), purpose(prp) {}

    void operator()( ComponentInfo &ci, const NetlistInfo &netlistInfo ) const
    {
        if (ci.purpose==purpose)
        {
            designators.push_back( ci.designator );
        }
    }
}; // struct ComponentPurposeDesignatorsCollector




struct ComponentTypesUpdater
{
    //const std::map<std::string, std::vector< ComponentInfo > > &components;
    RoboconfOptions &rbcOpts;
    const std::vector< ComponentInfo > &components;

    //ComponentTypesUpdater( const std::map<std::string, std::vector< ComponentInfo > > &c ) : components(c) {}
    ComponentTypesUpdater( RoboconfOptions &r, const std::vector< ComponentInfo > &c) : rbcOpts(r), components(c) {}

    void operator()( ComponentInfo &ci, const NetlistInfo &netlistInfo ) const
    {
        std::string typeName = ci.typeName;
        if (ci.componentClass == ComponentClass::cc_RESISTOR && ci.assembly)
        {
            std::ostringstream oss;
            oss << "RESISTOR_X" << ci.assembly;
            typeName = oss.str();
        }

        for( const auto & candy : components )
        {
            if (candy.checkTypeNameMatch(typeName))
                ci.merge(rbcOpts, candy);
        }

        if (ci.componentClass==ComponentClass::cc_UNKNOWN)
        {
        }

        //UNDONE:???
        // merge checks package
        //bool checkTypeNameMatch( const std::string otherTypeNameMatchTo ) const
/*
        std::vector<std::string> candidates = generateComponentNames(typeName); //  &cmpPackage
        for( const auto& candidate : candidates)
        {
            auto cit = components.find( candidate );
            if (cit==components.end())
                continue;
            for( const auto &rci : cit->second )
               ci.merge(rci);
        }
*/
    }
}; // struct ComponentTypesUpdater





struct ComponentClassUpdater
{
    RoboconfOptions &rbcOpts;

    ComponentClassUpdater( RoboconfOptions &r) : rbcOpts(r) {}

    void operator()( ComponentInfo &ci, NetlistInfo &netlistInfo ) const
    {
        if (ci.componentClass==ComponentClass::cc_UNKNOWN)
        {
            ci.detectComponentClass( rbcOpts );
        }

    }
}; // struct ComponentClassUpdater






struct ComponentInternalNetsPublisher
{
    ComponentInternalNetsPublisher( )  {}

    void operator()( ComponentInfo &ci, NetlistInfo &netlistInfo ) const
    {
        auto internalNets = ci.getInternalNets();

        unsigned netNo = 0;
        for( auto in : internalNets )
        {
            std::ostringstream ossNetName;
            ossNetName<<ci.designator<<"_INTERNAL_NET_"<< netNo++;

            NetInfo netInfo;
            netInfo.name = ossNetName.str();
            netInfo.payload = ci.designator;

            for( auto inn : in )
            {
                NetNode netNode;
                netNode.targetDesignator = ci.designator;
                std::ostringstream ossTdf;
                ossTdf<<ci.designator<<"."<<inn;
                netNode.targetDesignatorFull = ossTdf.str();
                netNode.targetPin = inn;
                netInfo.nodes[netNode.targetDesignatorFull] = netNode;
            }

            netlistInfo.nets[netInfo.name] = netInfo;

        }
    }

}; // struct ComponentInternalNetsPublisher





struct ComponentPinMatchApplier
{
    RoboconfOptions &rbcOpts;
    ComponentPinMatchApplier(RoboconfOptions &r ) : rbcOpts(r) {}

    void operator()( ComponentInfo &ci, NetlistInfo &netlistInfo ) const
    {
        ci.matchPinFunctions(rbcOpts);
    }
}; // struct ComponentPinMatchApplier





struct ComponentPackageExistenceChecker
{
    RoboconfOptions                                 &rbcOpts;
    std::map<std::string, std::set<std::string> >   &componentsNoPackageDesignators;

    //ComponentPackageExistenceChecker( const std::map<std::string, std::vector< ComponentInfo > > &c ) : components(c) {}
    ComponentPackageExistenceChecker( RoboconfOptions &r, std::map<std::string, std::set<std::string> > &dsgs ) : rbcOpts(r), componentsNoPackageDesignators(dsgs) {}

    void operator()( ComponentInfo &ci, NetlistInfo &netlistInfo ) const
    {
        std::string typeName = ci.typeName;
        if (ci.componentClass == ComponentClass::cc_RESISTOR && ci.assembly)
        {
            std::ostringstream oss;
            oss << "RESISTOR_X" << ci.assembly;
            typeName = oss.str();
        }

        if (ci.package.empty())
        {
            componentsNoPackageDesignators[netlistInfo.name].insert( ci.designator );
            LOG_WARN_OPT("no-pkg")<<"component '"<<netlistInfo.name<<"."<<ci.designator<<"' ("<<typeName<<") - no package type taken\n";
        }
        else if (!rbcOpts.packagesDb.isKnownPackage(ci.package))
        {
            componentsNoPackageDesignators[netlistInfo.name].insert( ci.designator );
            LOG_WARN_OPT("unknown-pkg")<<"component '"<<netlistInfo.name<<"."<<ci.designator<<"' ("<<typeName<<") - unknown package type taken: '"<<ci.package<<"')\n";
        }

    }
}; // struct ComponentPackageExistenceChecker
