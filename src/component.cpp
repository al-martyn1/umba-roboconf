#include "logerr.h"

#include "component.h"

#include "rdlc-core/macros.h"
#include "regexp.h"
#include "csv.h"
#include "roboconf_options.h"

//-----------------------------------------------------------------------------
//struct ComponentPinInfo


//-----------------------------------------------------------------------------
void ComponentPinInfo::merge( const ComponentPinInfo &pi )
{
    if (pinDescription.empty())
        pinDescription = pi.pinDescription;
    pinFunctions.insert(  /* pinFunctions.end(),  */ pi.pinFunctions.begin(), pi.pinFunctions.end());
    //makeUniqueVector(pinFunctions);
}

//-----------------------------------------------------------------------------
bool ComponentPinInfo::isPinFunction( const std::string &fnName ) const
{
    return pinFunctions.find(fnName)!=pinFunctions.end();
}

//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
bool ComponentInfo::valid() const
{
    return !(designator.empty());
}

//-----------------------------------------------------------------------------
bool ComponentInfo::isDesignatorTypeEqualTo( std::string designatorType /* Type/Prefix */  ) const
{
    designatorType += "\\d+";
    return regexpSimpleMatch(designator, designatorType);
}

//-----------------------------------------------------------------------------
bool ComponentInfo::findPinNoByFunction( const std::string &pinFn, std::string &pinNo ) const
{
    for( const auto& pinPair : pins )
    {
        if (!pinPair.second.isPinFunction(pinFn))
            continue;
        pinNo = pinPair.second.pinNo;
        return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
void ComponentInfo::addPinsPart( const part_pins_t &part )
{
    //if ( std::find(partDef.begin(), partDef.end(), part) != partDef.end() )
    //    return;

    std::set< std::string > usedPins;

    for( const auto& partPins : partsDef )
    {
        if (partPins==part)
            return;
        usedPins.insert( partPins.begin(), partPins.end() );
    }

    part_pins_t insertCandidate;
    for( auto partPin : part )
    {
        if (usedPins.find(partPin)!=usedPins.end())
            continue;
        insertCandidate.insert(partPin);
    }

    if (insertCandidate.empty())
        return;

    partsDef.emplace_back(insertCandidate);
}

//-----------------------------------------------------------------------------
void ComponentInfo::mergePinParts( const std::vector< part_pins_t > &partsDefOther)
{
    for( auto p : partsDefOther )
    {
        addPinsPart( p );
    }
}

//-----------------------------------------------------------------------------
bool ComponentInfo::hasParts() const
{
    return partsDef.size() > 1;
}

//-----------------------------------------------------------------------------
int ComponentInfo::getPinPartNo( std::string pinNo ) const
{
    if (!hasParts())
        return 0;

    int partNo = 0;
    for( auto p : partsDef )
    {
        if (p.find(pinNo)!=p.end())
            return partNo;
        partNo++;
    }

    return -1;
}

//-----------------------------------------------------------------------------
std::vector< ComponentInfo::internal_net_t > ComponentInfo::getInternalNets() const
{
    /*
    if (designator=="R59")
    {
        internal_net_t tmpInet;
        tmpInet.emplace_back(1);
        tmpInet.emplace_back(2);
    }
    */

    if (!internalNets.empty())
        return internalNets;

    if ((componentClass!= ComponentClass::cc_RESISTOR && componentClass!= ComponentClass::cc_INDUCTOR) || assembly)
        return internalNets;

    internal_net_t tmpInet;
    tmpInet.emplace_back(std::string("1"));
    tmpInet.emplace_back(std::string("2"));

    return std::vector< internal_net_t >( 1, tmpInet );
}

//-----------------------------------------------------------------------------
void ComponentInfo::addPinFunctionMatch( const pin_function_match_t & match)
{
    auto pmIt = pinFunctionMatches.begin();
    for(; pmIt!=pinFunctionMatches.end(); ++pmIt )
    {
        if (pmIt->first==match.first)
        {
            pmIt->second.insert( pmIt->second.end(), match.second.begin(), match.second.end() );
            makeUniqueVector( pmIt->second );
            return;
        }
    }

    pinFunctionMatches.push_back( match );
}

//-----------------------------------------------------------------------------
bool ComponentInfo::isPackageAny() const
{
	if (package == "<ANY>" || package == "<any>" || package == "*")
		return true;
    else
		return false; // Раньше тут возвращалось true
}

//-----------------------------------------------------------------------------
bool ComponentInfo::isPackageCompatible(RoboconfOptions &rbcOpts, const ComponentInfo &ci ) const
{
	if (isPackageAny())
		return true;

	if (rbcOpts.packagesDb.getCanonicalPackageName(package) == rbcOpts.packagesDb.getCanonicalPackageName(ci.package))
        return true;

    return false;
}

//-----------------------------------------------------------------------------
void ComponentInfo::merge( RoboconfOptions &rbcOpts, const ComponentInfo &ci )
{
    if (!isPackageCompatible(rbcOpts, ci))
        return;

    if (package.empty() || isPackageAny())
       package = ci.package;

    if (isPackageAny())
        package.clear();

    if (description.empty())
       description = ci.description;

    if (purpose.empty())
       purpose = ci.purpose;

    if (datasheet.empty())
       datasheet = ci.datasheet;

    if (refman.empty())
       refman = ci.refman;

    //if (.empty())
    //   errata = ci.errata;

    modules.insert( modules.end(), ci.modules.begin(), ci.modules.end());
    makeUniqueVector(modules);

    headers.insert( headers.end(), ci.headers.begin(), ci.headers.end());
    makeUniqueVector(headers);

    for( auto ciPin : ci.pins )
    {
        const auto& thisPinIt = pins.find(ciPin.second.pinNo);
        if (thisPinIt==pins.end())
            pins[ciPin.second.pinNo] = ciPin.second;
        else
            thisPinIt->second.merge(ciPin.second);
    }

    internalNets.insert( internalNets.end(), ci.internalNets.begin(), ci.internalNets.end() );

    for( auto pfm : ci.pinFunctionMatches )
    {
        addPinFunctionMatch( pfm );
    }

    mergePinParts( ci.partsDef );

} // merge

//-----------------------------------------------------------------------------
void ComponentInfo::matchPinFunctions( RoboconfOptions &rbcOpts )
{
    for( auto &pinPair : pins )
    {
        ComponentPinInfo &pinInfo = pinPair.second;
        std::set< std::string > res = pinInfo.pinFunctions; // copy oroginals

        std::set< std::string > eraseFunctions;

        for( auto pfm : pinFunctionMatches )
        {
            for( auto pfn : pinInfo.pinFunctions)
            {
                auto inserter = std::inserter( res, res.end() );
                if (regexpEvalStrings( inserter, pfm.first, pfm.second.begin(), pfm.second.end(), pfn, rbcOpts.globalVars ))
                {
                    auto minusIt = res.find("-");
                    if (minusIt!=res.end())
                    {
                        res.erase(minusIt);
                        auto pfnIt = res.find(pfn);
                        if (pfnIt!=res.end())
                            res.erase(pfnIt);
                    }
                }
            }
        }

        res.swap(pinInfo.pinFunctions);
    }
}

//-----------------------------------------------------------------------------
//TODO:
std::string ComponentInfo::getClassDisplayString() const
{
    return getComponentClassDisplayString(componentClass);
}

//-----------------------------------------------------------------------------
std::string ComponentInfo::getComponentClassString() const
{
    return getComponentClassCanonicalName(componentClass);
}
//-----------------------------------------------------------------------------
void ComponentInfo::addPin( std::string pinNo, const std::string &pinFn )
{
    ComponentPinInfo pinInfo;
    pinInfo.pinNo = pinNo;
    pinInfo.pinFunctions.insert(pinFn);
    pins[pinInfo.pinNo] = pinInfo;
}

//-----------------------------------------------------------------------------
void ComponentInfo::addPin( std::string pinNo, const std::vector< std::string > &fns )
{
    addPin(pinNo, fns.begin(), fns.end() );
}

//-----------------------------------------------------------------------------
bool ComponentInfo::checkTypeNameMatch( const std::string otherTypeNameMatchTo ) const
{
    if (typeNameMatchType==ComponentTypeNameMatchType::matchExact)
    {
        return typeName==otherTypeNameMatchTo;
    }
    else if (typeNameMatchType==ComponentTypeNameMatchType::matchRegex)
    {
        return regexpSimpleMatch( otherTypeNameMatchTo, typeName );
    }
    else // matchSimple
    {
        std::string strRegex = "^";
        std::string::size_type i = 0, size = typeName.size();
        for(; i!=size; ++i )
        {
            char ch = typeName[i];
            if (ch==singleCharMask)
                strRegex.append(".{1,1}");
            else if (ch==anyCharMask)
                strRegex.append(".*");
            else if (ch=='\\')
                strRegex.append("\\\\");
            else if (ch=='.')
                strRegex.append("\\.");
            //else if (ch=='.')
            //    strRegex.append("");
            else 
                strRegex.append(1, ch);
        }
        strRegex.append("$");

        return regexpSimpleMatch( otherTypeNameMatchTo, strRegex );
    }
}

//-----------------------------------------------------------------------------
void ComponentInfo::detectComponentClass( RoboconfOptions &rbcOpts )
{
    //std::map< std::string, std::string> fields;
    NetlistComponentAttributesMap fields;
    fields["designator"] = designator;
    detectComponentClass( rbcOpts, fields );
}

//-----------------------------------------------------------------------------
void ComponentInfo::detectComponentClass( RoboconfOptions &rbcOpts, const NetlistComponentAttributesMap &fields )
{
    //componentInfo.
	componentClass = ComponentClass::cc_UNKNOWN;
    std::vector< ComponentClassDetectionRule >::const_iterator ccdrIt = rbcOpts.componentsClassDetectionRules.begin();

    for(; ccdrIt!=rbcOpts.componentsClassDetectionRules.end(); ++ccdrIt)
    {
        componentClass = ccdrIt->detectClassId( rbcOpts, &assembly, fields );
        if (componentClass != ComponentClass::cc_UNKNOWN)
            break;
    }

}

//-----------------------------------------------------------------------------


