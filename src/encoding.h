#pragma once

#include <winsock2.h>
#include <windows.h>
#include <map>
#include <string>



struct WindowsEncodingInfo
{
    UINT           codePage;
    const char*    codePageIdName;
    const char*    codePageInfo;
};





class EncodingsApi;

EncodingsApi* getEncodingsApi();

class EncodingsApi
{

public:

    static const UINT cpid_UTF16   = 1200;
    static const UINT cpid_UTF16BE = 1201;
    static const UINT cpid_UTF8    = 65001;

    friend EncodingsApi* getEncodingsApi();

    UINT getCodePageByName( const std::string name );
    std::string getCodePageName( UINT id );
    std::string getCodePageInfo( UINT id );

    std::wstring decode( const char   * data, size_t size, UINT cp );
    std::string  encode( const wchar_t* data, size_t size, UINT cp );
    std::string  detect( const char   * data, size_t size, size_t &bomSize, std::string httpHint = std::string(), std::string metaHint = std::string() );
    std::string  convert( const char   * data, size_t size, UINT cpSrc, UINT cpDst );

    std::wstring decode( const std::string &str, UINT cp )                 { return decode( str.data(), str.size(), cp); }
    std::string  encode( const std::wstring &str, UINT cp )                { return encode( str.data(), str.size(), cp); }
    std::string  detect( const std::string &str, size_t &bomSize, std::string httpHint = std::string(), std::string metaHint = std::string() )
                 { return detect( str.data(), str.size(), bomSize, httpHint, metaHint ); }
    std::string  convert( const std::string &str, UINT cpSrc, UINT cpDst ) { return convert( str.data(), str.size(), cpSrc, cpDst ); }

    bool isEqualEncodingNames( const std::string &enc1, const std::string &enc2 );

    bool getEncodingInfo( size_t encNumber, std::string &name, std::string &description);

private:

    UINT tryGetCodePageByName( const std::string name );

    UINT checkTheBom( const char* data, size_t size, size_t *pBomLen = 0 );

    static std::map< std::string, UINT>  nameToId;
    static std::map< UINT, std::string>  idToName;
    static std::map< UINT, std::string>  idToInfo;

    EncodingsApi();
};


