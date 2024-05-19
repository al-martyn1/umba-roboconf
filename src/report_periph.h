#pragma once

#include "reports.h"
#include "report_html_base.h"
//#include "datasheet.h"





struct PeriphReportGenerator : public ReportHtmlBase // IReportGenerator
{
    virtual
    std::string getDefaultFileExt() override
    {
        return "htm";
    }

    virtual
    std::string getReportNameDescription() override
    {
        return std::string("Peripherial report - display MCU to peripherial connections grouped by it's functionality");
    }

    virtual
    std::vector<std::string> getReportCommandLineNames() override
    {
        std::vector<std::string> names;
        names.emplace_back( "periph" );
        return names;
    }


    // https://www.st.com/en/microcontrollers/stm32f303vd.html#design-scroll
    virtual
    //bool generateReport( std::ostream &os, std:: map<std::string, NetlistInfo> &nets, std::map<std::string, std::vector< ComponentInfo > > libComponents, const expression_list_t &processingRules, const ConnectionBuildingOptions &opts ) override
    bool generateReport( RoboconfOptions &rbcOpts
                       , std::ostream &os
                       , std::map<std::string, NetlistInfo> &nets
                       , std::vector< ComponentInfo > libComponents
                       , const expression_list_t &processingRules
                       , const ConnectionBuildingOptions &opts
                       , size_t &processedMcus
                       ) override
    {
        std::string docTitle = "Peripherials - Roboconf";

        if (!nets.empty())
        {
            auto first = nets.begin();

            if (!first->second.projectName.empty())
                docTitle = first->second.projectName + std::string(" - ") + docTitle;
        }

        os<<htmlDocType();
        os<<"<html lang=\"en\">\n<head>\n<title>"<< docTitle << "</title>\n";
        //os<<;        
        os<<"</head>\n";
        os<<htmlStyle();

        for( const auto& nlIt : nets )
        {
            NetlistInfo netlistInfo = nlIt.second; 
            os<<"<h1>"<<netlistInfo.projectName<<" - "<<netlistInfo.name<<"</h1>\n";

            std::vector< std::string > mcuDesignators;

            findStartConnectionsDesignators( netlistInfo, "MCU" /* purpose */ , mcuDesignators );

            //size_t processedMcus = 0;

            for( auto curMcuD : mcuDesignators )
            {
                auto componentKV = netlistInfo.components.find(curMcuD);
                if (componentKV == netlistInfo.components.end())
                    continue;

                processedMcus++;

                os<<"<h2>MCU "<<curMcuD<<" "<<componentKV->second.typeName<<" peripherals</h2>\n";

                {
                    std::string datasheetLink;
                    std::string refmanLink;
                    std::string errataLink;
                    findDatasheet(rbcOpts, componentKV->second.datasheet, datasheetLink  );
                    findDatasheet(rbcOpts, componentKV->second.refman   , refmanLink     );
                    findDatasheet(rbcOpts, componentKV->second.errata   , errataLink     );

                    std::string iconLink;
                    findDatasheet(rbcOpts, "datasheet.gif", iconLink, true );

                    std::string target = "_blank";

                    if (!datasheetLink.empty())
                        os<<"<div>Datasheet: "<<makeIconLink( datasheetLink, iconLink, "Datasheet", "Datasheet", target )<<"</div>\n";
                    
                    if (!refmanLink.empty())
                        os<<"<div>Reference manual: "<<makeIconLink( refmanLink, iconLink, "Reference manual", "Reference manual", target )<<"<div>\n";
                    
                    if (!errataLink.empty())
                        os<<"<div>Errata: "<<makeIconLink( errataLink, iconLink, "Errata", "Errata", target )<<"<div>\n";

                }


                std::vector<Connection> connectionList;
                connectionsListBuild( rbcOpts, opts, netlistInfo, curMcuD, connectionList );

                if (!processConnectionModifyRules( rbcOpts, connectionList, processingRules ))
                    return false;

                connectionsListRemoveMcuDuplicates( connectionList );


                std::vector< ConnectionsGroup > connGroups;
                splitConnectionsToGroupsByTarget( rbcOpts, connGroups, connectionList );


                if (!connectionsDetectInterfaces( rbcOpts, connGroups, processingRules, operateVerbose ))
                    return false;

                //groupConnectionsBySheet(connectionList);

                os<<"<table border=\"0\" cellspacing=\"2\" cellpadding=\"5\" class=\"w95p\">\n";
                os<<"<thead><tr><th class=\"w3p\">Source Pin</th>"              /* 1 */
                                "<th class=\"w20p\">Net</th>"                    /* 2 */
                                "<th class=\"w30p\">Pin Functions</th>"          /* 3 */
                                /*"<th class=\"w3p\">Pass through</th>"*/           /* 4 */
                                "<th class=\"w3p\">Target</th>"                 /* 5 */
                                "<th class=\"w3p\">Target Pin Function</th>"    /* 6 */
                                "<th class=\"w3p\">Unit Class</th>"             /* 7 */
                                "<th class=\"w10p\">Unit Purpose</th>"           /* 8 */
                                /*"<th class=\"w10p\">Sheet</th>"*/                  /* 9 */
                                "<th class=\"w10p\">Unit Type</th>"              /* 10 */
                                "<th class=\"w30p\">Unit Description</th>"       /* 11 */
                                /* << "<th></th>" */
                                "</tr></thead>\n";
                
                os<<"<tbody>\n";



                for( auto& connGrp : connGroups )
                {
                    os<<"<tr>";
                    os<<"<th colspan=\"3\">"<< (connGrp.groupTitle.empty() ? connGrp.groupDesignator : connGrp.groupTitle) <<"</th>";  // 1
                    os<<"<tr>";

                    for( auto& conn : connGrp.connections )
                    {
                        os<<"<tr>";
                        os<<"<td>"<<conn.srcPinDesignator<<"<br/>("<<conn.interfaceType<<":"<<conn.interfaceLineType<<")"
                                  <<"</td>";  // 1
                   
                        os<<"<td>"<<conn.srcNetName;
                        std::string srcNetNameModified = conn.processedStrings["MCUNET"];
                        if (conn.srcNetName!=srcNetNameModified)
                        {
                            os<<"<br/>"<<srcNetNameModified;
                        }
                        os<<"</td>"; // 2
                   
                        os<<"<td>"; // 3
                        for( auto pfn : conn.srcPinInfo.pinFunctions)
                            os<<pfn<<" ";
                        os<<"</td>";
                        
                        /*
                        os<<"<td>"; // 4
                        for( auto pld : conn.payloads)
                            os<<pld<<" ";
                        os<<"</td>";
                        */
                   
                        if (!conn.isConnected())
                        {
                            os<<"<td class=\"warning\">NC</td><td></td><td></td><td></td><td></td><td></td><td></td>";
                        }
                        else if (!conn.netStop)
                        {
                            os<<"<td>"<<conn.dstPinDesignator
                                      <<"<br/>"
                                      <<"("<<conn.dstComponentInfo.sheetName<<")";

                            for( auto extraConn : conn.extraDestinations )
                            {
                                os<<"<br/>"<<extraConn.dstPinDesignator<<"<br/>("<<extraConn.dstComponentInfo.sheetName<<")";
                            }
                            os<<"</td>"; // 5 Pin or Net name


                            os<<"<td>"; // 6
                            for( auto pfn : conn.dstPinInfo.pinFunctions)
                                os<<pfn<<" ";
                            os<<"</td>";
                   
                   
                            os<<"<td>"<<conn.dstComponentInfo.getComponentClassString();
                            std::string dstComponentClassStrModified = conn.processedStrings["UNITCLASS"];
                            if (conn.dstComponentInfo.getComponentClassString() !=dstComponentClassStrModified)
                            {
                                os<<"<br/>"<<dstComponentClassStrModified;
                            }
                            os<<"</td>"; // 7
                   
                   
                            os<<"<td>"<<conn.dstComponentInfo.purpose;
                            std::string dstComponentPurposeModified = conn.processedStrings["UNITPURPOSE"];
                            if (conn.dstComponentInfo.purpose!=dstComponentPurposeModified)
                            {
                                os<<"<br/>"<<dstComponentPurposeModified;
                            }                        
                            os<<"</td>"; // 8
                   
                            //os<<"<td>"<<conn.dstComponentInfo.sheetName<<"</td>";  // 9
                            os<<"<td>"<<conn.dstComponentInfo.typeName;  // 10
                   
                            std::string                datasheet;
                            std::string                refman;
                            std::string                errata;
                   
                            std::string datasheetLink;
                            std::string refmanLink;
                            std::string errataLink;
                            findDatasheet(rbcOpts, conn.dstComponentInfo.datasheet, datasheetLink  );
                            findDatasheet(rbcOpts, conn.dstComponentInfo.refman   , refmanLink     );
                            findDatasheet(rbcOpts, conn.dstComponentInfo.errata   , errataLink     );
                   
                            std::string iconLink;
                            findDatasheet(rbcOpts, "datasheet.gif", iconLink, true );
                   
                            std::string target = "_blank";
                   
                            if (!datasheetLink.empty())
                                os<<makeIconLink( datasheetLink, iconLink, "Datasheet", "Datasheet", target );
                           
                            if (!refmanLink.empty())
                                os<<makeIconLink( refmanLink, iconLink, "Reference manual", "Reference manual", target );
                           
                            if (!errataLink.empty())
                                os<<makeIconLink( errataLink, iconLink, "Errata", "Errata", target );
                   
                            std::string dstComponentTypeModified = conn.processedStrings["UNITTYPE"];
                            if (conn.dstComponentInfo.typeName!=dstComponentTypeModified)
                            {
                                os<<"<br/>"<<dstComponentTypeModified;
                            }                        
                   
                   
                            os << "</td>";
                   
                            os<<"<td>"<<conn.dstComponentInfo.description<<"</td>";
                        }
                        else
                        {
                            os<<"<td>"<<conn.dstPinDesignator<<"</td>"; // 5 Pin or Net name
                            os<<"<td></td>"; // 6
                            os<<"<td>NET</td>"; // 7
                            os<<"<td></td>"; // 8
                            os<<"<td></td>"; // 9
                            os<<"<td></td>"; // 10
                            os<<"<td class=\"w30p\">"<<conn.dstComponentInfo.description<<"</td>"; // 11
                            //os<<"<td>"<<conn.<<"</td>";
                        }
                   
                        os<<"</tr>\n";
                    } // for( auto& conn : connectionList )

                } // for( auto& connGrp : connGroups )

            } // for( auto curMcuD : mcuDesignators )

            if (!processedMcus)
            {
                os<<"<div class=\"warning\">No MCU found in this netlist</div>\n";
            }

        } // for( const auto& nlIt : nets )


        os<<"<body>\n";
        os<<"</body>\n</html>\n";

        return true;

    }
}; // struct PeriphReportGenerator

