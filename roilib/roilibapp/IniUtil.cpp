#include "StdAfx.h"
#include "IniUtil.h"

CIniUtil::CIniUtil(void)
{
}

CIniUtil::~CIniUtil(void)
{
}

CIniUtil::CIniUtil(CString sFileName)
{
#ifdef _UNICODE
	if(!::PathFileExists(sFileName))
	{
		// UTF16-LE BOM(FFFE)
		WORD wBOM = 0xFEFF;
		DWORD NumberOfBytesWritten;

		HANDLE hFile = ::CreateFile(sFileName, GENERIC_WRITE, 0, 
					   NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		::WriteFile(hFile, &wBOM, sizeof(WORD), &NumberOfBytesWritten, NULL);
		::CloseHandle(hFile);
	}
#endif
	m_sFileName = sFileName;
}

int CIniUtil::ReadInt(CString sSection, CString sKey, int nDefaultValue)
{
	if(m_sFileName == _T(""))
		return 0;
	
	CString sDef;
	sDef.Format(_T("%d"),nDefaultValue);
	GetPrivateProfileString(sSection,sKey,sDef,sztemp,MAX_PATH,m_sFileName);
	int nRet = _ttoi(sztemp);
	return nRet; 
}

double CIniUtil::ReadDouble(CString sSection, CString sKey, double dDefaultValue)
{	
	if(m_sFileName == _T(""))
		return 0;

	CString sDef;
	sDef.Format(_T("%f"),dDefaultValue);
	GetPrivateProfileString(sSection,sKey,sDef,sztemp,MAX_PATH,m_sFileName);
	double dRet = _ttof(sztemp);
	return dRet;
}

CString CIniUtil::ReadString(CString sSection, CString sKey, CString sDefaultValue)
{
	if(m_sFileName == _T(""))
		return  _T("");

	GetPrivateProfileString(sSection,sKey,sDefaultValue,sztemp,MAX_PATH,m_sFileName);
	CString sRet = sztemp;
	return sRet;
}

DWORD CIniUtil::ReadDWORD(CString sSection, CString sKey, DWORD dDefaultValue)
{
	if(m_sFileName == _T(""))
		return  0;

	CString sDef;
	sDef.Format(_T("%lu"),dDefaultValue);
	GetPrivateProfileString(sSection,sKey,sDef,sztemp,MAX_PATH,m_sFileName);
	DWORD dRet = _ttol(sztemp);
	return dRet;
}

void CIniUtil::WriteInt(CString sSection, CString sKey, int nValue)
{
	if(m_sFileName == _T(""))
		return;
	CString str;
	str.Format(_T("%d"),nValue);
	WritePrivateProfileString(sSection,sKey,str,m_sFileName);
}

void CIniUtil::WriteDouble(CString sSection, CString sKey, double dValue)
{
	if(m_sFileName == _T(""))
		return;
	CString str;
	str.Format(_T("%f"),dValue);
	WritePrivateProfileString(sSection,sKey,str,m_sFileName);
}

void CIniUtil::WriteString(CString sSection, CString sKey, CString sValue)
{
	if(m_sFileName == _T(""))
		return;	
	WritePrivateProfileString(sSection,sKey,sValue,m_sFileName);
}

void CIniUtil::WriteDWORD(CString sSection, CString sKey, DWORD dValue)
{
	if(m_sFileName == _T(""))
		return;
	CString str;
	str.Format(_T("%lu"),dValue);
	WritePrivateProfileString(sSection,sKey,str,m_sFileName);
}

void CIniUtil::DeleteSection(CString sSection)
{
	if(m_sFileName == _T(""))
		return;	
	WritePrivateProfileString(sSection,NULL,NULL,m_sFileName);
}