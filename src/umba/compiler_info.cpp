#include "compiler_info.h"
//
#include "compiler_id_cpp.h"


//----------------------------------------------------------------------------
namespace umba {



//----------------------------------------------------------------------------
static
const char* getNumberStringSignificantPartVerHelper(const char* pStr)
{
    while(*pStr && *pStr=='0') ++pStr;
    return pStr;
}

//----------------------------------------------------------------------------
static
std::string composeMajorMinorPatchVerHelper(const std::string &vMajor, const std::string &vMinor, const std::string &vPatch)
{
    std::string res = vMajor;
    if (res.empty())
        return res;

    if (vMinor.empty())
        return res;

    res += ".";
    res += vMinor;

    if (vPatch.empty())
        return res;

    res += ".";
    res += vPatch;

    return res;

}

//----------------------------------------------------------------------------
static
std::string composeMajorMinorPatchVerHelper(const char* pMajor, const char* pMinor, const char* pPatch)
{
    return composeMajorMinorPatchVerHelper( std::string(getNumberStringSignificantPartVerHelper(pMajor))
                                          , std::string(getNumberStringSignificantPartVerHelper(pMinor))
                                          , std::string(getNumberStringSignificantPartVerHelper(pPatch))
                                          );
}


//----------------------------------------------------------------------------
std::string getCompilerName()
{
    #if defined(COMPILER_ID)

       std::string compilerName = COMPILER_ID;

    #else

       std::string compilerName;

    #endif

    if (compilerName.empty())
    {
        compilerName = "Unknopn C++ compiler";
    }

    return compilerName;
}

//----------------------------------------------------------------------------
std::string getCompilerVersionString()
{
    #if !defined(COMPILER_VERSION_MAJOR)

        return std::string();

    #else

        static const char vMajor[] = { COMPILER_VERSION_MAJOR , '\0' };
        static const char vMinor[] = { 
            #if defined(COMPILER_VERSION_MINOR)
                COMPILER_VERSION_MINOR, '\0'
            #else
                '\0'
            #endif
            };
        static const char vPatch[] = { 
            #if defined(COMPILER_VERSION_PATCH)
                COMPILER_VERSION_PATCH, '\0'
            #else
                '\0'
            #endif
            };

        return composeMajorMinorPatchVerHelper(&vMajor[0], &vMinor[0], &vPatch[0]);

    #endif
}

//----------------------------------------------------------------------------
std::string getCompilerInternalVersionString()
{
    #if defined(COMPILER_VERSION_INTERNAL_STR)

        return std::string(COMPILER_VERSION_INTERNAL_STR);

    #else

        return std::string();

    #endif

}

//----------------------------------------------------------------------------
std::string getCompilerNameVersionString()
{
    std::string compilerNameVerStr = getCompilerName();
    std::string compilerVerStr     = getCompilerVersionString();

    if (!compilerVerStr.empty())
    {
        compilerNameVerStr += " v";
        compilerNameVerStr += compilerVerStr;
    }

    std::string compilerInternalVerStr = getCompilerInternalVersionString();
    if (!compilerInternalVerStr.empty())
    {
        compilerNameVerStr += " (";
        compilerNameVerStr += compilerInternalVerStr;
        compilerNameVerStr += ")";
    }

    return compilerNameVerStr;
}

//----------------------------------------------------------------------------
std::string getCompilerSimulateName()
{
    #if defined(SIMULATE_ID)

       return std::string(SIMULATE_ID);

    #else

       return std::string();

    #endif
}

//----------------------------------------------------------------------------
std::string getCompilerSimulateVersionString()
{
    #if !defined(SIMULATE_VERSION_MAJOR)

        return std::string();

    #else

        static const char vMajor[] = { SIMULATE_VERSION_MAJOR , '\0' };
        static const char vMinor[] = { 
            #if defined(SIMULATE_VERSION_MINOR)
                SIMULATE_VERSION_MINOR, '\0'
            #else
                '\0'
            #endif
            };
        static const char vPatch[] = { 
            #if defined(SIMULATE_VERSION_PATCH)
                SIMULATE_VERSION_PATCH, '\0'
            #else
                '\0'
            #endif
            };

        return composeMajorMinorPatchVerHelper(&vMajor[0], &vMinor[0], &vPatch[0]);

    #endif
}

//----------------------------------------------------------------------------
std::string getCompilerSimulateNameVersionString()
{
    std::string compilerNameVerStr = getCompilerSimulateName();

    if (compilerNameVerStr.empty())
        return compilerNameVerStr;

    std::string compilerVerStr     = getCompilerSimulateVersionString();

    if (!compilerVerStr.empty())
    {
        compilerNameVerStr += " v";
        compilerNameVerStr += compilerVerStr;
    }

    // std::string compilerInternalVerStr = getCompilerInternalVersionString();
    // if (!compilerInternalVerStr.empty())
    // {
    //     compilerNameVerStr += " (";
    //     compilerNameVerStr += compilerInternalVerStr;
    //     compilerNameVerStr += ")";
    // }

    return compilerNameVerStr;
}

//----------------------------------------------------------------------------
std::string getAppPlatformName()
{
    std::string res;

    #if defined(PLATFORM_ID)

        res = std::string(PLATFORM_ID);

    #endif

    if (res.empty())
        res = std::string("Unknown Platform");

    if (res=="MinGW")
    {
        res = "Windows-MinGW";
    }
    else if (res=="MSYS")
    {
        res = "Windows-MSYS";
    }
    else if (res=="Cygwin")
    {
        res = "Windows-Cygwin";
    }
    //else 



// #elif defined(__MINGW32__)
// # define PLATFORM_ID "MinGW"
//  
// #elif defined(__APPLE__)
// # define PLATFORM_ID "Darwin"
//  
// #elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// # define PLATFORM_ID "Windows"


    return res;
}

//----------------------------------------------------------------------------
std::string getAppArchitectureName()
{
    std::string res;

    #if defined(ARCHITECTURE_ID)

        res = std::string(ARCHITECTURE_ID);

    #endif

    if (res.empty())
    {
        #if defined(__x86_64) || defined(_M_AMD64) || defined(_M_X64) // || defined() || defined()

            res = "x64";

        #elif defined(_X86_) || defined(_M_IX86) // || defined() || defined() || defined() || defined()

            res = "x86";

        #elif defined(_M_IA64)

            res = "IA64";

        #elif defined(_M_ARM64)

            res = "ARM64";

        #elif defined(_M_ARM)

            res = "ARM";

        #else 

            res = std::string("Unknown Architecture");

        #endif
    }


    return res;
}

//----------------------------------------------------------------------------
std::string getAppPlatformArchitecture()
{
    return getAppPlatformName() + "/" + getAppArchitectureName();
}

//----------------------------------------------------------------------------



} // namespace umba





