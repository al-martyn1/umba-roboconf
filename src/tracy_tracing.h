#pragma once


// Под MSVC что-то идёт не так, пока и фик с ним
// Собираем для профайлинга в GCC/MinGW и там вроде нормас
// Не, это просто я облажался

#if defined(TRACY_ENABLE) // && !defined(_MSC_VER)
    // https://github.com/wolfpld/tracy?tab=readme-ov-file
    // https://luxeengine.com/integrating-tracy-profiler-in-cpp/
    #include "Tracy.hpp"

    #define UmbaTracyTraceScope()   ZoneScoped

#else

    #define UmbaTracyTraceScope()

#endif

