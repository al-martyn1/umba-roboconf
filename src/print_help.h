#pragma once

#include "umba/umba.h"

// https://stackoverflow.com/questions/11697820/how-to-use-date-and-time-predefined-macros-in-as-two-integers-then-stri
void printOnlyVersion()
{
    std::cout<<roboconfVersion<<"\n";
}

void printNameVersion( const std::string &indent = "" )
{
    std::cout<<indent<<"Robot Configuration Extraction Tool version ";
    printOnlyVersion();

    #if defined(UMBA_MSVC_COMPILER_USED)
        std::cout << "Built with MSVC\n";
    #elif defined(UMBA_GCC_COMPILER_USED)
        std::cout << "Built with GCC\n";
    #else
        std::cout << "Built with unknown compiler\n";
    #endif

    //<<rdlcVersion<<"\n";
}

void printCommitHash( const std::string &indent = "" )
{
    if (roboconfCommitHash.empty())
        return;
    std::cout<<indent<<"#"<<roboconfCommitHash<<"\n";
}

void printBuildDateTime( const std::string &indent = "" )
{
    std::cout<<indent<<"Built at "<<roboconfBuildDate<<" "<<roboconfBuildTime<<"\n";
}


void printHelp()
{
    printNameVersion();
    printBuildDateTime();
    printCommitHash();

    std::cout<<"\n"
"Usage: roboconf options input_file [output_file]\n\
Options:\n\
\n\
    -h, -?, --help\n\
             - small help\n\
\n\
    --build-info\n\
             - show extended info about version/build\n\
\n\
    -V, --version\n\
             - show version string\n\
\n\
    --where\n\
             - show where executable file is\n\
\n\
    -I=PATH, --include=PATH\n\
             - add semicilon separated path list PATH\n\
               to include lookup directory list\n\
               $INCLUDE$ is a special value which points\n\
               to 'bin/../include' directory\n\
\n\
    --no-builtin-options\n\
             - don't parse predefined options\n\
               from command line options file\n\
               'conf/rdlc.options'\n\
\n\
    --no-builtin-includes \n\
             - parse predefined options file\n\
               but ignore all '--inclide' directives\n\
\n\
    -W=WARN_TYPE, --dont-warn=WARN_TYPE \n\
             - disable warnings of WARN_TYPE:\n\
               reg-info  - don't warn on empty register description\n\
               type-info - don't warn on empty type description\n\
               type-val  - don't warn on empty type value description\n\
               all       - don't warn about any empty description\n\
\n\
    --ns-case=CASE_TYPE\n\
             - set case convertion for namespaces,\n\
               see --id-case option description\n\
\n\
    --class-case=CASE_TYPE\n\
             - set case convertion for classes,\n\
               see --id-case option description\n\
\n\
    --enum-case=CASE_TYPE\n\
             - set case convertion for enums,\n\
               see --id-case option description\n\
\n\
    --file-case=CASE_TYPE\n\
             - set case convertion for file names,\n\
               see --id-case option description\n\
\n\
    -C=CASE_TYPE, --case=CASE_TYPE\n\
    --all-case=CASE_TYPE\n\
             - set case convertion for all types,\n\
               see --id-case option description\n\
";


}

