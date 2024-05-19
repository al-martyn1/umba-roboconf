#pragma once

#include "config.h"

//
#include <string>


#if defined(ROBOCONF_STRING_SET_TYPE_USE_UNORDERED_SET)
    using string_set_type   = std::unordered_set<std::string>;
#else
    using string_set_type   = std:: set<std::string>;
#endif


