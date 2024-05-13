#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

#include "pugixml/src/pugixml.hpp"
#include "cpp.h"
#include "rdlc-core/splits.h"
#include "rdlc-core/fsUtils.h"
#include "rdlc-core/case.h"
#include "rdlc-core/cpp.h"

#include "icUtils.h"
//#include "rdlc-core/"



//-----------------------------------------------------------------------------
struct CubeMcuCoreInfo
{
    std::string   coreName;
    unsigned      coreFreq; // in mhz
    unsigned      ramSize;
    unsigned      flashSize;
    unsigned      numIos;

}; // struct CubeMcuCoreInfo



//-----------------------------------------------------------------------------
struct CubeMcuPeriphCountInfo
{
    std::string   name;
    unsigned      itemsCount;

    CubeMcuPeriphCountInfo() : name(), itemsCount(0) {}
    CubeMcuPeriphCountInfo( const std::string &n, unsigned u ) : name(n), itemsCount(u) {}

}; // struct CubeMcuPeriphInfo



//-----------------------------------------------------------------------------
struct CubeMcuPinInfo
{
    std::string                name;
    std::string                pos;
    std::vector<std::string>   functions;
    std::vector<std::string>   modes;

}; // struct CubeMcuPinInfo




//-----------------------------------------------------------------------------
struct CubeMcuInfo
{
    std::string                           family;
    std::string                           subFamily;
    std::string                           subFamilyDescription;
    std::string                           packageName;
                                         
    std::string                           name; // name attr - STM32F042G(4-6)Ux
    std::string                           refName;  // STM32F042G6Ux
    std::string                           rpnName;  // STM32F042G6
                                         
    CubeMcuCoreInfo                       coreInfo;
    std::vector<CubeMcuPeriphCountInfo>   periphCounts;
    std::vector<CubeMcuPinInfo>           pins;


    //Name="STM32F030C6Tx" PackageName="LQFP48" RefName="STM32F030C6Tx" RPN="STM32F030C6">

                 
}; // struct CubeMcuInfo



struct CubeDocFileInfo
{
    std::string     fileId;
    std::string     docName;
    std::string     displayName;
    std::string     title;
    std::string     url;
    std::string     docTypeDescription;
    std::string     docType;

}; // struct CubeDocFileInfo


struct CubeMcuDocs
{
    std::vector<std::string> refNames;
    std::string              rpn;

    std::map< std::string , std::vector<CubeDocFileInfo> >  docs;

}; // struct CubeMcuDocs


struct McuFamilyInfo
{
    std::string mcuName       ;
    std::string mcuFamily     ;
    std::string mcuSubfamily  ;
    std::string mcuCore       ;

    McuFamilyInfo( )
    : mcuName     ()
    , mcuFamily   ()
    , mcuSubfamily()
    , mcuCore     ()
    {}

    McuFamilyInfo( const CubeMcuInfo &mcuInfo )
    : mcuName     (makeCppDefineName(mcuInfo.rpnName)          )
    , mcuFamily   (makeCppDefineName(mcuInfo.family)           )
    , mcuSubfamily(makeCppDefineName(mcuInfo.subFamily)        )
    , mcuCore     (makeCppDefineName(mcuInfo.coreInfo.coreName))
    {
    /*
        mcuName      = makeCppDefineName(mcuInfo.rpnName)          ;
        mcuFamily    = makeCppDefineName(mcuInfo.family)           ;
        mcuSubfamily = makeCppDefineName(mcuInfo.subFamily)        ;
        mcuCore      = makeCppDefineName(mcuInfo.coreInfo.coreName);
    */
    }

};


struct McuDocInfo
{
    std::string            docType;
    std::string            docUrl;
    std::string            docFileName;
    std::string            docFileNameCanonical;

    std::set<std::string>  mcuList;
    std::set<std::string>  mcuFamilyList;
    std::set<std::string>  mcuSubfamilyList;
    std::set<std::string>  mcuCoreList;

    McuDocInfo()
    : docType              ()
    , docUrl               ()
    , docFileName          ()
    , docFileNameCanonical ()
    , mcuList              ()
    , mcuFamilyList        ()
    , mcuSubfamilyList     ()
    , mcuCoreList          ()
    {}

    McuDocInfo( const std::string &t, const std::string &url )
    : docType              (t)
    , docUrl               (url)
    , docFileName          (toUpper(getNameFromFull(url)))
    , docFileNameCanonical (toUpper(makeSimpleCanonicalFilename(getNameFromFull(url))))
    , mcuList              ()
    , mcuFamilyList        ()
    , mcuSubfamilyList     ()
    , mcuCoreList          ()
    {}
};


inline
void expand_size( std::string & str, std::string::size_type sz, char expandCh = ' ' )
{
    if (str.size() < sz)
        str.resize( sz, expandCh );
}

inline
void expand_size( std::vector<std::string> &vec, std::vector<std::string>::size_type sz, std::string expandVal = std::string() )
{
    if (vec.size() < sz)
        vec.resize( sz, expandVal );
}

#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif




inline
std::string generateMaskForNameSet( const std::set<std::string> &s, bool minMaxOnly, char separator = 0 /* '-' */ )
{
    if (s.empty())
        return std::string();

    std::string maskPos;
    std::string maskMin;
    std::string maskMax;

    std::vector<std::string> allChars;

    std::string strAny;

    for( const auto &s1 : s )
    {
        if (strAny.empty())
            strAny = s1;

        for( const auto &s2 : s )
        {
            expand_size( maskPos, s1.size() );
            expand_size( maskMin, s1.size() );
            expand_size( maskMax, s1.size() );

            expand_size( maskPos, s2.size() );
            expand_size( maskMin, s2.size() );
            expand_size( maskMax, s2.size() );

            expand_size( allChars, s1.size() );
            expand_size( allChars, s2.size() );

            std::string::size_type pos = 0, size = (s1.size() < s2.size()) ? s1.size() : s2.size();
            for(; pos!=size; ++pos)
            {
                if ( s1[pos]==s2[pos] )
                    continue;

                maskPos[pos] = '*';

                char c1 = s1[pos];
                char c2 = s2[pos];

                // only unique chars
                if ( allChars[pos].find(c1)==std::string::npos )
                    allChars[pos].append( 1, c1 );
                if ( allChars[pos].find(c2)==std::string::npos )
                    allChars[pos].append( 1, c2 );

                char curMin = std::min( c1, c2 );
                char curMax = std::max( c1, c2 );

                if (maskMin[pos]==' ')
                    maskMin[pos] = curMin;
                else
                    maskMin[pos] = std::min( maskMin[pos], curMin );

                if (maskMax[pos]==' ')
                    maskMax[pos] = curMax;
                else
                    maskMax[pos] = std::max( maskMax[pos], curMax );
            }
        }
    }

    std::string resMask;

    std::string::size_type pos = 0, size = (strAny.size() < maskPos.size()) ? strAny.size() : maskPos.size();
    for(; pos!=size; ++pos)
    {
        if (maskPos[pos]==' ')
        {
            resMask.append(1, strAny[pos]);
            continue;
        }


        resMask.append(1, '(');

        if (minMaxOnly)
        {
            resMask.append(1, maskMin[pos]);
            if (separator!=0)
                resMask.append(1, '-');
            resMask.append(1, maskMax[pos]);
        }
        else
        {
            //const 
            std::string &allCharsStr = allChars[pos];
            std::sort( allCharsStr.begin(), allCharsStr.end() );
            bool bf = true;
            for( auto c : allCharsStr)
            {
                if (bf)
                    bf = false;
                else
                {
                    if (separator!=0)
                        resMask.append(1, '-');
                }
                resMask.append(1, c);
            }
        }

        resMask.append(1, ')');
    }
    
    return resMask;

}


template< typename TStream>
inline
TStream& printMcuDocInfoSetHelper( TStream &stream, const std::set< std::string > &valSet )
{
    if (valSet.empty())
        return stream;

    std::string maskV1 = generateMaskForNameSet( valSet, false );
    std::string maskV2 = generateMaskForNameSet( valSet, true );
    const std::string &firstVal = *valSet.begin();

    std::set< std::string > uniqueMasks;
    uniqueMasks.insert(maskV1);
    uniqueMasks.insert(maskV2);
    uniqueMasks.insert(firstVal);

    if (uniqueMasks.size()>1) // mask not equal to first value, print one or two masks
    {
        stream<<" [ ";
        if (uniqueMasks.size()>2)
            stream<<maskV1<<" / "<<maskV2;
        else
            stream<<maskV1; // maskV1==maskV2

        stream<<" ]";
    }

    for( const auto & n : valSet )
    {
        stream<<" "<<n;
    }

    return stream;
}

template< typename TStream>
inline
void printMcuDocInfo( TStream &s, const McuDocInfo &mcuDocInfo, bool printDocType = false )
{
    s<<"File       : "<<mcuDocInfo.docFileName<<"\n";
    s<<"Location   : "<<mcuDocInfo.docUrl     <<"\n";
    if (printDocType)
        s<<"Type       : "<<mcuDocInfo.docType<<"\n";

    s<<"Cores      :"; printMcuDocInfoSetHelper( s, mcuDocInfo.mcuCoreList )<<"\n";

    s<<"Families   :"; printMcuDocInfoSetHelper( s, mcuDocInfo.mcuFamilyList )<<"\n";

    s<<"Subfamilies:"; printMcuDocInfoSetHelper( s, mcuDocInfo.mcuSubfamilyList )<<"\n";

    s<<"MCU #      : "<<(unsigned)mcuDocInfo.mcuList.size()<<"\n";

    s<<"MCUs       :"; printMcuDocInfoSetHelper( s, mcuDocInfo.mcuList )<<"\n";

    s<<"\n";
}


/*
Для каждого документа нам нужно выбрать максимально короткое, но уникальное имя, отражающее содержимое с использованием имен процов.
Кандидаты - длинная "маска", где перечислены все версии STMки, и короткая маска, где только указан range

Оки. Делаем map set'ов, где ключ - маска STMки, а set - имена конкретных файлов.

Для каждого сета имен STMок генерим две маски имени. Каждая из этих масок работает ключем.
По этому ключу есть set, где содержатся конкретные имена файлов.

Далее - есть set имен STMок, генерим две версии маски.
Сортируем их по длине: короткая - раньше
Если находим, и соответствует единственный документ, то выбираем эту версию маски.
Инaче - выбираем длинную
*/

struct StringsLessByLen
{
    bool operator()( const std::string &s1, const std::string &s2 ) const
    {
        return s1.size() < s2.size();
    }
};


class McuDocShortMaskSelector
{
    std::map< std::string, std::set<std::string> > maskToDocs;
    std::map< std::string, std::set<std::string> > docsToMasks;

public:

    McuDocShortMaskSelector() : maskToDocs(), docsToMasks() {}

    void addDocName( const std::string & docName, const std::set<std::string> &targetNames )
    {
        std::string m1 = generateMaskForNameSet( targetNames, false /* minMaxOnly */ );
        std::string m2 = generateMaskForNameSet( targetNames, true  /* minMaxOnly */ );
        maskToDocs[m1].insert(docName);
        maskToDocs[m2].insert(docName);
        docsToMasks[docName].insert(m1);
        docsToMasks[docName].insert(m2);
    }

    bool getMaskForDoc( const std::string & docName, std::string & resultMask )
    {
         std::map< std::string, std::set<std::string> >::const_iterator docIt = docsToMasks.find( docName );
         if (docIt == docsToMasks.end())
             return false;

         if (docIt->second.empty())
             return false;

         std::vector< std::string > sortedByLen = std::vector< std::string >( docIt->second.begin(), docIt->second.end() );
         std::stable_sort( sortedByLen.begin(), sortedByLen.end(), StringsLessByLen() );

         //resultMask = sortedByLen[0];
         resultMask = sortedByLen.back();
         return true;
    }

}; // class McuDocShortMaskSelector


template< typename TStream>
inline
void printMcuDocInfoWgetBat( TStream &s, const McuDocInfo &mcuDocInfo, std::map< std::string, McuDocShortMaskSelector > &docTypeShortMaskSelectors )
{
    std::string targetFileName; //  = generateMaskForNameSet( mcuDocInfo.mcuList, false );

    docTypeShortMaskSelectors[mcuDocInfo.docType].getMaskForDoc( mcuDocInfo.docFileName, targetFileName );

    if (!targetFileName.empty())
       targetFileName.append("-");
    targetFileName.append(mcuDocInfo.docType);
    targetFileName.append("-");
    targetFileName.append(mcuDocInfo.docFileName);

    std::string labelName = makeCppDefineName(targetFileName) + std::string("_COMPLETE");

    // wget --output-document=CD00161561.pdf http://www.st.com/resource/en/datasheet/CD00161561.pdf
    //s<<"if not exist "<<targetFileName<<" wget --output-document="<<targetFileName<<" "<<mcuDocInfo.docUrl<<"\n";
    s<<"if exist "<<targetFileName<<" goto "<<labelName<<"\n";
    s<<"timeout 10\n";
    s<<"wget --output-document="<<targetFileName<<" "<<mcuDocInfo.docUrl<<"\n";
    s<<":"<<labelName<<"\n";
    

}





/*
 "Files": [ {"id_file":"430",
             "name":"CD00160362",
             "displayName":"AN2548",
             "title":"Using the STM32F0/F1/Lx Series DMA controller",
             "URL":"http://www.st.com/resource/en/application_note/CD00160362.pdf",
             "type":"Application note",
             "versionNumber":"5",
             "related_MCU_count":"453"
            },
*/

/*
<Families xsi:noNamespaceSchemaLocation="families.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
    <Family Name="STM32F0">
        <SubFamily Name="STM32F0x0 Value Line">
            <Mcu Name="STM32F030C6Tx" PackageName="LQFP48" RefName="STM32F030C6Tx" RPN="STM32F030C6">
                <Core>ARM Cortex-M0</Core>
                <Frequency>48</Frequency>
                <Ram>4</Ram>
                <IONb>39</IONb>
                <Flash>32</Flash>
*/
//-----------------------------------------------------------------------------
inline
void parseMcu( std::vector< CubeMcuInfo > &mcus, const pugi::xml_node &mcuNode, const std::string &familyName, const std::string &subfamilyName )
{
    CubeMcuInfo cubeMcuInfo;
    cubeMcuInfo.family      = familyName;

    std::string subfamily, subfamilyDescription;
    if (splitToPair( subfamilyName, subfamily, subfamilyDescription, ' ' ))
    {
        cubeMcuInfo.subFamily             = subfamily;
        cubeMcuInfo.subFamilyDescription  = subfamilyDescription;
    }
    else
    {
        cubeMcuInfo.subFamily = subfamilyName;
    }

    cubeMcuInfo.packageName = mcuNode.attribute("PackageName").as_string();
    cubeMcuInfo.name        = mcuNode.attribute("Name").as_string();
    cubeMcuInfo.refName     = mcuNode.attribute("RefName").as_string();
    cubeMcuInfo.rpnName     = mcuNode.attribute("RPN").as_string();

    if (cubeMcuInfo.refName.empty())
        cubeMcuInfo.refName = cubeMcuInfo.name;

    if (cubeMcuInfo.name.empty())
    {
        std::cerr<<"Mcu '"<<cubeMcuInfo.refName<<"' - missing 'Name' attribute"<<std::endl;
        return;
    }

    if (cubeMcuInfo.rpnName.empty())
    {
        std::cerr<<"Mcu '"<<cubeMcuInfo.refName<<"' - missing 'RPN' attribute"<<std::endl;
        return;
    }

    if (cubeMcuInfo.packageName.empty())
    {
        std::cerr<<"Mcu '"<<cubeMcuInfo.packageName<<"' - missing 'PackageName' attribute"<<std::endl;
        return;
    }

    pugi::xml_node_iterator nit = mcuNode.begin();
	for (; nit != mcuNode.end(); ++nit)
	{
        std::string nodeName = nit->name();
        if (nodeName=="Core")
        {
			cubeMcuInfo.coreInfo.coreName = nit->text().as_string(); // nit->value();
        }
        else if (nodeName=="Frequency")
        {
            cubeMcuInfo.coreInfo.coreFreq  = nit->text().as_uint();
        }
        else if (nodeName=="Ram")
        {
            cubeMcuInfo.coreInfo.ramSize   = nit->text().as_uint();
        }
        else if (nodeName=="IONb")
        {
            cubeMcuInfo.coreInfo.numIos    = nit->text().as_uint();
        }
        else if (nodeName=="Flash")
        {
            cubeMcuInfo.coreInfo.flashSize = nit->text().as_uint();
        }
        else if (nodeName=="Peripheral")
        {
            //cubeMcuInfo.coreInfo.
            const pugi::xml_node &periphNode = *nit;
            std::string periphName = periphNode.attribute("Type").as_string();
            unsigned    nperiph    = periphNode.attribute("MaxOccurs").as_uint();
            cubeMcuInfo.periphCounts.push_back( CubeMcuPeriphCountInfo( periphName, nperiph ) );
        }
    }

    mcus.push_back(cubeMcuInfo);

}

//-----------------------------------------------------------------------------
inline
void parseSubFamily( std::vector< CubeMcuInfo > &mcus, const pugi::xml_node &subfamilyNode, const std::string &familyName, const std::string &subfamilyName )
{
    pugi::xml_node_iterator nit = subfamilyNode.begin();
	for (; nit != subfamilyNode.end(); ++nit)
	{
        std::string nodeName = nit->name();
        if (nodeName=="Mcu")
            parseMcu( mcus, *nit, familyName, subfamilyName );
    }

}

//-----------------------------------------------------------------------------
inline
void parseSubFamily( std::vector< CubeMcuInfo > &mcus, const pugi::xml_node &subfamilyNode, const std::string &familyName )
{
    const pugi::xml_attribute &subfamilyAttrName = subfamilyNode.attribute("Name");
    if (!subfamilyAttrName)
        return;

    parseSubFamily( mcus, subfamilyNode, familyName, subfamilyAttrName.value() );
}

inline
void parseFamily( std::vector< CubeMcuInfo > &mcus, const pugi::xml_node &familyNode, const std::string &familyName )
{
    pugi::xml_node_iterator nit = familyNode.begin();
	for (; nit != familyNode.end(); ++nit)
	{
        std::string nodeName = nit->name();
        if (nodeName=="SubFamily")
        {
            parseSubFamily( mcus, *nit, familyName );
        }
        else if (nodeName=="Mcu")
        {
            parseMcu( mcus, *nit, familyName, std::string() );
        }
    }
}


//-----------------------------------------------------------------------------
inline
void parseFamily( std::vector< CubeMcuInfo > &mcus, const pugi::xml_node &familyNode )
{
    const pugi::xml_attribute &familyAttrName = familyNode.attribute("Name");
    if (!familyAttrName)
         return;
    
    parseFamily( mcus, familyNode, familyAttrName.value() );
}

//-----------------------------------------------------------------------------
inline
void parseFamilies( std::vector< CubeMcuInfo > &mcus, const pugi::xml_node &familiesNode )
{
    pugi::xml_node_iterator nit = familiesNode.begin();
	for (; nit != familiesNode.end(); ++nit)
	{
        std::string nodeName = nit->name();
        if (nodeName=="Family")
        {
            parseFamily( mcus, *nit );
        }
        else if (nodeName=="SubFamily")
        {
            parseSubFamily( mcus, *nit, std::string() );
        }
        else if (nodeName=="Mcu")
        {
            parseMcu( mcus, *nit, std::string(), std::string() );
        }
    }

}

//-----------------------------------------------------------------------------
inline
void  parseMcuPins(std::vector< CubeMcuPinInfo > &mcuPins, const pugi::xml_node &mcuNode, const std::string &mcuName, const std::string &mcuFileName)
{
    pugi::xml_node_iterator nit = mcuNode.begin();
    for (; nit != mcuNode.end(); ++nit)
    {
        std::string nodeName = nit->name();

        if (nodeName=="Pin")
        {
            const pugi::xml_node &pinNode = *nit;
            CubeMcuPinInfo pinInfo;

            /*
            <Pin Name="VBAT" Position="1" Type="Power"/>
            <Pin Name="PC13-TAMPER-RTC" Position="2" Type="I/O">
                <Signal Name="RTC_OUT"/>
                <Signal Name="RTC_TAMPER"/>
                <Signal IOModes="Input,Output,Analog,EVENTOUT,EXTI" Name="GPIO"/>
            </Pin>
            */

            pinInfo.name = pinNode.attribute("Name").as_string();
            pinInfo.pos  = pinNode.attribute("Position").as_string();
            std::string pinType = pinNode.attribute("Type").as_string();
            //pinInfo.functions
            //pinInfo.modes

            if (pinInfo.name.empty())
            {
                throw std::runtime_error( std::string("Failed to parse '") + mcuName + std::string("' pins - pin name not taken (file: '") + mcuFileName + std::string("')") );
            }
            if (pinInfo.pos.empty())
            {
                throw std::runtime_error( std::string("Failed to parse '") + mcuName + std::string("' pins - pin position not taken (file: '") + mcuFileName + std::string("')") );
            }

            pinInfo.name = makeCppDefineName(pinInfo.name);
            std::string pinName, pinExtFn;
            if (splitToPair( pinInfo.name, pinName, pinExtFn, '_' ))
            {
                pinInfo.functions.push_back(pinName);
                pinInfo.functions.push_back(pinExtFn);
                pinInfo.functions.push_back(pinInfo.name);
                pinInfo.name = pinName;
            }
            else
            {
                pinInfo.functions.push_back(pinInfo.name);
            }

            pugi::xml_node_iterator pit = pinNode.begin();
            for (; pit != pinNode.end(); ++pit)
            {
                std::string nodeName = pit->name();
                if (nodeName!="Signal")
                    continue;

                const pugi::xml_node &signalNode = *pit;
                std::string signalName = signalNode.attribute("Name").as_string();
                if (signalName!="GPIO")
                {
                    pinInfo.functions.push_back(makeCppDefineName(signalName));
                    continue;
                }

                std::string ioModesVecStr = signalNode.attribute("IOModes").as_string();
                if (ioModesVecStr.empty())
                    continue;

                std::vector<std::string> ioModesVec;
                splitToVector( ioModesVecStr, ioModesVec, ',' );

                std::vector<std::string>::const_iterator mvIt = ioModesVec.begin();
                for(; mvIt != ioModesVec.end(); ++mvIt)
                {
                    if (mvIt->empty())
                        continue;

                    if (*mvIt == "Input")
                    {
                        pinInfo.modes.push_back("IN");
                    }
                    else if (*mvIt == "Output")
                    {
                        pinInfo.modes.push_back("OUT");
                    }
                    else if (*mvIt == "Power")
                    {
                        pinInfo.modes.push_back("PWR");
                    }
                    else
                    {
                        pinInfo.modes.push_back( makeCppDefineName(*mvIt) );
                    }

                } // for(; mvIt != ioModesVec.end(); ++mvIt)

            } // for (; pit != pinNode.end(); ++pit)

            if (pinInfo.modes.empty())
            {
                if (pinType=="Power")
                    pinInfo.modes.push_back( "PWR" );
                else
                    pinInfo.modes.push_back( makeCppDefineName(pinType) );
            }

            if (pinInfo.name=="VCC")
            {
                pinInfo.modes.push_back( "PWR_PLUS" );
            }
            else if (pinInfo.name=="VEE")
            {
                //pinInfo.modes.push_back( "PWR_BP_MINUS (-- Bipolar power minus)" );
                pinInfo.modes.push_back( "PWR_GND" );
            }
            else if (pinInfo.name=="VDD")
            {
                pinInfo.modes.push_back( "PWR_PLUS" );
            }
            else if (pinInfo.name=="VDDUSB")
            {
                // <Pin Name="VDDUSB" Position="G11" Type="Power"/>
                pinInfo.modes.push_back( "PWR_PLUS_USB" );
            }
            else if (pinInfo.name=="VSS")
            {
                pinInfo.modes.push_back( "PWR_GND" );
            }
            else if (pinInfo.name=="VDDA")
            {
                pinInfo.modes.push_back( "PWR_PLUS_ANALOG" );
            }
            else if (pinInfo.name=="VSSA")
            {
                pinInfo.modes.push_back( "PWR_GND_ANALOG" );
                pinInfo.modes.push_back( "PWR_GND" );
            }
            else if (pinInfo.name=="AGND")
            {
                pinInfo.modes.push_back( "PWR_GND_ANALOG" );
                pinInfo.modes.push_back( "PWR_GND" );
            }
            else if (pinInfo.name=="DGND")
            {
                pinInfo.modes.push_back( "PWR_GND" );
                pinInfo.modes.push_back( "PWR_GND" );
            }
            else if (pinInfo.name=="VBAT")
            {
                pinInfo.modes.push_back( "PWR_PLUS_BAT" );
                pinInfo.modes.push_back( "PWR_PLUS_RTC" );
            }
            else if (pinInfo.name=="VREF+" || pinInfo.name=="VREFPLUS" || pinInfo.name=="VREF")
            {
                pinInfo.modes.push_back( "PWR_PLUS_REF" );
            }
            else if (pinInfo.name=="VREF-" || pinInfo.name=="VREFMINUS" || pinInfo.name=="VREF_")
            {
                pinInfo.modes.push_back( "PWR_GND_REF" );
                pinInfo.modes.push_back( "PWR_GND" );
            }
            // VDDIO2
            else if (pinInfo.name=="VDDIO2")
            {
                pinInfo.modes.push_back( "PWR_PLUS_IO2" );
                pinInfo.modes.push_back( "PWR_PLUS" );
            }
            // VREF problem??? STM32L4S5ZI-LQFP144.cmp
//            else if (pinInfo.name=="")
//            {
//                pinInfo.modes.push_back( "" );
//            }


//(pin "28" "" ("VSSA" "VREF_" "VSSA_VREF_") ("POWER") )
//(pin "29" "" ("VREFPLUS" "VREFBUF_OUT") ("MONOIO") )



            mcuPins.push_back( pinInfo );


        }
    }

}


template< typename StreamType >
void writeCubeMcuInfo( StreamType &ofs, const CubeMcuInfo &mcuInfo
                     , const std::map< std::string , CubeDocFileInfo > &cubeDocFileInfos
                     , std::map< std::string , CubeMcuDocs >           &cubeMcuDocs
                     , bool writePackage
                     )
{
    ofs<<"(componentDefinition ";
    //ofs<<"((singleCharMask \"x\")";
    //ofs<<" (anyCharsMask \"_\")"
    //ofs<<") "

    ofs<<"\""<<mcuInfo.rpnName<<"\" ";
    if (mcuInfo.rpnName!=mcuInfo.refName)
        ofs<<"\""<<mcuInfo.refName<<"\"";
    ofs<<"\n";
    //ofs<<"    (nameMatch exact)\n";
    //ofs<<"    (nameMatch regex)\n";
    //ofs<<"    (nameMatch simple)\n"; // (single "x")(any "*")
    ofs<<"    (nameMatch simple (single \"x\")(any \"*\"))\n"; // 
    ofs<<"    (description \""<< mcuInfo.coreInfo.coreName<<" "
                               <<mcuInfo.coreInfo.coreFreq<<"MHz, "
                               <<mcuInfo.coreInfo.ramSize<<"Kb RAM, "
                               <<mcuInfo.coreInfo.flashSize<<"Kb Flash, "
                               <<mcuInfo.coreInfo.numIos<<" IOs"
                               <<"\")\n";
    ofs<<"    (purpose \"MCU\")\n";
    if (writePackage)
    {
        ofs<<"    (package \""<<icPackageGetCanonicalName(mcuInfo.packageName)<<"\")\n";
        unsigned nPackagePins = icPackageGetPinCount(mcuInfo.packageName);
        if (!nPackagePins)
            std::cerr<<"Unknown package - "<<mcuInfo.packageName<<"\n";
        else
            ofs<<"    (nPackagePins \""<<nPackagePins<<"\")\n";
    }

    if (!mcuInfo.coreInfo.coreName.empty())
        ofs<<"    (core \""<<makeCppDefineName(mcuInfo.coreInfo.coreName)<<"\" \""<<mcuInfo.coreInfo.coreName<<"\")\n";
    if (!mcuInfo.family.empty())
        ofs<<"    (family \""<<makeCppDefineName(mcuInfo.family)<<"\")\n";
    if (!mcuInfo.subFamily.empty())
        ofs<<"    (subfamily \""<<makeCppDefineName(mcuInfo.subFamily)<<"\")\n";
    if (!mcuInfo.subFamilyDescription.empty())
        ofs<<"    (subfamilyDescription \""<<mcuInfo.subFamilyDescription<<"\")\n";

    ofs<<"    (coreFreq \""<< mcuInfo.coreInfo.coreFreq<<"000\")\n";
    ofs<<"    (ramSize \""<<mcuInfo.coreInfo.ramSize<<"\")\n";
    ofs<<"    (flashSize \""<<mcuInfo.coreInfo.flashSize<<"\")\n";

    std::map< std::string , CubeMcuDocs >::const_iterator cmdIt = cubeMcuDocs.find(mcuInfo.refName);
    if (cmdIt != cubeMcuDocs.end())
    {
        std::map< std::string , std::vector<CubeDocFileInfo> >::const_iterator dit = cmdIt->second.docs.begin();
        for(; dit != cmdIt->second.docs.end(); ++dit)
        {
            std::vector<CubeDocFileInfo>::const_iterator dvIt = dit->second.begin();
            for(; dvIt != dit->second.end(); ++dvIt)
            {
                ofs<<"    ("<<dvIt->docType<<" \""<<dvIt->url<<"\")\n";
            }
        }
    }

    std::vector<CubeMcuPeriphCountInfo>::const_iterator prpIt = mcuInfo.periphCounts.begin();
    for(; prpIt != mcuInfo.periphCounts.end(); ++prpIt)
    {
        ofs<<"    (nPeriphs \""<<makeCppDefineName(prpIt->name)<<"\" \""<<prpIt->itemsCount<<"\")\n";
    }

    ofs<<"    (nPins \""<<(unsigned)mcuInfo.pins.size()<<"\")\n";
    std::vector<CubeMcuPinInfo>::const_iterator pinIt = mcuInfo.pins.begin();
    for(; pinIt != mcuInfo.pins.end(); ++pinIt)
    {
        ofs<<"    (pin \""<<pinIt->pos<<"\" \"\" ";

        if (!pinIt->functions.empty())  ofs<<"(";
        std::vector<std::string>::const_iterator it = pinIt->functions.begin();
        for(; it != pinIt->functions.end(); ++it)
        {
            if (it != pinIt->functions.begin())
                ofs<<" ";
            ofs<<"\""<<makeCppDefineName(*it)<<"\"";
        }
        if (!pinIt->functions.empty())  ofs<<")";

        if (!pinIt->modes.empty())  ofs<<"\n                 (";
        it = pinIt->modes.begin();
        for(; it != pinIt->modes.end(); ++it)
        {
            if (it != pinIt->modes.begin())
                ofs<<" ";
            ofs<<"\""<<makeCppDefineName(*it)<<"\"";
        }
        if (!pinIt->modes.empty())  ofs<<")";

        ofs<<" )\n";
    }

    ofs<<")\n\n";


}



