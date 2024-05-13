#pragma once

#ifndef SECURITY_WIN32
    #define SECURITY_WIN32
#endif

#include <security.h>
#include <secext.h>

#ifdef _MSC_VER
    #pragma comment(lib, "secur32")
#endif


/*
typedef enum  {
  NameUnknown,
  NameFullyQualifiedDN,
  NameSamCompatible,
  NameDisplay,
  NameUniqueId,
  NameCanonical,
  NameUserPrincipal,
  NameCanonicalEx,
  NameServicePrincipal,
  NameDnsDomain,
  NameGivenName,
  NameSurname
}  *PEXTENDED_NAME_FORMAT;
*/
inline
std::string platformGetUserName()
{
    #if defined(WIN32) || defined(_WIN32)

    char buf[1024] = { 0 };

    // https://docs.microsoft.com/ru-ru/windows/desktop/api/secext/nf-secext-getusernameexa
    // https://docs.microsoft.com/ru-ru/windows/desktop/api/secext/ne-secext-extended_name_format

    #ifdef ROBOCONF_PLATFORM_GET_USER_NAME_EX
    // NameCanonical - "rtc.local/ROOT/Подразделения научных исследований и разработок/11 ЦМРТК/113 Отдел/1134 Лаборатория/Мартынов Александр Георгиевич"
    // NameFullyQualifiedDN - "CN=Мартынов Александр Георгиевич,OU=1134 Лаборатория,OU=113 Отдел,OU=11 ЦМРТК,OU=Подразделения научных исследований и разработок,OU=ROOT,DC=rtc,DC=local"
    // NameSamCompatible - "RTC\\МартыновАГ"
    ULONG ullen = sizeof(buf)/sizeof(buf[0]);
    BOOLEAN blRes = GetUserNameExA( NameSamCompatible, &buf[0], &ullen );

    if (blRes)
    {
       return std::string(buf);
    }
    #endif

    // https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-getusernamea
    DWORD dwlen = sizeof(buf)/sizeof(buf[0]);
    BOOL bRes = GetUserNameA( &buf[0], &dwlen );

    if (bRes)
    {
       return std::string(buf);
    }

    #endif

    return std::string();
}

inline
std::string platformUserNameConvertToFilename( std::string n )
{
    for( auto & c : n )
    {
        switch(c)
           {
            case '\\':
            case '/':
            case '@':
            case '.':
                c = '_';
           }
    }

    return n;
}

std::string platformGetComputerName()
{
    #if defined(WIN32) || defined(_WIN32)
    char buf[1024] = { 0 };
    DWORD dwlen = sizeof(buf)/sizeof(buf[0]);
    BOOL bRes = GetComputerNameA( &buf[0], &dwlen );

    if (bRes)
    {
       return std::string(buf);
    }

    #endif

    return std::string();

}
