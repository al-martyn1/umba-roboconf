#pragma once

/*

Весь вывод разделяем на типы.
Типы организуем в группы.

Нужен ли detail level? 
В принципе - наверно удобно - задаем уровень, и сразу имеем список групп.
Не нужно руцами перечислять группы.
Возможность ручного перечисления также остается.


Итого - можно задавать:
 1) отдельные типы выводимых сообщений - --details=
 2) группы выводимых сообщений         - --details=
 3) --detail-level 0-9
    --verbose-level ?

 --detail-level - очищает список разрешенных типов сообщений. Или нет?

 --detail-level - почти как группы, но при задании detail-level задаются также все
 более низкие уровни

*/

class DetailLevels
{

public:

    std::set<std::string>                             enabledDetailTypes;
    std::map<std::string, std::set<std::string> >     detailTypeGroups;
    //std::map<unsigned   , std::set<std::string> >     messageTypeLevels;
    std::map<std::string, std::string >               descriptions;


    // Static configuration

    void addToDetailGroup( const std::string &detailGroup, const std::string &detailTypeOrGroup )
    {
        detailTypeGroups[detailGroup].insert(detailTypeOrGroup);
    }

    void setDetailDescription( const std::string &detailOrGroup, const std::string &descr)
    {
        descriptions[detailOrGroup] = descr;
    }

    // Helpers

    bool isDigitGroup(const std::string &detailGroup) const
    {
        if (detailGroup.size()!=1)
            return false;
        if (detailGroup[0]<'0' || detailGroup[0]>'9')
            return false;
        return true;
    }

    std::string getLowerDigitGroup(const std::string &detailGroup) const
    {
        if (!isDigitGroup(detailGroup))
            return std::string();

        int num = (int)(detailGroup[0] - '0');
        --num;
        if (num<0)
            return std::string();

        return std::string(1, ((char)num)+'0');
    }

    // Runtime configure

    bool enableDetailGroup( std::string detailGroup, bool bEnable = true )
    {
        if (!detailGroup.empty() && detailGroup[0]=='-')
        {
            detailGroup.erase(0,1);
            bEnable = false;
        }

        std::map<std::string, std::set<std::string> >::const_iterator it = detailTypeGroups.find(detailGroup);
        if (it==detailTypeGroups.end())
            return false;

        for( auto g : it->second )
        {
            if (!enableDetailGroup(g, bEnable))
            {
                if (bEnable)
                    enabledDetailTypes.insert(g);
                else
                    enabledDetailTypes.erase(g);
            }
        }

        return true;
    }

    void enableDetails( std::string detailTypeOrGroup, bool bEnable = true )
    {
        if (isDigitGroup(detailTypeOrGroup))
        {
            do
            {
                enableDetailGroup(detailTypeOrGroup);
                detailTypeOrGroup = getLowerDigitGroup(detailTypeOrGroup);
            } while(!detailTypeOrGroup.empty());
        }
        else if (!enableDetailGroup(detailTypeOrGroup, bEnable ))
        {
            if (!detailTypeOrGroup.empty() && detailTypeOrGroup[0]=='-')
            {
                detailTypeOrGroup.erase(0,1);
                bEnable = false;
            }
            if (bEnable)
                enabledDetailTypes.insert(detailTypeOrGroup);
            else
                enabledDetailTypes.erase(detailTypeOrGroup);
        }

    }

    bool isDetailEnabled( const std::string &detail ) const
    {
        return enabledDetailTypes.find(detail)!=enabledDetailTypes.end();
    }

    void resetEnabledDetails()
    {
        enabledDetailTypes.clear();
    }



}; // class DetailLevels



