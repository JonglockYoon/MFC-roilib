
// TESTView.cpp : CScrollViewEx 클래스의 구현
//

#include "stdafx.h"
#include "ScrollViewEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScrollViewEx

IMPLEMENT_DYNCREATE(CScrollViewEx, CScrollView)

BEGIN_MESSAGE_MAP(CScrollViewEx, CScrollView)
	ON_WM_MOUSEMOVE()
	//ON_WM_RBUTTONDOWN()
	//ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CScrollViewEx 생성/소멸

CScrollViewEx::CScrollViewEx()
: m_bMagnify(false)
, m_nVal(0)
, m_pImage(NULL)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CScrollViewEx::~CScrollViewEx()
{
}

// CScrollViewEx 그리기

void CScrollViewEx::OnDraw(CDC* pDC)
{
	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


void CScrollViewEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bMagnify)
	{
		CClientDC dc(this);
		CDC MemDC;
		CPen redPen(PS_SOLID,1,RGB(255,0,0)), *pOldPen;
		MemDC.CreateCompatibleDC(&dc);
		CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&m_bmpMagnify);
			
		pOldPen = (CPen *)dc.SelectObject(&redPen);
		dc.SelectObject(GetStockObject(NULL_BRUSH));
			
		dc.BitBlt(m_ptOld.x-RECTSIZE/2,m_ptOld.y-RECTSIZE/2,RECTSIZE,RECTSIZE,&MemDC, 0, 0, SRCCOPY);

		MemDC.BitBlt(0,0,RECTSIZE,RECTSIZE,&dc, point.x-RECTSIZE/2, point.y-RECTSIZE/2, SRCCOPY);

		dc.StretchBlt(point.x-RECTSIZE/2,point.y-RECTSIZE/2,RECTSIZE,RECTSIZE, &MemDC, 
					  3*RECTSIZE/8, 3*RECTSIZE/8, RECTSIZE/4, RECTSIZE/4, SRCCOPY);
		
		dc.Rectangle(point.x-RECTSIZE/2,point.y-RECTSIZE/2,point.x+RECTSIZE/2,point.y+RECTSIZE/2);
		dc.MoveTo(point.x, point.y-RECTSIZE/2); dc.LineTo(point.x, point.y+RECTSIZE/2);
		dc.MoveTo(point.x-RECTSIZE/2, point.y); dc.LineTo(point.x+RECTSIZE/2, point.y);
		
		CString str;
		CPoint pt = point+GetScrollPosition();

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(0, 255, 0));
		str.Format(_T("%d,%d"), pt.x, pt.y); 
		dc.TextOut(point.x , point.y-40, str);

		dc.SelectObject(pOldPen);
		MemDC.SelectObject(pOldBitmap);
		
		m_ptOld = point;
	}

	CScrollView::OnMouseMove(nFlags, point);
}
//
//void CScrollViewEx::OnRButtonDown(UINT nFlags, CPoint point)
//{
//	CClientDC dc(this);
//	CDC MemDC;
//	CPen redPen(PS_SOLID,1,RGB(255,0,0)), *pOldPen;
//	
//	CRect rect;
//	GetClientRect(&rect);
//	ClientToScreen(&rect);
//	ClipCursor(&rect);
//
//	ShowCursor(FALSE);
//
//	MemDC.CreateCompatibleDC(&dc);
//	m_bmpMagnify.CreateCompatibleBitmap(&dc, RECTSIZE, RECTSIZE);
//	CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&m_bmpMagnify);
//		
//	MemDC.BitBlt(0,0,RECTSIZE,RECTSIZE,&dc, point.x-RECTSIZE/2, point.y-RECTSIZE/2, SRCCOPY);
//			
//	pOldPen = (CPen *)dc.SelectObject(&redPen);
//	dc.SelectObject(GetStockObject(NULL_BRUSH));
//			
//	dc.StretchBlt(point.x-RECTSIZE/2,point.y-RECTSIZE/2,RECTSIZE,RECTSIZE,&MemDC, 3*RECTSIZE/8, 3*RECTSIZE/8, RECTSIZE/4, RECTSIZE/4, SRCCOPY);
//			
//	dc.Rectangle(point.x-RECTSIZE/2,point.y-RECTSIZE/2,point.x+RECTSIZE/2,point.y+RECTSIZE/2);
//	dc.MoveTo(point.x, point.y-RECTSIZE/2); dc.LineTo(point.x, point.y+RECTSIZE/2);
//	dc.MoveTo(point.x-RECTSIZE/2, point.y); dc.LineTo(point.x+RECTSIZE/2, point.y);
//	
//	CString str;
//	CPoint pt = point+GetScrollPosition();
//	
//	COLORREF val = MemDC.GetPixel(RECTSIZE/2, RECTSIZE/2);
//
//	dc.SetBkMode(TRANSPARENT);
//	dc.SetTextColor(RGB(0, 255, 0));
//	str.Format(_T("%d,%d"), pt.x, pt.y); 
//	dc.TextOut(point.x , point.y-40, str);
//
//	dc.SelectObject(pOldPen);
//	MemDC.SelectObject(pOldBitmap);
//	m_ptOld = point;
//	m_bMagnify = TRUE;	
//
//	CScrollView::OnRButtonDown(nFlags, point);
//}
//
//void CScrollViewEx::OnRButtonUp(UINT nFlags, CPoint point)
//{
//	if(m_bMagnify)
//	{
//		CClientDC dc(this);
//		CDC MemDC;
//
//		MemDC.CreateCompatibleDC(&dc);
//		CBitmap *pOldBitmap = (CBitmap *)MemDC.SelectObject(&m_bmpMagnify);
//			
//		dc.BitBlt(m_ptOld.x-RECTSIZE/2,m_ptOld.y-RECTSIZE/2,RECTSIZE,RECTSIZE,
//			&MemDC, 0, 0, SRCCOPY);
//
//		m_bmpMagnify.DeleteObject();
//		MemDC.SelectObject(pOldBitmap);
//		m_bMagnify = FALSE;	
//		ShowCursor(TRUE);
//		ClipCursor(NULL);
//	}
//
//	CScrollView::OnRButtonUp(nFlags, point);
//}
