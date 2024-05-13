#include "umba/umba.h"
#include "umba/simple_formatter.h"

#include <exception>
#include <stdexcept>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <ios>
#include <iomanip>


//std::map< std::string, std::string > globalVars;

#include "logerr.h"

#include "rdlc-core/trims.h"
#include "rdlc-core/splits.h"
#include "rdlc-core/fsUtils.h"
#include "rdlc-core/isa.h"
#include "rdlc-core/prog.h"
#include "rdlc-core/case.h"
#include "parse.h"
#include "netlist.h"
#include "component_handlers.h"
#include "connections.h"
#include "incsearch.h"
#include "rdlc-core/macros.h"
#include "regexp.h"
#include "interfaces.h"
#include "ioUtils.h"
#include "roboconf_options.h"
#include "roboconf_options_conf.h"
#include "list_dump.h"
#include "platformGetUserName.h"





// https://en.wikipedia.org/wiki/Netlist
// https://en.wikipedia.org/wiki/EDIF
// https://ru.wikipedia.org/wiki/%D0%A1%D0%BF%D0%B8%D1%81%D0%BE%D0%BA_%D1%81%D0%BE%D0%B5%D0%B4%D0%B8%D0%BD%D0%B5%D0%BD%D0%B8%D0%B9



std::string roboconfVersion    = "0.9";
std::string roboconfCommitHash;  //UNDONE
std::string roboconfBuildDate  = __DATE__;
std::string roboconfBuildTime  = __TIME__;

umba::StdStreamCharWriter coutWriter(std::cout);
umba::StdStreamCharWriter cerrWriter(std::cerr);
//umba::NulCharWriter       nulWriter;

umba::SimpleFormatter logMsg(&coutWriter);
umba::SimpleFormatter logErr(&cerrWriter);
//umba::SimpleFormatter logNul(&nulWriter);


//RoboconfOptions    rbcOpts = RoboconfOptions( &logMsg, &logErr );
RoboconfOptions             rbcOpts( &logMsg, &logErr );
RoboconfOptionsConfigurator rbcOptsConfigurator(rbcOpts);

bool                     quet = false;
std::set<std::string>   argsNeedHelp;
bool                    hasHelpOption  = false;


std::string inputFilename;
std::string outputFilename;
std::string localLibCacheFilename;

bool disableBuiltins = false;
bool disableBuiltinIncludes = false;
bool disableBuiltinLibPath = false;
bool disableBuiltinRulesPath = false;

/*
std::vector<std::string> includePaths;
std::vector<std::string> libPaths;
std::vector<std::string> datasheetPaths;
std::vector<std::string> rulesPaths;
*/

std::vector<std::string> rules;
bool noDefaultRules     = false;


bool printInfoFlag      = false;
bool printVersionNoFlag = false;

bool makeNetlistCache   = false;
bool showComponentsList = false;
bool verboseLibSearch   = false;
bool makeLibCache       = false;
bool makeDump           = false;
bool operateVerbose     = false;


std::string reportType = "summary";
std::string regexpTest;
std::string regexpTestTarget;



std::string exeFullName         = "roboconf";
std::string progBinPath         = ".";
std::string progRootPath        = ".";
std::string progConfPath        = ".\\conf" ;
std::string progIncludePath     = ".\\include";
std::string progLibPath         = ".\\lib";
std::string progUserLibPath     = ".\\lib-user";
std::string progDatasheetPath   = ".\\datasheets";
std::string progRulesPath       = ".\\lib";


ComponentImportOptions  componentImportOptions;
//




//#include "read_ini.h"
//#include "prog.h"

#include "reports.h"
#include "report_summary.h"
#include "report_mcu.h"
#include "report_periph.h"
#include "report_generic.h"

#include "print_help.h"
#include "parse_arg.h"


//#include ""

class CommandLineOptionCollectorImpl : public CommandLineOptionCollectorImplBase
{
protected:
    virtual void onOptionDup( const std::string &opt ) override
    {
        LOG_ERR_OPT<<"Duplicated option key - '"<<opt<<"'\n";

        throw std::runtime_error("Duplicated option key");
    }

};

int safe_main(int argc, char* argv[]);

int main(int argc, char* argv[])
   {
       try
       {
           return safe_main(argc, argv);
       }
       catch(const std::exception &e)
       {
           LOG_ERR_OPT<<"fatal error: "<<e.what()<<umba::omanip::endl;
       }
       catch(...)
       {
           LOG_ERR_OPT<<"fatal error: unknown error"<<umba::omanip::endl;
       }

       return 2;
   }

int safe_main(int argc, char* argv[])
   {
    using std::cin;
    using std::cout;
    using std::cerr;

    

    CommandLineOptionCollectorImpl commandLineOptionCollector;

    std::map< std::string, IReportGenerator* > reportGenerators;

    addReportGenerator( reportGenerators, new SummaryReportGenerator() );
    addReportGenerator( reportGenerators, new McuReportGenerator() );
    addReportGenerator( reportGenerators, new PeriphReportGenerator() );

    
    detectProgLocation( exeFullName, progBinPath, progRootPath );
    progConfPath      = appendPath( progRootPath, std::string("/conf") );
    progIncludePath   = appendPath( progRootPath, std::string("/include") );
    progLibPath       = appendPath( progRootPath, std::string("/lib") );
    progUserLibPath   = appendPath( progRootPath, std::string("/lib-user") );
    progDatasheetPath = appendPath( progRootPath, std::string("/datasheets") );
    progRulesPath     = appendPath( progRootPath, std::string("/rules") );

    rbcOpts.globalVars["RoboconfBin"]  = progBinPath;
    rbcOpts.globalVars["RoboconfRoot"] = progRootPath;
    rbcOpts.globalVars["RoboconfConfPath"] = "$(RoboconfRoot)/conf";
    rbcOpts.globalVars["RoboconfLibPath"]  = "$(RoboconfRoot)/lib";
    rbcOpts.globalVars["RoboconfLibUserPath"]  = "$(RoboconfRoot)/lib-user";
    rbcOpts.globalVars["RoboconfDatasheetPath"]  = "$(RoboconfRoot)/datasheets";
    rbcOpts.globalVars["RoboconfRulesPath"]  = "$(RoboconfRoot)/rules";

    rbcOpts.globalVars["LibPath"]  = "$(RoboconfRoot)/lib";
    rbcOpts.globalVars["RulesPath"]  = "$(RoboconfRoot)/rules";
    rbcOpts.globalVars["DatasheetPath"]  = "$(RoboconfRoot)/datasheets";


/*
    std::string exeFullName = getProgExeName();
    if (!exeFullName.empty())
    {
        progBinPath = getPath( exeFullName );
        bool isExeFolderBin = false;

        if (!progBinPath.empty())
        {
            std::string exeFolder = toLower(getFileName( progBinPath ));
            if (exeFolder=="bin" || exeFolder=="debug" || exeFolder=="release")
                isExeFolderBin = true;
        }

        if (isExeFolderBin)
        {
            progRootPath    = getPath( progBinPath );
        }
        else
        {
            progRootPath    = progBinPath;
        }

        progConfPath      = progRootPath + std::string("/conf");
        progIncludePath   = progRootPath + std::string("/include");
        progLibPath       = progRootPath + std::string("/lib");
        progUserLibPath   = progRootPath + std::string("/lib-user");
        progDatasheetPath = progRootPath + std::string("/datasheets");
        progRulesPath     = progRootPath + std::string("/rules");


        rbcOpts.globalVars["RoboconfBin"]  = progBinPath;
        rbcOpts.globalVars["RoboconfRoot"] = progRootPath;
        rbcOpts.globalVars["RoboconfConfPath"] = "$(RoboconfRoot)/conf";
        rbcOpts.globalVars["RoboconfLibPath"]  = "$(RoboconfRoot)/lib";
        rbcOpts.globalVars["RoboconfLibUserPath"]  = "$(RoboconfRoot)/lib-user";
        rbcOpts.globalVars["RoboconfDatasheetPath"]  = "$(RoboconfRoot)/datasheets";
        rbcOpts.globalVars["RoboconfRulesPath"]  = "$(RoboconfRoot)/rules";

        rbcOpts.globalVars["LibPath"]  = "$(RoboconfRoot)/lib";
        rbcOpts.globalVars["RulesPath"]  = "$(RoboconfRoot)/rules";
        rbcOpts.globalVars["DatasheetPath"]  = "$(RoboconfRoot)/datasheets";

    }
*/    

    std::vector<std::string> args = prepareArgs( argc, &argv[0] );
    hasHelpOption = argsFilterForHelpOption( args, argsNeedHelp /* , { "wiki", "bash", "clink" } */ );

    for( const auto & a : args)
    {
        CommandLineOption opt(a);
     
        if (!opt.isOption())
            continue;

        if (opt.isOption("no-builtin-options"))
            disableBuiltins = true;
        else if (opt.isOption("no-builtin-lib-paths"))
            disableBuiltinLibPath = true;
        else if (opt.isOption("no-builtin-rules-paths"))
            disableBuiltinRulesPath = true;
        else if (opt.isSomeKindOfQuet() && !hasHelpOption)
            quet = true;
        else if (opt.isWhereOption())
        {
            LOG_MSG_OPT<<exeFullName<<"\n";
            return 0;
        }
    }
/*
    for( int argNo = 1; argNo<argc; ++argNo)
    {
        std::string a = argv[argNo];
        CommandLineOption opt(a);
        if (a.empty())
            continue;
     
        if (!opt.isOption())
            continue;

        if (opt.isOption("no-builtin-options"))
            disableBuiltins = true;
        //else if (opt.isOption("no-builtin-includes"))
        //    disableBuiltinIncludes = true;
        else if (opt.isOption("no-builtin-lib-paths"))
            disableBuiltinLibPath = true;
        else if (opt.isOption("no-builtin-rules-paths"))
            disableBuiltinRulesPath = true;
        //else if (opt.isOption("quet") || opt.isOption('q'))
        //    quet = true;
        //else if (opt.isOption("verbose") || opt.isOption('V'))
        //    operateVerbose = true;
        //else if (opt.isOption("build-info"))
        //{
        //    printNameVersion();
        //    printBuildDateTime();
        //    printCommitHash();
        //    return 1;
        //}
        //else if (opt.isOption("version") || opt.isOption('v'))
        //{
        //    printOnlyVersion();
        //    return 0;
        //}
        //else if (opt.isOption("help") || opt.isOption('h') || opt.isOption('?'))
        //{
        //    printHelp();
        //    return 0;
        //}
        else if (opt.isOption("where"))
        {
            std::cout<<exeFullName<<"\n";
            return 0;
        }
    }
*/

    if (!quet)
    {
        printNameVersion();
        //std::cout<<"\n";
    }
    else
    {
        rbcOpts.resetEnabledMessages();
    }

    if (!disableBuiltins)
    {
        {
            std::string rbcOptsFileName = appendPath(progConfPath, std::string("/roboconf.options") );
            std::vector<std::string> opts;
            readOptionsFile(rbcOptsFileName, opts );
            for( const auto & o : opts)
            {
                int paRes = parseArg( o, &commandLineOptionCollector, false, false );
                if (paRes)
                   return paRes<0 ? 1 : 0;
            }

            /*
            std::ifstream optFile(rbcOptsFileName.c_str());
            if (!!optFile)
            {
                std::string optLine;
                while( std::getline( optFile, optLine) )
                {
                    trim(optLine);
                    if (optLine.empty())
                        continue;
           
                    if (isComment( optLine ))
                        continue;
           
                    int paRes = parseArg( optLine, 0, false, true );
                    if (paRes)
                    {
                       LOG_ERR_OPT<<"error in options file '"<< rbcOptsFileName<<"'\n";
                       return paRes<0 ? 1 : 0;
                    }
                }
            
            }
            */
        }
        

        {
            std::string rbcOptsFileName = appendPath( progConfPath, std::string("/roboconf.options") );
            std::vector<std::string> opts;
            readOptionsFile(rbcOptsFileName, opts );
            for( const auto & o : opts)
            {
                int paRes = parseArg( o, &commandLineOptionCollector, false, false );
                if (paRes)
                   return paRes<0 ? 1 : 0;
            }
            /*
            std::string rbcOptsFileName = progConfPath + std::string("/roboconf.options.user");
            std::ifstream optFile(rbcOptsFileName.c_str());
            if (!!optFile)
            {
                std::string optLine;
                while( std::getline( optFile, optLine) )
                {
                    trim(optLine);
                    if (optLine.empty())
                        continue;
           
                    if (isComment( optLine ))
                        continue;
           
                    int paRes = parseArg( optLine, 0, false, true );
                    if (paRes)
                    {
                       LOG_ERR_OPT<<"error in options file '"<< rbcOptsFileName<<"'\n";
                       return paRes<0 ? 1 : 0;
                    }
                }
            
            }
            */
        }

        std::string userName     = platformGetUserName();
        if (userName.empty())
            userName = "user";
        std::string computerName = platformGetComputerName();
        if (computerName.empty())
            computerName = "PC";
        if (!userName.empty() || !computerName.empty())
        {
            std::string computerUser = computerName;
            if (!userName.empty())
            {
                if (!computerUser.empty())
                    computerUser += "_";
                computerUser += userName;
            }

            std::string rbcOptsFileName = appendPath( progConfPath, std::string("/roboconf.options.") + platformUserNameConvertToFilename(computerUser) );
            std::vector<std::string> opts;

            //std::string rbcOptsFileName = progConfPath + std::string("/roboconf.options.") + platformUserNameConvertToFilename(computerUser);
            //std::ifstream optFile(rbcOptsFileName.c_str());
            //if (!!optFile)
            if (readOptionsFile(rbcOptsFileName, opts ))
            {
                for( const auto & o : opts)
                {
                    int paRes = parseArg( o, &commandLineOptionCollector, false, false );
                    if (paRes)
                       return paRes<0 ? 1 : 0;
                }
                /*
                std::string optLine;
                while( std::getline( optFile, optLine) )
                {
                    trim(optLine);
                    if (optLine.empty())
                        continue;
           
                    if (isComment( optLine ))
                        continue;
           
                    int paRes = parseArg( optLine, 0, false, true );
                    if (paRes)
                    {
                       LOG_ERR_OPT<<"error in options file '"<< rbcOptsFileName<<"'\n";
                       return paRes<0 ? 1 : 0;
                    }
                }
                */
            }
            else
            {
                std::ofstream optFile(rbcOptsFileName.c_str());
                if (!!optFile)
                {
                    optFile<<"; Options file for "<<userName<<" at "<<computerName<<"\n";
                }
            }
        }
    
    
    
    }

    for( const auto & a : args)
    {
        int paRes = parseArg( a, &commandLineOptionCollector, false, false );
        if (paRes)
           return paRes<0 ? 1 : 0;
    }
/*
    for( int argNo = 1; argNo<argc; ++argNo)
    {
        int paRes = parseArg( argv[argNo], &commandLineOptionCollector, false, false );
        if (paRes)
           return paRes<0 ? 1 : 0;
    }
*/
    if (!noDefaultRules)
    {
        rules.insert( rules.begin(), std::string("default.rul") );
        rules.insert( rules.begin(), std::string("default_power.rul") );
        rules.insert( rules.begin(), std::string("default_packages.rul") );
    }

    if (inputFilename.empty())
    {
        LOG_ERR_OPT<<"no input file taken\n";
        return 1;
    }


    if (!regexpTest.empty())
    {
        cout<<"Regex test\n"
            <<"Expression: '"<<regexpTest<<"'\n"
            <<"Template  : '"<<regexpTestTarget<<"'\n"
            <<"Text      : '"<<inputFilename<<"'\n"
            <<"Result    : ";

        try
        {
            std::string res;
            if (!regexpEvalString( res, regexpTest, regexpTestTarget, inputFilename, rbcOpts.globalVars ) )
                cout<<"Not matched";
            else
            {
                if (regexpTestTarget.empty())
                    res = "Matched";
                cout<<"'"<<res<<"'";
            }
            cout<<"\n";
        }
        catch(const  std::exception &e )
        {
            cout<<"Error: "<<e.what()<<"\n";
        }
        catch(...)
        {
            cout<<"Error: unknown\n";
        }
        return 0;
    }


    if (outputFilename.empty())
    {
        //cerr<<"No output file name taken\n";
        //return 1;
        //outputFilename = getPathName( inputFilename );
        //defaultOutputUsed = true;
    }

    std::ifstream input( inputFilename );
    if (!input)
    {
        LOG_ERR_OPT<<"failed to open input file '"<<inputFilename<<"'\n";
        return 2;
    }


    std::ofstream outputStream;
    //if (outputFinalName.emp)
    std::ostream &os = outputFilename.empty() ? std::cout : outputStream;

    if (!componentImportOptions.namesList.empty())
    {
        ComponentInfo ci;
        if (!componentImportFromCsv(rbcOpts, input, componentImportOptions, ci, ';' /* sep */ ))
        {
            LOG_ERR_OPT<<"failed to import component from CSV\n";
            return 2;
        }

        input.close();

        std::string outputName = getFileName(inputFilename);
        // componentImportOptions.namesList[0]

        if (!outputFilename.empty())
        {
            std::string outputFinalName = generateOutputFilename( inputFilename, outputFilename, outputName, "cmp" );
            outputStream.open(outputFinalName);

            if (!outputStream)
            {
                LOG_ERR_OPT<<"failed to open output file '"<<outputFinalName<<"'\n";
                return 2;
            }
        }

        componentExportToCmp( os, ci );

        if (!quet)
        {
            cout<<"Export component from CSV: Done ('"<<outputName<<"')\n";
        }

        return 0;
    }


    // Test reading for conditionals
    //rbcOpts.readComponentClassDetectionRules( inputFilename, rbcOpts.rulesPaths, "ACCEL" /* netlistType */ );
    ////rbcOpts.readComponentClassDetectionRules( inputFilename, rbcOpts.rulesPaths, "EDIF" /* netlistType */ );
    //rbcOpts.componentsClassDetectionRules.clear();
    //--- Clearing test


    std::string projectName;
    std::map<std::string, NetlistInfo> allNets;
    if (!netlistRead(rbcOpts, rbcOpts.usedFiles.getFileId(inputFilename), input, projectName, allNets ))
    {
        return 2;
    }


    input.close();

    if (projectName.empty())
        projectName = getNameFromFull( inputFilename );


    if (makeNetlistCache)
    {

        if (!outputFilename.empty())
        {
            std::string outputFinalName = generateOutputFilename( inputFilename, outputFilename, getFileName(projectName), "rbc_net" );
            outputStream.open(outputFinalName);

            if (!outputStream)
            {
                LOG_ERR_OPT<<"failed to open output file '"<<outputFinalName<<"'\n";
                return 2;
            }
        }

        //bool 
        netlistWritetCache( os, projectName, allNets );
        return 0;
    }


    //----------
    expression_list_t processingRules;
    for( auto rulesFilename : rules )
    {
        //std::string fullName = libFileName + std::string(".cmp");
        std::string foundName;
        std::ifstream rulesStream;
        if ( !includeSearch( rulesStream, rulesFilename, foundName, makeIncVectorFromFileName(inputFilename), /* <= swap to change lookup order => */ rbcOpts.rulesPaths ) )
        {
            LOG_ERR_OPT<<"rules file '"<<rulesFilename<<"' not found\n";
            return 3;
        }

        expression_list_t lst;
        
		FileSet::file_id_t fileNo = rbcOpts.usedFiles.getFileId(foundName);
		size_t lineNo = 0;
        if ( !readList(fileNo, lineNo, rulesStream, lst ) )
        {
            LOG_ERR<<"failed to read rules file '"<<rulesFilename<<"'\n";
            return 3;
        }
        rulesStream.close();

        removeCommentExpressionLists( lst );

        processingRules.insert( processingRules.end(), lst.begin(), lst.end() );
    }

    {
        //auto &sv = 
        LOG_MSG("conf-dump-short-rules")<<"--- Short rules dump (Common)\n";
        dumpExpressionListSimpleShort( rbcOpts, "conf-dump-short-rules", processingRules );

        auto &sv = LOG_MSG("conf-dump-rules")<<"--- Rules dump (Common)"; // "\nA"
        dumpExpressionList( rbcOpts, 0, sv, "conf-dump-rules", processingRules );
    }


    if (!extractCustomGenerators( processingRules, reportGenerators ))
    {
        LOG_ERR_OPT<<"failed to read custom generator rules\n";
        return 3;
    }


///-----------------
/*
    std::map< std::string, IReportGenerator* > reportGenerators;

    addReportGenerator( reportGenerators, new SummaryReportGenerator() );
    addReportGenerator( reportGenerators, new McuReportGenerator() );
    addReportGenerator( reportGenerators, new PeriphReportGenerator() );
*/
///-----------------






    std::vector< ExternalDeviceConnectInfo > externalDeviceConnections;
    if (!parseExternalConnectionsRules(rbcOpts, processingRules, externalDeviceConnections, makeIncVectorFromFileName(inputFilename), rbcOpts.libPaths, allNets ))
    {
        LOG_ERR_OPT<<"error in 'connection' rules\n";
        return 3;
    }

    if (!connectExternalDevices( rbcOpts, externalDeviceConnections, allNets ))
    {
        LOG_ERR_OPT<<"failed to connect devices\n";
        return 3;
    }


    if (!rbcOpts.packagesDb.extractPackages(rbcOpts, processingRules ))
        return 4;

    if (!rbcOpts.extractGroupingRules( processingRules ))
        return 4;

    rbcOpts.packagesDb.logKnownPackages(rbcOpts);

    if (!rbcOpts.packagesDb.extractDesignatorAssignments(rbcOpts, processingRules ))
        return 4;

    if (!rbcOpts.packagesDb.applyDesignatorAssignments(rbcOpts, allNets ))
        return 5;

    // processingRules
    // expression_list_t processingRules;


    //???
    std::set<ComponentClass> importantComponentClasses;
    importantComponentClasses.insert(ComponentClass::cc_DD         );
    importantComponentClasses.insert(ComponentClass::cc_DA         );
    importantComponentClasses.insert(ComponentClass::cc_AA         );
    importantComponentClasses.insert(ComponentClass::cc_TRANSISTOR );

    bool allowProcessAssemblies = true;

    std::vector<ComponentTypePackage> types;

    //types.push_back(ComponentTypePackage("AMS_AS5145-341457"));

    collectComponentTypes( allNets, types, importantComponentClasses, allowProcessAssemblies );


    if (showComponentsList)
    {
        std::sort( types.begin(), types.end() );
        for( auto typePackage : types )
        {
            std::cout<<typePackage.type<<"\n";
        }
        return 0;
    }



    //----------
    // Read libs
    std::vector<std::string> libFiles;

    bool writeLibCache = true;
    //----------

    //if (libFiles.empty())
    {
        for( auto typePackage : types )
        {

            std::string packageCanonicalName;
            if (!rbcOpts.packagesDb.getCanonicalPackageName( typePackage.package, packageCanonicalName ))
                packageCanonicalName = typePackage.package;
            std::vector<std::string> lookupNames = generateComponentNames( typePackage.type, packageCanonicalName,  "user" ) ;
            libFiles.insert( libFiles.end(), lookupNames.begin(), lookupNames.end() );
        }
       
        makeUniqueVector(libFiles);
    }


    //----------

    std::vector< ComponentInfo > components;

    // Read 
    {
        std::set<std::string> readedNames;
       
        for( auto libFileName : libFiles)
        {
            LOG_MSG("search-lib-log")<<"Lookup for: "<<libFileName<<"\n";
            
            if (readedNames.find(libFileName)!=readedNames.end())
                continue;
       
            std::string fullName = libFileName + std::string(".cmp");
            std::string foundName;
            std::ifstream libStream;
            std::vector<std::string> checkedFiles;
            if ( !includeSearch( libStream, fullName, foundName, makeIncVectorFromFileName(inputFilename), /* <= swap to change lookup order => */ rbcOpts.libPaths, &checkedFiles ) )
            {
                LOG_MSG("search-lib-log")<<"- not found\n";
                for( const auto &cf : checkedFiles)
                {
                    LOG_MSG("search-lib-details")<<"- not found at - "<<cf<<"\n";
                }
                continue;
            }
       
            readedNames.insert(libFileName);

            LOG_MSG("search-lib-log")<<"+ found: "<<foundName<<"\n";

            componentReadCmp(rbcOpts, rbcOpts.usedFiles.getFileId(foundName), libStream, components );
            libStream.close();
        }

        if (verboseLibSearch)
        {
            LOG_MSG("search-lib-summary")<<"Total lib files checked : "<<libFiles.size()<<"\n";
            LOG_MSG("search-lib-summary")<<"Found unique lib files  : "<<readedNames.size()<<"\n";
        }

    }


    //----------
    auto reportIt = reportGenerators.find(reportType);
    if (reportIt == reportGenerators.end())
    {
        LOG_ERR_OPT<<"unknow report type '"<<reportType<<"'\n";
        return 3;
    }

    IReportGenerator* pGen = reportIt->second;

    //----------

    traverseComponents( allNets, ComponentClassUpdater(rbcOpts)             , std::set<ComponentClass>(), allowProcessAssemblies );
    traverseComponents( allNets, ComponentTypesUpdater(rbcOpts, components) , importantComponentClasses, allowProcessAssemblies );

    std::map<std::string, std::set<std::string> >   componentsNoPackageDesignators;
    traverseComponents( allNets, ComponentPackageExistenceChecker(rbcOpts, componentsNoPackageDesignators), importantComponentClasses, allowProcessAssemblies );
    
    traverseComponents( allNets, ComponentInternalNetsPublisher()           , std::set<ComponentClass>(), true );
    traverseComponents( allNets, ComponentPinMatchApplier(rbcOpts)          , std::set<ComponentClass>(), true );

    makeDesignatorsMap(allNets);


    //----------
    for( const auto &netlist : allNets )
    {
        //cout<<netlist.second<<"\n";
        LOG_MSG("net-dump-cmn")<<"--- Netlist: "<<netlist.second.name<<"\n";
        LOG_MSG("net-dump-cmps")<<"--- Components\n";
        for(auto ci : netlist.second.components)
        {
            auto &s = LOG_MSG("net-dump-cmps")<<ci.second.designator<<" - "<<ci.second.typeName<<" - "<<ci.second.getClassDisplayString()<<" - assembly: ";

            if (ci.second.assembly>1)
                s<<ci.second.assembly;
            else
                s<<"not";

            s<<" - "<<ci.second.package<<" - "<<ci.second.sheetName<<"\n";


            for( auto pi : ci.second.pins )
            {
                LOG_MSG("net-dump-pins")<<"    "<<pi.first<<"\n";
            }
        }

        LOG_MSG("net-dump-nets")<<"--- Nets\n"; 
        for(auto nt : netlist.second.nets)
        {
			auto &s = LOG_MSG("net-dump-nets")<< "Net: " << nt.second.name << ":";
            for( auto nd : nt.second.nodes )
            {
                s<<" "<<nd.first;
            }
            s<<"\n";
        }
    }


    ConnectionBuildingOptions connectionBuildingOptions;

    connectionBuildingOptions.extractStopNets(rbcOpts, processingRules);


    if (!outputFilename.empty())
    {
        std::string outputFinalName = generateOutputFilename( inputFilename, outputFilename, getFileName(projectName), pGen->getDefaultFileExt() );
        outputStream.open(outputFinalName);

        if (!outputStream)
        {
            LOG_ERR_OPT<<"failed to open output file '"<<outputFinalName<<"'\n";
            return 4;
        }
    }

    pGen->generateReport( rbcOpts, os, allNets, components, processingRules, connectionBuildingOptions );

/*    
    for( auto n : allNets )
    {
        cout<<n.second;
    }
*/
    return 0;
   }

