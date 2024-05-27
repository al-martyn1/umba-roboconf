#pragma once

#include "reports.h"
#include "report_html_base.h"
//#include "datasheet.h"





struct McuReportGenerator : public ReportHtmlBase // IReportGenerator
{
    virtual
    std::string getDefaultFileExt() override
    {
        return "htm";
    }

    virtual
    std::string getReportNameDescription() override
    {
        return std::string("MCU report - display MCU connections");
    }

    virtual
    std::vector<std::string> getReportCommandLineNames() override
    {
        std::vector<std::string> names;
        names.emplace_back( "mcu" );
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
        std::string docTitle = "MCU connections - Roboconf";

        if (!nets.empty())
        {
            auto first = nets.begin();

            if (!first->second.projectName.empty())
                docTitle = first->second.projectName + std::string(" - ") + docTitle;
        }

        os<<htmlDocType();
        os<<"<html>\n<head>\n<title>"<< docTitle << "</title>\n";
        //os<<;        
        os<<"</head>\n";
        os<<htmlStyle();

        for( const auto& nlIt : nets )
        {
            NetlistInfo netlistInfo = nlIt.second; 
            os<<"<h1>"<<netlistInfo.projectName<<" - "<<netlistInfo.name<<"</h1>\n";

            std::vector< std::string > mcuDesignators;

            findStartConnectionsDesignators( netlistInfo, "MCU" /* purpose */ , mcuDesignators );

            std::sort(mcuDesignators.begin(), mcuDesignators.end(), designatorPinNamesLess );

            for( auto curMcuD : mcuDesignators )
            {
                auto componentKV = netlistInfo.components.find(curMcuD);
                if (componentKV == netlistInfo.components.end())
                    continue;

                processedMcus++;

                os<<"<h2>MCU "<<curMcuD<<" "<<componentKV->second.typeName<<" connections</h2>\n";

                {
                    std::string datasheetLink;
                    std::string refmanLink;
                    std::string errataLink;
                    findDatasheet( rbcOpts, componentKV->second.datasheet, datasheetLink  );
                    findDatasheet( rbcOpts, componentKV->second.refman   , refmanLink     );
                    findDatasheet( rbcOpts, componentKV->second.errata   , errataLink     );

                    //std::string iconLink;
                    //findDatasheet(rbcOpts, "datasheet.gif", iconLink, true );

                    std::string target = "_blank";

                    if (!datasheetLink.empty())
                        os<<"<div>Datasheet: "<<makeIconLink( rbcOpts, datasheetLink, "Datasheet", "Datasheet", target )<<"</div>\n";
                    
                    if (!refmanLink.empty())
                        os<<"<div>Reference manual: "<<makeIconLink( rbcOpts, refmanLink, "Reference manual", "Reference manual", target )<<"<div>\n";
                    
                    if (!errataLink.empty())
                        os<<"<div>Errata: "<<makeIconLink( rbcOpts, errataLink, "Errata", "Errata", target )<<"<div>\n";

                }


                std::vector<Connection> connectionList;
                connectionsListBuild( rbcOpts, opts, netlistInfo, curMcuD, connectionList );

                if (!processConnectionModifyRules( rbcOpts,  connectionList, processingRules ))
                    return false;

                std::sort( connectionList.begin(), connectionList.end()
                         , [](const Connection &c1, const Connection &c2)
                           {
                               return designatorPinNamesLess(c1.srcPinDesignator, c2.srcPinDesignator);
                           }
                         //  designatorPinNamesLess
                         );


                //groupConnectionsBySheet(connectionList);

                os<<"<table border=\"0\" cellspacing=\"2\" cellpadding=\"5\" class=\"w95p\">\n";
                os<<"<thead><tr><th>Source Pin</th>"              /* 1 */
                                "<th>Net</th>"                    /* 2 */
                                "<th class=\"w30p\">Pin Functions</th>"          /* 3 */
                                "<th>Pass through</th>"           /* 4 */
                                "<th>Target</th>"                 /* 5 */
                                "<th>Target Pin Function</th>"    /* 6 */
                                "<th>Unit Class</th>"             /* 7 */
                                "<th>Unit Purpose</th>"           /* 8 */
                                "<th>Sheet</th>"                  /* 9 */
                                "<th>Unit Type</th>"              /* 10 */
                                "<th class=\"w30p\">Unit Description</th>"       /* 11 */
                                /* << "<th></th>" */
                                "</tr></thead>\n";
                
                os<<"<tbody>\n";

                for( auto& conn : connectionList )
                {
                    os<<"<tr>";
                    os<<"<td>"<<conn.srcPinDesignator<<"</td>";  // 1

                    os<<"<td>"<<conn.srcNetName;
                    std::string srcNetNameModified = conn.processedStrings["MCUNET"];
                    if (conn.srcNetName!=srcNetNameModified)
                    {
                        os<<"<br/>"<<srcNetNameModified;
                    }
                    os<<"</td>"; // 2

                    os<<"<td class=\"w30p\">"; // 3
                    for( auto pfn : conn.srcPinInfo.pinFunctions)
                        os<<pfn<<" ";
                    os<<"</td>";
                    
                    os<<"<td>"; // 4
                    for( auto pld : conn.payloads)
                        os<<pld<<" ";
                    os<<"</td>";

                    if (!conn.isConnected())
                    {
                        os<<"<td class=\"warning\">NC</td><td></td><td></td><td></td><td></td><td></td><td class=\"w30p\"></td>";
                    }
                    else if (!conn.netStop)
                    {
                        os<<"<td>"<<conn.dstPinDesignator<<"</td>"; // 5 Pin or Net name
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
                        if (conn.dstComponentInfo.purpose !=dstComponentPurposeModified)
                        {
                            os<<"<br/>"<<dstComponentPurposeModified;
                        }                        
                        os<<"</td>"; // 8

                        os<<"<td>"<<conn.dstComponentInfo.sheetName<<"</td>";  // 9
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

                        // std::string iconLink;
                        // findDatasheet(rbcOpts, "datasheet.gif", iconLink, true );

                        std::string target = "_blank";

                        if (!datasheetLink.empty())
                            os<<" "<<makeIconLink(rbcOpts, datasheetLink, "Datasheet", "Datasheet", target );
                       
                        if (!refmanLink.empty())
                            os<<" "<<makeIconLink(rbcOpts, refmanLink, "Reference manual", "Reference manual", target );
                       
                        if (!errataLink.empty())
                            os<<" "<<makeIconLink(rbcOpts, errataLink, "Errata", "Errata", target );

                        std::string dstComponentTypeModified = conn.processedStrings["UNITTYPE"];
                        if (conn.dstComponentInfo.typeName!=dstComponentTypeModified)
                        {
                            os<<"<br/>"<<dstComponentTypeModified;
                        }                        


                        os << "</td>";

                        os<<"<td class=\"w30p\">"<<conn.dstComponentInfo.description<<"</td>";
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
                }

                // DD1.23   PC14, GPIO, UART1_TX   MCU_VIDEO_EN
                //      DD2.17 ENABLE Type datasheet
                //      NET Name Descr
/*
    bool               netStop = false;
    std::string        dstPinDesignator;
    std::string        netDescription;

    ComponentPinInfo   dstPinInfo;
    std::string        dstComponentSheet;
    std::string        dstComponentType;
    std::string        dstComponentPurpose;
    unsigned           dstComponentClass;
    std::string        dstComponentClassStr;

    std::vector< std::string> payloads;
*/


                //os<<"<table border=\"0\" cellspacing=\"2\" cellpadding=\"5\" >\n";
                //os<<"<thead><tr><th>ID</th><th>Name</th><th>Type</th><th>Purpose</th><th>Sheet</th><th>Description</th></tr></thead>\n";
                //// << <th></th>
                //os<<"<tbody>\n";
                   //os<<"<tr>";
                   //os<<"<td>"<<designator<<"</td>";
                   //os<<"<td>"<<component.typeName<<" ";
                   //os<<"<td>"<<component.getClassDisplayString()<<"</td>";
                   //os<<"<td>"<<component.purpose<<"</td>";
                   //os<<"<td>"<<component.sheetName<<"</td>";
                   //os<<"<td>"<<component.description<<"</td>";
                   ////os<<"<td>"<<component.<<"</td>";
                   //os<<"</tr>\n";
                //os<<"</tbody>\n";
                //os<<"</table>\n";
                
            
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
}; // struct McuReportGenerator

