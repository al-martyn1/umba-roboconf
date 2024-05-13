#pragma once



struct IReportGenerator
{
    virtual
    std::string getDefaultFileExt() = 0;

    virtual
    std::string getReportNameDescription() = 0;

    virtual
    std::vector<std::string> getReportCommandLineNames() = 0;

    virtual
    bool generateReport( RoboconfOptions &rbcOpts, std::ostream &os, std::map<std::string, NetlistInfo> &nets, std::vector< ComponentInfo > libComponents, const expression_list_t &processingRules, const ConnectionBuildingOptions &opts) = 0;
    //bool generateReport( std::ostream &os, std::map<std::string, NetlistInfo> &nets, std::map<std::string, std::vector< ComponentInfo > > libComponents, const expression_list_t &processingRules, const ConnectionBuildingOptions &opts) = 0;
};



void addReportGenerator( std::map< std::string, IReportGenerator* > &reportGenerators, IReportGenerator *pReportGenerator )
{
    std::vector<std::string> names = pReportGenerator->getReportCommandLineNames();
    for( auto name : names )
    {
        reportGenerators[name] = pReportGenerator;
    }
}

