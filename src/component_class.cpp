#include "component_class.h"
#include "cpp.h"
#include "regexp.h"
#include "roboconf_options.h"
#include "logerr.h"
#include "rdlc-core/utils.h"



//-----------------------------------------------------------------------------
const std::map< std::string, std::string >&
getComponentClassNameAliasMap()
{
    static std::map< std::string, std::string > _;
    if (!_.empty())
        return _;

    _["DD"]     = "DIGITAL_DEVICE";
    _["DA"]     = "DIGITAL_ANALOG";
    _["A"]      = "ANALOG";
    _["AA"]     = "ANALOG";
    _["R"]      = "RESISTOR";
    _["C"]      = "CAPACITOR";
    _["I"]      = "INDUCTOR";
    _["T"]      = "TRANSISTOR";
    _["VT"]     = "TRANSISTOR";
    _["VD"]     = "DIODE";
    _["L"]      = "LED";
    _["H"]      = "HEADER";
    _["Q"]      = "QUARTZ";
    _["O"]      = "OPTOPAIR";
    _["TEST"]   = "TESTPOINT";
    _["POINT"]  = "TESTPOINT";
    _["M"]      = "MOUNT";
    _["J"]      = "JUMPER";
    _["FU"]     = "FUSE";
    //_[""] = "";

    return _;
}

//-----------------------------------------------------------------------------
const std::map< std::string, ComponentClass >&
getComponentClassNameToIdMap()
{
    static std::map< std::string, ComponentClass > _;
    if (!_.empty())
        return _;

    _["DIGITAL_DEVICE"] = ComponentClass::cc_DD        ;
    _["DIGITAL_ANALOG"] = ComponentClass::cc_DA        ;
    _["ANALOG"]         = ComponentClass::cc_AA        ;
    _["RESISTOR"]       = ComponentClass::cc_RESISTOR  ;
    _["CAPACITOR"]      = ComponentClass::cc_CAPACITOR ;
    _["INDUCTOR"]       = ComponentClass::cc_INDUCTOR  ;
    _["TRANSISTOR"]     = ComponentClass::cc_TRANSISTOR;
    _["DIODE"]          = ComponentClass::cc_DIODE     ;
    _["LED"]            = ComponentClass::cc_LED       ;
    _["HEADER"]         = ComponentClass::cc_HEADER    ;
    _["QUARTZ"]         = ComponentClass::cc_QUARTZ    ;
    _["OPTOPAIR"]       = ComponentClass::cc_OPTOPAIR  ;
    _["TESTPOINT"]      = ComponentClass::cc_TESTPOINT ;
    _["MOUNT"]          = ComponentClass::cc_MOUNT     ;
    _["FUSE"]           = ComponentClass::cc_FUSE      ;
    _["JUMPER"]         = ComponentClass::cc_JUMPER    ;
    //_[""] = ;

    return _;
}

//-----------------------------------------------------------------------------
const std::map< ComponentClass, std::string >&
getComponentClassIdToNameMap()
{
    static std::map< ComponentClass, std::string > _;
    if (!_.empty())
        return _;

    const std::map< std::string, ComponentClass > &m = getComponentClassNameToIdMap();

    for( const auto &p : m )
    {
        _[p.second] = p.first;
    }

    return _;
}

//-----------------------------------------------------------------------------
const std::map< ComponentClass, std::string >&
getComponentClassToDescriptionMap()
{
    static std::map< ComponentClass, std::string > _;
    if (!_.empty())
        return _;

    _[ComponentClass::cc_UNKNOWN   ] = "!!! Unknown component class !!!";
    _[ComponentClass::cc_DD        ] = "Integrated circuit (digital)";
    _[ComponentClass::cc_DA        ] = "Integrated circuit (digital-analog)";
    _[ComponentClass::cc_AA        ] = "Integrated circuit (analog)";
    _[ComponentClass::cc_RESISTOR  ] = "Resistor";
    _[ComponentClass::cc_CAPACITOR ] = "Capacitor";
    _[ComponentClass::cc_INDUCTOR  ] = "Inductor";
    _[ComponentClass::cc_TRANSISTOR] = "Transistor";
    _[ComponentClass::cc_DIODE     ] = "Diode";
    _[ComponentClass::cc_LED       ] = "Led";
    _[ComponentClass::cc_HEADER    ] = "Header";
    _[ComponentClass::cc_QUARTZ    ] = "Quartz";
    _[ComponentClass::cc_OPTOPAIR  ] = "Optopair";
    _[ComponentClass::cc_TESTPOINT ] = "Testpoint";
    _[ComponentClass::cc_MOUNT     ] = "Mount";
    _[ComponentClass::cc_FUSE      ] = "Fuse";
    _[ComponentClass::cc_JUMPER    ] = "Jumper";
    //_[""] = ;

    return _;
}

//-----------------------------------------------------------------------------
const std::map< ComponentClass, std::string >& getComponentClassToDisplayStringMap()
{
    return getComponentClassToDescriptionMap();
/*
    static std::map< ComponentClass, std::string > _;
    if (!_.empty())
        return _;

    _[ComponentClass::cc_UNKNOWN   ] = 
    _[ComponentClass::cc_DD        ] = 
    _[ComponentClass::cc_DA        ] = 
    _[ComponentClass::cc_AA        ] = 
    _[ComponentClass::cc_RESISTOR  ] = 
    _[ComponentClass::cc_CAPACITOR ] = 
    _[ComponentClass::cc_INDUCTOR  ] = 
    _[ComponentClass::cc_TRANSISTOR] = 
    _[ComponentClass::cc_DIODE     ] = 
    _[ComponentClass::cc_LED       ] = 
    _[ComponentClass::cc_HEADER    ] = 
    _[ComponentClass::cc_QUARTZ    ] = 
    _[ComponentClass::cc_OPTOPAIR  ] = 
    _[ComponentClass::cc_TESTPOINT ] = 
    _[ComponentClass::cc_MOUNT     ] = 
    //_[""] = ;

    return _;
*/
}

//-----------------------------------------------------------------------------
std::string getComponentClassCanonicalName( std::string name )
{
    const std::map< std::string, std::string > &ma = getComponentClassNameAliasMap();

    name = makeCppDefineName( name, false );
    
    while(true)
    {
        std::map< std::string, std::string >::const_iterator it = ma.find(name);
        if (it==ma.end())
            break;
        name = it->second;
    }

    return name;
}

//-----------------------------------------------------------------------------
std::string getComponentClassCanonicalName( ComponentClass id )
{
    const std::map< ComponentClass, std::string > &m = getComponentClassIdToNameMap();

    std::map< ComponentClass, std::string >::const_iterator it = m.find(id);

    if (it==m.end())
        return std::string("UNKNOWN_CLASS");

    return it->second;
}

//-----------------------------------------------------------------------------
ComponentClass getComponentClassId( std::string name )
{
    name = getComponentClassCanonicalName(name);

    const std::map< std::string, ComponentClass > &m = getComponentClassNameToIdMap();

    std::map< std::string, ComponentClass >::const_iterator it = m.find(name);

    if (it==m.end())
        return ComponentClass::cc_UNKNOWN;

    return it->second;
}

//-----------------------------------------------------------------------------
std::string getComponentClassDescription( ComponentClass id )
{
    const std::map< ComponentClass, std::string > &m = getComponentClassToDescriptionMap();

    std::map< ComponentClass, std::string >::const_iterator it = m.find(id);

    if (it==m.end())
        return std::string("??? Unknown component class id ???");

    return it->second;
}

//-----------------------------------------------------------------------------
std::string getComponentClassDisplayString( ComponentClass id )
{
    const std::map< ComponentClass, std::string > &m = getComponentClassToDisplayStringMap();

    std::map< ComponentClass, std::string >::const_iterator it = m.find(id);

    if (it==m.end())
        return std::string("??? Unknown component class id ???");

    return it->second;
}

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
ComponentClass ComponentClassDetectionRule::detectClassId(RoboconfOptions &rbcOpts, unsigned *pAss, const std::map< std::string, std::string > &fields ) const
{
    std::map< std::string, std::string >::const_iterator fit = fields.find(checkField);
    if (fit==fields.end())
        return ComponentClass::cc_UNKNOWN;

    try
    {
        bool checkRes = regexpSimpleMatchUnsafe( fit->second, checkMask );
        if (checkRes)
        {
            //ComponentClass res = getComponentClassId( std::string name )
            LOG_MSG("cls-chk-matched")<<"class: "<<fit->second<<" matched to "<<checkMask<<" ("<<getComponentClassCanonicalName(targetId)<<")\n";
			if (pAss)
				*pAss = assembly;
            return targetId;
        }
        else
        {
            LOG_MSG("cls-chk-not-matched")<<"class: "<<fit->second<<" not matched to "<<checkMask<<"\n";
        }
    }
    catch(const std::regex_error &e)
    {
        LOG_ERR<<"regex error: "<<e.what()<<"\n";
    }
    catch(...)
    {
        LOG_ERR<<"regex unknown error\n";
    }

    return ComponentClass::cc_UNKNOWN;
}

//-----------------------------------------------------------------------------
ExpressionParsingResult ComponentClassDetectionRule::extractRuleFromListItem( RoboconfOptions &rbcOpts, const ExpressionItem & rule )
{
    if (rule.isText())
        return ExpressionParsingResult::skip; // simple skip unknow text item
    return extractRuleFromListItem( rbcOpts, rule.itemList );
}

ExpressionParsingResult ComponentClassDetectionRule::extractRuleFromListItem( RoboconfOptions &rbcOpts, const expression_list_t &lst )
{
    expression_list_t::const_iterator lstIt = lst.begin();
   
    std::string expected;
    std::string found;
   
    std::vector< ExpressionParsingResultItem > readedVals;

    //(componentClass (set DD)        (assembly 4) designator match "DD.*" )
    //(componentClass (set HEADER)    (assembly 4) compRef match "HEADER.*" )
    //(componentClass (set TESTPOINT) (assembly 4) originalName match "TESTPOINT.*" )

                                           // 0                  1 2 3        4
    //bool readFieldsRes = readListByTemplate( "Ti!:componentclass;V;V;Ti:match;V"

                                           // 0                  1               2 3        4
    ExpressionParsingResult
    ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:componentclass;M1$:set,assembly,match,matchTo"
                                           , lstIt, lst.end()
                                           , readedVals, expected, found
                                           );
    ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_OBJ(lst, "processing rule 'componentClass'");
    ROBOCONF_CHECK_PARSE_READ_RESULT_RET_SKIP();

    fileNo = lst.fileNo;
    lineNo = lst.lineNo;
    targetId = ComponentClass::cc_UNKNOWN;

    std::map< std::string, std::vector<std::string> >::const_iterator it = readedVals[1].mapOfVectorsValue.begin();    // map of vectors
    for( ; it != readedVals[1].mapOfVectorsValue.end(); ++it)
    {
        if (it->first=="set")
        {
            if (targetId!=ComponentClass::cc_UNKNOWN)
            {
                LOG_ERR_OBJ(readedVals[1])<<"processing rule 'componentClass': 'set' - component class already taken\n";
                return ExpressionParsingResult::failed;
            }
            targetId = getComponentClassId( it->second[0] );
            if (targetId==ComponentClass::cc_UNKNOWN)
            {
                LOG_ERR_OBJ(readedVals[1])<<"processing rule 'componentClass': 'set' - unknown component class - '"<<it->second[0]<<"'\n";
                return ExpressionParsingResult::failed;
            }
        }
        else if (it->first=="assembly")
        {
            assembly = (unsigned)parseIntegerSimple( it->second[0], 10 );
            if (!assembly || assembly<2)
            {
                LOG_ERR_OBJ(readedVals[1])<<"processing rule 'componentClass': invalid 'assembly' value '"<<it->second[0]<<"'\n";
                return ExpressionParsingResult::failed;
            }
        }
        else if (it->first=="match")
        {
            if (!checkField.empty())
            {
                LOG_ERR_OBJ(lst)<<"processing rule 'componentClass': match value already taken ('match')\n";
                return ExpressionParsingResult::failed;
            }
            checkField = toLower(it->second[0]);
        }
        else if (it->first=="matchTo")
        {
            if (!checkMask.empty())
            {
                LOG_ERR_OBJ(lst)<<"processing rule 'componentClass': match mask already taken ('matchTo')\n";
                return ExpressionParsingResult::failed;
            }
            checkMask = it->second[0];
        }
        else
        {
        }
    }

    if (checkField.empty())
        LOG_ERR_OBJ(lst)<<"processing rule 'componentClass': match value not taken ('match')\n";

    if (checkMask.empty())
        LOG_ERR_OBJ(lst)<<"processing rule 'componentClass': match mask not taken ('matchTo')\n";

    //checkField = toLower(readedVals[2].singleValue);
    //checkMask  = readedVals[4].singleValue;

/*
    targetId = getComponentClassId( readedVals[1].singleValue );
    if (targetId==ComponentClass::cc_UNKNOWN)
    {
        LOG_ERR_OBJ(rule)<<"unknown component class - '"<<readedVals[1].singleValue<<"'\n";
        return ExpressionParsingResult::failed;
    }

*/

    return ExpressionParsingResult::success;
}


