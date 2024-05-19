#pragma once

//
#include "config.h"
//
//#include "net.h"


struct NetlistInfo;

#if defined(ROBOCONF_ALL_NETS_USE_UNORDERED_MAP)
    using all_nets_map_type = std::unordered_map<std::string, NetlistInfo>;
#else
    using all_nets_map_type = std:: map<std::string, NetlistInfo>;
#endif

