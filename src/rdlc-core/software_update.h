#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sstream> 
#include <process.h> 

//-----------------------------------------------------------------------------
inline
std::string swuGetTempPath()
{
    #if defined(WIN32) || defined(_WIN32)

    char buf[1024] = "";
    DWORD res = GetTempPathA( sizeof(buf)/sizeof(buf[0]), &buf[0] );

    return std::string( &buf[0], res );

    #else

    return "/tmp";

    #endif
}

//-----------------------------------------------------------------------------
std::string swuGetVersionLocalDistrOnShare( std::string distrPath, std::string exeSubPathName, std::string verOption = "-v" )
{
    std::string tempFolderPath = swuGetTempPath();

    std::string distrExecutable = appendPath( distrPath, exeSubPathName );
    distrExecutable = makeSimpleCanonicalFilename( distrExecutable );

    std::string distrVersionFilename = appendPath( tempFolderPath, makeCppName( exeSubPathName ) + std::string("_ver.txt") );

    remove(distrVersionFilename.c_str());

    system( (distrExecutable + std::string(" ") + verOption + std::string(">") + distrVersionFilename).c_str() );
     
    std::ifstream verFileStream(distrVersionFilename.c_str());

    std::string strDistrVer;
    if (!std::getline( verFileStream, strDistrVer ))
    {
        return std::string();
    }

    return strDistrVer;

}

//-----------------------------------------------------------------------------
/*
    Проверить версию
    -1 - error
     0 - same ver
     1 - not same
*/
inline
int swuCompareVersionAndLog( std::string strThisVersion, std::string strDistrVer )
{
    /*
    std::string tempFolderPath = swuGetTempPath();

    std::string distrExecutable = appendPath( distrPath, exeSubPathName );
    distrExecutable = makeSimpleCanonicalFilename( distrExecutable );

    std::string distrVersionFilename = appendPath( tempFolderPath, makeCppName( exeSubPathName ) + std::string("_ver.txt") );

    remove(distrVersionFilename.c_str());

    system( (distrExecutable + std::string(" ") + verOption + std::string(">") + distrVersionFilename).c_str() );
     
    std::ifstream verFileStream(distrVersionFilename.c_str());

    if (!verFileStream)
    {
        return -1;
    }

    std::string strDistrVer;
    if (!std::getline( verFileStream, strDistrVer ))
    {
        return -1;
    }
    */

    //std::string strDistrVer = swuGetVersionLocalDistrOnShare( distrPath, exeSubPathName, verOption );

    std::vector< unsigned > remoteVer = readNumericParts(strDistrVer);
    std::vector< unsigned > thisVer   = readNumericParts(strThisVersion);

    LOG_MSG_OPT<<"Current version: "<<strThisVersion<<"\n";
    LOG_MSG_OPT<<"Remote version : "<<strDistrVer<<"\n";

    int cmpRes = compareUnsignedVectors( thisVer, remoteVer );
    if (cmpRes<0)
    {
        LOG_WARN_OPT("version")<<"Remote version is newer\n";
        return 1;
    }
    if (cmpRes>=0) //-V547
    {
        LOG_MSG_OPT<<"You have an actual version of the software\n";
    }

    return 0;
}


//xcopy /E /Y /I ..\distr s:\rdlc_stable\manual_build

//-----------------------------------------------------------------------------
inline
int swuCheckVersionLocalDistrOnShare( std::string distrPath, std::string exeSubPathName, std::string strThisVersion, std::string verOption = "-v" )
{
    return swuCompareVersionAndLog( strThisVersion, swuGetVersionLocalDistrOnShare( distrPath, exeSubPathName, verOption ) );
}

//-----------------------------------------------------------------------------
inline
std::string swuGenerateLastCheckTimeFilename( std::string exeSubPathName )
{
    std::string tempFolderPath = swuGetTempPath();
    return appendPath( tempFolderPath, makeCppName( exeSubPathName ) + std::string("_swu_last_check.txt") );
}

//-----------------------------------------------------------------------------
inline
std::string swuGenerateDontCheckFilename( std::string exeSubPathName )
{
    std::string tempFolderPath = swuGetTempPath();
    return appendPath( tempFolderPath, makeCppName( exeSubPathName ) + std::string("_swu_dont_check.txt") );
}

//-----------------------------------------------------------------------------
inline
void swuClearCheckNeedCheckUpdate( std::string exeSubPathName )
{
    remove(swuGenerateLastCheckTimeFilename(exeSubPathName).c_str());
    remove(swuGenerateDontCheckFilename(exeSubPathName).c_str());
}

//-----------------------------------------------------------------------------
// true - need to check
inline
bool swuCheckNeedCheckUpdate( std::string exeSubPathName, time_t checkPeriod = 86400/2 )
{
    std::ifstream dontCheckStream(swuGenerateDontCheckFilename(exeSubPathName).c_str());
    if (!!dontCheckStream)
        return false;

    std::string lastCheckTimeFilename = swuGenerateLastCheckTimeFilename(exeSubPathName);

    time_t timeNow = time(0);

    std::ifstream in(lastCheckTimeFilename.c_str());

    if (!in)
    {
        std::ofstream out(lastCheckTimeFilename.c_str());
        out<<timeNow;
        return true;
    }

    time_t timeLastUpdate = 0;

    in>>timeLastUpdate;
    in.close();

    std::ofstream out(lastCheckTimeFilename.c_str());
    out<<timeNow;

    time_t dt = timeNow - timeLastUpdate;
    if (dt>checkPeriod)
        return true;

    return false;
}

//-----------------------------------------------------------------------------
inline
void swuUpdateVersionLocalDistrOnShare(std::string distrPath, std::string thisRootPath, std::string exeSubPathName)
{
    #if defined(WIN32) || defined(_WIN32)
    std::string runCmd = appendPath( swuGetTempPath(), makeCppName( exeSubPathName ) + std::string("_update.bat") );
    #else
    std::string runCmd = appendPath( swuGetTempPath(), makeCppName( exeSubPathName ) + std::string("_update.sh") );
    #endif

    remove(runCmd.c_str());

    {
        std::ofstream out(runCmd.c_str());
        #if defined(WIN32) || defined(_WIN32)
        out<<"timeout 3\n";
        out<<"xcopy /E /Y /I "<<distrPath<<" "<<thisRootPath<<"\n";
        // /Q 
        #endif
    }

    #if defined(_MSC_VER)
    _spawnl( _P_OVERLAY, runCmd.c_str(), runCmd.c_str(), 0 );
    #else
    spawnl( P_OVERLAY, runCmd.c_str(), runCmd.c_str(), 0 );
    #endif

}

//-----------------------------------------------------------------------------
inline
bool swuAutoCheckVersionLocalDistrOnShare( std::string caption, std::string distrPath, std::string thisRootPath, std::string exeSubPathName, std::string strThisVersion, std::string verOption = "-v", time_t checkPeriod = 86400/2 )
{
    if (swuCheckNeedCheckUpdate( exeSubPathName, checkPeriod ))
    {
        std::string remoteVer = swuGetVersionLocalDistrOnShare( distrPath, exeSubPathName, verOption );
        auto cmpRes = swuCompareVersionAndLog( strThisVersion, remoteVer );
        if (cmpRes>0)
        {
            #if defined(WIN32) || defined(_WIN32)

            std::stringstream ss;
            ss<<"There is a newest version of the software - "<<remoteVer<<".\nDo you want to update it?";
            auto res = MessageBoxA( 0, ss.str().c_str(),  /* getNameFromFull( exeSubPathName ) */ caption.c_str(), MB_YESNOCANCEL | MB_ICONEXCLAMATION );
            if (res==IDYES || res==IDOK)
            {
                // Do update
                swuUpdateVersionLocalDistrOnShare( distrPath, thisRootPath, exeSubPathName);
            }
            else if (res==IDNO)
            {
                std::ofstream dontCheckStream(swuGenerateDontCheckFilename(exeSubPathName).c_str());
                dontCheckStream<<1;
            }
            #endif
        }
    }

    return false;

}




/*
    while( std::getline( in, strReaded ) )


bool readFile( std::string fileName, std::string &readTo )
{
    readTo.clear();
    std::string strReaded;

    std::ifstream input( fileName );
    if (!input)
    {
        GeneratorOptions &gopts = *this;
        LOG_ERR_OPT<<"Failed to read prolog/epilog file '"<<fileName<<"'\n";
        return false;
    }

    while( std::getline( input, strReaded ) )
    {
        if (!readTo.empty())
            readTo.append(1, '\n');
        readTo.append(strReaded);
    }

    return true;
}
*/

