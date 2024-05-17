#pragma once

#include "umba/time_service.h"


class ElapsedTimer
{

public:

    using tick_t = umba::time_service::TimeTick;

protected:

    tick_t   m_startTick = 0;

public:

    ElapsedTimer(bool bStart=true)
    {
        if (bStart)
            start();
    }

    void start()
    {
        m_startTick = umba::time_service::getCurTimeMs();
    }

    void restart()
    {
        start();
    }

    tick_t getElapsed() const
    {
        tick_t tickNow = umba::time_service::getCurTimeMs();
        return tickNow - m_startTick;
    }


}; // class ElapsedTimer


