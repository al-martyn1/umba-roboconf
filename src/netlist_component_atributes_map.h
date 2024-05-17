#pragma once

#include "config.h"
#include <string>


#if defined(ROBOCONF_NETLIST_COMPONENT_ATTRIBUTES_MAP_USE_UNORDERED_MAP)
    using NetlistComponentAttributesMap = std::unordered_map< std::string, std::string>;
#else
    using NetlistComponentAttributesMap = std::map< std::string, std::string>;
#endif





