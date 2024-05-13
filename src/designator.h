#pragma once

#include <sstream>

inline
std::string makeFullDesignator( const std::string dsg, std::string pinNo )
{
    //std::ostringstream oss;
    //oss<<dsg<<"."<<pinNo;
    //return oss.str();
    return dsg + std::string(".") + pinNo;
}