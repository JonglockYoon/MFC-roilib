
// ScrollViewEx.h : CScrollViewEx Ŭ������ �������̽�
//

#pragma once

#define RECTSIZE	200

class CScrollViewEx : public CScrollView
{
public: // serialization������ ��������ϴ�.
	CScrollViewEx();
	virtual ~CScrollViewEx();

	DECLARE_DYNCREATE(CScrollViewEx)

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.

public:
	CBitmap m_bmpMagnify;		//Ȯ����� ���� ��Ʈ��
	bool	m_bMagnify;
	CPoint	m_ptOld;
	int		m_nVal;
	CxImage *m_pImage;

// �����Դϴ�.
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


