#pragma once

//#include "rdlc-core/read_ini.h"
#include "rdlc-core/isa.h"
#include "rdlc-core/cli_impl.h"

extern RoboconfOptions rbcOpts;

// 0 - ok, 1 normal stop, -1 - error
inline
int parseArg( std::string a, ICommandLineOptionCollector *pCol, bool fBuiltin, bool ignoreInfos )
{
    trim(a);

    if (a.empty())
    {
        LOG_ERR_OPT<<"invalid (empty) argument\n";
        return -1;
    }

    CommandLineOption opt(a, pCol);

    if (!opt.isHelpOption())
    {
        //pCol = 0;
        opt.resetOptionInfoCollector();
    }

    if (opt.isOption())
    {
        std::string errMsg;
        int intVal;

        if (opt.name.empty())
        {
            LOG_ERR_OPT<<"invalid (empty) option name\n";
            return -1;
        }

        if (opt.isOption("quet") || opt.isOption('q') || opt.setDescription("Operate quetly"))
        {
            quet = true;
        }
        else if (opt.isOption("build-info") || opt.isOption("build-info-x") || opt.setDescription("Show extended info about version/build"))
        {
            if (hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                std::string indent;
                //if (opt.isOption("build-info-x"))
                if (opt.name=="build-info-x")
                    indent = "  ";
                printNameVersion(indent);
                printBuildDateTime(indent);
                printCommitHash(indent);
                return 1;
            }
        }
        else if (opt.isOption("version") || opt.isOption('v') || opt.setDescription("Show version info"))
        {
            if (hasHelpOption) return 0;

            if (!ignoreInfos)
            {
                printOnlyVersion();
                return 1;
            }
        }
        else if (opt.isOption("where") || opt.setDescription("Show where the executable file is"))
        {
            if (hasHelpOption) return 0;

            //LOG_MSG_OPT<<exeFullName<<"\n";
            std::cout << exeFullName << "\n";
            return 0;
        }
/*
        else if (opt.isOption("verbose") || opt.isOption('V') || opt.setDescription("Operate verbose") )
        {
            if (quet)
                operateVerbose = false;

            return 0;
        }
*/
        else if (opt.isOption("regexp-test") || opt.setParam("REGEX") || opt.setDescription("Test regex") )
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"expression not taken (--regexp-test)\n";
                return -1;
            }

            regexpTest = opt.optArg;
        }
        else if (opt.isOption("regexp-template") || opt.setParam("TPL") || opt.setDescription("Test regex template"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"template not taken (--regexp-template)\n";
                return -1;
            }

            regexpTestTarget = opt.optArg;
        }
        else if (opt.isOption("report") || opt.isOption('R') || opt.setParam("REPORT") || opt.setDescription("Set output type"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"LOG_ERR_OPTreport type not taken (--csv-component)\n";
                return -1;
            }

            reportType = opt.optArg;
        }
        else if (opt.isOption("csv-component") || opt.isOption('S') || opt.setParam("NAME") || opt.setDescription("Component"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"component name not taken (--csv-component)\n";
                return -1;
            }
            std::vector<std::string> names;
            splitToVector(opt.optArg, names, ',' );
            componentImportOptions.namesList.insert( componentImportOptions.namesList.end(), names.begin(), names.end() );
        }
        else if (opt.isOption("csv-import-option") || opt.isOption('O') || opt.setParam("OPT") || opt.setDescription("Import options"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"csv import option not taken (--csv-import-option)\n";
                return -1;
            }

            std::string importOptName, importOptVal;
            if (!splitToPair(opt.optArg, importOptName, importOptVal, ':'))
            {
                LOG_ERR_OPT<<"illformed csv import option (--csv-import-option)\n";
                return -1;
            }

            if (importOptName=="pin-filter-chars")
            {
                if (importOptVal=="default")
                    componentImportOptions.invalChars = "[]()/*";
                else if (importOptVal=="clear")
                    componentImportOptions.invalChars.clear();
                else
                    componentImportOptions.invalChars = importOptVal;
            }
            else if (importOptName=="description")
            {
                componentImportOptions.description = importOptVal;
            }
            else if (importOptName=="datasheet")
            {
                componentImportOptions.datasheet = importOptVal;
            }
            else if (importOptName=="refman")
            {
                componentImportOptions.refman = importOptVal;
            }
            else if (importOptName=="errata")
            {
                componentImportOptions.errata = importOptVal;
            }
            else if (importOptName=="package")
            {
                componentImportOptions.package = rbcOpts.packagesDb.getCanonicalPackageName(importOptVal);
            }
            else if (importOptName=="purpose")
            {
                componentImportOptions.purpose = importOptVal;
            }
            else if (importOptName=="modules")
            {
                std::vector<std::string> names;
                splitToVector(importOptVal, names, ',' );
                componentImportOptions.modules.insert( componentImportOptions.modules.end(), names.begin(), names.end() );
                makeUniqueVector( componentImportOptions.modules );
            }
            else if (importOptName=="headers") // C/C++ headers required for component
            {
                std::vector<std::string> names;
                splitToVector(importOptVal, names, ',' );
                componentImportOptions.headers.insert( componentImportOptions.headers.end(), names.begin(), names.end() );
                makeUniqueVector( componentImportOptions.headers );
            }
            else if (importOptName=="csv-separator")
            {
                if (importOptVal=="comma" || importOptVal==",")
                    componentImportOptions.csvSeparator = ',';
                else if (importOptVal=="semicolon" || importOptVal==";")
                    componentImportOptions.csvSeparator = ';';
                else
                {
                    LOG_ERR_OPT<<"invalid value in csv import option 'csv-separator' (--csv-import-option)\n";
                    return -1;
                }
            }
            else if (importOptName=="designator-field-title")
            {
                componentImportOptions.designatorFieldCaption = importOptVal;
            }
            else if (importOptName=="name-field-title")
            {
                componentImportOptions.nameFieldCaption = importOptVal;
            }

        }
        else if (opt.isOption("make-cache") || opt.isOption('H') || opt.setDescription("Make net cache"))
        {
            if (hasHelpOption) return 0;

            makeNetlistCache = true;
        }
        else if (opt.isOption("make-dump") || opt.setDescription("Make dump"))
        {
            if (hasHelpOption) return 0;

            makeDump = true;
        }
        //else if (opt.isOption("verbose-lib-search") || opt.setDescription("Lib search verbose mode"))
        //{
        //    verboseLibSearch = true;
        //}
        else if (opt.isOption("show-components") || opt.isOption('P') || opt.setDescription("Show components list"))
        {
            if (hasHelpOption) return 0;

            showComponentsList = true;
        }
        else if (opt.isOption("var") || opt.setParam("VAR") || opt.setDescription("Set variable"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"var not taken (--var)\n";
                return -1;
            }

            std::string varName, varVal;
            splitToPair(opt.optArg, varName, varVal, ':');
            rbcOpts.globalVars[varName] = varVal;
        }
        else if (opt.isOption("dont-warn") || opt.isOption('W') || opt.setParam("WARNTYPE") || opt.setDescription("Disable warnings"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"warning type not taken (--dont-warn)\n";
                return -1;
            }

            rbcOpts.disableWarnings(opt.optArg);
        }
        else if (opt.isOption("verbose") || opt.isOption('V') || opt.setParam("DETAILSTYPE") || opt.setDescription("Operate verbose, enable specified messages/details"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
                rbcOpts.enableMessages("9");
            else
                rbcOpts.enableMessages(opt.optArg);
        }
        else if (opt.isOption("log-source-code-pos") || opt.setDescription("Log source code position of message in error/warning messages"))
        {
            if (hasHelpOption) return 0;

            rbcOpts.logOptions.logSourceInfo = true;
            //generatorOptions.protocolVersion = 1;
        }
        else if (opt.isOption("time") || opt.isOption('T') || opt.setDescription("Report time elapsed"))
        {
            if (hasHelpOption) return 0;

            showTime = true;
        }
/*
        else if (opt.isOption("include") || opt.isOption('I'))
        {
            if (fBuiltin && disableBuiltinIncludes)
                return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"include path not taken (--include)\n";
                return -1;
            }

            std::vector<std::string> pathList;
            splitToVector(opt.optArg, pathList, ';' );
            for( auto &incPath : pathList )
            {
                trim(incPath);

                if (incPath.empty())
                    continue;

                if (incPath=="$INCLUDE$")
                {
                    incPath = progIncludePath;
                }

                includePaths.push_back(incPath);
            }
        }
*/
        else if (opt.isOption("datasheet-path") || opt.setParam("PATH") || opt.setDescription("Datasheet path"))
        {
            if (hasHelpOption) return 0;

            //if (fBuiltin && disableBuiltinLibPath)
            //    return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"datasheet path not taken (--datasheet-path)\n";
                return -1;
            }

            std::vector<std::string> pathList;
            splitToVector(opt.optArg, pathList, ';' );
            for( auto &libPath : pathList )
            {
                trim(libPath);

                if (libPath.empty())
                    continue;

                libPath = rbcOpts.macroExpandString(libPath);
                /*
                if (libPath=="$DATASHEET$")
                {
                    libPath = progDatasheetPath;
                }
                */
                rbcOpts.datasheetPaths.push_back(libPath);
            }
        }
        else if (opt.isOption("lib-path") || opt.isOption('L') || opt.setParam("PATH") || opt.setDescription("Lib path"))
        {
            if (hasHelpOption) return 0;

            if (fBuiltin && disableBuiltinLibPath)
                return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"library path not taken (--lib-path)\n";
                return -1;
            }

            std::vector<std::string> pathList;
            splitToVector(opt.optArg, pathList, ';' );
            for( auto &libPath : pathList )
            {
                trim(libPath);

                if (libPath.empty())
                    continue;

                libPath = rbcOpts.macroExpandString(libPath);
                /*
                if (libPath=="$LIB$")
                {
                    libPath = progLibPath;
                }
                else if (libPath=="$LIB_USER$")
                {
                    libPath = progUserLibPath;
                }
                */
                rbcOpts.libPaths.push_back(libPath);
            }
        }
        else if (opt.isOption("rules-path") /*  || opt.isOption('L') */  || opt.setParam("PATH") || opt.setDescription("Rules path"))
        {
            if (hasHelpOption) return 0;

            if (fBuiltin && disableBuiltinLibPath)
                return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"rules path not taken (--rules-path)\n";
                return -1;
            }

            std::vector<std::string> pathList;
            splitToVector(opt.optArg, pathList, ';' );
            for( auto &rulesPath : pathList )
            {
                trim(rulesPath);

                if (rulesPath.empty())
                    continue;

                rulesPath = rbcOpts.macroExpandString(rulesPath);
                /*
                if (rulesPath=="$RULES$")
                {
                    rulesPath = progRulesPath;
                }
                */

                rbcOpts.rulesPaths.push_back(rulesPath);
            }
        }
        else if (opt.isOption("class-rules") /*  || opt.isOption('L') */ || opt.setParam("CLASSES-FILE") || opt.setDescription("Set class detection rules file") )
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg() || opt.optArg.empty())
            {
                LOG_ERR_OPT<<"class detection rules name not taken (--class-rules)\n";
                return -1;
            }

            //rbcOpts.componentsClassDetectionRulesName = opt.optArg;
            rbcOpts.componentsClassDetectionRulesFiles.emplace_back(opt.optArg);

        }
        else if (opt.isOption("rules") /*  || opt.isOption('L') */ || opt.setParam("RULES") || opt.setDescription("Set rules") )
        {
            if (hasHelpOption) return 0;

            //if (fBuiltin && disableBuiltinRulesPath)
            //    return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"rules file name not taken (--rules)\n";
                return -1;
            }

            std::vector<std::string> rulesList;
            char splitCh = contains( opt.optArg, ",;" );
            if ( splitCh )
            {
                splitToVector( opt.optArg, rulesList, splitCh );
            }
            else
                rulesList.push_back(opt.optArg);

            for( auto &rfName : rulesList )
            {
                trim(rfName);

                if (rfName.empty())
                    continue;

                rules.push_back(rfName);
            }
        }
        else if (opt.isOption("local-lib-cache") || opt.setParam("FILE") || opt.setDescription("Set lib cache file"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"library cache filename not taken (--local-lib-cache)\n";
                return -1;
            }

            localLibCacheFilename = opt.optArg;
        
        }
        else if (opt.isOption("no-default-rules") || opt.setDescription("Disable default rules"))
        {
            if (hasHelpOption) return 0;

            noDefaultRules = true;
        }
        else if (opt.isOption("no-builtin-options") || opt.setDescription("Disable builtin options"))
        {
            if (hasHelpOption) return 0;

            // simple skip
        }
        else if (opt.isOption("no-builtin-lib-paths") || opt.setDescription("Disable builtin lib paths"))
        {
            if (hasHelpOption) return 0;

            // simple skip
        }
        else if (opt.isOption("no-builtin-rules-paths") || opt.setDescription("Disable builtin rules paths"))
        {
            if (hasHelpOption) return 0;

            // simple skip
        }
        else if (opt.isOption("prolog") || opt.setParam("FILE") || opt.setDescription("Tells to read prolog for generated files from file 'filename'. Prolog can taken eplicitly using prefix - 'inline:SOME_TEXT'"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"prolog file name not taken (--prolog)\n";
                return -1;
            }

            try
            {
                rbcOpts.readProlog(opt.optArg);
            }
            catch( const std::exception &e)
            {
                LOG_ERR_OPT<<"error reading epilog file (--prolog): "<<e.what()<<"\n";
                return -1;
            }
            catch(...)
            {
                LOG_ERR_OPT<<"error reading prolog file (--prolog): unknown error\n";
                return -1;
            }
            //if (!rbcOpts.readProlog(opt.optArg))
            //    return -1;
        }
        else if (opt.isOption("epilog") || opt.setParam("FILE") || opt.setDescription("Tells to read epilog for generated files from file 'filename'. Epilog can taken eplicitly using prefix - 'inline:SOME_TEXT'"))
        {
            if (hasHelpOption) return 0;

            if (!opt.hasArg())
            {
                LOG_ERR_OPT<<"epilog file name not taken (--prolog)\n";
                return -1;
            }

            try
            {
                rbcOpts.readEpilog(opt.optArg);
            }
            catch( const std::exception &e)
            {
                LOG_ERR_OPT<<"error reading epilog file (--epilog): "<<e.what()<<"\n";
                return -1;
            }
            catch(...)
            {
                LOG_ERR_OPT<<"error reading epilog file (--epilog): unknown error\n";
                return -1;
            }
            /* if (!rbcOpts.readEpilog(opt.optArg))
             *     return -1;
             */
        }
        else if (opt.isOption("list-encodings") || opt.setDescription("List of supported encodings"))
        {
            if (hasHelpOption) return 0;

            EncodingsApi* pEncApi = getEncodingsApi();
            size_t encIdx = 0;
            std::string name, info;

            std::cout<<"Supported encodings:\n";

            while(pEncApi->getEncodingInfo(encIdx++, name, info))
            {
                std::cout<<name;
                if (name.size()<18)
                {
                    std::cout<<std::string( 18-name.size(), ' ' );
                }
                std::cout<<" - "<<info<<"\n";
            }

            return 1;

        }
        else if (opt.setParam("CLR", 0, "no/none/file|" 
                                        "ansi/term|" 
                                        #if defined(WIN32) || defined(_WIN32)
                                        "win32/win/windows/cmd/console"
                                        #endif
                             )
              || opt.setInitial(-1) || opt.isOption("console-colors") 
              || opt.setDescription("Force set console output coloring")
              /* ", can be:\nno, none, file - disable coloring\nansi, term - set ansi terminal coloring\nwin32, win, windows, cmd, console - windows console specific coloring method" */
              )
        {
            if (hasHelpOption) return 0;

            umba::term::ConsoleType res;
            auto mapper = [](int i) -> umba::term::ConsoleType
                          {
                              switch(i)
                              {
                                  case 0 : return umba::term::ConsoleType::file;
                                  case 1 : return umba::term::ConsoleType::ansi_terminal;
                                  case 2 : return umba::term::ConsoleType::windows_console;
                                  default: return umba::term::ConsoleType::file;
                              };
                          };
            if (!opt.getParamValue( res, errMsg, mapper ) )
            {
                LOG_ERR_OPT<<errMsg<<"\n";
                return -1;
            }

            coutWriter.forceSetConsoleType(res);
            cerrWriter.forceSetConsoleType(res);
        }
        else if (opt.isOption("autocomplete-install") || opt.setDescription("Install autocompletion to bash/clink(cmd)"))
        {
            if (hasHelpOption) return 0;

            //return autocomplete(opt, true);
            return autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), true, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }
        else if (opt.isOption("autocomplete-uninstall") || opt.setDescription("Remove autocompletion from bash/clink(cmd)"))
        {
            if (hasHelpOption) return 0;

            //return autocomplete(opt, false);
            return autocompletionInstaller( pCol, opt, pCol->getPrintHelpStyle(), false, [&]( bool bErr ) -> decltype(auto) { return bErr ? LOG_ERR_OPT : LOG_MSG_OPT; } );
        }
        else if (opt.isHelpStyleOption())
        {
            // Job is done in isHelpStyleOption
        }
        else if (opt.isHelpOption()) // if (opt.isOption("help") || opt.isOption('h') || opt.isOption('?'))
        {
            if (!ignoreInfos)
            {
                if (pCol && !pCol->isNormalPrintHelpStyle())
                    quet = true;
                //printNameVersion();
                if (!quet)
                {
                    printBuildDateTime();
                    printCommitHash();
                    std::cout<<"\n";
                //printHelp();
                }

                if (pCol && pCol->isNormalPrintHelpStyle() && argsNeedHelp.empty())
                    std::cout<<"Usage: roboconf [OPTIONS] input_file [output_file]\n\nOptions:\n\n"<<opt.getHelpOptionsString();
                
                if (pCol) // argsNeedHelp
                    std::cout<<pCol->makeText( 64, &argsNeedHelp );

                return 1;
            }
            return 0; // simple skip then parse builtins
        }
        else
        {
            LOG_ERR_OPT<<"unknown option: "<<opt.argOrg<<"\n";
            return -1;
        }

        return 0;

    } // if (opt.isOption())
    else if (opt.isResponseFile())
    {
        std::ifstream optFile(opt.name.c_str());
        if (!optFile)
        {
            LOG_ERR_OPT<<"failed to read response file '"<<opt.name<<"'\n";
            return -1;
        }
        else
        {
            std::string optLine;
            while( std::getline( optFile, optLine) )
            {
                trim(optLine);
                if (optLine.empty())
                    continue;

                if (isComment( optLine ))
                    continue;

                int paRes = parseArg( optLine, pCol, false, true );
                if (paRes)
                {
                   return paRes;
                }
            }
        }

        return 0;
    
    }

    
    if (inputFilename.empty())
        inputFilename = a;
    else
        outputFilename = a;

    return 0;

}
