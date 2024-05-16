#if defined(_MSC_VER)

    // Задолбало выключать отдельные варнинги, делаем нулевой уровень предупреждений
    #pragma warning( push, 0 )

    #pragma warning(disable:4668) // warning C4668: '__LP64__' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
    #pragma warning(disable:4838) // warning C4838: conversion from 'int' to 'char' requires a narrowing conversion
    #pragma warning(disable:4310) // warning C4310: cast truncates constant value
    #pragma warning(disable:4309) // warning C4309: 'initializing': truncation of constant value
    #pragma warning(disable:4365) // warning C4365: 'initializing': conversion from 'int' to 'size_t', signed/unsigned mismatch
    #pragma warning(disable:4244) // warning C4244: 'initializing': conversion from '__int64' to 'int', possible loss of data
    #pragma warning(disable:4061) // warning C4061: enumerator 'CompactEncDet::NUM_CORPA' in switch of enum 'CompactEncDet::TextCorpusType' is not explicitly handled by a case label
    #pragma warning(disable:4267) // warning C4267: 'initializing': conversion from 'size_t' to 'int', possible loss of data
    #pragma warning(disable:4996) // warning C4996: 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
    #pragma warning(disable:5039) // warning C5039: 'qsort': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception
    #pragma warning(disable:5045) // warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

#elif defined(__GNUC__)

    #pragma GCC diagnostic push

    #pragma GCC diagnostic ignored "-Wnarrowing"
    #pragma GCC diagnostic ignored "-Wduplicated-branches"

#endif

