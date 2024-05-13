#pragma once

#include <map>

enum class NetlistSrcType
{
    netlistSrcType_Unknown                 =  0,
    netlistSrcType_ACCEL_ASCII             =  1,
    netlistSrcType_EDIF                    =  2,
    netlistSrcType_ORCAD_PCB_II            =  3,
    netlistSrcType_PROTEL_PCB              =  4,
    netlistSrcType_PROTEL_NETLIST_20       =  5,
    netlistSrcType_SCI_CARDS               =  6,
    netlistSrcType_TANGO                   =  7,
    netlistSrcType_TELESIS                 =  8,
    netlistSrcType_WIRE_LIST               =  9,
    netlistSrcType_KICAD_EXPORT            = 10,
    netlistSrcType_EAGLE_PARTLIST          = 11,

    netlistSrcType_Cache                   = 1999,
};

inline
std::map< NetlistSrcType, std::string >&
getNetlistSrcTypeMapIdToName()
{
    static std::map< NetlistSrcType, std::string > _;
    if (_.empty())
    {
     //_[NetlistSrcType::netlistSrcType_Unknown                 ] = "";
     _[NetlistSrcType::netlistSrcType_ACCEL_ASCII             ] = "ACCEL"                  ; //"ACCEL_ASCII";
     _[NetlistSrcType::netlistSrcType_EDIF                    ] = "EDIF"                   ;
     _[NetlistSrcType::netlistSrcType_ORCAD_PCB_II            ] = "ORCAD_PCB_II"           ;
     _[NetlistSrcType::netlistSrcType_PROTEL_PCB              ] = "PROTEL_PCB"             ;
     _[NetlistSrcType::netlistSrcType_PROTEL_NETLIST_20       ] = "PROTEL_NETLIST_20"      ;
     _[NetlistSrcType::netlistSrcType_SCI_CARDS               ] = "SCI_CARDS"              ;
     _[NetlistSrcType::netlistSrcType_TANGO                   ] = "TANGO"                  ;
     _[NetlistSrcType::netlistSrcType_TELESIS                 ] = "TELESIS"                ;
     _[NetlistSrcType::netlistSrcType_WIRE_LIST               ] = "WIRE_LIST"              ;
     _[NetlistSrcType::netlistSrcType_KICAD_EXPORT            ] = "KICAD_EXPORT"           ;
     _[NetlistSrcType::netlistSrcType_EAGLE_PARTLIST          ] = "EAGLE_PARTLIST"         ;
                                                                                          
     _[NetlistSrcType::netlistSrcType_Cache                   ] = "ROBOCONF"               ;
     //_[] = "";
    }

    return _;
}

inline
std::string getNetlistSrcTypeName(NetlistSrcType t)
{
    auto &m = getNetlistSrcTypeMapIdToName();
    auto it = m.find(t);
    if (it==m.end())
        return "<UNKNOWN>"; //NetlistSrcType::netlistSrcType_Unknown;

    return it->second;
}


inline
std::map< std::string, NetlistSrcType >&
getNetlistSrcTypeMapNameToId()
{
    static std::map< std::string, NetlistSrcType > _;
    if (_.empty())
    {
        _["ACCEL"                  ] = NetlistSrcType::netlistSrcType_ACCEL_ASCII             ;
        _["ACCEL_ASCII"            ] = NetlistSrcType::netlistSrcType_ACCEL_ASCII             ;

        _["EDIF"                   ] = NetlistSrcType::netlistSrcType_EDIF                    ;

        _["ORCAD_PCB_II"           ] = NetlistSrcType::netlistSrcType_ORCAD_PCB_II            ;
        _["ORCAD_PCB_2"            ] = NetlistSrcType::netlistSrcType_ORCAD_PCB_II            ;

        _["PROTEL_PCB"             ] = NetlistSrcType::netlistSrcType_PROTEL_PCB              ;
        _["PROTEL_NETLIST_20"      ] = NetlistSrcType::netlistSrcType_PROTEL_NETLIST_20       ;
        _["SCI_CARDS"              ] = NetlistSrcType::netlistSrcType_SCI_CARDS               ;
        _["TANGO"                  ] = NetlistSrcType::netlistSrcType_TANGO                   ;
        _["TELESIS"                ] = NetlistSrcType::netlistSrcType_TELESIS                 ;

        _["WIRE_LIST"              ] = NetlistSrcType::netlistSrcType_WIRE_LIST               ;
        _["WIRELIST"               ] = NetlistSrcType::netlistSrcType_WIRE_LIST               ;

        _["KICAD_EXPORT"           ] = NetlistSrcType::netlistSrcType_KICAD_EXPORT            ;
        _["EAGLE_PARTLIST"         ] = NetlistSrcType::netlistSrcType_EAGLE_PARTLIST          ;

        _["ROBOCONF"               ] = NetlistSrcType::netlistSrcType_Cache                   ;
        //_[] = ;
    }

    return _;
}

inline
NetlistSrcType getNetlistSrcTypeId( const std::string &name )
{
    auto &m = getNetlistSrcTypeMapNameToId();
    auto it = m.find(toUpper(name));
    if (it==m.end())
        return NetlistSrcType::netlistSrcType_Unknown;

    return it->second;
}







