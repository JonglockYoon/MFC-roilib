#include "StdAfx.h"
#include "Log.h"

#pragma comment(lib,"shlwapi.lib")
#include <Shlwapi.h>

CLogClass *g_cLog = NULL;

CLogData::CLogData(void){
	msg=_T("");
}
CLogData::~CLogData(void){
	
}

CLogClass::CLogClass(CWnd *pWnd)
{
	m_pWnd = pWnd;
	m_sLogPath=_T("");
	mMesaageID = NULL;
	InitializeCriticalSectionAndSpinCount(&csAllDefectHandle, 100);

	CheckLog();
}

CLogClass::~CLogClass(void)
{
	DeleteCriticalSection(&csAllDefectHandle);
	Sleep(100);
}


CLogClass::CLogClass(void)
{
	m_pWnd = NULL;
	m_sLogPath=_T("");
	mMesaageID = NULL;
	InitializeCriticalSection(&csAllDefectHandle);
	//m_InitFlag = false;
}

int CLogClass::AddLog(int LogType, LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start (args, lpszFormat);
	TCHAR buf1[10000];
	_vstprintf(buf1,lpszFormat,args);
	int ret = AddLog(buf1, LogType);
	va_end (args);
	return ret;
}

int CLogClass::AddLog(CString str, int LogType)
{
	CSingleLock csl(&cs);

	int ret = 1;

	CString nStr, nDateStr, nTimeStr, s;
	int *pLogType;


		std::FILE *pFile = NULL;
		SYSTEMTIME	lsystime;


		GetLocalTime(&lsystime);
	
		nDateStr.Format(_T("%04d/%02d/%02d"), lsystime.wYear, lsystime.wMonth, lsystime.wDay);
		nTimeStr.Format(_T("%02d:%02d:%02d.%03d"), lsystime.wHour, lsystime.wMinute, lsystime.wSecond, lsystime.wMilliseconds); 
		nStr.Format(_T("[%s %s] %s\n"), nDateStr, nTimeStr, str	);
		if(m_pWnd != NULL) 
			::SendMessage(m_pWnd->GetSafeHwnd(), WM_LOG_MSG, (WPARAM)LogType, (LPARAM)&nStr);

		if((lsystime.wHour > 23 && lsystime.wMinute > 59) ||  (lsystime.wHour > 00 && lsystime.wMinute < 01)){
			CheckLog();
		}

		if((pFile = _tfopen((LPCTSTR)m_sLogPath, _T("ab+"))) == NULL){
			ret = 2;
		}
		else{
#ifdef _UNICODE
			fseek( pFile, 0, SEEK_END);
			long lSize = ftell(pFile);
			if (lSize == 0) {
				WORD mark = 0xFEFF;
				fwrite(&mark, sizeof(TCHAR), 1, pFile);
				fseek( pFile, 0, SEEK_END);
			}
#endif
			fwrite(nStr, sizeof(TCHAR), nStr.GetLength(), pFile);
			fclose(pFile);
			ret = 0;
		} 


		//csl.Unlock();
	return ret;
}



bool CLogClass::CheckLog(int LogType){
	CString nStr, s;

	CTime nTime = CTime::GetCurrentTime();
	nStr = GetExePath();
	s.Format(_T("\\LOG\\%04d\\%02d\\%02d.txt"), nTime.GetYear(), nTime.GetMonth(), nTime.GetDay());

	nStr.Append(s);

	if(IsPathTrue(nStr) == false){//File Create
		nStr = GetExePath();
		s.Format(_T("\\LOG"));
		nStr.Append(s);
		CreateDirectory(nStr, NULL);
		s.Format(_T("\\%04d"), nTime.GetYear());
		nStr.Append(s);
		CreateDirectory(nStr, NULL);
		s.Format(_T("\\%02d"), nTime.GetMonth());
		nStr.Append(s);
		CreateDirectory(nStr, NULL);
		s.Format(_T("\\%02d.txt"), nTime.GetDay());
		nStr.Append(s);
		
		CFile file;
		if (file.Open(nStr, CFile::modeCreate | CFile::modeReadWrite))
		{
			file.Close();
		}

	}
	else{
		
	}

	m_sLogPath = nStr;

	return true;
}
CString CLogClass::GetExePath(){
	
	CString nExePath;

	TCHAR nPathName[_MAX_PATH];
	::GetModuleFileName(::AfxGetInstanceHandle(), (LPTSTR)(LPWSTR)nPathName, _MAX_PATH);
	PathRemoveFileSpec(nPathName);

	nExePath.Format(_T("%s"), nPathName);

	if(nExePath.GetLength() < 1){
		nExePath = "D:\\PKG";
	}
	return nExePath;
}

bool CLogClass::IsPathTrue(CString path){
	CFileFind nFinder;
	bool return_value=false;

	return_value = nFinder.FindFile(path);

	return return_value;
}

void CLogClass::SetParentWnd(CWnd * pWnd)
{
	if(pWnd != NULL)
		m_pWnd = pWnd;
}
