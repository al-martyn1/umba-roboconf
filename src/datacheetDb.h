#pragma once


#include "datacheetUtils.h"
#include "rdlc-core/textUtils.h"
#include "ioUtils.h"

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>



//----------------------------------------------------------------------------

/*

 - [ ] Даташиты в разных конторах могут лежать в разных местах, и не всегда в одном месте, а могут располагаться в развесистой иерархии каталогов
       Поэтому можно указывать имя даташита, и в отдельной базе искать альтернативные места расположения.
       Это простой текстовый файл, строчка без отступа - оригинальное имя даташита, следующие за ней строчки с отступом - алтернативные имена (с путями).
       Алгоритм поиска немного меняется. Если не нашли такой даташит/рефман/итп по путям, которые заданы в конфиге, ищем alter базе.
       Базу datasheet-mapping'а храним в простом текстовике, чтобы простым людям было просто что-то подправить.

       Нам нужно по имени файла хранить список его возможных расположений.
       При этом хранить нужно в том порядке, что указан, но без дубликатов.
       Можно хранить set и vector - но это доп расходы памяти.
       Можно хранить map std::string -> index, при помощи этого мапа можно узнать индекс, но как по индексу получить строку?
       Либо перебором мапы, либо заводим вторую мапу index -> строка.
       Тоже херня какая-то.
       Ладно, не будем парится, заведем set и vector

       Порядок важен, но сетевые ресурсы идут после локальных всё равно


       Иконки - https://www.flaticon.com/ru/free-icons/pdf


*/


//----------------------------------------------------------------------------
class DatasheetAltNames
{
    std::unordered_set<std::string>    alreadyAddedNames;
    std::vector<std::string>           namesLocal  ;
    std::vector<std::string>           namesNetwork;


public:

    // std::vector<std::string>::const_iterator begin() const { return names.begin();  }
    // std::vector<std::string>::const_iterator end  () const { return names.end  ();  }
    //  
    // std::vector<std::string>::const_iterator cbegin() const { return names.cbegin();  }
    // std::vector<std::string>::const_iterator cend  () const { return names.cend  ();  }
    //  
    // std::vector<std::string>::iterator begin() { return names.begin();  }
    // std::vector<std::string>::iterator end  () { return names.end  ();  }

    bool addName(const std::string &name)
    {
        if (alreadyAddedNames.find(name)!=alreadyAddedNames.end())
            return false;

        if (!isDatasheetNetworkLink(name))
            namesLocal.emplace_back(name);
        else
            namesNetwork.emplace_back(name);

        alreadyAddedNames.insert(name);

        return true;
    }

    std::vector<std::string> getNames() const
    {
        std::vector<std::string> res = namesLocal; // локальные ресурсы - в начале
        res.insert(res.end(), namesNetwork.begin(), namesNetwork.end());
        return res;
    }

    std::vector<std::string> getLocalNames() const
    {
        return namesLocal;
    }

    std::vector<std::string> getNetworkNames() const
    {
        return namesNetwork;
    }


}; // class DatasheetAltNames

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
class DatasheetsDb
{

    std::unordered_map<std::string, DatasheetAltNames>    allNames;

protected:

public:

    static
    std::string makeKeyFilename(const std::string &name, bool keepCase=false)
    {
        std::string filenameOnly = getNameFromFull(name);
        std::string ext = getFileExtention(filenameOnly);

        // Если нет расширения?
        if (ext.empty())
        {
            // Если сетевое, то оставляем полное имя в качестве ключа
            //if (!isDatasheetNetworkLink(name))
            // Если не сетевое, то тоже самое
            return name;
        }

        // Расширение есть, это либо локальный файл, либо в сети.
        // В этом случае игнорируем регистр

        //return toLower(filenameOnly);
        return keepCase ? filenameOnly : toUpper(filenameOnly);
    }


    bool addAlias(const std::string &dsFilename, const std::string &dsAlias)
    {
        allNames[makeKeyFilename(dsFilename)].addName(dsAlias);
        return true;
    }

    std::vector<std::string> getAliases(const std::string &dsFilename) const
    {
        std::unordered_map<std::string, DatasheetAltNames>::const_iterator it = allNames.find(makeKeyFilename(dsFilename));
        if (it==allNames.end())
            return std::vector<std::string>();
        return it->second.getNames();
    }
    
    std::vector<std::string> getLocalAliases(const std::string &dsFilename) const
    {
        std::unordered_map<std::string, DatasheetAltNames>::const_iterator it = allNames.find(makeKeyFilename(dsFilename));
        if (it==allNames.end())
            return std::vector<std::string>();
        return it->second.getLocalNames();
    }
    
    std::vector<std::string> getNetworkAliases(const std::string &dsFilename) const
    {
        std::unordered_map<std::string, DatasheetAltNames>::const_iterator it = allNames.find(makeKeyFilename(dsFilename));
        if (it==allNames.end())
            return std::vector<std::string>();
        return it->second.getNetworkNames();
    }


    bool readDatabaseFile(std::istream &in)
    {
        EncodingsApi* pEncApi = getEncodingsApi();
        UINT sysCp = pEncApi->getSystemCharMulticharCodePage();
        std::string sysCpName = pEncApi->getCodePageName(sysCp);

        std::string fileText = readFileEncoded(in, std::string() /* srcEnc */, sysCpName );

        std::vector<std::string> lines;
        std::string detectedLinefeed;
        textSplitToLines( fileText, lines, detectedLinefeed );


        std::string curDatasheet;

        for(std::string line : lines)
        {
            std::string ltrimmedLine = line;
            ltrim(ltrimmedLine);


            if (ltrimmedLine.empty() || isComment(ltrimmedLine))
                continue;

            bool isKey = line[0]!=' ';
            trim(line);

            if (isKey)
            {
                curDatasheet = line;
            }
            else
            {
                if (!curDatasheet.empty())
                {
                    addAlias(curDatasheet, line);
                }
            }
        }

        return true;

    }

    bool readDatabaseFile(const std::string &fileName)
    {
        auto in = std::ifstream(fileName.c_str());
        if (!in)
            return false;

        return readDatabaseFile(in);
    }
    

}; // class DatasheetsDb












