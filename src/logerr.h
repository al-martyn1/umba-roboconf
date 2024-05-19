#pragma once


#include "umba/umba.h"
#include "umba/simple_formatter.h"

// requires global vars 
// logGccFormat
// logSourceInfo

//struct GeneratorOptions;
// if (generatorOptions.allowWarn("warn"))

#include "rdlc-core/log.h"
#include "roboconf_options.h"

#include <stdio.h>

inline
umba::SimpleFormatter& startLogMessage( RoboconfOptions         &rbcOpts
                                      , LogEntryType            logEntryType
                                      , const std::string       &msgType
                                      , size_t fileNo, unsigned lineNo
                                      , const char* srcFile = 0, unsigned srcLineNo = 0
                                      )
{
    if (logEntryType==LogEntryType::warn && !rbcOpts.isWarningAllowed(msgType))
        return rbcOpts.getNulStream();

    if (logEntryType==LogEntryType::msg && !rbcOpts.isMessageAllowed(msgType))
        return rbcOpts.getNulStream();


    umba::SimpleFormatter &s = rbcOpts.getLogStream( logEntryType );

    using namespace umba::omanip;

    if (logEntryType==LogEntryType::err || logEntryType==LogEntryType::warn)
    {
        if (logEntryType==LogEntryType::err)
            s<<error;
        else
            s<<warning;
    }

    if ( fileNo!=(size_t)-1)
    {
        // format file:line
        if (rbcOpts.logOptions.logGccFormat)
        {
            s<<rbcOpts.getFileName(fileNo)<<":"<<lineNo<<": ";
        }
        else
        {
            s<<rbcOpts.getFileName(fileNo)<<"("<<lineNo<<"): ";
        }

    }

    if (logEntryType==LogEntryType::err)
    {
        s<< "error:";
    }
    else if (logEntryType==LogEntryType::warn)
    {
        s<< "warning";
        if (rbcOpts.logOptions.logWarnType)
            s<< "("<<msgType<<")";
        s<< ":";
    }
    else if (logEntryType==LogEntryType::msg)
    {
        if (rbcOpts.logOptions.logMsgType)
        {
            s<< "info";
            //if (rbcOpts.logOptions.logWarnType)
                s<< "("<<msgType<<")";
            s<< ":";
        }
    }

    if (logEntryType==LogEntryType::err || logEntryType==LogEntryType::warn)
        s<<normal;

    if (logEntryType==LogEntryType::err || logEntryType==LogEntryType::warn || logEntryType==LogEntryType::msg)
        s<<" ";

    if (srcFile && rbcOpts.logOptions.logSourceInfo)
    {
        s<<coloring( UMBA_TERM_COLORS_MAKE_COMPOSITE( umba::term::colors::blue, umba::term::colors::color_default, true  ,  false,  false ) );
        if (rbcOpts.logOptions.logGccFormat)
        {
            s<<"["<<srcFile<<":"<<srcLineNo<<"]:";
        }
        else
        {
            s<<"["<<srcFile<<"("<<srcLineNo<<")]: ";
        }

        s<<normal<<" ";
    }

    //if (logEntryType==LogEntryType::err && rbcOpts.pauseOnError)
    {
        //#if defined(UMBA_MSVC_COMPILER_USED)

        //getchar();

        //#endif
    }

    return s;
}

/*
inline
umba::SimpleFormatter& startLogError( umba::SimpleFormatter &s, bool bWarning
                                    , const std::string const char* inputFile, unsigned inputLineNo
                                    , const char* srcFile = 0, unsigned srcLineNo = 0
                                    )
{
    return startLogError( umba::SimpleFormatter &s, bool bWarning
                                    , const char* inputFile, unsigned inputLineNo
                                    , const char* srcFile = 0, unsigned srcLineNo = 0
                                    )
}
*/

// source parsing errors
#define LOG_ERR                      startLogMessage( rbcOpts, LogEntryType::err , "err"    , fileNo, lineNo, __FILE__, __LINE__ )
#define LOG_WARN(warnType)           startLogMessage( rbcOpts, LogEntryType::warn, warnType , fileNo, lineNo, __FILE__, __LINE__ )

#define LOG_ERR_OBJ(obj)             startLogMessage( rbcOpts, LogEntryType::err , "err"    , (obj).fileNo, (obj).lineNo, __FILE__, __LINE__ )
#define LOG_WARN_OBJ(warnType, obj)  startLogMessage( rbcOpts, LogEntryType::warn, warnType , (obj).fileNo, (obj).lineNo, __FILE__, __LINE__ )

// options and other errors
#define LOG_ERR_OPT                  startLogMessage( rbcOpts, LogEntryType::err , "err"    , (FileSet::file_id_t)-1, 0, __FILE__, __LINE__ )
#define LOG_WARN_OPT(warnType)       startLogMessage( rbcOpts, LogEntryType::warn, warnType , (FileSet::file_id_t)-1, 0, __FILE__, __LINE__ )

#define LOG_MSG_OPT                  startLogMessage( rbcOpts, LogEntryType::msg ,  /* gopts, */  "msg", 0, 0, 0, 0 )
#define LOG_MSG(msgType)             startLogMessage( rbcOpts, LogEntryType::msg ,  /* gopts, */  msgType /* "msg" */ , 0, 0, 0, 0 )
                                    
//#define LOG_MSG(msgType)             startLogMessage( rbcOpts, LogEntryType::msg , msgType  , (FileSet::file_id_t)-1, 0, 0, 0 )
//#define LOG_MSG_OPT(msgType)         startLogMessage( rbcOpts, LogEntryType::msg , msgType  , (FileSet::file_id_t)-1, 0, 0, 0 )

