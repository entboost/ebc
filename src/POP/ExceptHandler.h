/*  
Module : EXCEPTHANDLER.H
Purpose: Definition for a class which intercepts and logs details about unhandled exceptions.
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


#ifndef __EXCEPTHANDLER_H__
#define __EXCEPTHANDLER_H__

////////////////////////////// Includes /////////////////////////////
#ifndef _DBGHELP_
#pragma message("To avoid this message, put dbghelp.h in your PCH")
#include <dbghelp.h>
#endif



////////////////////////////// Classes //////////////////////////////

class CExceptionLogger
{
public:
//Constructors / Destructors
  CExceptionLogger();
  ~CExceptionLogger();

protected:
//Methods
  static long  WINAPI UnHandledExceptionFilter(_EXCEPTION_POINTERS* pExceptionInfo);
  static void  GenerateCrashLog(_EXCEPTION_POINTERS* pExceptionInfo);
  static int   Log(const TCHAR* format, ...); 
  static BOOL  GetAddressDetails(void* pAddress, LPTSTR pszModule, DWORD& dwOffset, char* pszSectionName);
  static void  LogStack(CONTEXT* pContext, HANDLE hProcess);
  static BOOL  CALLBACK EnumSymbols(PSYMBOL_INFO pSymbolInfo, ULONG SymbolSize, PVOID pUserContext);
  static void  LogSymbol(PSYMBOL_INFO pSymbolInfo, STACKFRAME* pStackFrame);
  static void  LogBasicSymbolDetails(DWORD dwBasicType, DWORD64 dwLength, void* pSymbolAddress);
  static BOOL  LogUDTSymbolDetails(DWORD dwTypeIndex, DWORD64 modBase, DWORD dwOffset, DWORD dwIndent);
  static DWORD GetBasicType(DWORD dwTypeIndex, DWORD64 modBase);

//Data
  static TCHAR                        m_pszLogFilename[_MAX_PATH];                                               //The path to the log file we will use
  static LPTOP_LEVEL_EXCEPTION_FILTER m_pPreviousFilter;                                                         //The previous exception filter which we chain to
  static HANDLE                       m_hLogFile;                                                                //The open handle of the log file
  static TCHAR                        m_szTempLogLine[4096];                                                     //The static buffer we use for writing each log line
  static TCHAR                        m_szTempFileName[_MAX_PATH];                                               //The static buffer we use for any paths which need to be written out
  static BYTE                         m_bySymbolInfo[sizeof(SYMBOL_INFO) + 1024];                                //The static buffer we use for storing symbol info in
};

#endif //__EXCEPTHANDLER_H__