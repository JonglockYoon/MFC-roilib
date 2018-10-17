//////////////////////////////////////////////////////////////////////
//	Implemented by Samuel Gonzalo 
//
//	You may freely use or modify this code 
//////////////////////////////////////////////////////////////////////
//
// Path.h: interface for the CPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__PATH_H__)
#define __PATH_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

const char	sCEmptyString = 0x0;
enum		{PATH_CMDLINE, PATH_MODULE};
enum		{FILE_CREATION, FILE_ACCESS, FILE_WRITE};

class CPath  
{
public:
	CPath();
	CPath(LPCTSTR szPath, BOOL bIsFolderPath = FALSE, BOOL bHasArguments = FALSE);
	CPath(DWORD dwSpecial);
	virtual ~CPath();

	// Parses a path or PATH_CMDLINE, PATH_MODULE
	void	SetPath(LPCTSTR szPath, BOOL bIsFolderPath = FALSE, BOOL bHasArguments = FALSE);
	void	SetPath(DWORD dwSpecial);
	CString	GetPath(BOOL bAppendArgs = FALSE, BOOL bOriginal = FALSE);
	CString	GetShortPath();
	CString	GetLongPath();

	BOOL IsLocalPath();
	BOOL IsRelativePath();
	BOOL IsFilePath();

	BOOL ExistFile();
	BOOL ExistLocation();


	// If the path set in the object is not a relative one returns empty
	CString	GetAbsolutePath(LPCTSTR szBaseFolder);

	// If the path set in the object is a relative one returns empty
	CString	GetRelativePath(LPCTSTR szBaseFolder);


	// Get drive string (empty for a network path) [e.g.: "c:"]
	CString	GetDrive();

	// Get drive label (pc name for a network path) [e.g.: "MAIN_HD"]
	CString	GetDriveLabel(BOOL bPCNameIfNetwork = FALSE);

	// Get folder count in path [e.g.: 2 for "c:\folder\subfolder\file.ext"]
	int		GetDirCount();

	// Get 0 based nIndex folder string [e.g.: "folder" for nIndex = 0]
	// If nIndex = -1 the return string is the full dir string 
	// [e.g.: "\folder\subfolder\" or "\\pcname\folder\" for non-local]
	// If it's a relative path no "\" is added at the beginning [e.g.: "..\sub\"]
	CString	GetDir(int nIndex = -1);

	// File location or directory path [e.g.: "c:\folder\subfolder\"]
	CString	GetLocation();

	// File title string (without extension) [e.g.: "file" for "..\file.ext"]
	CString	GetFileTitle();

	// Filename = File title + extension [e.g.: "file.ext"]
	CString	GetFileName();

	// Extension string (dot included) [e.g.: ".ext"]
	CString	GetExtension();

	// Extension name (dot not included) [e.g.: "ext"]
	CString	GetExtName();

	// Get argument count [e.g.: 2 for <c:\app.exe param1 "param 2">]
	int		GetArgCount();

	// Get 0 based nIndex argument string
	// If nIndex = -1 the return string is the argument part of the path
	// if bGetFlag is true, return the 0 based nIndex argument flag
	CString	GetArgument(int nIndex = -1, BOOL bGetFlag = FALSE);

	// Set the arguments for the current file path
	void	SetArguments(LPCTSTR szArgs);

	// Add or set an argument
	void	AddSetArgument(LPCTSTR szFlag, LPCTSTR szArgument);

	// Remove argument nIndex
	void	RemoveArgument(int nIndex);

	// Return 0 based index of the argument whose flag matches szFlag
	// If it's not found, the return value is -1
	int		FindArgument(LPCTSTR szFlag);


	// Get the size in bytes of the current file
	BOOL	GetFileSize(__int64 &nSize);

	// Get the size in bytes of the current file
	// values: FILE_CREATION, FILE_ACCESS, FILE_WRITE
	BOOL	GetFileTime(CTime &time, DWORD dwType = FILE_WRITE);


	// Return a temporary character pointer to the path data.
	operator LPCTSTR ();

	// Same as SetPath(szPath, FALSE, FALSE)
	const CPath& operator = (LPCTSTR szPath);

	// Makes a copy of the object
	const CPath& operator = (CPath &ref);

	// Add a back slash ('\' or '/' if bInverted is TRUE) if necessary
	static CString AddBackSlash(LPCTSTR szFolderPath, BOOL bInverted = FALSE);

	// Removes a trailing back slash if found
	static CString RemoveBackSlash(LPCTSTR szFolderPath);

	BOOL DeleteDirectoryFile(LPCTSTR RootDir);

private:
	void FillDirArray();
	void FillArgArray();
	BOOL FillFileInfoStruct();

	CString			_sOriginalPath;

	CString			_sDrive;
	CString			_sDriveLabel;
	CStringArray	_aDir;
	CString			_sDir;
	CString			_sFileTitle;
	CString			_sExtName;

	class CArgument
	{
	public:
		CArgument() 
		{ cFlagMark = '/'; }
		virtual ~CArgument() {}

		CString GetString()
		{
			CString sArg;
			if (!sFlag.IsEmpty()) sArg.Format(_T(" %c%s"), cFlagMark, sFlag);
			if (!sValue.IsEmpty())
			{
				if (sValue.Find(' ') != -1)
					sArg += CString(_T(" \"")) + sValue + CString(_T("\""));
				else
					sArg += CString(_T(" ")) + sValue;
			}

			return sArg;
		}

		void SetFlag(CString sFlagValue)
		{
			sFlag = sFlagValue;
			if (sFlag.Remove('/') > 0) cFlagMark = '/';
			if (sFlag.Remove('-') > 0) cFlagMark = '-';
			sFlag.Remove(' ');
		}

		char	cFlagMark;
		CString	sFlag;
		CString	sValue;
	};

	typedef CArray<CArgument, CArgument&> ATArguments;

	CString			_sArgs;
	ATArguments		_aArgs;

	BOOL			_bIsRelative;

	BY_HANDLE_FILE_INFORMATION _fis;

	CString			_sLPCTSTRPath;
};

#endif // !defined(__PATH_H__)
