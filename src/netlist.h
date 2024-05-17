#pragma once

//
#include "config.h"
//

#include "parse.h"
#include "rdlc-core/trims.h"
#include "rdlc-core/utils.h"
#include "rdlc-core/case.h"

#include "component.h"
#include "net.h"
#include "netlist_src_type.h"
#include "list_dump.h"


//
#include "netlist_types.h"


//-----------------------------------------------------------------------------
bool netlistParseComponent_ACCEL_ASCII( RoboconfOptions &rbcOpts, const expression_list_t &lst, ComponentInfo &componentInfo );
bool netlistParseNet_ACCEL_ASCII( const expression_list_t &lst, NetInfo &netInfo );
bool netlistParseCache( RoboconfOptions &rbcOpts, std::vector<std::string> lines, std::string &projectName, all_nets_map_type &allNets );

//-----------------------------------------------------------------------------
inline
bool netlistParse_EDIF( RoboconfOptions &rbcOpts, const expression_list_t &netlist, all_nets_map_type &allNets )
{
    using ListSimpleXPath::PathValues;
    using ListSimpleXPath::executeQuery;

    std::vector< expression_list_t* >   enumEdifQueryResults;
    
#if 0

    executeQuery( netlist, "edif/edifVersion", &enumEdifQueryResults, true );
    for( auto qr : enumEdifQueryResults)
    {
        expression_list_t &rList = *qr;
        LOG_MSG("data-dump-short")<<"--- Input short dump (edif/edifVersion test)\n";
        dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", rList );
    }


    enumEdifQueryResults.clear();
    
    executeQuery( netlist, "/edif/status/written", &enumEdifQueryResults, true );
    for( auto qr : enumEdifQueryResults)
    {
        expression_list_t &rList = *qr;
        LOG_MSG("data-dump-short")<<"--- Input short dump (/edif/status/written test)\n";
        dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", rList );
    }

    enumEdifQueryResults.clear();
    
    executeQuery( netlist, "//cellType|viewType", &enumEdifQueryResults, true );
    for( auto qr : enumEdifQueryResults)
    {
        expression_list_t &rList = *qr;
        LOG_MSG("data-dump-short")<<"--- Input short dump (//cellType|viewType test)\n";
        dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", rList );
    }

    enumEdifQueryResults.clear();
    
#endif

    // EDIF - https://www10.edacafe.com/book/ASIC/CH09/CH09.4.php
    // https://ru.scribd.com/document/17496353/ASIC-CH09
    // https://www.researchgate.net/publication/4328372_An_Approach_to_Graph_and_Netlist_Compression
    // http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.409.5571&rep=rep1&type=pdf
    // https://sourceforge.net/projects/gedif/

    /*
    (edif main_board_master_PrjPcb 
        (edifVersion ...)
        (edifLevel ...)
        (keywordMap ...)
        (status ...)
        (library ...)
        (library ...)
        (design ...)
    )
    */

    bool res = true;

    executeQuery( netlist, "/edif", &enumEdifQueryResults, caseIgnore );

    for( auto pqr : enumEdifQueryResults)
    {
        const expression_list_t            &edifList = *pqr;
        expression_list_t::const_iterator  edifIt    = edifList.begin();

        std::string expected;
        std::string found;
        std::vector< ExpressionParsingResultItem > readedVals;

        ExpressionParsingResult
        ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti:edif;V", edifIt, edifList.end()
                                           , readedVals, expected, found
                                           );
        ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_FALSE_OBJ(netlist, "EDIF input file");

        NetlistInfo netlistInfo;

        netlistInfo.projectName = readedVals[1].singleValue;
        //netlistInfo.name = generateNewName( allNets, netlistInfo.name );

        unsigned edifVerMajor = 2;


        // Looking for the version
        //edifQueryResults.clear();
        std::vector< expression_list_t* > queryResults;
        executeQuery( edifList, "edifVersion", &queryResults, caseIgnore );
        if (!queryResults.empty())
        {
            const expression_list_t &edifVersionList = *queryResults[0];
            expression_list_t::const_iterator  edifVerIt = edifVersionList.begin();

            ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti:edifVersion;V;V;V", edifVerIt, edifVersionList.end()
                                           , readedVals, expected, found
                                           );
            ROBOCONF_CHECK_PARSE_READ_LIST_EXPECTED_RET_FALSE_OBJ(netlist, "EDIF input file");

            edifVerMajor = (unsigned)parseIntegerTolerant(readedVals[1].singleValue);
        }

        // library/cell/view/interface
        // library/cell/view/contents/Instance
        // library/cell/view/contents/Net

        queryResults.clear();
        executeQuery( edifList, "library/cell/view/contents/Instance", &queryResults, caseIgnore );

        for( auto pqr : queryResults)
        {
            const expression_list_t            &instanceList = *pqr;
            expression_list_t::const_iterator  instanceIt    = instanceList.begin();

            LOG_MSG("data-dump-short")<<"--- Input short dump (library/cell/view/contents/Instance query res)\n";
            dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", instanceList );
        }

        queryResults.clear();
        executeQuery( edifList, "library/cell/view/contents/Net", &queryResults, caseIgnore );

        for( auto pqr : queryResults)
        {
            const expression_list_t            &netList = *pqr;
            expression_list_t::const_iterator  netIt    = edifList.begin();

            LOG_MSG("data-dump-short")<<"--- Input short dump (library/cell/view/contents/Net query res)\n";
            dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", netList );
        }
        
        //allNets[netlistInfo.name] = netlistInfo;
    }


#if 0

    expression_list_t::const_iterator netlistIt = netlist.begin();

    std::string expected;
    std::string found;

    std::vector< ExpressionParsingResultItem > readedVals;

    bool readFieldsRes = ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti:edif;V", netlistIt, netlist.end()
                                           , readedVals, expected, found
                                           );
    if (!readFieldsRes)
    {
        LOG_ERR_OBJ(netlist)<<"EDIF input file': expected "<<expected<<", but found "<<found<<"\n";
        return false; // ExpressionParsingResult::failed;
    }

    UMBA_ASSERT(!readedVals.empty());

    projectName = readedVals[1].singleValue;
    unsigned edifMajorVersion = 2;

    

    for(; netlistIt!=netlist.end(); ++netlistIt)
    {
        if (netlistIt->isText())
            continue;

        const expression_list_t &edifList = netlistIt->itemList;
        expression_list_t::const_iterator edifIt = edifList.begin();


        readFieldsRes = ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti:edifVersion:edifLevel:keywordMap:status:library:design;V?", edifIt, edifList.end()
                                           , readedVals, expected, found
                                           );
        if (!readFieldsRes)
        {
            LOG_ERR_OBJ(edifList)<<"EDIF input file: expected "<<expected<<", but found "<<found<<"\n";
            return false; // ExpressionParsingResult::failed;
        }

        UMBA_ASSERT(!readedVals.empty());

        if (readedVals[0].tokenId==0) // edifVersion
        {
            /*
            readFieldsRes = ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "V;V;V", it, lst.end()
                                               , readedVals, expected, found
                                               );
            if (!readFieldsRes)
            {
                LOG_ERR_OBJ(lst)<<"EDIF input file: expected "<<expected<<", but found "<<found<<"\n";
                return false; // ExpressionParsingResult::failed;
            }
            UMBA_ASSERT(readedVals.size()!=3);
            */

            if (readedVals.size()<2)
            {
                LOG_ERR_OBJ(edifList)<<"EDIF input file: expected version value\n";
                return false;
            }

            edifMajorVersion = (unsigned)parseIntegerTolerant(readedVals[1].singleValue);
        }
        else if (readedVals[0].tokenId==1) // edifLevel
        {
        }
        else if (readedVals[0].tokenId==2) // keywordMap
        {
        }
        else if (readedVals[0].tokenId==3) // status
        {
        }
        else if (readedVals[0].tokenId==4) // library
        {
            if (readedVals.size()<1)
            {
                LOG_ERR_OBJ(edifList)<<"EDIF input file: expected library name, but found nested list\n";
                return false; // ExpressionParsingResult::failed;
            }

            std::string libName = toUpper(readedVals[1].singleValue);

            LOG_MSG("data-dump-short")<<"--- Input short dump (library "<<libName<<")\n";
            dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", edifList);

            /*
            if (libName!="COMPONENT_LIB" && libName!="SHEET_LIB")
            {
                LOG_ERR_OBJ(lst)<<"EDIF input file': expected library COMPONENT_LIB/SHEET_LIB, but found library "<<libName<<"\n";
                return false; //  ExpressionParsingResult::failed;
            }
            */

            for(; edifIt!=edifList.end(); ++edifIt)
            {
                if (edifIt->isText())
                    continue;

                const expression_list_t &libSubList = edifIt->itemList;
                expression_list_t::const_iterator libSubIt = libSubList.begin();

                readFieldsRes = ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:cell;V?", libSubIt, libSubList.end()
                                                   , readedVals, expected, found
                                                   );
                if (!readFieldsRes)
                {
                    LOG_ERR_OBJ(libSubList)<<"EDIF input file: expected "<<expected<<", but found "<<found<<"\n";
                    return false;
                }
               
                UMBA_ASSERT(!readedVals.empty());
                UMBA_ASSERT(!(readedVals.size()==1 && readedVals[0].tokenId>=0));

                if (readedVals.size()==1 && readedVals[0].tokenId==-1)
                {
                    continue; // not cell
                }

                std::string cellName;
                if (readedVals.size()>1)
                    cellName = readedVals[1].singleValue;

                LOG_MSG("data-dump-short")<<"--- Input short dump (cell "<<cellName<<")\n";
                dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", libSubList);


            }


/*
            //const expression_list_t &libList = edifIt->itemList;
            //expression_list_t::const_iterator libIt = libList.begin();

            for(; libIt!=libList.end(); ++libIt)
            {
                if (libIt->isText())
                    continue;
                const expression_list_t &libSubList = libIt->itemList;
                expression_list_t::const_iterator libSubIt = libSubList.begin();

                readFieldsRes = ROBOCONF_PARSE_READ_LIST_BY_TEMPLATE( "Ti!:cell;V?", libSubIt, libSubList.end()
                                                   , readedVals, expected, found
                                                   );
                if (!readFieldsRes)
                {
                    LOG_ERR_OBJ(libSubList)<<"EDIF input file: expected "<<expected<<", but found "<<found<<"\n";
                    return false;
                }
               
                UMBA_ASSERT(!readedVals.empty());
                UMBA_ASSERT(!(readedVals.size()==1 && readedVals[0].tokenId>=0));

                if (readedVals.size()==1 && readedVals[0].tokenId==-1)
                {
                    continue; // not cell
                }

                std::string cellName;
                if (readedVals.size()>1)
                    cellName = readedVals[1].singleValue;

                if (!libSubIt->isList())
                {
                    LOG_ERR_OBJ(libSubList)<<"EDIF input file: expected cell nested list\n";
                    return false; // ExpressionParsingResult::failed;
                }

                const expression_list_t &cellList = libSubIt->itemList;
                expression_list_t::const_iterator cellIt = cellList.begin();

                LOG_MSG("data-dump-short")<<"--- Input short dump (cell "<<cellName<<")\n";
                dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", cellList);
                
            } // for(; libIt!=libList.end(); ++libIt)
*/
           

            /*
            (cell (rename TEST_1 "test$1") ...
            In this example the EDIF string contains the original name, test$1, and a new name, TEST_1 , is created as an EDIF identifier.

            (Instance A1
              (viewRef NetlistView
                (cellRef LMR16006
                  (LibraryRef COMPONENT_LIB)
                )
              )
              (Property Comment (String "LMR16006" ))
              (Property (rename Component_Kind "Component Kind") (String "Standard" ))
              (Property Description (String "Преобразователь напряжения" ))
              (Property Footprint (String "LMR16006" ))
              (Property (rename Ibis_Model "Ibis Model") (String "" ))
              (Property (rename Library_Name "Library Name") (String "PCH_Lib.SchLib" ))

              (Property Comment (String "OMRON XF2W-2415-1A" ))
              (Property (rename Component_Kind "Component Kind") (String "Standard" ))
              (Property Contact_1 (String "UART1_RX" ))
              (Property Contact_2 (String "UART1_TX" ))
              (Property Contact_3 (String "UART2_RX" ))
              (Property Contact_4 (String "UART2_TX" ))
              (Property Contact_5 (String "GND" ))


            (Net NetDA8_3
              (Joined 
                  (PortRef &3 (InstanceRef DA8))
            (Net (rename &1 "1")
              (Joined 
                  (PortRef &1 (InstanceRef X17))
              )
            )
            (Net (rename PLUS14_4_ "+14,4В")
              (Joined 
                  (PortRef &1 (InstanceRef A1))
            */

        }
        else if (readedVals[0].tokenId==5) // design
        {
            if (readedVals.size()>1)
                netlistInfo.name = readedVals[1].singleValue;
        }

    }

#endif


    return true;
}

//-----------------------------------------------------------------------------
inline
bool netlistParse_ACCEL_ASCII( RoboconfOptions &rbcOpts, const expression_list_t &netlist, NetlistInfo &netlistInfo, std::string &projectName )
{
    expression_list_t::const_iterator nlIt = netlist.begin();

    std::string itemText;
    if (!readListTextItem( netlist, nlIt, itemText ))
       return false; // failed to get list type

    if (itemText!="netlist") // is a netlist?
       return false;

    if (!readListTextItem( netlist, nlIt, netlistInfo.name ))
       return false; // failed to get netlist name

    unquoteTrimString( netlistInfo.name );


    for( ; nlIt != netlist.end(); ++nlIt)
    {
        if (!nlIt->isList())
        {
            LOG_ERR_OBJ(*nlIt)<<"ListItem: not a sublist (text: '"<<nlIt->itemText<<"'), something wrong!!!\n";
            continue;
        }

        std::string sublistHeader                = nlIt->getHeadText();
        const expression_list_t &subList = nlIt->itemList;
        if (sublistHeader=="globalAttrs")
        {
            continue;
        }
        else if (sublistHeader=="compInst")
        {
            ComponentInfo componentInfo;
            if (netlistParseComponent_ACCEL_ASCII(rbcOpts, subList, componentInfo ))
            {
                if (componentInfo.componentClass== ComponentClass::cc_RESISTOR)
                    componentInfo.package.clear();
                netlistInfo.components[componentInfo.designator] = componentInfo;
            }
        }
        else if (sublistHeader=="net")
        {
            NetInfo netInfo;
            if (netlistParseNet_ACCEL_ASCII( subList, netInfo ))
            {
                netlistInfo.nets[netInfo.name] = netInfo;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
inline
bool netlistParseComponent_ACCEL_ASCII( RoboconfOptions &rbcOpts, const expression_list_t &lst, ComponentInfo &componentInfo )
{
    expression_list_t::const_iterator it = lst.begin();

    std::string itemText;
    if (!readListTextItem( lst, it, itemText ))
       return false; // failed to get list type

    if (itemText!="compInst")
       return false;

    if (!readListTextItem( lst, it, componentInfo.designator ))
       return false;

    componentInfo.designator = toUpper(componentInfo.designator);

    unquoteTrimString( componentInfo.designator );

    //std::map< std::string, std::string > info;
    NetlistComponentAttributesMap info;

    info["designator"] = componentInfo.designator;

    for(; it!=lst.end(); ++it)
    {
        if (it->isText())
            continue;

        std::vector< std::string > v;
        if (readListToVector( it->itemList, v, true ) < 2)
            continue;
        //TODO:
        v[0] = toLower(v[0]);
        if (v[0]=="compref" || v[0]=="originalname")
        {
            info[v[0]] = v[1];
        }
        else if (v[0]=="attr")
        {
            if (v.size()>2)
            {
                unquoteTrimString(v[1]);
                unquoteTrimString(v[2]);
                v[1] = toLower(v[1]);

                if (startsWithAndStrip(v[1], "contact"))
                {
                    componentInfo.addPin( v[1], v[2] );
                }
                else
                {
                    std::string attrName = toLower(v[1]);

                    // Корпус - D0 9A D0 BE D1 80 D0 BF D1 83 D1 81
                    // корпус - D0 BA D0 BE D1 80 D0 BF D1 83 D1 81
                    // КОРПУС - D0 9A D0 9E D0 A0 D0 9F D0 A3 D0 A1

                    // Фикс для русскоязычного имени атрибута корпуса
                    // https://docs.microsoft.com/en-us/cpp/cpp/string-and-character-literals-cpp?view=msvc-170

                    if ( attrName=="\xD0\x9A\xD0\xBE\xD1\x80\xD0\xBF\xD1\x83\xD1\x81"
                      || attrName=="\xD0\xBA\xD0\xBE\xD1\x80\xD0\xBF\xD1\x83\xD1\x81"
                      || attrName=="\xD0\x9A\xD0\x9E\xD0\xA0\xD0\x9F\xD0\xA3\xD0\xA1"
                       )
                         attrName = "package";

                    info[std::string("attr") + attrName] = v[2];
                }

                // (attr "Корпус" "UFQFPN-48" (textStyleRef "(Default)"))
            }
        }
    }

    componentInfo.detectComponentClass( rbcOpts, info );

    if (componentInfo.componentClass == ComponentClass::cc_UNKNOWN)
        LOG_MSG("cls-chk-not-at-all")<<"component '"<<componentInfo.designator<<"': component class not detected\n";

    //std::map< std::string, std::string >::const_iterator infoIt = info.begin();
    NetlistComponentAttributesMap::const_iterator infoIt = info.begin();
    for(; infoIt != info.end(); ++infoIt)
    {
        if (infoIt->first=="attrsheet")
        {
            componentInfo.sheetName = infoIt->second;
        }    
        else if (infoIt->first=="attrpackage" || infoIt->first=="attrcase")
        {
            if (componentInfo.package.empty() || !rbcOpts.packagesDb.isKnownPackage(componentInfo.package))
                componentInfo.package = rbcOpts.packagesDb.getCanonicalPackageName(infoIt->second);
        }
        else if (infoIt->first=="attrdescription")
        {
            componentInfo.description = infoIt->second;
        }
        else if (infoIt->first=="compref" || 
                 infoIt->first=="originalname" || 
                 infoIt->first=="attrname" || 
                 infoIt->first=="attrdevice" || 
                 infoIt->first=="attrpart number"
                )
        {
            if (componentInfo.typeName.size() < infoIt->second.size())
                componentInfo.typeName = infoIt->second;
        }
        
    }

    return true;
}

//-----------------------------------------------------------------------------
inline
bool netlistParseNet_ACCEL_ASCII( const expression_list_t &lst, NetInfo &netInfo )
{
    expression_list_t::const_iterator it = lst.begin();

    std::string itemText;
    if (!readListTextItem( lst, it, itemText ))
       return false; // failed to get list type

    if (itemText!="net")
       return false;

    if (!readListTextItem( lst, it, netInfo.name ))
       return false;

    unquoteTrimString( netInfo.name );

    for(; it!=lst.end(); ++it)
    {
        if (!it->isList())
            continue;

        //const std::list<ExpressionItem> &nodeList = item->listItem;
        std::vector< std::string > v;
        if (readListToVector( it->itemList, v ) < 3)
            continue;

        if (v[0]!="node")
            continue;

        NetNode node;
        node.targetDesignator     = v[1];
        node.targetDesignatorFull = v[1] + std::string(".") + v[2];
        //node.targetPin = (unsigned)parseIntegerTolerant( v[2], 10 );
        node.targetPin = v[2];

        netInfo.nodes[node.targetDesignatorFull] = node;
    
    }

    return true;
}

//-----------------------------------------------------------------------------
//#define NETLISTREAD_CHECK_FORCED_TYPE( t )
//            (netlistType == NetlistSrcType::netlistSrcType_Unknown && netlistType == NetlistSrcType:: t )
inline
bool netlistRead( RoboconfOptions &rbcOpts
                , FileSet::file_id_t fileNo
                , std::istream &in
                , std::string &projectName
                , all_nets_map_type &allNets
                , NetlistSrcType netlistType = NetlistSrcType::netlistSrcType_Unknown
                )
{

    std::vector< std::string > lines;

    std::string allText = readFileEncoded /* Safe */ ( in /*, std::string srcEnc = std::string(), std::string targetEnc = "UTF-8", std::string httpHint = std::string(), std::string metaHint = std::string() */ );
    std::string allTextTrimmed = allText; ltrim(allTextTrimmed);
    bool bForcedType = true;

    //std::vector< std::string > lines;

    std::string prefix;

    if (netlistType == NetlistSrcType::netlistSrcType_Unknown)
    {
        bForcedType = false;

        if ( startsWith( allTextTrimmed, prefix="ACCEL_ASCII" ) /*|| startsWith( allTextTrimmed, "(--" )*/ )
        {
            netlistType = NetlistSrcType::netlistSrcType_ACCEL_ASCII;
            //auto lfPos = allTextTrimmed.find_first_of( '\n' );
            //projectName = std::string( allTextTrimmed, 0, lfPos);
            //trim(projectName);
            //allTextTrimmed.erase( 0, lfPos );
            //allText = allTextTrimmed;
        }
        else if ( startsWith( allTextTrimmed, prefix="(asciiHeader" ) || startsWith( allTextTrimmed, prefix="(netlist" ) /*|| startsWith( allTextTrimmed, "(--" )*/ )
        {
            netlistType = NetlistSrcType::netlistSrcType_ACCEL_ASCII;
        }
        else if (startsWith( allTextTrimmed, prefix="(edif"))
        {
            // data/nets/main_board_master_MCU_Calay.NET.EDF
            netlistType = NetlistSrcType::netlistSrcType_EDIF;
        }
        else if (startsWith( allTextTrimmed, prefix="( {OrCAD PCB II Netlist Format}"))
        {
            // data/nets/main_board_master_MCU_OrCAD-PCB2.NET
            netlistType = NetlistSrcType::netlistSrcType_ORCAD_PCB_II;
        }
        else if (startsWith( allTextTrimmed, prefix="BOARD = PROTEL.PCB;"))
        {
            // data/nets/main_board_master_MCU_PCAD_nlt.NET
            netlistType = NetlistSrcType::netlistSrcType_PROTEL_PCB;
        }
        else if (startsWith( allTextTrimmed, prefix="PROTEL NETLIST 2.0"))
        {
            // data/nets/main_board_master_MCU_Protel2.NET
            netlistType = NetlistSrcType::netlistSrcType_PROTEL_NETLIST_20;
        }
        else if (startsWith( allTextTrimmed, prefix="PARTS LIST"))
        {
            // data/nets/main_board_master_MCU_SciCards.NET
            netlistType = NetlistSrcType::netlistSrcType_SCI_CARDS;
        }
        else if (startsWith( allTextTrimmed, prefix="["))
        {
            // data/nets/main_board_master_MCU_Tango.NET
            netlistType = NetlistSrcType::netlistSrcType_TANGO;
        }
        else if (startsWith( allTextTrimmed, prefix="$PACKAGES"))
        {
            // data/nets/main_board_master_MCU_Telesis.NET
            netlistType = NetlistSrcType::netlistSrcType_TELESIS;
        }
        else if (startsWith( allTextTrimmed, prefix="Wire List"))
        {
            // data/nets/main_board_master_MCU_WireList.NET
            netlistType = NetlistSrcType::netlistSrcType_WIRE_LIST;
        }
        else if (startsWith( allTextTrimmed, prefix="(export"))
        {
            // \stm_shchematics\WolfieMouse\hardware\wolfiemouse\micromouse.net
            netlistType = NetlistSrcType::netlistSrcType_KICAD_EXPORT;
        }
        else if (startsWith( allTextTrimmed, prefix="Partlist"))
        {
            // stm_shchematics\STM32F030F4P6-breakout-board\eagle\stm32f030f4p6_breakout.net  
            // stm_shchematics\STM32F030F4P6-breakout-board\eagle\stm32f030f4p6_breakout.parts
            netlistType = NetlistSrcType::netlistSrcType_EAGLE_PARTLIST; // Need to glue part and nets files
        }
        else if (startsWith( allTextTrimmed, prefix="#!roboconf netlist "))
        {
            netlistType = NetlistSrcType::netlistSrcType_Cache;
            //splitToVector( allText, lines, '\n' );
            //rtrim(lines);
        }
        /*
        else if (startsWith( allTextTrimmed, ""))
        {
            // data/nets/
            netlistType = NetlistSrcType::netlistSrcType_;
        }
        */
        //else if (startsWith( allTextTrimmed, "(netlist"))
        //{
        //    // stm_shchematics\dap42-hardware\dap42-hardware-accel.net
        //    //netlistType = NetlistSrcType::netlistSrcType_;
        //}

    }


    if (netlistType == NetlistSrcType::netlistSrcType_Unknown)
    {
        //if (bForcedType)
        size_t lineNo = 1;
        LOG_ERR<<"unknown net list type. Please force net list type with '--netlist-type' option.";
        return false;
    }


    if (netlistType == NetlistSrcType::netlistSrcType_ACCEL_ASCII)
    {
        if ( startsWithAndStrip( allTextTrimmed, prefix ) /*|| startsWith( allTextTrimmed, "(--" )*/ )
        {
            auto lfPos = allTextTrimmed.find_first_of( '\n' );
            projectName = std::string( allTextTrimmed, 0, lfPos);
            unquoteTrimString(projectName);
            //trim(projectName);
            allTextTrimmed.erase( 0, lfPos );
            allText = allTextTrimmed;
        }
    }
    else if (netlistType == NetlistSrcType::netlistSrcType_Cache)
    {
        if (startsWith /* AndStrip */ ( allTextTrimmed, prefix ))
        {
            netlistType = NetlistSrcType::netlistSrcType_Cache;
            splitToVector( allText, lines, '\n' );
            rtrim(lines);
        }
    }


    if (bForcedType)
        LOG_MSG("netlist-type")<<"netlist type: "<<getNetlistSrcTypeName(netlistType)<<"\n";
    else
        LOG_MSG("netlist-type")<<"detected netlist type: "<<getNetlistSrcTypeName(netlistType)<<"\n";

    // inputFilename
    // rbcOpts.getFileName(fileNo)

    if (!rbcOpts.readComponentClassDetectionRules( rbcOpts.getFileName(fileNo), rbcOpts.rulesPaths, getNetlistSrcTypeName(netlistType)) )
        return false;


    switch(netlistType)
       {
	    case NetlistSrcType::netlistSrcType_ACCEL_ASCII:
             {
                 expression_list_t netList;
				 size_t lineNo = 0;
                 if (!readList(fileNo, lineNo, allText, netList ))
                 {
                     LOG_ERR<<"failed to parse netlist\n";
                     return false;
                 }

                 removeCommentExpressionLists( netList );

                 netList.updateParentPtr(0);

                 {
                     //auto &sv = 
                     LOG_MSG("data-dump-short")<<"--- Input short dump\n";
                     dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", netList );
                
                     //auto &sv = LOG_MSG("conf-dump-rules")<<"--- Rules dump (Common)"; // "\nA"
                     //dumpExpressionList( rbcOpts, 0, sv, "conf-dump-rules", processingRules );
                 }

                 expression_list_t::const_iterator nlIt = netList.begin();
                 for(; nlIt != netList.end(); ++nlIt)
                 {
                     if (!nlIt->itemText.empty())
                     {
                     }
                     else
                     {
                         if (nlIt->getHeadText()=="netlist")
                         {
                             NetlistInfo netlistInfo;
                             if (netlistParse_ACCEL_ASCII(rbcOpts, nlIt->itemList, netlistInfo, projectName ))
                             {
                                 netlistInfo.name = generateNewName( allNets, netlistInfo.name );
                                 netlistInfo.projectName = projectName;
                                 allNets[netlistInfo.name] = netlistInfo;
                             }
                         }
                     }
                 } // for(; nlIt != netList.end(); ++nlIt)
                 return true;
             }

        case NetlistSrcType::netlistSrcType_EDIF:
             {
                 expression_list_t netList;
				 size_t lineNo = 0;
                 if (!readList(fileNo, lineNo, allText, netList ))
                 {
                     LOG_ERR<<"failed to parse netlist\n";
                     return false;
                 }

                 removeCommentExpressionLists( netList );

                 netList.updateParentPtr(0);

                 {
                     //auto &sv = 
                     LOG_MSG("data-dump-short")<<"--- Input short dump\n";
                     dumpExpressionListSimpleShort( rbcOpts, "data-dump-short", netList );
                
                     //auto &sv = LOG_MSG("conf-dump-rules")<<"--- Rules dump (Common)"; // "\nA"
                     //dumpExpressionList( rbcOpts, 0, sv, "conf-dump-rules", processingRules );
                 }

                 if (netlistParse_EDIF(rbcOpts, netList /* nlIt->itemList */ , allNets ))
                 {
                 }
                 else
                 {
                     return false;
                 }

                 return true;
             }

		case NetlistSrcType::netlistSrcType_Cache:
             {
                 return netlistParseCache(rbcOpts, lines, projectName, allNets );
             }

        default: return false;
       }

    return true;

}

//-----------------------------------------------------------------------------
inline
bool netlistWritetCache( std::ostream &os, const std::string &projectName, const all_nets_map_type &allNets )
{
    os<<"#!roboconf netlist --help \""<<projectName<<"\"\n";
    for( auto netlistPair : allNets )
    {
        const auto &netlist = netlistPair.second;

        os<<"netlist \""<<netlist.name<<"\"\n";

        for( auto componentPair : netlist.components )
        {
            auto & component = componentPair.second;
            os<<"component \""<<component.designator<<"\"\n";
            if (!component.package.empty())
            os<<"    package \""<<component.package<<"\"\n";
            os<<"    type    \""<<component.typeName<<"\"\n";
            os<<"    class   \""<<component.getComponentClassString()<<"\"\n";
            if (component.assembly)
                os<<"    assembly \""<<component.assembly<<"\"\n";
            os<<"    sheet   \""<<component.sheetName<<"\"\n";

            for( auto pinPair : component.pins )
            {
                auto& pin = pinPair.second;
                os<<"    pin   \""<<pin.pinNo<<"\"";
                for( auto fn : pin.pinFunctions )
                {
                    os<<" \""<<fn<<"\"";
                }
                os<<"\n";
            }
        }

        for( auto net : netlist.nets )
        {
            os<<"net \""<<net.second.name<<"\"\n";
            for( auto node : net.second.nodes )
            {
                os<<"    node \""<<node.second.targetDesignator<<"\" \""<<node.second.targetPin<<"\"\n";
            }
        }

    }

    return true;
}

inline
bool netlistParseCache(RoboconfOptions &rbcOpts, std::vector<std::string> lines, std::string &projectName, all_nets_map_type &allNets )
{
    //std::string    projectName;
    NetlistInfo    curNetlist;
    ComponentInfo  curComponent;
    NetInfo        curNet;

    auto finalizeNetlist   = [ &curNetlist, &allNets, &projectName]
                             {
                                 if (curNetlist.valid())
                                 {
                                     curNetlist.name = generateNewName( allNets, curNetlist.name );
                                     curNetlist.projectName = projectName;
                                     allNets[curNetlist.name] = curNetlist;
                                 }
                                 curNetlist = NetlistInfo();
                             };

    auto finalizeComponent = [ &curNetlist, &curComponent ]
                             {
                                 if (curComponent.valid())
                                     curNetlist.components[ curComponent.designator ] = curComponent;
                                 curComponent = ComponentInfo();
                             };

    auto finalizeNet       = [ &curNetlist, &curNet ]
                             {
                                 if (curNet.valid())
                                     curNetlist.nets[ curNet.name ] = curNet;
                                 curNet = NetInfo();
                             };

    unsigned lineNo = 0;

    for( auto line : lines )
    {
        lineNo++;

        trim(line);

        if (startsWithAndStrip( line, "#!roboconf netlist"))
        {
            trim(line);
            startsWithAndStrip( line, "--help");
            trim(line);
            unquoteTrimString( line );
            projectName = line;
            continue;
        }

        if ( startsWith( line, "#") || startsWith( line, ";") || startsWith(line, "//"))
        {
            continue;
        }

        if ( startsWithAndStrip( line, "netlist") )
        {
            finalizeComponent();
            finalizeNet();
            finalizeNetlist();

            ltrim(line);
            unquoteTrimString( line );
            curNetlist.name = line;
            //curNetlist.projectName = projectName;
        }
        else if ( startsWithAndStrip( line, "component") )
        {
            finalizeComponent();
            finalizeNet();

            ltrim(line);
            unquoteTrimString( line );
            curComponent.designator = line;
        }
        else if ( startsWithAndStrip( line, "type") )
        {
            ltrim(line);
            unquoteTrimString( line );
            curComponent.typeName = rbcOpts.packagesDb.getCanonicalPackageName(line);
        }
        else if ( startsWithAndStrip( line, "package") )
        {
            ltrim(line);
            unquoteTrimString( line );
            curComponent.package = rbcOpts.packagesDb.getCanonicalPackageName(line); // toUpper(filterForValidName( line, ComponentPackageNameCharValidator() ));
        }
        else if ( startsWithAndStrip( line, "class") )
        {
            ltrim(line);
            unquoteTrimString( line );

            curComponent.componentClass = getComponentClassId( line );

        }
        else if ( startsWithAndStrip( line, "assembly") )
        {
            ltrim(line);
            unquoteTrimString( line );
            curComponent.assembly = (unsigned)parseIntegerTolerant( line, 10  /* ss */ );
        }
        else if ( startsWithAndStrip( line, "sheet") )
        {
            ltrim(line);
            unquoteTrimString( line );
            curComponent.sheetName = line;
        }
        else if ( startsWithAndStrip( line, "pin") )
        {
            ltrim(line);
            std::vector<std::string> vec;
            splitToVector( line, vec, ' ' );
            auto it = vec.begin();
            if (it==vec.end())
               continue;

            //std::string pinStr = *it;
            unquoteTrimString( *it );
            ComponentPinInfo pinInfo;
            //pinInfo.pinNo = (unsigned)parseIntegerTolerant(*it, 10);
            pinInfo.pinNo = *it;

            ++it;

            for(; it!=vec.end(); ++it)
            {
                unquoteTrimString( *it );
                pinInfo.pinFunctions.insert(*it);
            }

            //makeUniqueVector(pinInfo.pinFunctions);

            curComponent.pins[pinInfo.pinNo] = pinInfo;

        }
        else if ( startsWithAndStrip( line, "net") )
        {
            finalizeNet();
            finalizeComponent();

            ltrim(line);
            unquoteTrimString( line );
            curNet.name = line;
        }
        else if ( startsWithAndStrip( line, "node") )
        {
            ltrim(line);
            std::string designatorStr, pinStr;

            splitToPair( line, designatorStr, pinStr, " " );
            unquoteTrimString( designatorStr );
            unquoteTrimString( pinStr );

            if (pinStr.empty())
            {
                splitToPairFromEnd( designatorStr, designatorStr, pinStr, "." );
            }

            if (pinStr.empty())
                continue;

            NetNode netNode;
            //netNode.targetPin = (unsigned)parseIntegerTolerant( pinStr, 10  /* ss */ );
            netNode.targetPin = pinStr;

            netNode.targetDesignator     = designatorStr;
            netNode.targetDesignatorFull = designatorStr + std::string(".") + pinStr;

            curNet.nodes[netNode.targetDesignatorFull] = netNode;
        }

    }

    finalizeComponent();
    finalizeNet();
    finalizeNetlist();

    return true;
}


//----------------------------------------------------------------------------
inline
std::map<std::string, NetlistInfo> makeAllNetsOrderedMap(const all_nets_map_type &allNets)
{
    std::map<std::string, NetlistInfo> res;

    for(const auto &netInfoPair : allNets)
    {
        res[netInfoPair.first] = netInfoPair.second;
    }

    return res;
}


