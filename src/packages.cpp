#include "logerr.h"

#include "parse.h"
#include "netlist.h"
#include "component_handlers.h"
#include "connections.h"
#include "packages.h"
#include "roboconf_options.h"

//
#include "tracy_tracing.h"



//-----------------------------------------------------------------------------
bool PackagesDb::generatePackageStdNameAndAlias( std::string &pkgName, std::string &pkgAlias )
{
    // Name - without '-', alias with '-':
    // LQFP48 - std name, LQFP-48 - std alias

    trim(pkgName);

    std::string::size_type i = 0, size = pkgName.size();
    for(; i!=size; ++i)
    {
        if (!isAlphaChar(pkgName[i]) && pkgName[i]!='_')
            break;
    }

    if (i==0 || i==size)
        return false; // Alias not generated

    std::string firstPart = std::string( pkgName, 0, i );
    if (pkgName[i]=='-')
        ++i;

    std::string secondPart = std::string( pkgName, i );

    pkgName  = toUpper(firstPart + secondPart);
    pkgAlias = toUpper(firstPart + std::string("-") + secondPart);

    return true;
}

//-----------------------------------------------------------------------------
bool PackagesDb::isKnownPackage( const std::string &n ) const
{
    package_info_map_type::const_iterator it = packages.find( toUpper(n) );
    if (it==packages.end())
        return false;

    return true;
}

//-----------------------------------------------------------------------------
bool PackagesDb::addPackage( const PackageInfo &pi )
{
    if (isKnownPackage(pi.packageName))
        return false;

    packages[toUpper(pi.packageName)] = pi;

    return true;
}

//-----------------------------------------------------------------------------
bool PackagesDb::addPackage( const std::string &n, unsigned np )
{
    if (isKnownPackage(n))
        return false;

    packages[toUpper(n)] = PackageInfo(toUpper(n),np);

    return true;
}

//-----------------------------------------------------------------------------
bool PackagesDb::addPackageAlias( const std::string &n, const std::string &a )
{
    if (isKnownPackage(a))
        return false; // alias or such package already exists
    if (!isKnownPackage(n))
        return false; // target canonical package is unknown

    packages[toUpper(a)] = PackageInfo(toUpper(a),toUpper(n));

    return true;
}

//-----------------------------------------------------------------------------
bool PackagesDb::getCanonicalPackage( std::string n, PackageInfo &pi ) const
{
    //if (n==)
    package_info_map_type::const_iterator it = packages.find( toUpper(n) );
    while(it!=packages.end())
    {
        if (it->second.aliasFor.empty())
        {
            pi = it->second;
            return true;
        }

        n = it->second.aliasFor;
        it = packages.find( toUpper(n) );
    }

    return false;
}

//-----------------------------------------------------------------------------
bool PackagesDb::getCanonicalPackageName( std::string n, std::string &foundName ) const
{
    PackageInfo pi;
    if (!getCanonicalPackage( n, pi ))
        return false;
    foundName = pi.packageName;
    return true;
}

//-----------------------------------------------------------------------------
std::string PackagesDb::getCanonicalPackageName( std::string n ) const
{
    std::string foundName;
    if (!getCanonicalPackageName( n, foundName ))
        return n;
    return foundName;
}

//-----------------------------------------------------------------------------
bool PackagesDb::applyAssignPackage( RoboconfOptions &rbcOpts, const PackageAssignRule &rule, NetlistInfo &net )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif
    
    if (!isKnownPackage( rule.packageName ))
    {
        LOG_ERR_OBJ(rule)<<"assign package: package '"<<rule.packageName<<"' unknown\n";
        return false;
    }

    typename NetlistInfo::components_map_type::iterator cit = net.components.find(rule.assignDesignator);
    if (cit==net.components.end())
    {
        LOG_ERR_OBJ(rule)<<"assign package: designator '"<<rule.assignDesignator<<"' not found in netlist '"<<net.name<<"'\n";
        return false;
    }

    cit->second.package = rule.packageName;

    return true;
}

//-----------------------------------------------------------------------------
bool PackagesDb::applyDesignatorAssignments(RoboconfOptions &rbcOpts, all_nets_map_type &nets )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    bool totalRes = true;
    std:: vector< PackageAssignRule >::const_iterator rit = assignRules.begin();
    for(; rit != assignRules.end(); ++rit)
    {
        typename all_nets_map_type::iterator nit = nets.begin();
        for(; nit != nets.end(); ++nit )
        {
            bool res = true;
            bool processsed = false;
            if (rit->assignNetlist.empty())
            {
                res = applyAssignPackage( rbcOpts, *rit, nit->second );
                processsed = true;
            }
            else
            {
                bool isEq = false;
                if (rit->netlistExactCompare)
                    isEq = (nit->first == rit->assignNetlist);
                else
                    isEq = regexpSimpleMatch( nit->first, rit->assignNetlist );

                if (isEq)
                {
                    res = applyAssignPackage( rbcOpts, *rit, nit->second );
                    processsed = true;
                }
            }

            if (processsed && !res)
                totalRes = false;
        }
    }

    assignRules.clear();

    return totalRes;
}

//-----------------------------------------------------------------------------
// (package "SOIC-23" numPins 5 ("SOIC23-5") )
// (packageAlias "SOIC23" ("SOIC23-5" "SOIC23-5B") )
bool PackagesDb::extractPackages(RoboconfOptions &rbcOpts, expression_list_t & rulesList )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    return processExpressionList( rulesList, true /* erase */
                                , [this, &rbcOpts]( const ExpressionItem & rule )
                                  {
                                      return this->extractPackageFromRule( rbcOpts, rule );
                                  }
                                );
}

//-----------------------------------------------------------------------------
bool PackagesDb::extractDesignatorAssignments(RoboconfOptions &rbcOpts, expression_list_t & rulesList )
{
    #if defined(ROBOCONF_TRACY_TRACE_ALL)
        UmbaTracyTraceScope();
    #endif

    return processExpressionList(rulesList, true /* erase */
        , [this, &rbcOpts](const ExpressionItem & rule)
    {
        return this->extractDesignatorAssignment( rbcOpts, rule);
    }
    );
}

//-----------------------------------------------------------------------------
ExpressionParsingResult PackagesDb::extractDesignatorAssignment( RoboconfOptions &rbcOpts, const ExpressionItem & rule )
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


//(packageAssign (designator "DD1")
//               (netlist "Net1")
//               (compare regex|exact)
//               (package "SOIC-8")
//)

    ExpressionParsingResult
    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:packageassign;M1+:designator,netlist,compare,package", it, lst.end()
                                           , readedVals, expected, found
                                           );
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ(rule, "processing rule 'packageAssign'");
    ROBOCONF_CHECK_PARSE_READ_RESULT_RET_SKIP();

    PackageAssignRule packageAssignRule;

    packageAssignRule.fileNo = lst.fileNo;
    packageAssignRule.lineNo = lst.lineNo;

    bool designatorExists = false;

    string_string_vector_map_type::const_iterator mvIt = readedVals[1].mapOfVectorsValue.begin();    // map of vectors
    for( ; mvIt != readedVals[1].mapOfVectorsValue.end(); ++mvIt)
    {
        if (mvIt->first=="designator")
        {
            //packageAssignRule.assignDesignator = mvIt->second[0];
            designatorExists = true;
        }
        else if (mvIt->first=="netlist")
        {
            packageAssignRule.assignNetlist = mvIt->second[0];
        }
        else if (mvIt->first=="compare")
        {
            //packageAssignRule = mvIt.second[0];
            packageAssignRule.netlistExactCompare = (mvIt->second[0]!="regex");
        }
        else if (mvIt->first=="package")
        {
            packageAssignRule.packageName = mvIt->second[0];
        }
    }

    //if (packageAssignRule.assignDesignator.empty())
    if (!designatorExists)
    {
        LOG_ERR_OBJ(packageAssignRule)<<"processing rule 'packageAssign': expected designator value\n";
        return ExpressionParsingResult::failed;
    }

    if (packageAssignRule.packageName.empty())
    {
        LOG_ERR_OBJ(packageAssignRule)<<"processing rule 'packageAssign': expected package value\n";
        return ExpressionParsingResult::failed;
    }

    mvIt = readedVals[1].mapOfVectorsValue.begin();    // map of vectors
    for( ; mvIt != readedVals[1].mapOfVectorsValue.end(); ++mvIt)
    {
        if (mvIt->first=="designator")
        {
            for( const auto &v : mvIt->second)
            {
                packageAssignRule.assignDesignator = v;
                assignRules.push_back(packageAssignRule);
            }
            //packageAssignRule.assignDesignator = mvIt->second[0];
            //designatorExists = true;
        }
    }
    //assignRules.push_back(packageAssignRule);

    return ExpressionParsingResult::success;
}

//-----------------------------------------------------------------------------
ExpressionParsingResult PackagesDb::extractPackageFromRule( RoboconfOptions &rbcOpts, const ExpressionItem & rule )
{
    if (rule.isText())
        return ExpressionParsingResult::skip; // simple skip unknow text item

    std::string expected;
    std::string found;

    std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    const expression_list_t &lst = rule.itemList;
    expression_list_t::const_iterator it = lst.begin();

    ExpressionParsingResult
    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:package,packagealias", it, lst.end()
                                           , readedVals, expected, found
                                           );
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ(rule, "processing rule 'package'");
    ROBOCONF_CHECK_PARSE_READ_RESULT_RET_SKIP();

    if (readedVals[0].tokenId==0)
        return extractPackageFromList( rbcOpts, lst, it );
    else
        return extractPackageAliasFromList( rbcOpts, lst, it );
}

//-----------------------------------------------------------------------------
// (package "SOIC-23" numPins 5 ("SOIC23-5") )
ExpressionParsingResult PackagesDb::extractPackageFromList( RoboconfOptions &rbcOpts, const expression_list_t &lst, expression_list_t::const_iterator it )
{
    std::string expected;
    std::string found;

    std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    // (package "SOT23-5" numPins 5 (SOT25 SC74A SOT753 SMT5 SOT23-5A SOT23-5B) )
    ExpressionParsingResult
    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "V;Ti:numpins;V;A$", it, lst.end()
                                           , readedVals, expected, found
                                           );
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ(lst, "processing rule 'package'");

    std::string packageName = readedVals[0].singleValue;
    unsigned numPins = (size_t)parseIntegerSimple(readedVals[2].singleValue, 10 );
    if (!numPins)
    {
        LOG_ERR_OBJ(readedVals[2])<<"processing rule 'package': package "<<packageName<<": invalid number of pins ('numPins')\n";
        return ExpressionParsingResult::failed;
    }

    if (isKnownPackage(packageName))
    {
        LOG_ERR_OBJ(readedVals[0])<<"processing rule 'package': package '"<<packageName<<"' already defined\n";
        return ExpressionParsingResult::failed;
    }

    std::string alias;
    if ( generatePackageStdNameAndAlias(packageName, alias) )
    {
        addPackage( packageName, numPins );
        addPackageAlias( packageName, alias );
    }
    else
    {
        addPackage( packageName, numPins );
    }

    for( size_t i = 0; i!=readedVals[3].vectorValue.size(); ++i)
    {
        if (readedVals[3].vectorValue[i].empty())
        {
            LOG_ERR_OBJ(readedVals[3])<<"processing rule 'package': package '"<<packageName<<"': empty alias taken\n";
            return ExpressionParsingResult::failed;
        }

        if (isKnownPackage(readedVals[3].vectorValue[i]))
        {
            LOG_ERR_OBJ(readedVals[3])<<"processing rule 'package': package '"<<packageName<<"': alias '"<< readedVals[3].vectorValue[i] <<"' already defined\n";
            return ExpressionParsingResult::failed;
        }

        if ( generatePackageStdNameAndAlias(readedVals[3].vectorValue[i], alias) )
        {
            addPackageAlias( packageName, readedVals[3].vectorValue[i]);
            addPackageAlias( packageName, alias );
        }
        else
        {
            addPackageAlias( packageName, readedVals[3].vectorValue[i]);
        }
    }

    return ExpressionParsingResult::success;
}

//-----------------------------------------------------------------------------
// (packageAlias "SOT23-3" ("WSWSOIC23-5" "SOIC23-5B-AEFC") )
ExpressionParsingResult PackagesDb::extractPackageAliasFromList( RoboconfOptions &rbcOpts, const expression_list_t &lst, expression_list_t::const_iterator it )
{
    std::string expected;
    std::string found;

    std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

                                           // 0 1+
    ExpressionParsingResult
    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "V;A+$", it, lst.end()
                                           , readedVals, expected, found
                                           );
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ(lst, "processing rule 'packageAlias'");

    std::string packageName = readedVals[0].singleValue;

    if (!isKnownPackage(packageName))
    {
        LOG_ERR_OBJ(readedVals[0])<<"processing rule 'packageAlias': unknown package '"<<packageName<<"'\n";
        return ExpressionParsingResult::failed;
    }

    std::string alias;
    generatePackageStdNameAndAlias(packageName, alias);

    for( size_t i = 0; i!=readedVals[1].vectorValue.size(); ++i)
    {
        if (readedVals[1].vectorValue[i].empty())
        {
            LOG_ERR_OBJ(readedVals[1])<<"processing rule 'package': package "<<packageName<<": empty alias taken\n";
            return ExpressionParsingResult::failed;
        }

        if (isKnownPackage(readedVals[1].vectorValue[i]))
        {
            LOG_ERR_OBJ(readedVals[1])<<"processing rule 'package': package "<<packageName<<": alias '"<<readedVals[1].vectorValue[i]<<"' already defined\n";
            return ExpressionParsingResult::failed;
        }

        if ( generatePackageStdNameAndAlias(readedVals[1].vectorValue[i], alias) )
        {
            addPackageAlias( packageName, readedVals[1].vectorValue[i] );
            addPackageAlias( packageName, alias );
        }
        else
        {
            addPackageAlias( packageName, readedVals[1].vectorValue[i] );
        }
    }

    return ExpressionParsingResult::success;
}

//-----------------------------------------------------------------------------
void PackagesDb::logKnownPackages( RoboconfOptions &rbcOpts ) const
{
    std::map< std::string , std:: vector< std::string > >  aliases;
    for( const auto &mp : packages )
    {
        if (!mp.second.aliasFor.empty())
        {
            aliases[mp.second.aliasFor].push_back(mp.first);
        }
    }

    for( const auto &ap : aliases )
    {
        auto &s = LOG_MSG("pkg-known")<<ap.first;
        //if ( packages[ap.first].numPins>1)
        package_info_map_type::const_iterator pkgIt = packages.find(ap.first);
        if ( pkgIt!=packages.end() && pkgIt->second.numPins>1 )
            s<<" (pins: "<<pkgIt->second.numPins<<")";
        //s<<": ";
        if (!ap.second.empty())
        {
            auto it = ap.second.begin();
            s<<": "<<*it++;
            for(; it!=ap.second.end(); ++it)
                s<<", "<<*it;
        }

        s<<umba::omanip::endl;
    }

    
}
