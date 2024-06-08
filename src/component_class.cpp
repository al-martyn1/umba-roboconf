#include "component_class.h"
#include "cpp.h"
#include "regexp.h"
#include "roboconf_options.h"
#include "logerr.h"
#include "rdlc-core/utils.h"

//

#include "string_set_type.h"
#include "string_string_map_type.h"
#include "string_string_vector_map_type.h"

//
#include <type_traits>

//
#include "tracy_tracing.h"


namespace
{

template<class T>
struct remove_cvref
{
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};		

}


//-----------------------------------------------------------------------------
static
ComponentClassNameAliasMap makeComponentClassNameAliasMap()
{
    ComponentClassNameAliasMap m;

    m["DD"]     = "DIGITAL_DEVICE";
    m["DA"]     = "DIGITAL_ANALOG";
    m["A"]      = "ANALOG";
    m["AA"]     = "ANALOG";
    m["R"]      = "RESISTOR";
    m["C"]      = "CAPACITOR";
    m["I"]      = "INDUCTOR";
    m["T"]      = "TRANSISTOR";
    m["VT"]     = "TRANSISTOR";
    m["VD"]     = "DIODE";
    m["L"]      = "LED";
    m["H"]      = "HEADER";
    m["Q"]      = "QUARTZ";
    m["O"]      = "OPTOPAIR";
    m["TEST"]   = "TESTPOINT";
    m["POINT"]  = "TESTPOINT";
    m["M"]      = "MOUNT";
    m["J"]      = "JUMPER";
    m["FU"]     = "FUSE";
    //m[""] = "";

    return m;
}

//-----------------------------------------------------------------------------
const ComponentClassNameAliasMap& getComponentClassNameAliasMap()
{
    static ComponentClassNameAliasMap m = makeComponentClassNameAliasMap();
    return m;
}

//-----------------------------------------------------------------------------
static
ComponentClassNameTokenMap makeComponentClassNameToIdMap()
{
    ComponentClassNameTokenMap m;

    m["DIGITAL_DEVICE"] = ComponentClass::cc_DD        ;
    m["DIGITAL_ANALOG"] = ComponentClass::cc_DA        ;
    m["ANALOG"]         = ComponentClass::cc_AA        ;
    m["RESISTOR"]       = ComponentClass::cc_RESISTOR  ;
    m["CAPACITOR"]      = ComponentClass::cc_CAPACITOR ;
    m["INDUCTOR"]       = ComponentClass::cc_INDUCTOR  ;
    m["TRANSISTOR"]     = ComponentClass::cc_TRANSISTOR;
    m["DIODE"]          = ComponentClass::cc_DIODE     ;
    m["LED"]            = ComponentClass::cc_LED       ;
    m["HEADER"]         = ComponentClass::cc_HEADER    ;
    m["QUARTZ"]         = ComponentClass::cc_QUARTZ    ;
    m["OPTOPAIR"]       = ComponentClass::cc_OPTOPAIR  ;
    m["TESTPOINT"]      = ComponentClass::cc_TESTPOINT ;
    m["MOUNT"]          = ComponentClass::cc_MOUNT     ;
    m["FUSE"]           = ComponentClass::cc_FUSE      ;
    m["JUMPER"]         = ComponentClass::cc_JUMPER    ;
    //m[""] = ;

    return m;
}

//-----------------------------------------------------------------------------
const ComponentClassNameTokenMap& getComponentClassNameToIdMap()
{
    static ComponentClassNameTokenMap m = makeComponentClassNameToIdMap();
    return m;
}

//-----------------------------------------------------------------------------
static
ComponentClassTokenStringMap makeComponentClassIdToNameMap()
{
    ComponentClassTokenStringMap mRes;

    const ComponentClassNameTokenMap &m = getComponentClassNameToIdMap();

    for( const auto &p : m )
    {
        mRes[p.second] = p.first;
    }

    return mRes;
}

//-----------------------------------------------------------------------------
const ComponentClassTokenStringMap& getComponentClassIdToNameMap()
{
    static ComponentClassTokenStringMap m = makeComponentClassIdToNameMap();
    return m;
}

//-----------------------------------------------------------------------------
static
ComponentClassTokenStringMap makeComponentClassToDescriptionMap()
{
    ComponentClassTokenStringMap m;

    m[ComponentClass::cc_UNKNOWN   ] = "!!! Unknown component class !!!";
    m[ComponentClass::cc_DD        ] = "Integrated circuit (digital)";
    m[ComponentClass::cc_DA        ] = "Integrated circuit (digital-analog)";
    m[ComponentClass::cc_AA        ] = "Integrated circuit (analog)";
    m[ComponentClass::cc_RESISTOR  ] = "Resistor";
    m[ComponentClass::cc_CAPACITOR ] = "Capacitor";
    m[ComponentClass::cc_INDUCTOR  ] = "Inductor";
    m[ComponentClass::cc_TRANSISTOR] = "Transistor";
    m[ComponentClass::cc_DIODE     ] = "Diode";
    m[ComponentClass::cc_LED       ] = "Led";
    m[ComponentClass::cc_HEADER    ] = "Header";
    m[ComponentClass::cc_QUARTZ    ] = "Quartz";
    m[ComponentClass::cc_OPTOPAIR  ] = "Optopair";
    m[ComponentClass::cc_TESTPOINT ] = "Testpoint";
    m[ComponentClass::cc_MOUNT     ] = "Mount";
    m[ComponentClass::cc_FUSE      ] = "Fuse";
    m[ComponentClass::cc_JUMPER    ] = "Jumper";
    //m[""] = ;

    return m;
}

//-----------------------------------------------------------------------------
const ComponentClassTokenStringMap& getComponentClassToDescriptionMap()
{
    static ComponentClassTokenStringMap m = makeComponentClassToDescriptionMap();
    return m;
}

//-----------------------------------------------------------------------------
const ComponentClassTokenStringMap& getComponentClassToDisplayStringMap()
{
    return getComponentClassToDescriptionMap();
/*
    static std:: map< ComponentClass, std::string > _;
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
    const auto &ma = getComponentClassNameAliasMap();

    name = makeCppDefineName( name, false );
    
    while(true)
    {
        typename remove_cvref<decltype(ma)>::type::const_iterator it = ma.find(name);
        if (it==ma.end())
            break;
        name = it->second;
    }

    return name;
}

//-----------------------------------------------------------------------------
std::string getComponentClassCanonicalName( ComponentClass id )
{
    const auto &m = getComponentClassIdToNameMap();

    typename remove_cvref<decltype(m)>::type::const_iterator it = m.find(id);

    if (it==m.end())
        return std::string("UNKNOWN_CLASS");

    return it->second;
}

//-----------------------------------------------------------------------------
ComponentClass getComponentClassId( std::string name )
{
    name = getComponentClassCanonicalName(name);

    const auto &m = getComponentClassNameToIdMap();

    typename remove_cvref<decltype(m)>::type::const_iterator it = m.find(name);

    if (it==m.end())
        return ComponentClass::cc_UNKNOWN;

    return it->second;
}

//-----------------------------------------------------------------------------
std::string getComponentClassDescription( ComponentClass id )
{
    const auto &m = getComponentClassToDescriptionMap();

    typename remove_cvref<decltype(m)>::type::const_iterator it = m.find(id);

    if (it==m.end())
        return std::string("??? Unknown component class id ???");

    return it->second;
}

//-----------------------------------------------------------------------------
std::string getComponentClassDisplayString( ComponentClass id )
{
    const auto &m = getComponentClassToDisplayStringMap();

    typename remove_cvref<decltype(m)>::type::const_iterator it = m.find(id);

    if (it==m.end())
        return std::string("??? Unknown component class id ???");

    return it->second;
}

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
ComponentClass ComponentClassDetectionRule::detectClassId(RoboconfOptions &rbcOpts, unsigned *pAss, const NetlistComponentAttributesMap &fields ) const
{
    NetlistComponentAttributesMap::const_iterator fit = fields.find(checkField);
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
   
    std:: vector< ExpressionParsingResultItem > readedVals; readedVals.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

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

    const auto & readedVals1       = readedVals[1];
    const auto & mapOfVectorsValue = readedVals1.mapOfVectorsValue;

    // std:: map< std::string, std:: vector<std::string> >
    string_string_vector_map_type::const_iterator it = mapOfVectorsValue.begin();    // map of vectors
    for( ; it != mapOfVectorsValue.end(); ++it)
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


