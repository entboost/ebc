#pragma once

using namespace Gdiplus;

class CUtility
{
public:
	static CString GetModulePath(HMODULE hModule = NULL);

	static BOOL IsFileExist(LPCTSTR lpszFilePath);

	static BOOL ExtractResourceToFile( LPCTSTR lpszType
									, UINT nResID
									, LPCTSTR lpszFilename
									, HMODULE hModule
									);

	static Image * LoadImage( UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL);

	static HRGN CreateRegionFromBitmap(Bitmap* bitmap, BYTE alphaValve = 0);

	static HRGN CreateRegion(LPCTSTR lpszFile, BYTE alphaValve = 0);

	static HRGN CreateRegion(UINT nID, LPCTSTR lpszType, HINSTANCE hInstance = NULL, BYTE alphaValve = 0); 
};
