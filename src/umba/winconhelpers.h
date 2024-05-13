#pragma once

//#include <ntstatus.h>

#include <winternl.h>

#ifdef UNICODE
    #undef UNICODE
    #include <tlhelp32.h>
    #define UNICODE
#else
    #include <tlhelp32.h>
#endif


#if defined(WIN32) || defined(_WIN32)

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <stdio.h>
    #include <io.h>

#else

    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>   // for isatty()
    #include <stdio.h>    // for fileno()

    #define _isatty isatty
    #define _fileno fileno
    #define _tell   tell
    #define S_IFCHR _S_IFCHR

#endif

#if !defined(S_ISCHR)
    #define S_ISCHR(m) ((m)&(_S_IFCHR))
#endif


#include <string>
#include <vector>
#include <set>

#ifdef DEBUG_WINCONHELPERS
    
    #include <io.h>
    #include <fcntl.h>
    #include <sys/types.h>
    #include <sys/stat.h>

    #ifndef _MSC_VER
        #define _write write
    #endif

    #define  DEBUG_WINCONHELPERS_WRITE_STR(str) do { write( 1 /* stdout */ , (const void*)str, strlen((const char*)str) ); } while(0)
    #define  DEBUG_WINCONHELPERS_PRINTF( ... )  do { printf(__VA_ARGS__); fflush( stdout ); } while(0)

#else

    #define  DEBUG_WINCONHELPERS_WRITE_STR(str) str
    #define  DEBUG_WINCONHELPERS_PRINTF( ... )

#endif


namespace winhelpers
{

namespace utils
{

//-----------------------------------------------------------------------------
inline
char toUpper( char ch )
{
    if (ch>='a' && ch<='z')
        return ch - 'a' + 'A';
    return ch;
}

inline
char toLower( char ch )
{
    if (ch>='A' && ch<='Z')
        return ch - 'A' + 'a';
    return ch;
}

//-----------------------------------------------------------------------------
inline
std::string toUpper( std::string s )
{
    for (char& ch : s) 
    {
        ch = toUpper(ch);
    }

    return s;
}

inline
std::string toLower( std::string s )
{
    for (char& ch : s) 
    {
        ch = toLower(ch);
    }

    return s;
}

//-----------------------------------------------------------------------------
inline
std::vector<std::string> toUpper( std::vector<std::string> v )
{
    for (std::string& s : v) 
    {
        s = toUpper(s);
    }

    return v;
}

inline
std::vector<std::string> toLower( std::vector<std::string> v )
{
    for (std::string& s : v) 
    {
        s = toLower(s);
    }

    return v;
}


} // namespace utils






inline
bool seekableChecker(int fd)
{
    off_t off = _tell(fd);
    if (off==(off_t)-1)
        return false;

    // Проблемы под Win10 - простой tell не работает
    // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/lseek-lseeki64?view=vs-2017
    long seekRes = _lseek(fd, 0, SEEK_SET);
    if (seekRes==-1l)
        return false;

    // В начало сикнулись без проблем

    seekRes = _lseek(fd, 0, SEEK_END);
    if (seekRes==-1l)
        return false;

    // в конец тоже сикнулись без проблем

    // Восстанавливаем, раз уж сикнулись
    _lseek(fd, off, SEEK_SET);

    return true;
}

inline
bool isConsoleAttached( int fd )
{
    if (seekableChecker(fd))
        return false; // seekable stream - not a console

    if (!_isatty(fd))
        return false;

    struct stat stats;
    fstat( fd, &stats);
    if (!S_ISCHR(stats.st_mode))
        return false;

    return true;
}

inline
void showProcessInformation()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                #ifdef DEBUG_WINCONHELPERS
                printf("pid %d %s\n", pe32.th32ProcessID, pe32.szExeFile);
                #endif
            }
            while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
}

inline
bool GetProcessExeName(DWORD pid, std::string &processExeName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                if (pid==pe32.th32ProcessID)
                {
                    processExeName = pe32.szExeFile;
                    CloseHandle(hSnapshot);
                    return true;
                }
            }
            while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }

    return false;
}



inline
LONG /* NTSTATUS */  NtQueryInformationProcessImpl(
  IN HANDLE           ProcessHandle,
  IN PROCESSINFOCLASS ProcessInformationClass,
  OUT PVOID           ProcessInformation,
  IN ULONG            ProcessInformationLength,
  OUT PULONG          ReturnLength
)
{
    typedef
    LONG (WINAPI *NtQueryInformationProcessFnPtrT)(HANDLE ProcessHandle, ULONG ProcessInformationClass,
            PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);

    NtQueryInformationProcessFnPtrT NtQueryInformationProcessFnPtr = (NtQueryInformationProcessFnPtrT)GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");


    if (!NtQueryInformationProcessFnPtr)
    {
        return (LONG)0xC0000002L;
    }

    return NtQueryInformationProcessFnPtr( ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

inline
ULONG_PTR GetParentProcessIdByHandle( HANDLE hCurProcess = GetCurrentProcess() )
{
    ULONG_PTR pbi[6];
    ULONG ulSize = 0;

    if (NtQueryInformationProcessImpl( hCurProcess, (PROCESSINFOCLASS)0, &pbi, sizeof(pbi), &ulSize) >= 0 && ulSize == sizeof(pbi))
        return pbi[5];
    return (ULONG_PTR)-1;
}

inline
ULONG_PTR GetParentProcessIdByPid( DWORD pid = GetCurrentProcessId() )
{
    HANDLE h = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );
    //HANDLE h = OpenProcess( SYNCHRONIZE, FALSE, pid );
    //HANDLE h = OpenProcess(SYNCHRONIZE, TRUE, pid);
    
    if (h!=0)
    {
        ULONG_PTR res = GetParentProcessIdByHandle( h );
        CloseHandle(h);
        return res;
    }
    return (ULONG_PTR)-1;
}


inline
std::vector<std::string> getProcessParentNames( ULONG_PTR pid = GetCurrentProcessId() )
{
    DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames enter\n");

    std::set<ULONG_PTR> pidSet; // strange bug - sometimes we got next:
/*
    getProcessParentNames, pid: 5336
    getProcessParentNames, parent name: explorer.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 5240
    getProcessParentNames, parent name: chrome.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 4528
    getProcessParentNames, parent name: chrome.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 5336
    getProcessParentNames, parent name: explorer.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 5240
    getProcessParentNames, parent name: chrome.exe
    getProcessParentNames - put parent name
    getProcessParentNames, pid: 4528
    getProcessParentNames, parent name: chrome.exe
*/

    std::vector<std::string> res;
    bool bFirst = true;
    while(pid!=(ULONG_PTR)-1 && pidSet.find(pid)==pidSet.end())
    {
        DEBUG_WINCONHELPERS_PRINTF("getProcessParentNames, pid: %d\n", (unsigned)pid );
        std::string exeName;
        if (!GetProcessExeName( pid, exeName ))
        {
            DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames - invalid pid\n");
            pid = (ULONG_PTR)-1;
        }
        else
        {
            if (bFirst)
            {
                DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames - dont put self name\n");
                bFirst = false;
                pidSet.insert(pid);
            }
            else
            {
                DEBUG_WINCONHELPERS_PRINTF("getProcessParentNames, parent name: %s\n", exeName.c_str() );
                DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames - put parent name\n");
                res.push_back( utils::toLower(exeName) );
                pidSet.insert(pid);
            }
            
            pid = GetParentProcessIdByPid(pid);
        }
    }

    DEBUG_WINCONHELPERS_WRITE_STR("getProcessParentNames leave\n");

    return res;
}

enum class ConsoleType
{
    file,
    windows_console,
    ansi_terminal
};


inline
ConsoleType detectConsoleType( int fd )
{
    std::vector<std::string> parentNames = getProcessParentNames( );
    for( auto parentName : parentNames )
    {
        auto pos = parentName.find_last_of('.');
        if (pos!=parentName.npos)
        {
            parentName.erase( pos, parentName.npos );

            if (parentName=="bash")
            {
                // Виндовый bash от гита всегда аттачит потоки stdout/stderr к файлам, 
                // потом видимо сам парсит и обрабатывает управляющте escape-последовательности
                // и если вывод на консоль - то раскрашивает, если в файл - то отрезает
                return ConsoleType::ansi_terminal;
            }
        }
    }

    if (isConsoleAttached( fd ))
        return ConsoleType::windows_console;

    return ConsoleType::file;
}


} // namespace winhelpers


//DWORD GetCurrentProcessId();

/*
HANDLE GetCurrentProcess(

);

OpenProcess(PROCESS_ALL_ACCESS, TRUE, procId);

HANDLE OpenProcess(
  DWORD dwDesiredAccess,
  BOOL  bInheritHandle,
  DWORD dwProcessId
);
SYNCHRONIZE

IsWow64Process2 - https://docs.microsoft.com/ru-ru/windows/desktop/api/wow64apiset/nf-wow64apiset-iswow64process2
*/



/* 

SetConsoleTextAttribute работает

Far under ConEmu
192 - rdlc.exe
1080 - cmd.exe
15260 - ConEmuC64.exe
10188 - Far.exe
2628 - ConEmuC.exe
9296 - ConEmu.exe

6028 - rdlc.exe
11228 - cmd.exe
5724 - explorer.exe


git bash - SetConsoleTextAttribute не работает

14820 - rdlc.exe
12128 - cmd.exe
8932 - bash.exe
15000 - bash.exe


*/



