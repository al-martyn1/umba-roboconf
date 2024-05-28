#pragma once

#include "reports.h"
#include "report_html_base.h"
#include "datasheet.h"



struct SummaryReportGenerator : public ReportHtmlBase // IReportGenerator
{
    virtual
    std::string getDefaultFileExt() override
    {
        return "htm";
    }

    virtual
    std::string getReportNameDescription() override
    {
        return std::string("Summaru report - displays used chips and gates");
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

        processedMcus = 1; // Нам в данном отчете MCU без надобности, мы их тут не обрабатываем, но чтобы наверху не ругались, говорим, что нашли

        os<<htmlDocType();
        os<<"<html>\n<head>\n<title>"<< docTitle << "</title>\n";
        //os<<;        
        os<<"</head>\n";
        os<<htmlStyle();

        for( const auto& nlIt : nets )
        {
            NetlistInfo netlistInfo = nlIt.second; 
            os<<"<h1>"<<netlistInfo.projectName<<" - "<<netlistInfo.name<<"</h1>\n";

            std::unordered_set<ComponentClass> reportComponentClasses;
            reportComponentClasses.insert(ComponentClass::cc_DD         );
            reportComponentClasses.insert(ComponentClass::cc_DA         );
            reportComponentClasses.insert(ComponentClass::cc_AA         );
            reportComponentClasses.insert(ComponentClass::cc_TRANSISTOR );
           
            bool allowProcessAssemblies = false;//true;
           
            std::vector< std::string > designators;

            netlistInfo.traverseComponents( ComponentDesignatorsCollector(designators), reportComponentClasses, allowProcessAssemblies );

            std::sort(designators.begin(), designators.end(), designatorPinNamesLess );
           
            os<<"<table border=\"0\" cellspacing=\"2\" cellpadding=\"5\" >\n";
            os<<"<thead><tr><th>ID</th><th>Component Type</th><th>Package</th><th>Class</th><th>Purpose</th><th>Sheet</th><th>Description</th></tr></thead>\n";
            // << <th></th>
            os<<"<tbody>\n";

            for( auto designator : designators )
            {
                const auto& componentIt = netlistInfo.components.find(designator);
                if (componentIt==netlistInfo.components.end())
                    continue;


                auto& component = componentIt->second;

                std::string datasheetLink;
                std::string refmanLink;
                std::string errataLink;
                findDatasheet( rbcOpts, component.datasheet, datasheetLink  );
                findDatasheet( rbcOpts, component.refman   , refmanLink     );
                findDatasheet( rbcOpts, component.errata   , errataLink     );

                // std::string iconLink;
                // findDatasheet(rbcOpts, "datasheet.gif", iconLink, true );
                //findDatasheet( 0, "icon_pdf.png", iconLink );
                

/*
                if (!datasheetLink.empty() && !refmanLink.empty())
                {
                    // both
                }
                else if (!datasheetLink.empty())
                {
                    // datasheet present
                }
                else if (!refmanLink.empty())
                {
                    // refman present
                }
*/
                os<<"<tr>";
                os<<"<td>"<<designator<<"</td>";
                os<<"<td>"<<component.typeName<<" ";

                std::string target = "_blank";

                if (!datasheetLink.empty())
                {
                    os<<" ";
                    os<<makeIconLink( rbcOpts, datasheetLink, "Datasheet", "Datasheet", target );
                    //os << "<a title=\"Datasheet\" href=\"" << datasheetLink << "\"><img src=\"" << iconLink << "\" alt=\"Datasheet\"></a>";
                }

                if (!refmanLink.empty())
                {
                    os<<" ";
                    os<<makeIconLink( rbcOpts, refmanLink, "Reference manual", "Reference manual", target );
                    //os << "<a title=\"Reference manual\" href=\"" << refmanLink << "\"><img src=\"" << iconLink << "\" alt=\"Reference manual\"></a>";
                }

                if (!errataLink.empty())
                {
                    os<<" ";
                    os<<makeIconLink( rbcOpts, errataLink, "Errata", "Errata", target );
                    //os << "<a title=\"Errata\" href=\"" << errataLink << "\"><img src=\"" << iconLink << "\" alt=\"Errata\"></a>";
                }
                
                os<<"</td>";


                os<<"<td>"<<component.package;
                if (!rbcOpts.packagesDb.isKnownPackage(component.package))
                {
                    os<<" (Unknown)";
                }
                os<<"</td>";

                os<<"<td>"<<component.getClassDisplayString()<<"</td>";
                os<<"<td>"<<component.purpose<<"</td>";
                os<<"<td>"<<component.sheetName<<"</td>";
                os<<"<td>"<<component.description<<"</td>";
                //os<<"<td>"<<component.<<"</td>";
                os<<"</tr>\n";
            }

            os<<"</tbody>\n";
            os<<"</table>\n";

        }


        //std::map< std::string, ComponentInfo > components;
        //std::map< std::string, NetInfo >       nets;



        os<<"<body>\n";
        os<<"</body>\n</html>\n";

        return true;
    }
};

