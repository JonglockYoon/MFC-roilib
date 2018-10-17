#ifndef DataBlock_h

#define DataBlock_h
//#include "lists.h"

///////////////////////////////////////////////////////////////////////////
// CDataBlock

class CDataBlock //: public CObject
{
// Construction
public:
	CDataBlock(int dir, TCHAR* pBlock, int nLength, BOOL bKeep = TRUE);
	virtual ~CDataBlock();

// Attributes
public:
	operator int() { return m_nLength; }
	//operator LPCTSTR() { return (LPCTSTR)m_pBlock; }

protected:
	TCHAR* 	m_pBlock;
	int	    m_nLength;
	BOOL	m_bKeep;
	BOOL	m_bRequest;
	SYSTEMTIME m_Time;
	int		m_nDir;
// Operations
public:
	TCHAR *GetDataString();
	TCHAR *GetDirString();
	TCHAR *GetTimeString();
};

#endif