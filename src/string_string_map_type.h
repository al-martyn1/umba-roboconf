#pragma once


#include "config.h"

//
#include <string>


#if defined(ROBOCONF_STRING_STRING_MAP_TYPE_USE_UNORDERED_MAP)
    using string_string_map_type = std::unordered_map< std::string, std::string >;
#else
    using string_string_map_type = std:: map< std::string, std::string >;
#endif


