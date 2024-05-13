/*! \file
\brief Базовые интерфейсы
*/

#pragma once

#include "interface.h"
#include "umba/time_service.h"


namespace umba
{

//! Интерфейс полируемого
interface IPollCapable : inherits umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xB54FCCA9);

    virtual
    void poll() = 0;

    virtual
    bool isReadyForPoll() = 0;

}; // interface IPollCapable


//! Интерфейс ждуна
interface IIdleCapable : inherits umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xD001476E);

    virtual
    void onIdle() = 0;

}; // interface IIdleCapable


//! "Something is complete" event handler interface
interface ICompletionHandler : inherits umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xA1A1E85B);

    /*! Called when something is done.
    */
    virtual
    void onComplete( unsigned customEventId //!< Some user defined code
                   ) = 0;

}; // interface ICompletionHandler


//! Обработчик события таймера
/* Каждый обработчик может обрабатывать несколько таймеров
   различними ID.
   ID задается при создании таймера.
 */
interface ITimerHandler : inherits umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0xF10B5BD3);

    /*! Called when timer event generated
    */
    virtual
    void onTimer( unsigned timerId ) = 0;

}; // interface ITimerHandler


//! Обработчик запросов пинга
interface IPingRequestHandler : inherits umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0x800459C3);

    //! Можно ли пинговать, или чем-то занят?
    virtual
    bool isReadyForPing() = 0;
    /*! Called when ping request generated
    */
    virtual
    void onPing( ) = 0;

}; // interface IPingRequestHandler




//! Интерфейс генератора запросов пинга
/*! Сам по себе интерфейс ничего не говорит о том, 
    как он генерирует события - по аппаратному таймеру,
    например, или этот объект, надо полировать.
    Также этот интерфейс ничего не говорит о том, кто и как обрабатывает
    запросы пингов
 */
interface IPingRequestGenerator : inherits umba::IUnknown
{

    UMBA_DECLARE_INTERFACE_ID(0x6A8CA481);

    //! Задает период пингов, 0 - пинги не генерятся. Пинги обычно генерятся только если никакого другого обмена с устройством не производится
    virtual
    umba::time_service::TimeTick setPingPeriod( umba::time_service::TimeTick t ) = 0;

    //! Возвращает период пингов
    virtual
    umba::time_service::TimeTick getPingPeriod( ) = 0;

}; // interface ICompletionHandler





} // namespace umba

