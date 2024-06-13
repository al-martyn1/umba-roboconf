#pragma once


#ifndef __cplusplus
    #error "Only for C++"
#endif

#include <string>

/*

Стыбрил в сгенерённом CMake-ом

Генерируются макросы

COMPILER_ID  - имя компилятора/его вендора - Intel/MSVC/GCC/IntelLLVM/SCO/ARMCC/IAR/AppleClang/ARMClang/Clang/PathScale/Embarcadero/Borland/Watcom/OpenWatcom/SunPro/HP/Compaq/zOS/IBMClang/XLClang/XL/NVHPC/PGI/CrayClang/Cray/FujitsuClang/Fujitsu/
COMPILER_VERSION_MAJOR
COMPILER_VERSION_MINOR
COMPILER_VERSION_PATCH
COMPILER_VERSION_INTERNAL_STR


SIMULATE_ID  - определён, если компилятор работает в режиме совместимости с другим компилятором
SIMULATE_VERSION_MAJOR
SIMULATE_VERSION_MINOR
SIMULATE_VERSION_PATCH

COMPILER_VERSION_TWEAK - дата сборки компилятора?


PLATFORM_ID (целевая, видимо)     - Linux/MSYS/Cygwin/MinGW/Darwin/Windows/FreeBSD/NetBSD/OpenBSD/SunOS/QNX/Tru64/...

ARCHITECTURE_ID (целевая, видимо) - IA64/ARM64EC/x64/X86/ARM64/ARMV4I/ARMV5I/ARMV/MIPS


*/


//----------------------------------------------------------------------------
namespace umba {



//----------------------------------------------------------------------------
std::string getCompilerName();
std::string getCompilerVersionString();
std::string getCompilerInternalVersionString();
std::string getCompilerNameVersionString();
std::string getCompilerSimulateName();
std::string getCompilerSimulateVersionString();
std::string getCompilerSimulateNameVersionString();
std::string getAppPlatformName();
std::string getAppArchitectureName();
std::string getAppPlatformArchitecture();


} // namespace umba


