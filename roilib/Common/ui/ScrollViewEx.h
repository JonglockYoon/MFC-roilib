
// ScrollViewEx.h : CScrollViewEx 클래스의 인터페이스
//

#pragma once

#define RECTSIZE	200

class CScrollViewEx : public CScrollView
{
public: // serialization에서만 만들어집니다.
	CScrollViewEx();
	virtual ~CScrollViewEx();

	DECLARE_DYNCREATE(CScrollViewEx)

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.

public:
	CBitmap m_bmpMagnify;		//확대경을 위한 비트맵
	bool	m_bMagnify;
	CPoint	m_ptOld;
	int		m_nVal;
	CxImage *m_pImage;

// 구현입니다.
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


