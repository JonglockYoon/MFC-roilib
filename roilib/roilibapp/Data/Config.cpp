#include "StdAfx.h"
#include "Data/Config.h"
#include "IniUtil.h"

CConfig gCfg;

CConfig::CConfig()
{
	m_sLastRecipeName =_T("");
	m_sFileName.Format(_T(".\\Config\\Config.ini"));
}
CConfig::CConfig(TCHAR *pszPath)
{
	m_sLastRecipeName =_T("");
	m_sFileName.Format(_T("%s\\Config\\CONFIG.ini"), pszPath);
}

CConfig::~CConfig(void)
{
}

void CConfig::ReadConfig()
{
	CIniUtil* ini = new CIniUtil(m_sFileName);
	CString str;
	CString dumy;

	m_sLastRecipeName =	ini->ReadString(_T("SYSTEMINFO"),_T("LASTRECIPE"),_T("Test"));
	
	m_bSaveEngineImg		=	ini->ReadInt(_T("SYSTEMINFO"), _T("SAVEENGINEIMG"), false);
	m_bSaveGrabImg	=	ini->ReadInt(_T("SYSTEMINFO"), _T("SAVEGRABIMG"), false);

	for (int i = 0; i < MAX_CH_NUM; i++) {
		str.Format(_T("WEBCAM%d"), i);
		m_Webcam[i] = ini->ReadInt(_T("SYSTEMINFO"), str, false);
	}

	m_sSaveRootDir	=	ini->ReadString(_T("SYSTEMINFO"), _T("SAVE_DIR"), _T("C"));
	if (m_sSaveRootDir.GetLength() == 1) {
		m_sSaveRootDir += _T(":");
	} else {
		int len = m_sSaveRootDir.GetLength();
		if (len > 0) {
			TCHAR c = m_sSaveRootDir.GetAt(len-1);
			if (c == '\\')
				m_sSaveRootDir.TrimRight(c);
		}
	}

	m_dRulerMaxY = ini->ReadDouble(_T("RULLER"),_T("MAXY"), 0);
	m_dRulerMaxX = ini->ReadDouble(_T("RULLER"), _T("MAXX"), 0);
	m_Root = ini->ReadString(_T("SYSTEMINFO"),_T("ROOT_DIR"),_T(""));

	delete ini;
}
void CConfig::WriteConfig()
{
	CIniUtil* ini = new CIniUtil(m_sFileName);
	CString str;
	CString dumy;

	ini->WriteString(_T("SYSTEMINFO"), _T("ROOT_DIR"), gCfg.m_Root);
	ini->WriteString(_T("SYSTEMINFO"),_T("LASTRECIPE"),m_sLastRecipeName);
	ini->WriteInt(_T("SYSTEMINFO"), _T("SAVEENGINEIMG"), m_bSaveEngineImg);
	ini->WriteInt(_T("SYSTEMINFO"), _T("SAVEGRABIMG"), m_bSaveGrabImg);
	ini->WriteString(_T("SYSTEMINFO"), _T("SAVE_DIR"), m_sSaveRootDir);

	for (int i = 0; i < MAX_CH_NUM; i++) {
		str.Format(_T("WEBCAM%d"), i);
		ini->WriteInt(_T("SYSTEMINFO"), str, m_Webcam[i]);
	}
	ini->WriteDouble(_T("RULLER"), _T("MAXY"), m_dRulerMaxY);
	ini->WriteDouble(_T("RULLER"), _T("MAXX"), m_dRulerMaxX);

	delete ini;
}
