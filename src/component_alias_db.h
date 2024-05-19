#pragma once


#include "rdlc-core/isa.h"
#include "rdlc-core/case.h"
#include "rdlc-core/splits.h"
#include "rdlc-core/trims.h"
#include "rdlc-core/textUtils.h"

//
#include "net.h"

#include <unordered_map>
#include <iostream>
#include <string>



class ComponentAliasDb
{
    std::unordered_map<std::string,std::string> m_aliasMap;


    void addAlias(const std::string correctName, const std::string wrongAlias)
    {
        m_aliasMap[toUpper(wrongAlias)] = toUpper(correctName);
    }

    void parseDatabaseLine(std::string line, RoboconfOptions &rbcOpts, FileSet::file_id_t fileNo, size_t lineNo )
    {
        trim(line);

        std::string first, second;
        if (splitToPair(line, first, second, "->"))
        {
            // BAD_COMPONENT_NAME -> CORRECT_COMPONENT_NAME
            // first : bad
            // second: correct

            trim(first);
            trim(second);

            if (first.empty())
            {
                LOG_WARN("cmp-alias-db") << "empty incorrect component name (empty alias) at the left of '->'\n";
                return;
            }

            if (second.empty())
            {
                LOG_WARN("cmp-alias-db") << "empty correct component name at the right of '->'\n";
                return;
            }

            addAlias(second, first);
        }
        else if (splitToPair(line, first, second, "<-"))
        {
            // CORRECT_COMPONENT_NAME <- BAD_COMPONENT_NAME1,BAD_COMPONENT_NAME2,...

            // first : correct
            // second: bad list

            trim(first);
            trim(second);

            if (first.empty())
            {
                LOG_WARN("cmp-alias-db") << "empty correct component name at the left of '<-'\n";
                return;
            }

            if (second.empty())
            {
                LOG_WARN("cmp-alias-db") << "empty incorrect components name list at the right of '<-'\n";
                return;
            }

            std:: vector<std::string> bads; bads.reserve(ROBOCONF_COMMON_VECTOR_RESERVE_SIZE);
            splitToVector(second, bads, ',');
            for(auto b : bads)
            {
                trim(b);
                if (b.empty())
                {
                    // cmp-alias-db-alempty
                    LOG_WARN("cmp-alias-db") << "empty incorrect component name (empty alias) at the right of '<-'\n";
                    continue;
                }

                addAlias(first, b);
            }
        }
        else
        {
            // cmp-alias-db-wrline
            LOG_WARN("cmp-alias-db") << "invalid database file line, missing separator\n";
        }

    }

// RoboconfOptions             rbcOpts( &logMsg, &logErr );
//     	FileSet::file_id_t fileNo = rbcOpts.usedFiles.getFileId(foundName);
// 		size_t lineNo = 0;
//  
//         file_id_t getFileId( std::string fileName )

// #define LOG_ERR                      startLogMessage( rbcOpts, LogEntryType::err , "err"    , fileNo, lineNo, __FILE__, __LINE__ )
// #define LOG_WARN(warnType)           startLogMessage( rbcOpts, LogEntryType::warn, warnType , fileNo, lineNo, __FILE__, __LINE__ )
//  
// #define LOG_ERR_OBJ(obj)             startLogMessage( rbcOpts, LogEntryType::err , "err"    , (obj).fileNo, (obj).lineNo, __FILE__, __LINE__ )
// #define LOG_WARN_OBJ(warnType, obj)  startLogMessage( rbcOpts, LogEntryType::warn, warnType , (obj).fileNo, (obj).lineNo, __FILE__, __LINE__ )
//  
// // options and other errors
// #define LOG_ERR_OPT                  startLogMessage( rbcOpts, LogEntryType::err , "err"    , (FileSet::file_id_t)-1, 0, __FILE__, __LINE__ )
// #define LOG_WARN_OPT(warnType)       startLogMessage( rbcOpts, LogEntryType::warn, warnType , (FileSet::file_id_t)-1, 0, __FILE__, __LINE__ )

// #define LOG_MSG_OPT                  startLogMessage( rbcOpts, LogEntryType::msg ,  /* gopts, */  "msg", 0, 0, 0, 0 )
// #define LOG_MSG(msgType)             startLogMessage( rbcOpts, LogEntryType::msg ,  /* gopts, */  msgType /* "msg" */ , 0, 0, 0, 0 )


public:

    ComponentAliasDb() = default;
    ComponentAliasDb(const ComponentAliasDb&) = default;
    ComponentAliasDb& operator=(const ComponentAliasDb&) = default;
    ComponentAliasDb(ComponentAliasDb&&) = default;
    ComponentAliasDb& operator=(ComponentAliasDb&&) = default;

    void parseDatabaseFile(std::ifstream &dbFile, RoboconfOptions &rbcOpts, FileSet::file_id_t fileNo)
    {
        size_t lineNo = 0;
        std::string line;
        while( std::getline( dbFile, line) )
        {
            lineNo++;

            std::string lineCopy = line;
            trim(lineCopy);
    
            if (lineCopy.empty())
                continue;
    
            if (isComment(lineCopy))
                continue;
    
            //resVec.push_back(optLine);
            parseDatabaseLine(line, rbcOpts, fileNo, lineNo);
        }
        
        //return true;
    
    }

    void parseDatabaseFile(RoboconfOptions &rbcOpts, const std::string &fileName)
    {
        FileSet::file_id_t fileNo = rbcOpts.usedFiles.getFileId(fileName);

        std::ifstream dbFile(fileName.c_str());
        if (!dbFile)
        {
            LOG_WARN_OPT("read-file-failed") << "Failed to read component alias database file: '" << fileName << "'\n";
        }
        else
        {
            parseDatabaseFile(dbFile, rbcOpts, fileNo);
        }
    }

    std::string findComponentCanonicalName(const std::string &name, bool *pFound = 0) const
    {
        std::string upperName = toUpper(name);
        std::unordered_map<std::string,std::string>::const_iterator it = m_aliasMap.find(upperName);
        bool bFound = false;
        std::string resName;
        if (it==m_aliasMap.end())
        {
            resName = name; // upperName;
        }
        else
        {
            resName = it->second;
            bFound = true;
        }

        if (pFound)
            *pFound = bFound;

        return resName;
    }

    bool findComponentCanonicalName(const std::string &name, std::string & foundName) const
    {
        bool res = false;
        foundName = findComponentCanonicalName(name, &res);
        return res;
    }

    void makeComponentTypesCanonical(RoboconfOptions &rbcOpts, NetlistInfo &nlInfo) const
    {
        typename NetlistInfo::components_map_type::iterator cit = nlInfo.components.begin();
        for(; cit!=nlInfo.components.end(); ++cit)
        {
            // if (cit->second.designator == "U1" || cit->second.designator == "U1A")
            // {
            //     std::cout << "MCU\n";
            // }
            std::string componentTypeName = cit->second.typeName;
            if (findComponentCanonicalName(componentTypeName, cit->second.typeName))
            {
                LOG_MSG("cmp-alias-db")<<"found canonical component name for '" << componentTypeName << "': '"<< cit->second.typeName <<"'\n";
                cit->second.typeNameOrg = componentTypeName;
            }
        }
    }

    void makeComponentTypesCanonical(RoboconfOptions &rbcOpts, all_nets_map_type &nets) const
    {
        typename all_nets_map_type::iterator nit = nets.begin();
        for(; nit!=nets.end(); ++nit)
        {
            makeComponentTypesCanonical(rbcOpts, nit->second);
        }
    }



};
