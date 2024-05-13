#pragma once

inline
std::string::size_type searchAndReplace( std::string &textWhere, const std::string &searchFor, const std::string &replaceWith, std::string::size_type startFrom = 0 )
{
    auto pos = textWhere.find(searchFor, startFrom);
    if (pos==textWhere.npos)
        return pos;

    textWhere.erase( pos, searchFor.size() );
    textWhere.insert( pos, replaceWith );

    return pos + replaceWith.size();
}

inline
size_t searchAndReplaceAll( std::string &textWhere, const std::string &searchFor, const std::string &replaceWith )
{
    size_t count = 0;
    std::string::size_type pos = 0;
    do
    {
        pos = searchAndReplace( textWhere, searchFor, replaceWith, pos );
        ++count;

    } while(pos!=std::string::npos);

    return count - 1;
}

