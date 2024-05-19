#pragma once


#include "config.h"

//
#include "string_set_type.h"
//
#include <string>


#if defined(ROBOCONF_STRING_STRING_MAP_TYPE_USE_UNORDERED_MAP)
    using string_string_set_map_type = std::unordered_map< std::string, string_set_type >;
#else
    using string_string_set_map_type = std:: map< std::string, string_set_type >;
#endif


