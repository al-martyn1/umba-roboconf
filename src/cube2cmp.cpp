#if defined(WIN32) || defined(_WIN32)
    #include <winsock2.h>
    #include <windows.h>
#endif

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <set>

#include "rdlc-core/fsUtils.h"
#include "ioUtils.h"

#include "pugixml/src/pugixml.hpp"
#include "cubeXmlParsing.h"
#include "icUtils.h"


#include "rapidjson-master/include/rapidjson/rapidjson.h"
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/writer.h"
#include "rapidjson-master/include/rapidjson/reader.h"
#include "rapidjson-master/include/rapidjson/stringbuffer.h"


using namespace rapidjson;



//#include "pugixml/src/pugixml.hpp"


//#include <filesystem>
//#include <experimental\filesystem>
//namespace fs = std::filesystem;

//using namespace std;
using std::cout;
using std::cerr;
using std::endl;







int main( int argc, char* argv[] )
{
    if (argc<3)
    {
        cout<<"Usage: cube2cmp RootCubeMX RootCmp\nRootCubeMX - is often is \"C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeMX\"\n";
        // C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeMX
        return 0;
    }


    //pinCountsByPackage[""]  = ;

    std::map< std::string , CubeDocFileInfo > cubeDocFileInfos;
    std::map< std::string , CubeMcuDocs >     cubeMcuDocs;

    std::map<std::string, McuFamilyInfo >  mcuNames;
    std::map< std::string , std::set<std::string> > mcuFamilies;
    std::map< std::string , std::set<std::string> > mcuSubFamilies;
    std::map< std::string , std::set<std::string> > mcuCores;

    std::map< std::string , McuDocInfo >             mcuDocInfos;
    std::map< std::string, McuDocShortMaskSelector > docTypeShortMaskSelectors;
    

    bool generateCmp                     = false;
    bool generateStmDetect               = false;

    // doc lists generators
    bool generateDocListDatasheet        = false;
    bool generateDocListRefman           = false;
    bool generateDocListErrata           = false;
    bool generateDocListAppnotes         = false;
    bool generateDocListProgman          = false;

   // download batch generators (wget)
    bool wgetDatasheet                   = false;
    bool wgetRefman                      = false;
    bool wgetErrata                      = false;
    bool wgetAppnotes                    = false;
    bool wgetProgman                     = false;
    
    std::vector<std::string> argvNames;

    for( int argIdx = 1; argIdx<argc; ++argIdx)
    {
        std::string argStr = argv[argIdx];
        if (argStr=="--cmp")
            generateCmp = true;
        else if (argStr=="--detect")
            generateStmDetect = true;

        else if (argStr=="--doclistdatasheet")
            generateDocListDatasheet = true;
        else if (argStr=="--doclistrefman")
            generateDocListRefman = true;
        else if (argStr=="--doclisterrata")
            generateDocListErrata = true;
        else if (argStr=="--doclistappnotes")
            generateDocListAppnotes = true;
        else if (argStr=="--doclistprogman")
            generateDocListProgman = true;

        else if (argStr=="--wgetdatasheet")
            wgetDatasheet = true;
        else if (argStr=="--wgetrefman")
            wgetRefman = true;
        else if (argStr=="--wgeterrata")
            wgetErrata = true;
        else if (argStr=="--wgetappnotes")
            wgetAppnotes = true;
        else if (argStr=="--wgetprogman")
            wgetProgman = true;
            
        else if (startsWith(argStr, "--"))
        {
            cerr<<"Unknown option: "<<argStr<<endl;
            return 1;
        }
        else
            argvNames.push_back(argStr);
    }

    if (argvNames.size()<2)
    {
        cerr<<"Usage --cmp|--detect RootCubeMX TargetRoot\n  RootCubeMX - 'STM32CubeMX' directory, e.g. \"C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeMX\""<<endl;
        return 1;
    }

    std::string cubeMxRoot = argvNames[0];
    std::string targetRoot = argvNames[1];

    cout<<"CubeMX root : "<<cubeMxRoot<<"\n";
    cout<<"Target root : "<<targetRoot<<"\n";

    #define PRINT_BOOL_OPTION(varName)  cout<< #varName << " : " << varName << "\n"
    PRINT_BOOL_OPTION(generateCmp                    );
    PRINT_BOOL_OPTION(generateStmDetect              );

    PRINT_BOOL_OPTION(generateDocListDatasheet       );
    PRINT_BOOL_OPTION(generateDocListRefman          );
    PRINT_BOOL_OPTION(generateDocListErrata          );
    PRINT_BOOL_OPTION(generateDocListAppnotes        );
    PRINT_BOOL_OPTION(generateDocListProgman         );

    PRINT_BOOL_OPTION(wgetDatasheet                  );
    PRINT_BOOL_OPTION(wgetRefman                     );
    PRINT_BOOL_OPTION(wgetErrata                     );
    PRINT_BOOL_OPTION(wgetAppnotes                   );
    PRINT_BOOL_OPTION(wgetProgman                    );


    std::string mcuListFile = appendPath( cubeMxRoot, "/db/mcu/families.xml" );
    cout<<"Reading     : "<<mcuListFile<<"\n";

    std::string mcuDocsJsonFile                   = appendPath( cubeMxRoot, "/db/plugins/mcufinder/mcu/mcusDocs.json" );
    std::string mcuFeaturesAndDescriptionJsonFile = appendPath( cubeMxRoot, "/db/plugins/mcufinder/mcu/mcusFeaturesAndDescription.json" );


    try
    {
        std::string mcuDocsJson = readFileSafe( mcuDocsJsonFile );
        if (mcuDocsJson.empty())
        {
            throw std::runtime_error( "Failed to read CubeMX mcusDocs.json" );
        }

        std::string mcuFeaturesAndDescriptionJson = readFileSafe( mcuFeaturesAndDescriptionJsonFile );
        if (mcuFeaturesAndDescriptionJson.empty())
        {
            throw std::runtime_error( "Failed to read CubeMX mcusFeaturesAndDescription.json" );
        }

        Document mcuDocsJsonDocument;
        mcuDocsJsonDocument.Parse(mcuDocsJson.c_str());
        Value& mcusDocFiles = mcuDocsJsonDocument["Files"];

        for (SizeType i = 0; i < mcusDocFiles.Size(); i++) // rapidjson uses SizeType instead of size_t.
        {
            CubeDocFileInfo  cubeDocFileInfo;
            cubeDocFileInfo.fileId      = mcusDocFiles[i]["id_file"].GetString();
            cubeDocFileInfo.docName     = mcusDocFiles[i]["name"].GetString();
            cubeDocFileInfo.displayName = mcusDocFiles[i]["displayName"].GetString();
            cubeDocFileInfo.title       = mcusDocFiles[i]["title"].GetString();
            cubeDocFileInfo.url         = mcusDocFiles[i]["URL"].GetString();
            cubeDocFileInfo.docTypeDescription = mcusDocFiles[i]["type"].GetString();
            if (cubeDocFileInfo.docTypeDescription=="Application note")
            {
                cubeDocFileInfo.docType = "appnote";
            }
            else if (cubeDocFileInfo.docTypeDescription=="Datasheet")
            {
                cubeDocFileInfo.docType = "datasheet";
            }
            else if (cubeDocFileInfo.docTypeDescription=="Reference manual")
            {
                cubeDocFileInfo.docType = "refman";
            }
            else if (cubeDocFileInfo.docTypeDescription=="User manual")
            {
                cubeDocFileInfo.docType = "manual";
            }
            else if (cubeDocFileInfo.docTypeDescription=="Errata sheet")
            {
                cubeDocFileInfo.docType = "errata";
            }
            else if (cubeDocFileInfo.docTypeDescription=="Programming manual")
            {
                cubeDocFileInfo.docType = "progman";
            }
            else if (cubeDocFileInfo.docTypeDescription=="Data brief")
            {
                cubeDocFileInfo.docType = "breif";
            }
            else
            {
                cerr<<"Unknown document type: "<<cubeDocFileInfo.docTypeDescription<<endl;
            }
            cubeDocFileInfos[cubeDocFileInfo.fileId] = cubeDocFileInfo;
        }

        // {"package":"WLCSP64","refname":"STM32F103RCYx","iONb":"50"}
        Document mcuFeaturesAndDescriptionJsonDocument;
        mcuFeaturesAndDescriptionJsonDocument.Parse(mcuFeaturesAndDescriptionJson.c_str());
        Value& mcuFnD = mcuFeaturesAndDescriptionJsonDocument["MCUs"];
        for (SizeType i = 0; i < mcuFnD.Size(); i++) // rapidjson uses SizeType instead of size_t.
        {
            CubeMcuDocs docs;

            Value& subpackages = mcuFnD[i]["subpackages"];
            if (subpackages.IsArray())
            {
                for (SizeType s = 0; s < subpackages.Size(); s++) // rapidjson uses SizeType instead of size_t.
                {
                    std::string refname = subpackages[s]["refname"].GetString();
                    docs.refNames.push_back(refname);
                    //Value& subPkg = subpackages[s];
                }
            }

            if (docs.refNames.empty())
                docs.refNames.push_back(mcuFnD[i]["name"].GetString());
            //docs.refName = mcuFnD[i]["name"].GetString(); // STM8AL3L68Tx
            docs.rpn     = mcuFnD[i]["RPN"].GetString();  // STM8AL3L68
            Value& docFiles = mcuFnD[i]["files"];
            for(SizeType d = 0; d < docFiles.Size(); d++)
            {
                std::string fileId = docFiles[d]["file_id"].GetString();
                std::map< std::string , CubeDocFileInfo >::const_iterator cdfiIt = cubeDocFileInfos.find(fileId);
                if (cdfiIt != cubeDocFileInfos.end())
                {
                    docs.docs[ cdfiIt->second.docType ].push_back(cdfiIt->second); 
                }
            }

            for( auto refName : docs.refNames )
            {
                cubeMcuDocs[refName] = docs;
            }
        }

        std::string familiesXml = readFileSafe( mcuListFile );
        if (familiesXml.empty())
        {
            throw std::runtime_error( "Failed to read CubeMX families.xml" );
        }

        cout<<"Readed      : "<<familiesXml.size()<<" bytes"<<endl;

        pugi::xml_document familiesDocRoot;
        auto pgXmlLoadRes = familiesDocRoot.load_buffer( familiesXml.data(), familiesXml.size()  /* , unsigned int options = parse_default, xml_encoding encoding = encoding_auto */  );
        if (pgXmlLoadRes.status!=pugi::xml_parse_status::status_ok)
        {
            throw std::runtime_error( std::string("Failed to parse CubeMX families.xml - ") + pgXmlLoadRes.description() );
        }

		if (familiesDocRoot.begin() == familiesDocRoot.end())
		{
			throw std::runtime_error(std::string("Failed to parse CubeMX families.xml - missing root node"));
		}

        std::vector< CubeMcuInfo > mcus;

		const pugi::xml_node &familiesNode = *(familiesDocRoot.begin());

        parseFamilies( mcus, familiesNode );

        std::vector< CubeMcuInfo >::const_iterator mcuit = mcus.begin();
        for(; mcuit != mcus.end(); ++mcuit )
        {
            std::string mcuFile = appendPath( appendPath( cubeMxRoot, "/db/mcu/" ), mcuit->name );
            mcuFile += ".xml";
            try
            {
                std::string mcuXml = readFileSafe( mcuFile );
                if (mcuXml.empty())
                {
                    throw std::runtime_error( std::string("Failed to read MCU XML - ") + mcuFile );
                }

                pugi::xml_document mcuDocRoot;
                auto pgMcuXmlLoadRes = mcuDocRoot.load_buffer( mcuXml.data(), mcuXml.size()  /* , unsigned int options = parse_default, xml_encoding encoding = encoding_auto */ );

                if (pgMcuXmlLoadRes.status!=pugi::xml_parse_status::status_ok)
                {
                    throw std::runtime_error( std::string("Failed to parse '") + mcuFile + std::string("' - ") + pgXmlLoadRes.description() );
                }
               
                if (mcuDocRoot.begin() == mcuDocRoot.end())
                {
                    throw std::runtime_error(std::string("Failed to parse '") + mcuFile + std::string("' - missing root node"));
                }

                CubeMcuInfo mcuInfo = *mcuit;
                const pugi::xml_node &mcuNode = *(mcuDocRoot.begin());
                parseMcuPins( mcuInfo.pins, mcuNode, mcuit->rpnName, mcuFile );
               
                std::string outFilename = mcuInfo.rpnName + std::string("-") + icPackageGetCanonicalName(mcuInfo.packageName) + std::string(".cmp");
                std::string outFullFilename = appendPath( targetRoot, outFilename );

                if (generateCmp)
                {
                    std::ofstream ofs( outFullFilename.c_str() );
                    writeCubeMcuInfo( ofs, mcuInfo, cubeDocFileInfos, cubeMcuDocs, true /* writePackage */ );
                }
               
                McuFamilyInfo mcuFamilyInfo = McuFamilyInfo(mcuInfo);
                //std::map<std::string, McuFamilyInfo >  mcuNames;
                mcuNames[mcuFamilyInfo.mcuName] = mcuFamilyInfo;
               
                mcuNames      [ mcuFamilyInfo.mcuName      ] = mcuFamilyInfo;
                mcuFamilies   [ mcuFamilyInfo.mcuFamily    ] .insert(mcuFamilyInfo.mcuName);
                mcuSubFamilies[ mcuFamilyInfo.mcuSubfamily ] .insert(mcuFamilyInfo.mcuName);
                mcuCores      [ mcuFamilyInfo.mcuCore      ] .insert(mcuFamilyInfo.mcuName);
               
                std::map< std::string , CubeMcuDocs >::const_iterator cmdIt = cubeMcuDocs.find(mcuInfo.refName);
                if (cmdIt != cubeMcuDocs.end())
                {
                    //const CubeMcuDocs &mcuDocs = cmdIt->second;
                    std::map< std::string , std::vector<CubeDocFileInfo> >::const_iterator dit = cmdIt->second.docs.begin();
                    for(; dit != cmdIt->second.docs.end(); ++dit)
                    {
                        std::vector<CubeDocFileInfo>::const_iterator dvIt = dit->second.begin();
                        for(; dvIt != dit->second.end(); ++dvIt)
                        {
                            McuDocInfo mcuDocInfo = McuDocInfo(dvIt->docType, dvIt->url);
               
                            std::map< std::string , McuDocInfo >::const_iterator mcudisIt = mcuDocInfos.find(mcuDocInfo.docFileNameCanonical);
                            if ( mcudisIt == mcuDocInfos.end() )
                                mcuDocInfos[mcuDocInfo.docFileNameCanonical] = mcuDocInfo;
               
                            mcuDocInfos[mcuDocInfo.docFileNameCanonical].mcuList         .insert( mcuFamilyInfo.mcuName      );
                            mcuDocInfos[mcuDocInfo.docFileNameCanonical].mcuFamilyList   .insert( mcuFamilyInfo.mcuFamily    );
                            mcuDocInfos[mcuDocInfo.docFileNameCanonical].mcuSubfamilyList.insert( mcuFamilyInfo.mcuSubfamily );
                            mcuDocInfos[mcuDocInfo.docFileNameCanonical].mcuCoreList     .insert( mcuFamilyInfo.mcuCore      );
               
                            
                            //ofs<<"    ("<<dvIt->docType<<" \""<<dvIt->url<<"\")\n";
                        }
                    }
                }


                //

                if (endsWith( mcuInfo.name, "Tx" ))
                {
                    if (generateCmp)
                    {
                        outFilename = mcuInfo.rpnName + std::string(".cmp");
                        outFullFilename = appendPath( targetRoot, outFilename );
                        std::ofstream ofs2( outFullFilename.c_str() );
                        writeCubeMcuInfo( ofs2, mcuInfo, cubeDocFileInfos, cubeMcuDocs, false /* writePackage */ );
                    }
                }

            }
            catch( const std::exception &e )
            {
                cerr<<"Error: "<<e.what()<<endl;
                return 2;
            }
            catch( ... )
            {
                cerr<<"Error: Unknown error"<<endl;
                return 2;
            }
        }

    } // try
    catch( const std::exception &e )
    {
        cerr<<"Error: "<<e.what()<<endl;
        return 2;
    }
    catch( ... )
    {
        cerr<<"Error: Unknown error"<<endl;
        return 2;
    }


    for( const auto & docInfo : mcuDocInfos )
    {
        docTypeShortMaskSelectors[ docInfo.second.docType ].addDocName( docInfo.second.docFileName, docInfo.second.mcuList );
    }
    //docTypeShortMaskSelectors



    if (generateStmDetect)
    {
        std::string outFullFilename = appendPath( targetRoot, "detect_stm32.h" );
        std::ofstream s( outFullFilename.c_str() );
        s<<"#pragma once\n\n";

        std::map<std::string, McuFamilyInfo >::const_iterator mcuNameIt = mcuNames.begin();
        for(; mcuNameIt != mcuNames.end(); ++mcuNameIt)
        {
            std::set<std::string> usedNames;

            s<<"#if defined("<<mcuNameIt->first<<")\n";

            s<<"    #define EASYMCU_TARGET_MCU\n"
             <<"    #define EASYMCU_TARGET_MCU_STM32\n";

            if (!mcuNameIt->second.mcuFamily.empty())
            {
               s<<"    #define EASYMCU_TARGET_MCU_"<<mcuNameIt->second.mcuFamily<<"\n";
               usedNames.insert(mcuNameIt->second.mcuFamily);
            }

            if (!mcuNameIt->second.mcuSubfamily.empty() && usedNames.find(mcuNameIt->second.mcuSubfamily)==usedNames.end())
            {
               s<<"    #define EASYMCU_TARGET_MCU_"<<mcuNameIt->second.mcuSubfamily<<"\n";
               usedNames.insert(mcuNameIt->second.mcuSubfamily);
            }

            if (!mcuNameIt->second.mcuCore.empty() && usedNames.find(mcuNameIt->second.mcuCore)==usedNames.end())
            {
               s<<"    #define EASYMCU_TARGET_MCU_CORE_"<<mcuNameIt->second.mcuCore<<"\n";
               usedNames.insert(mcuNameIt->second.mcuCore);
            }

            s<<"#endif /* "<<mcuNameIt->first<<" */\n\n";
        }
    }
    //std::map< std::string , std::set<std::string> > mcuFamilies;
    //std::map< std::string , std::set<std::string> > mcuSubFamilies;
    //std::map< std::string , std::set<std::string> > mcuCores;

    #define OPEN_MOD , std::ofstream::out | std::ofstream::trunc

    if (generateDocListDatasheet || generateDocListRefman || generateDocListErrata || generateDocListAppnotes || generateDocListProgman )
    {
        std::string outFullFilenameDatasheet  = appendPath( targetRoot, "dl_datasheet_stm32.txt" );
        std::string outFullFilenameRefman     = appendPath( targetRoot, "dl_refman_stm32.txt" );
        std::string outFullFilenameErrata     = appendPath( targetRoot, "dl_errata_stm32.txt" );
        std::string outFullFilenameAppnotes   = appendPath( targetRoot, "dl_appnotes_stm32.txt" );
        std::string outFullFilenameProgman    = appendPath( targetRoot, "dl_progman_stm32.txt" );

        //std::ofstream s( outFullFilename.c_str() );
        std::ofstream sD; // datasheet
        std::ofstream sR; // Refman
        std::ofstream sE; // Errata
        std::ofstream sA; // Appnotes
        std::ofstream sM; // Progman

        if (generateDocListDatasheet)
            sD.open( outFullFilenameDatasheet .c_str() OPEN_MOD );
        if (generateDocListRefman)
            sR.open( outFullFilenameRefman    .c_str() OPEN_MOD );
        if (generateDocListErrata)
            sE.open( outFullFilenameErrata    .c_str() OPEN_MOD );
        if (generateDocListAppnotes)
            sA.open( outFullFilenameAppnotes  .c_str() OPEN_MOD );
        if (generateDocListProgman)
            sM.open( outFullFilenameProgman   .c_str() OPEN_MOD );

        bool printDocType = true;
        std::map< std::string , McuDocInfo >::const_iterator mcudisIt = mcuDocInfos.begin();
        for(; mcudisIt != mcuDocInfos.end(); ++mcudisIt)
        {
            //const McuDocInfo &mcuDocInfo = mcudisIt->second;
            if (mcudisIt->second.docType=="datasheet")
                printMcuDocInfo( sD, mcudisIt->second, printDocType );
            else if (mcudisIt->second.docType=="refman")
                printMcuDocInfo( sR, mcudisIt->second, printDocType );
            else if (mcudisIt->second.docType=="errata")
                printMcuDocInfo( sE, mcudisIt->second, printDocType );
            else if (mcudisIt->second.docType=="appnote")
                printMcuDocInfo( sA, mcudisIt->second, printDocType );
            else if (mcudisIt->second.docType=="progman")
                printMcuDocInfo( sM, mcudisIt->second, printDocType );
        }
        
    }

    if ( wgetDatasheet || wgetRefman || wgetErrata || wgetAppnotes || wgetProgman )
    {
        std::string outFullFilenameDatasheet  = appendPath( targetRoot, "wget_datasheet_stm32.bat" );
        std::string outFullFilenameRefman     = appendPath( targetRoot, "wget_refman_stm32.bat" );
        std::string outFullFilenameErrata     = appendPath( targetRoot, "wget_errata_stm32.bat" );
        std::string outFullFilenameAppnotes   = appendPath( targetRoot, "wget_appnotes_stm32.bat" );
        std::string outFullFilenameProgman    = appendPath( targetRoot, "wget_progman_stm32.bat" );

        //std::ofstream s( outFullFilename.c_str() );
        std::ofstream sD; // datasheet
        std::ofstream sR; // Refmans
        std::ofstream sE; // Erratas
        std::ofstream sA; // Appnotes
        std::ofstream sM; // Progman

        if (wgetDatasheet)
            sD.open( outFullFilenameDatasheet .c_str() OPEN_MOD );
        if (wgetRefman)
            sR.open( outFullFilenameRefman    .c_str() OPEN_MOD );
        if (wgetErrata)
            sE.open( outFullFilenameErrata    .c_str() OPEN_MOD );
        if (wgetAppnotes)
            sA.open( outFullFilenameAppnotes  .c_str() OPEN_MOD );
        if (wgetProgman)
            sM.open( outFullFilenameProgman   .c_str() OPEN_MOD );

        std::map< std::string , McuDocInfo >::const_iterator mcudisIt = mcuDocInfos.begin();
        for(; mcudisIt != mcuDocInfos.end(); ++mcudisIt)
        {
            if (mcudisIt->second.docType=="datasheet")
                printMcuDocInfoWgetBat( sD, mcudisIt->second, docTypeShortMaskSelectors );
            else if (mcudisIt->second.docType=="refman")
                printMcuDocInfoWgetBat( sR, mcudisIt->second, docTypeShortMaskSelectors );
            else if (mcudisIt->second.docType=="errata")
                printMcuDocInfoWgetBat( sE, mcudisIt->second, docTypeShortMaskSelectors );
            else if (mcudisIt->second.docType=="appnote")
                printMcuDocInfoWgetBat( sA, mcudisIt->second, docTypeShortMaskSelectors );
            else if (mcudisIt->second.docType=="progman")
                printMcuDocInfoWgetBat( sM, mcudisIt->second, docTypeShortMaskSelectors );
        }
    }        

    return 0;
}


