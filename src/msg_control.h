#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>

//
#include "tracy_tracing.h"


class MsgControl
{
    std::set<std::string>    disabledMsgs;

public:

    MsgControl() : disabledMsgs() {}

    void disable( const std::string &msgType )
    {
        disabledMsgs.insert(msgType);
    }

    bool isAllowed(const std::string &warnType) const
    {
        if (disabledMsgs.find("all")!=disabledMsgs.end())
            return false;
        if (disabledMsgs.find(warnType)!=disabledMsgs.end())
            return false;

        return true;
    }

}; // class MsgControl


