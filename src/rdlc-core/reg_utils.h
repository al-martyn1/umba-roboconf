#pragma once


//-----------------------------------------------------------------------------
inline
bool enumerateRegs( DeviceInfo &device
                  , NamespaceRegs &nsRegs
                  //, const std::map<std::string, NsIndecesInfo> &nsStartIndeces
                  //, NsIndecesInfo &unknownNsIndecesInfo
                  , GeneratorOptions generatorOptions
                  , bool enumPublicMaster = false
                  )
{
    const GeneratorOptions &gopts = generatorOptions;

    unsigned regIdx = 0;

    //const NsIndecesInfo *pIdxInfo = &unknownNsIndecesInfo;
    /*
    const NsIndecesInfo *pIdxInfo = &generatorOptions.nsStartIndeces[""];

    auto nsIdxIt = generatorOptions.nsStartIndeces.find(nsRegs.nsName);
    if (nsIdxIt==generatorOptions.nsStartIndeces.end())
    {
        generatorOptions.nsStartIndeces[""].startIdx += generatorOptions.nsStartIndeces[""].numIdx;
    }
    else
    {
        pIdxInfo = &nsIdxIt->second;
    }

    NsIndecesInfo idxInfo = *pIdxInfo;
    */
    NsIndecesInfo idxInfo = generatorOptions.getNamespaceIndecesInfo(nsRegs);

    nsRegs.regIdxBase     = idxInfo.startIdx;
    nsRegs.minUsedRegIdx  = idxInfo.startIdx;

    size_t publicCount = 0;
    bool isFirstMasterToSlaveRegister = true;

    /* При обходе регистров мастера мы можем обнаружить, что регистр импортирован:
       if (!reg.slaveDeviceName.empty())

       Нам нужно узнать, является ли этот регистр первым импортированным/
       Для этого, ранее, при енумерации регистров слейва, нужно устанровить регистру флажок -
       что он первый из импортируемых. И тогда при обработке регистров мастера достаточно только 
       проверить этот флаг.
     */


    auto regsOrder = nsRegs.getRegistryOrder( false /* generatorOptions.bSort */  );
    auto regIt = regsOrder.begin();
    for(; regIt != regsOrder.end(); ++regIt)
    {
        auto regMapIt = nsRegs.nsRegs.find(*regIt);
        if (regMapIt==nsRegs.nsRegs.end())
        {
            LOG_ERR_OPT<<"inconsistent data state\n";
            return false;
        }

        //bool 
    
        Registry &reg = regMapIt->second;

        //if (reg.importOptions.regVisibility!=REG_VISIBILITY_PUBLIC)
        //    continue;

        TypeInfo typeInfo;
        if (!typeMap.findType( reg.regType, typeInfo ))
        {
            LOG_ERR_OPT<<"unknown type '"<<reg.regType<<"', registry: '"<<device.deviceName<<"."<<nsRegs.nsName<<"."<<*regIt<<"\n";
            return false;
        }

        if (regIdx >= idxInfo.numIdx)
        {
            LOG_ERR_OPT<<"registry: '"<<device.deviceName<<"."<<nsRegs.nsName<<"."<<*regIt<<" - index ("<<(regIdx+idxInfo.startIdx)<<") is out of range\n";
            return false;
        }

        // Обрабатываем только видимые регистры (которые импортируются)
        if (enumPublicMaster) // for slave
        {
            if (reg.importOptions.regVisibility==REG_VISIBILITY_PUBLIC)
            {
                reg.regMasterOffset = regIdx;

                if (!publicCount)
                    reg.firstImported = true;
                //    nsRegs.masterBases.push_back(regIdx);

                publicCount++;

                regIdx += generatorOptions.getRegNumberIncrementValue(typeInfo);

                /*
                if (generatorOptions.protocolVersion!=1)
                    regIdx += 1;
                else
                    regIdx += typeInfo.byteSize;
                */
            }
        }
        else // master registers
        {
            reg.regIdx      = regIdx+idxInfo.startIdx;
            reg.regNsOffset = regIdx;
           
            if (!reg.slaveDeviceName.empty())
            {
                //if (reg.firstImported)
                if (device.deviceSlaves[reg.slaveDeviceName].deviceNamespaces[nsRegs.nsName].nsRegs[reg.slaveRegOrgName].firstImported)
                {
                    //nsRegs.masterBases.push_back(reg.regIdx);
                    device.deviceSlaves[reg.slaveDeviceName].deviceNamespaces[nsRegs.nsName] /* .nsRegs */ .masterBases.push_back(reg.regIdx);
                    device.deviceSlaves[reg.slaveDeviceName].deviceNamespaces[nsRegs.nsName] /* .nsRegs */ .masterBaseNames.push_back(*regIt);
                    
                    //isFirstMasterToSlaveRegister = false;
                }

                device.deviceSlaves[reg.slaveDeviceName].deviceNamespaces[nsRegs.nsName].nsRegs[reg.slaveRegOrgName].regMasterIdx = reg.regIdx;
                //auto &slaveRegMasterOffset = device.deviceSlaves[reg.slaveDeviceName].deviceNamespaces[nsRegs.nsName].nsRegs[reg.slaveRegOrgName].regMasterOffset;
                //if (slaveRegMasterOffset==(unsigned)-1)
                //    slaveRegMasterOffset = reg.regIdx;
                if (device.deviceSlaves[reg.slaveDeviceName].deviceNamespaces[nsRegs.nsName].nsName.empty())
                    device.deviceSlaves[reg.slaveDeviceName].deviceNamespaces[nsRegs.nsName].nsName = nsRegs.nsName;
            }

            regIdx += generatorOptions.getRegNumberIncrementValue(typeInfo);
            /*
            if (generatorOptions.protocolVersion!=1)
                regIdx += 1;
            else
                regIdx += typeInfo.byteSize;
            */

        }

        //regMasterOffset

    
    } // for(; regIt != regsOrder.end(); ++regIt)

    nsRegs.maxUsedRegIdx = regIdx+idxInfo.startIdx - 1;

    return true;
}

//-----------------------------------------------------------------------------
inline
bool enumerateRegs( DeviceInfo &device
                  //, const std::map<std::string, NsIndecesInfo> &nsStartIndeces
                  //, NsIndecesInfo &unknownNsIndecesInfo
                  , GeneratorOptions generatorOptions
                  , bool enumPublicMaster = false
                  )
{
    auto nsIt = device.deviceNamespaces.begin();
    for(; nsIt!=device.deviceNamespaces.end(); ++nsIt)
    {
        if (!enumerateRegs( device, nsIt->second, generatorOptions, enumPublicMaster ))
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
inline
bool enumerateRegs( std::map< std::string, DeviceInfo > &devices
                  //, const std::map<std::string, NsIndecesInfo> &nsStartIndeces
                  , GeneratorOptions generatorOptions
                  )
{
    std::map< std::string, DeviceInfo >::iterator dit = devices.begin();
    for(; dit!=devices.end(); ++dit)
    {
        //NsIndecesInfo unknownNsIndecesInfo = NsIndecesInfo( 256, 128 );

        if (!enumerateRegs( dit->second, generatorOptions ))
            return false;
    } // for(; dit!=devices.end(); ++dit)

    return true;
}

//-----------------------------------------------------------------------------


