/*  
Module : EXCEPTHANDLER.CPP
Purpose: Implementation for a class which intercepts and logs details about unhandled exceptions
         such as access violations, stack overflows and division by zero.
         The class is based upon the February 2002 MSDN article called "Under the Hood: Improved
         Error Reporting with DBGHELP 5.1 by Matt Pietrek. 

         To compile the class you need the November 2002 Platform SDK or later installed.
         You can add the ExceptHandler.cpp/h files directly to your C++ project or put the exception
         handler class in a DLL and arrange for your app / dll to get it installed via a LoadLibrary
         call.

         A binary version of the ExceptHandler is provided in the XCPTHLR.DLL which is included
         in this distribution.

         To get this code to work on client machines you will need to arrange for the distribution
         of the DBGHELP 5.1 dll which can be obtained from November 2002 Platform SDK or later. Select
         "Install Debugging Tools for Windows" to get the DLL installed on your machine. Also
         note that the DBGHELP dll is under Windows System File protection on recent versions of Windows, 
         so put the DBGHELP dll in the same directory as your application.

         To provide symbols for your code in release mode, you should modify your projects settings as 
         follows:

         1. Project Settings -> Link -> Debug (Category) , Enable "Debug Info" and pick "Microsoft Format".
         2. Same Place, pick "Separate Types".
         3. In the Project Options edit box on the same page, add "/OPT:REF". This will ensure that 
         unreferrenced functions are removed from the resultant binary.
         4. Project Settings -> C++ -> General (Category) Debug Info Combo -> "Program Database"
         5. Sample Place, you may need to set "Optimizations" to "Disable (Debug)" if your are getting
         call stacks which do not make sense.

         Also remember to ship the resultant pdb files with your code (or arrange for them to be somehow
         available) so that CExceptionLogger can give the source and line information in the resultant
         log.

         By default the CExceptionLogger class will generate a log file with the name "name of exe.exception".
         In this file (which is ASCII if the CExceptionLogger is build as ASCII and UNICODE if the 
         CExceptionLogger is build as a UNICODE) is the following information:

         1.  Date and Time when the exception occurred.
         2.  Exception Code
         3.  Details about exception if an access violation
         4.  Details about exception address including linear address, section, offset and module path
         5.  Full path of the process
         6.  Current Win32 working directory
         7.  Command line for the process
         8.  Process ID
         9.  Thread ID where the exception occurred.
         10. Enumeration of all the threads in the process (assuming Toolhelp32 is available) including
              i)   Thread ID
              ii)  Priority and Delta priority
              iii) References
              iv)  Creation Time
              v)   Kernel and User Time
         11. Enumeration of the Modules in the process (again only if Toolhelp32 is available) including
              i)   Name and full path
              ii)  Global and per Process reference count
              iii) module handle
              iv)  size
              v)   All symbols for that module fully expanded (see details later)
         12. All the x86 registers
         13. Call Stack where exception occured, including section, offset, module, function and line 
             information. 
         14. At each stack frame as well as for all modules, all variables and parameters all also logged. 
             All basic types such as voids, chars, shorts, words, ints, DWORDS, floats, doubles and longs 
             are logged. UDT's including structs, unions and classes are also fully recursed into to 
             display their members. Info for each type includes its name, address, type and value. 
             In addition if the variable is a array, each value in the array is fully logged.

         Please note that due to all the symbols which even a very small appp has, each log of an exception
         can generate upwards of 100K of data or more. My opinion on this is that disk space is cheap while 
         developers time looking for an intermittent release only bug is not!!!!!.             

         References:

         Bugslayer,      MSJ,  August 1998 by John Robbins, http://www.microsoft.com/msj/defaultframe.asp?page=/msj/0898/bugslayer0898.htm
         Under the Hood, MSDN, March 2002 by Matt Pietrek,  http://msdn.microsoft.com/msdnmag/issues/02/03/Hood/Hood0203.asp



Created: PJN / 14-03-2002
History: None


Copyright (c) 2002 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////// Includes //////////////////////////////
#include "stdafx.h"
#include "ExceptHandler.h"
#ifndef _INC_TCHAR
#pragma message("To avoid this message, put tchar.h in your PCH")
#include <tchar.h>
#endif
#ifndef _INC_TOOLHELP32
#pragma message("To avoid this message, put tlhelp32.h in your PCH")
#include <tlhelp32.h>
#endif
#ifndef _INC_STDIO
#pragma message("To avoid this message, put stdio.h in your PCH")
#include <stdio.h>
#endif


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
//#include <windows.h>
//#include <dbghelp.h>
//#include <stdio.h>
//#include <tchar.h>
//#include <TlHelp32.h>

/////////////////////// Macros / Defines  / Locals //////////////////////

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "oleaut32.lib")

//Class which looks after pulling in ToolHelp32 and OpenThread dynamically. 
//That way the CExceptionlogger class will work on NT 4 (without of course the Thread
//and module lists) and Win95 which does not have OpenThread (but does
//have Toolhelp32!!!!)
class _EXCEPTION_LOGGER_DATA
{
public:
//Constructors /Destructors
  _EXCEPTION_LOGGER_DATA();

  typedef HANDLE (WINAPI CREATETOOLHELP32SNAPSHOT)(DWORD, DWORD);
  typedef CREATETOOLHELP32SNAPSHOT* LPCREATETOOLHELP32SNAPSHOT;

  typedef BOOL (WINAPI THREAD32FIRST)(HANDLE, LPTHREADENTRY32);
  typedef THREAD32FIRST* LPTHREAD32FIRST;

  typedef BOOL (WINAPI THREAD32NEXT)(HANDLE, LPTHREADENTRY32);
  typedef THREAD32NEXT* LPTHREAD32NEXT;

  typedef BOOL (WINAPI MODULE32FIRST)(HANDLE, LPMODULEENTRY32);
  typedef MODULE32FIRST* LPMODULE32FIRST;

  typedef BOOL (WINAPI MODULE32NEXT)(HANDLE, LPMODULEENTRY32);
  typedef MODULE32NEXT* LPMODULE32NEXT;

  typedef HANDLE (WINAPI OPENTHREAD)(DWORD, BOOL, DWORD);
  typedef OPENTHREAD* LPOPENTHREAD;

  LPCREATETOOLHELP32SNAPSHOT m_lpfnCreateToolhelp32Snapshot;
  LPTHREAD32FIRST            m_lpfnThread32First;
  LPTHREAD32NEXT             m_lpfnThread32Next;
  LPMODULE32FIRST            m_lpfnModule32First;
  LPMODULE32NEXT             m_lpfnModule32Next;
  LPOPENTHREAD               m_lpfnOpenThread;
  HMODULE                    m_hKernel32;

  void ResetFunctionPointers();
};

_EXCEPTION_LOGGER_DATA::_EXCEPTION_LOGGER_DATA()
{
  m_hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));
  if (m_hKernel32)
  {
    m_lpfnOpenThread               = (LPOPENTHREAD) GetProcAddress(m_hKernel32, "OpenThread");
    m_lpfnCreateToolhelp32Snapshot = (LPCREATETOOLHELP32SNAPSHOT) GetProcAddress(m_hKernel32, "CreateToolhelp32Snapshot");
    m_lpfnThread32First            = (LPTHREAD32FIRST) GetProcAddress(m_hKernel32, "Thread32First");
    m_lpfnThread32Next             = (LPTHREAD32NEXT) GetProcAddress(m_hKernel32, "Thread32Next");
#ifdef _UNICODE
    m_lpfnModule32First = (LPMODULE32FIRST) GetProcAddress(m_hKernel32, "Module32FirstW");
    m_lpfnModule32Next  = (LPMODULE32NEXT) GetProcAddress(m_hKernel32, "Module32NextW");
#else
    m_lpfnModule32First = (LPMODULE32FIRST) GetProcAddress(m_hKernel32, "Module32First");
    m_lpfnModule32Next  = (LPMODULE32NEXT) GetProcAddress(m_hKernel32, "Module32Next");
#endif
  }
  
  //Any function pointer NULL, then set them all to NULL. Helps simplify the code which 
  //uses the function pointers
  if (m_lpfnCreateToolhelp32Snapshot == NULL || m_lpfnThread32First == NULL || 
      m_lpfnThread32Next == NULL || m_lpfnModule32First == NULL || m_lpfnModule32Next == NULL)
    ResetFunctionPointers();
}

void _EXCEPTION_LOGGER_DATA::ResetFunctionPointers()
{
  m_lpfnCreateToolhelp32Snapshot = NULL;
  m_lpfnThread32First = NULL;
  m_lpfnThread32Next = NULL;
  m_lpfnModule32First = NULL;
  m_lpfnModule32Next = NULL;
  if (m_hKernel32)
  {
    FreeLibrary(m_hKernel32);
    m_hKernel32 = NULL;
  }
}

//Statics used by the CExceptionLogger class
TCHAR                        CExceptionLogger::m_pszLogFilename[_MAX_PATH];
LPTOP_LEVEL_EXCEPTION_FILTER CExceptionLogger::m_pPreviousFilter = NULL;
HANDLE                       CExceptionLogger::m_hLogFile = INVALID_HANDLE_VALUE;
TCHAR                        CExceptionLogger::m_szTempLogLine[4096];
TCHAR                        CExceptionLogger::m_szTempFileName[_MAX_PATH];
BYTE                         CExceptionLogger::m_bySymbolInfo[sizeof(SYMBOL_INFO) + 1024];


//The one and only instance of the crash handler
CExceptionLogger g_CrashHandlerLogger;

//The local variable which handle the function pointers
_EXCEPTION_LOGGER_DATA _ExceptionLoggerData;



/////////////////////// Implementation ////////////////////////

CExceptionLogger::CExceptionLogger()
{
  //Install our exception handler
  m_pPreviousFilter = SetUnhandledExceptionFilter(UnHandledExceptionFilter);

  //The name of the Crash handler log file will by default
  //be "name of exe.crash.log"
  GetModuleFileName(NULL, m_pszLogFilename, _MAX_PATH);
  
  //Note we use the C Runtime here as the exception has not occured yet, so we
  //can be confident that the CRT is ok at this time
  _tcscat(m_pszLogFilename, _T(".exception"));
}

CExceptionLogger::~CExceptionLogger()
{
  //Restore the old exception handler
  SetUnhandledExceptionFilter(m_pPreviousFilter);
}

long CExceptionLogger::UnHandledExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo)
{
  //Open up the log file, Notice we use FILE_FLAG_WRITE_THROUGHT to 
  //avoid caching of writes. That way if our code goes belly up, we will
  //at least have all the data up to then safely written to disk
  m_hLogFile = CreateFile(m_pszLogFilename, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0);
  if (m_hLogFile != INVALID_HANDLE_VALUE)
  {
    //Seek to the end of the Log file so that we always append to the crash log file
    SetFilePointer(m_hLogFile, 0, 0, FILE_END);

    //Let the helper function do its job
    GenerateCrashLog(pExceptionInfo);

    //Close the file now that we are finished with it
    CloseHandle(m_hLogFile);
    m_hLogFile = INVALID_HANDLE_VALUE;
  }

  //Chain to the previous exception if there is one
  if (m_pPreviousFilter)
    return m_pPreviousFilter(pExceptionInfo);
  else
    return EXCEPTION_CONTINUE_SEARCH;
}

void CExceptionLogger::GenerateCrashLog(_EXCEPTION_POINTERS* pExceptionInfo)
{
  //Write out a banner first to separate multiple log entries
  Log(_T("-----------------------------------------------------\r\n"));

  //get the current time so that it goes into the file
  SYSTEMTIME st;
  GetLocalTime(&st);
  Log(_T("Time of Exception: %02d:%02d:%02d.%03d %d/%d/%d (D/M/Y)\r\n"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, st.wDay, st.wMonth, st.wYear);

  //Log the type of unhandled exception
  Log(_T("Exception Code: 0x%08x\r\n"), pExceptionInfo->ExceptionRecord->ExceptionCode);

  //Display extra info if an Access Violation
  if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
  {
    if (pExceptionInfo->ExceptionRecord->NumberParameters > 0)
    {
      if (pExceptionInfo->ExceptionRecord->ExceptionInformation[0] == 0)
        Log(_T("Access Violation Exception: Due to the thread attempting to read from an inaccessible address\r\n"));
      else
        Log(_T("Access Violation Exception: Due to the thread attempting to write to an inaccessible address\r\n"));
    }
    if (pExceptionInfo->ExceptionRecord->NumberParameters > 1)
      Log(_T("Access Violation Address: 0x%08X\r\n"), pExceptionInfo->ExceptionRecord->ExceptionInformation[1]);
  }

  //Is the exception continuable
  if (pExceptionInfo->ExceptionRecord->ExceptionFlags == 0)
    Log(_T("Exception is continuable\r\n"));
  else
    Log(_T("Exception is noncontinuable\r\n"));

  //Log where the fault occurred
  DWORD dwOffset;
  char pszSectionName[IMAGE_SIZEOF_SHORT_NAME+1];
  if (GetAddressDetails(pExceptionInfo->ExceptionRecord->ExceptionAddress, m_szTempFileName, dwOffset, pszSectionName))
    Log(_T("Exception Address Details: 0x%08X [%hs]:0x%08X %s\r\n"), pExceptionInfo->ExceptionRecord->ExceptionAddress, pszSectionName, dwOffset, m_szTempFileName);
  else
  {
    //Fall back to just reporting the linear faulting address, rather than the additional details
    Log(_T("Exception Address Details: 0x%08X [UNKNOWN]\r\n"), pExceptionInfo->ExceptionRecord->ExceptionAddress);
  }

  //Write out the full path to the process which is affected
  GetModuleFileName(NULL, m_szTempFileName, _MAX_PATH);
  Log(_T("Process Path: %s\r\n"), m_szTempFileName);

  //Get the current working directory
  GetCurrentDirectory(_MAX_PATH, m_szTempFileName);
  Log(_T("Current Directory: %s\r\n"), m_szTempFileName);

  //Get the current working directory
  Log(_T("Command Line: %s\r\n"), GetCommandLine());

  //Write out the Process and Thread ID 
  DWORD dwProcessID = GetCurrentProcessId();
  Log(_T("Process ID: 0x%08X\r\n"), dwProcessID);
  Log(_T("Thread ID where exception has occurred: 0x%08X\r\n"), GetCurrentThreadId());

  //Initialize DBGHELP
  HANDLE hProcess = GetCurrentProcess();
  BOOL bInitDBGHelp = SymInitialize(hProcess, NULL, TRUE);
  if (!bInitDBGHelp)
    Log(_T(" Failed to initialize Debug Help Library, Error:%d\r\n"), GetLastError());
  SymSetOptions(SYMOPT_DEFERRED_LOADS);

  //Log details on each module in this process
  HANDLE hToolHelp32Snap = INVALID_HANDLE_VALUE;
  if (_ExceptionLoggerData.m_lpfnCreateToolhelp32Snapshot)
  {
    hToolHelp32Snap = _ExceptionLoggerData.m_lpfnCreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (hToolHelp32Snap != INVALID_HANDLE_VALUE) 
    {
      //Log all the threads in the current process 
      Log(_T("\r\nThreads:\r\n"));

      //Fill in the size of the structure before using it. 
      THREADENTRY32 te32;; 
      te32.dwSize = sizeof(THREADENTRY32); 

      //Walk the thread snapshot to find all threads of the process. 
      //If the thread belongs to the process, add its information 
      //to the display list.
      if (_ExceptionLoggerData.m_lpfnThread32First(hToolHelp32Snap, &te32)) 
      { 
        do 
        { 
          if (te32.th32OwnerProcessID == dwProcessID) 
          {
            Log(_T("  Thread ID: 0x%08X\r\n"), te32.th32ThreadID);
            Log(_T("  Priority Level: %d\r\n"), te32.tpBasePri);
            Log(_T("  Delta Priority: %d\r\n"), te32.tpDeltaPri);
            Log(_T("  References: %d\r\n"), te32.cntUsage);

            //Write out some detailed info on each thread (over and above what ToolHelp32 gives you)
            HANDLE hThread = _ExceptionLoggerData.m_lpfnOpenThread ? _ExceptionLoggerData.m_lpfnOpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID) : NULL;
            if (hThread)
            {
              FILETIME ct;
              FILETIME et;
              FILETIME kt;
              FILETIME ut;
              if (GetThreadTimes(hThread, &ct, &et, &kt, &ut))
              {
                SYSTEMTIME st;
                FileTimeToSystemTime(&ct, &st);
                Log(_T("  Creation Time: %02d:%02d:%02d.%03d %d/%d/%d (D/M/Y)\r\n"), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, st.wDay, st.wMonth, st.wYear);
                unsigned __int64 TimeTaken;
                TimeTaken = (((unsigned __int64)kt.dwHighDateTime) << 32);
                TimeTaken += kt.dwLowDateTime;
                Log(_T("  Kernel Time (Seconds): %I64u\r\n"), TimeTaken/10000000);
                TimeTaken = (((unsigned __int64)ut.dwHighDateTime) << 32);
                TimeTaken += ut.dwLowDateTime;
                Log(_T("  User Time: (Seconds) %I64u\r\n"), TimeTaken/10000000);
              }
              CloseHandle(hThread);
            }
            Log(_T("\r\n"));
          }
        } 
        while (_ExceptionLoggerData.m_lpfnThread32Next(hToolHelp32Snap, &te32)); 
      } 
      else
        Log(_T(" Failed in call to Thread32First, Error:%d\r\n"), GetLastError());

      //Log all the modules in the current process 
      Log(_T("Modules:\r\n"));

      //Fill in the size of the structure before using it. 
      MODULEENTRY32 me32;
      me32.dwSize = sizeof(MODULEENTRY32); 

      //Walk the thread snapshot to find all threads of the process. 
      //If the thread belongs to the process, add its information 
      //to the display list.
      if (_ExceptionLoggerData.m_lpfnModule32First(hToolHelp32Snap, &me32)) 
      { 
        do 
        { 
          if (me32.th32ProcessID == dwProcessID)
          {
            Log(_T(" Name: %s\r\n"), me32.szModule);
            Log(_T(" Path: %s\r\n"), me32.szExePath);
            Log(_T(" Global Usage Count: %d\r\n"), me32.GlblcntUsage);
            Log(_T(" Process Usage Count: %d\r\n"), me32.ProccntUsage);
            Log(_T(" HMODULE: %08X\r\n"), me32.modBaseAddr);
            Log(_T(" Size: %d\r\n"), me32.modBaseSize);

            //Also display all the symbols for this module
            if (bInitDBGHelp)
            {
              //display all the global variables
              Log(_T("Variables for this Module\r\n"));
              SymEnumSymbols(hProcess, (ULONG64)me32.modBaseAddr, 0, EnumSymbols, 0);
              Log(_T("\r\n"));
            }
          }
        } 
        while (_ExceptionLoggerData.m_lpfnModule32Next(hToolHelp32Snap, &me32)); 
      } 
      else
        Log(_T(" Failed in call to Module32First, Error:%d\r\n"), GetLastError());

      //close the handle now that we are finished with it
      CloseHandle(hToolHelp32Snap); 
    }
    else
      Log(_T(" Failed in call to CreateToolhelp32Snapshot, Error:%d\r\n"), GetLastError());
  }
  else
    Log(_T("Threads / Modules: ToolHelp32 dll not availble, Cannot enumerate threads or modules\r\n"));

  //Next thing to log is the i86 Registers
  Log(_T("\r\nRegisters:\r\n EAX    %08X\r\n"), pExceptionInfo->ContextRecord->Eax);
  Log(_T(" EBX    %08X\r\n"), pExceptionInfo->ContextRecord->Ebx);
  Log(_T(" ECX    %08X\r\n"), pExceptionInfo->ContextRecord->Ecx);
  Log(_T(" EDX    %08X\r\n"), pExceptionInfo->ContextRecord->Edx);
  Log(_T(" ESI    %08X\r\n"), pExceptionInfo->ContextRecord->Esi);
  Log(_T(" EDI    %08X\r\n"), pExceptionInfo->ContextRecord->Edi);
  Log(_T(" CS:EIP %08X:%08X\r\n"), pExceptionInfo->ContextRecord->SegCs, pExceptionInfo->ContextRecord->Eip);
  Log(_T(" SS:ESP %08X:%08X\r\n"), pExceptionInfo->ContextRecord->SegSs, pExceptionInfo->ContextRecord->Esp);
  Log(_T(" EBP    %08X\r\n"), pExceptionInfo->ContextRecord->SegSs, pExceptionInfo->ContextRecord->Ebp);
  Log(_T(" DS     %08X\r\n"), pExceptionInfo->ContextRecord->SegDs);
  Log(_T(" ES     %08X\r\n"), pExceptionInfo->ContextRecord->SegEs);
  Log(_T(" FS     %08X\r\n"), pExceptionInfo->ContextRecord->SegFs);
  Log(_T(" GS     %08X\r\n"), pExceptionInfo->ContextRecord->SegGs);
  Log(_T(" Flags  %08X\r\n"), pExceptionInfo->ContextRecord->EFlags);

  //Next do the call stack
  Log(_T("\r\nCall Stack:\r\n"));
  LogStack(pExceptionInfo->ContextRecord, hProcess);

  //Cleanup our usage of DBGHELP
  SymCleanup(hProcess);
}

//A Helper function to allow use to do sprintf style logging to the crash log file.
//Please note that the file will be Unicode if you build using UNICODE.
int CExceptionLogger::Log(const TCHAR* format, ...)
{
  //Generate the line
  va_list args;
  va_start(args, format);
  int nLen = _vstprintf(m_szTempLogLine, format, args); //Note that we are using the C runtime here!!
  va_end(args);

  //write the line to disk
  DWORD dwWritten = 0;
  WriteFile(m_hLogFile, m_szTempLogLine, nLen*sizeof(TCHAR), &dwWritten, NULL);
  return nLen;
}

//Coverts a linear address to a module, section, offset, name of section
BOOL CExceptionLogger::GetAddressDetails(void* pAddress, LPTSTR pszModule, DWORD& dwOffset, char* pszSectionName)
{
  //First thing to do is we need to get the base address in memory (i.e. a HINSTANCE) from pAddress
  MEMORY_BASIC_INFORMATION MemInfo;
  if (!VirtualQuery(pAddress, &MemInfo, sizeof(MEMORY_BASIC_INFORMATION)))
    return FALSE;

  //From the HINSTANCE we can get the module name
  HMODULE hModule = (HMODULE) MemInfo.AllocationBase;
  if (!GetModuleFileName(hModule, pszModule, _MAX_PATH))
    return FALSE;

  //Now do our in memory traversal of the PE image to loop across the sections in that image
  PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) hModule;
  PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) (((BYTE*)pDosHeader) + pDosHeader->e_lfanew);
  PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNTHeader);

  //Express the "pAddress" parameter as an "RVA"
  DWORD dwRVAAddress = (DWORD) ((BYTE*)pAddress - (BYTE*)MemInfo.AllocationBase); 

  //Loop across the sections to find the one which contains our faulting address
  for (WORD i=0; i<pNTHeader->FileHeader.NumberOfSections; i++, pSection++)
  {
    DWORD dwSectionStart = pSection->VirtualAddress;
    DWORD dwSectionEnd = dwSectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

    //Is the "pAddress" parameter (expressed as a RVA) inside this section?
    if ((dwRVAAddress >= dwSectionStart) && (dwRVAAddress <= dwSectionEnd))
    {
      dwOffset = dwRVAAddress - dwSectionStart;
      CopyMemory(pszSectionName, pSection->Name, IMAGE_SIZEOF_SHORT_NAME);
      pszSectionName[IMAGE_SIZEOF_SHORT_NAME] = NULL; //Don't forget to NULL terminate the Section name
      return TRUE;
    }
  }

  //Got here, then return failure
  return FALSE;
}

void CExceptionLogger::LogStack(CONTEXT* pContext, HANDLE hProcess)
{
  //Variables required inside the loop
  HANDLE hThread = GetCurrentThread();

  //Initialize the STACKFRAME variable to start walking the stack from
  STACKFRAME sf;
  ZeroMemory(&sf, sizeof(STACKFRAME));
  sf.AddrPC.Offset    = pContext->Eip;
  sf.AddrPC.Mode      = AddrModeFlat;
  sf.AddrStack.Offset = pContext->Esp;
  sf.AddrStack.Mode   = AddrModeFlat;
  sf.AddrFrame.Offset = pContext->Ebp;
  sf.AddrFrame.Mode   = AddrModeFlat;

  //Now let's walk the walk
  BOOL bContinue = TRUE;
  while (bContinue)
  {
    if (StackWalk(IMAGE_FILE_MACHINE_I386, hProcess, hThread, &sf, 
                   pContext, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))
    {
      DWORD dwOffset;
      char pszSectionName[IMAGE_SIZEOF_SHORT_NAME+1];
      if (GetAddressDetails((PVOID)sf.AddrPC.Offset, m_szTempFileName, dwOffset, pszSectionName))
        Log(_T(" 0x%08X [%hs]:0x%08X %s"), sf.AddrPC.Offset, pszSectionName, dwOffset, m_szTempFileName);
      else
      {
        //Fall back to just reporting the linear faulting address, rather than the additional details
        Log(_T(" 0x%08X [UNKNOWN]"), sf.AddrPC.Offset);
      }

      //Get the Symbol Info for the offending address
      SYMBOL_INFO* pSymbolInfo = (SYMBOL_INFO*) m_bySymbolInfo;
      pSymbolInfo->SizeOfStruct = sizeof(m_bySymbolInfo);
      pSymbolInfo->MaxNameLen = 1024;
      DWORD64 SymbolOffset;
      if (SymFromAddr(hProcess, sf.AddrPC.Offset, &SymbolOffset, pSymbolInfo))
        Log(_T(" %hs+%I64X"), pSymbolInfo->Name, SymbolOffset);

      //Get the source line if we can
      IMAGEHLP_LINE LineInfo;
      LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE);
      DWORD dwLineDisplacement;
      if (SymGetLineFromAddr(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &LineInfo))
        Log(_T(" %hs line %u"), LineInfo.FileName, LineInfo.LineNumber);

      //Write out the variables at this stack frame
      Log(_T("\r\n  Variables\r\n"));
      IMAGEHLP_STACK_FRAME imageHelpStackFrame;
      imageHelpStackFrame.InstructionOffset = sf.AddrPC.Offset;
      SymSetContext(hProcess, &imageHelpStackFrame, 0);
      SymEnumSymbols(hProcess, 0, 0, EnumSymbols, &sf);

      //Start the next stack frame log on a new line
      Log(_T("\r\n"));
    }
    else
      bContinue = FALSE;
  }
}

BOOL CExceptionLogger::EnumSymbols(PSYMBOL_INFO pSymbolInfo, ULONG /*SymbolSize*/, PVOID pUserContext)
{
  LogSymbol(pSymbolInfo, (STACKFRAME*) pUserContext);
  Log(_T("\r\n")); //Each symbol gets its own line

  //Continue enumeration
  return TRUE;
}

void CExceptionLogger::LogSymbol(PSYMBOL_INFO pSymbolInfo, STACKFRAME* pStackFrame)
{
  //Display details on the symbols type
  if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER)
    Log(_T("   Parameter: "));
  else if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_FUNCTION)
    Log(_T("   Function:  "));
  else if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL)
    Log(_T("   Local:     "));
  else if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_INFO_CONSTANT)
    Log(_T("   Constant:  "));

  //Display the symbols name
  Log(_T(" %hs"), pSymbolInfo->Name);

  //Return immediately if the symbol is not for data
  if (pSymbolInfo->Tag != 7) //SymTagData is 7 as taken from the DIA SDK Header file cvconst.h
    return;

  //Display the value if valid
  if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_INFO_VALUEPRESENT)
    Log(_T(", Value: %I64d"), pSymbolInfo->Value);

  //Punt on register values
  if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER)
  {
    Log(_T(", Register"));
    return;
  } 

  //Work out the address of the variable
  DWORD dwSymbolAddress = 0;
  if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE)
  {
    Log(_T(", Register Relative"));
    dwSymbolAddress = pStackFrame->AddrFrame.Offset;
    dwSymbolAddress += ((DWORD) pSymbolInfo->Address);
  }
  else 
  {
    if (pSymbolInfo->Flags & IMAGEHLP_SYMBOL_INFO_FRAMERELATIVE)
    {
      Log(_T(", Frame Relative"));
      return;
    }

    dwSymbolAddress = ((DWORD) pSymbolInfo->Address);
  }
  Log(_T(", Address: %08X"), dwSymbolAddress);

  BOOL bHandled = LogUDTSymbolDetails(pSymbolInfo->TypeIndex, pSymbolInfo->ModBase, dwSymbolAddress, 3);
  if (!bHandled)
  {
    //Get the type of the symbol
    DWORD dwType = GetBasicType(pSymbolInfo->TypeIndex, pSymbolInfo->ModBase);

    //Now that we have the type display the actual symbols value
    Log(_T(", "));
    LogBasicSymbolDetails(dwType, pSymbolInfo->Size, (void*)dwSymbolAddress);
  }
}

BOOL CExceptionLogger::LogUDTSymbolDetails(DWORD dwTypeIndex, DWORD64 modBase, DWORD dwOffset, DWORD dwIndent)
{
  __try
  {
    HANDLE hProcess = GetCurrentProcess();

    //First get the symbol name, which will either be the 
    //type name if a UDT or the member name otherwise
    wchar_t* pszTypeName = NULL;
    if (SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_GET_SYMNAME, &pszTypeName))
    {
      Log(_T(" %ls"), pszTypeName);
      LocalFree(pszTypeName);
    }

    //Next we need to find out how many children this symbol has
    DWORD dwChildren = 0;
    SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT, &dwChildren);

    //No children then return immediately
    if (dwChildren == 0)
      return FALSE;

    //Form the identation correctly
    Log(_T("\r\n"));
	DWORD j=0;
    for (j=0; j<dwIndent; j++)
      Log(_T("\t"));

    //Next get all the type Id's for all the children
    struct myTI_FIND_PARAMS : TI_FINDCHILDREN_PARAMS
    {
      ULONG nOtherChildren[1024];
    } myFindParams;
    myFindParams.Count = dwChildren;
    myFindParams.Start = 0;
    if (!SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN, &myFindParams))
      return FALSE;

    //Iterate thro all the children
    for (DWORD i=0; i<dwChildren; i++)
    {
      if (!LogUDTSymbolDetails(myFindParams.ChildId[i], modBase, dwOffset, dwIndent+1))
      {
        //This child item must be a basic type, so log it's info
      
        //First need to get it's offset relative to the parent
        DWORD dwMemberOffset = 0;
        SymGetTypeInfo(hProcess, modBase, myFindParams.ChildId[i], TI_GET_OFFSET, &dwMemberOffset);

        //Get the TypeID of this child
        DWORD dwTypeId = 0;
        SymGetTypeInfo(hProcess, modBase, myFindParams.ChildId[i], TI_GET_TYPEID, &dwTypeId);

        //Get the length of the child
        ULONG64 Length = 0;
        SymGetTypeInfo(hProcess, modBase, dwTypeId, TI_GET_LENGTH, &Length);

        //get the basic type of the child
        DWORD dwBasicType = GetBasicType(myFindParams.ChildId[i], modBase);

        //Now call the helper function to display all the child's details
        DWORD dwSymbolAddress = dwOffset + dwMemberOffset;     
        Log(_T(", "));
        LogBasicSymbolDetails(dwBasicType, Length, (void*)dwSymbolAddress);

        //Each child starts on a new line
        if (i<(dwChildren-1))
        {
          Log(_T("\r\n"));
          for (j=0; j<dwIndent; j++)
            Log(_T("\t"));
        }
      }
    }
  }
  __except(1)
  {
  }

  return TRUE;
}

void CExceptionLogger::LogBasicSymbolDetails(DWORD dwBasicType, DWORD64 dwLength, void* pSymbolAddress)
{
  __try
  {
    switch (dwBasicType)
    {
      case 0: //btNoType is 0 from DIA SDK header file cvconst.h
      {
        Log(_T("No Type"));
        break;
      }
      case 1: //btVoid is 1 from DIA SDK header file cvconst.h
      {
        Log(_T("void, Value: %08X"), *((DWORD*)pSymbolAddress));
        break;
      }
      case 2: //btChar is 2 from DIA SDK header file cvconst.h
      {
        BYTE* pAddr = (BYTE*) pSymbolAddress;
        Log(_T("char, len: %d, value(s): "), dwLength);
        for (DWORD64 i=0; i<dwLength; i++)
          Log(_T("%hc"), pAddr[i]);
        break;
      }
      case 6: //btInt is 6 from DIA SDK header file cvconst.h
      {
        //Handle the special case of a short
        if (dwLength == 2 || dwLength == 6)
        {
          short* pAddr = (short*) pSymbolAddress;
          DWORD64 dwSize = dwLength / sizeof(short);
          Log(_T("short, len: %d, value(s): "), dwSize);
          for (DWORD64 i=0; i<dwSize; i++)
            Log(_T("%d "), pAddr[i]);
        }
        else
        {
          int* pAddr = (int*) pSymbolAddress;
          DWORD64 dwSize = dwLength / sizeof(int);
          Log(_T("int, len: %d, value(s): "), dwSize);
          for (DWORD64 i=0; i<dwSize; i++)
            Log(_T("%d "), pAddr[i]);
        }
        break;
      }
      case 7: //btUInt is 7 from DIA SDK header file cvconst.h
      {
        //Handle the special case of a WORD
        if (dwLength == 2 || dwLength == 6)
        {
          WORD* pAddr = (WORD*) pSymbolAddress;
          DWORD64 dwSize = dwLength / sizeof(WORD);
          Log(_T("WORD, len: %d, value(s): "), dwSize);
          for (DWORD64 i=0; i<dwSize; i++)
            Log(_T("%u "), pAddr[i]);
        }
        else
        {
          unsigned int* pAddr = (unsigned int*) pSymbolAddress;
          DWORD64 dwSize = dwLength / sizeof(unsigned int);
          Log(_T("unsigned int, len: %d, value(s): "), dwSize);
          for (DWORD64 i=0; i<dwSize; i++)
            Log(_T("%u "), pAddr[i]);
        }
        break;
      }
      case 8: //btFloat is 8 from DIA SDK header file cvconst.h
      {
        if (dwLength == 4)
          Log(_T("float, Value: %f"), *((float*)pSymbolAddress));
        else if (dwLength == 8)
          Log(_T("double, Value: %lf"), *((double*)pSymbolAddress));
        else 
        {
          BYTE* pAddr = (BYTE*) pSymbolAddress;
          Log(_T("btFloat, len (bytes): %d, value(s): "), dwLength);
          for (DWORD64 i=0; i<dwLength; i++)
            Log(_T("%02X "), pAddr[i]);
        }
        break;
      }
      case 13: //btLong is 13 from DIA SDK header file cvconst.h
      {
        long* pAddr = (long*) pSymbolAddress;
        DWORD64 dwSize = dwLength / sizeof(long);
        Log(_T("long, len: %d, value(s): "), dwSize);
        for (DWORD64 i=0; i<dwSize; i++)
          Log(_T("%ld "), pAddr[i]);
        break;
      }
      case 14: //btULong is 14 from DIA SDK header file cvconst.h
      {
        unsigned long* pAddr = (unsigned long*) pSymbolAddress;
        DWORD64 dwSize = dwLength / sizeof(unsigned long);
        Log(_T("unsigned long, len: %d, value(s): "), dwSize);
        for (DWORD64 i=0; i<dwSize; i++)
          Log(_T("%lu "), pAddr[i]);
        break;
      }
      default:
      {
        Log(_T("Unknown Type"));
        break;
      }
    }
  }
  __except(1)
  {
    Log(_T("Error displaying symbol"));
  }
}

DWORD CExceptionLogger::GetBasicType(DWORD dwTypeIndex, DWORD64 modBase)
{
  HANDLE hProcess = GetCurrentProcess();

  DWORD dwType;
  if (SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_GET_BASETYPE, &dwType))
    return dwType;

  DWORD dwTypeID;
  if (SymGetTypeInfo(hProcess, modBase, dwTypeIndex, TI_GET_TYPEID, &dwTypeID))
  {
    if (SymGetTypeInfo(hProcess, modBase, dwTypeID, TI_GET_BASETYPE, &dwType) )
      return dwType;
  }

  return 0;
}
