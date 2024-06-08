#pragma once

#include <utility>
#include <sstream>

//
#include "config.h"
//


#include "rdlc-core/macros.h"
#include "regexp.h"
#include "csv.h"
#include "rdlc-core/utils.h"
#include "rdlc-core/splits.h"

#include "component_class.h"
#include "roboconf_options.h"

#include "ioUtils.h"

#include "netlist_src_type.h"
#include "netlist_component_atributes_map.h"

#include "string_set_type.h"
#include "string_string_map_type.h"


#include "roboconf_options.h"

//#include "icUtils.h"

//
//
#include "tracy_tracing.h"



//-----------------------------------------------------------------------------
std:: vector<std::string> generateComponentNames( std::string cmpName, const std::string &cmpPackage = std::string(), const std::string &cmpUserSuffix = std::string() );
std::string filterPinNameNegation( std::string str );

//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
struct ComponentPinInfo
{
    #if defined(ROBOCONF_COMPONENT_PIN_INFO_USE_UNORDERED_SET)
        using set_type = std::unordered_set<std::string>;
    #else
        using set_type = std::set<std::string>;
    #endif

    std::string     pinNo;
    set_type        pinFunctions;
    std::string     pinDescription;

    void merge( const ComponentPinInfo &pi );
    bool isPinFunction( const std::string &fnName ) const;

}; // struct ComponentPinInfo


inline
bool operator<( const ComponentPinInfo &ci1, const ComponentPinInfo &ci2 )
{
    return ci1.pinNo < ci2.pinNo;
}

//-----------------------------------------------------------------------------
/*
template < typename StreamType >
StreamType& operator<<( StreamType &s, const ComponentPinInfo &pi )
{
    s<<pi.pinNo<<" - ";
    std::set< std::string >::const_iterator pit = pi.pinFunctions.begin();
    for(; pit != pi.pinFunctions.end(); ++pit)
    {
        if (pit != pi.pinFunctions.begin())
            s<<", ";
        s<<*pit;
    }
    return s;
}
*/
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
struct ComponentPackageNameCharValidator
{
    bool operator()( char ch ) const
    {
        if (ch>='a' && ch<='z')
            return true;
        if (ch>='A' && ch<='Z')
            return true;
        if (ch>='0' && ch<='9')
            return true;
        return false;
    }

};

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
enum class ComponentTypeNameMatchType
{
    matchExact,
    matchRegex,
    matchSimple
};

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
struct ComponentInfo //-V730
{
    #if defined(ROBOCONF_COMPONENT_INFO_PINS_USE_UNORDERED_MAP)
        using pins_map_type = std::unordered_map< std::string, ComponentPinInfo >;
    #else
        using pins_map_type = std:: map< std::string, ComponentPinInfo >;
    #endif

    // net info
    std::string                designator;
    std::string                typeName;
    std::string                typeNameOrg; // before making it canonical
    std::string                sheetName;
    ComponentClass             componentClass = ComponentClass::cc_UNKNOWN;
    unsigned                   assembly = 0;

    char                       singleCharMask;
    char                       anyCharMask;
    ComponentTypeNameMatchType typeNameMatchType = ComponentTypeNameMatchType::matchExact; // matchExact, matchRegex, matchSimple
    

    // net or lib info
    std::string                package;
    pins_map_type              pins;

    // lib info
    std:: vector<std::string>  allTypeNames;
    std::string                description;
    std::string                purpose;
    std::string                datasheet;
    std::string                refman;
    std::string                errata;
    std:: vector<std::string>  modules;
    std:: vector<std::string>  headers;

    typedef std:: vector< std::string > internal_net_t;
    std:: vector< internal_net_t > internalNets;

    typedef std::pair< std::string, std:: vector< std::string > > pin_function_match_t;
    std:: vector< pin_function_match_t > pinFunctionMatches;

    typedef std::set< std::string >  part_pins_t;
    std:: vector< part_pins_t >    partsDef;

   

    bool valid() const;
    bool isDesignatorTypeEqualTo( std::string designatorType /* Type/Prefix */  ) const;
    //bool detectComponentClassFromDesignator();
    bool findPinNoByFunction( const std::string &pinFn, std::string &pinNo ) const;
    void addPinsPart( const part_pins_t &part );
    void mergePinParts( const std:: vector< part_pins_t > &partsDefOther);
    bool hasParts() const;
    int getPinPartNo( std::string pinNo ) const;
    std:: vector< internal_net_t > getInternalNets() const;
    void addPinFunctionMatch( const pin_function_match_t & match);
    bool isPackageCompatible( RoboconfOptions &rbcOpts, const ComponentInfo &ci ) const;
    bool isPackageAny() const;
    void merge( RoboconfOptions &rbcOpts, const ComponentInfo &ci );
    void matchPinFunctions( RoboconfOptions &rbcOpts );
    std::string getClassDisplayString() const;
    std::string getComponentClassString() const;
    void addPin( std::string pinNo, const std::string &pinFn );
    void detectComponentClass( RoboconfOptions &rbcOpts );
    void detectComponentClass( RoboconfOptions &rbcOpts, const NetlistComponentAttributesMap &fields );

    template< typename Iter >
    void addPin( std::string pinNo, Iter b, Iter e )
    {
        ComponentPinInfo pinInfo;
        pinInfo.pinNo = pinNo;

        for(; b!=e; ++b)
        {
            pinInfo.pinFunctions.insert( *b );
        }

        pins[pinInfo.pinNo] = pinInfo;
    }

    void addPin( std::string pinNo, const std:: vector< std::string > &fns );
    bool checkTypeNameMatch( const std::string otherTypeNameMatchTo ) const;


}; // struct ComponentInfo

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
struct ComponentTypePackage
{
    std::string type;
    std::string package;

    ComponentTypePackage() : type(), package() {}
    ComponentTypePackage( const std::string &t, const std::string &p = std::string() ) : type(t), package(p) {}

    std::string getFullName() const
    {
        if (package.empty())
            return type;
        return type + std::string("-") + package;
    }

}; // struct ComponentTypePackage

//-----------------------------------------------------------------------------
inline
bool operator<( const ComponentTypePackage &cp1, const ComponentTypePackage &cp2 )
{
    return cp1.getFullName() < cp2.getFullName();
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
inline
std::string normalizeComponentNameBasic(std::string cmpName)
{
    for(auto &ch : cmpName)
    {
        if (ch=='_')
        {
            ch = '-';
        }
        else if (ch>='a' && ch<='z')
        {
            ch = ch - 'a' + 'A';
        }
    }

    return cmpName;
}

//-----------------------------------------------------------------------------
inline
std::string normalizeComponentName(RoboconfOptions &rbcOpts, std::string cmpName, std::string *pFoundPackageName)
{
    cmpName = normalizeComponentNameBasic(cmpName);

    std::vector<std::string> nameParts;
    splitToVector(cmpName, nameParts, '-');

    if (nameParts.size()<2)
        return cmpName;

    std::string pkgCandi;
    std::size_t partIdx = nameParts.size();
    for(; partIdx!=1; --partIdx)
    {
        auto namePart = nameParts[partIdx-1];
        trim(namePart, "- ");

        if (pkgCandi.empty())
        {
            pkgCandi = namePart;
        }
        else
        {
            pkgCandi = namePart + "-" + pkgCandi;
        }
    
        if (rbcOpts.packagesDb.isKnownPackage(pkgCandi))
        {
            if (pFoundPackageName)
            {
                *pFoundPackageName = rbcOpts.packagesDb.getCanonicalPackageName(pkgCandi);
            }

            nameParts.erase(nameParts.begin()+partIdx-1, nameParts.end());
            cmpName = mergeStrings(nameParts, std::string("-"), false);
            break;
        }
    }

    #if 0
    auto lastPart = nameParts.back();
    trim(lastPart, "- ");

    if (rbcOpts.packagesDb.isKnownPackage(lastPart))
    {
        // Удаляем корпус из названия компонета
        std:: vector<std::string>::iterator backIt = nameParts.end();
        --backIt;
        nameParts.erase(backIt, nameParts.end());

        // Собираем имя компонента обратно
        cmpName = mergeStrings(nameParts, std::string("-"), false);

        // Сохраняем имя типа корпуса как каноническое
        if (pFoundPackageName)
        {
            *pFoundPackageName = rbcOpts.packagesDb.getCanonicalPackageName(lastPart);
        }
    }
    #endif

    return cmpName;

}

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/*
template < typename StreamType >
inline
StreamType& operator<<( StreamType &s, const ComponentInfo &ci )
{
    s<<ci.designator<<" - "<<ci.typeName<<" - "<<ci.getClassDisplayString()<<" - "<<ci.package<<" - "<<ci.sheetName<<"\n";
    
    for( auto pi : ci.pins )
    {
        s<<"    "<<pi.second<<"\n";
    }

    return s;
}
*/

//-----------------------------------------------------------------------------
// Как происходит поиск компонента?
// Разбиваем имя:
//   1) убираем все неалфавитно-цифровые символы, кроме '_' и '-' - подчерк заменяем на минус
//   2) разбиваем на части по цифрам/буквам (классам - alpha, digit, sep)
//   3) Минусы присовокупляем к именам после них
// К вектору частей имени добавляем имя package, если задан - не-не-не-не. Имя package добавляем на каждом шаге, пробуем с ним и без
// Последовательно, от самого длинного к самому короткому имени:
// Пытаемся читать файлы, если еще не прочитан
// То, что вычитали в файле - добавляет информацию к компонентам
//
// После того, как всё прочитали:
// также последовательно ищем компонент по тому же алгоритму

// Обрабатываем только компоненты - DD, DA, A, VT, R

// В одном файле может быть описание нескольких компонентов


//-----------------------------------------------------------------------------
struct ComponentImportOptions
{
    std:: vector< std::string >  namesList;
    std::string                  description;
    std::string                  purpose;
    std::string                  datasheet;
    std::string                  refman;
    std::string                  errata;
    std::string                  package;
    std:: vector<std::string>    modules;
    std:: vector<std::string>    headers;
    std::string                  invalChars = "[]()/*";

    char                         csvSeparator = ',';
    std::string                  designatorFieldCaption = "Pin Designator"; // Altium 20, for 17 that is simple "Designator"
    std::string                  nameFieldCaption       = "Display Name"; // Altium 20, for 17 that is simple "Name"

}; // struct ComponentImportOptions


//-----------------------------------------------------------------------------
inline
bool componentImportFromCsv(RoboconfOptions &rbcOpts, std::istream &in, const ComponentImportOptions &opts, ComponentInfo &ci /* , char sep = ';' */  )
{
    std:: vector< csv_line_t > csvLines; csvLines.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    if (!readCsv( in, csvLines, opts.csvSeparator ))
        return false;

// "Pin Designator,Net Name,Display Name" -> "Designator,Net,Name"
// "Altium Pin Report File" -> "Report Generated From Altium Designer;;;;;;;;;;;;;;;;;;;;;;;;"


// Altium 20
// # Altium Pin Report File
// 
// Pin Designator,Net Name,Display Name,IO Bank,IO Standard,Slew Rate,Drive Strength,IO Pin,Vref Pin,CLK Pin,Config Pin,Special Pin,Group,Differential Pair
// 1,NetC45_1,Z,,,,,,,,,,,
// 2,term_p1,Y7,,,,,,,,,,,
// 3,term_p2,Y6,,,,,,,,,,,


// Altium 17

// ;;;;;;;;;;;;;;;;;;;;;;;;
// Report Generated From Altium Designer;;;;;;;;;;;;;;;;;;;;;;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;
// Designator;;Name;Desc;;;;TO-PMOD-7;;;Type;;Owner;;Show;;Number;;Name;;;Pin/Pkg Length;;;
// 1;;VIN;;;;;1;;;Passive;;1;;True;;True;;VIN;;;0mm;;;
// ;;;;;;;;;;;;;;;;;;;;;;;;
// вторник 16-окт-16.2018   7:01:23 PM;;;;;;;;;;;;;;;;;;;;;;;Page 1 of 1;
// ;;;;;;;;;;;;;;;;;;;;;;;;

    size_t nameIdx = (size_t)-1;
    size_t designatorIdx = (size_t)-1;
    std:: vector< csv_line_t > validLines; validLines.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    for( auto line : csvLines )
    {
        // пропускаем всю левую шляпу

        if (line.size()<2)
            continue;

        if (csvLineGetNonEmptyCount( line )==0)
            continue;

        if (!line[0].empty() && startsWith(line[0], "Report Generated"))
            continue;

        if (!line[0].empty() && startsWith(line[0], "# Altium Pin Report File"))
            continue;

        if (csvLineFindContaining( line, "Report Generated")==0)
            continue;

        size_t idx = csvLineFindContaining( line, "Page ");
        if (idx!=(size_t)-1)
        {
            size_t idx2 = csvLineFindContaining( line, " of ", idx);
            if (idx==idx2)
                continue;
        }

        // Вот тут пробуем найти индексы полей по их заголовкам
    // std::string                  designatorFieldCaption = "Pin Designator"; // Altium 20, for 17 that is simple "Designator"
    // std::string                  nameFieldCaption       = "Display Name"; // Altium 20, for 17 that is simple "Name"

        if (designatorIdx == (size_t)-1)
        {
            designatorIdx = csvLineFindEqual(line, opts.designatorFieldCaption /* "Designator" */  );
            if (designatorIdx==(size_t)-1)
                continue;
            nameIdx = csvLineFindEqual( line, opts.nameFieldCaption /* "Name" */  /* , 1 startFrom */  ); // Вот не факт, что они идут в том порядке, в котором я нашел их в какой-то версии альтиума
            continue;
        }

        if (nameIdx == (size_t)-1)
            continue;

        if (line.size() <= nameIdx )
            continue;

        if (line[designatorIdx]=="Designator")
            continue;

        if (line[0].empty())
        {
            if ( !validLines.empty() && validLines.back().size() > nameIdx )
            {
                validLines.back()[nameIdx].insert( validLines.back()[nameIdx].end(), line[nameIdx].begin(), line[nameIdx].end() );
            }
            continue;
        }
        else
        {
            validLines.push_back(line);
        }
    }

    //---

    for( auto line : validLines )
    {
        if (line.size()<nameIdx)
            continue;

        ComponentPinInfo pinInfo;

        //uint64_t pinNo = (unsigned)
        //pinInfo.pinNo = (unsigned)parseIntegerTolerant( line[designatorIdx], 10 );
        pinInfo.pinNo = line[designatorIdx];

        //startsWithAndStrip( line[nameIdx], "[*]" );
        //endsWithAndStrip( line[nameIdx], "[*]" );
        
        std::string strFunctions = line[nameIdx];
        strFunctions = replaceForbiddenChars( strFunctions, opts.invalChars, ' ');
        std:: vector<std::string> pinFunctionsTmp; pinFunctionsTmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
        splitToVector(strFunctions, pinFunctionsTmp, ' ');
        trim(pinFunctionsTmp);

        for(auto &pinFunc : pinFunctionsTmp)
        {
            pinFunc = filterPinNameNegation(pinFunc);
        }
        
        pinFunctionsTmp.erase( std::remove(pinFunctionsTmp.begin(), pinFunctionsTmp.end(), std::string() ), pinFunctionsTmp.end() );

        pinInfo.pinFunctions.insert(pinFunctionsTmp.begin(), pinFunctionsTmp.end());

        //pinInfo.pinFunctions.erase( std::remove( pinInfo.pinFunctions.begin(), pinInfo.pinFunctions.end(), std::string() ), pinInfo.pinFunctions.end() );
        //pinInfo.pinFunctions.erase( std::remove( pinInfo.pinFunctions.begin(), pinInfo.pinFunctions.end(), std::string("*") ), pinInfo.pinFunctions.end() );

        ci.pins[pinInfo.pinNo] = pinInfo;
    }

    //---

    ci.allTypeNames = opts.namesList;
    ci.description  = opts.description;
    ci.purpose      = opts.purpose;
    ci.datasheet    = opts.datasheet;
    ci.refman       = opts.refman;
    ci.errata       = opts.errata;
    ci.package      = (opts.package=="<ANY>" || opts.package=="*") ? std::string("*") : rbcOpts.packagesDb.getCanonicalPackageName( opts.package );
    ci.modules      = opts.modules;
    ci.headers      = opts.headers;

    return true;
}

//-----------------------------------------------------------------------------
inline
bool componentExportToCmp( std::ostream &os, ComponentInfo &ci )
{
    os<<"(componentDefinition";
    for( auto n : ci.allTypeNames)
    {
        os<<" \""<<n<<"\"";
    }
    os<<"\n";

    if (!ci.description.empty())
        os << "    (description \"" << ci.description << "\")\n";

    if (!ci.purpose.empty())
        os << "    (purpose \"" << ci.purpose << "\")\n";

    if (!ci.datasheet.empty())
        os << "    (datasheet \"" << ci.datasheet << "\")\n";

    if (!ci.refman.empty())
        os << "    (refman\"" << ci.refman << "\")\n";

    if (!ci.errata.empty())
        os << "    (errata \"" << ci.errata << "\")\n";

    if (!ci.package.empty())
        os << "    (package \"" << ci.package << "\")\n";


    if (!ci.modules.empty())
    {
        //os << "    (modules ( ";
        os << "    (modules";
        for( auto m : ci.modules)
        {
            //os<<"\""<<m<<"\" ";
            os<<" \""<<m<<"\"";
        }
        //os << ") )\n";
        os << ")\n";
    }
        
    if (!ci.headers.empty())
    {
        //os << "    (headers ( ";
        os << "    (headers";
        for( auto h : ci.headers)
        {
            //os<<"\""<<h<<"\" ";
            os<<" \""<<h<<"\"";
        }
        //os << ") )\n";
        os << ")\n";
    }

    for( auto p : ci.pins )
    {
        const ComponentPinInfo &pinInfo = p.second;
        os<<"    (pin \""<<pinInfo.pinNo<<"\" \""<<pinInfo.pinDescription<<"\" (";

        // 
        for( auto pf : pinInfo.pinFunctions )
        {
            os<<"\""<<pf<<"\" ";
        }
        os<<") )\n";
    }
        
/*
    if (!ci..empty())
        os << "    ( \"" << ci. << "\")"
*/

    os<<")\n";

    return true;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> splitComponentNamePart( const std::string &name, RoboconfOptions *pRbcOpts, bool keepSeps = true, bool splitAlsoDigits = true )
{

    enum CharClass{
        charClassInitial = 0,
        charClassAlpha   = 1,
        charClassDigit   = 2,
        charClassSep     = 3
    };

    CharClass prevCharClass = charClassInitial;

    std:: vector<std::string> res; res.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    std::string buf;
    char prevSep = 0;

    size_t i = 0, size = name.size();

    for(; i!=size; ++i)
    {
        char ch = name[i];

        CharClass  charClass = charClassInitial;

        if (ch>='a' && ch<='z')
            charClass = charClassAlpha;
        else if (ch>='A' && ch<='Z')
            charClass = charClassAlpha;
        else if (ch>='0' && ch<='9')
        {
            if (splitAlsoDigits)
                charClass = charClassDigit;
            else
                charClass = charClassAlpha;
        }
        else 
        {
            charClass = charClassSep;
            prevSep   = ch;
        }

        if (prevCharClass!=charClassSep && prevCharClass!=charClass && !buf.empty())
        {
            res.push_back(buf);
            buf.clear();
        }

        if (charClass==charClassSep)
        {
            if (!prevSep)
                prevSep = '-';
            if (keepSeps)
                buf.append(1,prevSep);
            //charClass = charClassAlpha;
        }
        else
        {
            buf.append(1,ch);
        }

        prevCharClass = charClass;

    }

    if (!buf.empty())
    {
        res.push_back(buf);
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> splitComponentName( const std::string &name, RoboconfOptions *pRbcOpts, bool keepSeps = true, bool splitAlsoDigits = true )
{
    std::vector<std::string> parts;
    splitToVector(name, parts, '-');

    std:: vector<std::string> resParts;

    bool bFirst = true;
    for(const auto &p : parts)
    {
        auto p_ = bFirst ? p : "-" + p;
        bFirst = false;

        if (pRbcOpts && pRbcOpts->packagesDb.isKnownPackage(p))
        {
            resParts.emplace_back(p_);
        }
        else
        {
            auto partParts = splitComponentNamePart(p_, pRbcOpts, keepSeps, splitAlsoDigits);
            for(const auto &pp : partParts)
            {
                resParts.emplace_back(pp);
            }
        }
    }

    return resParts;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> generateComponentNamePartSingleX( const std::string &name, char ch = '_' )
{
    std:: vector<std::string> res; res.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    res.push_back( name );

    std::string::size_type i = 0, size = name.size();
    
    for(; i!=size; ++i)
    {
        if (name[i]!='-')
            break;
    }

    if (i==size)
        return res;

    for(; i!=size; ++i)
    {
        //if (name[size-1-i]=='-')
        //    continue;
        std::string tmp = name;
        tmp[size-1-i] = ch;
        res.push_back(tmp);
    }
    return res;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> generateComponentNamePartXXX( std::string name, char ch = '_')
{
    //std::string nameBak = name;

    //std:: vector<std::string> tmp = generateComponentNamePartSingleX(name);

    std:: vector<std::string> res; res.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    name = toUpper( name );

    size_t i = 0, size = name.size();

    if (size==0)
        return res;

    if (size>15)
        size = 15;

    size_t maxVal = 1 << size;
    for(; i!=maxVal; ++i)
    {
        std::string tmpName = name;
        size_t pos = size - 1;
        size_t j = i;
        while( j )
        {
            if ( j & 1 )
                tmpName[pos] = '_';
            j >>= 1;
            pos--;
        }

        if (res.empty())
            res.reserve(maxVal);

        res.emplace_back(tmpName);
    }

    std::sort( res.begin(), res.end() );

    return res;
    
/*
    std::string::size_type i = 0, size = name.size();
    
    for(; i!=size; ++i)
    {
        if (name[i]!='-')
            break;
    }

    if (i==size)
    {
        //res.push_back( nameBak );
        return res;
    }

    //++i; // allow single sign at begininng to be not masked

    for(; i!=size; ++i)
    {
        //if (name[size-1-i]=='-')
        //    continue;
        name[size-1-i  ] = ch; // +1
        res.push_back(name);
    }

    return res;
*/
}

//-----------------------------------------------------------------------------
// which returns ​true if the first argument is less than (i.e. is ordered before) the second. 
inline
bool stringLessByLen( const std::string &s1, const std::string &s2 )
{
    return s1.size() < s2.size();
}

//-----------------------------------------------------------------------------
inline
bool stringGreateByLen( const std::string &s1, const std::string &s2 )
{
    return s1.size() > s2.size();
}



//-----------------------------------------------------------------------------
inline
std:: vector<std::string> mergeComponentNameXXX( const std::string &prefix, const std:: vector< std:: vector<std::string> > &alts, size_t idx )
{
    if (idx>=alts.size())
        return std:: vector<std::string>();

    const std:: vector<std::string> &vec = alts[idx];
    std:: vector<std::string> res;

    for(  auto s : vec )
    {
        std::string curPrefix = prefix + s;
        auto tmp = mergeComponentNameXXX( curPrefix, alts, idx+1 );
        res.reserve( res.size() + tmp.size() );
        res.insert( res.end(), tmp.begin(), tmp.end() );
        res.emplace_back(curPrefix);
    }

    return res;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> mergeComponentNameXXX( const std:: vector< std:: vector<std::string> > &alts )
{
    return mergeComponentNameXXX( std::string(), alts, 0 );
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> generateComponentNamesSingleLen(  RoboconfOptions &rbcOpts, const std::string &cmpName, const std::string &cmpPackage, const std::string &cmpUserSuffix )
{
    std:: vector<std::string> nameParts = splitComponentName( cmpName, &rbcOpts, true /* keepSeps */ , true /* splitAlsoDigits */ ); //TODO: !!! Добавил явно умолчательные аргументы. Надо разобраться, а точно ли тут нужно отделять цифры?  Вообще, они были такими всегда

    std:: vector< std:: vector< std::string > > namePartsXXX; namePartsXXX.reserve(16);
    namePartsXXX.reserve(nameParts.capacity());

    for( std:: vector<std::string>::const_iterator it = nameParts.begin(); it != nameParts.end(); ++it )
    {
        //std:: vector< std::string > partSetX;
        //if (it == nameParts.begin())
        //{
        //    partSetX.push_back(*it);
        //}
        //else
        //{
            std::string partName = *it;
            std:: vector<std::string> xxx = generateComponentNamePartXXX( partName, '_' );
            //partSetX.insert(partSetX.end(), xxx.begin(), xxx.end());
            /*
            while( !partName.empty() && partName!="-" )
            {
                std:: vector<std::string> xxx = generateComponentNamePartXXX( partName, '_' );
                partSetX.insert(partSetX.end(), xxx.begin(), xxx.end());
                partName.erase( partName.size() - 1 );
            }
            */
        //}
        //namePartsXXX.push_back(partSetX);
        //namePartsXXX.push_back(xxx);
        namePartsXXX.emplace_back(xxx);
    }

    std:: vector<std::string> lookupNames = mergeComponentNameXXX( namePartsXXX );
    makeUniqueVector( lookupNames );
    //std::stable_sort( lookupNames.begin(), lookupNames.end(), stringGreateByLen );

    //if (cmpPackage.empty())
    //    return lookupNames;

    std:: vector<std::string> res;
    
    size_t resReserve = lookupNames.size();
    if (!cmpPackage.empty())
        resReserve *= 2;
    if (!cmpUserSuffix.empty())
        resReserve *= 2;

    res.reserve(resReserve);

    for( auto n : lookupNames )
    {
        if (!cmpPackage.empty())
        {
            if (!cmpUserSuffix.empty())
            {
                res.emplace_back( n + std::string("-") + cmpPackage );
                res.emplace_back( n + std::string("-") + cmpPackage + std::string("-") + cmpUserSuffix );
                res.emplace_back( n );
                res.emplace_back( n + std::string("-") + cmpUserSuffix );
            }
            else
            {
                res.emplace_back( n + std::string("-") + cmpPackage );
                res.emplace_back( n );
            }
        }
        else // cmpPackage - empty
        {
            if (!cmpUserSuffix.empty())
            {
                res.emplace_back( n );
                res.emplace_back( n + std::string("-") + cmpUserSuffix );
            }
            else
            {
                res.emplace_back( n );
            }
        }
    }
    
    return res;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> generateComponentNamesHelper( std::set<std::string> &usedNames, std:: vector<std::string> names, const std:: vector<std::string> &suffixes )
{
    for( const auto& sfx : suffixes )
    {
        std:: vector<std::string> tmp; tmp.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

        for( const auto &n : names )
        {
            //if (usedNames.find(n)==usedNames.end())
            {
                tmp.emplace_back(n);
            //    usedNames.insert(n);
            }

            std::string newName = n;
            if (!sfx.empty())
                newName += std::string("-") + sfx;

            //if (usedNames.find(newName)==usedNames.end())
            {
                tmp.emplace_back(newName);
            //    usedNames.insert(newName);
            }
        }

        names.swap(tmp);
    }

    //std:: vector<std::string> tmp;
    makeUniqueVector(names);

    return names;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> generateComponentNamesHelper( const std:: vector<std::string> &names, const std:: vector<std::string> &suffixes )
{
    std::set<std::string> usedNames;
    return generateComponentNamesHelper( usedNames, names, suffixes );
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> generateComponentNamesHelper( const std::string &name, const std:: vector<std::string> &suffixes )
{
    std:: vector< std::string > names; names.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    names.emplace_back(name);
    return generateComponentNamesHelper( names, suffixes );
}

//-----------------------------------------------------------------------------
// Минимальная длина имени компонента - это такая длина, когда много левого не будет обнаруживаться
// Пока идут латинские буквы - нормас, потом обычно числовой индекс и потом дальше что угодно может быть
// Так вот, по минимум это буквенный префикс + одна цифра
inline
std::size_t calcComponentMinimalName(std::string name)
{
    std::size_t minLen = 0;
    for(auto ch: name)
    {
        if ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'))
        {
            ++minLen;
        }
        else
        {
            break;
        }
    }

    if (minLen<name.size())
       ++minLen;

    return minLen;
}

//-----------------------------------------------------------------------------
inline
std:: vector<std::string> generateComponentNames( RoboconfOptions &rbcOpts, std::string cmpName, std::string cmpPackage, const std::string &cmpUserSuffix )
{
    if (cmpName.empty())
        return std:: vector<std::string>();

    //cmpName = normalizeComponentNameBasic(cmpName);
    {
        std::string cmpPackage2;
        cmpName = normalizeComponentName(rbcOpts, cmpName, &cmpPackage2);
        if (!cmpPackage2.empty() && cmpPackage.empty())
            cmpPackage = cmpPackage2;
    }

    std:: vector<std::string> suffixes; suffixes.reserve(16);

    //bool componentNameIncludesPackage = false;

    std::string::size_type minLen = calcComponentMinimalName(cmpName);
    if (!minLen)
        minLen = 1;


    suffixes.emplace_back(cmpPackage);

    if (!cmpUserSuffix.empty())
        suffixes.emplace_back(cmpUserSuffix);

    std:: vector< std::string > resNames; resNames.reserve(1024);

    std::string tmpName = cmpName;
    while(tmpName.size() >= minLen)
    {
        std:: vector<std::string> names = generateComponentNamesHelper( tmpName, suffixes );
        resNames.insert( resNames.end(), names.begin(), names.end() );
        tmpName.erase( tmpName.size()-1, 1 );
    }

    return resNames;

}


//-----------------------------------------------------------------------------
bool componentParseList    ( RoboconfOptions &rbcOpts, const expression_list_t &lst, ComponentInfo &ci);
bool componentParseRoboconf( RoboconfOptions &rbcOpts, const std:: vector<std::string> &lines, std:: vector< ComponentInfo > &components );
/*
//-----------------------------------------------------------------------------
bool componentsMerge( std:: vector< ComponentInfo > &components, const ComponentInfo &ci )
{
    unsigned mergeCount = 0;
    auto cit = components.begin();
    for(; cit != components.end(); ++cit)
    {
        if (!cit->isPackageCompatible(ci))
            continue;
        cit->merge(ci);
        mergeCount++;
    }

    if (!mergeCount)
        components.emplace_back(ci);

    return true;
}

//-----------------------------------------------------------------------------
bool componentsMerge( std:: map<std::string, std:: vector< ComponentInfo > > &components, ComponentInfo ci, const std::string &typeName )
{
    ci.typeName = typeName;

    auto cit = components.find(typeName);
    if (cit == components.end())
    {
        components[typeName].emplace_back(ci);
        return true;
    }

    return componentsMerge( cit->second, ci );
}
*/
//-----------------------------------------------------------------------------
inline
bool componentReadCmp(RoboconfOptions &rbcOpts, FileSet::file_id_t fileNo, std::istream &in, std:: vector< ComponentInfo > &components )
{

    NetlistSrcType netlistType = NetlistSrcType::netlistSrcType_Unknown;

    std::string allText = readFileEncoded /* Safe */ ( in /*, std::string srcEnc = std::string(), std::string targetEnc = "UTF-8", std::string httpHint = std::string(), std::string metaHint = std::string() */ );
    std::string allTextTrimmed = allText; ltrim(allTextTrimmed);

    std:: vector< std::string > lines; lines.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);

    if (startsWith( allTextTrimmed, "(componentDefinition" ) || startsWith( allTextTrimmed, "(--" ))
        netlistType = NetlistSrcType::netlistSrcType_ACCEL_ASCII;
    else if (startsWith( allTextTrimmed, "#!roboconf component"))
    {
        netlistType = NetlistSrcType::netlistSrcType_Cache;
        splitToVector( allText, lines, '\n' );
        rtrim(lines);
    }

    switch(netlistType)
    {
	    case NetlistSrcType::netlistSrcType_ACCEL_ASCII:
             {
                 expression_list_t netList;
				 size_t lineNo = 0;
                 if (!readList(fileNo, lineNo, allText, netList ))
                 {
                     LOG_ERR<<"failed to parse netlist\n";
                     return false;
                 }

                 expression_list_t::const_iterator nlIt = netList.begin();
                 for(; nlIt != netList.end(); ++nlIt)
                 {
                     if (!nlIt->itemText.empty())
                     {
                     }
                     else
                     {
                         if (nlIt->getHeadText()=="componentDefinition")
                         {
                             ComponentInfo              ci;
                             if (componentParseList(rbcOpts, nlIt->itemList, ci ))
                             {
                                 for( auto name : ci.allTypeNames )
                                 {
                                     //componentsMerge( components, ci, name );
                                     ci.typeName = name;
                                     components.emplace_back( ci );
                                 }
                             }
                         }
                     }
                 } // for(; nlIt != netList.end(); ++nlIt)
                 return true;
             }

	   case NetlistSrcType::netlistSrcType_Cache:
             {
                 componentParseRoboconf(rbcOpts, lines, components );
             }

        default: return false;
       }

    return true;

}

//-----------------------------------------------------------------------------
inline
std::string pinFunctionNameWithoutNegation( std::string fnName )
{
    if (!fnName.empty() && fnName[0]=='~')
        return std::string( fnName, 1, std::string::npos );

    return fnName;
}

//-----------------------------------------------------------------------------
inline
std:: vector< std::string > pinFunctionNameSplit( const std::string &pinFunction, bool removeNegations = true )
{
    std:: vector<std::string> vec; vec.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
    splitToVector( pinFunction, vec, "_" );
    if (removeNegations)
    {
        for( auto & v : vec )
        {
            v = pinFunctionNameWithoutNegation(v);
        }
    }

    return vec;
}

//-----------------------------------------------------------------------------
inline
bool pinFunctionIsContain( const std::string &longPinFunction, const std::string &pinFunctionFind )
{
    auto pfnLongParts = pinFunctionNameSplit( longPinFunction, true  /* removeNegations */ ); 
    return std::find( pfnLongParts.begin(), pfnLongParts.end(), pinFunctionNameWithoutNegation(pinFunctionFind) ) != pfnLongParts.end();
}

//-----------------------------------------------------------------------------
inline
bool pinFunctionIsContain( const std::string &longPinFunction, const ComponentPinInfo::set_type &pinFunctionsFind )
{
    auto pfnLongParts = pinFunctionNameSplit( toUpper(longPinFunction), true  /* removeNegations */ ); 

    for( auto findName : pinFunctionsFind )
    {
        if ( std::find( pfnLongParts.begin(), pfnLongParts.end(), toUpper(pinFunctionNameWithoutNegation(findName)) ) != pfnLongParts.end() )
            return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
inline
bool pinFunctionIsContain( const std::set<std::string> &longPinFunction, const ComponentPinInfo::set_type &pinFunctionsFind )
{
    for( auto longName : longPinFunction )
    {
        if ( pinFunctionIsContain( longName, pinFunctionsFind ))
            return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
inline
std::string filterPinNameNegation( std::string str )
{
    bool hasTilde = false;

    std::string res;
    size_t bslCnt = 0;
    for( auto ch : str )
    {
        if (ch=='\\')
           bslCnt++;
        else 
        {
            if (ch=='~')
                hasTilde = true;
            res.append(1,ch);
        }
    }

    //int delta = ((int)res.size()) - ((int)bslCnt);
    //if (delta<0)
    //   delta = -delta;

    if (bslCnt)
    {
        return hasTilde ? res : std::string("~") + res;
    }

    std::string regexRes;

    if ( regexpEvalString( regexRes, "n_([A-Z].*)", "~$({1})", res, string_string_map_type()) )
       return regexRes;

    if ( regexpEvalString( regexRes, "n([A-Z].*)", "~$({1})", res, string_string_map_type()) )
       return regexRes;

    if ( regexpEvalString( regexRes, "(.*)_n", "~$({1})", res, string_string_map_type()) )
       return regexRes;

    if ( regexpEvalString( regexRes, "(.*)n", "~$({1})", res, string_string_map_type()) )
       return regexRes;

    return res;
}

enum class ComponentCmpListToken
{
    tokenUnknown,
    tokenComment,
    tokenKeywordAny,
    tokenNameMatch,
    tokenRegex,
    tokenSimple,
    tokenExact,
    tokenSingleChar, // single & singlechar
    tokenAnyChar   , // any & anyChar
    tokenDescription,
    tokenPurpose    ,
    tokenAssembly   ,
    tokenDatasheet  ,
    tokenRefman     ,
    tokenErrata     ,
    tokenPackage    ,
    tokenModules    ,
    tokenHeaders    ,
    tokenPin        ,
    tokenPartPins   ,
    tokenInternalNet,
    tokenPinFunctionMatch
}; // ComponentCmpListToken

//-----------------------------------------------------------------------------
#if defined(ROBOCONF_COMPONENT_CMP_LIST_TOKENS_USE_UNORDERED_MAP)
    using ComponentCmpListTokenMap = std::unordered_map< std::string , ComponentCmpListToken >;
#else
    using ComponentCmpListTokenMap = std:: map< std::string , ComponentCmpListToken >;
#endif

//-----------------------------------------------------------------------------
inline
ComponentCmpListTokenMap makeComponentCmpListTokens()
{
    ComponentCmpListTokenMap m;
    m["--"              ] = ComponentCmpListToken::tokenComment ;
    m["//"              ] = ComponentCmpListToken::tokenComment ;
    m["<any>"           ] = ComponentCmpListToken::tokenKeywordAny  ;
    m["*"               ] = ComponentCmpListToken::tokenKeywordAny  ;
    m["namematch"       ] = ComponentCmpListToken::tokenNameMatch ;
    m["regex"           ] = ComponentCmpListToken::tokenRegex ;
    m["simple"          ] = ComponentCmpListToken::tokenSimple ;
    m["exact"           ] = ComponentCmpListToken::tokenExact ;
    m["single"          ] = ComponentCmpListToken::tokenSingleChar ;
    m["singlechar"      ] = ComponentCmpListToken::tokenSingleChar ;
    m["any"             ] = ComponentCmpListToken::tokenAnyChar ;
    m["anychar"         ] = ComponentCmpListToken::tokenAnyChar ;
    m["description"     ] = ComponentCmpListToken::tokenDescription ;
    m["purpose"         ] = ComponentCmpListToken::tokenPurpose     ;
    m["assembly"        ] = ComponentCmpListToken::tokenAssembly    ;
    m["datasheet"       ] = ComponentCmpListToken::tokenDatasheet   ;
    m["refman"          ] = ComponentCmpListToken::tokenRefman      ;
    m["errata"          ] = ComponentCmpListToken::tokenErrata      ;
    m["package"         ] = ComponentCmpListToken::tokenPackage     ;
    m["modules"         ] = ComponentCmpListToken::tokenModules     ;
    m["headers"         ] = ComponentCmpListToken::tokenHeaders     ;
    m["pin"             ] = ComponentCmpListToken::tokenPin         ;
    m["partpins"        ] = ComponentCmpListToken::tokenPartPins    ;
    m["internalnet"     ] = ComponentCmpListToken::tokenInternalNet ;
    m["pinfunctionmatch"] = ComponentCmpListToken::tokenPinFunctionMatch ;
    //m[""] = ComponentCmpListToken:: ;

    return m;
}

//-----------------------------------------------------------------------------
inline
const ComponentCmpListTokenMap& getComponentCmpListTokens()
{
    static ComponentCmpListTokenMap m = makeComponentCmpListTokens();
    return m;
}

//-----------------------------------------------------------------------------
inline
ComponentCmpListToken componentParseListGetToken( const std::string &name )
{
    const ComponentCmpListTokenMap &m = getComponentCmpListTokens();

    typename ComponentCmpListTokenMap::const_iterator mit = m.find(toLower(name));
    if (mit==m.end())
        return ComponentCmpListToken::tokenUnknown;

    return mit->second;
}

//-----------------------------------------------------------------------------
inline
bool componentParseList    (RoboconfOptions &rbcOpts, const expression_list_t &lst, ComponentInfo &ci )
{
    expression_list_t::const_iterator it = lst.begin();

    std::string itemText;
    if (!readListTextItem( lst, it, itemText ))
       return false; // failed to get list type

    if (itemText!="componentDefinition")
       return false;

    std::string str;
    while(readListTextItem( lst, it, str, true ))
    {
        //ci.allTypeNames.push_back( toUpper(str) );
        ci.allTypeNames.push_back( str );
    }

    if (ci.allTypeNames.empty())
    {
        LOG_ERR_OBJ(lst)<<"Component name(s) not taken\n";
        return false;
    }

    for( ; it != lst.end(); ++it)
    {
        if (!it->isList())
        {
            LOG_ERR_OBJ(lst)<<"ListItem: not a sublist (text: '"<<it->itemText<<"'), something wrong!!!\n";
            continue;
        }

        const expression_list_t &subList = it->itemList;
        expression_list_t::const_iterator subIt = subList.begin();

        std::string sublistHeader;

        if (!readListTextItem( subList, subIt, sublistHeader, true ))
            continue;
        //std::string sublistHeader                = nlIt->getHeadText();
        //const expression_list_t &subList = it->itemList;

        ComponentCmpListToken sublistHeaderToken = componentParseListGetToken( sublistHeader );

        if (sublistHeaderToken==ComponentCmpListToken::tokenComment)
        {
            // skip comment
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenNameMatch)
        {
            std::string nameMatch;
            if (readListTextItem( subList, subIt, nameMatch, true ))
            {
                ComponentCmpListToken nameMatchToken = componentParseListGetToken( nameMatch );
                //nameMatch = toLower( nameMatch );
                if (nameMatchToken==ComponentCmpListToken::tokenRegex)
                {
                    ci.typeNameMatchType = ComponentTypeNameMatchType::matchRegex;
                }
                else if (nameMatchToken==ComponentCmpListToken::tokenSimple)
                {
                    ci.typeNameMatchType = ComponentTypeNameMatchType::matchSimple;
                    ci.singleCharMask    = 'x';
                    ci.anyCharMask       = '*';
                    // (nameMatch simple (single "x")(any "*"))

                    while (subIt!=subList.end() && subIt->isList())
                    {
                        const expression_list_t &matchOptionsList       = subIt->itemList;
                        expression_list_t::const_iterator matchOptionIt = matchOptionsList.begin();
                        std::string matchOption, matchChar;
                        if (matchOptionIt!=matchOptionsList.end() && readListTextItem( matchOptionsList, matchOptionIt, matchOption, true ))
                        {
                            if (matchOptionIt!=matchOptionsList.end() && readListTextItem( matchOptionsList, matchOptionIt, matchChar, true ))
                            {
                                if (!matchChar.empty())
                                {
                                    ComponentCmpListToken matchOptionToken = componentParseListGetToken(matchOption);
                                    //matchOption = toLower(matchOption);
                                    if (matchOptionToken==ComponentCmpListToken::tokenSingleChar)
                                        ci.singleCharMask = matchChar[0];
                                    else if (matchOptionToken==ComponentCmpListToken::tokenAnyChar)
                                        ci.anyCharMask    = matchChar[0];
                                }
                            }
                        }

                        ++subIt;
                    }
                }
                else // exact
                {
                    ci.typeNameMatchType = ComponentTypeNameMatchType::matchExact;
                }
            }
                
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenDescription)
        {
            if (readListTextItem( subList, subIt, str, true ))
                ci.description = str;
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenPurpose)
        {
            if (readListTextItem( subList, subIt, str, true ))
                ci.purpose = str;
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenAssembly)
        {
            if (readListTextItem( subList, subIt, str, true ))
            {
                ci.assembly = (unsigned)parseIntegerTolerant( str, 10 );
            }
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenDatasheet)
        {
            if (readListTextItem( subList, subIt, str, true ))
                ci.datasheet = str;
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenRefman)
        {
            if (readListTextItem( subList, subIt, str, true ))
                ci.refman = str;
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenErrata)
        {
            if (readListTextItem( subList, subIt, str, true ))
                ci.errata = str;
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenPackage)
        {
            if (readListTextItem( subList, subIt, str, true ))
            {
                ComponentCmpListToken packageToken = componentParseListGetToken( str );
                if (packageToken == ComponentCmpListToken::tokenKeywordAny)
                {
                    ci.package = "*";
                }
                else
                {
                    ci.package = rbcOpts.packagesDb.getCanonicalPackageName(str); //  toUpper(filterForValidName( str, ComponentPackageNameCharValidator() ));
                }
            }
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenModules)
        {
            while(readListTextItem( subList, subIt, str, true ))
            {
                ci.modules.push_back(str);
            }
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenHeaders)
        {
            while(readListTextItem( subList, subIt, str, true ))
            {
                ci.headers.push_back(str);
            }
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenPin)
        {
            ComponentPinInfo pinInfo;
            if (!readListTextItem( subList, subIt, str, true ))
                continue;
            pinInfo.pinNo = str; // (unsigned)parseIntegerTolerant( str, 10 );

            if (!subIt->isList())
            {
                readListTextItem( subList, subIt, pinInfo.pinDescription, true );
            }

            if (readListTextItem( subList, subIt, str, true ))
            {
                pinInfo.pinFunctions.insert(filterPinNameNegation(str));
            }
            else
            {
                const expression_list_t &pinList = subIt->itemList;
                expression_list_t::const_iterator pinIt = pinList.begin();
                while(readListTextItem(pinList, pinIt, str, true ))
                {
                    pinInfo.pinFunctions.insert(filterPinNameNegation(str));
                }
            }
            
            //makeUniqueVector(pinInfo.pinFunctions);

            ci.pins[pinInfo.pinNo] = pinInfo;
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenPartPins)
        {
            std::string str;
            ComponentInfo::part_pins_t partPins;
            while(readListTextItem( subList, subIt, str, true ))
            {
                //partPins.insert( (unsigned)parseIntegerTolerant( str, 10 ) );
                partPins.insert( str );
            }

            ci.addPinsPart(partPins);
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenInternalNet)
        {
            //(internalNet "1" "2")
            ComponentInfo::internal_net_t net;
            while(readListTextItem( subList, subIt, str, true ))
            {
                //net.push_back( (unsigned)parseIntegerTolerant( str, 10 ) );
                net.push_back( str );
            }

            ci.internalNets.push_back( net );
        }
        else if (sublistHeaderToken==ComponentCmpListToken::tokenPinFunctionMatch)
        {
            // (pinFunctionMatch "JTDO"       ("RBC_EXCLUDE") )
            ComponentInfo::pin_function_match_t match;
            if (readListTextItem( subList, subIt, str, true ))
            {
                match.first = str;
                if (subIt->isList())
                {
                    readListToVector( subIt->itemList, match.second, true );
                }
            }
            ci.addPinFunctionMatch(match);
        }
    }

    return true;
}

inline
bool componentParseRoboconf( RoboconfOptions &rbcOpts, const std:: vector<std::string> &lines, std:: vector< ComponentInfo > &components )
{
    return false;
}


