#pragma once

#include <map>
#include <string>

//
#include "tracy_tracing.h"


//-----------------------------------------------------------------------------
inline
std::map< std::string, std::string >& icPackageGetAliasesMap()
{
    static std::map< std::string, std::string > _;
    if (!_.empty())
        return _;

    _["TSSOP-14"]  = "TSSOP14";
    _["TSSOP-20"]  = "TSSOP20";

    _["SOT1397-7"] = "WLCSP25";
    _["SOT1397"]   = "WLCSP25";

    _["SOT1780-5"] = "WLCSP36";
    _["SOT1780"]   = "WLCSP36";

    _["SOT1444-5"] = "WLCSP49";
    _["SOT1444"]   = "WLCSP49";

    _["SOT1782-3"] = "WLCSP64";
    _["SOT1782"]   = "WLCSP64";

    _["SOT1966-1"] = "WLCSP81";
    _["SOT1966-2"] = "WLCSP81";
    _["SOT1966"]   = "WLCSP81";

    _["SOT1073-1"] = "TFBGA64";
    _["SOT1073"]   = "TFBGA64";

    _["SOT926-1"] = "TFBGA100";
    _["SOT926"]   = "TFBGA100";

    // SOIC23-5 ???

    _["SOIC-8"]    = "SOIC8";
    _["SOIC-10"]   = "SOIC10";
    _["SOIC-12"]   = "SOIC12";
    _["SOIC-14"]   = "SOIC14";
    _["SOIC-16"]   = "SOIC16";
    _["SOIC-18"]   = "SOIC18";
    _["SOIC-20"]   = "SOIC20";

    _["SO-8"]      = "SOIC8";
    _["SO-10"]     = "SOIC10";
    _["SO-12"]     = "SOIC12";
    _["SO-14"]     = "SOIC14";
    _["SO-16"]     = "SOIC16";
    _["SO-18"]     = "SOIC18";
    _["SO-20"]     = "SOIC20";

    _["SO8"]       = "SOIC8";
    _["SO10"]      = "SOIC10";
    _["SO12"]      = "SOIC12";
    _["SO14"]      = "SOIC14";
    _["SO16"]      = "SOIC16";
    _["SO18"]      = "SOIC18";
    _["SO20"]      = "SOIC20";

    _["LQFP-32"  ]  = "LQFP32"  ;
    _["LQFP-48"  ]  = "LQFP48"  ;
    _["LQFP-64"  ]  = "LQFP64"  ;
    _["LQFP-100" ]  = "LQFP100" ;
    _["LQFP-144" ]  = "LQFP144" ;
    _["LQFP-176" ]  = "LQFP176" ;
    _["LQFP-208" ]  = "LQFP208" ;
                               
    _["WLCSP-25" ]  = "WLCSP25" ;
    _["WLCSP-36" ]  = "WLCSP36" ;
    _["WLCSP-49" ]  = "WLCSP49" ;
    _["EWLCSP-49"]  = "EWLCSP49";
    _["WLCSP-63" ]  = "WLCSP63" ;
    _["WLCSP-64" ]  = "WLCSP64" ;
    _["WLCSP-66" ]  = "WLCSP66" ;
    _["EWLCSP-66"]  = "EWLCSP66";
    _["WLCSP-72" ]  = "WLCSP72" ;
    _["WLCSP-81" ]  = "WLCSP81" ;
    _["WLCSP-90" ]  = "WLCSP90" ;
    _["WLCSP-100"]  = "WLCSP100";
    _["WLCSP-104"]  = "WLCSP104";
    _["WLCSP-143"]  = "WLCSP143";
    _["WLCSP-144"]  = "WLCSP144";
    _["WLCSP-168"]  = "WLCSP168";
    _["WLCSP-180"]  = "WLCSP180";
                               
    _["UFQFPN-20"]  = "UFQFPN20";
    _["UFQFPN-28"]  = "UFQFPN28";
    _["UFQFPN-32"]  = "UFQFPN32";
    _["UFQFPN-48"]  = "UFQFPN48";
                               
    _["VFQFPN-36"]  = "VFQFPN36";
                               
    _["UFBGA-64" ]  = "UFBGA64" ;
    _["UFBGA-100"]  = "UFBGA100";
    _["UFBGA-132"]  = "UFBGA132";
    _["UFBGA-144"]  = "UFBGA144";
    _["UFBGA-169"]  = "UFBGA169";
    _["UFBGA-176"]  = "UFBGA176";
                               
    _["TFBGA-64" ]  = "TFBGA64" ;
    _["TFBGA-100"]  = "TFBGA100";
    _["TFBGA-216"]  = "TFBGA216";
    _["TFBGA-240"]  = "TFBGA240";
                               
    _["LFBGA-100"]  = "LFBGA100";
    _["LFBGA-144"]  = "LFBGA144";

    // SOT / TSOT TO-3 TO-5 TO-18 TO-66 TO-92 TO-126 TO-220 TO-263 / D2PAK
    // DO-204 DO-213 / MELF DO-214 / SMA / SMB / SMC SOD
    // SIP / SIL
    // DFN DIP / DIL Flat Pack[en] SO / SOIC SOP / SSOP TSOP / TSSOP ZIP[en]
    // LCC PLCC QFN QFP QUIP / QUIL
    // BGA eWLB LGA PGA

    // https://en.wikipedia.org/wiki/Small_Outline_Integrated_Circuit




    return _;
}

//-----------------------------------------------------------------------------
inline
std::map< std::string, unsigned >& icPackageGetPinCountsMap()
{
    static std::map< std::string, unsigned > _;
    if (!_.empty())
        return _;

    _["TSSOP14"]   =  14;
    _["TSSOP20"]   =  20;

    _["LQFP32"]    =  32;
    _["LQFP48"]    =  48;
    _["LQFP64"]    =  64;
    _["LQFP100"]   = 100;
    _["LQFP144"]   = 144;
    _["LQFP176"]   = 176;
    _["LQFP208"]   = 208;

    _["WLCSP25"]   =  25; // aka SOT1397-7 - BGA
    _["WLCSP36"]   =  36; // aka SOT1780-5 - BGA
    _["WLCSP49"]   =  49; // aka SOT1444-5 - BGA    
    _["EWLCSP49"]  =  49; //
    _["WLCSP63"]   =  63; //
    _["WLCSP64"]   =  64; // aka SOT1782-3 - BGA
    _["WLCSP66"]   =  66; // 
    _["EWLCSP66"]  =  66; // 
    _["WLCSP72"]   =  72; // 
    _["WLCSP81"]   =  81; // aka SOT1966-1/SOT1966-2
    _["WLCSP90"]   =  90; // 
    _["WLCSP100"]  = 100; // 
    _["WLCSP104"]  = 104; // 
    _["WLCSP143"]  = 143; // 
    _["WLCSP144"]  = 144; // 
    _["WLCSP168"]  = 168; // 
    _["WLCSP180"]  = 180; // 
    
    _["UFQFPN20"]  =  20;
    _["UFQFPN28"]  =  28;
    _["UFQFPN32"]  =  32;
    _["UFQFPN48"]  =  48;

    _["VFQFPN36"]  =  36;

    _["UFBGA64"]   =  64;
    _["UFBGA100"]  = 100;
    _["UFBGA132"]  = 132;
    _["UFBGA144"]  = 144;
    _["UFBGA169"]  = 169;
    _["UFBGA176"]  = 176;

    _["TFBGA64"]   =  64; // aka SOT1073-1
    _["TFBGA100"]  = 100; // aka SOT926-1
    _["TFBGA216"]  = 216; //
    _["TFBGA240"]  = 240; //
    
    _["LFBGA100"]  = 100;
    _["LFBGA144"]  = 144;

    return _;
}

//-----------------------------------------------------------------------------

inline
std::string icPackageMakeCanonicalName( const std::string &str )
{
    std::string res; res.reserve(str.size());

    std::string::size_type i = 0, size = str.size();
    for(; i!=size; ++i)
       {
        char ch = str[i];

        if ( ch>='A' && ch<='Z' )
           {
            res.append(1, ch );
            continue;
           }

        if ( ch>='a' && ch<='z' )
           {
            res.append(1, ch - 'a' + 'A');
            continue;
           }

        if ( ch>='0' && ch<='9' )
           {
            res.append(1, ch);
            continue;
           }

        if ( ch==' ' )
           {
            res.append(1,'-');
            continue;
           }
           
        if ( ch=='_' )
           {
            res.append(1,'-');
            continue;
           }
           
        if ( ch=='(' || ch==')' )
           {
            res.append(1,ch);
            continue;
           }
           
        res.append(1, '-');
       }

    return res;
}

//-----------------------------------------------------------------------------
inline
std::string icPackageGetCanonicalName( std::string pkgName )
{
    const std::map< std::string, std::string >& m = icPackageGetAliasesMap();

    pkgName = icPackageMakeCanonicalName(pkgName);
    
    std::map< std::string, std::string >::const_iterator it = m.find(pkgName);
    if (it == m.end())
        return pkgName;

    return it->second;
}

//-----------------------------------------------------------------------------
inline
unsigned icPackageGetPinCount( const std::string &pkgName )
{
    std::map< std::string, unsigned >& m = icPackageGetPinCountsMap();

    std::map< std::string, unsigned >::const_iterator it = m.find(icPackageGetCanonicalName(pkgName));
    if (it == m.end())
        return 0;

    return it->second;
}

//-----------------------------------------------------------------------------
std::string icComponentTypeGetCanonicalName( const std::string &str )
{
    return icPackageMakeCanonicalName( str );
}


