/*! \file
\brief Совместимое со всеми компиляторами подключение стандартных заголовков
*/

#pragma once
#include "umba/umba.h"

#include <stdint.h>

#include <utility>
#include <algorithm>
#include <cstddef>

#if defined(UMBA_KEIL_ARMCC_COMPILER_USED)
    #include "stl_keil.h"
#else
    #include <type_traits>
#endif

