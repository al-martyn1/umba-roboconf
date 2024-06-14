#pragma once

#include <sstream>

//
#include "tracy_tracing.h"


inline
std::string makeFullDesignator( const std::string dsg, std::string pinNo )
{
    return dsg + std::string(".") + pinNo;
}