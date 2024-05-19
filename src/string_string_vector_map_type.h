#pragma once

#include "config.h"

//
#include <string>


#if defined(ROBOCONF_STRING_STRING_VECTOR_MAP_TYPE_USE_UNORDERED_MAP)
    using string_string_vector_map_type = std::unordered_map< std::string, std::vector<std::string> >;
#else
    using string_string_vector_map_type = std:: map< std::string, std::vector<std::string> >;
#endif



