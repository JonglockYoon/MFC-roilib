#include "stdafx.h"
#include "DataBlock.h"

/////////////////////////////////////////////////////////////////////////////
// CDataBlock

CDataBlock::CDataBlock(int dir, TCHAR *pBlock, int nLength, BOOL bKeep) 
{ 
	m_bRequest = FALSE;
	m_bKeep = FALSE;
	m_nDir = dir;
	if (bKeep) {
		m_pBlock = pBlock;
		m_nLength = nLength; 
	}
	else {
		m_nLength = nLength;
		m_pBlock = new TCHAR[nLength];
		if (m_pBlock != NULL) ::CopyMemory(m_pBlock, pBlock, m_nLength*sizeof(TCHAR)); 
		else m_nLength = 0;
	}
	GetLocalTime(&m_Time);
}

CDataBlock::~CDataBlock() 
{ 
	if (!m_bKeep && m_pBlock)
		delete m_pBlock;
}

TCHAR *CDataBlock::GetDataString()
{
	return m_pBlock;
}

TCHAR *CDataBlock::GetDirString()
{
	static TCHAR result[16];
	result[0] = m_nDir;
	result[1] = '\0';
	return result;
}

TCHAR *CDataBlock::GetTimeString()
{
	static TCHAR result[32];
	//SYSTEMTIME sysTime;
	//GetSystemTime(&sysTime);

	_stprintf(result, _T("%02d:%02d:%02d.%03d"), m_Time.wHour, m_Time.wMinute, m_Time.wSecond,
										 m_Time.wMilliseconds);
	return result;
}

