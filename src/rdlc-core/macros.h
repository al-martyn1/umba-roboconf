#pragma once

#include <string>
#include <map>
#include <set>


struct SimpleMacroNameSymbolsValidator
{
       bool operator()( char ch ) const
       {
               if (ch>='a' && ch<='z')
                   return true;
               if (ch>='A' && ch<='Z')
                   return true;
               if (ch>='0' && ch<='9')
                   return true;
               if (ch==':' || ch=='_' || ch=='.')
                   return true;
               /*
               if (ch=='<' || ch=='>')
                   return true;
               if (ch=='{' || ch=='}')
                   return true;
               if (ch=='#')
                   return true;
               */    
               return false;    
       }
};

struct RegexMacroNameSymbolsValidator
{
       bool operator()( char ch ) const
       {
               if (ch>='a' && ch<='z')
                   return true;
               if (ch>='A' && ch<='Z')
                   return true;
               if (ch>='0' && ch<='9')
                   return true;
               if (ch==':' || ch=='_' || ch=='.')
                   return true;
               if (ch=='{' || ch=='}')
                   return true;
                   
               return false;    
       }
       
};


template< typename TNameCharChecker >
std::string macroExpandString( const std::string &text
                             , const std::map< std::string, std::string > &macros
                             , const TNameCharChecker &nameCharChecker
                             , const std::string &syntax 
                             , const std::string &unknownPrefix
                             , const std::string &recursePrefix
                             , const std::set<std::string> &usedMacros
                             )
{
       std::string res;
       std::string nameBuf;

       if (syntax.size()<3)
           return std::string("Bad syntax taken");

       const char startChar  = syntax[0];
       const char openChar   = syntax[1];
       const char closeChar  = syntax[2];
       
       static const unsigned state_WaitStart = 1;
       static const unsigned state_WaitOpen  = 2;
       static const unsigned state_ReadName  = 3; // WaitClose
       
       unsigned state = state_WaitStart;
       
       for( auto ch : text )
       {
               switch(state)
               {
                       case state_WaitStart: 
                           {
                                   if (ch==startChar)
                                   {
                                           state = state_WaitOpen;
                                           break;
                                   }
                                   
                                   res.append(1, ch);
                                   break;
                           }

                       case state_WaitOpen: 
                           {
                                   if (ch!=openChar)
                                   {
                                           res.append(1, startChar);
                                           if (ch==startChar)
                                           {
                                                   // keep state state_WaitOpen
                                           }
                                           else
                                           {
                                                   res.append(1,ch);
                                                   state = state_WaitStart;
                                           }
                                           break;
                                   }
                                   state = state_ReadName;
                                   break;
                           }
                           
                       case state_ReadName: 
                           {
                                   if (ch==closeChar)
                                   {
                                           if (usedMacros.find(nameBuf)==usedMacros.end())
                                           {
                                                   // not used before
                                                   auto mit = macros.find(nameBuf);
                                                   if (mit==macros.end())
                                                   {
                                                           // unknown macro, put is input
                                                           res.append(unknownPrefix);
                                                           res.append(1, startChar);
                                                           res.append(1, openChar);
                                                           res.append(nameBuf);
                                                           res.append(1, closeChar);
                                                           nameBuf.clear();
                                                           state = state_WaitStart;
                                                           break;
                                                   }
                                                   else
                                                   {
                                                           // known macro, expand it itself before insertion
                                                           std::set<std::string> usedMacrosCopy = usedMacros;
                                                           usedMacrosCopy.insert(nameBuf);
                                                           res.append( macroExpandString( mit->second
                                                                                        , macros
                                                                                        , nameCharChecker
                                                                                        , syntax
                                                                                        , unknownPrefix
                                                                                        , recursePrefix
                                                                                        , usedMacrosCopy
                                                                                        )
                                                                     );
                                                           nameBuf.clear();
                                                           state = state_WaitStart;
                                                           break;
                                                   }
                                                   
                                           }
                                           else
                                           {
                                                   // already used, insert as text
                                                   res.append(recursePrefix);
                                                   res.append(1, startChar);
                                                   res.append(1, openChar);
                                                   res.append(nameBuf);
                                                   res.append(1, closeChar);
                                                   nameBuf.clear();
                                                   state = state_WaitStart;
                                                   break;
                                           }
                                   }
                                   else if (nameCharChecker(ch))
                                   {
                                           // macro name reading continue
                                           nameBuf.append(1, ch);
                                           break;
                                   }
                                   else
                                   {
                                           // invalid name char, but no end found - append as is
                                           res.append(1, startChar);
                                           res.append(1, openChar);
                                           res.append(nameBuf);
                                           nameBuf.clear();
                                           state = state_WaitStart;
                                           break;
                                   }
                           }
               }
       }

       if (!nameBuf.empty())
       {
               // inclomplete macro - missing closing brace
           res.append(1, startChar);
           res.append(1, openChar);
           res.append(nameBuf);
       }
       
       return res;
}                            


template< typename TNameCharChecker = SimpleMacroNameSymbolsValidator >
std::string macroExpandString( const std::string &text
                             , const std::map< std::string, std::string > &macros
                             , const TNameCharChecker &nameCharChecker = SimpleMacroNameSymbolsValidator()
                             , const std::string &syntax = "$()"
                             , const std::string &unknownPrefix = "<UNKNOWN>"
                             , const std::string &recursePrefix = "<RECURSE>"
                             )
{
       std::set<std::string> usedMacros;
       return macroExpandString( text, macros, nameCharChecker, syntax, unknownPrefix, recursePrefix, usedMacros );
}

/*

int main() {
       // your code goes here
       
       std::map< std::string, std::string > macros;
       macros["M1"] = "Macro#1";
       macros["M2"] = "Macro#2 - $(M1)$(M1) (twice M1), and M3 - $(M3)";
       macros["M3"] = "(try recurse macros - $(M2))";
       macros["Q"] = "QUESTION";
       
       std::string text = "The $$$ $(M1) or the $(M2) - that is the $(Q). And Unk is $(Unk)";
       
       std::cout<< macroExpandString( text, macros, SimpleMacroNameSymbolsValidator() );
       
       
       return 0;
}

*/