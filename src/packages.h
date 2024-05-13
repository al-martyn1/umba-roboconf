#pragma once

#include <map>
#include <vector>
#include <string>

#include "parse.h"


struct NetlistInfo;
class RoboconfOptions;

struct PackageInfo
{
    std::string packageName;
    unsigned    numPins;
    std::string aliasFor;

    PackageInfo() : packageName(), numPins(0), aliasFor() {}
    PackageInfo(const PackageInfo &pi) : packageName(pi.packageName), numPins(pi.numPins), aliasFor(pi.aliasFor) {}
    PackageInfo( const std::string &name, unsigned np ) : packageName(name), numPins(np), aliasFor() {}
    PackageInfo( const std::string &name, const std::string &af ) : packageName(name), numPins(0), aliasFor(af) {}
};




// (packageAssign designator "DD1" netlist "NAME" package "NAME")
struct PackageAssignRule //-V730
{
    std::string assignDesignator;
    std::string assignNetlist;
    std::string packageName;
    bool        netlistExactCompare;

    FileSet::file_id_t fileNo = (FileSet::file_id_t)-1;
    size_t             lineNo = 0;

}; // struct PackageAssignRule




class PackagesDb
{
    std::map< std::string, PackageInfo >  packages;
    std::vector< PackageAssignRule >      assignRules;
    //PackageAssignRules                    assignRules

public:


    bool generatePackageStdNameAndAlias( std::string &pkgName, std::string &pkgAlias );
    bool isKnownPackage( const std::string &n ) const;
    bool addPackage( const PackageInfo &pi );
    bool addPackage( const std::string &n, unsigned np );
    bool addPackageAlias( const std::string &n, const std::string &a );
    bool getCanonicalPackage( std::string n, PackageInfo &pi ) const;
    bool getCanonicalPackageName( std::string n, std::string &foundName ) const;
    std::string getCanonicalPackageName( std::string n ) const;

    bool applyAssignPackage( RoboconfOptions &rbcOpts, const PackageAssignRule &rule, NetlistInfo &net );
    bool applyDesignatorAssignments( RoboconfOptions &rbcOpts, std::map<std::string, NetlistInfo> &nets );
    bool extractPackages( RoboconfOptions &rbcOpts, expression_list_t & rulesList );
    bool extractDesignatorAssignments( RoboconfOptions &rbcOpts, expression_list_t & rulesList );
    ExpressionParsingResult extractDesignatorAssignment( RoboconfOptions &rbcOpts, const ExpressionItem & rule );
    ExpressionParsingResult extractPackageFromRule( RoboconfOptions &rbcOpts, const ExpressionItem & rule );
    ExpressionParsingResult extractPackageFromList( RoboconfOptions &rbcOpts, const expression_list_t &lst, expression_list_t::const_iterator it );
    ExpressionParsingResult extractPackageAliasFromList( RoboconfOptions &rbcOpts, const expression_list_t &lst, expression_list_t::const_iterator it );

    void logKnownPackages( RoboconfOptions &rbcOpts ) const;
   


}; // class PackagesDb


