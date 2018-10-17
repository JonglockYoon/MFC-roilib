#include "StdAfx.h"
#include "Direct_Info.h"

#include <direct.h>//디렉토리 관련 헤더

CDirect_Info * g_pDirectory;

CDirect_Info::CDirect_Info(void)
{
}

CDirect_Info::~CDirect_Info(void)
{
}

CString CDirect_Info::Get_Root_Path(){
    HMODULE hModule;
    TCHAR sFilepath[MAX_PATH];
    TCHAR *p;
    hModule = ::GetModuleHandle(NULL);
    if(NULL == ::GetModuleFileName(hModule, sFilepath, MAX_PATH))
        return _T("");
    if((p = _tcsrchr(sFilepath, _T('\\'))) == NULL)
        if((p = _tcsrchr(sFilepath, _T('/'))) == NULL)
            return _T("");
    *p=0;
    return sFilepath;
}

void CDirect_Info::Create_Directory(CString dir)
{
    TCHAR	sTargetFile[MAX_PATH];

    _tcscpy(sTargetFile, dir);


    if(_tchdir((LPTSTR)sTargetFile)){
        _tmkdir((LPTSTR)sTargetFile);
        _tchdir((LPTSTR)sTargetFile);
    }
}

bool CDirect_Info::CreateOrNoTuncate_File(CString file_name)
{
    CFile file;

    if(!file.Open(file_name, CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate))
        return false;

#ifdef _UNICODE
    if (file.GetLength() == 0) {
        WORD wBOM = 0xFEFF;
        DWORD NumberOfBytesWritten;
        file.Write(&wBOM, sizeof(WORD));
    }
#endif

    file.Close();
    return true;
}

bool CDirect_Info::Search_FileName(CString file_name){
    HANDLE	hSrch;
    TCHAR	sTargetFile[MAX_PATH];
    WIN32_FIND_DATA	fileData1;

    CString str;

    str.Format(file_name);

    _tcscpy(sTargetFile, str);

    hSrch = FindFirstFile(sTargetFile, &fileData1);

    if(hSrch == INVALID_HANDLE_VALUE){
        FindClose(hSrch);
        return false;
    }
    else{
        FindClose(hSrch);
        return true;
    }
}

CString CDirect_Info::Get_Day(bool only_number = true){

    CTime time;
    CString Str, day, Year, Month, Day;
    time = CTime::GetCurrentTime();

    Str.Format(_T("%04d"),time.GetYear());

    Year = Str;

    Month.Format(_T("%02d"),time.GetMonth());
    Day.Format(_T("%02d"),time.GetDay());

    if(only_number){
        day.Format(_T("%s%s%s"), Year, Month, Day);
    }
    else{
        day.Format(_T("%s - %s - %s"), Year, Month, Day);
    }


    return day;
}


CString CDirect_Info::Get_Time(bool only_number = true){
    CTime time;
    CString Str, Hour, Min, Sec;
    time = CTime::GetCurrentTime();

    Hour.Format(_T("%02d"),time.GetHour());
    Min.Format(_T("%02d"),time.GetMinute());
    Sec.Format(_T("%02d"),time.GetSecond());

    if(only_number){
        Str.Format(_T("%s%s%s"), Hour, Min, Sec);
    }
    else{
        Str.Format(_T("%s:%s:%s"), Hour, Min, Sec);
    }


    return Str;
}

int CDirect_Info::Delete_DirOrFile(CString sFile, HWND hWnd)
{
    TCHAR File[1024];
    int nRet;
    SHFILEOPSTRUCT op ={0};

    memset(File,'\0',sizeof(File));
    _tcscpy(File,(LPCTSTR)sFile);
    File[sFile.GetLength()+1] = '\0';

    //Dir 삭제
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
void CDirect_Info::DirectroyCopy(CString SrcFolder, CString DesFolder, HWND hWnd)
{
    SHFILEOPSTRUCT FileOp = {0};
    TCHAR szTemp[MAX_PATH];

    _tcscpy(szTemp, SrcFolder);
    szTemp[SrcFolder.GetLength() + 1] = NULL;	// NULL문자가 두개 들어가야 한다.

    FileOp.hwnd = hWnd;
    FileOp.wFunc = FO_COPY;
    FileOp.pFrom = szTemp;
    FileOp.pTo = LPCTSTR(DesFolder);
    FileOp.fFlags = FOF_MULTIDESTFILES |FOF_NOCONFIRMATION;  // 확인메시지가 안뜨도록 설정
    FileOp.fAnyOperationsAborted = false;
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = NULL;

    SHFileOperation(&FileOp);
}
//---------------------------------------------------------------------------
void CDirect_Info::DirectroyCopy(TCHAR *SrcFolder, TCHAR *DesFolder, HWND hWnd)
{
    SHFILEOPSTRUCT FileOp = {0};
    TCHAR szTemp[MAX_PATH];

    _tcscpy(szTemp, SrcFolder);
    szTemp[_tcslen(SrcFolder) + 1] = NULL;	// NULL문자가 두개 들어가야 한다.

    FileOp.hwnd = hWnd;
    FileOp.wFunc = FO_COPY;
    FileOp.pFrom = szTemp;
    FileOp.pTo = DesFolder;
    FileOp.fFlags = FOF_NOCONFIRMMKDIR |FOF_MULTIDESTFILES |FOF_NOCONFIRMATION;  // 확인메시지가 안뜨도록 설정
    FileOp.fAnyOperationsAborted = false;
    FileOp.hNameMappings = NULL;
    FileOp.lpszProgressTitle = NULL;

    SHFileOperation(&FileOp);
}


void CDirect_Info::SetPath(CString path){
    //헐키... 썅.. 더럽게 문제 많구낭...ㅡㅡ;;; 폴더의 권한을 이함수가 잡아버림으로... 제길슨한 문제가 발생하네..ㄷㄷ
    int index =0;
    CString s;
    int old_index = index;
    CString StrPath[MAX_PATH];

    for(int i=0; i<path.GetLength(); i++){
        s = path.Mid(i, 1);
        if(s == '\\'){
            old_index = index;
            index++;
        }
        else{
            StrPath[index] += s;
        }
    }
    index++;
    s="";
    for(int i=0; i<index; i++){
        s+=StrPath[i];
        if(Search_FileName(s) == false){
            Create_Directory(s);
        }
        s+= "\\";
    }
}


