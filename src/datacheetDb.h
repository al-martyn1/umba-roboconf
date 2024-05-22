#pragma once


#include "datacheetUtils.h"

#include <unordered_set>
#include <vector>
#include <string>

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
       Либо перебором мапы, либо заводим вторую мапу ibdex -> строка.
       Тоже херня какая-то.
       Ладно, не будем парится, заведем set и vector

       Порядок важен, но сетевые ресурсы идут после локальных всё равно


       Иконки - https://www.flaticon.com/ru/free-icons/pdf


*/


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
    
    }

    std::vector<std::string> getNames() const
    {
    
    }

    //bool n1 = isDatasheetNetworkLink(name1);


}
