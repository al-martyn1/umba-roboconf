#include "encoding.h"
#include <rdlc-core/case.h>
#include <vector>

#include "compact_enc_det/compact_enc_det.h"
#include "util/encodings/encodings.h"
#include "util/languages/languages.h"
#include "util/port.h"


WindowsEncodingInfo knownEncodings[] = 
{
{ 37	    , "IBM037"	, "IBM EBCDIC (US-Canada)" },
{ 37	    , "CP037"	, "IBM EBCDIC (US-Canada)" },
{ 437	    , "IBM437"	, "OEM United States" },
{ 437	    , "CP437"	, "OEM United States" },
{ 500	    , "IBM500"	, "IBM EBCDIC (International)" },
{ 500	    , "CP500"	, "IBM EBCDIC (International)" },
{ 708	    , "ASMO-708"	, "Arabic (ASMO 708)" },
{ 720	    , "DOS-720"	, "Arabic (DOS)" },
{ 737	    , "ibm737"	, "Greek (DOS)" },
{ 737	    , "CP737"	, "Greek (DOS)" },
{ 775	    , "ibm775"	, "Baltic (DOS)" },
{ 775	    , "CP775"	, "Baltic (DOS)" },
{ 850	    , "ibm850"	, "Western European (DOS)" },
{ 850	    , "CP850"	, "Western European (DOS)" },
{ 852	    , "ibm852"	, "Central European (DOS)" },
{ 852	    , "CP852"	, "Central European (DOS)" },
{ 855	    , "IBM855"	, "OEM Cyrillic" },
{ 855	    , "CP855"	, "OEM Cyrillic" },
{ 857	    , "ibm857"	, "Turkish (DOS)" },
{ 857	    , "CP857"	, "Turkish (DOS)" },
{ 858	    , "IBM00858"	, "OEM Multilingual Latin I" },
{ 860	    , "IBM860"	, "Portuguese (DOS)" },
{ 860	    , "CP860"	, "Portuguese (DOS)" },
{ 861	    , "ibm861"	, "Icelandic (DOS)" },
{ 861	    , "CP861"	, "Icelandic (DOS)" },
{ 862	    , "DOS-862"	, "Hebrew (DOS)" },
{ 862	    , "IBM862"	, "Hebrew (DOS)" },
{ 862	    , "CP862"	, "Hebrew (DOS)" },
{ 863	    , "IBM863"	, "French Canadian (DOS)" },
{ 863	    , "CP863"	, "French Canadian (DOS)" },
{ 864	    , "IBM864"	, "Arabic (864)" },
{ 864	    , "CP864"	, "Arabic (864)" },
{ 865	    , "IBM865"	, "Nordic (DOS)" },
{ 865	    , "CP865"	, "Nordic (DOS)" },
{ 866	    , "IBM866"	, "Cyrillic (DOS)" },
{ 866	    , "CP866"	, "Cyrillic (DOS)" },
{ 869	    , "ibm869"	, "Greek, Modern (DOS)" },
{ 869	    , "CP869"	, "Greek, Modern (DOS)" },
{ 870	    , "IBM870"	, "IBM EBCDIC (Multilingual Latin-2)" },
{ 870	    , "CP870"	, "IBM EBCDIC (Multilingual Latin-2)" },
{ 874	    , "windows-874"	, "Thai (Windows)" },
{ 875	    , "IBM875"	, "IBM EBCDIC (Greek Modern)" },
{ 875	    , "CP875"	, "IBM EBCDIC (Greek Modern)" },
{ 932	    , "shift_jis"	, "Japanese (Shift-JIS)" },
{ 936	    , "gb2312"	, "Chinese Simplified (GB2312)" },
{ 949	    , "ks_c_5601-1987"	, "Korean" },
{ 950	    , "big5"	, "Chinese Traditional (Big5)" },
{ 1026	, "IBM1026"	, "IBM EBCDIC (Turkish Latin-5)" },
{ 1047	, "IBM01047"	, "IBM Latin-1" },
{ 1140	, "IBM01140"	, "IBM EBCDIC (US-Canada-Euro)" },
{ 1141	, "IBM01141"	, "IBM EBCDIC (Germany-Euro)" },
{ 1142	, "IBM01142"	, "IBM EBCDIC (Denmark-Norway-Euro)" },
{ 1143	, "IBM01143"	, "IBM EBCDIC (Finland-Sweden-Euro)" },
{ 1144	, "IBM01144"	, "IBM EBCDIC (Italy-Euro)" },
{ 1145	, "IBM01145"	, "IBM EBCDIC (Spain-Euro)" },
{ 1146	, "IBM01146"	, "IBM EBCDIC (UK-Euro)" },
{ 1147	, "IBM01147"	, "IBM EBCDIC (France-Euro)" },
{ 1148	, "IBM01148"	, "IBM EBCDIC (International-Euro)" },
{ 1149	, "IBM01149"	, "IBM EBCDIC (Icelandic-Euro)" },
{ 1200	, "utf-16"	, "Unicode" },
{ 1200	, "utf16"	, "Unicode" },
{ 1200	, "utf-16le"	, "Unicode" },
{ 1200	, "utf16le"	, "Unicode" },
{ 1201	, "unicodeFFFE"	, "Unicode (Big endian)" },
{ 1201	, "utf-16be"	, "Unicode (Big endian)" },
{ 1201	, "utf16be"	, "Unicode (Big endian)" },
{ 1250	, "windows-1250"	, "Central European (Windows)" },
{ 1251	, "windows-1251"	, "Cyrillic (Windows)" },
{ 1252	, "Windows-1252"	, "Western European (Windows)" },
{ 1253	, "windows-1253"	, "Greek (Windows)" },
{ 1254	, "windows-1254"	, "Turkish (Windows)" },
{ 1255	, "windows-1255"	, "Hebrew (Windows)" },
{ 1256	, "windows-1256"	, "Arabic (Windows)" },
{ 1257	, "windows-1257"	, "Baltic (Windows)" },
{ 1258	, "windows-1258"	, "Vietnamese (Windows)" },
{ 1361	, "Johab"	, "Korean (Johab)" },
{ 10000	, "macintosh"	, "Western European (Mac)" },
{ 10001	, "x-mac-japanese"	, "Japanese (Mac)" },
{ 10002	, "x-mac-chinesetrad"	, "Chinese Traditional (Mac)" },
{ 10003	, "x-mac-korean"	, "Korean (Mac)" },
{ 10004	, "x-mac-arabic"	, "Arabic (Mac)" },
{ 10005	, "x-mac-hebrew"	, "Hebrew (Mac)" },
{ 10006	, "x-mac-greek"	, "Greek (Mac)" },
{ 10007	, "x-mac-cyrillic"	, "Cyrillic (Mac)" },
{ 10008	, "x-mac-chinesesimp"	, "Chinese Simplified (Mac)" },
{ 10010	, "x-mac-romanian"	, "Romanian (Mac)" },
{ 10017	, "x-mac-ukrainian"	, "Ukrainian (Mac)" },
{ 10021	, "x-mac-thai"	, "Thai (Mac)" },
{ 10029	, "x-mac-ce"	, "Central European (Mac)" },
{ 10079	, "x-mac-icelandic"	, "Icelandic (Mac)" },
{ 10081	, "x-mac-turkish"	, "Turkish (Mac)" },
{ 10082	, "x-mac-croatian"	, "Croatian (Mac)" },
{ 12000	, "utf-32"	, "Unicode (UTF-32)" },
{ 12001	, "utf-32BE"	, "Unicode (UTF-32 Big endian)" },
{ 20000	, "x-Chinese-CNS"	, "Chinese Traditional (CNS)" },
{ 20001	, "x-cp20001"	, "TCA Taiwan" },
{ 20002	, "x-Chinese-Eten"	, "Chinese Traditional (Eten)" },
{ 20003	, "x-cp20003"	, "IBM5550 Taiwan" },
{ 20004	, "x-cp20004"	, "TeleText Taiwan" },
{ 20005	, "x-cp20005"	, "Wang Taiwan" },
{ 20105	, "x-IA5"	, "Western European (IA5)" },
{ 20106	, "x-IA5-German"	, "German (IA5)" },
{ 20107	, "x-IA5-Swedish"	, "Swedish (IA5)" },
{ 20108	, "x-IA5-Norwegian"	, "Norwegian (IA5)" },
{ 20127	, "us-ascii"	, "US-ASCII" },
{ 20261	, "x-cp20261"	, "T.61" },
{ 20269	, "x-cp20269"	, "ISO-6937" },
{ 20273	, "IBM273"	, "IBM EBCDIC (Germany)" },
{ 20277	, "IBM277"	, "IBM EBCDIC (Denmark-Norway)" },
{ 20278	, "IBM278"	, "IBM EBCDIC (Finland-Sweden)" },
{ 20280	, "IBM280"	, "IBM EBCDIC (Italy)" },
{ 20284	, "IBM284"	, "IBM EBCDIC (Spain)" },
{ 20285	, "IBM285"	, "IBM EBCDIC (UK)" },
{ 20290	, "IBM290"	, "IBM EBCDIC (Japanese katakana)" },
{ 20297	, "IBM297"	, "IBM EBCDIC (France)" },
{ 20420	, "IBM420"	, "IBM EBCDIC (Arabic)" },
{ 20423	, "IBM423"	, "IBM EBCDIC (Greek)" },
{ 20424	, "IBM424"	, "IBM EBCDIC (Hebrew)" },
{ 20833	, "x-EBCDIC-KoreanExtended"	, "IBM EBCDIC (Korean Extended)" },
{ 20838	, "IBM-Thai"	, "IBM EBCDIC (Thai)" },
{ 20866	, "koi8-r"	, "Cyrillic (KOI8-R)" },
{ 20871	, "IBM871"	, "IBM EBCDIC (Icelandic)" },
{ 20880	, "IBM880"	, "IBM EBCDIC (Cyrillic Russian)" },
{ 20905	, "IBM905"	, "IBM EBCDIC (Turkish)" },
{ 20924	, "IBM00924"	, "IBM Latin-1" },
{ 20932	, "EUC-JP"	, "Japanese (JIS 0208-1990 and 0212-1990)" },
{ 20936	, "x-cp20936"	, "Chinese Simplified (GB2312-80)" },
{ 20949	, "x-cp20949"	, "Korean Wansung" },
{ 21025	, "cp1025"	, "IBM EBCDIC (Cyrillic Serbian-Bulgarian)" },
{ 21866	, "koi8-u"	, "Cyrillic (KOI8-U)" },
{ 28591	, "iso-8859-1"	, "Western European (ISO)" },
{ 28592	, "iso-8859-2"	, "Central European (ISO)" },
{ 28593	, "iso-8859-3"	, "Latin 3 (ISO)" },
{ 28594	, "iso-8859-4"	, "Baltic (ISO)" },
{ 28595	, "iso-8859-5"	, "Cyrillic (ISO)" },
{ 28596	, "iso-8859-6"	, "Arabic (ISO)" },
{ 28597	, "iso-8859-7"	, "Greek (ISO)" },
{ 28598	, "iso-8859-8"	, "Hebrew (ISO-Visual)" },
{ 28599	, "iso-8859-9"	, "Turkish (ISO)" },
{ 28603	, "iso-8859-13"	, "Estonian (ISO)" },
{ 28605	, "iso-8859-15"	, "Latin 9 (ISO)" },
{ 29001	, "x-Europa"	, "Europa" },
{ 38598	, "iso-8859-8-i"	, "Hebrew (ISO-Logical)" },
{ 50220	, "iso-2022-jp"	, "Japanese (JIS)" },
{ 50221	, "csISO2022JP"	, "Japanese (JIS-Allow 1 byte Kana)" },
{ 50222	, "iso-2022-jp"	, "Japanese (JIS-Allow 1 byte Kana - SO/SI)" },
{ 50225	, "iso-2022-kr"	, "Korean (ISO)" },
{ 50227	, "x-cp50227"	, "Chinese Simplified (ISO-2022)" },
{ 51932	, "euc-jp"	, "Japanese (EUC)" },
{ 51936	, "EUC-CN"	, "Chinese Simplified (EUC)" },
{ 51949	, "euc-kr"	, "Korean (EUC)" },
{ 52936	, "hz-gb-2312"	, "Chinese Simplified (HZ)" },
{ 54936	, "GB18030"	, "Chinese Simplified (GB18030)" },
{ 57002	, "x-iscii-de"	, "ISCII Devanagari" },
{ 57003	, "x-iscii-be"	, "ISCII Bengali" },
{ 57004	, "x-iscii-ta"	, "ISCII Tamil" },
{ 57005	, "x-iscii-te"	, "ISCII Telugu" },
{ 57006	, "x-iscii-as"	, "ISCII Assamese" },
{ 57007	, "x-iscii-or"	, "ISCII Oriya" },
{ 57008	, "x-iscii-ka"	, "ISCII Kannada" },
{ 57009	, "x-iscii-ma"	, "ISCII Malayalam" },
{ 57010	, "x-iscii-gu"	, "ISCII Gujarati" },
{ 57011	, "x-iscii-pa"	, "ISCII Punjabi" },
{ 65000	, "utf-7"	, "Unicode (UTF-7)" },
{ 65000	, "utf7"	, "Unicode (UTF-7)" },
{ 65001	, "utf-8"	, "Unicode (UTF-8)" },
{ 65001	, "utf8"	, "Unicode (UTF-8)" },
//{ 0,0,0 }
};


static
std::string filterString(std::string s, bool strong)
{
    std::string res;

    for( auto ch : s )
    {
        if (ch >='A' && ch<='Z')
            res.append( 1, ch );
        else if (ch >='a' && ch<='z')
            res.append( 1, ch-'a'+'A' );
        else if (ch >='0' && ch<='9')
            res.append( 1, ch );

        if (strong)
            continue;

        if (ch =='-' || ch=='_')
            res.append( 1, ch );
    }

    return res;
}

std::map< std::string, UINT>  EncodingsApi::nameToId;
std::map< UINT, std::string>  EncodingsApi::idToName;
std::map< UINT, std::string>  EncodingsApi::idToInfo;

EncodingsApi::EncodingsApi()
{
    for(auto ei : knownEncodings)
    {
        if (ei.codePageIdName==0)
            break;
        nameToId[ filterString(ei.codePageIdName, false) ] = ei.codePage;
        nameToId[ filterString(ei.codePageIdName, false) ] = ei.codePage;
        idToName[ei.codePage] = ei.codePageIdName;
        idToInfo[ei.codePage] = ei.codePageInfo;
    }
}

UINT EncodingsApi::tryGetCodePageByName( std::string name )
{
    std::map< std::string, UINT>::const_iterator it = nameToId.find( filterString(name, false) );
    if (it==nameToId.end())
        return 0;
    return it->second;
}

UINT EncodingsApi::getCodePageByName( std::string name )
{
    UINT cp = tryGetCodePageByName( name );
    if (cp)
        return cp;

    if (name.size()<3)
        return cp;

    cp = tryGetCodePageByName( filterString(name, true) );
    if (cp)
        return cp;

    if (name.size()<3)
        return cp;

    name = toUpper(name);
    if (name[0]=='C' && name[1]=='P' && name[2]>='0' && name[2]<='9')
    {
        name.erase(0, 2);
        name = std::string("windows-") + name;
    }

    return tryGetCodePageByName( name );
}

std::string EncodingsApi::getCodePageName( UINT id )
{
    std::map< UINT, std::string>::const_iterator it = idToName.find(id);
    if (it==idToName.end())
        return std::string();
    return it->second;
}

std::string EncodingsApi::getCodePageInfo( UINT id )
{
    std::map< UINT, std::string>::const_iterator it = idToInfo.find(id);
    if (it==idToInfo.end())
        return std::string();
    return it->second;
}

bool EncodingsApi::isEqualEncodingNames( const std::string &enc1, const std::string &enc2 )
{
    return filterString(enc1, true)==filterString(enc2, true);
}

std::string  EncodingsApi::convert( const char   * data, size_t size, UINT cpSrc, UINT cpDst )
{
    if (cpSrc==cpDst)
    {
        size_t bomSize = 0;
        UINT bomCp = checkTheBom( data, size, &bomSize );
        if (bomCp)
        {
            // strip bom
            data += bomSize;
            size -= bomSize;
        }
        return std::string( data, size );
    }

    std::wstring wstr = decode( data, size, cpSrc );
    return encode( wstr.data(), wstr.size(), cpDst );
}

std::wstring EncodingsApi::decode( const char   * data, size_t size, UINT cp )
{
    if (cp==cpid_UTF16 || cp==cpid_UTF16BE || cp==cpid_UTF8)
    {
        size_t bomSize = 0;
        UINT bomCp = checkTheBom( data, size, &bomSize );
        if (bomCp)
        {
            // strip bom
            data += bomSize;
            size -= bomSize;
        }
    }

    if (cp==cpid_UTF16 || cp==cpid_UTF16BE)
    {
        size /= 2;
        std::wstring res;
        for(size_t i = 0; i!=size; ++i)
        {
            wchar_t w1 = (wchar_t)(unsigned char)*data++;
            wchar_t w2 = (wchar_t)(unsigned char)*data++;
            wchar_t w = cp==cpid_UTF16 ? ((w1) | (w2<<8)) : ((w1<<8) | (w2));
            res.append(1, w);
        }
        return res;
    }

    DWORD flags = MB_PRECOMPOSED;

    if (cp==50220 || cp==50221 || cp==50222 || cp==50225 || cp==50227 || cp==50229 || (cp>=57002 && cp<=57011) || cp==65000 || cp==42)
        flags = 0;

    int reqChars = MultiByteToWideChar( cp, flags, data, (int)size, 0, 0 );
    if (reqChars==0)
        return std::wstring();

    std::vector<wchar_t> buf = std::vector<wchar_t>( (size_t)reqChars, 0 );
    reqChars = MultiByteToWideChar( cp, flags, data, (int)size, &buf[0], (int)buf.size() );
    if (reqChars==0)
        return std::wstring();

    return std::wstring( &buf[0], buf.size() );

}

std::string  EncodingsApi::encode( const wchar_t* data, size_t size, UINT cp )
{
    DWORD flags = WC_NO_BEST_FIT_CHARS;

    if (cp==50220 || cp==50221 || cp==50222 || cp==50225 || cp==50227 || cp==50229 || (cp>=57002 && cp<=57011) || cp==65000 || cp==42 || cp==65001)
        flags = 0;

    int reqSize = WideCharToMultiByte( cp, flags, data, size, 0, 0, 0, 0 );
    if (!reqSize)
       return std::string();

    std::vector<char> buf = std::vector<char>( (size_t)reqSize, 0 );

    reqSize = WideCharToMultiByte( cp, flags, data, size, &buf[0], (int)buf.size(), 0, 0 );
    if (!reqSize)
       return std::string();

    return std::string( &buf[0], buf.size() );
}

UINT EncodingsApi::checkTheBom( const char* data, size_t size, size_t *pBomLen )
{
    if (size>=2 && data[0]==0xFF && data[1]==0xFE)
    {
        if (pBomLen)
            *pBomLen = 2;
        return cpid_UTF16;
    }
    else if (size>=2 && data[0]==0xFE && data[1]==0xFF)
    {
        if (pBomLen)
            *pBomLen = 2;
        return cpid_UTF16BE;
    }
    else if (size>=3 && data[0]==0xEF && data[1]==0xBB && data[2]==0xBF)
    {
        if (pBomLen)
            *pBomLen = 3;
        return cpid_UTF8;
    }

    return 0;
}

std::string  EncodingsApi::detect( const char   * data, size_t size, size_t &bomSize, std::string httpHint, std::string metaHint )
{
    bomSize = 0;

    UINT bomCp = checkTheBom( data, size, &bomSize );
    if (bomCp)
        return getCodePageName(bomCp);

    if ((size & 1)==0)
    {
        INT test = IS_TEXT_UNICODE_STATISTICS | IS_TEXT_UNICODE_CONTROLS | IS_TEXT_UNICODE_ASCII16;
        IsTextUnicode( (void*)data, (int)size, &test);
        if (test & ~IS_TEXT_UNICODE_SIGNATURE)
        {
            return "UTF-16";
        }

        test = IS_TEXT_UNICODE_REVERSE_STATISTICS | IS_TEXT_UNICODE_REVERSE_CONTROLS | IS_TEXT_UNICODE_REVERSE_ASCII16;
        IsTextUnicode( (void*)data, (int)size, &test);
        if (test & ~IS_TEXT_UNICODE_REVERSE_SIGNATURE)
        {
            return "UTF-16BE";
        }

        test = IS_TEXT_UNICODE_NULL_BYTES;
        IsTextUnicode( (void*)data, (int)size, &test);
        if (test & ~IS_TEXT_UNICODE_NULL_BYTES)
        {
            return "UTF-16";
        }
    }


    int bytes_consumed;
    bool is_reliable;

    Encoding enc = 
    CompactEncDet::DetectEncoding( data, size, nullptr,  // URL hint nul
                                  httpHint.empty() ? (const char*)0 : httpHint.c_str(), //nullptr,                      // HTTP hint
                                  metaHint.empty() ? (const char*)0 : metaHint.c_str(), //nullptr,                      // Meta hint
                                  UNKNOWN_ENCODING,
                                  UNKNOWN_LANGUAGE,
                                  CompactEncDet::QUERY_CORPUS, // WEB_CORPUS,
                                  false,  // Include 7-bit encodings?
                                  &bytes_consumed, &is_reliable);

    if ( !IsValidEncoding(enc) )
    {
        return std::string();
    }
    else
    {
        //return EncodingName(enc);
        return MimeEncodingName(enc);
    }

}

bool EncodingsApi::getEncodingInfo( size_t encNumber, std::string &name, std::string &description)
{
    size_t totalKnownEncodings = sizeof(knownEncodings) / sizeof(knownEncodings[0]);
    if (encNumber>=totalKnownEncodings)
        return false;
    name        = knownEncodings[encNumber].codePageIdName;
    description = knownEncodings[encNumber].codePageInfo;
    return true;
}

EncodingsApi* getEncodingsApi()
{
    static EncodingsApi _;
    return &_;
}



