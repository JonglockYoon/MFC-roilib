#pragma once

#define WM_LOG_MSG	WM_USER + 2800

enum LOGLIST_TYPE{
	_LOG_LIST_SYS,
	_LOG_LIST_INSP,
	_LOG_LIST_NUMBER
};

class CLogData{
public:
	CLogData(void);
	~CLogData(void);

	int		iLogType;
	int		iErrorType;
	CString msg;
};

struct TLogList{
	int type;
	CString data;
};

class CLogClass
{
public:

	CLogClass(void);
	CLogClass(CWnd *pWnd);
	~CLogClass(void);

	CWnd *m_pWnd;
	CCriticalSection	cs;
	

	CLogData mcData;
	CString m_sLogPath;

	long	mMesaageID;
	CRITICAL_SECTION csAllDefectHandle;

	//bool m_InitFlag;
	int m_nUpdateTime;
	int AddLog(int LogType, LPCTSTR lpszFormat, ...);
	int AddLog(CString data, int LogType = 0);
	bool CheckLog(int LogType = _LOG_LIST_SYS);

private:
	CString GetExePath();
	bool IsPathTrue(CString path);
public:
	void SetParentWnd(CWnd * pWnd);
};

extern CLogClass *g_cLog;
