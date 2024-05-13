#pragma once

#include <set>
#include <sstream>
#include <iomanip>

#include "utils.h"
#include "textUtils.h"

#include "prog.h"
#include "fsUtils.h"


//#include "igen.h"


struct ITypeGen;
struct TypeInfo;
struct ITypeGen;
class  TypeMap;
struct DeviceInfo;
struct GeneratorOptions;
struct NsIndecesInfo;
struct NamespaceRegs;


#include "itypegen.h"


#define REG_VISIBILITY_UNSET       0
#define REG_VISIBILITY_PUBLIC      1
#define REG_VISIBILITY_PROTECTED   2
#define REG_VISIBILITY_PRIVATE     3

#define REG_SYNC_UNSET             0
#define REG_SYNC_NONE              1
#define REG_SYNC_ALWAYS            2
#define REG_SYNC_CONDITION         3


//-----------------------------------------------------------------------------
// utility functions
inline
void splitMultiAttrsString( const std::string & str, std::vector<std::string> &res )
{
    std::vector<std::string> tmp;
    splitToVector( str, tmp, ';' );
    for( const auto &t : tmp )
    {
        std::vector<std::string> tmp2;
        splitToVector( t, tmp2, ',' );
        res.insert( res.end(), tmp2.begin(), tmp2.end() );
    }
}

//-----------------------------------------------------------------------------
struct RegImportOptions
{
    int regVisibility         = REG_VISIBILITY_UNSET;
    int regVisibilityModifier = REG_VISIBILITY_UNSET;
    int regVisibilityAssigner = REG_VISIBILITY_UNSET;
    int regSync               = REG_SYNC_UNSET;
    std::string               syncCondition;
    std::set<std::string>     tags;

    // used only in 'import' directive
    std::set<std::string> tagsPlus;
    std::set<std::string> tagsMinus;

    bool operator==( const RegImportOptions &rio ) const
    {
        if ( regVisibility != rio.regVisibility )
            return false;

        if ( regVisibilityModifier != rio.regVisibilityModifier )
            return false;

        if ( regVisibilityAssigner != rio.regVisibilityAssigner )
            return false;

        if ( regSync != rio.regSync )
            return false;

        if ( syncCondition != rio.syncCondition )
            return false;

        return true;
    }

    bool operator!=( const RegImportOptions &rio ) const
    {
        return !operator==( rio );
    }

}; // struct RegImportOptions


struct NsIndecesInfo
{
    unsigned         startIdx;
    unsigned         numIdx  ;
    std::string      oppositeNs;

    NsIndecesInfo( unsigned s = 0, unsigned n = 128, const std::string &opo = "") : startIdx(s), numIdx(n), oppositeNs(opo) {}
};



struct GeneratorOptions
{
    bool         bSort           = false;
    //bool         idxStep1        = false;
    unsigned     protocolVersion       = 1;
    bool         mbRegsFilling         = false; // generate filling values for multibyte registers
    bool         mbRegsOnlyFilling     = false;
    bool         nsReverseOrder        = false;
    bool         forceAllTypes         = false;
    bool         onlyTypes             = false;
    bool         noTypes               = false;
    bool         decimalIndexComment   = false;
    bool         orgValue              = false;
    bool         orgType               = false;
    bool         generateMeta          = false;
    bool         generateReadonly      = true;
    //bool         generateAttrs         = false; // for RDL output
    std::vector<std::string>             nsOrder;

    mutable std::map<std::string, NsIndecesInfo>      nsStartIndeces;
    std::map< std::string, ICaseConverter* >  caseConverters;
    std::map< std::string, std::string >      elementTypesCase; // enum->XXX, class->XXX, ns->XXX, id->XXX
    std::set<std::string>                     disabledWarnings;
    std::map<std::string, std::string>        typeGeneratorNames;   // from command line: user_type -> generator_name
    std::map<std::string, std::map<std::string, ITypeGen* > >  typeGenerators;   // output_type -> { type_generator_name -> type_generator }
    //std::vector<std::string>                  knownLangGenerators;

    std::map< std::string, std::string >      targetOptions;

    std::string defaultTypeGenerator = "none";

    std::string topNamespace; // = "regs";
    std::string slavesNamespace; // = "regs";
    std::string typesNamespace;
    //std::string slavesNamespace;
    std::string slavesBase;
    bool        typesInDeviceNamespace = false;

    std::string outputType;

    std::string outputProlog;
    std::string outputEpilog;

    std::string outputFileName;


    NsIndecesInfo getNamespaceIndecesInfo( const NamespaceRegs &nsRegs ) const;

    //GeneratorOptions
    size_t getRegNumberIncrementValue( const TypeInfo ti ) const;

    std::vector<std::string> getLangGenerators() const
    {
        std::vector<std::string> res;
        //std::map<std::string, std::map<std::string, ITypeGen* > >  typeGenerators;
        for( const auto &it : typeGenerators )
        {
            res.push_back(it.first);
        }

        return res;
    }

    std::string getExePath() const
    {
        std::string exeFullName = getProgExeName();
        if (!exeFullName.empty())
        {
            return getPath( exeFullName );
        }

        return std::string();
    }

    std::string getInstallRootPath() const
    {
        std::string progBinPath = getExePath();
        bool isExeFolderBin = false;
        if (!progBinPath.empty())
        {
            std::string exeFolder = toLower(getFileName( progBinPath ));
            if (exeFolder=="bin" || exeFolder=="debug" || exeFolder=="release")
                isExeFolderBin = true;
        }

        if (isExeFolderBin)
        {
            return getPath( progBinPath );
        }
        else
        {
            return progBinPath;
        }
    }

    std::string getContribRootPath() const
    {
        std::string installRootPath = getInstallRootPath();
        if (!installRootPath.empty())
        {
            installRootPath = appendPath(installRootPath, "contrib");
        }
        else
        {
            installRootPath = "contrib";
        }
        return installRootPath;
    }


    bool readFile( std::string fileName, std::string &readTo )
    {
        readTo.clear();
        std::string strReaded;

        std::ifstream input( fileName );
        if (!input)
        {
            GeneratorOptions &gopts = *this;
            LOG_ERR_OPT<<"Failed to read prolog/epilog file '"<<fileName<<"'\n";
            return false;
        }

        while( std::getline( input, strReaded ) )
        {
            if (!readTo.empty())
                readTo.append(1, '\n');
            readTo.append(strReaded);
        }

        return true;
    }

    bool readPrologEpilog( std::string fileName, std::string &readTo )
    {
        if (startsWithAndStrip( fileName, "inline:" ))
        {
            readTo = parseEscapes(fileName);
            return true;
        }
        else
        {
            return readFile( fileName, readTo );
        }
    }
    
    bool readProlog( std::string fileName )
    {
        return readPrologEpilog( fileName, outputProlog );
    }

    bool readEpilog( std::string fileName )
    {
        return readPrologEpilog( fileName, outputEpilog );
    }


    ITypeGen* getTypeGen( std::string typeName, const TypeMap &typeMap ) const;

    bool allowMbRegsFilling() const
    {
        return protocolVersion==1 ? mbRegsFilling : false;
    }

    bool isMbRegsOnlyFilling() const
    {
        if (!allowMbRegsFilling())
            return false;
        return mbRegsOnlyFilling;
    }

    bool allowWarn(std::string warnType) const
    {
        if (disabledWarnings.find("all")!=disabledWarnings.end())
            return false; // найден all, запрещаем все варнинги
        return disabledWarnings.find(warnType)==disabledWarnings.end();
    }

    bool isValidConverter( std::string cvtName ) const
    {
        if (caseConverters.find(cvtName)==caseConverters.end())
            return false;
        return true;
    }

    std::string caseConvert( std::string elemType, std::string s ) const
    {
        s = makeCppName(s);

        auto cvtNameIt = elementTypesCase.find(elemType); 
        if (cvtNameIt==elementTypesCase.end())
            return s; // unknown element, return untouched

        auto cvtIt = caseConverters.find(cvtNameIt->second);
        if (cvtIt == caseConverters.end())
            return s; // unknown converter, return untouched

        return cvtIt->second->convertCase( s );
    }

    std::string  makeEnumName( std::string s ) const
    {
        return caseConvert("enum", s);
    }

    std::string  makeMethodName( std::string s ) const
    {
        return caseConvert("method", s);
    }

    std::string  makeClassName( std::string s ) const
    {
        return caseConvert("class", s);
    }

    std::string  makeNsName( std::string s ) const
    {
        return caseConvert("ns", s);
    }

    std::string  makeIdName( std::string s ) const
    {
        return caseConvert("id", s);
    }

    std::string  makeValName( std::string s ) const
    {
        return caseConvert("val", s);
    }

    std::string  makeVarName( std::string s ) const
    {
        return caseConvert("var", s);
    }

    std::string  makeFileName( std::string s ) const
    {
        return caseConvert("file", s);
    }

    std::string  makeDeviceName( std::string s ) const
    {
        return caseConvert("device", s);
    }

    std::string  makeRegsNsName( std::string s ) const
    {
        return caseConvert("regsns", s);
    }

/*
    std::string makeRegistersScopeName( std::string s, bool useStruct )
    {
        if (useStruct)
            return makeClassName(s);
        else
            return makeNsName(s);
    }
*/
};


struct Registry
{
    std::string regType;
    std::string regDescription;
    unsigned    regIdx;          // regNsOffset + NsStartIdx
    unsigned    regNsOffset;     // clean value 
    unsigned    regMasterOffset = (unsigned)-1;
    unsigned    regMasterIdx    = (unsigned)-1;

    unsigned    regGroup;

    RegImportOptions  importOptions;

    std::string slaveDeviceName; // as
    std::string slaveRegOrgName;

    std::string linkedReg; // linked to this name

    bool        firstImported = false;


    Registry( std::string t = ""
            , std::string descr = ""
            , unsigned grp = 0
            , RegImportOptions importOps = RegImportOptions()
            , std::string slave = ""
            , std::string regOrgName = ""
            , std::string linkedReg = ""
            
            )
       : regType(t)
       , regDescription(descr)
       , regIdx(0)
       , regNsOffset(0)
       , regMasterOffset(0)
       , regGroup(grp)
       , importOptions(importOps)
       , slaveDeviceName(slave)
       , slaveRegOrgName(regOrgName)
       , linkedReg(linkedReg)
       {}

    bool operator==( const Registry &r ) const
    {
        if ( regType != r.regType )
            return false;

        if ( regDescription != r.regDescription )
            return false;

        //if ( regIdx != r.regIdx )
        //    return false;

        //if ( regNsOffset != r.regNsOffset )
        //    return false;

        if ( regGroup != r.regGroup )
            return false;

        if ( importOptions != r.importOptions )
            return false;

        return true;
    }

    bool operator!=( const Registry &r ) const
    {
        return !operator==( r );
    }


}; // struct Registry


struct NamespaceRegs
{
    std::string                       nsName;
    std::map< std::string, Registry>  nsRegs;
    std::vector<std::string>          regOrder;

    std::vector< size_t >             masterBases;
    std::vector< std::string >        masterBaseNames;

    unsigned                          regIdxBase;
    unsigned                          minUsedRegIdx;
    unsigned                          maxUsedRegIdx;

    RegImportOptions                  importOptions;

    bool operator==( const NamespaceRegs &r ) const
    {
        if ( nsName != r.nsName )
            return false;

        if ( !isEqual( nsRegs, r.nsRegs ) )
            return false;

        if ( !isEqual( regOrder, r.regOrder ) )
            return false;

        return true;
    }

    bool operator!=( const NamespaceRegs &r ) const
    {
        return !operator==( r );
    }

    std::vector<std::string> getRegistryOrder( bool bSorted ) const
    {
        if (!bSorted)
            return regOrder;

        std::vector<std::string> res;

        std::map< std::string, Registry>::const_iterator it = nsRegs.begin();
        for(; it != nsRegs.end(); ++it)
            res.push_back(it->first);

        return res;
    }

    //bool isFirst

    std::string getMasterBasesStr(bool hexFormat) const
    {
        std::string res;

        for( auto i : masterBases )
        {
            if (!res.empty())
                res.append(", ");

            std::ostringstream oss;
            if (hexFormat)
                oss<<"0x"<<std::hex<<std::setw(2)<<std::setfill('0');
            oss<<i;

            res.append(oss.str());
        }

        return res;
    }

    std::string getMasterBaseConstants( bool hexFormat, std::string prefix ) const
    {
        std::string res;

        std::vector< size_t >::const_iterator      idxIt  = masterBases.begin();
        std::vector< std::string >::const_iterator nameIt = masterBaseNames.begin();
        for( ; idxIt  != masterBases.end() && nameIt != masterBaseNames.end(); ++idxIt, ++nameIt )
        {
            std::ostringstream oss;
            oss<<"const static uint8_t "<<prefix<<"_"<<*nameIt<<" = ";
             if (hexFormat)
                oss<<"0x"<<std::hex<<std::setw(2)<<std::setfill('0');
            oss<<*idxIt;
            oss<<";\n";
            res.append(oss.str());
        }
        
        return res;
    }

    std::string getMasterBaseConstantsSwitch( bool hexFormat, std::string prefix, std::string deviceNameVar, bool generateAssert ) const
    {
        std::string res;

        {
            std::ostringstream oss;

            oss<<"\n";

            if (generateAssert)
            {
                oss<<"UMBA_ASSERT( "<<deviceNameVar<<" < "<<masterBases.size()<<");\n\n";
            }

            oss<<"// "<<prefix<<"\n";
            oss<<"switch("<<deviceNameVar<<")\n{\n";

            res.append(oss.str());
        }
        
        size_t number = 0;
        std::vector< size_t >::const_iterator      idxIt  = masterBases.begin();
        std::vector< std::string >::const_iterator nameIt = masterBaseNames.begin();
        for( ; idxIt  != masterBases.end() && nameIt != masterBaseNames.end(); ++idxIt, ++nameIt, ++number )
        {
            std::ostringstream oss;
            oss<<"    case "<<number<<": m_"<<prefix<<"Offset = ";
            if (hexFormat)
                oss<<"0x"<<std::hex<<std::setw(2)<<std::setfill('0');
            oss<<*idxIt;
            oss<<"; break; // "<<*nameIt<<"\n";
            res.append(oss.str());
        }

        {
            std::ostringstream oss;
            oss<<"} // switch("<<deviceNameVar<<")\n";
            res.append(oss.str());
        }
        
        return res;
    }

    

//    std::vector< size_t >             masterBases;
//    std::vector< std::string >        masterBaseNames;


    

}; // struct NamespaceRegs




struct DeviceInfo
{
    std::string                           deviceName;
    std::string                           deviceOrgName;
    std::string                           deviceDescription;

    std::map<std::string, NamespaceRegs>  deviceNamespaces;

    std::map<std::string, DeviceInfo>     deviceSlaves;

    RegImportOptions                      importOptions;

    std::map< unsigned, std::string >     groupDescriptions;
    unsigned                              currentGroup = 0;
    RegImportOptions                      groupImportOptions;

    unsigned                              importCount = 0; // used by owner/master

    void checkEmptyGroup()
    {
        if (groupDescriptions.find(0)==groupDescriptions.end())
            groupDescriptions[0] = std::string();
    }

    unsigned generateNewGroupId() const
    {
        unsigned newId = currentGroup + 1;
        while(groupDescriptions.find(newId)!=groupDescriptions.end())
             ++newId;
        return newId;
    }

    unsigned getResetGroupId() const
    {
        return 0;
    }

    bool operator==( const DeviceInfo &d ) const
    {
        if ( deviceName != d.deviceName )
            return false;

        if ( deviceOrgName != d.deviceOrgName )
            return false;

        if ( !isEqual( deviceNamespaces, d.deviceNamespaces ) )
            return false;

        if ( !isEqual( deviceSlaves, d.deviceSlaves ) )
            return false;

        if ( !isEqual( groupDescriptions, d.groupDescriptions ) )
            return false;

        return true;
    }

    bool operator!=( const DeviceInfo &d ) const
    {
        return !operator==( d );
    }

    RegImportOptions getFullQualifiedImportOptions( RegImportOptions opts, const std::string &nsName ) const
    {
        //int vis[] = { groupImportOptions.regVisibility, importOptions.regVisibility, REG_VISIBILITY_PUBLIC };
        RegImportOptions nsImportOptions;
        int nsVis = REG_VISIBILITY_UNSET;

        std::map<std::string, NamespaceRegs>::const_iterator nsIt = deviceNamespaces.find(nsName);
        if (nsIt!=deviceNamespaces.end())
        {
            nsImportOptions = nsIt->second.importOptions;
        }
        
        int vis[] = { groupImportOptions.regVisibility
                    , nsImportOptions.regVisibility
                    , importOptions.regVisibility
                    , REG_VISIBILITY_PUBLIC
                    };

        unsigned idx = 0;
        while(opts.regVisibility == REG_VISIBILITY_UNSET)
        {
            opts.regVisibility = vis[idx++];
        }

        idx = 0;
        int         sync     [] = { groupImportOptions.regSync      , nsImportOptions.regSync      , importOptions.regSync      , REG_SYNC_ALWAYS };
        std::string syncConds[] = { groupImportOptions.syncCondition, nsImportOptions.syncCondition, importOptions.syncCondition, std::string() };
        while(opts.regSync == REG_SYNC_UNSET)
        {
            opts.regSync   = sync[idx];
            opts.syncCondition = syncConds[idx++];
        }

        opts.tags.insert( groupImportOptions.tags.begin(), groupImportOptions.tags.end() );
        opts.tags.insert( nsImportOptions.tags.begin()   , nsImportOptions.tags.end() );

        return opts;
    }

    /*
    std::vector<std::string> appendAllKnownNamespaces( std::vector<std::string> nsOrder )
    {
        std::set< std::string >
    }
    */

    std::vector<std::string> getNamespaceOrder( const GeneratorOptions &gopts, bool bReverse ) const
    {
        std::set<std::string> usedNs;

        std::vector<std::string> res;
        res.reserve(deviceNamespaces.size());

        for( const auto &nsName : deviceNamespaces)
        {
            if (usedNs.find(nsName.first)!=usedNs.end()) // already used?
                continue;
            usedNs.insert(nsName.first);
            res.push_back(nsName.first);
        }

        // add other NSs
        for( const auto &allNsIt : gopts.nsStartIndeces)
        {
            if (usedNs.find(allNsIt.first)!=usedNs.end()) // already used?
                continue;
            if (allNsIt.first.empty())
                continue;
            usedNs.insert(allNsIt.first);
            res.push_back(allNsIt.first);
        }

        if (bReverse)
            std::reverse( res.begin(), res.end() );

        return res;
    }

    std::vector<std::string> getNamespaceOrder( const GeneratorOptions &gopts ) const
    {
        if (gopts.nsOrder.empty())
        {
            return getNamespaceOrder( gopts, gopts.nsReverseOrder );
        }
        else
        {
            std::set<std::string> usedNs;
            std::vector<std::string> nsResultOrder;

            for( auto nsName : gopts.nsOrder )
            {
                trim(nsName);

                if (nsName.empty())
                    continue;

                if (usedNs.find(nsName)!=usedNs.end()) // already used?
                    continue;

                usedNs.insert(nsName);
                nsResultOrder.push_back(nsName);
            }

            // fill order vector with implicit values

            auto autoNamespaces = getNamespaceOrder( gopts, gopts.nsReverseOrder );

            for( auto nsName : autoNamespaces )
            {
                trim(nsName);

                if (nsName.empty())
                    continue;

                if (usedNs.find(nsName)!=usedNs.end()) // already used?
                    continue;

                usedNs.insert(nsName);
                nsResultOrder.push_back(nsName);
            }

            return nsResultOrder;
        }
    }



}; // struct DeviceInfo


struct ReadContext
{
    std::string deviceName;
    std::string namespaceName;

    void setDevice( std::string s )
    {
        deviceName = s;
        namespaceName = "ro";
    }

    void setNamespace( std::string s )
    {
        namespaceName = s;
    }

};



struct ValueInfo
{
    std::string  name;
    uint64_t     value;
    std::string  floatValue;
    std::string  description;
    std::string  orgDefinition;
    bool         simpleOrgDef;

    bool operator==( const ValueInfo &v ) const
    {
        if ( name != v.name )
            return false;
    
        if ( value != v.value )
            return false;
    
        if ( floatValue != v.floatValue )
            return false;
    
        if ( description != v.description )
            return false;
    
        if ( orgDefinition != v.orgDefinition )
            return false;
    
        if ( simpleOrgDef != v.simpleOrgDef )
            return false;
    
        return true;
    }

    bool operator!=( const ValueInfo &v ) const
    {
        return !operator==( v );
    }

};


struct TypeInfo
{
    unsigned     byteSize;
    bool         fSigned;
    bool         fIntegral;
    bool         fBuiltin;
    std::string  typeName;   // canonical name
    std::string  typeSuffix;
    std::string  description;

    std::vector< std::string >            valuesOrder;
    std::map< std::string, ValueInfo>     values;

    std::map< std::string, std::string >  prefferedTypeGenerators;


    static const unsigned flag_size_8   = 0x0001;
    static const unsigned flag_size_16  = 0x0002;
    static const unsigned flag_size_32  = 0x0004;
    static const unsigned flag_size_64  = 0x0008;
    static const unsigned size_mask     = 0x000F;

    static const unsigned flag_signed   = 0x0010;
    static const unsigned flag_unsigned = 0x0020;
    static const unsigned flag_float    = 0x0040;
    static const unsigned type_mask     = 0x00F0;

    static const unsigned invalid_float_8  = 0x0040 | 0x0001;
    static const unsigned invalid_float_16 = 0x0040 | 0x0002;



    bool operator==( const TypeInfo &t) const
    {
        if ( byteSize != t.byteSize )
            return false;

        if ( fSigned != t.fSigned )
            return false;

        if ( fIntegral != t.fIntegral )
            return false;

        if ( typeName != t.typeName )
            return false;

        if ( typeSuffix != t.typeSuffix )
            return false;

        if ( description != t.description )
            return false;

        if ( !isEqual( valuesOrder, t.valuesOrder ) )
            return false;

        if ( !isEqual( values, t.values ) )
            return false;

        if ( !isEqual( prefferedTypeGenerators, t.prefferedTypeGenerators ) )
            return false;

        return true;
    }

    bool operator!=( const TypeInfo &t) const
    {
        return !operator==(t);
    }

/*
    void getTypeGenerators( std::vector< > &g ) const
    {
        for( const auto & name : prefferedTypeGenerators )
            g.push_back(name);
    }
*/

    void setTypeGenerators( const std::string &lst, const std::vector<std::string> & knownLangGenerators)
    {
        std::vector<std::string> generatorsByTarget;
        //splitToVector( lst, generatorsByTarget, ';' );
        splitMultiAttrsString( lst, generatorsByTarget );
        for( auto g : generatorsByTarget )
        {
            std::string lng, name;
            splitToPair( g, lng, name, ':' );
            if (!lng.empty())
            {
                if (!name.empty())
                    prefferedTypeGenerators[lng] = name;
                else
                {
                    name = lng;
                    for( const auto &l : knownLangGenerators)
                        prefferedTypeGenerators[l] = name;
                }
            }
                
        }
    }


    std::string getByteSizeStr() const
    {
        std::ostringstream s;
        s<<byteSize;
        return s.str();
    }

    std::string getByteSizeTypeStr() const
    {
        std::ostringstream s;
        s<<"rdlc_regsize_"<<8*byteSize;
        return s.str();
    }

    std::string getByteSizeTypeStrNoNamespace() const
    {
        std::ostringstream s;
        s<<"regsize_"<<8*byteSize;
        return s.str();
    }

    const char* getTypeIdStr() const
    {
        if (!fIntegral)
            return "RDLC_REGTYPE_FLOAT";
        if (fSigned)
            return "RDLC_REGTYPE_INT";
        return "RDLC_REGTYPE_UNSIGNED";
    }

    std::string getRdlcType()
    {
        std::ostringstream s;
        if (!fIntegral)
            s<<"f";
        else if (fSigned)
            s<<"s";
        else
            s<<"u";

        s<<8*byteSize;

        return s.str();
    }

    /*
    struct rdlc_regtype_float{};
    struct rdlc_regtype_int{};
    struct rdlc_regtype_unsigned{};
    struct rdlc_regsize_8{};
    struct rdlc_regsize_16{};
    struct rdlc_regsize_32{};
    */

    const char* getTypeTypeIdStr() const
    {
        if (!fIntegral)
            return "rdlc_regtype_float";
        if (fSigned)
            return "rdlc_regtype_signed";
        return "rdlc_regtype_unsigned";
    }

    const char* getTypeTypeIdStrNoNamespace() const
    {
        if (!fIntegral)
            return "regtype_float";
        if (fSigned)
            return "regtype_signed";
        return "regtype_unsigned";
    }

    int getTypeIdInt() const
    {
        if (!fIntegral)
            return -1;
        if (fSigned)
            return 1;
        return 0;
    }


    uint64_t getMask() const
    {
        switch(byteSize)
           {
            case 1:
                 return 0x00000000000000FF;
            case 2:
                 return 0x000000000000FFFF;
            case 4:
                 return 0x00000000FFFFFFFF;
            default:
                 return 0xFFFFFFFFFFFFFFFF;
           }
    }

    /*
    значение - десятичнаяя, 0x или 0b константа.
      Для задания бита в определенной позиции можно использовать
    префикс (или суффикс) bitNN / NNbit (решить, как, оба два
    или одно, пробел допустим или нет), NN - десятичное.
      Можно использовать сдвиг NN<<MM, где NN/MM любое десятичное
    число в разумных пределах
      Можно объединять по или - '|' все вышеперечисленные
    конструкции.
      Можно использовать ранее определенные имена констант
    */

    // -1 - integer but error, 0 - ok, 1 - not an integer
    static
    int tryParseSingleValueAsInt ( std::string str, uint64_t &u, uint64_t sizeBytes, std::string curFile, unsigned lineNo )
    {
        trim( str );

        uint64_t sizeBits = sizeBytes*8;

        std::string strValueToShift, strShiftCount;
        if (startsWithAndStrip( str, "bit" ) || endsWithAndStrip( str, "bit" ))
        {
            trim(str);
            uint64_t bitNo = 0;
            if (parseInteger( str, bitNo, 10 ))
            {
                if (bitNo >= sizeBits)
                {
                    GeneratorOptions gopts; // temporary stub for log
                    LOG_ERR<<"too much bit number\n";
                    return -1;
                }
                u = 1ull << bitNo;
                return 0;
            }
        }
        else if ( splitToPair(str, strValueToShift, strShiftCount, "<<") )
        {
            uint64_t valueToShift;
            if (!parseInteger ( strValueToShift, valueToShift))
            {
                return 0;
            }
            uint64_t shiftCountNum;
            if (!parseInteger ( strShiftCount, shiftCountNum))
            {
                return 0;
            }
            if (shiftCountNum >= sizeBits)
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"too much bit number\n";
                return -1;
            }

            uint64_t shiftCount = 0;
            u = valueToShift;
            for(; shiftCount!=shiftCountNum; ++shiftCount)
            {
                uint64_t prevU = u;
                u = u << 1;
                if (u<prevU)
                {
                    GeneratorOptions gopts; // temporary stub for log
                    LOG_ERR<<"value is out of range\n";
                    return -1;
                }
            }

            return 0;
        }
        else if (parseInteger ( str, u ))
        {
            return 0;
        }

        return 1; // not integer
    }


    //bool parseInteger ( const std::string &str, uint64_t &u, int ss )

    unsigned getTypeFlags() const
    {
        return byteSize | (fIntegral ? (fSigned?flag_signed:flag_unsigned) : flag_float);
    }

    void setPropsFromFlags( unsigned flags )
    {
        fIntegral = true;
        switch(flags&type_mask)
           {
            case flag_signed:
                 fSigned   = true;
                 break;
            case flag_unsigned:
                 fSigned   = false;
                 break;
            default: // flag_float
                 fIntegral = false;
           }

        byteSize = flags & size_mask;
    }


    static
    std::string  constructSuffix(unsigned bs, bool s, bool i)
    {
        std::string res;
        if (i)
        {
            if (s)
                res = "s";
            else
                res = "u";
        }
        else
        {
            res = "f";
        }

        switch(bs)
           {
            case 1:
                 res += "8";
                 break;
            case 2:
                 res += "16";
                 break;
            case 4:
                 res += "32";
                 break;
            case 8:
                 res += "64";
                 break;
            default:
                 res += "X";
           }

        return res;
    }

    void constructSuffix()
    {
        typeSuffix = constructSuffix(byteSize,fSigned,fIntegral);
    }

    TypeInfo( std::string n = "", unsigned bs = 1, bool s = false, bool i = true, bool fbi = false)
    : byteSize(bs)
    , fSigned(s)
    , fIntegral(i)
    , fBuiltin(fbi)
    , typeName(n)
    , typeSuffix(constructSuffix(bs, s, i))
    {}

}; // struct TypeInfo





class TypeMap
{

public:

    TypeMap() {}

    bool addType( TypeInfo t )
    {
        auto it = m_map.find(t.typeName);
        if (it!=m_map.end())
            return false;

        m_map[t.typeName] = t;

        return true;
    }

    bool addAlias( std::string name, std::string alias, bool fbi = false )
    {
        auto it = m_map.find(alias);
        if (it!=m_map.end())
            return false; // alias already exists

        it = m_map.find(name);
        if (it==m_map.end())
            return false; // unknown name

        m_map[alias] = TypeInfo(name);
        m_map[alias].fBuiltin = fbi;

        return true;
    }


    bool addAliasAllowSame( std::string name, std::string alias, bool checkTargetType = true )
    {
        auto it = m_map.find(alias);
        if (it!=m_map.end())
        {
            if (it->second.typeName==name)
                return true; // same alias, ok
            return false; // different alias already exists
        }

        if (checkTargetType) 
        {
            it = m_map.find(name);
            if (it==m_map.end())
                return false; // unknown name
        }

        m_map[alias] = TypeInfo(name);

        return true;
    }


    bool addTypeAllowSame( TypeInfo t )
    {
        auto it = m_map.find(t.typeName);
        if (it!=m_map.end())
        {
            if (it->second!=t)
                return false;
            return true;
        }

        m_map[t.typeName] = t;

        return true;
    }


    const TypeInfo* findType(std::string name) const
    {
        auto it = m_map.find(name);
        if (it==m_map.end())
            return 0;

        while( it->first != it->second.typeName )
        {
            it = m_map.find(it->second.typeName);
            if (it==m_map.end())
                return 0;
        }

        return &it->second;
    }

    const TypeInfo* findAliasInfo(std::string name) const
    {
        auto it = m_map.find(name);
        if (it==m_map.end())
            return 0;

        return &it->second;
    }

    const TypeInfo* findType(std::string name, std::set<std::string> &aliases) const
    {
        auto it = m_map.find(name);
        if (it==m_map.end())
            return 0;

        while( it->first != it->second.typeName )
        {
            aliases.insert(it->first);
            it = m_map.find(it->second.typeName);
            if (it==m_map.end())
                return 0;
        }

        return &it->second;
    }

    bool findType(std::string name, TypeInfo &t) const
    {
        const TypeInfo * pType = findType(name);
        if (pType)
        {
            t = *pType;
            return true;
        }

        return false;
    }

    bool findAliasInfo(std::string name, TypeInfo &t) const
    {
        const TypeInfo * pType = findAliasInfo(name);
        if (pType)
        {
            t = *pType;
            return true;
        }

        return false;
    }

    bool findValue( const TypeInfo *pTypeInfo, ValueInfo &v, unsigned &typeFlags, const std::string &name, std::string curFile, unsigned lineNo, bool bQuet = false ) const
    {
        std::string typeName, valName;
        if (!splitToPair( name, typeName, valName, ':' ))
        {
            std::swap(typeName, valName);
        }

        trim(typeName);
        trim(valName);

        if (!typeName.empty())
        {
            const TypeInfo* pFoundTypeInfo = findType(typeName);
            if (!pFoundTypeInfo)
            {
                //if (!bQuet)
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"value reference '"<<name<<"' - type '"<<typeName<<"' not found\n";
                return false;
            }

            pTypeInfo = pFoundTypeInfo;
        }

        std::map< std::string, ValueInfo>::const_iterator vit = pTypeInfo->values.find(valName);
        if (vit==pTypeInfo->values.end())
        {
            //if (!bQuet)
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"value reference '"<<name<<"' - value '"<<valName<<"' not found\n";
            return false;
        }

        typeFlags = pTypeInfo->getTypeFlags();
        v = vit->second;

        return true;
    }


    int tryParseSingleValueAsInt( TypeInfo &typeInfo, std::string str, uint64_t &u, std::string curFile, unsigned lineNo )
    {
        bool negResult = false;
        trim(str);
        if (startsWithAndStrip( str, "~" ))
        {
            trim(str);
            negResult = true;
        }
        
        // сдвиг проверить тут
        std::string strValueToShift, strShiftCount;
        std::vector< std::string > valsToParse;
        if ( !splitToPair(str, strValueToShift, strShiftCount, "<<"))
        {
            valsToParse.push_back(str);
        }
        else
        {
            valsToParse.push_back(strValueToShift);
            valsToParse.push_back(strShiftCount);
        }

        std::vector< uint64_t > tmpResVec;

        bool bFail = false;

        for( const auto &strValToParse : valsToParse )
        {
            uint64_t tmpRes = 0;
            int tpRes = TypeInfo::tryParseSingleValueAsInt ( strValToParse, tmpRes, typeInfo.byteSize, curFile, lineNo );
            if (tpRes<1)
            {
                if (tpRes<0)
                    bFail = true;

                tmpResVec.push_back(tmpRes);
                continue;
                /*
                if (!tpRes && negResult)
                {
                    u = ~u;
                    u &= typeInfo.getMask();
                }            
                return tpRes; // integer, bad or good
                */
            }

            ValueInfo val;
            unsigned typeFlags = 0;
            if (!findValue( &typeInfo, val, typeFlags, strValToParse, curFile, lineNo ))
               return -1;
           
            if ((typeFlags&TypeInfo::type_mask)==TypeInfo::flag_float)
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"value reference '"<<strValToParse<<"' references to float type\n";
                return -1;
            }

            tmpResVec.push_back(val.value);
        }

        if (tmpResVec.empty())
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"unexpected parsing error - '"<<str<<"'\n";
            return -1;
        }

        if (tmpResVec.size()<2)
        {
            u = tmpResVec[0];
        }
        else
        {
            u = tmpResVec[0] << tmpResVec[1];
        }


        if (negResult)
        {
            u = ~u;
            u &= typeInfo.getMask();
        }
        else
        {
            //u = val.value;
        }

        return bFail ? -1 : 0;
    }

    bool tryParseValueAsInt( TypeInfo &typeInfo, std::string str, uint64_t &u, std::string curFile, unsigned lineNo, bool &bSimple )
    {
        std::vector<std::string> vals;

        trim(str);

        bool negResult = false;
        std::string negTest = str;
        if (startsWithAndStrip( negTest, "~" ))
        {
            trim(negTest);
            if (unquote(negTest, '(', ')'))
            {
                trim(negTest);
                str = negTest;
                negResult = true;
            }
        }
        else
        {
            unquote(str, '(', ')');
            trim(str);
        }

        

        splitToVector( str, vals, '|');

        if (vals.empty())
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"no value taken\n";
            return false;
        }

        if (vals.size() > 1)
            bSimple = false;
        else
            bSimple = true;

        u = 0;

        for( auto s : vals)
        {
            uint64_t tmp;
            if (tryParseSingleValueAsInt( typeInfo, s, tmp, curFile, lineNo ))
                return false;
            u |= tmp;
        }

        if (negResult)
        {
            u = ~u;
            u &= typeInfo.getMask();
        }

        if (!typeInfo.fSigned)
        {
            uint64_t byteSizeCheckMask = 0;
           
            switch(typeInfo.byteSize)
               {
                case 1:
                     byteSizeCheckMask = 0xFFFFFFFFFFFFFF00ull;
                     break;
                case 2:
                     byteSizeCheckMask = 0xFFFFFFFFFFFF0000ull;
                     break;
                case 4:
                     byteSizeCheckMask = 0xFFFFFFFF00000000ull;
                     break;
               }
            
            if ((u&byteSizeCheckMask)!=0)
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"value is out of range\n";
                return false;
            }
        }
        else
        {
            int64_t i = (int64_t)u;
            int64_t minLim = 0;
            int64_t maxLim = 0;

            switch(typeInfo.byteSize)
               {
                case 1:
                     minLim = -128ll;
                     maxLim =  127ll;
                     break;
                case 2:
                     minLim = -32768ll;
                     maxLim =  32767ll;
                     break;
                case 4:
                     minLim = -2147483648ll;
                     maxLim =  2147483647ll;
                     break;
                default: return true;
               }

            if (i<minLim || i>maxLim)
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"value is out of range\n";
                return false;
            }
        }

        return true;
    }

    bool tryParseValue( TypeInfo &typeInfo, std::string str, ValueInfo &valParseTo, std::string curFile, unsigned lineNo )
    {
        if ((typeInfo.getTypeFlags()&TypeInfo::type_mask)!=TypeInfo::flag_float)
        {
            uint64_t u;

            if (tryParseValueAsInt( typeInfo, str, u, curFile, lineNo, valParseTo.simpleOrgDef ))
            {
                valParseTo.value = u;
                valParseTo.orgDefinition = str;
                return true;
            }

            return false;
        }

        valParseTo.simpleOrgDef = true;
        
        trim(str);
        std::string::size_type badCharPos = str.find_first_not_of( "0123456789.+-Ee" );
        if (badCharPos==str.npos)
        {
            valParseTo.floatValue = str;
            return true;
        }

        ValueInfo val;
        unsigned typeFlags = 0;
        if (!findValue( &typeInfo, val, typeFlags, str, curFile, lineNo ))
        {
            return false;
        }

        valParseTo.floatValue = val.floatValue;

        return true;
    }


    bool constructCustomTypeBase( TypeInfo &typeInfo, std::string typeInitString, std::string curFile, unsigned lineNo )
    {
        // NAME : attr1, attr2 - description
        // NAME : existing_type[name1,name2] - description
        std::string typeName, typeDefinition, typeDescription;
        splitToPair( typeInitString, typeName, typeDefinition, ':' );
        trim(typeName);
        unquoteString(typeName);
        trim(typeName);

        if (typeName.empty())
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"$type - type name not taken\n";
            return false;
        }

        if (findType(typeName))
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"$type - type '"<<typeName<<"' already exists\n";
            return false;
        }

        splitToPair( typeDefinition, typeDefinition, typeDescription, '-' );
        unquoteTrimString(typeDescription);


        std::vector<std::string> typeDefinitionVec;
        splitToVectorWithRestectForBraces( typeDefinition, typeDefinitionVec, ',' );

        static std::map< std::string, unsigned > typeFlagsMap;
        if (typeFlagsMap.empty())
        {
            typeFlagsMap["int"]      = TypeInfo::flag_signed;
            typeFlagsMap["integer"]  = TypeInfo::flag_signed;
            typeFlagsMap["signed"]   = TypeInfo::flag_signed;
            typeFlagsMap["s"]        = TypeInfo::flag_signed;
            typeFlagsMap["i"]        = TypeInfo::flag_signed;

            typeFlagsMap["uint"]     = TypeInfo::flag_unsigned;
            typeFlagsMap["unsigned"] = TypeInfo::flag_unsigned;
            typeFlagsMap["u"]        = TypeInfo::flag_unsigned;

            typeFlagsMap["float"]    = TypeInfo::flag_float;
            typeFlagsMap["f"]        = TypeInfo::flag_float;

            typeFlagsMap["8"]        = TypeInfo::flag_size_8;
            typeFlagsMap["16"]       = TypeInfo::flag_size_16;
            typeFlagsMap["32"]       = TypeInfo::flag_size_32;
            typeFlagsMap["64"]       = TypeInfo::flag_size_64;
            //typeFlagsMap[""]       = TypeInfo::;
        }

        unsigned curFlags = 0;

        for( auto &td : typeDefinitionVec)
        {
            trim(td);

            if (td.empty())
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"$type '"<<typeName<<"' - type definition keyword or existing type name is empty - extra comma found\n";
                return false;
            }

            std::string indecesStr;
            std::vector<std::string> indecesVec;
            bool bCopyAll = false;
            if (splitToPair( td, td, indecesStr, '[' ))
            {
                trim(td);
                //splitToVectorWithRestectForBraces( typeDefinition, typeDefinitionVec, ',' );
                indecesStr = std::string("[") + indecesStr;
                unquote( indecesStr, '[', ']' );
                trim(indecesStr);
                splitToVector( indecesStr, indecesVec, ',' );
                for( auto &idxStr : indecesVec)
                {
                    trim(idxStr);
                    if (idxStr=="*")
                        bCopyAll = true;
                    if (idxStr.empty())
                    {
                        GeneratorOptions gopts; // temporary stub for log
                        LOG_ERR<<"$type '"<<typeName<<"' - empty reference to existing type value (possible wrong double comma ',,')\n";
                        return false;
                    }
                }
            }

            unsigned newFlags = 0;

            if (indecesVec.empty())
            {
                auto tfIt = typeFlagsMap.find(td);
                if (tfIt!=typeFlagsMap.end())
                {
                    newFlags = tfIt->second;
                }
                else
                {
                    TypeInfo tmpTypeInfo;
                    if (findType(td, tmpTypeInfo))
                        newFlags = tmpTypeInfo.getTypeFlags();
                }
            }
            else
            {
                // taken indeces - values to copy - don't lookup in keywords, lookup only in types
                TypeInfo tmpTypeInfo;
                if (findType(td, tmpTypeInfo))
                    newFlags = tmpTypeInfo.getTypeFlags();

                if (!newFlags)
                {
                    GeneratorOptions gopts; // temporary stub for log
                    LOG_ERR<<"$type '"<<typeName<<"' - existing type not found ('"<<td<<"')\n";
                    return false;
                }

                if (bCopyAll)
                {
                    typeInfo.valuesOrder = tmpTypeInfo.valuesOrder;
                    typeInfo.values      = tmpTypeInfo.values     ;
                }
                else
                {
                    for(auto v : indecesVec)
                    {
                        std::string orgName, newName;
                        splitToPair( v, orgName, newName, ':' );
                        trim(orgName);
                        trim(newName);
                        if (newName.empty())
                            newName = orgName;

                        auto valIt = tmpTypeInfo.values.find(orgName);
                        if (valIt==tmpTypeInfo.values.end())
                        {
                            GeneratorOptions gopts; // temporary stub for log
                            LOG_ERR<<"$type '"<<typeName<<"' - value '"<<orgName<<"' not found in type '"<<td<<"'\n";
                            return false;
                        }

                        auto newValIt = typeInfo.values.find(newName);
                        if (newValIt!=typeInfo.values.end())
                        {
                            GeneratorOptions gopts; // temporary stub for log
                            LOG_ERR<<"$type '"<<typeName<<"' - value '"<<newName<<"' already exist\n";
                            return false;
                        }

                        ValueInfo valueInfo = valIt->second;
                        valueInfo.name = newName;
                        typeInfo.valuesOrder.push_back(newName);
                        typeInfo.values[newName] = valueInfo;
                    }
                }
            }

            if (!newFlags)
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"$type '"<<typeName<<"' - type definition keyword neither existing type not found ('"<<td<<"')\n";
                return false;
            }

            if ( (newFlags&TypeInfo::type_mask)!=0 && (curFlags&TypeInfo::type_mask)!=0 )
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"$type '"<<typeName<<"' - basic type already defined ('"<<td<<"')\n";
                return false;
            }
            
            if ( (newFlags&TypeInfo::size_mask)!=0 && (curFlags&TypeInfo::size_mask)!=0 )
            {
                GeneratorOptions gopts; // temporary stub for log
                LOG_ERR<<"$type '"<<typeName<<"' - type size already defined ('"<<td<<"')\n";
                return false;
            }
            
            curFlags |= newFlags;

        } // for( auto &td : typeDefinitionVec)

        if (curFlags==TypeInfo::invalid_float_8 || curFlags==TypeInfo::invalid_float_16)
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"$type '"<<typeName<<"' - invalid size taken for float type\n";
            return false;
        }

        if ((curFlags&TypeInfo::type_mask)==0)
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"$type '"<<typeName<<"' - basic type not defined\n";
            return false;
        }

        if ((curFlags&TypeInfo::size_mask)==0)
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR<<"$type '"<<typeName<<"' - type size not defined\n";
            return false;
        }

        typeInfo.typeName     = typeName;
        typeInfo.description  = typeDescription;
        typeInfo.setPropsFromFlags( curFlags );
        typeInfo.constructSuffix();

        return true;

    }

    std::set<std::string> getTypesAsSet( bool fGet, bool getAliases = false ) const
    {
        std::set<std::string> res;
        if (!fGet)
            return res;

        for( auto it : m_map)
        {
            if (getAliases)
            {
                res.insert( it.first );
            }
            else
            {
                if (it.first==it.second.typeName)
                    res.insert( it.first );
            }
        }

        return res;
    }

    bool mergeTypeMap( const TypeMap &t, std::string &failedType )
    {
        std::map< std::string, TypeInfo >::const_iterator tit = t.m_map.begin();
        for(; tit != t.m_map.end(); ++tit )
        {
            if (tit->first!=tit->second.typeName)
               continue; // alias found

            if (!addTypeAllowSame(tit->second))
            {
                failedType = tit->second.typeName;
                return false;
            }
        }

        tit = t.m_map.begin();
        for(; tit != t.m_map.end(); ++tit )
        {
            if (tit->first==tit->second.typeName)
               continue; // type found

            if (!addAliasAllowSame(tit->second.typeName, tit->first, false /* checkTargetType */ ))
            {
                failedType = tit->second.typeName;
                return false;
            }
        }

        return true;
    }

protected:

    std::map< std::string, TypeInfo >  m_map;


}; // class TypeMap


#include "igen.h"


inline
ITypeGen* GeneratorOptions::getTypeGen( std::string typeName, const TypeMap &typeMap ) const

{
    std::string 
    generatorName = defaultTypeGenerator; // none by default
    auto it = typeGeneratorNames.find(typeName); // from command line: user_type -> generator_name
    if (it!=typeGeneratorNames.end())
    {
        generatorName = it->second;
    }


    std::string foundOutputLang = outputType;

    if (generatorName=="default")
    {
        const TypeInfo* pTypeInfo = typeMap.findType(typeName);
        if (!pTypeInfo)
        {
            generatorName = "none";
        }
        else
        {
            std::vector<std::string> langTypeCandidates;
            langTypeCandidates.emplace_back(outputType);
            char splitCh = contains( outputType, "_-");
            if ( splitCh )
            {
                // contains splitter
                std::string lang, subLang;
                splitToPair( outputType, lang, subLang, splitCh );
                if (!lang.empty())
                    langTypeCandidates.emplace_back(lang);
            }

            // add 'any' generator name
            langTypeCandidates.emplace_back("*");

            bool bFound = false;

            for( auto outputLang : langTypeCandidates )
            {
                std::map< std::string, std::string >::const_iterator ptgIt = pTypeInfo->prefferedTypeGenerators.find(outputLang);
                if (ptgIt != pTypeInfo->prefferedTypeGenerators.end())
                {
                    bFound = true;
                    generatorName   = ptgIt->second;
                    foundOutputLang = outputLang;
                    break;
                }
            }

            if (!bFound)
            {
                generatorName = "none";
            }
        }
    }

    std::vector<std::string> langTypeCandidates;

    langTypeCandidates.emplace_back(foundOutputLang);

    if (foundOutputLang!=outputType)
       langTypeCandidates.emplace_back(outputType);

    char splitCh = contains( outputType, "_-");
    if ( splitCh )
    {
        // contains splitter
        std::string lang, subLang;
        splitToPair( outputType, lang, subLang, splitCh );
        if (!lang.empty())
            langTypeCandidates.emplace_back(lang);
    }

    // add 'any' generator name
    langTypeCandidates.emplace_back("*");


    bool bFound = false;

    for( auto outputLang : langTypeCandidates )
    {
        auto outputIt = typeGenerators.find(outputLang);
        if (outputIt!=typeGenerators.end())
        {
            bFound = true;
            foundOutputLang = outputLang;
            break;
        }
    }

// new TypeGenNone();
    if (!bFound)
        return new TypeGenNone();
        //return 0;

    auto outputIt = typeGenerators.find(foundOutputLang);
    if (outputIt==typeGenerators.end())
        return new TypeGenNone();
        //return 0;

    // new TypeGenNone();
    auto genIt = outputIt->second.find(generatorName);
    if (genIt == outputIt->second.end())
    {
        if (generatorName=="none")
        {
            static ITypeGen *pNoneGen = 0;
            if (!pNoneGen)
                pNoneGen = new TypeGenNone();
            return pNoneGen;
        }
        else
        {
            GeneratorOptions gopts; // temporary stub for log
            LOG_ERR_OPT<<"failed to find generator '"<<generatorName<<"' for type '"<<typeName<<"'\n";
        }
        return 0;
    }

    return genIt->second;
    
}


inline
size_t GeneratorOptions::getRegNumberIncrementValue( const TypeInfo ti ) const
{
    if (protocolVersion==1)
    {
        return ti.byteSize;
    }

    return 1;
}

inline
NsIndecesInfo GeneratorOptions::getNamespaceIndecesInfo( const NamespaceRegs &nsRegs ) const
{
    const NsIndecesInfo *pIdxInfo = &nsStartIndeces[""];
    auto nsIdxIt = nsStartIndeces.find(nsRegs.nsName);
    if (nsIdxIt==nsStartIndeces.end())
        nsStartIndeces[""].startIdx += nsStartIndeces[""].numIdx;
    else
        pIdxInfo = &nsIdxIt->second;

    return *pIdxInfo;
}

