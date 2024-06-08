#pragma once

#include "rdlc-core/fsUtils.h"

#include <string>

//
#include "tracy_tracing.h"



struct IReportGenerator;


struct ReportGenerationInfo
{
    std::string          reportType;
    std::string          reportFile;   // Output file< can be empty
    IReportGenerator*    pGen = 0;

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

