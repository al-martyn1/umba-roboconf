#pragma once

#include <map>
#include <string>
#include <exception>
#include <stdexcept>

#include "rdlc-core/case.h"

class FileSet
{

public:

    typedef size_t file_id_t;

private:

    file_id_t curId;
    std::map<std::string, file_id_t   >    filesIds;
    std::map<file_id_t   , std::string>    idFiles;
    //std::map<std::string, std::string>    filesOrgNames;

public:

    FileSet() : curId(0), filesIds(), idFiles() {}

    file_id_t getFileId( std::string fileName )
    {
        std::string orgName = fileName;
        #if defined(WIN32) || defined(_WIN32)
        fileName = toLower(fileName);
        #endif

        std::map<std::string, file_id_t>::const_iterator it = filesIds.find(fileName);
        if (it != filesIds.end())
            return it->second;

        file_id_t res      = curId++;
        filesIds[fileName] = res;
        idFiles[res]       = orgName;

        return res;
    }

    std::string getFileName(file_id_t id) const
    {
        std::map<file_id_t   , std::string>::const_iterator it = idFiles.find(id);
        if (it==idFiles.end())
            //return (file_id_t)-1;
            throw std::runtime_error("invalid file ID");
        return it->second;
    }

}; // class FileSet

