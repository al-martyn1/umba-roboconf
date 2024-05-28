#pragma once


#include "reports.h"
#include "datacheetUtils.h"
#include "encoding.h"
#include "roboconf_options.h"

#include <sstream>


struct ReportHtmlBase : public IReportGenerator
{
    std::string htmlDocType( const std::string &suf = "\n" ) const
    {
        return std::string("<!DOCTYPE html>\n<html>") + suf;
    }

// https://habr.com/ru/articles/160997/
    std::string htmlStyle( const std::string &suf = "\n" ) const
    {
        return std::string("<style>\n"
"@media print {\n"
"td, th {\n"
"  margin : 0;\n"
"  padding: 0.3pt;"
"}\n"

".screen-only { display: none; }\n"
"a.datasheet { display: none; }\n"
".even {\n"
"  background-color: #F4F4F4 /* #efeff2 */ ;\n"
"}\n"
".odd {\n"
"  background-color: #FFFFFF;\n"
"}\n"
"body {\n"
"  font-size: 9pt /* 55% */  /* 1.3rem */ ;\n"
"}\n"
"/*header { background: none; color: #000; }*/\n"
"/*header img { -webkit-filter: invert(100%);*/\n"
"/*filter: invert(100%); }*/\n"
"}\n"
"\n"
"h1,h2,h3,h4 {\n\
    COLOR: #3570a0;\n\
}\n\
\n"
".even {\n"
"  background-color: #F4F4F4 /* #efeff2 */ ;\n"
"}\n"
".odd {\n"
"  background-color: #FFFFFF;\n"
"}\n\
body{\n\
    /*word-break: break-all;*/\n\
}\n\
\n\
TH {\n\
    /*font-family: Verdana, Geneva, sans-serif;*/\n\
    background-color: #D4D4D4;\n\
}\n\
\n"
"td, th {\n"
"  margin : 0;\n"
"  padding: 1pt;"
"}\n"
"th {\n\
    font-weight: bold;\n\
    text-align: -internal-center;\n\
}\n\
\n\
TD {\n\
    /*font-family: Verdana, Geneva, sans-serif;*/\n\
    /*background-color: #F4F4F4;*/\n\
}\n\
\n\
td, th {\n\
    display: table-cell;\n\
    vertical-align: inherit;\n\
}\n\
.warning, .warning TD {\n\
    background-color: #FFE4E4;\n\
    color: #FF5555;\n\
    padding: 1pt;\n\
    font-family: Verdana, Geneva, sans-serif;\n\
    text-align: justify;\n\
}\n\
.w3p {\n\
    width: 3% !important;\n\
}\n\
.w5p {\n\
    width: 5% !important;\n\
}\n\
.w6p {\n\
    width: 6% !important;\n\
}\n\
.w8p {\n\
    width: 8% !important;\n\
}\n\
.w10p {\n\
    width: 10% !important;\n\
}\n\
.w12p {\n\
    width: 12% !important;\n\
}\n\
.w15p {\n\
    width: 15% !important;\n\
}\n\
.w20p {\n\
    width: 20% !important;\n\
}\n\
.w25p {\n\
    width: 25% !important;\n\
}\n\
.w28p {\n\
    width: 28% !important;\n\
}\n\
.w30p {\n\
    width: 30% !important;\n\
}\n\
.w40p {\n\
    width: 40% !important;\n\
}\n\
.w50p {\n\
    width: 50% !important;\n\
}\n\
.w60p {\n\
    width: 60% !important;\n\
}\n\
.w70p {\n\
    width: 70% !important;\n\
}\n\
.w80p {\n\
    width: 80% !important;\n\
}\n\
.w90p {\n\
    width: 90% !important;\n\
}\n\
.w95p {\n\
    width: 95% !important;\n\
}\n\
.w100p {\n\
    width: 100% !important;\n\
}\n\
\n\
\n\
\n</style>\n") + suf;
    }

/*

<table border="0" cellspacing="2" cellpadding="5"><tbody><tr><th><b>Фаза I: 1971–1980 гг.</b></th><th><b>Фаза II: 1981–1990 гг.</b></th><th><b>Фаза III: 1991–2000 гг.</b></th></tr>
<tr><td><b>Период изобретения</b></td><td><b>Период замешательства</b></td><td><b>Период созревания</b></td></tr>
<tr><td>Моделирование дискретных событий</td><td>WIMP-интерфейсы Xerox &amp; Apple</td><td>Акцент на анализ, проектирование, архитектуру и бизнес модели</td></tr>
<tr><td>Язык Simula</td><td>Расширения Lisp</td><td>Коммерческие приложения</td></tr>
<tr><td>Kay: машина Flex</td><td>Среда ИИ </td><td>Распределенные и Web системы</td></tr>
<tr><td>PARC: Dynabook</td><td>Новые языки: Eiffel, C++, …</td><td>Объектно ориентированные базы данных</td></tr>
<tr><td>Smalltalk</td><td></td><td>Стандарты, шаблоны, Java, компоненты, миграция</td></tr>
</tbody></table>

SPAN.title {
    COLOR: #3570a0;
}

table {
    display: table;
    border-collapse: separate;
    border-spacing: 2px;
    border-color: grey;
}

.percent10 {
    width: 10% !important;
}
.text-center {
    text-align: center !important;
}
table thead tr th, table thead tr td {
    font-size: 14px;
    font-weight: normal;
}
table thead tr th, table thead tr td {
    padding: .5rem .625rem .625rem;
    font-size: .875rem;
    font-weight: bold;
    color: #222;
}
table {
    display: table;
    border-collapse: separate;
    border-spacing: 2px;
    border-color: grey;
}
*/

    std::string prepareLinkUrl(RoboconfOptions &rbcOpts, std::string url) const
    {
        // Из системной CP конвертируем в UTF-8

        if (isDatasheetNetworkLink(url))
            return url; // Для сетевого имени ничего не делаем


        // Перекодируем из ANSI в UTF8
        EncodingsApi* pEncApi = getEncodingsApi();
        UINT sysCp = pEncApi->getSystemCharMulticharCodePage();
        //std::string sysCpName = pEncApi->getCodePageName(sysCp);

        // UINT cpSrc = pEncApi->getCodePageByName( srcEnc );
        // if (!cpSrc)
        //     throw std::runtime_error( std::string("Unknown or unsupported file encoding - ") + srcEnc );
        //  
        // UINT cpDst = pEncApi->getCodePageByName( targetEnc );
        // if (!cpDst)
        //     throw std::runtime_error( std::string("Unknown or unsupported target encoding - ") + targetEnc );
    
        url = pEncApi->convert(url, sysCp, pEncApi->cpid_UTF8 );

        if (isDatacheetAbsFileName(url))
        {
            url = "file:///" + url;
        }

        return url;
    }

    //<img alt="My Image" src="data:image/png;base64,iVBORwA<MoreBase64SringHere>" />

    std::string makeTextLink( RoboconfOptions &rbcOpts, std::string url, const std::string &text, std::string title = std::string(), std::string target = std::string() ) const
    {
        url = prepareLinkUrl(rbcOpts, url);

        if (!title.empty())
            title += " - ";
        title += url;

        if (title.empty())
            title = text;

        std::ostringstream oss;
        oss << "<a ";
        if (!target.empty())
        {
            oss << "target=\"" << target << "\" ";
        }
        oss << "title=\"" << title << "\" ";
        oss << "href=\""  << url << "\">";
        oss << text << "</a>";

        return oss.str();
        
    }

    std::string makeIconLink( RoboconfOptions &rbcOpts, std::string url /* , const std::string &iconUrl */ , const std::string &text, std::string title = std::string(), std::string target = std::string() ) const
    {
        url = prepareLinkUrl(rbcOpts, url);

        if (!title.empty())
            title += " - ";
        title += url;

        if (title.empty())
            title = text;

        std::ostringstream oss;
        oss << "<a class=\"datasheet\"";
        if (!target.empty())
        {
            oss << "target=\"" << target << "\" ";
        }
        oss << "title=\"" << title << "\" ";
        oss << "href=\""  << url << "\">";

        std::string iconData;
        std::string foundName;
        if (rbcOpts.findIcon(url, foundName, iconData, false /* !quetMode */ ))
        {
            oss << "<img alt=\"" << text << "\" ";
            oss << "src=\"" << iconData << "\"/>";
        }

        oss << "</a>";

        return oss.str();

        // return std::string("<a ") + (target.empty() ? std::string() : std::string("target=\"")+target+std::string("\" ")) 
        //      + std::string("title=\"") + title + std::string("\" href=\"") + url + std::string("\"><img alt=\"") + text + std::string("\" src=\"") + iconUrl + std::string("\"/></a>");
    }


/*
    std::string (  const std::string &suf = "\n" ) const
    {
        return std::string() + suf;
    }

    std::string (  const std::string &suf = "\n" ) const
    {
        return std::string() + suf;
    }
*/
    

}; // struct ReportHtmlBase


