//////////////////////////////////////////////////////////////////////
//	Implemented by Samuel Gonzalo
//
//	You may freely use or modify this code
//////////////////////////////////////////////////////////////////////
//
// Path.cpp: implementation of the CPath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Path.h"
#include "Shlwapi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPath::CPath()
{
    _bIsRelative = FALSE;
}

CPath::CPath(LPCTSTR szPath, BOOL bIsFolderPath, BOOL bHasArguments)
{
    SetPath(szPath, bIsFolderPath, bHasArguments);
}

CPath::CPath(DWORD dwSpecial)
{
    SetPath(dwSpecial);
}

CPath::~CPath()
{

}

void CPath::SetPath(DWORD dwSpecial)
{
    switch (dwSpecial)
    {
    case PATH_CMDLINE:
        SetPath(GetCommandLine(), FALSE, TRUE);
        break;

    case PATH_MODULE:
        {
            TCHAR szPath[_MAX_PATH];
            GetModuleFileName(0, szPath, _MAX_PATH);
            SetPath(szPath);
        }
        break;
    }
}

void CPath::SetPath(LPCTSTR szPath, BOOL bIsFolderPath, BOOL bHasArguments)
{
    TCHAR szParamPath[_MAX_PATH];
    TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
    TCHAR szName[_MAX_FNAME], szExt[_MAX_EXT];

    // Reset
    _sOriginalPath.Empty();
    _sDriveLabel.Empty();
    _bIsRelative = FALSE;
    _aDir.RemoveAll();
    _sExtName.Empty();
    _aArgs.RemoveAll();

    // Original path
    _sOriginalPath = szPath;

    // Get args and remove them from path
    szParamPath[0] = 0x0;
    _tcscpy(szParamPath, szPath);

    if (bHasArguments)
    {
        _sArgs = PathGetArgs(szParamPath);
        PathRemoveArgs(szParamPath);
    }

    PathUnquoteSpaces(szParamPath);
    if (szParamPath[0] == 0x0) return;

    _tsplitpath(szParamPath, szDrive, szDir, szName, szExt);

    // Drive
    _sDrive = szDrive;

    // Directory
    _sDir = szDir;
    _sDir.Replace('/', '\\');
    if (!_sDir.IsEmpty()) _bIsRelative = (_sDir[0] != '\\');

    // FileTitle
    if (bIsFolderPath)
    {
        _sDir = CPath::AddBackSlash(_sDir);
        _sDir += szName;
        _sDir = CPath::AddBackSlash(_sDir);
    }
    else
    {
        _sFileTitle = szName;
    }

    // Get extension name (e.g.: "txt")
//	if (IsFilePath()) // Jerry_050809
    {
        _sExtName = szExt;
        _sExtName.Remove('.');
    }
}

BOOL CPath::IsLocalPath()
{
    return !_sDrive.IsEmpty() && !_bIsRelative;
}

BOOL CPath::IsRelativePath()
{
    return _bIsRelative;
}

BOOL CPath::IsFilePath()
{
    return !_sFileTitle.IsEmpty();
}

BOOL CPath::ExistFile()
{
    if (!IsFilePath()) return FALSE;

    return PathFileExists(GetPath());
}

BOOL CPath::ExistLocation()
{
    return PathFileExists(GetLocation());
}

CString CPath::GetAbsolutePath(LPCTSTR szBaseFolder)
{
    if (!IsRelativePath()) return sCEmptyString;

    TCHAR	szAbsolutePath[_MAX_PATH];
    CString sFullPath(szBaseFolder);

    if (sFullPath.IsEmpty()) return sCEmptyString;

    sFullPath = CPath::AddBackSlash(sFullPath);
    sFullPath += GetPath();

    if (!PathCanonicalize(szAbsolutePath, sFullPath)) return sCEmptyString;

    return szAbsolutePath;
}

CString CPath::GetRelativePath(LPCTSTR szBaseFolder)
{
    if (IsRelativePath()) return sCEmptyString;

    TCHAR	szRelativePath[_MAX_PATH];
    CString	sRelPath;

    PathRelativePathTo(szRelativePath, szBaseFolder, FILE_ATTRIBUTE_DIRECTORY,
                    GetPath(), IsFilePath() ? 0 : FILE_ATTRIBUTE_DIRECTORY);

    sRelPath = szRelativePath;
    if (sRelPath.GetLength() > 1)
    {
        // Remove ".\" from the beginning
        if ((sRelPath[0] == '.') && (sRelPath[1] == '\\'))
            sRelPath.Right(sRelPath.GetLength() - 2);
    }

    return sRelPath;
}

CString CPath::GetPath(BOOL bAppendArgs, BOOL bOriginal)
{
    CString sPath;

    if (bOriginal)
        sPath = _sOriginalPath;
    else
        sPath = GetLocation() + GetFileName();

    if (bAppendArgs) sPath += GetArgument();

    return sPath;
}

CString	CPath::GetShortPath()
{
    TCHAR szShortPath[_MAX_PATH];
    szShortPath[0] = 0x0;

    GetShortPathName(GetPath(), szShortPath, _MAX_PATH);

    return szShortPath;
}

CString	CPath::GetLongPath()
{
    TCHAR szLongPath[_MAX_PATH];
    szLongPath[0] = 0x0;

    GetLongPathName(GetPath(), szLongPath, _MAX_PATH);

    return szLongPath;
}

CString CPath::GetDrive()
{
    return _sDrive;
}

CString	CPath::GetDriveLabel(BOOL bPCNameIfNetwork)
{
    if (_sDriveLabel.IsEmpty() && !IsRelativePath())
    {
        if ((bPCNameIfNetwork) && (!IsLocalPath()))
            _sDriveLabel = GetDir(0);
        else
        {
            CString sRoot;
            TCHAR	szVolumeName[256];

            szVolumeName[0] = '\0';
            if (IsLocalPath())
            {
                sRoot = _sDrive + CString(_T("\\"));
            }
            else if (GetDirCount() > 1)
            {
                sRoot.Format(_T("\\\\%s\\%s\\"), GetDir(0), GetDir(1));
            }

            GetVolumeInformation(sRoot, szVolumeName, 255, NULL, NULL, NULL, NULL, 0);

            _sDriveLabel = szVolumeName;
        }
    }

    return _sDriveLabel;
}

int	CPath::GetDirCount()
{
    FillDirArray();
    return _aDir.GetSize();
}

CString CPath::GetDir(int nIndex)
{
    if (nIndex < 0)
        return _sDir;
    else if (nIndex < GetDirCount())
    {
        FillDirArray();
        return _aDir[nIndex];
    }

    return sCEmptyString;
}

CString	CPath::GetLocation()
{
    return _sDrive + GetDir();
}

CString CPath::GetFileTitle()
{
    return _sFileTitle;
}

CString CPath::GetFileName()
{
    return _sFileTitle + GetExtension();
}

CString CPath::GetExtension()
{
    if (_sExtName.IsEmpty()) return sCEmptyString;

    return CString(_T(".")) + _sExtName;
}

CString CPath::GetExtName()
{
    return _sExtName;
}

int CPath::GetArgCount()
{
    FillArgArray();
    return _aArgs.GetSize();
}

CString CPath::GetArgument(int nIndex, BOOL bGetFlag)
{
    if (nIndex < 0)
    {
        if (_sArgs.IsEmpty())
        {
            for (int nItem = 0; nItem < _aArgs.GetSize(); nItem++)
                _sArgs += _aArgs[nItem].GetString();

            _sArgs.TrimLeft();
        }

        return _sArgs;
    }
    else if (nIndex < GetArgCount())
    {
        FillArgArray();

        if (bGetFlag)
            return _aArgs[nIndex].sFlag;
        else
            return _aArgs[nIndex].sValue;
    }

    return sCEmptyString;
}

void CPath::SetArguments(LPCTSTR szArgs)
{
    _aArgs.RemoveAll();
    _sArgs = szArgs;
}

void CPath::AddSetArgument(LPCTSTR szFlag, LPCTSTR szArgument)
{
    int	nIndex;

    nIndex = FindArgument(szFlag);

    if (nIndex != -1)
    {
        // An argument with the same flag already exists. Update it!
        _aArgs[nIndex].sValue = szArgument;
    }
    else
    {
        CArgument arg;

        arg.sValue = szArgument;
        arg.SetFlag(szFlag);

        _aArgs.Add(arg);
        _sArgs.Empty();
    }
}

void CPath::RemoveArgument(int nIndex)
{
    if ((nIndex >= 0) && (nIndex < _aArgs.GetSize()))
    {
        _aArgs.RemoveAt(nIndex);
        _sArgs.Empty();
    }
}

int CPath::FindArgument(LPCTSTR szFlag)
{
    bool		bFound = false;
    int			nIndex;
    CArgument	arg;

    FillArgArray();

    arg.SetFlag(szFlag);

    if (!arg.sFlag.IsEmpty())
    {
        for (nIndex = 0; nIndex < _aArgs.GetSize(); nIndex++)
        {
            bFound = (_aArgs[nIndex].sFlag.CompareNoCase(arg.sFlag) == 0);
            if (bFound) break;
        }
    }

    return (bFound ? nIndex : -1);
}

BOOL CPath::GetFileSize(__int64 &nSize)
{
    BOOL bResult;

    bResult = FillFileInfoStruct();
    nSize = ((__int64)_fis.nFileSizeHigh * (__int64)MAXDWORD) + (__int64)_fis.nFileSizeLow;
    return bResult;
}

BOOL CPath::GetFileTime(CTime &time, DWORD dwType)
{
    BOOL bResult;
    FILETIME *pTime = NULL;

    bResult = FillFileInfoStruct();
    switch (dwType)
    {
    case FILE_CREATION:	pTime = &_fis.ftCreationTime;	break;
    case FILE_WRITE:	pTime = &_fis.ftLastWriteTime;	break;
    case FILE_ACCESS:
    default:			pTime = &_fis.ftLastAccessTime;	break;
    }

    if (pTime != NULL) time = CTime(*pTime);
    return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Private methods

// This function must be called whenever _aDir array is needed, since this
// method is the one which parses _sDir and fill _aDir
void CPath::FillDirArray()
{
    if (_sDir.IsEmpty() || (_aDir.GetSize() > 0)) return;

    int nFrom, nTo;

    // nFrom: 0 - relative / 1 - local / 2 - network
    nFrom = IsLocalPath() ? 1 : (IsRelativePath() ? 0 : 2);

    while ((nTo = _sDir.Find('\\', nFrom)) != -1)
    {
        _aDir.Add(_sDir.Mid(nFrom, nTo - nFrom));
        nFrom = nTo + 1;
    }
}

// This function must be called whenever _aArgs array is needed, since this
// method is the one which parses _sArgs and fill _aArgs
void CPath::FillArgArray()
{
    if (_sArgs.IsEmpty() || (_aArgs.GetSize() > 0)) return;

    CString		sArgs(_sArgs);
    int			nFrom, nTo;
    bool		bQuotedArg = false;
    CArgument	arg;

    sArgs.TrimLeft();
    sArgs.TrimRight();

    // add a blank space at the end of the string to include the last argument
    sArgs += " ";
    while ((nTo = sArgs.FindOneOf(_T(" \""))) != -1)
    {
        bQuotedArg = (sArgs[nTo] == '\"');

        if (bQuotedArg)
        {
            nFrom = nTo + 1;
            if ((nTo = sArgs.Find('\"', nFrom)) == -1) break;

            arg.sValue = sArgs.Mid(nFrom, nTo - nFrom);
        }
        else
        {
            CString *pStr;

            if ((sArgs[0] == '/') || (sArgs[0] == '-'))
            {
                if (!arg.sFlag.IsEmpty())
                {
                    arg.SetFlag(arg.sFlag);
                    _aArgs.Add(arg);
                    arg.sFlag.Empty();
                    arg.sValue.Empty();
                }

                pStr = &arg.sFlag;
            }
            else
                pStr = &arg.sValue;

            *pStr = sArgs.Left(nTo);
        }

        if (!arg.sValue.IsEmpty())
        {
            arg.SetFlag(arg.sFlag);
            _aArgs.Add(arg);
            arg.sFlag.Empty();
            arg.sValue.Empty();
        }

        sArgs = sArgs.Right(sArgs.GetLength() - nTo - 1);
        sArgs.TrimLeft();
    }

    // if the last argument is only a flag it hasn't been added yet
    if (!arg.sFlag.IsEmpty())
    {
        arg.SetFlag(arg.sFlag);
        _aArgs.Add(arg);
    }
}

BOOL CPath::FillFileInfoStruct()
{
    HANDLE	hFile;
    BOOL	bResult;

    ::memset(&_fis, 0, sizeof(_fis));

    hFile = CreateFile(GetPath(), GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN |
                        FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM, NULL);

    if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    bResult = GetFileInformationByHandle(hFile, &_fis);

    CloseHandle(hFile);

    return bResult;
}

CString CPath::AddBackSlash(LPCTSTR szFolderPath, BOOL bInverted)
{
    CString	sResult(szFolderPath);
    int		nLastChar = sResult.GetLength() - 1;

    if (nLastChar >= 0)
    {
        if ((sResult[nLastChar] != '\\') && (sResult[nLastChar] != '/'))
            sResult += bInverted ? '/' : '\\';
    }

    return sResult;
}

CString CPath::RemoveBackSlash(LPCTSTR szFolderPath)
{
    CString	sResult(szFolderPath);
    int		nLastChar = sResult.GetLength() - 1;

    if (nLastChar >= 0)
    {
        if ((sResult[nLastChar] == '\\') || (sResult[nLastChar] == '/'))
            sResult = sResult.Left(nLastChar);
    }

    return sResult;
}

CPath::operator LPCTSTR ()
{
    _sLPCTSTRPath = GetPath();
    return _sLPCTSTRPath;
}

const CPath& CPath::operator = (LPCTSTR szPath)
{
    SetPath(szPath);
    return *this;
}

const CPath& CPath::operator = (CPath &ref)
{
    _aArgs.RemoveAll();
    _aArgs.Copy(ref._aArgs);
    _aDir.RemoveAll();
    _aDir.Copy(ref._aDir);

    _bIsRelative = ref._bIsRelative;

    _fis = ref._fis;

    _sArgs = ref._sArgs;
    _sDir = ref._sDir;
    _sDrive = ref._sDrive;
    _sDriveLabel = ref._sDriveLabel;
    _sExtName = ref._sExtName;
    _sFileTitle = ref._sFileTitle;
    _sOriginalPath = ref._sOriginalPath;
    return *this;
}

BOOL CPath::DeleteDirectoryFile(LPCTSTR RootDir)
{
    if( RootDir == NULL )
    {
        return FALSE;
    }
    BOOL bRval = FALSE;
    int nRval = 0;
    CString szNextDirPath   = _T("");
    CString szRoot = _T("");

    // 해당 디렉토리의 모든 파일을 검사한다.
    szRoot.Format(_T("%s\\*.*"), RootDir);
    CFileFind find;
    bRval = find.FindFile( szRoot );
    if( bRval == FALSE )
    {
        return bRval;
    }
    while( bRval )
    {
        bRval = find.FindNextFile();
        // . or .. 인 경우 무시 한다.
        if( find.IsDots() == FALSE )
        {
            // Directory 일 경우 재귀호출 한다.
            if( find.IsDirectory() )
            {
                DeleteDirectoryFile(find.GetFilePath());
            }
            // file일 경우 삭제
            else
            {
                bRval = DeleteFile(find.GetFilePath());
            }
        }
    }
    find.Close();

    bRval = RemoveDirectory( RootDir );
    return bRval;
}
