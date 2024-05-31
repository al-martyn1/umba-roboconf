#pragma once

//#include "component.h"
#include <map>
#include <string>

#include "file_set.h"
#include "parse.h"
#include "netlist_component_atributes_map.h"


class RoboconfOptions;

enum class ComponentClass
{
    cc_UNKNOWN       =  0,
    cc_DD            =  1,
    cc_DA            =  2,
    cc_AA            =  3,
    cc_TRANSISTOR    =  4,
    cc_RESISTOR      =  5,
    cc_CAPACITOR     =  6,
    cc_INDUCTOR      =  7,
    cc_DIODE         =  8,
    cc_LED           =  9,
    cc_HEADER        = 10,
    cc_QUARTZ        = 11,
    cc_OPTOPAIR      = 12,
    cc_TESTPOINT     = 13,
    cc_MOUNT         = 14,
    cc_FUSE          = 15,
    cc_JUMPER        = 16,

};

//-----------------------------------------------------------------------------
#if defined(ROBOCONF_COMPONENT_CLASS_NAME_MAPS_USE_UNORDERED_MAP)
    using ComponentClassNameAliasMap   = std::unordered_map< std::string , std::string >;
    using ComponentClassNameTokenMap   = std::unordered_map< std::string , ComponentClass >;
    using ComponentClassTokenStringMap = std::unordered_map< ComponentClass, std::string >;
#else
    using ComponentClassNameAliasMap   = std:: map< std::string , std::string >;
    using ComponentClassNameTokenMap   = std:: map< std::string , ComponentClass >;
    using ComponentClassTokenStringMap = std:: map< ComponentClass, std::string >;
#endif

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
const ComponentClassNameAliasMap& getComponentClassNameAliasMap();
const ComponentClassNameTokenMap& getComponentClassNameToIdMap();
const ComponentClassTokenStringMap& getComponentClassIdToNameMap();
const ComponentClassTokenStringMap& getComponentClassToDescriptionMap();
const ComponentClassTokenStringMap& getComponentClassToDisplayStringMap();
std::string getComponentClassCanonicalName( std::string name );
std::string getComponentClassCanonicalName( ComponentClass id );
ComponentClass getComponentClassId( std::string name );
std::string getComponentClassDescription( ComponentClass id );
std::string getComponentClassDisplayString( ComponentClass id );

//-----------------------------------------------------------------------------
inline
int getComponentClassWeight( ComponentClass cc )
{
    // 0 - others
    // 1 - analog
    // 2 - analog digital, DIODE
    // 3 - DD, TRANSISTOR, LED, OPTOPAIR
    switch(cc)
    {
        case ComponentClass::cc_UNKNOWN   : return 0;
        case ComponentClass::cc_DD        : return 3;
        case ComponentClass::cc_DA        : return 2;
        case ComponentClass::cc_AA        : return 1;
        case ComponentClass::cc_TRANSISTOR: return 3;
        case ComponentClass::cc_RESISTOR  : return 0;
        case ComponentClass::cc_CAPACITOR : return 0;
        case ComponentClass::cc_INDUCTOR  : return 0;
        case ComponentClass::cc_DIODE     : return 2;
        case ComponentClass::cc_LED       : return 3;
        case ComponentClass::cc_HEADER    : return 0;
        case ComponentClass::cc_QUARTZ    : return 0;
        case ComponentClass::cc_OPTOPAIR  : return 3;
        case ComponentClass::cc_TESTPOINT : return 0;
        case ComponentClass::cc_MOUNT     : return 0;
        case ComponentClass::cc_FUSE      : return 0;
        case ComponentClass::cc_JUMPER    : return 0;
        default: return 0;
    };
}

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
// (componentClass DD designator match "DD.*")
struct ComponentClassDetectionRule //-V730
{

    ComponentClass      targetId;
    std::string         checkField;
    std::string         checkMask;
    unsigned            assembly = 0;

    FileSet::file_id_t  fileNo = (FileSet::file_id_t)-1;
    size_t              lineNo = 0;

    ComponentClass detectClassId(RoboconfOptions &rbcOpts, unsigned *pAss, const NetlistComponentAttributesMap &fields ) const;
    ExpressionParsingResult extractRuleFromListItem( RoboconfOptions &rbcOpts, const expression_list_t &lst );
    ExpressionParsingResult extractRuleFromListItem( RoboconfOptions &rbcOpts, const ExpressionItem & rule );

};



// std::string                          classDetectionRules;
/*
  (compInst "R59"
    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")


std::string ComponentClass::getComponentClassDisplayString() const
{
    switch(componentClass)
    {
        case cc_DD             : return std::string("IC (DIGITAL)");
        case cc_DA             : return std::string("IC (DIGITAL-ANALOG)");
        case cc_AA             : return std::string("IC (ANALOG)");
        case cc_RESISTOR       : return std::string("RESISTOR");
        case cc_CAPACITOR      : return std::string("CAPACITOR");
        case cc_INDUCTOR       : return std::string("INDUCTOR");
        case cc_TRANSISTOR     :return std::string("TRANSISTOR");
        case cc_DIODE          : return std::string("DIODE");
        case cc_LED            : return std::string("LED");
        case cc_HEADER         : return std::string("HEADER");
        case cc_QUARTZ         : return std::string("QUARTZ");
        case cc_OPTOPAIR       : return std::string("OPTOPAIR");
        case cc_TESTPOINT      : return std::string("TESTPOINT");
        case cc_MOUNT          : return std::string("MOUNT");
        default : return std::string("!!! Unknown");
    };
}

//-----------------------------------------------------------------------------
std::string ComponentClass::getComponentClassString() const
{
    switch(componentClass)
    {
        case cc_DD       : return std::string("DD");
        case cc_DA       : return std::string("DA");
        case cc_AA       : return std::string("AA");
        case cc_RESISTOR : return std::string("RESISTOR");
        case cc_CAPACITOR: return std::string("CAPACITOR");
        case cc_INDUCTOR : return std::string("INDUCTOR");
        case cc_TRANSISTOR     : return std::string("TRANSISTOR");
        case cc_DIODE    : return std::string("DIODE");
        case cc_LED      : return std::string("LED");
        case cc_HEADER   : return std::string("HEADER");
        case cc_QUARTZ   : return std::string("QUARTZ");
        case cc_OPTOPAIR : return std::string("OPTOPAIR");
        case cc_TESTPOINT      : return std::string("TESTPOINT");
        case cc_MOUNT          : return std::string("MOUNT");
        default : return std::string("!!! Unknown");
    };
}

*/