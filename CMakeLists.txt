cmake_minimum_required(VERSION 3.27.0)
project(roboconf VERSION 0.9.0 LANGUAGES C CXX)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}\bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# Полезные ключики GCC - https://codeforces.com/blog/entry/15547?locale=ru

#add_compile_definitions("_WIN32" "WIN32" "_UNICODE" "UNICODE")
add_compile_definitions("_WIN32" "WIN32" "CONSOLE" "_CONSOLE")

#add_link_options("-municode")
add_link_options("-mconsole")
add_link_options("-Wl,--subsystem,console")
#add_link_options("--subsystem,console")

#set(CMAKE_GENERATOR_PLATFORM "x64")

set(CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(X64 ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

#set(CMAKE_COLOR_DIAGNOSTICS ON)

#-O0 
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    #-fdiagnostics-color=always
	#set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -flto -flto-report  -fno-rtti -Wall -Wextra -Werror -pedantic-errors -Wctor-dtor-privacy -Wextra-semi -Wnon-virtual-dtor -Woverloaded-virtual -Wredundant-decls -Wsign-promo")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  #-fignore-unknown-pragmas
  set(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wno-ctor-dtor-privacy -Wsign-promo")
  #add_compile_options(-fdiagnostics-color=always)
  add_compile_options(-Wa,-mbig-obj)
  # -fno-rtti
  add_compile_options(-Wall -Wno-unknown-pragmas -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Wredundant-decls -Wno-unused-function)
  add_compile_options(-Wno-unknown-pragmas)
  #-Wno-unknown-pragmas
  #-Wnon-virtual-dtor -Wctor-dtor-privacy
  #add_compile_options(-w)
  #add_compile_options(-w -Wa,-mbig-obj)
  #-fdiagnostics-color=always 
	#set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -fno-rtti")
  #-Wall   
  #-flto -flto-report
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()

set(PRJ_ROOT ${PROJECT_SOURCE_DIR})
set(SRC_ROOT ${PRJ_ROOT}/src)
set(LIBS_ROOT ${PRJ_ROOT}/_libs)


include_directories(
  "${LIBS_ROOT}"
  "${SRC_ROOT}/enc_det"
  "${SRC_ROOT}/sfmt/inc"
  "${SRC_ROOT}"
)


set (ROBOCONF_COMMON_SOURCES
    "${SRC_ROOT}/umba/time_service.cpp"
    "${SRC_ROOT}/component.cpp"
    "${SRC_ROOT}/component_class.cpp"
    "${SRC_ROOT}/encoding.cpp"
    "${SRC_ROOT}/enc_det/compact_enc_det/compact_enc_det.cc"
    "${SRC_ROOT}/enc_det/compact_enc_det/compact_enc_det_hint_code.cc"
    "${SRC_ROOT}/enc_det/util/encodings/encodings.cc"
    "${SRC_ROOT}/enc_det/util/languages/languages.cc"
    "${SRC_ROOT}/packages.cpp"
    "${SRC_ROOT}/roboconf_options.cpp"
    )

#set_source_files_properties("${LIBS_ROOT}/encoding/compact_enc_det/compact_enc_det.cc"           PROPERTIES LANGUAGE C )


add_compile_definitions("USE_UMBA_LOUT_COUT")



add_executable(roboconf
    ${ROBOCONF_COMMON_SOURCES}
    "${SRC_ROOT}/roboconf.cpp"
    )
target_link_libraries(roboconf ws2_32 iphlpapi shell32)


#target_compile_definitions(umba-md-pp-view-console PRIVATE "UMBA_MD_PP_VIEW_CONSOLE")
#https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements

#https://stackoverflow.com/questions/74071243/how-do-i-change-the-working-directory-for-debugging-in-vs-code-cmaketools
#You can change the output directory for the executable using the RUNTIME_OUTPUT_DIRECTORY property. When debugging, the executable is run in that directory. 
#set_target_properties(roboconf
#                      PROPERTIES 
#                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#                     )

#https://github.com/microsoft/vscode-cmake-tools/issues/53
