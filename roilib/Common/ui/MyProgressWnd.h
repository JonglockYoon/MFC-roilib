#ifndef _INCLUDE_PROGRESSWND_H
#define _INCLUDE_PROGRESSWND_H

#pragma once


// CMyProgressWnd

class CMyProgressWnd : public CWnd
{
	DECLARE_DYNAMIC(CMyProgressWnd)

public:
	HBRUSH m_hbrush;

    CMyProgressWnd(BOOL bCenter=TRUE);
    CMyProgressWnd(CWnd* pParent, LPCTSTR pszTitle, BOOL bPos = TRUE, BOOL bSmooth = TRUE);
    virtual ~CMyProgressWnd();

    BOOL Create(CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth = TRUE);

protected:
	//{{AFX_MSG(CMyProgressWnd)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //}}AFX_MSG
	afx_msg void OnCancel();
	DECLARE_MESSAGE_MAP()

	void CommonConstruct();

	// Operations
public:
	BOOL m_MostTopFlag;
	BOOL m_bCenter;
    void SetRange(int nLower, int nUpper, int nStep = 1);
                                                    // Set range and step size
    int OffsetPos(int nPos);                        // Same as CProgressCtrl
    int StepIt();                                   //    "
    int SetStep(int nStep);                         //    "
    int SetPos(int nPos);                           //    "

    void SetText(LPCTSTR fmt, ...);                 // Set text in text area

    void Clear();                                   // Clear text, reset bar
    void Hide();                                    // Hide window
    void Show();                                    // Show window

	void SetWindowSize(int nNumTextLines, int nWindowWidth = 390);

    void PeekAndPump();  // Message pumping for modal operation   
    
// Implementation
protected:
    int  m_nPrevPos, m_nPrevPercent;
    int  m_nStep;
    int  m_nMaxValue, m_nMinValue;
    int  m_nNumTextLines;

    CStatic       m_Text;
    CProgressCtrl m_wndProgress;
    CButton       m_CancelButton;
    CString       m_strTitle;
    CFont         m_font;

public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

#endif


