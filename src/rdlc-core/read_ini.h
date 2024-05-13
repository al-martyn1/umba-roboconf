#pragma once

#include <regex>
#include <exception>
#include <stdexcept>
#include <stack>

#include "isa.h"
#include "incsearch.h"

#include "sys_utils.h"

#include "read_ini_helpers.h"


enum IniReadState
{
    readNormal,
    readType
};


//-----------------------------------------------------------------------------
template<typename TContainer>
struct ScopeExitContainerCleaner
{
    TContainer &str;
    ScopeExitContainerCleaner(TContainer &s) : str(s) {}
    ~ScopeExitContainerCleaner()
    {
        str.clear();
    }

}; // struct ScopeExitContainerCleaner



    

//-----------------------------------------------------------------------------
bool readIni( std::istream &in, ReadContext ctx, std::string curFile, std::map< std::string, DeviceInfo > &devices, TypeMap &typeMap, std::set< std::string > &onceIncluded, const GeneratorOptions &generatorOptions );

bool logUserWarn( std::string  curFile
            , int          lineNo
            , std::string  warnMsg
            , const GeneratorOptions &generatorOptions
            )
{
    const GeneratorOptions &gopts = generatorOptions;
    unquoteString(warnMsg);
    if (warnMsg.empty())
        warnMsg = "warning directive found";
    LOG_WARN("warn") <<warnMsg<<"\n";
    return true;
}

//-----------------------------------------------------------------------------
inline
bool readIniParseLine( std::string   str
                     , IniReadState &readState
                     , TypeInfo     &constructingType
                     , ReadContext  &ctx
                     , std::string  curFile
                     , int          lineNo
                     , std::map< std::string, DeviceInfo > &devices
                     , TypeMap &typeMap
                     , std::set< std::string > &onceIncluded
                     , const GeneratorOptions &generatorOptions
                     )
{
    const GeneratorOptions &gopts = generatorOptions;

    std::string directive, directiveArg;
    std::map< std::string, std::string > attrs;

    //-------
    if (readState==readType)
    {
        if (isDirective( str, directive, directiveArg ))
        {
            if (directive=="warning")
            {
                return logUserWarn( curFile, lineNo, directiveArg, generatorOptions );
            }
            else if (directive=="type_end" || directive=="end_type")
            {
                typeMap.addType( constructingType );
                readState = readNormal;
            }
            else
            {
                LOG_ERR<<"directive '"<<directive<<"' not allowed here\n";
                return false;
            }
        }
        else
        { // read values here
            // VAL [:] значение - описание

            ValueInfo curVal;

            std::string valDefinition, valValueStr;

            if (!readAttributes( curFile, false /* allowEndAttributes */, lineNo, str, attrs ))
                return false;

            splitToPair( str, curVal.name, valDefinition, ':' );
            trim(curVal.name);
            trim(valDefinition);
            splitToPairNotDecimalMinus( valDefinition, valValueStr, curVal.description, '-' );
            trim(valValueStr);
            trim(curVal.description);
            unquoteString(curVal.description);
            trim(curVal.description);

            if (curVal.name.empty())
            {
                LOG_ERR<<"constant name not taken\n";
                return false;
            }

            if (valValueStr.empty())
            {
                LOG_ERR<<"no value taken for constant\n";
                return false;
            }

            if (constructingType.values.find(curVal.name)!=constructingType.values.end())
            {
                LOG_ERR<<"constant '"<<curVal.name<<"' already defined\n";
                return false;
            }

            if (!typeMap.tryParseValue( constructingType, valValueStr, curVal, curFile, lineNo ))
            {
                return false;
            }

            constructingType.values[curVal.name] = curVal;
            constructingType.valuesOrder.push_back(curVal.name);

        }
    }
    //-------
    else if (readState==readNormal)
    {

        if (isShortNamespace( str, directiveArg ))
        {
            ctx.setNamespace(directiveArg);
            return true; // continue;

        } else if (isDirective( str, directive, directiveArg ))
        {
/*
bool readAttributes( std::string curFile, int lineNo, std::string &s, std::map< std::string, std::string > &attrs );
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedAttrs );
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &allowedAttrs );
bool checkAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &mutAttrs );
bool checkAttrFormat( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::string &attrName, std::string attrExpr );
int convertAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, int firstAttrNo, const std::vector<std::string> &mutAttrs );
*/
       
            if (directive=="warning")
            {
                return logUserWarn( curFile, lineNo, directiveArg, generatorOptions );
            }
            //bool readDirectiveWords( std::string curFile, int lineNo, std::string &s, std::vector< std::string > &words, std::string directiveName = "require" )
            else if (directive=="require" )
            {
                std::vector<std::string> params;
                if (!readDirectiveWords(curFile, lineNo, directiveArg, params, "require" ))
                    return false;
                
                if (params.empty())
                {
                    LOG_ERR<<"'require' - missing parameters\n";
                    return false;
                }


                if (toLower(params[0])=="rdlc")
                {
                    if (params.size()<2)
                    {
                        LOG_ERR<<"'require rdlc' - missing parameters\n";
                        return false;
                    }

                    if (toLower(params[1])!="version")
                    {
                        LOG_ERR<<"'require rdlc' - unknown parameter - '"<<params[1]<<"'\n";
                        return false;
                    }

                    if (params.size()<3)
                    {
                        LOG_ERR<<"'require rdlc version' - missing version number\n";
                        return false;
                    }

                    std::vector< unsigned > takenVer = readNumericParts(params[2]);
                    std::vector< unsigned > rdlcVer  = readNumericParts(rdlcVersion);
                    int cmpRes = compareUnsignedVectors( rdlcVer, takenVer );
                    if (cmpRes<0)
                    {
                        LOG_ERR<<"this file requires RDLC version "<<params[2]<<" or higher\n";
                        return false;
                    }

//std::vector<unsigned> readNumericParts( const std::string &str, int ss = 10 )
//int compareUnsignedVectors( std::vector< unsigned > v1, std::vector< unsigned > v2 )


                    //rdlcVersion
                
                }
                else
                {
                    LOG_ERR<<"'require' - unknown parameter - '"<<params[0]<<"'\n";
                    return false;
                }

            }
            else if (directive=="type" )
            {
                constructingType = TypeInfo();

                if (!readAttributes( curFile, false /* allowEndAttributes */, lineNo, directiveArg, attrs, makeStringSet("generator", "format") ))
                    return false;

                if (!checkAttrsAllowed( curFile, lineNo, attrs, makeStringVector("generator", "format") ))
                     return false;

                std::map< std::string, std::string >::const_iterator attrsIt = attrs.find("generator");
                if (attrsIt != attrs.end())
                    constructingType.setTypeGenerators( attrsIt->second, generatorOptions.getLangGenerators() );

                if (!typeMap.constructCustomTypeBase( constructingType, directiveArg, curFile, lineNo ))
                    return false;

                readState = readType;
            }
            else if (directive=="namespace" )
            {
                RegImportOptions importOptions;

                if (!readAttributes( curFile, false /* allowEndAttributes */, lineNo, directiveArg, attrs, makeStringSet("group", "tag") ))
                    return false;

                if (!checkAttrsAllowed( curFile, lineNo, attrs, makeStringVector("group", "public", "protected", "private", "sync", "tag") ))
                     return false;

                if (!extractRegImportOptionsFromAttributes( curFile, lineNo, attrs, importOptions ))
                     return false;

                trim(directiveArg);
                unquoteString(directiveArg);

                if (directiveArg.empty())
                {
                    LOG_ERR<<"namespace name not taken\n";
                    return false;
                }
       
                std::string nsName = directiveArg;
                ctx.setNamespace(nsName);

                DeviceInfo &nsDev = devices[ctx.deviceName];

                nsDev.deviceNamespaces[nsName].importOptions = importOptions;

                if (attrs.find("group")!=attrs.end())
                {
                    std::string grpName = attrs["group"];

                    trim(grpName);
                    unquoteString(grpName);

                    DeviceInfo &grpDev = devices[ctx.deviceName];

                    grpDev.checkEmptyGroup();
                   
                    if (grpName.empty())
                    {
                        grpDev.currentGroup = grpDev.getResetGroupId();
                        grpDev.groupImportOptions = importOptions;
                    }
                    else
                    {
                        grpDev.currentGroup = grpDev.generateNewGroupId();
                        grpDev.groupDescriptions[grpDev.currentGroup] = grpName;
                        grpDev.groupImportOptions = importOptions;
                    }
                }
            }
            else  if (directive=="group" )
            {
                RegImportOptions importOptions;

                if (!readAttributes( curFile, true /* allowEndAttributes */, lineNo, directiveArg, attrs, makeStringSet("tag") ))
                    return false;

                if (!checkAttrsAllowed( curFile, lineNo, attrs, makeStringVector("public", "protected", "private", "sync", "tag") ))
                     return false;

                if (!extractRegImportOptionsFromAttributes( curFile, lineNo, attrs, importOptions ))
                     return false;

                trim(directiveArg);
                unquoteString(directiveArg);

                DeviceInfo &grpDev = devices[ctx.deviceName];
                grpDev.checkEmptyGroup();
                grpDev.currentGroup = grpDev.generateNewGroupId();
                grpDev.groupDescriptions[grpDev.currentGroup] = directiveArg;
                grpDev.groupImportOptions = importOptions;
       
            }
            else if (directive=="once" )
            {
                std::string canonicalFileName = getOsCanonicalFullPathName(curFile);
                onceIncluded.insert(canonicalFileName);
            }
            else if (directive=="include")
            {
       
                bool sysInclude = unquoteInclude(directiveArg);
       
                if (directiveArg.empty())
                {
                    LOG_ERR<<"Include file name not taken\n";
                    return false;
                }
       
                bool bFound = false;
                std::ifstream included;
       
                std::string foundName;
       
                if (sysInclude)
                    bFound = includeSearch( included, directiveArg, foundName, includePaths, makeIncVectorFromFileName(curFile) );
                else
                    bFound = includeSearch( included, directiveArg, foundName, makeIncVectorFromFileName(curFile), includePaths );
       
                if (!bFound)
                {
                    LOG_ERR<<"Failed to read include file '"<<directiveArg<<"'\n";
                    return false;
                }
       
                if (!readIni( included, ctx, foundName, devices, typeMap, onceIncluded, generatorOptions ))
                    return false;
       
            }
            else  if (directive=="device" )
            {
                RegImportOptions importOptions;
                if (!readAttributes( curFile, false /* allowEndAttributes */, lineNo, directiveArg, attrs ))
                    return false;

                if (!checkAttrsAllowed( curFile, lineNo, attrs, makeStringVector("public", "protected", "private", "sync") ))
                     return false;

                if (!extractRegImportOptionsFromAttributes( curFile, lineNo, attrs, importOptions ))
                     return false;

                trim(directiveArg);

                std::string takenDeviceName;

                if (!readName( curFile, lineNo, "\'\"<", directiveArg, takenDeviceName ))
                {
                    LOG_ERR<<"device directive - invalid device name or device name not taken\n";
                    return false;
                }

                unquoteString(takenDeviceName);

                if (takenDeviceName.empty())
                {
                    LOG_ERR<<"device name not taken\n";
                    return false;
                }

                trim(directiveArg);
                std::string takenDeviceDescription;
                if (!directiveArg.empty() && directiveArg[0]=='-')
                {
                    while(!directiveArg.empty() && directiveArg[0]=='-')
                        directiveArg.erase(0,1);
                    trim(directiveArg);
                    takenDeviceDescription = directiveArg;
                }
       
                unquoteString(takenDeviceDescription);
                trim(takenDeviceDescription);

                //splitToPair( typeDefinition, typeDefinition, typeDescription, '-' );
                //importOptions
                ctx.setDevice(takenDeviceName);

                DeviceInfo &dev = devices[ctx.deviceName];
                dev.importOptions = importOptions;
                dev.deviceName    = ctx.deviceName;
                dev.deviceOrgName = ctx.deviceName;
                dev.deviceDescription = takenDeviceDescription;
            }
            else  if (directive=="import" )
            {
                std::map< std::string, std::string > importParams;
                std::set<std::string> allowedImportVerbs;
                allowedImportVerbs.insert("from");
                allowedImportVerbs.insert("as");
                allowedImportVerbs.insert("prefix");
                if (!readImportParams( curFile, lineNo, false /* allowEndAttributes */, directiveArg, importParams, attrs, allowedImportVerbs, makeStringSet("tag", "+tag", "-tag") /* allowedMultiVal */ ))
                    return false;

                RegImportOptions importOptions;

                if (!checkAttrsAllowed( curFile, lineNo, attrs, makeStringVector("public", "protected", "private", "tag", "+tag", "-tag" /* , "sync" */  /* , "prefix" */ ) ))
                     return false;

                if (!extractRegImportOptionsFromAttributes( curFile, lineNo, attrs, importOptions, true ))
                     return false;

                if (importOptions.regVisibility == REG_VISIBILITY_UNSET)
                    importOptions.regVisibility = REG_VISIBILITY_PUBLIC;

                std::string deviceName = importParams["device"];
                std::string varPrefix  = importParams["prefix"];
                std::string readFrom   = importParams["from"];
                std::string deviceAs   = importParams["as"];
                std::string deviceDescription = importParams["-"];

                unquoteTrimString( deviceName );
                unquoteTrimString( varPrefix );
                //unquoteTrimString( deviceAs );
                unquoteTrimString( deviceAs );
                unquoteTrimString( deviceDescription );

                if (!varPrefix.empty() && varPrefix.back()!='_')
                    varPrefix.append(1,'_');

                if (deviceAs.empty())
                    deviceAs = deviceName;

                bool sysInclude = unquoteInclude(readFrom);

                if (readFrom.empty())
                {
                    LOG_ERR<<"import - file name not taken (empty file name)\n";
                    return false;
                }


                bool bFound = false;
                std::ifstream included;
       
                std::string foundName;
       
                if (sysInclude)
                    bFound = includeSearch( included, readFrom, foundName, includePaths, makeIncVectorFromFileName(curFile) );
                else
                    bFound = includeSearch( included, readFrom, foundName, makeIncVectorFromFileName(curFile), includePaths );
       
                if (!bFound)
                {
                    LOG_ERR<<"Failed to read import file '"<<readFrom<<"'\n";
                    return false;
                }

                std::map< std::string, DeviceInfo > devicesImportFrom;
                TypeMap typeMapImportFrom;
                initTypeMap(typeMapImportFrom);

                std::set< std::string > onceIncludedImportFrom;


                ReadContext importCtx = ctx;
                importCtx.setDevice( getFileName(getNameFromFull(foundName)) );
                
       
                if (!readIni( included, importCtx, foundName, devicesImportFrom, typeMapImportFrom, onceIncludedImportFrom, generatorOptions ))
                    return false;

                std::string failedType;
                if (!typeMap.mergeTypeMap( typeMapImportFrom, failedType ))
                {
                    LOG_ERR<<"type '"<<failedType<<"' already defined\n";
                    return false;
                }

                std::map< std::string, DeviceInfo >::const_iterator difIt = devicesImportFrom.find(deviceName);
                if (difIt == devicesImportFrom.end())
                {
                    LOG_ERR<<"import - device '"<<deviceName<<"' not found in '"<<foundName<<"' file\n";
                    return false;
                }

                DeviceInfo slaveDevice = difIt->second;
                slaveDevice.deviceOrgName = slaveDevice.deviceName;
                slaveDevice.deviceName    = deviceAs;
                if (!deviceDescription.empty())
                    slaveDevice.deviceDescription = deviceDescription;

                std::string slaveRegsGrpName = slaveDevice.deviceDescription; // deviceAs;
                if (!slaveRegsGrpName.empty())
                    slaveRegsGrpName += " - ";

                slaveRegsGrpName += deviceAs;

                if (deviceAs != deviceName)
                {
                    slaveRegsGrpName.append(" (");
                    slaveRegsGrpName.append( deviceName );
                    slaveRegsGrpName.append(")");
                }

                DeviceInfo &masterDevice = devices[ctx.deviceName];

                unsigned savedMasterGroup = masterDevice.currentGroup;
                masterDevice.currentGroup = masterDevice.generateNewGroupId();
                masterDevice.groupDescriptions[masterDevice.currentGroup] = slaveRegsGrpName;

                {
                    auto slaveNsIt = slaveDevice.deviceNamespaces.begin();
                    for(; slaveNsIt!=slaveDevice.deviceNamespaces.end(); ++slaveNsIt)
                    {
                        NamespaceRegs &slaveNsRegs = slaveNsIt->second;
                        auto regsOrder = slaveNsRegs.getRegistryOrder( false /* generatorOptions.bSort */  );
                        auto slaveRegNameIt = regsOrder.begin();
                        for(; slaveRegNameIt != regsOrder.end(); ++slaveRegNameIt)
                        {
                            auto regMapIt = slaveNsRegs.nsRegs.find(*slaveRegNameIt);
                            if (regMapIt==slaveNsRegs.nsRegs.end())
                            {
                                LOG_ERR_OPT<<"inconsistent data state\n";
                                return false;
                            }
                        
                            Registry &reg = regMapIt->second;
                       
                            TypeInfo typeInfo;
                            if (!typeMap.findType( reg.regType, typeInfo ))
                            {
                                LOG_ERR_OPT<<"unknown type '"<<reg.regType<<"', registry: '"<<slaveDevice.deviceName<<"."<<slaveNsRegs.nsName<<"."<<*slaveRegNameIt<<"\n";
                                return false;
                            }

                            bool importAllowed = (reg.importOptions.regVisibility <= importOptions.regVisibility);
                            /*
                               Тэги регистра        Тэги импорта

                                   Нет                 Нет       - импортируем

                                   Нет                 Есть      - импортируем

                                   Есть                Нет       - не импортируем

                                   Есть                Есть      - импортируем если есть одинаковый тэг

                            */
                            // если importOptions не содержит 
                            if (reg.importOptions.tags.empty())
                            {
                                // Импортируем
                            }
                            else if (importOptions.tags.empty())
                            {
                                // При импорте тэги не заданы
                                importAllowed = false; // не импортируем
                            }
                            else
                            {
                                bool foundSameTag      = false;
                                bool foundDisableFlag  = false;
                                for( auto t : importOptions.tags)
                                {
                                    if (t.empty()) continue;

                                    bool withMinus = false;
                                    if (t[0]=='-' || t[0]=='+')
                                    {
                                        if (t[0]=='-')
                                            withMinus = true;
                                        t.erase(0,1);
                                    }

                                    if (!withMinus)
                                    {
                                        if (t=="*" || reg.importOptions.tags.find(t)!=reg.importOptions.tags.end())
                                        {
                                            foundSameTag = true;
                                            //break;
                                        }
                                    }
                                    else
                                    {
                                        if (t=="*" || reg.importOptions.tags.find(t)!=reg.importOptions.tags.end())
                                        {
                                            foundDisableFlag = true;
                                            break;
                                        }
                                    }
                                }

                                if (!foundSameTag || foundDisableFlag)
                                    importAllowed = false;
                            }

                            if (importAllowed)
                            {
        //importOptions.regVisibilityModifier = +1;
        //importOptions.regVisibilityAssigner = ;

                                //reg.importOptions
                                for( const auto &clrTag : importOptions.tagsMinus)
                                {
                                    if (clrTag=="*")
                                        reg.importOptions.tags.clear();
                                    else
                                        reg.importOptions.tags.erase(clrTag);
                                }

                                for( const auto &setTag : importOptions.tagsPlus)
                                {
                                    reg.importOptions.tags.insert(setTag);
                                }

                                std::string masterRegName = varPrefix + *slaveRegNameIt;

                                if ( masterDevice.deviceNamespaces[ slaveNsRegs.nsName ].nsRegs.find( masterRegName ) != masterDevice.deviceNamespaces[ slaveNsRegs.nsName ].nsRegs.end() )
                                {
                                    LOG_ERR<<"Registry "<<masterDevice.deviceName<<"."<<slaveNsRegs.nsName<<"."<<masterRegName<<" already defined\n";
                                    return false;
                                }

                                RegImportOptions regImportOptionsApplied = reg.importOptions;
                                regImportOptionsApplied.regVisibility += importOptions.regVisibilityModifier;
                                if (importOptions.regVisibilityAssigner!=REG_VISIBILITY_UNSET)
                                    regImportOptionsApplied.regVisibility = importOptions.regVisibilityAssigner;

                                if (regImportOptionsApplied.regVisibility < REG_VISIBILITY_PUBLIC)
                                    regImportOptionsApplied.regVisibility = REG_VISIBILITY_PUBLIC;
                                if (regImportOptionsApplied.regVisibility > REG_VISIBILITY_PRIVATE)
                                    regImportOptionsApplied.regVisibility = REG_VISIBILITY_PRIVATE;

                                masterDevice.deviceNamespaces[ slaveNsRegs.nsName ].nsRegs[ masterRegName ] = 
                                      Registry( reg.regType, reg.regDescription, masterDevice.currentGroup, regImportOptionsApplied, slaveDevice.deviceName  /* deviceAs */ , *slaveRegNameIt ); // masterDevice.getFullQualifiedImportOptions(reg.importOptions)

                                masterDevice.deviceNamespaces[ slaveNsRegs.nsName ].regOrder.push_back(masterRegName);

                                if (masterDevice.deviceNamespaces[ slaveNsRegs.nsName ].nsName.empty())
                                    masterDevice.deviceNamespaces[ slaveNsRegs.nsName ].nsName = slaveNsRegs.nsName;

                                reg.importOptions.regVisibility = REG_VISIBILITY_PUBLIC;
                                // slaveNsRegs.nsName
                            }
                            else
                            {
                                reg.importOptions.regVisibility = REG_VISIBILITY_PRIVATE;
                            }

                        } // for(; slaveRegNameIt != regsOrder.end(); ++slaveRegNameIt)

                    }
                }

                masterDevice.currentGroup = savedMasterGroup;

                //NsIndecesInfo unknownNsIndecesInfo = NsIndecesInfo( 256, 128 );
                enumerateRegs( slaveDevice,  /* nsStartIndeces, unknownNsIndecesInfo, */  generatorOptions, true  /* enumPublicMaster */ );
                enumerateRegs( slaveDevice,  /* nsStartIndeces, unknownNsIndecesInfo, */  generatorOptions, false );

                std::map<std::string, DeviceInfo>::iterator dsIt = masterDevice.deviceSlaves.find( slaveDevice.deviceName );
                if (dsIt==masterDevice.deviceSlaves.end())
                {
                    slaveDevice.importCount = 1;
                    masterDevice.deviceSlaves[slaveDevice.deviceName] = slaveDevice;
                }
                else
                {
                    if (dsIt->second!=slaveDevice)
                    {
                        LOG_ERR<<"device '"<<masterDevice.deviceName<<"' already imported '."<<slaveDevice.deviceName<<"' but with different options\n";
                        return false;
                    }

                    dsIt->second.importCount++;
                }

            }
            else if (directive=="typedef" )
            {
       
                if (directiveArg.empty())
                {
                    LOG_ERR<<"no type and alias taken in $typedef\n";
                    return false;
                }
       
                std::string alias, typeName;
                splitToPair( directiveArg, typeName, alias, ':' );
                trim(alias);
                trim(typeName);
       
                if (alias.empty())
                {
                    LOG_ERR<<"$typedef - no type alias taken\n";
                    return false;
                }
       
                if (typeName.empty())
                {
                    LOG_ERR<<"$typedef - no type name taken\n";
                    return false;
                }
       
                if (!typeMap.addAlias( typeName, alias ))
                {
                    LOG_ERR<<"$typedef - alias '"<<alias<<"' already exists, or type '"<<typeName<<"' is unknown\n";
                    return false;
                }
       
            }
            else 
            {
                LOG_ERR<<"unknown directive - '"<<directive<<"'\n";
                return false;
            }
            
            return true; // continue;
       
        } // if (str[0]=='$')
       
        // parse regular lines
       
        std::string regName, regDef;

        //std::set<std::string>()  /* allowedMultiVal */
        RegImportOptions importOptions;
        if (!readAttributes( curFile, false /* allowEndAttributes */, lineNo, str, attrs, makeStringSet("tag") /* allowedMultiVal */ ))
            return false;

        if (!checkAttrsAllowed( curFile, lineNo, attrs, makeStringVector("public", "protected", "private", "sync", "align", "offset", "format", "tag", "pair", "link" ) ))
             return false;

        if (!extractRegImportOptionsFromAttributes( curFile, lineNo, attrs, importOptions ))
             return false;

        trim(str);

       
        splitToPair( str, regName, regDef, ':' );
        trim(regName);
        trim(regDef);
       
        std::vector<std::string> fqRegNameParts;
        splitToVector( regName, fqRegNameParts, '.' );
       
        if (str.find(':')!=std::string::npos && fqRegNameParts.size()==1 && regDef.empty())
        {
            ctx.setNamespace(fqRegNameParts[0]);
            return true; // continue;
        }
       
        while(fqRegNameParts.size()<3)
            fqRegNameParts.insert(fqRegNameParts.begin(), std::string() );
       
        fqRegNameParts.resize(3);
       
        auto rgnPartsIt = fqRegNameParts.begin();
        for(; rgnPartsIt!=fqRegNameParts.end(); ++rgnPartsIt)
            trim(*rgnPartsIt);
       
        if (fqRegNameParts[0].empty())
            fqRegNameParts[0] = ctx.deviceName;
       
        if (fqRegNameParts[1].empty())
            fqRegNameParts[1] = ctx.namespaceName;
       
        if (fqRegNameParts[2].empty())
        {
            LOG_ERR<<"registry name not taken\n";
            return false;
        }
       
       
        const std::string &regDeviceName = fqRegNameParts[0];
        const std::string &regNamespace  = fqRegNameParts[1];
        const std::string &regNameOnly   = fqRegNameParts[2];
        // parse registry definition here
       
        std::string regType, regDescription;
        splitToPair( regDef, regType, regDescription, '-' );
        trim(regType);
        trim(regDescription);           
        unquoteString(regDescription);
        trim(regDescription);           
       
        if (!typeMap.findType(regType))
        {
            LOG_ERR<<"unknown registry type: "<<regType<<"\n";
            return false;
        }
        
       
        // std::map< std::string, DeviceInfo > &devices, TypeMap &typeMap

        DeviceInfo &regDevice = devices[regDeviceName];

        if (regDevice.deviceName.empty())
            regDevice.deviceName = regDeviceName;

        if (regDevice.deviceNamespaces[ regNamespace ].nsName.empty() )
            regDevice.deviceNamespaces[ regNamespace ].nsName = regNamespace;

        if ( regDevice.deviceNamespaces[ regNamespace ].nsRegs.find( regNameOnly ) != regDevice.deviceNamespaces[ regNamespace ].nsRegs.end() )
        {
            LOG_ERR<<"Registry "<<regDeviceName<<"."<<regNamespace<<"."<<regNameOnly<<" already defined\n";
            return false;
        }

        regDevice.deviceNamespaces[ regNamespace ].nsRegs[ regNameOnly ] = Registry( regType
                                                                                   , regDescription
                                                                                   , regDevice.currentGroup
                                                                                   , regDevice.getFullQualifiedImportOptions(importOptions, regNamespace)
                                                                                   );
        regDevice.deviceNamespaces[ regNamespace ].regOrder.push_back(regNameOnly);

    }
    


    return true;
}

//-----------------------------------------------------------------------------
inline
bool readIni( std::istream &in
            , ReadContext ctx
            , std::string curFile
            , std::map< std::string, DeviceInfo > &devices
            , TypeMap &typeMap
            , std::set< std::string > &onceIncluded
            , const GeneratorOptions &generatorOptions
            )
{
    bool readMultilineComment = false;

    const GeneratorOptions &gopts = generatorOptions;
    //std::string osFullFilename = osGetFullPathName( curFile );
    //std::string osFullFilenameCanonical = makeSimpleCanonicalFilename( osFullFilename );

    if (onceIncluded.find( getOsCanonicalFullPathName(curFile)) != onceIncluded.end())
        return true; // already included and $once directive used


    std::string str;
    std::string strReaded;

    //std::string deviceName;
    //std::string namespaceName = "ro";

    IniReadState readState = readNormal;

    TypeInfo     constructingType;

    int lineNo = 0;

    while( std::getline( in, strReaded ) )
    {
        lineNo++;

        std::string::size_type tabPos = strReaded.find('\t');
        if (tabPos!=std::string::npos)
        {
            LOG_WARN("tabs") <<"[TAB] symbol found at position "<<tabPos<<"\n";
        }

        trim(strReaded);

        if (readMultilineComment)
        {
            // in comment
            if (endsWith( strReaded, "*/" ))
                readMultilineComment = false; // end of comment
            continue; // continue anyway
        }

        if (startsWith( strReaded, "/*" ))
        {
            if (endsWith( strReaded, "*/" ))
            {
                // single line multiline comment
                // simply skip this line only
            }
            else
            {
                readMultilineComment = true;
            }

            if (!str.empty())
            {
                LOG_WARN("cont-comment") <<"multiline comment breaks line continuation\n";
            }

            continue;
        }

        //if (!readMultilineComment)
        {
            if (endsWith( strReaded, "*/" ))
            {
                LOG_WARN("err-comment") <<"multiline comment ends on this line, but never starts before\n";
            }
            else if (strReaded.find("/*")!=strReaded.npos || strReaded.find("*/")!=strReaded.npos)
            {
                LOG_WARN("err-comment") <<"line seems like it has a multiline comment inside it\n";
            }
        }

        if (strReaded.empty())
        {
            if (str.empty())
            {
                continue;
            }
            // else parse previosly readed and collected str

        } else if (isComment( strReaded ))
        {
            if (startsWith( strReaded, ";" ))
            {
                LOG_WARN("depr-comment") <<"comment start symbol ';' was deprecated\n";
            }

            if (str.empty())
            {
                continue;
            }
            else
            {
                LOG_WARN("cont-comment") <<"comment line breaks line continuation\n";
                // parse previosly readed and collected str
                if (str.back()=='\\')
                {
                    str.erase( str.size()-1 );
                    trim(str);
                }
            }

        } else if (!str.empty()) // collected continuation found
        {
            if (isContinuationLine(str))
            {
                stripLineContinuation(str);
                // append continuation
                if (!strReaded.empty() && strReaded!="\\") //-V560
                    str.append( 1, ' ');
                str.append(strReaded);
                strReaded.clear();
            }

            if (isContinuationLine(str))
            {
                // continuation continues
                continue;
            }

            // parse prev collected
            bool res = readIniParseLine( str, readState, constructingType, ctx, curFile, lineNo-1, devices, typeMap, onceIncluded, generatorOptions );
            if (!res)
                return res;

            str = strReaded;
        }
        else
        {
            str = strReaded;
        }

        if (str.empty())
            continue;

        if (isContinuationLine(str))
            continue;

        bool res = readIniParseLine( str, readState, constructingType, ctx, curFile, lineNo, devices, typeMap, onceIncluded, generatorOptions );
        if (!res)
            return res;

        str.clear();

    } // while( std::getline( in, strReaded ) )

    if (!str.empty())
    {
        if (isContinuationLine(str))
        {
            LOG_WARN("cont-last") <<"line continuation found in the last line, ignored\n";
            stripLineContinuation(str);
        }

        trim(str);
    }

    if (readMultilineComment)
    {
        LOG_WARN("eof-comment") <<"end of file reached while processing multiline comment\n";
    }
    else
    {
        if (!str.empty())
            return readIniParseLine( str, readState, constructingType, ctx, curFile, lineNo, devices, typeMap, onceIncluded, generatorOptions );
    }


    return true;
}


inline
bool readIni( std::istream &in
            , ReadContext ctx
            , std::string curFile
            , std::map< std::string, DeviceInfo > &devices
            , TypeMap &typeMap
            , const GeneratorOptions &generatorOptions
            )
{
    std::set< std::string > onceIncluded;
    return readIni( in, ctx, curFile, devices, typeMap, onceIncluded, generatorOptions );
}

