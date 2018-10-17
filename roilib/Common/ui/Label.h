#if !defined(AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
#define AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Label.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLabel window
enum FlashType {None, Text, Background };

class CLabel : public CStatic
{
// Construction
public:
	CLabel();
	CLabel& SetBkColor(COLORREF crBkgnd);
	CLabel& SetTextColor(COLORREF crText);
	CLabel& SetText(const CString& strText);
	CLabel& SetFontBold(BOOL bBold);
	CLabel& SetFontName(const CString& strFont);
	CLabel& SetFontUnderline(BOOL bSet);
	CLabel& SetFontItalic(BOOL bSet);
	CLabel& SetFontSize(int nSize);
	CLabel& SetSunken(BOOL bSet);
	CLabel& SetBorder(BOOL bSet);
	CLabel& SetBntFlag(BOOL bSet);
	CLabel& FlashText(BOOL bActivate);
	CLabel& FlashBackground(BOOL bActivate);
	//CLabel& SetLink(BOOL bLink);
	CLabel& SetLink(BOOL bLink, CString strlink = _T(""), bool bend = false);
	CLabel& SetLinkCursor(HCURSOR hCursor);

// Attributes
public:
protected:
	void ReconstructFont();
	COLORREF	m_crText;
	COLORREF	m_crBack;
	HBRUSH		m_hBrush;
	HBRUSH		m_hwndBrush;
	LOGFONT		m_lf;
	CFont		m_font;
	CString		m_strText;
	BOOL		m_bState;
	BOOL		m_bTimer;
	BOOL		m_bLink;
	FlashType	m_Type;
	HCURSOR		m_hCursor;
	CString		m_url;
	BOOL		m_bend;
	BOOL		m_BntFlag;
	CWnd*		m_mCap;
	int			m_iStatus;
			// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLabel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLabel)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LABEL_H__A4EABEC5_2E8C_11D1_B79F_00805F9ECE10__INCLUDED_)
