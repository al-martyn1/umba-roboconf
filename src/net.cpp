#include "logerr.h"
//

#include "net.h"
#include "connections.h"


bool processConnectionModifyRules( RoboconfOptions &rbcOpts, std:: vector<Connection> &connectionList, const expression_list_t &processingRules );

void McuConnectionsInfo::buildConnectionList( RoboconfOptions &rbcOpts
                                            , const ConnectionBuildingOptions &buildingOptions
                                            , const NetlistInfo &netlist
                                            , const std::string &mcuDesignator_
                                            )
{
    mcuDesignator = mcuDesignator_;
    connectionList.clear();
    connectionsListBuild( rbcOpts, buildingOptions, netlist, mcuDesignator, connectionList);
}

bool McuConnectionsInfo::processConnectionModifyRules( RoboconfOptions &rbcOpts
                                                     , const expression_list_t &processingRules
                                                     )
{
    bool res = ::processConnectionModifyRules(rbcOpts, connectionList, processingRules);
    if (res)
    {
        connectionsListRemoveMcuDuplicates( connectionList );
    }

    return res;
}

void McuConnectionsInfo::splitConnectionsToGroups( RoboconfOptions &rbcOpts )
{
    connectionGroups.clear();
    splitConnectionsToGroupsByTarget( rbcOpts, connectionGroups, connectionList );
}

bool McuConnectionsInfo::detectConnectionsInterfaces( RoboconfOptions &rbcOpts
                                                    , const expression_list_t &processingRules
                                                    , bool operateVerbose
                                                    //, std::string targetPurpose
                                                    )
{
    return connectionsDetectInterfaces( rbcOpts, connectionGroups, processingRules, operateVerbose /*, targetPurpose*/ );
}
 
