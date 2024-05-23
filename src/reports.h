#pragma once

#include "pinUtils.h"
#include "rdlc-core/fsUtils.h"

struct IReportGenerator;


struct ReportGenerationInfo
{
    string            reportType;
    string            reportFile;   // Output file< can be empty
    IReportGenerator* pGen = 0;

    ReportGenerationInfo() = default;
    ReportGenerationInfo(const std::string &type, const std::string &file=std::string())
    : reportType(type)
    , reportFile(file)
    {
    }


    void configureOutputFile(std::string commonOutputPath, const std::string &inputFileName )
    {
        if (commonOutputPath.empty()) // берем путь от входного файла, если выходной путь не задан
        {
            commonOutputPath = getPath(inputFileName);
        }

        // Если для отчета не задан явно путь, используем выходной путь
        std::string reportOutputPath = getPath(reportFile);
        if (reportOutputPath.empty())
        {
            reportOutputPath = commonOutputPath;
        }

        std::string reportFileNoPath = getNameFromFull(reportFile);
        if (reportFileNoPath.empty())
        {
            std::string inputFileNoPathNoExt = getFileName(inputFileName);
            reportFileNoPath = inputFileNoPathNoExt + "-" + reportType + ".html";
        }

        reportFile = appendPath(reportOutputPath, reportFileNoPath);
    }

};



struct IReportGenerator
{
    virtual
    std::string getDefaultFileExt() = 0;

    virtual
    std::string getReportNameDescription() = 0;

    virtual
    std::vector<std::string> getReportCommandLineNames() = 0;

    virtual
    bool generateReport( RoboconfOptions &rbcOpts
                       , std::ostream &os
                       , std::map<std::string, NetlistInfo> &nets
                       , std::vector< ComponentInfo > libComponents
                       , const expression_list_t &processingRules
                       , const ConnectionBuildingOptions &opts
                       , size_t &processedMcus
                       ) = 0;
    //bool generateReport( std::ostream &os, std:: map<std::string, NetlistInfo> &nets, std::map<std::string, std::vector< ComponentInfo > > libComponents, const expression_list_t &processingRules, const ConnectionBuildingOptions &opts) = 0;
};



void addReportGenerator( std::map< std::string, IReportGenerator* > &reportGenerators, IReportGenerator *pReportGenerator )
{
    std::vector<std::string> names = pReportGenerator->getReportCommandLineNames();
    for( auto name : names )
    {
        reportGenerators[name] = pReportGenerator;
    }
}


