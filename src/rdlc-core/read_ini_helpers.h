#pragma once

#include <regex>
#include <exception>
#include <stdexcept>
#include <stack>

#include "isa.h"
#include "incsearch.h"

#include "sys_utils.h"



//-----------------------------------------------------------------------------
bool readAttributes( std::string curFile, bool allowEndAttributes, int lineNo, std::string &s, std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedMultiVal = std::set<std::string>() );
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedAttrs );
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &allowedAttrs );
bool checkAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &mutAttrs );
bool checkAttrFormat( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::string &attrName, std::string attrExpr );
std::string getAttrValue( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::string &attrName, const std::string &unexistDefVal, const std::string &emptyDefVal );
int convertAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, int firstAttrNo, int defRes, const std::vector<std::string> &mutAttrs, std::string *pVal = 0 );
bool readName( std::string curFile, int lineNo, const std::string &allowedOpenChars, std::string &s, std::string &readTo );


//-----------------------------------------------------------------------------
inline
std::string getOsCanonicalFullPathName( const std::string &fn )
{
    std::string osFullFilename = osGetFullPathName( fn );
    return makeSimpleCanonicalFilename( osFullFilename );
}

//-----------------------------------------------------------------------------
inline
bool attrsAddAttr( std::string curFile, int lineNo, std::map< std::string, std::string > &attrs, std::string name, std::string val, const std::set<std::string> &allowedMultiVal )
{
    GeneratorOptions gopts; // temporary stub for log

    std::map< std::string, std::string >::iterator attrIt = attrs.find(name);

    if (attrIt==attrs.end())
    {
        attrs[name] = val;
        return true;
    }

    if (allowedMultiVal.find(name)==allowedMultiVal.end())
    {
        LOG_ERR<<"attribute '"<<name<<"' already taken\n";
        return false;
    }

    if (!attrIt->second.empty())
    {
        attrIt->second.append(1,';');
    }

    attrIt->second.append(val);
    return true;

}

//-----------------------------------------------------------------------------
inline
char getBracePair( char brace )
{
    switch(brace)
    {
        case '[': return ']';
        case ']': return '[';
        case '{': return '}';
        case '}': return '{';
        case '(': return ')';
        case ')': return '(';
        case '<': return '>';
        case '>': return '<';

        case '\'': return '\'';
        case '\"': return '\"';

        default: return 0;
    }
}

//-----------------------------------------------------------------------------
inline
bool readName( std::string curFile, int lineNo, const std::string &allowedOpenChars, std::string &s, std::string &readTo )
{
    ltrim(s);
    if (s.empty()) 
       return false;

    readTo.clear();

    std::string::size_type i = 0, size = s.size();
    char openQuot = 0;
    //char closePair = 0;

    for(; i!=size; ++i)
    {
        if (readTo.empty() )
        {
            openQuot = contains( allowedOpenChars, s[i] );
            readTo.append(1, s[i]);
        }
        else if (openQuot!=0)
        {
            readTo.append(1, s[i]);
            if (s[i] == getBracePair( openQuot ))
            {
                s.erase( 0, i+1 );
                return true;
            }
        }
        else
        {
            if (s[i]==' ' || s[i]=='\t')
            {
                s.erase( 0, i );
                return true;
            }
            readTo.append(1, s[i]);
        }
    }

    if (readTo.empty() || openQuot!=0)
        return false;

    s.erase( 0, i );
    return true;
}

//-----------------------------------------------------------------------------
inline
bool readAttributes( std::string curFile, bool allowEndAttributes, int lineNo, std::string &s, std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedMultiVal )
{
    GeneratorOptions gopts; // temporary stub for log

    auto pos = s.find_first_not_of( " \t" );
    if (pos==s.npos)
        return true;

    if (s[pos]!='[') // not an attrs open
        return true;


    std::stack<char> braceStack;
    // top, empty, size, push, emplace, pop
    braceStack.push( s[pos] );

    ++pos;
    //int bracketLevel = 1;
    char qChar = 0;

    std::string attrName, attrVal, buf;
    for(; pos!=s.size(); ++pos)
    {
        char ch = s[pos];
        if (ch=='\'' || ch=='"')
        {
            if (qChar==ch)
            {
                qChar = 0;
                continue;
            }
            else
            {
                qChar = ch;
            }

        } else if (ch=='[' || ch=='(' || ch=='{' || ch=='<')
        {
            if (qChar==0)
               braceStack.push( ch );
               //bracketLevel++;
            buf.append(1,ch);
            continue;

        } else if (ch==']' || ch==')' || ch=='}' || ch=='>')
        {
            if (qChar==0)
            {
               if (braceStack.top()!=getBracePair(ch))
               {
                   LOG_ERR<<"closing brace '"<<ch<<"' not matched the open one ('"<<braceStack.top()<<"')\n";
                   return false;
               }

               braceStack.pop();

               //bracketLevel--;
               if (braceStack.empty())
               {
                   s.erase( 0, pos+1 );
                   trim(s);
                   if (s.empty() && !allowEndAttributes)
                   {
                       LOG_ERR<<"found attributes without any other definition\n";
                       return false;
                   }

                   if (!buf.empty())
                   {
                       if (attrName.empty())
                          attrName = buf;
                       else
                          attrVal  = buf;
                
                       if (!attrsAddAttr( curFile, lineNo, attrs, attrName, attrVal, allowedMultiVal ))
                           return false;
                   }

                   return true;
               }
            }
            buf.append(1,ch);
            continue;

        } else if (ch=='=')
        {
            if (qChar==0)
            {
                if (braceStack.size()>1)
                {
                    buf.append(1,ch);
                }
                else
                {
                    attrName = buf;
                    buf.clear();
                }
            }
            else
            {
                buf.append(1,ch);
            }
            continue;

        } else if (ch==';')
        {
            if (qChar==0)
            {
                if (braceStack.size()>1)
                {
                    buf.append(1,ch);
                }
                else
                {
                    if (attrName.empty())
                       attrName = buf;
                    else
                       attrVal  = buf;
                   
                    buf.clear();
                   
                    if (!attrsAddAttr( curFile, lineNo, attrs, attrName, attrVal, allowedMultiVal ))
                        return false;
                   
                    attrName.clear();
                    attrVal.clear();
                }

            }
            else
            {
                buf.append(1,ch);
            }
            continue;

        } else
        {
            if (ch==' ' || ch=='\t')
            {
                if (qChar!=0)
                    buf.append(1,ch);
            }
            else
            {
                buf.append(1,ch);
            }
            
        }

    } // for(; pos!=s.size(); ++pos)

    LOG_ERR<<"attributes - expected enclosing ']', but found end of line\n";

    return false;
}

//-----------------------------------------------------------------------------
inline
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedAttrs )
{
    GeneratorOptions gopts; // temporary stub for log
    for( auto attrFS : attrs )
    {
        if (allowedAttrs.find(attrFS.first)==allowedAttrs.end())
        {
            LOG_ERR<<"attribute '"<<attrFS.first<<"' not allowed here\n";
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
inline
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &allowedAttrs )
{
    return checkAttrsAllowed( curFile, lineNo, attrs, std::set<std::string>(allowedAttrs.begin(), allowedAttrs.end()) );
}

//-----------------------------------------------------------------------------
inline
bool checkAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &mutAttrs )
{
    const GeneratorOptions gopts; // temporary stub
    size_t foundCount = 0;
    std::string lastFound;
    for( auto muta : mutAttrs)
    {
        //auto mutaIt = attrs.find(muta)
        if (attrs.find(muta)!=attrs.end())
        {
            foundCount++;
            if (foundCount>1)
            {
                LOG_ERR<<"attribute '"<<muta<<"' is mutually exclusive with '"<<lastFound<<"' attribute\n";
            }

            lastFound = muta;
        }
    }

    return foundCount < 2;
}

//-----------------------------------------------------------------------------
inline
int convertAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, int firstAttrNo, int defRes, const std::vector<std::string> &mutAttrs, std::string *pAttr, std::string *pVal )
{
    size_t i = 0, size = mutAttrs.size();
    for(; i!=size; ++i)
    {
        std::map< std::string, std::string >::const_iterator cit = attrs.find(mutAttrs[i]);
        if (cit!=attrs.end())
        {
            if (pAttr)
                *pAttr = cit->first;
            if (pVal)
                *pVal = cit->second;

            return firstAttrNo + (int)i;
        }
    }
    return defRes;
}

//-----------------------------------------------------------------------------
inline
bool checkAttrFormat( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::string &attrName, std::string attrExpr )
{
    if (attrExpr.empty())
    {
        attrExpr = "^.*$";
    }
    else
    {
        if (attrExpr.front()!='^')
            attrExpr = "^" + attrExpr;
        if (attrExpr.back()!='$')
            attrExpr.append(1,'$');
    }

    auto it = attrs.find(attrName);
    if (it==attrs.end())
        return true;

    std::regex r = std::regex(attrExpr);
    if (std::regex_match(it->second, r))
        return true;

    return false;
}

//-----------------------------------------------------------------------------
inline
std::string getAttrValue( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::string &attrName, const std::string &unexistDefVal, const std::string &emptyDefVal )
{
    auto it = attrs.find(attrName);

    if (it==attrs.end())
        return unexistDefVal;

    if (it->second.empty())
        return emptyDefVal;

    return it->second;
}

//-----------------------------------------------------------------------------
inline
bool extractRegImportOptionsFromAttributes( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, RegImportOptions &importOptions, bool allowExtraVisibilitySyntax = false )
{
    auto ppp = makeStringVector("public", "protected", "private");

    if (!checkAttrsMutualExclusive( curFile, lineNo, attrs, ppp ))
         return false;

    std::string visibilityModifierStr;
    std::string visibilityAttributeStr;
    importOptions.regVisibility  = convertAttrsMutualExclusive( curFile, lineNo, attrs
                                                              , REG_VISIBILITY_PUBLIC, REG_VISIBILITY_UNSET
                                                              , ppp // makeStringVector("public", "protected", "private")
                                                              , &visibilityAttributeStr, &visibilityModifierStr
                                                              );

    if (!allowExtraVisibilitySyntax)
    {
        if (!visibilityModifierStr.empty()) // visibilityAttributeStr
        {
            LOG_ERR<<"invalid attribute '"<<visibilityAttributeStr<<"' value: '"<<visibilityModifierStr<<"'\n";
            return false;
        }
    }

    if (visibilityModifierStr=="+" || visibilityModifierStr=="publicize")
    {
        importOptions.regVisibilityModifier = -1;
        //importOptions.regVisibilityAssigner = ;
    }
    else if (visibilityModifierStr=="-" || visibilityModifierStr=="privatize")
    {
        importOptions.regVisibilityModifier = +1;
        //importOptions.regVisibilityAssigner = ;
    }
    else if (visibilityModifierStr=="public")
    {
        //importOptions.regVisibilityModifier = ;
        importOptions.regVisibilityAssigner = REG_VISIBILITY_PUBLIC;
    }
    else if (visibilityModifierStr=="protected")
    {
        //importOptions.regVisibilityModifier = ;
        importOptions.regVisibilityAssigner = REG_VISIBILITY_PROTECTED;
    }
    else if (visibilityModifierStr=="private")
    {
        //importOptions.regVisibilityModifier = ;
        importOptions.regVisibilityAssigner = REG_VISIBILITY_PRIVATE;
    }
    else if (visibilityModifierStr.empty())
    {
        // Do nothing
    }
    else
    {
        if (!visibilityAttributeStr.empty())
        {
            LOG_ERR<<"invalid attribute '"<<visibilityAttributeStr<<"' value: '"<<visibilityModifierStr<<"'\n";
            return false;
        }
    }



    std::string syncAttrStr = getAttrValue(curFile, lineNo, attrs, "sync", "", "always");
    if (syncAttrStr.empty())
    {
        //LOG_ERR<<"Invalid attribute 'sync' value\n";
        //return false;
        importOptions.regSync = REG_SYNC_UNSET;
    }
    else if (syncAttrStr=="none")
    {
        importOptions.regSync = REG_SYNC_NONE;
    }
    else if (syncAttrStr=="always")
    {
        importOptions.regSync = REG_SYNC_ALWAYS;
    }
    else 
    {
        importOptions.regSync       = REG_SYNC_CONDITION;
        importOptions.syncCondition = syncAttrStr;
    }

    {
        std::string tags = getAttrValue( curFile, lineNo, attrs, "tag", std::string(), std::string() );
        for( auto &c : tags )
        {
            if (c == ',')
               c = ';';
        }
        std::vector< std::string > tagsVec;
        splitToVector( tags, tagsVec, ';' );
        for(auto s : tagsVec)
        {
            if (!s.empty())
                importOptions.tags.insert(s);
        }
    }

    {
        std::string tags = getAttrValue( curFile, lineNo, attrs, "+tag", std::string(), std::string() );
        for( auto &c : tags )
        {
            if (c == ',')
               c = ';';
        }
        std::vector< std::string > tagsVec;
        splitToVector( tags, tagsVec, ';' );
        for(auto s : tagsVec)
        {
            if (!s.empty())
                importOptions.tagsPlus.insert(s);
        }
    }

    {
        std::string tags = getAttrValue( curFile, lineNo, attrs, "-tag", std::string(), std::string() );
        for( auto &c : tags )
        {
            if (c == ',')
               c = ';';
        }
        std::vector< std::string > tagsVec;
        splitToVector( tags, tagsVec, ';' );
        for(auto s : tagsVec)
        {
            if (!s.empty())
                importOptions.tagsMinus.insert(s);
        }
    }



    return true;
}

//-----------------------------------------------------------------------------
inline
bool readDirectiveWords( std::string curFile, int lineNo, std::string &s, std::vector< std::string > &words, std::string directiveName = "require" )
{
    while( !s.empty())
    {
        std::string w;
        if (!readName( curFile, lineNo, "\'\"", s, w ))
        {
            LOG_ERR<<"'"<<directiveName<<"' directive - invalid parameters\n";
            return false;
        }

        words.push_back(w);
    }

    return true;
}

// bool readName( std::string curFile, int lineNo, const std::string &allowedOpenChars, std::string &s, std::string &readTo )

//-----------------------------------------------------------------------------
inline
bool extractSignAtEnd( std::string curFile, int lineNo, std::string &first, std::string &second, const std::string &attrName )
{
    const GeneratorOptions &gopts = generatorOptions;

    if (first.size()==attrName.size())
        return true;

    if (!startsWith( first, attrName ))
        return true;

    std::string strEnd = std::string( first, attrName.size() );

    if (strEnd!="+" && strEnd!="-")
    {
        LOG_ERR<<"import directive - invalid attribute '"<<first<<"'\n";
        return false;
    }
    
    first.erase( attrName.size() );

    if (!second.empty())
    {
        LOG_ERR<<"import directive - invalid attribute syntax for attribute '"<<first<<"'\n";
        return false;
    }

    second = strEnd;

    return true;
}

//-----------------------------------------------------------------------------
inline
bool readImportParams( std::string curFile, int lineNo, bool allowEndAttributes
                     , std::string &s
                     , std::map< std::string, std::string > &importParams
                     , std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedImportVerbs
                     , const std::set<std::string> &allowedMultiVal = std::set<std::string>()
                     )
{
    const GeneratorOptions &gopts = generatorOptions;

    if (!readAttributes( curFile, allowEndAttributes, lineNo, s, attrs, allowedMultiVal ))
        return false;

    // $import [public,prefix="bat1_"] battery from batteryV1.rdl as batteryV1 

    {
        std::map< std::string, std::string > attrsTmp = attrs;
        attrs.clear();
        for( auto kv : attrsTmp )
        {
            std::string first = kv.first, second = kv.second;

            if ( !extractSignAtEnd( curFile, lineNo, first, second, "public" )
              || !extractSignAtEnd( curFile, lineNo, first, second, "protected" )
              || !extractSignAtEnd( curFile, lineNo, first, second, "private" )
               )
               return false;

            attrs[first] = second;
        }
    }

    std::string name;

    if (!readName( curFile, lineNo, "\'\"", s, name ))
    {
        LOG_ERR<<"'import' directive - invalid device name or device name not taken\n";
        return false;
    }

    importParams["device"] = name;

    ltrim(s);

    while( !s.empty() && s[0]!='-')
    {
        std::string verb;
       
        if (!readName( curFile, lineNo, "\'\"", s, verb ))
        {
            LOG_ERR<<"import directive - invalid parameters\n";
            return false;
        }

        if (allowedImportVerbs.find(verb)==allowedImportVerbs.end())
        {
            LOG_ERR<<"import directive - invalid expression - '"<<verb<<"'\n";
            return false;
        }
       
        if (!readName( curFile, lineNo, (verb=="from" ? "\'\"<" : "\'\""), s, name ))
        {
            LOG_ERR<<"import directive - invalid '"<<verb<<"' name\n";
            return false;
        }

        if (importParams.find(verb)!=importParams.end())
        {
            LOG_ERR<<"import directive - '"<<verb<<"' expression already taken\n";
        }

        importParams[verb] = name;

        ltrim(s);
    }

    if ( !s.empty() && s[0]=='-')
    {
        while(!s.empty() && s[0]=='-')
            s.erase(0,1);
        trim(s);

        if (!s.empty())
           importParams["-"] = s;
    }

    return true;
}
/*
bool readAttributes( std::string curFile, int lineNo, std::string &s, std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedMultiVal = std::set<std::string>() );
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::set<std::string> &allowedAttrs );
bool checkAttrsAllowed( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &allowedAttrs );
bool checkAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::vector<std::string> &mutAttrs );
bool checkAttrFormat( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::string &attrName, std::string attrExpr );
std::string getAttrValue( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, const std::string &attrName, const std::string &unexistDefVal, const std::string &emptyDefVal );
int convertAttrsMutualExclusive( std::string curFile, int lineNo, const std::map< std::string, std::string > &attrs, int firstAttrNo, int defRes, const std::vector<std::string> &mutAttrs );
bool readName( std::string curFile, int lineNo, const std::string &allowedOpenChars, std::string &s, std::string &readTo );

*/
