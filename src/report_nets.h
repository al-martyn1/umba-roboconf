#pragma once

#include "reports.h"
#include "report_html_base.h"
#include "datasheet.h"



struct NetsReportGenerator : public ReportHtmlBase // IReportGenerator
{
    virtual
    std::string getDefaultFileExt() override
    {
        return "htm";
    }

    virtual
    std::string getReportNameDescription() override
    {
        return std::string("Nets report - display MCU connections");
    }

    virtual
    std::vector<std::string> getReportCommandLineNames() override
    {
        std::vector<std::string> names;
        names.emplace_back( "summary" );
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
        std::string docTitle = "Project summary - Roboconf";

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

            for( auto curMcuD : mcuDesignators )
            {
                auto componentKV = netlistInfo.components.find(curMcuD);
                if (componentKV == netlistInfo.components.end())
                    continue;

                os<<"<h2>"<<curMcuD<<" "<<componentKV.second.typeName<<" connections</h2>\n";

                std::vector<Connection> connectionList;
                connectionsListBuild( netlist, curMcuD, connectionList );

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

        } // for( const auto& nlIt : nets )


        os<<"<body>\n";
        os<<"</body>\n</html>\n";

        return true;

    } // void generateReport

};

