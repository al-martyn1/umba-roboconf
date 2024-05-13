/*! \file
\brief Реализация некоторых базовых интерфейсов
*/

#pragma once

#include "basic_interfaces.h"

#include <utility>


namespace umba
{


struct PingRequestGeneratorImplBase : implements IPingRequestGenerator
{

    //! Задает период пингов
    virtual
    umba::time_service::TimeTick setPingPeriod( umba::time_service::TimeTick t ) override
    {
        std::swap( m_pingPeriod, t );
        return t;
    }

    //! Возвращает период пингов
    virtual
    umba::time_service::TimeTick getPingPeriod( ) override
    {
        return m_pingPeriod;
    }


protected:

    umba::time_service::TimeTick  m_pingPeriod = 0;
};







} // namespace umba

