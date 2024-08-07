#pragma once

#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "trims.h"
#include "splits.h"
#include "textUtils.h"
#include "sys_paths.h"
#include "cpp.h"

//bool startsWithAndStrip( std::string &str, const std::string &prefix );

#include "fsUtils.h"
#include "prog.h"
#include "isa.h"


enum class PrintHelpStyle
{
    normal,
    wiki,
    bash_complete,
    clink_complete
};


inline
bool readOptionsFile( std::ifstream &optFile, std::vector<std::string> &resVec )
{
    std::string optLine;
    while( std::getline( optFile, optLine) )
    {
        trim(optLine);

        if (optLine.empty())
            continue;

        if (isComment( optLine ))
            continue;

        resVec.push_back(optLine);

    }
    
    return true;
}

inline
bool readOptionsFile( const std::string &fileName, std::vector<std::string> &resVec )
{
    std::ifstream optFile(fileName.c_str());
    if (!optFile)
        return false; // resVec;

    return readOptionsFile( optFile, resVec );
}

inline
bool readBinaryFile( std::ifstream &input, std::string &data )
{
    char buf[4096];
    
    while( input.read( buf, sizeof(buf)) )
    {
        size_t rdSz = (size_t)input.gcount();
        data.append(buf, (size_t)rdSz);
    }
    
    size_t rdSz = (size_t)input.gcount();
    if (rdSz!=0  /* && rdSz!=(size_t)-1 */ )
    {
        data.append(buf, (size_t)rdSz);
    }

    return true;
}

inline
bool readBinaryFile( const std::string &fileName, std::string &data )
{
    std::ifstream input( fileName, std::ios_base::in | std::ios_base::binary );
    if (!input)
    {
        return false;
    }

    return readBinaryFile( input, data );
}

inline
std::vector<std::string> prepareArgs( int argc, char **argv )
{
    std::vector<std::string> resVec;

    for( int i = 1; i<argc; ++i)
    {
        std::string argStr = argv[i];

        if (argStr.empty())
           continue;

        if (argStr[0]!='-') // not an option at all
        {
            resVec.push_back(argStr);
            continue;
        }

        if (argStr.size()<2) // exact one char len
        {
            resVec.push_back(argStr);
            continue;
        }

        if (argStr[1]=='-') // long option '--'
        {
            resVec.push_back(argStr);
            continue;
        }


        // Here we got an arg in form '-X...'
        std::string::size_type pos = 1, sz = argStr.size(); // sz 2 or more
        for(; pos!=sz; ++pos)
        {
            auto nextPos = pos+1;
            if (nextPos!=sz && argStr[nextPos]=='=')
            {
                resVec.push_back(std::string("-") + std::string( argStr, pos ));
                break;
            }
            else if (nextPos!=sz && (argStr[nextPos]=='+' || argStr[nextPos]=='-'))
            {
                resVec.push_back(std::string("-") + std::string( argStr, pos, 2 ));
                pos = nextPos;
            }
            else
            {
                resVec.push_back( std::string("-") + std::string( argStr, pos, 1 ));
            }
        }
    }

    return resVec;
}



enum class OptionType
{
    optUnknown     ,

    optFlag        ,
    optInt         ,
    optString      ,
    optEnum        ,

    optIntList     ,
    optStringList  ,
    optEnumList
};



struct CommandLineOptionEnumInfo
{
    std::set<std::string> valueNames;
    int value;
};


struct CommandLineOptionInfo
{
    std::vector< std::string > optNames;
    bool                       internalOption = false;
    std::string                param;
    bool                       paramOptional  = false;
    std::string                description;

    OptionType                 optType;

    bool                       hasDefVal;
    bool                       hasInitVal;
    bool                       hasMinMax;

    bool                       boolDefVal;
    int                        intDefVal;    // also enum defVal
    std::string                stringDefVal;

    bool                       boolInitVal;
    int                        intInitVal;    // also enum defVal
    std::string                stringInitVal;

    int                        minVal;
    int                        maxVal;

    std::map<std::string, size_t>             enumValsAll; // second - index in enums
    std::vector< CommandLineOptionEnumInfo >  enums;


    void reset()
    {
        optNames.clear();
        internalOption = false;
        param.clear();
        paramOptional  = false;
        description.clear();

        optType = OptionType::optUnknown;

        hasDefVal  = false;
        hasInitVal = false;
        hasMinMax  = false;

        boolInitVal  = false;
        intInitVal   = 0;    // also enum defVal
        stringInitVal.clear();

        enumValsAll.clear();
        enums.clear();
    }

    int getEnumValueByName( std::string name ) const
    {
        name = toLower(name);
        std::map<std::string, size_t>::const_iterator eit = enumValsAll.find(name);
        if (eit==enumValsAll.end())
            return -1;

        if ( eit->second >= enums.size() )
            return -1;
        
        return enums[eit->second].value;
    }

    void parseEnumConfigString( std::string enumValues )
    {
        enumValues = toLower(enumValues);

        int enumVal = 0;
        std::vector< std::string > namesVec;
        splitToVector( enumValues, namesVec, '|' );

        for( auto nameAlters : namesVec )
        {
            trim(nameAlters);
            if (nameAlters.empty())
                continue;

            std::vector< std::string > altersVec;
            splitToVector( nameAlters, altersVec, '/' );

            CommandLineOptionEnumInfo ei;
            ei.value = enumVal;

            size_t idx = (size_t)enums.size();

            for( auto na : altersVec )
            {
                trim(na);
                if (na.empty())
                   continue;

                ei.valueNames.insert(na);
                enumValsAll[na] = idx;

            }
            //ei.valueNames.insert( altersVec.begin(), altersVec.end() );
            enums.push_back(ei);

            ++enumVal;
        }
    }


    std::string getAllEnumNames( const std::string &sep, const std::string &sepLast, const std::string &alter, const std::string &before = std::string(), const std::string &after = std::string() ) const
    {
        std::string res;

        if (enums.empty())
            return res;

        std::vector< CommandLineOptionEnumInfo >::const_iterator eit = enums.begin();
        std::vector< CommandLineOptionEnumInfo >::const_iterator last = enums.end();
        --last;

        for(; eit!=enums.end(); ++eit )
        {
            std::string curEnumAltersStr;
            const std::set<std::string> &valueNames = eit->valueNames;
            for( const auto& en : valueNames)
            {
                if (!curEnumAltersStr.empty())
                    curEnumAltersStr += alter;
                curEnumAltersStr += before + en + after;
            }

            if (!res.empty())
            {
                if (eit==last)
                    res += sepLast;
                else
                    res += sep;
            }

            //res += before + curEnumAltersStr + after;
            res += curEnumAltersStr;
        }

        return res;

        /*
        std::map<std::string, int>::const_iterator it = enumVals.begin();
        if (enumVals.size()==1)
            return it->first;

        std::map<std::string, int>::const_iterator e = enumVals.end();
        --e;

        for(; it!=e; ++it)
        {
            if (!res.empty())
                res += sep;
            res += before + it->first + after;
        }

        if (!res.empty())
            res += sepLast;
        res += before + it->first + after;

        return res;
        */
    }


    std::string getEnumValNameByValue( int enumValue ) const
    {
        std::vector< CommandLineOptionEnumInfo >::const_iterator eit = enums.begin();
        for(; eit!=enums.end(); ++eit )
        {
            if (eit->value==enumValue)
            {
                return * (eit->valueNames.rbegin());
            }
        }

        return std::string();
    }

    std::string getDefValString() const
    {
        if (!hasDefVal)
            return std::string();

        if (!paramOptional)
            return std::string();

        switch(optType)
        {
            case OptionType::optFlag  : return boolDefVal ? "TRUE" : "FALSE";
            case OptionType::optInt   : return cppHelpersFormatInt( intDefVal );
            case OptionType::optString: return std::string("'") + stringDefVal + std::string("'");
            case OptionType::optEnum  :
            {
                /*
                std::map<std::string, int>::const_iterator it = enumVals.begin();
                for(; it != enumVals.end(); ++it)
                {
                    if (it->second==intDefVal)
                        return std::string("'") + it->first + std::string("'");
                }
                */

                auto res = getEnumValNameByValue(intDefVal);
                if (res.empty())
                    return "<AUTO>";
                return std::string("'") + res + std::string("'");
            }
        }

        return std::string();
    }

    std::string getInitialValString() const
    {
        if (!hasInitVal)
            return std::string();

        switch(optType)
        {
            case OptionType::optFlag  : return boolInitVal ? "TRUE" : "FALSE";
            case OptionType::optInt   : return cppHelpersFormatInt( intInitVal );
            case OptionType::optString: return std::string("'") + stringInitVal + std::string("'");
            case OptionType::optEnum  :
            {
                /*
                std::map<std::string, int>::const_iterator it = enumVals.begin();
                for(; it != enumVals.end(); ++it)
                {
                    if (it->second==intInitVal)
                        return std::string("'") + it->first + std::string("'");
                }

                return "<AUTO>";
                */

                auto res = getEnumValNameByValue(intInitVal);
                if (res.empty())
                    return "<AUTO>";
                return std::string("'") + res + std::string("'");
            }
        }

        return std::string();
    }


    std::string getAllOptionNames( const std::string &sep ) const
    {
        //return mergeStrings( optNames, sep );
        std::string res;

        for( const auto & s : optNames )
        {
            if (!res.empty())
               res.append( sep );

            if (s.size()==1)
                res.append( std::string("-") + s );
            else
                res.append( std::string("--") + s );
        }

        return res;
    }

};




struct ICommandLineOptionCollector
{
    virtual void ignoreOptInfo( ) = 0;
    virtual void setCollectMode( bool bCollect ) = 0;

    virtual void addOption( std::string optLongName ) = 0;
    virtual void addOption( char optShort ) = 0;

    //virtual const CommandLineOptionInfo& getLastAddedOptionInfo() = 0;
    virtual const CommandLineOptionInfo& getCurrentOptionInfo() = 0;

    virtual const std::set<std::string> * getTakenOpts() = 0;
    virtual void  addTakenOpt( const std::string &optName ) = 0;

    virtual void setOptionInitial( bool v ) = 0;
    virtual void setOptionInitial( int  v  ) = 0;
    virtual void setOptionInitial( const std::string &v ) = 0;

    virtual void setOptionParam( const std::string &param ) = 0;
    virtual void setOptionParam( bool defVal ) = 0;
    virtual void setOptionParam( const std::string &param, OptionType optType ) = 0;
    virtual void setOptionParam( const std::string &param, bool defVal ) = 0;
    virtual void setOptionParam( const std::string &param, int defVal  ) = 0;
    virtual void setOptionParam( const std::string &param, int defVal, int minVal, int maxVal ) = 0;
    virtual void setOptionParam( const std::string &param, const std::string &defVal ) = 0;
    virtual void setOptionParam( const std::string &param, int defVal, std::string enumValues ) = 0;

    virtual void addOptionDescription( const std::string &d ) = 0;
    virtual void resetCurCollectedOption( ) = 0;

    virtual bool isNormalPrintHelpStyle() = 0;
    virtual void setPrintHelpStyle( PrintHelpStyle printHelpStyle ) = 0;
    virtual PrintHelpStyle getPrintHelpStyle() = 0;

    virtual std::string makeText( std::string::size_type width, PrintHelpStyle printHelpStyle, const std::set<std::string> *pHelpOnlyThats ) = 0;
    virtual std::string makeText( std::string::size_type width, PrintHelpStyle printHelpStyle, std::string *pPrefix, std::string *pSuffix, bool useExt ) = 0;
    virtual std::string makeText( std::string::size_type width, PrintHelpStyle printHelpStyle, std::string *pPrefix, std::string *pSuffix, bool useExt, const std::set<std::string> *pHelpOnlyThats ) = 0;

    virtual std::string makeText( std::string::size_type width, const std::set<std::string> *pHelpOnlyThats ) = 0;
    virtual std::string makeText( std::string::size_type width, std::string *pPrefix, std::string *pSuffix, bool useExt ) = 0;
    virtual std::string makeText( std::string::size_type width, std::string *pPrefix, std::string *pSuffix, bool useExt, const std::set<std::string> *pHelpOnlyThats ) = 0;
    

    //const std::set<std::string>

};





// Это чекер для опций
struct CommandLineOption
{
    bool          fOption;
    bool          fResponseFile;
    bool          fShort;
    std::string   argOrg;
    std::string   name;
    std::string   optArg;
    bool          valueOptional;
    ICommandLineOptionCollector *pCollector;

    CommandLineOption( std::string a, ICommandLineOptionCollector *pCol = 0) : fOption(false), fResponseFile(false), fShort(false), argOrg(a), pCollector(pCol), valueOptional(false)
    {
        fOption = isCommandLineOption( argOrg, name, optArg, fShort );
        if (!fOption)
        {
            name = argOrg;
            if (startsWithAndStrip( name, "@"))
                fResponseFile = true;
        }
    }

    ~CommandLineOption()
    {
        if (pCollector)
            pCollector->resetCurCollectedOption();
    }

    void resetOptionInfoCollector()
    {
        pCollector = 0;
    }

    bool hasArg() const
    {
        return !optArg.empty();
    }

    bool isResponseFile() const
    {
        return fResponseFile;
    }

    bool isOption() const
    {
        return fOption;
    }

    bool isPureHelpOption() const
    {
        if (!fOption)
            return false;

        if (fShort)
            return name=="h" || name=="?";
        else
            return name=="help";
    }

    bool isHelpOption() const
    {
        if (!fOption)
            return false;

        if (isPureHelpOption() || name=="autocomplete-install" || name=="autocomplete-uninstall")
            return true;

        return false;
    }

    bool isSomeKindOfQuet() const
    {
        if (!fOption)
            return false;

        return name=="quet" || name=="q" || name=="version" || name=="v" || name=="build-info" || name=="build-info-x";
    }

    bool isWhereOption() const
    {
        if (!fOption)
            return false;

        return name=="where";
    }

    bool isHelpStyleOption() const
    {
        if (!fOption)
            return false;

        PrintHelpStyle phs = PrintHelpStyle::normal;

        if (name=="wiki")
            phs = PrintHelpStyle::wiki;
        else if (name=="bash")
            phs = PrintHelpStyle::bash_complete;
        else if (name=="clink")
            phs = PrintHelpStyle::clink_complete;
        else
            return false;

        if (pCollector)
        {
            pCollector->setPrintHelpStyle( phs );
        }

        return true;
    }

    static
    std::set<std::string> getHelpStyleOptions()
    {
        static std::set<std::string> _ = { "wiki", "bash", "clink" };
        return _;
    }


/*
        

        else if (opt.isOption("!wiki") || opt.setDescription("Help output in Wiki format"))
        {
            printHelpStyle = PrintHelpStyle::wiki;
        }
        else if (opt.isOption("!bash") || opt.setDescription("Help output in 'bash complete' format"))
        {
            printHelpStyle = PrintHelpStyle::bash_complete;
        }
        else if (opt.isOption("!clink") || opt.setDescription("Help output in 'clink complete' format"))
*/


    std::string getHelpOptionsString() const
    {
        return
            "-h\n"
            "-?\n"
            "--help\n"
            "    Show this help.\n"
            //"    See also RTC RFC010 for details (http://wiki.dep111.rtc.local/rfcrtc:rfcrtc010).\n";
            ;
    }

    bool isOption( std::string optLongName ) const
    {
        if (pCollector)
        {
            pCollector->addOption( optLongName );
        }

        if (!fOption)
            return false;
        if (fShort)
            return false;

        //if (optLongName)
        ltrim( optLongName, ",+-?!" );
        rtrim( optLongName, ",+-" );

        return name==optLongName;
    }

    bool isOption( char optShort ) const
    {
        if (pCollector)
        {
            pCollector->addOption( optShort );
        }

        if (!fOption)
            return false;
        if (!fShort)
            return false;

        return name==std::string( 1, optShort );
    }

    bool infoIgnore( )
    {
        if (pCollector)
        {
            pCollector->ignoreOptInfo( );
        }
        return false;
    }

    bool setDescription( const std::string &d )
    {
        if (pCollector)
        {
            pCollector->addOptionDescription( d );
        }
        return false;
    }


    // setParam должен вызываться самым первым в if

    bool setInitial( bool b )
    {
        if (pCollector)
        {
            pCollector->setOptionInitial( b );
        }
        return false;
    }

    bool setInitial( int i )
    {
        if (pCollector)
        {
            pCollector->setOptionInitial( i );
        }
        return false;
    }

    bool setInitial( size_t i )
    {
        if (pCollector)
        {
            pCollector->setOptionInitial( (int)i );
        }
        return false;
    }

    bool setInitial( const std::string &s )
    {
        if (pCollector)
        {
            pCollector->setOptionInitial( s );
        }
        return false;
    }

    bool setInitial( const char *s )
    {
        if (pCollector)
        {
            if (s)
                pCollector->setOptionInitial( std::string(s) );
            else
                pCollector->setOptionInitial( std::string() );
        }
        return false;
    }

    bool setParam( const std::string &p )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p );
        }
        return false;
    }

    // Задаем тип опции без дефолтного параметра
    bool setParam( const std::string &p, OptionType optType )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, optType );
        }
        return false;
    }

    bool setParam( bool defVal )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( defVal );
        }
        return false;
    }
    

    // Опциональный параметр задается символом '?' перед именем параметра
    // Для обязательного параметра defVal не используется, но он используется для задания типа опции

    bool setParam( const std::string &p, bool defVal )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, defVal );
        }
        return false;
    }

    //---

    bool setParam( const std::string &p, int defVal )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, defVal );
        }
        return false;
    }

    bool setParam( const std::string &p, int defVal, int minVal, int maxVal )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, defVal, minVal, maxVal );
        }
        return false;
    }

    bool setParam( const std::string &p, size_t defVal )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, (int)defVal );
        }
        return false;
    }

    bool setParam( const std::string &p, size_t defVal, size_t minVal, size_t maxVal )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, (int)defVal, (int)minVal, (int)maxVal );
        }
        return false;
    }

    //---

    bool setParam( const std::string &p, std::string defVal )
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, defVal );
        }
        return false;
    }

    bool setParam( const std::string &p, const char* defVal )
    {
        if (pCollector)
        {
            if (defVal)
               pCollector->setOptionParam( p, std::string(defVal) );
            else
               pCollector->setOptionParam( p, OptionType::optString );
        }
        return false;
    }

    //---

    bool setParam( const std::string &p, int defVal, std::string enumValues ) // "one|two|three"
    {
        if (pCollector)
        {
            pCollector->setOptionParam( p, defVal, enumValues );
        }
        return false;
    }

    template< typename T>
    bool getParamValueImplHelper( T &val, std::string &errMsg, T defVal, OptionType optType )
    {
        const auto &optInfo = pCollector->getCurrentOptionInfo();

        if (optInfo.optType==OptionType::optUnknown)
        {
            errMsg = std::string("Option not configured to use getParamValue method (") + optInfo.getAllOptionNames("/") + std::string(")");
            return false;
        }

        if (optInfo.optType!=optType)
        {
            errMsg = std::string("Option configured to use getParamValue method with different type (") + optInfo.getAllOptionNames("/") + std::string(")");
            return false;
        }

        if (optArg.empty() && optInfo.optType==OptionType::optFlag)
        {
            if (optInfo.hasDefVal)
               val = defVal;
            else
               val = true;

            return true;
        }


        if (optArg.empty())
        {
            if (!optInfo.paramOptional)
            {
                errMsg = std::string("Missing mandatory value (") + optInfo.getAllOptionNames("/") + std::string(")");
                return false;
            }

            if (!optInfo.hasDefVal)
            {
                errMsg = std::string("Missing mandatory value, and there is no default value taken (") + optInfo.getAllOptionNames("/") + std::string(")");
                return false;
            }

            val = defVal;

            return true;
        }

        return false;
    
    }

    bool getParamValue( bool &val, std::string &errMsg )
    {
        if (!pCollector)
        {
            errMsg = std::string("pCollector==0 (") + name + std::string(")");
            return false;
        }

        const auto &optInfo = pCollector->getCurrentOptionInfo();

        if (getParamValueImplHelper( val, errMsg, optInfo.boolDefVal, OptionType::optFlag ))
            return true; // readed ok

        if (!errMsg.empty())
            return false; // get value failed

        // need parsing
        std::string optArgLower = toLower(optArg);

        if (optArgLower=="-" || optArgLower=="0" || optArgLower=="false" || optArgLower=="no")
        {
            val = false;
            return true;
        }
        else if (optArgLower=="+" || optArgLower=="1" || optArgLower=="true" || optArgLower=="yes")
        {
            val = true;
            return true;
        }

        errMsg = std::string("Invalid option value taken (") + optInfo.getAllOptionNames("/") + std::string(")");
        return false;
        
    }

    bool getParamValue( int &val, std::string &errMsg )
    {
        if (!pCollector)
        {
            errMsg = std::string("pCollector==0 (") + name + std::string(")");
            return false;
        }

        const auto &optInfo = pCollector->getCurrentOptionInfo();

        if (getParamValueImplHelper( val, errMsg, optInfo.intDefVal, OptionType::optInt ))
            return true; // readed ok

        if (!errMsg.empty())
            return false; // get value failed

        // need parsing

        bool neg = false;
        std::string optArgCopy = optArg;
        if (!optArgCopy.empty() && (optArgCopy[0]=='+' || optArgCopy[0]=='-'))
        {
            if (optArgCopy[0]=='-')
                neg = true;
            optArgCopy.erase( 0, 1 );
        }

        uint64_t v = 0;
        if (!parseInteger(optArgCopy, v))
        {
            errMsg = std::string("Invalid option value taken (") + optInfo.getAllOptionNames("/") + std::string(")");
            return false;
        }

        val = (int)v;
        if (neg)
            val = -val;

        if (!optInfo.hasMinMax)
           return true;

        if ( (val < optInfo.minVal) || (val > optInfo.maxVal) )
        {
            errMsg = std::string("Option value is out of range (") + optInfo.getAllOptionNames("/") + std::string(")");
            return false;
        }

        return true;
    }

    bool getParamValue( size_t &val, std::string &errMsg )
    {
        int i;
        if (!getParamValue( i, errMsg ))
            return false;

        const auto &optInfo = pCollector->getCurrentOptionInfo();

        if (i<0)
        {
            errMsg = std::string("Option value is invalid - negative value taken for unsigned option (") + optInfo.getAllOptionNames("/") + std::string(")");
            return false;
        }

        val = (size_t)i;

        return true;
    }


    bool getParamValue( std::string &val, std::string &errMsg )
    {
        if (!pCollector)
        {
            errMsg = std::string("pCollector==0 (") + name + std::string(")");
            return false;
        }

        const auto &optInfo = pCollector->getCurrentOptionInfo();

        if (getParamValueImplHelper( val, errMsg, optInfo.stringDefVal, OptionType::optString ))
            return true; // readed ok

        if (!errMsg.empty())
            return false; // get value failed

        // need parsing

        val.clear();

        return true;
    }

    template<typename EnumType, typename EnumMapper >
    bool getParamValue( EnumType &val, std::string &errMsg, const EnumMapper &mapper )
    {
        if (!pCollector)
        {
            errMsg = std::string("pCollector==0 (") + name + std::string(")");
            return false;
        }

        const auto &optInfo = pCollector->getCurrentOptionInfo();

        int intVal;
        if (getParamValueImplHelper( intVal, errMsg, optInfo.intDefVal, OptionType::optEnum ))
        {
            val = mapper(intVal);
            return true; // readed ok
        }

        if (!errMsg.empty())
            return false; // get value failed

        // need parsing

        //std::string optArgCopy = toLower(optArg);

        /*
        !!!
        std::map<std::string, int>::const_iterator eit = optInfo.enumVals.find(optArgCopy);
        if (eit == optInfo.enumVals.end())
        {
            errMsg = std::string("Invalid option value taken. Option value can be one of: ") + optInfo.getAllEnumNames(", ", " or ") + std::string(" (") + optInfo.getAllOptionNames("/") + std::string(")");
            return false;
        }
        */

        int enumIntVal = optInfo.getEnumValueByName( optArg );
        if (enumIntVal<0)
        {
            errMsg = std::string("Invalid option value taken. Option value can be one of: ") + optInfo.getAllEnumNames(", ", " or ", "/") + std::string(" (") + optInfo.getAllOptionNames("/") + std::string(")");
            return false;
        }

        val = mapper(enumIntVal);

        return true;

    }

    std::string getOptionNames( std::string sep = "/" ) const
    {
        const auto &optInfo = pCollector->getCurrentOptionInfo();
        return optInfo.getAllOptionNames(sep);
    }


/*
    std::string getAllOptionNames( const std::string &sep ) const
    std::string getAllEnumNames( const std::string &sep, const std::string &sepLast ) const

*/
    //virtual const CommandLineOptionInfo& getCurrentOptionInfo() = 0;

}; // struct CommandLineOption

/*
enum class OptionType
{
    optUnknown     ,

    optFlag        ,
    optInt         ,
    optString      ,
    optEnum        ,

    optIntList     ,
    optStringList  ,
    optEnumList
};



struct CommandLineOptionInfo
{
    std::vector< std::string > optNames;
    bool                       internalOption = false;
    std::string                param;
    bool                       paramOptional  = false;
    std::string                description;

    OptionType                 optType;

    bool                       hasDefVal;
    bool                       hasMinMax;

    bool                       boolDefVal;
    int                        intDefVal;    // also enum defVal
    std::string                stringDefVal;

    int                        minVal;
    int                        maxVal;

    std::map<std::string, int> enumVals;

*/




// В наборе для проверки на уникальность храним все ключики - мапа ключ->опция
// Опция хранится как объект

class CommandLineOptionCollectorImplBase : public ICommandLineOptionCollector
{
protected:

    bool collectOptionsInfo = false;
    bool flagFailed = false;
    bool flagIgnore = false;
    CommandLineOptionInfo                 curOptInfo;
    std::vector<CommandLineOptionInfo>    optionsList;

    std::set<std::string>                 usedOpts;

    std::set<std::string>                 takenOpts;

    PrintHelpStyle                        m_printHelpStyle = PrintHelpStyle::normal;

    virtual void onOptionDup( const std::string &opt ) = 0;

public:

    virtual bool isNormalPrintHelpStyle() override
    {
        return m_printHelpStyle == PrintHelpStyle::normal;
    }
    //void setPrintHelpStyle( PrintHelpStyle printHelpStyle ) = 0;
    virtual void setPrintHelpStyle( PrintHelpStyle phs ) override
    {
        m_printHelpStyle = phs;
    }

    virtual PrintHelpStyle getPrintHelpStyle() override
    {
        return m_printHelpStyle;
    }

    bool isFailed()
    {
        return flagFailed;
    }

    virtual const std::set<std::string> * getTakenOpts() override
    {
        return &takenOpts;
    }

    virtual void  addTakenOpt( const std::string &optName ) override
    {
        takenOpts.insert( optName );
    }


    virtual void setCollectMode( bool bCollect ) override
    {
        collectOptionsInfo = bCollect;
    }

    virtual void ignoreOptInfo( ) override
    {
        flagIgnore = true;
    }

    /*
    virtual const CommandLineOptionInfo& getLastAddedOptionInfo() override
    {
        return optionsList.back();
    }
    */

    virtual const CommandLineOptionInfo& getCurrentOptionInfo() override
    {
        return curOptInfo;
    }

    virtual void addOption( std::string optLongName ) override
    {
        //bool internalOption = false;
        if (!optLongName.empty() && optLongName[0]=='!')
        {
            optLongName.erase(0,1);
            curOptInfo.internalOption = true;
        }
        //internalOption
        curOptInfo.optNames.push_back(optLongName);

        if (!flagIgnore && collectOptionsInfo)
        {
            if (usedOpts.find(optLongName)!=usedOpts.end())
            {
                flagFailed = true;
                onOptionDup( optLongName );
            }
            usedOpts.insert(optLongName);
        }
    }

    virtual void addOption( char optShort ) override
    {
        addOption( std::string( 1, optShort ) );
    }

    virtual void setOptionInitial( bool v ) override
    {
        curOptInfo.hasInitVal  = true;
        curOptInfo.boolInitVal = v;
    }

    virtual void setOptionInitial( int  v  ) override
    {
        curOptInfo.hasInitVal = true;
        curOptInfo.intInitVal = v;
    }

    virtual void setOptionInitial( const std::string &v ) override
    {
        curOptInfo.hasInitVal     = true;
        curOptInfo.stringInitVal  = v;
    }


    virtual void setOptionParam( const std::string &param ) override
    {
        curOptInfo.param = param;
        if (!curOptInfo.param.empty() && curOptInfo.param[0]=='?')
        {
            curOptInfo.param.erase(0, 1);
            curOptInfo.paramOptional = true;
        }
    }

    virtual void setOptionParam( const std::string &param, OptionType optType ) override
    {
        setOptionParam(param);
        curOptInfo.optType = optType;
    }

    virtual void setOptionParam( bool defVal ) override
    {
        curOptInfo.optType = OptionType::optFlag;
        curOptInfo.hasDefVal = true;
        curOptInfo.boolDefVal = defVal;
    }

    virtual void setOptionParam( const std::string &param, bool defVal ) override
    {
        setOptionParam(param);
        curOptInfo.optType = OptionType::optFlag;

        curOptInfo.hasDefVal = true;
        curOptInfo.boolDefVal = defVal;
    }

    virtual void setOptionParam( const std::string &param, int defVal  ) override
    {
        setOptionParam(param);
        curOptInfo.optType = OptionType::optInt;

        curOptInfo.hasDefVal = true;
        curOptInfo.intDefVal = defVal;
    }

    virtual void setOptionParam( const std::string &param, int defVal, int minVal, int maxVal ) override
    {
        setOptionParam(param);
        curOptInfo.optType = OptionType::optInt;

        curOptInfo.hasDefVal = true;
        curOptInfo.intDefVal = defVal;

        curOptInfo.hasMinMax = true;
        curOptInfo.minVal = minVal;
        curOptInfo.maxVal = maxVal;
    }

    virtual void setOptionParam( const std::string &param, const std::string &defVal ) override
    {
        setOptionParam(param);
        curOptInfo.optType = OptionType::optString;

        curOptInfo.hasDefVal = true;
        curOptInfo.stringDefVal = defVal;
    }

    virtual void setOptionParam( const std::string &param, int defVal, std::string enumValues ) override
    {
        setOptionParam(param);
        curOptInfo.optType = OptionType::optEnum;

        curOptInfo.hasDefVal = true;
        curOptInfo.intDefVal = defVal;

        /*
        enumValues = toLower(enumValues);

        std::vector< std::string > vec;
        splitToVector( enumValues, vec, '|' );

        for( int i=0; i<(int)vec.size(); ++i)
        {
            curOptInfo.enumVals[ vec[i] ] = i;
        }

        //std::map<std::string, int> enumVals;
        */

        curOptInfo.parseEnumConfigString(enumValues);
    }

/*
enum class OptionType
{
    optUnknown     ,

    optFlag        ,
    optInt         ,
    optString      ,
    optEnum        ,

    optIntList     ,
    optStringList  ,
    optEnumList
};


struct CommandLineOptionInfo
{
    std::vector< std::string > optNames;
    bool                       internalOption = false;
    std::string                param;
    bool                       paramOptional  = false;
    std::string                description;

    OptionType                 optType;

    bool                       hasDefVal;
    bool                       hasMinMax;

    bool                       boolDefVal;
    int                        intDefVal;    // also enum defVal
    std::string                stringDefVal;

    int                        minVal;
    int                        maxVal;

    std::map<std::string, int> enumVals;

    void reset()
    {
        optNames.clear();
        internalOption = false;
        param.clear();
        paramOptional  = false;
        description.clear();

        optType = OptionType::optUnknown;

        hasDefVal = false;
        hasMinMax = false;
    }
};

*/

    virtual void addOptionDescription( const std::string &d ) override
    {
        curOptInfo.description = d;
        if (!curOptInfo.optNames.empty() && !flagIgnore && collectOptionsInfo)
        {
            std::sort( curOptInfo.optNames.begin(), curOptInfo.optNames.end(), StringsLessFirstBySize(true) ); // StringsLess
            optionsList.push_back( curOptInfo );
        }
        curOptInfo.reset();
        flagIgnore = false;
    }

    virtual void resetCurCollectedOption( ) override
    {
        curOptInfo.reset();
    }

    virtual std::string makeText( std::string::size_type width, const std::set<std::string> *pHelpOnlyThats ) override
    {
        return makeText( width, m_printHelpStyle, pHelpOnlyThats );
    }

    virtual std::string makeText( std::string::size_type width, std::string *pPrefix, std::string *pSuffix, bool useExt ) override
    {
        return makeText( width, m_printHelpStyle, pPrefix, pSuffix, useExt );
    }

    virtual std::string makeText( std::string::size_type width, std::string *pPrefix, std::string *pSuffix, bool useExt, const std::set<std::string> *pHelpOnlyThats ) override
    {
        return makeText( width, m_printHelpStyle, pPrefix, pSuffix, useExt, pHelpOnlyThats );
    }

    virtual std::string makeText( std::string::size_type width, PrintHelpStyle style, std::string *pPrefix, std::string *pSuffix, bool useExt, const std::set<std::string> *pHelpOnlyThats ) override
    {
        std::stringstream oss;

        std::string exeFullName = getProgExeName();
        std::string exeVarName  ;

        if (useExt)
        {
           exeFullName = appendExtention(getFileName(exeFullName), getFileExtention(exeFullName) );
        }
        else
        {
           exeFullName = getFileName(exeFullName);
        }

        exeVarName = //std::string("_") + 
                     makeCppName( exeFullName );

        std::string prefix;
        std::string suffix;

        if (style==PrintHelpStyle::bash_complete)
        {
            // complete -W "-a -b c" x
            prefix = "complete -W \"";
            suffix = std::string("\" ") + exeFullName;
            oss<<prefix;
        }
        else if (style==PrintHelpStyle::clink_complete)
        {
            prefix = std::string("local ") + exeVarName + std::string("_tree = { ");
            suffix = std::string(" } clink.arg.register_parser(\"") + exeFullName + std::string("\", ") + exeVarName + std::string("_tree)");
            oss<<prefix;
        }

        if (pPrefix)
           *pPrefix = prefix;

        if (pSuffix)
           *pSuffix = suffix;


        size_t totalNamesPrinted = 0;
        for( const auto &optInfo : optionsList )
        {
            if (optInfo.internalOption)
                continue;

            bool skipThisOption = false;

            if (pHelpOnlyThats && !pHelpOnlyThats->empty())
            {
                bool optionFound = false;
                size_t i = 0, sz = optInfo.optNames.size();
                //for( const auto &optName : optInfo.optNames )
                for(; i!=sz; ++i)
                {
                    const auto &optName = optInfo.optNames[i];
                    if (pHelpOnlyThats->find(optName)!=pHelpOnlyThats->end())
                    {
                        optionFound = true;
                        break;
                    }
                }

                if (!optionFound)
                    skipThisOption = true;
            }

            if (skipThisOption)
                continue;

            if (style==PrintHelpStyle::bash_complete)
            {
                for( const auto &optName : optInfo.optNames )
                {
                    if (optName.size()<=1)
                        oss<<" -";
                    else
                        oss<<" --";
                    oss<<optName;

                    ++totalNamesPrinted;
                }

                continue;
            }

            if (style==PrintHelpStyle::clink_complete)
            {
                for( const auto &optName : optInfo.optNames )
                {
                    if (totalNamesPrinted)
                        oss<<", ";
                    oss<<"\"";
                    if (optName.size()<=1)
                        oss<<"-";
                    else
                        oss<<"--";
                    oss<<optName;
                    oss<<"\"";

                    ++totalNamesPrinted;
                }

                continue;
            }


            //oss<<"\n";
            //bool bFirst = true;
            size_t i = 0, sz = optInfo.optNames.size();
            //for( const auto &optName : optInfo.optNames )
            for(; i!=sz; ++i)
            {
                const auto &optName = optInfo.optNames[i];

                if (i==0)
                {
                    oss<<"\n";
                }
                else
                {
                    if (style==PrintHelpStyle::wiki)
                        oss<<",\n";
                    else
                        oss<<"\n";
                }

                if (style==PrintHelpStyle::wiki)
                    oss<<"**%%";

                if (optName.size()<=1)
                    oss<<"-";
                else
                    oss<<"--";
                oss<<optName;
                if (!optInfo.param.empty())
                {
                    if (optInfo.paramOptional)
                        oss<<"[="<<optInfo.param<<"]";
                    else
                        oss<<"="<<optInfo.param;
                }

                if (style==PrintHelpStyle::wiki)
                    oss<<"%%**";

                //oss<<"\n";
            }

            std::stringstream descExtraOss;

            if (optInfo.optType==OptionType::optFlag)
            {
                descExtraOss<<"Flag option";
            }

            if (optInfo.hasMinMax)
            {
                descExtraOss<<"Min value: "<<optInfo.minVal<<", "
                            <<"max value: "<<optInfo.maxVal;
            }

            if (optInfo.optType==OptionType::optEnum)
            {
                std::string enumNames = optInfo.getAllEnumNames( ", ", " or ", " / ", "'", "'" );
                if (!enumNames.empty())
                   descExtraOss<<"Allowed values: "<<enumNames;
                   //descExtraOss<<"Enumeration: "<<enumNames;
            }

/*
        if (!hasDefVal)
            return std::string();

        if (!paramOptional)
            return std::string();
*/

            std::string initVal = optInfo.getInitialValString();
            if (!initVal.empty())
            {
                if (descExtraOss.str().empty())
                    descExtraOss<<"Initial value: ";
                else
                    descExtraOss<<", initial value: ";

                descExtraOss<<initVal;
            }

            if (optInfo.hasDefVal && optInfo.paramOptional)
            {
                std::string defVal = optInfo.getDefValString();
                if (!defVal.empty())
                {
                    if (descExtraOss.str().empty())
                        descExtraOss<<"Default value: ";
                    else
                        descExtraOss<<", default value: ";

                    descExtraOss<<defVal;
                }
            }

            std::string descr = textAppendDot(optInfo.description);
            //if (!descr.empty() && descr.back()!='.')
            //    descr.append(1, '.');

            std::string descExtra = descExtraOss.str();
            if (!descExtra.empty())
                descExtra = std::string("\n") + descExtra;

            descr.append(descExtra);
            descr = textAppendDot(descr);

            if (style==PrintHelpStyle::wiki)
            {
                std::string tmp = descr;
                trim(tmp);
                if (!tmp.empty() && tmp.back()!='.')
                    tmp.append(1, '.');
                oss<<" - " <<tmp;
            }
            else
            {
                oss<<"\n";
                oss<<textAddIndent( formatTextParas( descr, width ), "    " );
            }

            oss<<"\n";

        }


/*
enum class OptionType
{
    optUnknown     ,

    optFlag        ,
    optInt         ,
    optString      ,
    optEnum        ,

    optIntList     ,
    optStringList  ,
    optEnumList
};


struct CommandLineOptionInfo
{
    std::vector< std::string > optNames;
    bool                       internalOption = false;
    std::string                param;
    bool                       paramOptional  = false;
    std::string                description;

    OptionType                 optType;

    bool                       hasDefVal;
    bool                       hasMinMax;

    bool                       boolDefVal;
    int                        intDefVal;    // also enum defVal
    std::string                stringDefVal;

    int                        minVal;
    int                        maxVal;

    std::map<std::string, int> enumVals;
*/



        if (style==PrintHelpStyle::bash_complete || style==PrintHelpStyle::clink_complete)
        {
            oss<<suffix;
        }

        return oss.str();
    }

    virtual std::string makeText( std::string::size_type width, PrintHelpStyle printHelpStyle, const std::set<std::string> *pHelpOnlyThats ) override
    {
        return makeText( width, printHelpStyle, 0, 0, false, pHelpOnlyThats );
    }

    virtual std::string makeText( std::string::size_type width, PrintHelpStyle printHelpStyle, std::string *pPrefix, std::string *pSuffix, bool useExt ) override
    {
        return makeText( width, printHelpStyle, pPrefix, pSuffix, useExt, 0 );
    }

    //virtual std::string makeText( std::string::size_type width, PrintHelpStyle style, std::string *pPrefix, std::string *pSuffix, bool useExt, const std::set<std::string> *pHelpOnlyThats ) override



}; // class CommandLineOptionCollectorImplBase



inline
bool checkRcScriptFileLineForUpdate( std::string line, const std::string & lineTextToCompare, const std::string & prefix, const std::string & suffix )
{
    trim(line);

    if (prefix.empty() && suffix.empty())
    {
        return line == lineTextToCompare;
    }
    else
    {
        return startsWith( line, prefix ) && endsWith( line, suffix );
    }
}

inline
void updateRcScriptFileText( std::string &fileText, const std::string & updateLineText, const std::string & prefix, const std::string & suffix, bool simpleRemoveLine )
{
    std::string linefeed;
    std::vector< std::string > lines;
    splitTextToLines( fileText, lines, linefeed );

    std::vector< std::string > resLines;

    //size_t i = 0, sz = lines.size();

    bool actionPerformed = false;

    std::vector< std::string >::const_iterator lit = lines.begin();
    for(; lit != lines.end(); ++lit )
    {
        if (actionPerformed)
        {
            resLines.push_back(*lit);
            continue;
        }

        if ( !checkRcScriptFileLineForUpdate( *lit, updateLineText, prefix, suffix ) )
        {
            resLines.push_back(*lit);
            continue;
        }

        // found the line
        if (actionPerformed)
        {
           // action already performed? Hm-m-m...
           // Do nothing
            resLines.push_back(*lit);
            continue;
        }

        actionPerformed = true;

        if (simpleRemoveLine)
        {
            continue; // simply skip it
        }

        resLines.push_back(updateLineText);
    }


    if (!actionPerformed) // simply add line to the end of the file
        resLines.push_back(updateLineText);

    fileText.clear();

    for( const auto &l : resLines )
    {
        fileText.append(l);
        fileText.append(linefeed);
    }

}

inline
bool updateRcScriptFile( const std::string &fileName, const std::string & updateLineText, const std::string & prefix, const std::string & suffix, bool simpleRemoveLine )
{
    std::string fileText;

    readBinaryFile( fileName, fileText );

    // Пока ошибки нет
    //if (!readBinaryFile( fileName, fileText ))
    //    return false;

    updateRcScriptFileText( fileText, updateLineText, prefix, suffix, simpleRemoveLine );
    std::ofstream output( fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

    // А вот теперь - точно ошибка
    if (!output)
    {
        return false;
    }

    output.write( fileText.data(), fileText.size() );

    return true;
}


// .bashrc
//.bash_profile
// if [ -f ~/.bashrc ]; then . ~/.bashrc; fi

inline
bool updateAutocompletionBashRc( ICommandLineOptionCollector *pCol, bool simpleRemoveLine )
{
    std::string pref, suf, text = pCol->makeText( 0, PrintHelpStyle::bash_complete, &pref, &suf, false );
    if (!updateRcScriptFile( appendPath( getUserFolderPath(), ".bashrc" ), text, pref, suf, simpleRemoveLine ))
        return false;
    #if defined(WIN32) || defined(_WIN32)
        text = pCol->makeText( 0, PrintHelpStyle::bash_complete, &pref, &suf, true );
        if (!updateRcScriptFile( appendPath( getUserFolderPath(), ".bashrc" ), text, pref, suf, simpleRemoveLine ))
            return false;
    #endif

    return true;
}

inline
bool updateAutocompletionBashProfile( ICommandLineOptionCollector *pCol, bool simpleRemoveLine )
{
    //std::string pref, suf, text = pCol->makeText( 0, PrintHelpStyle::bash_complete, &pref, &suf );
    return updateRcScriptFile( appendPath( getUserFolderPath(), ".bash_profile" ), "if [ -f ~/.bashrc ]; then . ~/.bashrc; fi", std::string(), std::string(), simpleRemoveLine );
}

inline
bool updateAutocompletionBashScripts( ICommandLineOptionCollector *pCol, bool simpleRemoveLine )
{
    return updateAutocompletionBashRc( pCol, simpleRemoveLine ) && updateAutocompletionBashProfile( pCol, simpleRemoveLine );
}

// D:\clink\clink.lua
inline
bool updateAutocompletionCLinkScripts( ICommandLineOptionCollector *pCol, bool simpleRemoveLine, std::string clinkPathFile )
{
    std::string pathName = "d:/clink/";
    std::string fileName = "clink.lua";

    if (clinkPathFile.size()==1)
    {
        if ( (clinkPathFile[0]>='a' && clinkPathFile[0]<='z')
          || (clinkPathFile[0]>='A' && clinkPathFile[0]<='Z')
           )
        {
            pathName[0] = clinkPathFile[0];
        }
        else
        {
            return false;
        }
    }
    else if (!clinkPathFile.empty())
    {
        pathName = clinkPathFile;
    }

    stripPathSep(pathName);

    // https://docs.microsoft.com/ru-ru/cpp/c-runtime-library/reference/stat-functions?view=vs-2019
    
    bool isFile = true;

    #if defined(WIN32) || defined(_WIN32)
        struct _stat s;
        if( _stat( pathName.c_str(), &s ) == 0 )
        {
            if ( s.st_mode & _S_IFDIR )
                isFile = false;
            else if (s.st_mode & _S_IFREG)
                isFile = true;
            else
                return false;
        }
    #else
        struct stat s;
        if( stat( pathName.c_str(), &s ) == 0 )
        {
            if ( s.st_mode & S_IFDIR )
                isFile = false;
            else if (s.st_mode & S_IFREG)
                isFile = true;
            else
                return false;
        }
    #endif
        else
        {
            // stat failed
            return false;
        }

    if (isFile)
    {
        clinkPathFile = pathName;
    }
    else
    {
        clinkPathFile = appendPath( pathName, fileName );
    }

    std::string pref, suf, text = pCol->makeText( 0, PrintHelpStyle::clink_complete, &pref, &suf, false );
    if (!updateRcScriptFile( clinkPathFile, text, pref, suf, simpleRemoveLine ))
        return false;

    #if defined(WIN32) || defined(_WIN32)
        text = pCol->makeText( 0, PrintHelpStyle::clink_complete, &pref, &suf, true );
        if (!updateRcScriptFile( clinkPathFile, text, pref, suf, simpleRemoveLine ))
            return false;
    #endif

    return true;


}

template< typename StreamGetter >
inline
int autocompletionInstaller ( ICommandLineOptionCollector *pCol, CommandLineOption &opt, PrintHelpStyle printHelpStyle, bool installMode, const StreamGetter &streamGetter )
{
    bool useBash  = false;
    bool useCLink = false;

    if (printHelpStyle == PrintHelpStyle::bash_complete)
        useBash = true;
    else if (printHelpStyle == PrintHelpStyle::clink_complete)
        useCLink = true;
    else if (printHelpStyle == PrintHelpStyle::normal)
    {
        useBash = true;
        useCLink = true;
    }
    else
    {
        //LOG_ERR_OPT
        streamGetter(true)<<"Invalid subkey; --wiki used? ("<<opt.getOptionNames()<<")\n";
        return -1;
    }

    #if defined(WIN32) || defined(_WIN32)
    #else
        useCLink = false; // --clink simple ignored 
    #endif

    if (useBash)
    {
        if (!updateAutocompletionBashScripts(pCol, !installMode))
        {
            //LOG_ERR_OPT
            streamGetter(true)<<"Failed to update bash config ("<<opt.getOptionNames()<<")\n";
            return -1;
        }
    }

    if (useCLink)
    {
        if (!updateAutocompletionCLinkScripts(pCol, !installMode, opt.optArg))
        {
            //LOG_ERR_OPT
            streamGetter(true)<<"Failed to update clink config ("<<opt.getOptionNames()<<")\n";
            return -1;
        }
    }

    streamGetter(false)<<"Autocompletion successfully installed\n";

    return 1;
}


inline
bool argsHasHelpOption( const std::vector<std::string> &args )
{
    for( const auto &a : args)
    {
        CommandLineOption opt(a);
     
        if (opt.isPureHelpOption())
            return true;
    }

    return false;
}


inline
bool argsFilterForHelpOption( std::vector<std::string> &args, std::set<std::string> &argsNeedHelp, const std::set<std::string> &argsAllowedForHelp )
{
    std::vector<std::string> argsTmp;
    argsTmp.reserve(args.size());

    for( const auto &a : args )
    {
        CommandLineOption opt(a);

        auto b = a;

        ltrim(b, "-");

        bool allowedForHelp = argsAllowedForHelp.find(b)!=argsAllowedForHelp.end();
     
        //if (!allowedForHelp) // allowed to use with --help in special way
            argsTmp.push_back(a);

        if (opt.isPureHelpOption())
        {
            //args = argsTmp;
            swap(args, argsTmp);
            return true;
        }

        //ltrim(a, "-");
        if (!allowedForHelp)
            argsNeedHelp.insert(b); // print help only for that args
    }

    argsNeedHelp.clear();

    return false;
}

inline
bool argsFilterForHelpOption( std::vector<std::string> &args, std::set<std::string> &argsNeedHelp )
{
    return argsFilterForHelpOption( args, argsNeedHelp, CommandLineOption::getHelpStyleOptions() );
}

