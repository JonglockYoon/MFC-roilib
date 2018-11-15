#include "stdafx.h"
#include "Utils.h"
#include "rpcdce.h"

#include <io.h>
#include < direct.h >
#include < stdio.h >
#include < errno.h >
#include < conio.h >
#include < math.h >

//#include "dlportio.h"
#pragma comment(lib, "rpcrt4")

#define USE_DLPortIO

// WaitForUserMultipleObjects()는 MAXIMUM_WAIT_OBJECTS(64)보다 크면 안되므로.
// 64개가 넘으면 분할해서 처리한다.
DWORD WaitForUserMultipleObjects(DWORD nCount, CONST HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds)
{
    DWORD nRet = 0;
    if (nCount < MAXIMUM_WAIT_OBJECTS)
        return ::WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);

    int nCnt = MAXIMUM_WAIT_OBJECTS;
    int nStart = 0;

    while (1)
    {
        nRet |= ::WaitForMultipleObjects(nCnt, &lpHandles[nStart], bWaitAll, dwMilliseconds);
        if (nCnt < MAXIMUM_WAIT_OBJECTS)
            break;
        nStart += MAXIMUM_WAIT_OBJECTS;
        if ((nStart+MAXIMUM_WAIT_OBJECTS) > nCount)
            nCnt = nCount - nStart;
    }
    return nRet;
}

CStringList * GetList_Dir( CString SourceDir, CString Exe)
{
    CStringList *StringList;
    CString  SearchExe;
    CString  Path;
    WIN32_FIND_DATA  SearchRec;
    HANDLE         nFound;
    BOOL bFound;

    Path = SourceDir + Exe;
    StringList = new CStringList;

    StringList->RemoveAll();
    if(!DirectoryExists(SourceDir)) return StringList;

    nFound = FindFirstFile( Path, &SearchRec);
    if( nFound != INVALID_HANDLE_VALUE)
    {
        do
        {
            if( _tcscmp(SearchRec.cFileName,_T("."))!=0  && _tcscmp(SearchRec.cFileName,_T("..")) !=0)
            {
                //파일만
                if( SearchRec.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
                {
                    StringList->AddTail( SearchRec.cFileName);
                }
            }

            bFound = FindNextFile(nFound, &SearchRec);
        }while( bFound);

        FindClose(nFound);
    }
    return StringList;
}

//파일 리스트 얻기.
CStringList * GetList_File( CString SourceDir, CString Exe,CString Filter)
{
    CStringList *StringList;
    CString  SearchExe,sTemp;
    CString  Path;
    WIN32_FIND_DATA  SearchRec;
    HANDLE         nFound;
    BOOL bFound;

    Path = SourceDir + Exe;
    StringList = new CStringList;
    Filter.MakeUpper();
    StringList->RemoveAll();
    if(!DirectoryExists(SourceDir)) return StringList;

    nFound = FindFirstFile( Path, &SearchRec);
    if( nFound != INVALID_HANDLE_VALUE)
    {
        do
        {
            if( _tcscmp(SearchRec.cFileName,_T("."))!=0  && _tcscmp(SearchRec.cFileName,_T("..")) !=0)
            {

                if( SearchRec.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
                {
                    sTemp = SearchRec.cFileName;
                    sTemp.MakeUpper();
                    if(sTemp.Find(Filter) >= 0)
                    {
                        StringList->AddTail( SearchRec.cFileName);
                    }
                }
            }
            bFound = FindNextFile(nFound, &SearchRec);
        }while( bFound);
        FindClose(nFound);
    }


    return StringList;
}
//---------------------------------------------------------------------------
CStringList * GetList_File( CString SourceDir, CString Exe,CString Filter1,CString Filter2)
{
    CStringList *StringList;
    CString  SearchExe,sTemp;
    CString  Path;
    WIN32_FIND_DATA  SearchRec;
    HANDLE         nFound;
    BOOL bFound;

    Path = SourceDir + Exe;
    StringList = new CStringList;
    Filter1.MakeUpper();
    Filter2.MakeUpper();
    StringList->RemoveAll();
    if(!DirectoryExists(SourceDir)) return StringList;

    nFound = FindFirstFile( Path, &SearchRec);
    if( nFound != INVALID_HANDLE_VALUE)
    {
        do
        {
            if( _tcscmp(SearchRec.cFileName,_T("."))!=0  && _tcscmp(SearchRec.cFileName,_T("..")) !=0)
            {
                if( SearchRec.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
                {
                    sTemp = SearchRec.cFileName;
                    sTemp.MakeUpper();
                    if(sTemp.Find(Filter1) >= 0 || sTemp.Find(Filter2)>=0)
                    {
                        StringList->AddTail( SearchRec.cFileName);
                    }
                }

            }
            bFound = FindNextFile(nFound, &SearchRec);
        }while( bFound);
        FindClose(nFound);
    }

    return StringList;
}

//날짜 문자열을 구한다.
CString GetDateString(void)
{
    CTime NowTime = CTime::GetCurrentTime();
    CString sDate;

    sDate.Format(_T("%04d%02d%02d"),NowTime.GetYear(),NowTime.GetMonth(),NowTime.GetDay());

    return sDate;
}
//---------------------------------------------------------------------------

//날짜 문자열을 구한다.
CString GetDateString(CString cStr)
{
    CTime NowTime = CTime::GetCurrentTime();
    CString sDate;

    sDate.Format(_T("%04d%s%02d%s%02d"),NowTime.GetYear(),cStr,NowTime.GetMonth(),cStr,NowTime.GetDay());

    return sDate;
}
//---------------------------------------------------------------------------

//년월일 중 하나를 구한다.
CString GetDateSingle(CString index){//index="Y""M""D"

    CTime t_cur_time;
    CString t_year;
    CString t_month;
    CString t_day;

    t_cur_time = CTime::GetCurrentTime();
    t_year.Format(_T("%04d"), t_cur_time.GetYear());
    t_month.Format(_T("%02d"), t_cur_time.GetMonth());
    t_day.Format(_T("%02d"), t_cur_time.GetDay());

    CString str=_T("");

    if(index.Find(_T("Y"),0)!= -1){
        str+=t_year;
    }
    if(index.Find(_T("M"),0)!= -1){
        str+=t_month;
    }
    if(index.Find(_T("D"),0)!= -1){
        str+=t_day;
    }

    return str;
}
//---------------------------------------------------------------------------

//시간 문자열을 구한다.
CString GetTimeString(void)
{
    CTime NowTime = CTime::GetCurrentTime();
    CString sTime;

    sTime.Format(_T("%02d%02d%02d"),NowTime.GetHour(),NowTime.GetMinute(),NowTime.GetSecond());

    return sTime;
}
//---------------------------------------------------------------------------
//시간 문자열을 구한다.
CString GetTimeString(CString cStr)
{
    CTime NowTime = CTime::GetCurrentTime();
    CString sTime;

    sTime.Format(_T("%02d%s%02d%s%02d"),NowTime.GetHour(),cStr,NowTime.GetMinute(),cStr,NowTime.GetSecond());

    return sTime;
}

//---------------------------------------------------------------------------

//디렉토리 또는 파일 삭제하기.
int Delete_DirOrFile(CString sFile, HWND hWnd)
{
    TCHAR File[1024];
    int nRet;
    SHFILEOPSTRUCT op ={0};

    memset(File,'\0',sizeof(File));
    _tcscpy(File,(LPCTSTR)sFile);
    File[sFile.GetLength()+1] = '\0';

    //Dir 삭제.
    op.hwnd =  hWnd;
    op.wFunc = FO_DELETE;
    op.pFrom = File;
    op.fAnyOperationsAborted = false;
    op.hNameMappings = NULL;
    op.lpszProgressTitle = NULL;
    op.fFlags = FOF_NOERRORUI |FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;
    nRet = SHFileOperation(&op);

    return nRet;
}
//---------------------------------------------------------------------------
void DirectroyCopy(CString SrcFolder, CString DesFolder, HWND hWnd)
{
    SHFILEOPSTRUCT FileOp = {0};
    TCHAR szTemp[MAX_PATH];

    _tcscpy(szTemp, SrcFolder);
    szTemp[SrcFolder.GetLength() + 1] = NULL;	// NULL문자가 두개 들어가야 한다.

    FileOp.hwnd = hWnd;
    FileOp.wFunc = FO_COPY;
    FileOp.pFrom = szTemp;
    FileOp.pTo = LPCTSTR(DesFolder);
    FileOp.fFlags = FOF_MULTIDESTFILES |FOF_NOCONFIRMATION;  // 확인메시지가 안뜨도록 설정.
    FileOp.fAnyOperationsAborted = false;
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = NULL;

    SHFileOperation(&FileOp);
}
//---------------------------------------------------------------------------
void DirectroyCopy(TCHAR *SrcFolder, TCHAR *DesFolder, HWND hWnd)
{
    SHFILEOPSTRUCT FileOp = {0};
    TCHAR szTemp[MAX_PATH];

    _tcscpy(szTemp, SrcFolder);
    szTemp[_tcslen(SrcFolder) + 1] = NULL;	// NULL문자가 두개 들어가야 한다.

    FileOp.hwnd = hWnd;
    FileOp.wFunc = FO_COPY;
    FileOp.pFrom = szTemp;
    FileOp.pTo = DesFolder;
    FileOp.fFlags = FOF_NOCONFIRMMKDIR |FOF_MULTIDESTFILES |FOF_NOCONFIRMATION;  // 확인메시지가 안뜨도록 설정.
    FileOp.fAnyOperationsAborted = false;
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = NULL;

    SHFileOperation(&FileOp);
}


//////////////////////////////////////////////////////////////////////////
/**
@brief checkPath

@details
Path 존재 유무 검사 및 생성

@param[in] strAbsFullPath 검사를 하고자 하는 전체경로 (파일명 포함)
@param[in] bCreate 존재하지 않을 경우 폴더 생성 여부

@return
0: 주어진 경로가 존재
-1: 주어진 경로가 존재하지 않음 (bCreate == false 일경우)
-2: 주어진 경로 생성 시도 실패.
*/
int checkPath(const CString& strAbsFullPath, bool bCreate)
{
    TCHAR Drive[_MAX_DRIVE];
    TCHAR Path[_MAX_DIR];
    TCHAR Filename[_MAX_FNAME];
    TCHAR Ext[_MAX_EXT];
    _tsplitpath_s(strAbsFullPath
        , Drive, _MAX_DRIVE
        , Path, _MAX_DIR
        , Filename, _MAX_FNAME
        , Ext, _MAX_EXT
        );

    TCHAR szFullPath[_MAX_PATH];
    _stprintf_s(szFullPath, _MAX_PATH, _T("%s%s\0"), Drive, Path);

    // Check Directory
    if (_taccess(szFullPath, 0) == 0) // If Exists
    {
        return 0;
    }

    if (bCreate)
    {
        TCHAR* pPath = szFullPath;
        TCHAR* pCur = szFullPath;
        while (true)
        {
            if (*pCur == _T('\\') || *pCur == _T('/') || *pCur == _T('\0'))
            {
                TCHAR ch = *pCur;
                *pCur = _T('\0');

                // Check Directory
                if (_taccess(pPath, 0) != 0) // If Not Exists
                {
                    if (::CreateDirectory(pPath, NULL) == FALSE)
                    {
                        return -2; // Fail to create Directory
                    }
                }

                *pCur = ch;

                if (*pCur == _T('\0'))
                {
                    break;
                }
            }
            ++pCur;
        }
    }
    else
    {
        return -1; // Directory is Not Exist
    }

    return 0;
}

//---------------------------------------------------------------------------

//파일이나 디렉토리가 존재하는가.
bool ExistDirOrFile( CString sDir,CString sSearchStr)
{
    CString  SearchExe;
    CString  Path;
    WIN32_FIND_DATA  SearchRec;
    HANDLE         nFound;
    int bFound;
    bool bRet = false;

    Path = sDir + _T("*.*");
    nFound = ::FindFirstFile( Path, &SearchRec);
    if( nFound != INVALID_HANDLE_VALUE)
    {
        do
        {
            if( _tcscmp(SearchRec.cFileName,_T("."))!=0  && _tcscmp(SearchRec.cFileName,_T("..")) !=0)
            {

                if( SearchRec.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
                {
                    if(sSearchStr.CompareNoCase(SearchRec.cFileName) == 0)
                    {
                        bRet = true;
                        break;
                    }
                }
                else if( SearchRec.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY )
                {
                    if(sSearchStr.CompareNoCase(SearchRec.cFileName) == 0)
                    {
                        bRet = true;
                        break;
                    }
                }
            }

            bFound = FindNextFile(nFound, &SearchRec);
        }while( bFound);
        FindClose(nFound);
    }

    return bRet;
}

bool FileExists( CString sFile)
{
    return (_taccess((LPTSTR)(LPCTSTR)sFile,0) == 0 );
}
//---------------------------------------------------------------------------

//문자열 Trim하기.
CString GetAllTrim(CString sString)
{
    CString sTemp;

    sString.TrimLeft();
    sString.TrimRight();

    return sString;
}
//---------------------------------------------------------------------------

//
CString Encord_String(CString sString)
{
    CString sTemp;
    int nCount;
    TCHAR cTemp[256];

    nCount = sString.GetLength();
    sTemp = _T("");
    _tcscpy(cTemp,(LPCTSTR)sString);
    for( int i =0; i < nCount; i++)
    {
        // PASSWORD 변환 식.
        sTemp = sTemp + char(cTemp[i] + 'Z');
    }
    return sTemp;
}
//---------------------------------------------------------------------------

CString Decord_String(CString sString)
{

    CString sTemp;
    int nCount;
    TCHAR cTemp[256];

    nCount = sString.GetLength();
    sTemp = _T("");
    _tcscpy(cTemp,(LPCTSTR)sString);
    for( int i =0; i < nCount; i++)
    {
        // PASSWORD 변환 식.
        sTemp = sTemp + char(cTemp[i] - 'Z');
    }
    return sTemp;
}

//---------------------------------------------------------------------------
void GetList_String( CString String,CString Filter,CStringList *slReturn)
{
   CString  sTemp,sToken, sResult,sToken1;

   slReturn->RemoveAll();
   sTemp = String;

   while(true)
   {
       while( sTemp.GetLength() && sTemp[0] == Filter[0])
       {
           sTemp.Delete( 0, 1);
           slReturn->AddTail(sToken);
           sToken = _T("");
       }

       if( sTemp.GetLength() == 0)
       {
           if(sToken.GetLength() >0)
            slReturn->AddTail(sToken);
           break;
       }

       sToken = _T("");
       while( sTemp.GetLength() && sTemp[0] != Filter[0])
       {
           if(sTemp[0] != '\n' && sTemp[0] != '\r')
           sToken += sTemp[0];
           sTemp.Delete( 0, 1);
       }
   }
}
//---------------------------------------------------------------------------
void GetList_String( CString String,CString Filter,CString Skip1,CString Skip2,CStringList *slReturn)
{
   CString  sTemp,sToken, sResult,sToken1;

   slReturn->RemoveAll();
   sTemp = String;
   while(true)
   {
       while( sTemp.GetLength() && sTemp[0] == Filter[0])
       {
           sTemp.Delete( 0, 1);
           slReturn->AddTail(sToken);
           sToken = _T("");
       }

       if( sTemp.GetLength() == 0)
       {
           if(sToken.GetLength() >0)
            slReturn->AddTail(sToken);
           break;
       }

       sToken = _T("");
       while( sTemp.GetLength() && sTemp[0] != Filter[0])
       {
            if(sTemp[0] != '\n' && sTemp[0] != '\r' && sTemp[0] != Skip1[0] && sTemp[0] != Skip2[0])
            sToken += sTemp[0];
            sTemp.Delete( 0, 1);
       }
   }
}
//---------------------------------------------------------------------------
void GetList_String( CString String,CString Filter,CString Filter2 ,CString Skip1, CString Skip2,CStringList *slReturn)
{
    CString  sTemp,sToken, sResult,sToken1;

    slReturn->RemoveAll();
    sTemp = String;
    int nTemp =0;
    while(true)
    {
        while( sTemp.GetLength() && (sTemp[0] == Filter[0] || sTemp[0] == Filter2[0]))
        {
            sTemp.Delete( 0, 1);
            slReturn->AddTail(sToken);
            sToken = _T("");
            nTemp ++;
        }

        if( sTemp.GetLength() == 0)
        {
            if(sToken.GetLength() >0)
            {
                slReturn->AddTail(sToken);
            }
            break;
        }

        sToken = _T("");
        while( sTemp.GetLength() && sTemp[0] != Filter[0] && sTemp[0] !=Filter2[0])
        {
            if(sTemp[0] != '\n' && sTemp[0] != '\r' && sTemp[0] != Skip1[0])
            {
                sToken += sTemp[0];
            }
            sTemp.Delete( 0, 1);
        }
    }
}
//---------------------------------------------------------------------------
int Convert_StrHexaToInt(CString String, int nPort)
{
    int     res, temp;

    res = 0;
    for( int i = 0; i < String.GetLength(); i ++)
    {
        if( ('0' <= String[i]) && (String[i] <= '9'))
        {
            temp = String[i] - '0';
        }
        else if( ('A' <= String[i]) && (String[i] <= 'F'))
        {
            temp = String[i] - 'A' + 10;
        }
        else if( ('a' <= String[i]) && (String[i] <= 'f'))
        {
            temp = String[i] - 'a' + 10;
        }
        else if( res)
        {
            return res;
        }
        else
            continue;

        res = 16 * res + temp;
    }
    return res *nPort;
}
//---------------------------------------------------------------------------
int Line_Convert_StrHexaToInt(char *String)
{
    int     res, temp;

    res = 0;
    for( int i = 0; i < 2; i ++)
    {
        if( ('0' <= String[i]) && (String[i] <= '9'))
        {
            temp = String[i] - '0';
        }
        else if( ('A' <= String[i]) && (String[i] <= 'F'))
        {
            temp = String[i] - 'A' + 10;
        }
        else if( ('a' <= String[i]) && (String[i] <= 'f'))
        {
            temp = String[i] - 'a' + 10;
        }
        else if( res)
        {
            return res;
        }
        else
            continue;

        res = 16 * res + temp;
    }
    return res;
}
//---------------------------------------------------------------------------
CString Convert_StrToStrHexa(CString String)
{

    int  res, temp;
    TCHAR cRetStr[16];

    res = 0;
    for( int i = 0; i < String.GetLength(); i ++)
    {
        if( ('0' <= String[i]) && (String[i] <= '9'))
        {
            temp = String[i] - '0';
        }
        else if( ('A' <= String[i]) && (String[i] <= 'F'))
        {
            temp = String[i] - 'A' + 10;
        }
        else if( ('a' <= String[i]) && (String[i] <= 'f'))
        {
            temp = String[i] - 'a' + 10;
        }
        else if( res)
        {
            break;
        }
        else
        {
            continue;
        }

        res = 16 * res + temp;
    }

    memset(cRetStr,'\0',sizeof(cRetStr));
    if(res <= 0xff)
    {
        _stprintf(cRetStr,_T("0x%02x"),res);
    }
    else if(res > 0xff && res <= 0xfff)
    {
        _stprintf(cRetStr,_T("0x%03x"),res);
    }
    else if(res > 0xfff && res <= 0xffff)
    {
        _stprintf(cRetStr,_T("0x%04x"),res);
    }
    else
    {
        _tcscpy(cRetStr,_T("0x00"));
    }

    return CString(cRetStr);
}

//---------------------------------------------------------------------------
int ForceDirectories(CString Dir)
{
    TCHAR *cBuffer ;
    TCHAR *p;
    CString sDir;

    int nRet = 0;
    int len = Dir.GetLength();

    if (len <= 0)
        return 0;

    sDir = Dir;
    cBuffer = (TCHAR*)malloc(len+1);
    _tcscpy(cBuffer,(LPCTSTR)sDir);

    nRet = CreateDirectory((LPCTSTR)cBuffer,NULL);
    if (nRet != 0)
    {
        free(cBuffer);
        return 1;
    }

    p = cBuffer+1;
    while (1)
    {
        char hold;

        while(*p && *p != '\\' && *p != '/')  p++;
        hold = *p;
        *p = 0;

        nRet = CreateDirectory(cBuffer,NULL);
        if (nRet == -1 && errno == ENOENT)
        {
            AfxMessageBox(_T("couldn't create ") + Dir + _T(" directory"));
            free(cBuffer);
            return 0;
        }
        if (hold == 0)
            break;
        *p++ = hold;
    }
    free(cBuffer);
    return 1;
}
//---------------------------------------------------------------------------
//시작 Home Dir를 얻습니다.
CString ExtractFileDir()
{
    TCHAR szDir[256];
    CString sDir;
    TCHAR *pdest;

    ZeroMemory(szDir,sizeof(szDir));
    if(GetModuleFileName( NULL,szDir,256) == 0)
    {
        return _T("");
    }

    pdest = _tcsrchr(szDir, '\\');
    szDir[pdest-szDir +1] = NULL;
    sDir= (CString)szDir;

    return sDir;
}
//---------------------------------------------------------------------------

//시작 Home Dir를 얻습니다.
CString ChangeFileExt(CString sFile,CString sExe)
{
    if(sFile.IsEmpty()) return _T("");

    TCHAR *pdest;
    CString sTemp;
    TCHAR *szTemp;

    szTemp = new TCHAR[sFile.GetLength()+1];

    _tcscpy(szTemp,sFile);
    pdest = _tcsrchr(szTemp, '.');

    if(pdest != NULL)
    {
        szTemp[pdest-szTemp] = NULL;
    }

    sTemp= (CString)szTemp + sExe;

    delete[] szTemp;
    return sTemp;
}
//---------------------------------------------------------------------------
//시작 Home Dir를 얻습니다.
CString ExtractFileName(CString sFile)
{
    if(sFile.IsEmpty()) return _T("");

    TCHAR *pdest;
    CString sTemp;
    TCHAR *szTemp;

    szTemp = new TCHAR[sFile.GetLength()+1];

    _tcscpy(szTemp,sFile);
    pdest = _tcsrchr(szTemp, '\\');

    sTemp = _T("");
    if(pdest != NULL)
    {
        pdest++;
        sTemp= (CString)pdest;
    }
    else
    {
        sTemp = sFile;
    }

    delete[] szTemp;
    return sTemp;
}
//---------------------------------------------------------------------------

BOOL DirectoryExists(LPCTSTR Name)
{
    if(CString(Name).IsEmpty()) return 0;
    DWORD Code = GetFileAttributes(Name);
    return (Code != -1) && (Code & FILE_ATTRIBUTE_DIRECTORY );
}
//---------------------------------------------------------------------------
CString IntToStr(int iValue)
{
    CString sTemp = _T("");
    sTemp.Format(_T("%i"),iValue);
    return sTemp;
}
//---------------------------------------------------------------------------
CString FloatToStr(double fValue)
{
    CString sTemp = _T("");
    sTemp.Format(_T("%f"),fValue);
    return sTemp;
}
//---------------------------------------------------------------------------
/*
void SearchItem_TreeView(CTreeCtrl *pTreeView, CString sSearchName)
{
    HTREEITEM hItem;
    CString src;
    int nNotSearch = -1;
    sSearchName.MakeUpper();
    hItem = pTreeView->GetFirstVisibleItem( );
    for(UINT i = 0; i < pTreeView->GetCount( );i++)
    {
        src=pTreeView->GetItemText(hItem);
        src.MakeUpper();
        if(src == sSearchName)
        {
            pTreeView->SelectItem(hItem);
            break;
        }
        hItem = pTreeView->GetNextItem(hItem,TVGN_NEXT);
    }
}*/
//---------------------------------------------------------------------------
int	Get_FileSize( CString FilePath )
{
    int nRet;

    FILE * fp;

    fp = _tfopen((LPCTSTR)FilePath, _T("rb"));
    if( fp == NULL)
    {
        AfxMessageBox(_T("File open error!"), MB_OK, NULL);
        return -1;
    }
    nRet = fseek( fp, 0, SEEK_END);
    nRet = ftell(fp);
    fclose(fp);

    return nRet;

}
//---------------------------------------------------------------------------
int GetList_File( CString SourceDir, CString Exe,CString Filter, CStringList * List)
{
    //CStringList *StringList;
    CString  SearchExe,sTemp;
    CString  Path;
    WIN32_FIND_DATA  SearchRec;
    HANDLE         nFound;
    BOOL bFound;

    Path = SourceDir + Exe;
    //StringList = new CStringList;
    Filter.MakeUpper();
    List->RemoveAll();
    if(!DirectoryExists(SourceDir)) return 1;

    nFound = FindFirstFile( Path, &SearchRec);
    if( nFound != INVALID_HANDLE_VALUE)
    {
        do
        {
            if( _tcscmp(SearchRec.cFileName,_T("."))!=0  && _tcscmp(SearchRec.cFileName,_T("..")) !=0)
            {

                if( SearchRec.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
                {
                    sTemp = SearchRec.cFileName;
                    sTemp.MakeUpper();
                    if(sTemp.Find(Filter) >= 0)
                    {
                        List->AddTail( SearchRec.cFileName);
                    }
                }
            }
            bFound = FindNextFile(nFound, &SearchRec);
        }while( bFound);
        FindClose(nFound);
    }

    return 1;
}
//---------------------------------------------------------------------------
int GetList_File_With_FileName( CString SourceDir, CString FileNamem, CString Exe,CString Filter, CStringList * List)
{
    //CStringList *StringList;
    CString  SearchExe,sTemp;
    CString  Path;
    WIN32_FIND_DATA  SearchRec;
    HANDLE         nFound;
    BOOL bFound;

    Path = SourceDir + Exe;
    //StringList = new CStringList;
    Filter.MakeUpper();
    List->RemoveAll();
    if(!DirectoryExists(SourceDir)) return 1;

    nFound = FindFirstFile( Path, &SearchRec);
    if( nFound != INVALID_HANDLE_VALUE)
    {
        do
        {
            if( _tcscmp(SearchRec.cFileName,_T("."))!=0  && _tcscmp(SearchRec.cFileName,_T("..")) !=0)
            {

                if( SearchRec.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
                {
                    sTemp = SearchRec.cFileName;
                    sTemp.MakeUpper();
                    FileNamem.MakeUpper();
                    if(sTemp.Find(Filter) >= 0 && sTemp.Find(FileNamem) >= 0 )  // CString
                    {
                        List->AddTail( SearchRec.cFileName);
                    }
                }
            }
            bFound = FindNextFile(nFound, &SearchRec);
        }while( bFound);
        FindClose(nFound);
    }

    return 1;
}
//---------------------------------------------------------------------------
TCHAR GetCharIndex( int Index ) // 0, 1, 2, ~ , 9, A, B, C, ~,
{
    if( Index >= 0 && Index < 10 )
    {
        Index = 48 + Index;
    }
    else
    {
        Index = 55 + Index;
    }

    return (TCHAR)Index;
}
//---------------------------------------------------------------------------
int GetIntIndex( TCHAR Char )
{
    int nRet;

    if( (int)Char > 47 && (int)Char < 58 )
    {
        nRet = (int)Char - 48;
    }
    else
    {
        nRet = (int)Char - 55;
    }
    return nRet;
}
//--------------------------------------------------------------------------
CString ExtractFileName( CString sFile, CString sExe )
{
    CString sTemp;
    TCHAR szTemp[1024] = {0,};
    TCHAR szchar;

    if(sFile.Find(sExe) > -1)
    {
        for(int i =0; i < sFile.GetLength(); i++ )
        {
            szchar	= sFile.GetAt(i);
            if(szchar== '.'){
                break;
            }
            else{
                szTemp[i] = szchar;
            }
        }
        sTemp.Format(_T("%s"), szTemp);
    }
    else
    {
        sTemp = sFile;
    }

    return sTemp;
}
//--------------------------------------------------------------------------
CString  BlankFillter( CString sSource )
{
    CString sTemp;
    TCHAR szTemp[1024] = {0,};

    int nCnt = 0;


    for(int i = 0; i < sSource.GetLength(); i++)
    {
        if(sSource.GetAt(i) == ' '){}
        else
        {
            szTemp[nCnt] = sSource.GetAt(i);
            nCnt++;
        }
    }

    _stprintf( (LPTSTR)(LPCTSTR)sTemp, szTemp );

    return sTemp;
}

__int64 GetClockCount( void )
{
    __int64 count;
    _LARGE_INTEGER freq;
    _LARGE_INTEGER clock;

    QueryPerformanceFrequency((_LARGE_INTEGER*)&freq);	// cpu 클럭 얻어오기.
    QueryPerformanceCounter((_LARGE_INTEGER*)&clock);	// 현재 클럭 얻어오기.

    count = clock.QuadPart;
    count /= (freq.QuadPart*0.001);						// 현재 클럭을 클럭주파수로 나누기, usec->sec 변환.

    return count;
}

int Rotate(double OriginX, double OriginY, double *TargetX, double *TargetY, double Angle)
{
    Angle = Angle * ( 3.1415926535 / 180. );

    double dTargetX, dTargetY;

    dTargetX = *TargetX;
    dTargetY = *TargetY;

    *TargetX = OriginX + ( cos(Angle) * ( dTargetX - OriginX ) ) - ( sin(Angle) * (dTargetY - OriginY ) );
    *TargetY = OriginY + ( sin(Angle) * ( dTargetX - OriginX ) ) + ( cos(Angle) * (dTargetY - OriginY ) );

    return 1;
}

int ProcessMessages(void)
{
    MSG msg;
    while(::PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
            ::DispatchMessage(&msg);
    return 0;
}

int Delay( int ms )
{
    int nStick = GetTickCount();
    int nEtick = GetTickCount();
    while( nEtick-nStick < ms ){
        Sleep(1);
        ProcessMessages();
        nEtick = GetTickCount();
    }
    return 0;
}

int SubFolderSearchFile(LPCTSTR FolderName, LPCTSTR FileName)
{
    CFileFind finder;

    int nCount;

    nCount = 0;
    CString m_strPath , m_strFileName;
    m_strPath.Format(_T("%s\\*.*"),FolderName);
    m_strFileName.Format(_T("%s"), FileName);

    BOOL bWorking = finder.FindFile(m_strPath);

    while(bWorking)
    {
        bWorking = finder.FindNextFile();

        if(finder.IsDots())
        {
            continue;
        }
        else if (finder.IsDirectory())
        {
            SubFolderSearchFile(finder.GetFilePath()+"\\", FileName);
        }
        else //파일인 경우.
        {
            //CString tempPath = finder.GetFilePath();
            CString tempPath = finder.GetFileName();

            CString tempFileTitle = finder.GetFileTitle();

            if(tempPath.Right(3) !=_T("jpg")) continue;

            if(tempPath.Right(3) ==_T("jpg"))
            {
                nCount++;
                //continue;

            }

            else continue;

        }
    }

    finder.Close();

    return nCount;
}
TCHAR *getLastErrorText(int nErrorCode)
{
    static TCHAR buff[256];
    TCHAR *pBuf = buff;// , ULONG bufSize
    DWORD retSize;
    LPTSTR pTemp = NULL;
    retSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_ARGUMENT_ARRAY,
        NULL,
        nErrorCode, //GetLastError(),
        LANG_NEUTRAL,
        (LPTSTR)&pTemp,
        0,
        NULL);
    if (!retSize || pTemp == NULL) {
        pBuf[0] = '\0';
    }
    else {
        pTemp[_tcslen(pTemp) - 2] = '\0'; //remove cr and newline character
        _stprintf(pBuf, _T("%s (0x%x)"), pTemp, nErrorCode);// GetLastError());
        LocalFree((HLOCAL)pTemp);
    }
    return(pBuf);
}

CString MakeUuidString(UUID* pUUID/*=NULL*/)
{
    UUID uuid;
    CString guid = _T("");
    ::ZeroMemory(&uuid, sizeof(UUID));

    // Create uuid or load from a string by UuidFromString() function
    ::UuidCreate(&uuid);

    // If you want to convert uuid to string, use UuidToString() function
    WCHAR* wszUuid = NULL;
    ::UuidToStringW(&uuid, (RPC_WSTR*)&wszUuid);
    if (wszUuid != NULL)
    {
        guid = wszUuid;            // ERROR: no operator "=" matches these operands operand types are: std::string = WCHAR*

        ::RpcStringFreeW((RPC_WSTR*)&wszUuid);
        wszUuid = NULL;
    }


    return guid;
}
