#pragma once

class CIniUtil
{
public:
	CIniUtil(void);
	virtual ~CIniUtil(void);

	CIniUtil(CString sFileName);

	int ReadInt(CString sSection, CString sKey, int nDefaultValue = 0);
	double ReadDouble(CString sSection, CString sKey, double dDefaultValue = 0);
	CString ReadString(CString sSection, CString sKey, CString sDefaultValue = _T(""));
	DWORD	ReadDWORD(CString sSection, CString sKey, DWORD sDefaultValue);

	void WriteInt(CString sSection, CString sKey, int nValue);
	void WriteDouble(CString sSection, CString sKey, double dValue);
	void WriteString(CString sSection, CString sKey, CString sValue);
	void WriteDWORD(CString sSection, CString sKey, DWORD dValue);
	void DeleteSection(CString sSection);

protected:
	CString m_sFileName;
private:
	TCHAR sztemp[MAX_PATH];
};
