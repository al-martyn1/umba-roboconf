#pragma once

#if defined(_MSC_VER)

    #define UMBA_ATTR_FORCE_INLINE              __forceinline
    #define UMBA_ATTR_FORCE_FLATTEN             __forceinline
    #define UMBA_ATTR_FORCE_NO_INLINE

#elif defined(__GNUC__)

    // https://stackoverflow.com/questions/8381293/how-do-i-force-gcc-to-inline-a-function
    // https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Function-Attributes.html
    // 
    #define UMBA_ATTR_FORCE_INLINE              __attribute__((always_inline)) inline
    //#define UMBA_ATTR_FORCE_FLATTEN             __attribute__((always_inline)) __attribute__((flatten))
    #define UMBA_ATTR_FORCE_FLATTEN             __attribute__((flatten)) inline
    #define UMBA_ATTR_FORCE_NO_INLINE           __attribute__((noinline))

#else

    #define UMBA_ATTR_FORCE_INLINE              inline
    #define UMBA_ATTR_FORCE_FLATTEN             inline
    #define UMBA_ATTR_FORCE_NO_INLINE

#endif


// https://www.reddit.com/r/cpp/comments/x5b6qk/force_inline_in_c/
// #ifdef FASTFLOAT_VISUAL_STUDIO
// #define fastfloat_really_inline __forceinline
// #else
// #define fastfloat_really_inline inline __attribute__((always_inline))
// #endif


