/*! \file
\brief Базовые определения для интерфейсов, и связанные с ним определения
*/

#pragma once

#include <stdint.h>
#include "errors.h"

#ifndef interface
    #if (_MSC_VER >= 1100) && defined(__cplusplus)
        //#define interface struct 
        #define interface struct __declspec(novtable)
    #else
        #define interface struct
    #endif
#endif

#ifndef inherits
    #define inherits public
    //#define inherits public virtual
#endif

#ifndef implements
    #define implements public
    //#define implements public virtual
#endif



namespace umba
{

//! ID интерфейса, CRC32 от обычного GUID
typedef uint32_t  InterfaceId;


#define UMBA_DECLARE_INTERFACE_ID(id)  static const umba::InterfaceId interfaceId = id


#define UMBA_BEGIN_INTERFACE_MAP()                                             \
                   virtual                                                     \
                   umba::Result<void*> queryInterface( umba::InterfaceId id ) override  \
                   {                                                           \
                       switch(id)                                              \
                          {                                                    \
                              case umba::IUnknown::interfaceId : return (void*)static_cast<umba::IUnknown*>(this);

#define UMBA_BEGIN_INTERFACE_MAP_EX(mainInterfaceType)                         \
                   virtual                                                     \
                   umba::Result<void*> queryInterface( umba::InterfaceId id ) override \
                   {                                                           \
                       switch(id)                                              \
                          {                                                    \
                              case umba::IUnknown::interfaceId : return (void*)static_cast<umba::IUnknown*>(static_cast<mainInterfaceType*>(this));


#define UMBA_IMPLEMENT_INTERFACE(IterfaceType)    case IterfaceType :: interfaceId : return (void*)static_cast< IterfaceType* >(this);


#define UMBA_END_INTERFACE_MAP()                                                      \
                        default: return umba::Error(umba::errors::unknown_interface); \
                       }                                                              \
                   }

template<typename InterfaceType>
InterfaceId interfaceIdOf(InterfaceType *pInterface)
{
    return InterfaceType::interfaceId;
}

template<typename InterfaceType>
InterfaceId interfaceIdOf(InterfaceType **ppInterface)
{
    return InterfaceType::interfaceId;
}


//! Базовый интерфейс IUnknown
interface IUnknown
{
    UMBA_DECLARE_INTERFACE_ID(0x00000000);

    virtual
    Result<void*> queryInterface( InterfaceId ) = 0;

    template<typename TInterface>
    Result<TInterface*> queryInterface()
    {
        if (auto res = queryInterface(TInterface::interfaceId))
        {
            TInterface *pInterface = static_cast<TInterface*>(res.value);
            return pInterface;
        }
        else 
        {
            return Error(res);
        }
    }

    template<typename TInterface>
    Error queryInterface( TInterface **p)
    {
        if (!p)
            return errors::invalid_param;

        if (auto res = queryInterface<TInterface>())
        {
            *p = static_cast<TInterface*>(res.value);
            return errors::ok;
        }
        else 
        {
            return Error(res);
        }
    }

    template<typename TInterface>
    Error queryInterface( TInterface* &p)
    {
        if (auto res = queryInterface<TInterface>())
        {
            p = static_cast<TInterface*>(res.value);
            return errors::ok;
        }
        else 
        {
            return Error(res);
        }
    }


}; // interface IUnknown


}; // namespace umba





