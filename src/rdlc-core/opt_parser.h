#pragma once

#include <map>
#include <string>
#include <exception>
#include <stdexcept>
#include "utils.h"
#include "case.h"



class OptionsParser
{
    const std::map< std::string, std::string> &m_map;

public:


    OptionsParser( const std::map< std::string, std::string> &m ) : m_map(m) {}


    bool checkAllowedOptions( const std::set<std::string> &allowedOpts ) const
    {
        bool bErr = false;
        auto mit = m_map.begin();
        for(; mit != m_map.end(); ++mit )
        {
            std::string opt = mit->first;
            endsWithAndStrip( opt, "-" );
            endsWithAndStrip( opt, "+" );
            if (allowedOpts.find(opt)==allowedOpts.end())
            {
                bErr = true;
                LOG_ERR_OPT<<"target-option: unknown option '"<<opt<<"'\n";
            }
        }

        return !bErr;
    }
    
    void splitValToVector( const std::string &s, std::vector<std::string> &v, std::string seps = std::string()) const
    {
        //if (seps.empty())
        //    seps = ",;";
        seps += ",;";

        char splitCh = contains( s, seps );
        if ( splitCh )
        {
            splitToVector( s, v, splitCh );
        }
        else
            v.push_back(s);
    
    }

    std::string getLastVal( const std::string &s ) const
    {
        std::vector< std::string > v;
        splitValToVector( s, v );
        if (v.empty())
            return s;
        return v.back();
    }

    std::string getOptionValue( const std::string &optName, const std::string& defVal )
    {
        std::map< std::string, std::string>::const_iterator mit = m_map.find(optName);
        if (mit == m_map.end())
            return defVal;

        return getLastVal(mit->second);
    }

    std::vector<std::string> getOptionValueVector( const std::string &optName, bool makeFlat, std::string seps = std::string() )
    {
        std::map< std::string, std::string>::const_iterator mit = m_map.find(optName);
        if (mit == m_map.end())
            return std::vector<std::string>();

        std::vector<std::string> v;
        splitValToVector( mit->second, v, seps );
        if (makeFlat)
        {
            std::vector<std::string> tmpV;
            for( auto s : v )
            {
                std::vector<std::string> tmp;
                splitValToVector( s, tmp, seps );
                tmpV.insert( tmpV.end(), tmp.begin(), tmp.end() );
            }
            std::swap( v, tmpV);
        }
        return v;
    }

    template<typename EnumType>
    EnumType getOptionValue( const std::string &optName, std::vector< std::string > vals, int firstVal, EnumType defVal )
    {
        std::string optStrValue = getOptionValue( optName, std::string() );
        if (optStrValue.empty())
            return defVal;

        std::string orgOptStrValue = optStrValue;
        toLower(optStrValue);

        int res = firstVal;
        vals = toLower(vals);
        for( const auto &v : vals )
        {
            if (v==optStrValue)
                return (EnumType)res;
            ++res;
        }

        LOG_ERR_OPT<<"target-option: invalid option '"<<optName<<"' value: '"<<orgOptStrValue<<"'\n";
        throw std::runtime_error("invalid option value");
        return defVal;
    }

    int getOptionValue( const std::string &optName, int defVal)
    {
        std::string optStrValue = getOptionValue( optName, std::string() );
        if (optStrValue.empty())
            return defVal;

        uint64_t res;
        if (!parseInteger( optStrValue, res, 10 ))
        {
            LOG_ERR_OPT<<"target-option: invalid option '"<<optName<<"' value: '"<<optStrValue<<"'\n";
            throw std::runtime_error("invalid option value");
        }

        return (int)res;
    }

    bool getOptionValue( const std::string &optName, bool defVal )
    {
        std::map< std::string, std::string>::const_iterator mit = m_map.find(optName+std::string("-"));
        if (mit != m_map.end())
            return false;

        mit = m_map.find(optName+std::string("+"));
        if (mit != m_map.end())
            return true;

        mit = m_map.find(optName);
        if (mit == m_map.end())
            return defVal;

        std::string optStrValue = mit->second;
        optStrValue = getLastVal(optStrValue);
        if (optStrValue.empty())
            return true;

        std::string orgOptStrValue = optStrValue;
        toLower(optStrValue);

        if (optStrValue=="-" || optStrValue=="0" || optStrValue=="false" || optStrValue=="no" || optStrValue=="off")
            return false;
        if (optStrValue=="+" || optStrValue=="1" || optStrValue=="true" || optStrValue=="yes" || optStrValue=="on")
            return true;

        LOG_ERR_OPT<<"target-option: invalid option '"<<optName<<"' value: '"<<orgOptStrValue<<"'\n";
        throw std::runtime_error("invalid option value");
        return defVal;
    }




}; // class OptionsParser

