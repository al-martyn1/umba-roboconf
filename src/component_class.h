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

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05")
    (originalName "_езисторная_сборка_х4_0,05")

    (compRef "_езисторная_сборка_х4_0,05_реверс")
    (originalName "_езисторная_сборка_х4_0,05_реверс")

    if (isDesignatorTypeEqualTo("DD"))
       { componentClass = cc_DD;          return true; }
    else if (isDesignatorTypeEqualTo("DA"))
       { componentClass = cc_DA;          return true; }
    else if (isDesignatorTypeEqualTo("A"))
       { componentClass = cc_AA;          return true; }
    else if (isDesignatorTypeEqualTo("VT"))
       { componentClass = cc_TRANSISTOR;  return true; }
    else if (isDesignatorTypeEqualTo("VD"))
       { componentClass = cc_DIODE;       return true; }
    else if (isDesignatorTypeEqualTo("VU"))
       { componentClass = cc_OPTOPAIR;    return true; }
    else if (isDesignatorTypeEqualTo("C"))
       { componentClass = cc_CAPACITOR;   return true; }
    else if (isDesignatorTypeEqualTo("R"))
       { componentClass = cc_RESISTOR;    return true; }
    else if (isDesignatorTypeEqualTo("Q") || isDesignatorTypeEqualTo("Z"))
       { componentClass = cc_QUARTZ;      return true; }
    else if (isDesignatorTypeEqualTo("L"))
       { componentClass = cc_INDUCTOR;    return true; }
    else if (isDesignatorTypeEqualTo("HL"))
       { componentClass = cc_LED;         return true; }


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


                if (startsWith( v[1], "HEADER"))
                {
                    componentInfo.componentClass = componentInfo.cc_HEADER;
                }
                else if (startsWith( v[1], "CAPACITOR"))
                {
                    componentInfo.componentClass = componentInfo.cc_CAPACITOR;
                }
                else if ( startsWith( v[1], "RESISTOR") || startsWith( v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0") )
                {
                    componentInfo.componentClass = componentInfo.cc_RESISTOR;
                    // езитроная сборка_ХЭ2/4/8/16
                    //TODO: переделать - http://www.cyberforum.ru/post4211234.html
                    if (startsWith(v[1], "RESISTOR_X2") || startsWith(v[1], "RESISTOR-X2") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х2") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "2") )
                        componentInfo.assembly = 2;
                    else if (startsWith(v[1], "RESISTOR_X4") || startsWith(v[1], "RESISTOR-X4") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х4") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "4"))
                        componentInfo.assembly = 4;
                    else if (startsWith(v[1], "RESISTOR_X8") || startsWith(v[1], "RESISTOR-X8") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х8") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "8"))
                        componentInfo.assembly = 8;
                    else if (startsWith(v[1], "RESISTOR_X16") || startsWith(v[1], "RESISTOR-X16") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х16") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "16"))
                        componentInfo.assembly = 16;
                }
                else if (startsWith( v[1], "INDUCTOR"))
                {
                    componentInfo.componentClass = componentInfo.cc_INDUCTOR;
                }
                else if (startsWith( v[1], "LED"))
                {
                    componentInfo.componentClass = componentInfo.cc_LED;
                }
                else if (startsWith( v[1], "QUARTZ"))
                {
                    componentInfo.componentClass = componentInfo.cc_QUARTZ;
                }
            }
        }


                if (startsWith( v[1], "TESTPOINT"))
                {
                    componentInfo.componentClass = componentInfo.cc_TESTPOINT;
                }
                else if (startsWith( v[1], "SOCKET"))
                {
                    componentInfo.componentClass = componentInfo.cc_HEADER;
                }
                else if (startsWith( v[1], "MOUNT"))
                {
                    componentInfo.componentClass = componentInfo.cc_MOUNT;
                }
                else if (startsWith( v[1], "HEADER"))
                {
                    componentInfo.componentClass = componentInfo.cc_HEADER;
                }
                else if (startsWith( v[1], "CAPACITOR"))
                {
                    componentInfo.componentClass = componentInfo.cc_CAPACITOR;
                }
                else if ( startsWith( v[1], "RESISTOR")  || startsWith( v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0") )
                {
                    componentInfo.componentClass = componentInfo.cc_RESISTOR;
                    if (startsWith( v[1], "RESISTOR_X2") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х2") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "2"))
                       componentInfo.assembly = 2;
                    else if (startsWith( v[1], "RESISTOR_X4") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х4") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "4"))
                       componentInfo.assembly = 4;
                    else if (startsWith( v[1], "RESISTOR_X8") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х8") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "8"))
                       componentInfo.assembly = 8;
                    else if (startsWith( v[1], "RESISTOR_X16") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_х16") || startsWith(v[1], "_\xE5\xE7\xE8\xF1\xF2\xEE\xF0\xED\xE0\xFF_\xF1\xE1\xEE\xF0\xEA\xE0_\xF5" "16"))
                       componentInfo.assembly = 16;
                }
                else if (startsWith( v[1], "INDUCTOR"))
                {
                    componentInfo.componentClass = componentInfo.cc_INDUCTOR;
                }
                else if (startsWith( v[1], "LED"))
                {
                    componentInfo.componentClass = componentInfo.cc_LED;
                }
                else if (startsWith( v[1], "QUARTZ"))
                {
                    componentInfo.componentClass = componentInfo.cc_QUARTZ;
                }
            }
        }
*/