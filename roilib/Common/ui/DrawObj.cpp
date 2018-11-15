// author : jerry1455@gmail.com
// DrawObj.cpp - implementation for drawing objects
//    Prof-UIS의 DRAWCLI Project에 가져와서 구현.
//


#include "stdafx.h"
#include "RoilibApp.h"
#include "DrawDoc.h"
#include "DrawVw.h"
#include "DrawObj.h"

//IMPLEMENT_SERIAL(CDrawObj, CObject, 0)

static int nLastSeq = 0; // Undo에서 사용할 Unique Seq 번호.

//CDrawObj::CDrawObj() : m_pParent(NULL), m_ZoomFactor(1.0)
//{
//	m_nSeq = nLastSeq++;
//}

CDrawObj::~CDrawObj()
{
    //TRACE(_T("Delete : CDrawObj\n"));
    //m_font.DeleteObject();
}

CDrawObj::CDrawObj(const CRect& position)
{
    m_textOffset = { 0, 0 };
    m_pParent = NULL;
    m_ZoomFactor = 1.0;

    m_position = position;
    m_pDocument = NULL;
    m_nShape = none;
    m_bSizeFix = FALSE;

    m_bPen = TRUE;
    m_logpen.lopnStyle = PS_INSIDEFRAME;
    m_logpen.lopnWidth.x = 1;
    m_logpen.lopnWidth.y = 1;
    m_logpen.lopnColor = RGB(0, 255, 0);

    m_bBrush = FALSE;
    m_logbrush.lbStyle = BS_SOLID;
    m_logbrush.lbColor = RGB(192, 192, 192);
    m_logbrush.lbHatch = HS_HORIZONTAL;

    fontHeight = 16;
    _tcscpy(fontFaceName, _T("Arial Unicode MS 보통"));

    m_textColor = RGB(0, 255, 255);
    m_textPosition = DT_BOTTOM | DT_LEFT | DT_SINGLELINE;

    m_bSelected = FALSE;
    m_bHidden = FALSE;
    m_nUndoOper = unone;
    m_nSeq = nLastSeq++;

    m_selcolor = RGB(255, 0, 255);

    m_pRoiData = new CRoiData();
    m_pRoiData->m_nCh = theApp.m_nSelectCh;
    m_pRoiData->SetRect(position);

    m_dwType = DRAWOBJ_TYPE_RECIPE;
    m_dwFilter = 0xFFFFFFFF;
}

CDrawObj& CDrawObj::operator=(const CDrawObj& myObj)
{
    if (this == &myObj)
        return *this;

    m_pParent = myObj.m_pParent;
    m_ZoomFactor = myObj.m_ZoomFactor;
    m_position = myObj.m_position;
    m_pDocument = myObj.m_pDocument;
    m_nShape = myObj.m_nShape;
    m_bSizeFix = myObj.m_bSizeFix;
    m_bPen = myObj.m_bPen;
    m_logpen = myObj.m_logpen;
    m_bBrush = myObj.m_bBrush;
    m_logbrush = myObj.m_logbrush;
    m_textColor = myObj.m_textColor;
    m_textPosition = myObj.m_textPosition;
    m_bHidden = myObj.m_bHidden;
    *m_pRoiData = *myObj.m_pRoiData;
    m_nSeq = myObj.m_nSeq;

    return *this;
}

void CDrawObj::SetLineWidth(int w)
{
    ASSERT_VALID(this);

    m_logpen.lopnWidth.x = w;
    m_logpen.lopnWidth.y = w;
    Invalidate();
}

void CDrawObj::SetLineColor(COLORREF color)
{
    ASSERT_VALID(this);

    m_logpen.lopnColor = color;
    Invalidate();
}

void CDrawObj::SetTextOffset(int x, int y)
{
    ASSERT_VALID(this);

    m_textOffset.x = x;
    m_textOffset.y = y;
    Invalidate();
}
void CDrawObj::SetTextColor(COLORREF color)
{
    ASSERT_VALID(this);

    m_textColor = color;
    Invalidate();
}

void CDrawObj::SetTextPosition(DWORD position)
{
    ASSERT_VALID(this);

    m_textPosition = position;
    Invalidate();
}

void CDrawObj::SetTextFontSize(int size)
{
    ASSERT_VALID(this);

    fontHeight = size;

    Invalidate();
}

void CDrawObj::SetTextFontFaceName(TCHAR *pszFaceName)
{
    ASSERT_VALID(this);

    _tcscpy(fontFaceName, pszFaceName);

    Invalidate();
}

void CDrawObj::SetFillColor(COLORREF color)
{
    ASSERT_VALID(this);

    m_logbrush.lbColor = color;
    Invalidate();
}

COLORREF CDrawObj::GetLineColor()
{
    return m_logpen.lopnColor;
}

COLORREF CDrawObj::GetTextColor()
{
    return m_textColor;
}

COLORREF CDrawObj::GetFillColor()
{
    return m_logbrush.lbColor;
}

BOOL CDrawObj::IsNoFill()
{
    return !m_bBrush;
}

BOOL CDrawObj::IsNoOutline()
{
    return !m_bPen;
}

BOOL CDrawObj::SetNoFill( BOOL bSet /*= TRUE*/ )
{
    BOOL bPrev = !m_bBrush;
    m_bBrush = !bSet;
    return bPrev;
}

BOOL CDrawObj::SetNoOutline( BOOL bSet /*= TRUE*/ )
{
    BOOL bPrev = !m_bPen;
    m_bPen = !bSet;
    if( bSet )
    {
        m_logpen.lopnWidth.x = 0;
        m_logpen.lopnWidth.y = 0;
    }
    else
    {
        if( m_logpen.lopnWidth.x == 0
            || m_logpen.lopnWidth.y == 0
            )
            m_logpen.lopnWidth.x
                = m_logpen.lopnWidth.y
                = 1;
    }
    return bPrev;
}

// **** 죽는 지점.
void CDrawObj::Remove()
{
    if (m_pRoiData)
        delete m_pRoiData;
    m_pRoiData = NULL;
    delete this;
}

void CDrawObj::RemovingChild(CDrawObj *pChild)
{
}

void CDrawObj::Draw(CDC* pDC)
{

    if ((m_dwType&m_dwFilter) == 0)
        return;
    if (m_pRoiData == NULL)
        return;
    int len =  _tcslen(m_pRoiData->m_sName);
    if (len == 0)
        return;
    if (m_pParent == NULL) {

        CFont *pFont = new CFont();
        pDC->SetTextColor(m_textColor);
        pDC->SetBkMode(TRANSPARENT);

        LOGFONT m_logfont;
        //CFont m_font;

        //m_logfont.lfHeight = 16; // 폰트의 너비와높이.
        m_logfont.lfHeight = fontHeight;
        m_logfont.lfWeight = 0;
        m_logfont.lfEscapement = 0; // 기울어진 각도.
        m_logfont.lfOrientation = 0;
        m_logfont.lfWeight = FW_NORMAL; // 굵기.
        m_logfont.lfItalic = FALSE; // 기울임꼴과밑줄.
        m_logfont.lfUnderline = FALSE;
        m_logfont.lfStrikeOut = FALSE;  //취소선과 문자셋트.
        m_logfont.lfCharSet = DEFAULT_CHARSET;
        m_logfont.lfOutPrecision = OUT_CHARACTER_PRECIS;  //출력정확도.
        m_logfont.lfClipPrecision = CLIP_CHARACTER_PRECIS; //클립핑정확도.
        m_logfont.lfQuality = DEFAULT_QUALITY; //출력의질과시간.
        m_logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
        _tcscpy(m_logfont.lfFaceName, fontFaceName);
        pFont->CreateFontIndirect(&m_logfont);


        //CFont font;
        //font.CreateFont(// rect.Height() / 3,  //16:9
        //	16,
        //	0,
        //	0,
        //	0,
        //	FW_NORMAL, //FW_BOLD,
        //	0,
        //	0,
        //	0,
        //	DEFAULT_CHARSET,
        //	OUT_CHARACTER_PRECIS,
        //	CLIP_CHARACTER_PRECIS,
        //	DEFAULT_QUALITY,
        //	FF_DONTCARE,
        //	_T("Arial"));
        //font.Detach();

        CFont* pOldFont;
        pOldFont = pDC->SelectObject(pFont);

        CRect rect = m_position;
        rect.NormalizeRect();
        int textx = (int)(rect.left * m_ZoomFactor);
        int texty = (int)(rect.bottom * m_ZoomFactor);

        if (m_nShape == line) {
            textx = (int)(rect.CenterPoint().x * m_ZoomFactor);
            texty = (int)(rect.CenterPoint().y * m_ZoomFactor);
        }

        if ((m_textPosition&DT_VCENTER)==DT_VCENTER)
            texty -= (int)((double)(rect.Height()/2) * m_ZoomFactor);

        rect.left = (LONG)(rect.left * m_ZoomFactor);
        rect.right = (LONG)(rect.right * m_ZoomFactor);
        rect.top = (LONG)(rect.top * m_ZoomFactor);
        rect.bottom = (LONG)(rect.bottom * m_ZoomFactor);
        rect.right = rect.left;// +(16 * len); // n개 글자.
        rect.bottom = rect.top;// +16;

        rect += m_textOffset;

        if (rect.left == rect.right || rect.top == rect.bottom)
            pDC->TextOut(textx,texty, m_pRoiData->m_sName, len );
        else
        {
            if ((m_textPosition&DT_BOTTOM)==DT_BOTTOM) {
                //rect.top += 16;
                //rect.bottom += 16;
            } else if ((m_textPosition&(DT_VCENTER|DT_BOTTOM))==DT_TOP) {
                //rect.top -= 16;
                //rect.bottom -= 16;
            }
            pDC->DrawText(m_pRoiData->m_sName, &rect, m_textPosition);
        }

        pDC->SelectObject(pOldFont);

        pFont->DeleteObject();
        delete pFont;
    }
    //g_cLog->AddLog(_T("***")+CString(m_pRoiData->m_sName), _LOG_LIST_SYS);

}

void CDrawObj::DrawTracker(CDC* pDC, TrackerState state)
{
    ASSERT_VALID(this);

    switch (state)
    {
    case normal:
        break;

    case selected:
    case active:
        {
            int nHandleCount = GetHandleCount();
            if (nHandleCount == 1) {
                int w = m_position.Width();
                int h = m_position.Height();
                CPoint handle = GetHandle(0);
                handle.x = (LONG)((double)handle.x * m_ZoomFactor);
                handle.y = (LONG)((double)handle.y * m_ZoomFactor);
                pDC->PatBlt(handle.x - w/2, handle.y - h/2, w, h, DSTINVERT);
                break;
            }
            for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
            {
                CPoint handle = GetHandle(nHandle);
                handle.x = (LONG)((double)handle.x * m_ZoomFactor);
                handle.y = (LONG)((double)handle.y * m_ZoomFactor);

                pDC->PatBlt(handle.x - 3, handle.y - 3, 7, 7, DSTINVERT);
            }
        }
        break;
    }
}

// position is in logical
void CDrawObj::MoveTo(const CRect& position, CDrawView* pView)
{
    ASSERT_VALID(this);

    if (position == m_position)
        return;

    if (pView == NULL)
    {
        Invalidate();
        m_position = position;
        Invalidate();
    }
    else
    {
        pView->InvalObj(this);
        m_position = position;
        pView->InvalObj(this);
    }

}

void CDrawObj::OnEditProperties(CDrawView* pView)
{
    CDrawObj* pObj = this;
    if (this->m_pParent != NULL) // 패턴ROI Object는 자식으로 패턴영역 및 포인트 좌표가 있다.
        pObj = this->m_pParent;

    AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_EDIT, 0, (LPARAM)pObj);
}

void CDrawObj::OnOpen(CDrawView* pView )
{
    OnEditProperties(pView);
}

// Note: if bSelected, hit-codes start at one for the top-left
// and increment clockwise, 0 means no hit.
// If !bSelected, 0 = no hit, 1 = hit (anywhere)

// point is in logical coordinates
int CDrawObj::HitTest(CPoint point, CDrawView* pView, BOOL bSelected)
{
    ASSERT_VALID(this);
    ASSERT(pView != NULL);

    if (bSelected)
    {
        int nHandleCount = GetHandleCount();
        for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
        {
            // GetHandleRect returns in logical coords
            CRect rc = GetHandleRect(nHandle,pView);
            if (point.x >= rc.left && point.x < rc.right &&
                point.y >= rc.top && point.y < rc.bottom)
                return nHandle;
        }
    }
    else
    {
        int w = m_position.Width();
        int h = m_position.Height();

        CRect rect = m_position;

        rect.left -= (LONG)(((double)w/2) * (1.0/m_ZoomFactor));
        rect.top -= (LONG)(((double)h / 2) * (1.0 / m_ZoomFactor));
        rect.right += (LONG)(((double)w / 2) * (1.0 / m_ZoomFactor));
        rect.bottom += (LONG)(((double)h / 2) * (1.0 / m_ZoomFactor));

        if (m_bSizeFix) {
            if (point.x >= rect.left && point.x <= rect.right &&
                point.y >= rect.top && point.y <= rect.bottom)
                return 1;
        } else {
            if (point.x >= m_position.left && point.x < m_position.right &&
                point.y >= m_position.top && point.y < m_position.bottom)
                return 1;
        }
    }
    return 0;
}

// 라인과 원의 교차 test
BOOL CDrawObj::circleLineIntersect(float x1, float y1, float x2, float y2, float cx, float cy, float cr)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float a = dx * dx + dy * dy;
    float b = 2 * (dx * (x1 - cx) + dy * (y1 - cy));
    float c = cx * cx + cy * cy;
    c += x1 * x1 + y1 * y1;
    c -= 2 * (cx * x1 + cy * y1);
    c -= cr * cr;
    float bb4ac = b * b - 4 * a * c;

    if (bb4ac < 0){
        return FALSE;    // No collision
    }
    else{
        return TRUE;      //Collision
    }
}

BOOL CDrawObj::Intersects(float cx, float cy, float cr)
{
    BOOL ret = FALSE;
    CRect fixed = m_position;

    switch (m_nShape)
    {
    case line:
        ret |= circleLineIntersect((float)fixed.left, (float)fixed.top, (float)fixed.right, (float)fixed.bottom, cx, cy, cr);
        break;

    case cross:
    case point:
        ret |= circleLineIntersect((float)fixed.left, (float)fixed.top, (float)fixed.right, (float)fixed.bottom, cx, cy, cr);
        break;
    case roi:
    case rectangle:
    case roundRectangle:
    case ellipse:
    default:
        fixed.NormalizeRect();
        ret |= circleLineIntersect((float)fixed.left, (float)fixed.top, (float)fixed.right, (float)fixed.top, cx, cy, cr);
        ret |= circleLineIntersect((float)fixed.left, (float)fixed.bottom, (float)fixed.right, (float)fixed.bottom, cx, cy, cr);
        ret |= circleLineIntersect((float)fixed.left, (float)fixed.top, (float)fixed.left, (float)fixed.bottom, cx, cy, cr);
        ret |= circleLineIntersect((float)fixed.right, (float)fixed.top, (float)fixed.right, (float)fixed.bottom, cx, cy, cr);
        break;

    }

    return ret;
}

// box 와 box의 교차 test
// rect must be in logical coordinates
BOOL CDrawObj::Intersects(const CRect& rect)
{
    ASSERT_VALID(this);

    CRect fixed = m_position;
    fixed.NormalizeRect();
    CRect rectT = rect;
    rectT.NormalizeRect();

    BOOL rst = !(rectT & fixed).IsRectEmpty();
    return rst;
}

int CDrawObj::GetHandleCount()
{
    ASSERT_VALID(this);
    if (m_nShape == cross)
        return 1;
    return 8;
}

// returns logical coords of center of handle
CPoint CDrawObj::GetHandle(int nHandle)
{
    ASSERT_VALID(this);
    int x, y, xCenter, yCenter;

    // this gets the center regardless of left/right and top/bottom ordering
    xCenter = m_position.left + m_position.Width() / 2;
    yCenter = m_position.top + m_position.Height() / 2;

    switch (nHandle)
    {
    default:
        x = xCenter;
        y = yCenter;
        break;
        //ASSERT(FALSE);

    case 1:
        x = m_position.left;
        y = m_position.top;
        break;

    case 2:
        x = xCenter;
        y = m_position.top;
        break;

    case 3:
        x = m_position.right;
        y = m_position.top;
        break;

    case 4:
        x = m_position.right;
        y = yCenter;
        break;

    case 5:
        x = m_position.right;
        y = m_position.bottom;
        break;

    case 6:
        x = xCenter;
        y = m_position.bottom;
        break;

    case 7:
        x = m_position.left;
        y = m_position.bottom;
        break;

    case 8:
        x = m_position.left;
        y = yCenter;
        break;
    }

    return CPoint(x, y);
}

// return rectange of handle in logical coords
CRect CDrawObj::GetHandleRect(int nHandleID, CDrawView* pView)
{
    ASSERT_VALID(this);
    ASSERT(pView != NULL);

    CRect rect;
    // get the center of the handle in logical coords
    CPoint point = GetHandle(nHandleID);
    // convert to client/device coords
    pView->DocToClient(point);
    // return CRect of handle in device coords
    rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);
    pView->ClientToDoc(rect);

    return rect;
}

HCURSOR CDrawObj::GetHandleCursor(int nHandle)
{
    ASSERT_VALID(this);

    if (m_bSizeFix)
        return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    LPCTSTR id;
    switch (nHandle)
    {
    default:
        ASSERT(FALSE);

    case 1:
    case 5:
        id = IDC_SIZENWSE;
        break;

    case 2:
    case 6:
        id = IDC_SIZENS;
        break;

    case 3:
    case 7:
        id = IDC_SIZENESW;
        break;

    case 4:
    case 8:
        id = IDC_SIZEWE;
        break;
    }

    return AfxGetApp()->LoadStandardCursor(id);
}

// point must be in logical
void CDrawObj::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
    ASSERT_VALID(this);

    CRect position = m_position;
    switch (nHandle)
    {
    default:
        ASSERT(FALSE);

    case 1:
        position.left = point.x;
        position.top = point.y;
        break;

    case 2:
        position.top = point.y;
        break;

    case 3:
        position.right = point.x;
        position.top = point.y;
        break;

    case 4:
        position.right = point.x;
        break;

    case 5:
        position.right = point.x;
        position.bottom = point.y;
        break;

    case 6:
        position.bottom = point.y;
        break;

    case 7:
        position.left = point.x;
        position.bottom = point.y;
        break;

    case 8:
        position.left = point.x;
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // 20110804 - m_position 이 0 이 되면 선택이 안되는 Bug Fix를 위해 최소 2x2 이상이 되도록 강제함.
    int w = abs(position.Width());
    int h = abs(position.Height());
    if (w<MIN_SIZE) position.left = m_position.left, position.right = m_position.right;
    if (h<MIN_SIZE) position.top = m_position.top, position.bottom = m_position.bottom;
    /////////////////////////////////////////////////////////////////////////////////////////////////

    MoveTo(position, pView);
}

CRect CDrawObj::GetRect()
{
    CRect rect = m_position;

    rect.left	= abs(rect.left);
    rect.top	= abs(rect.top);
    rect.right	= abs(rect.right);
    rect.bottom = abs(rect.bottom);

    //if (m_nShape != line)
        rect.NormalizeRect();

    return rect;
}

void CDrawObj::Invalidate()
{
    ASSERT_VALID(this);
    //m_pDocument->UpdateAllViews(NULL, HINT_UPDATE_DRAWOBJ, this);
}


CDrawObj* CDrawObj::Clone(CDrawDoc* pDoc)
{
    ASSERT_VALID(this);

    if (m_pParent) return NULL; // Parent가 명시되어 있으면 Clone동작은 Parent에서 할수 있도록 한다.

    CDrawObj* pClone = new CDrawObj(m_position);
    *pClone = *this;
    //pClone->m_pRoiData->nIndex = -1; // index를 -1로 clear함으로써 새로운 index를 부여받을수 있다.

    ASSERT_VALID(pClone);

    if (pDoc != NULL) {
        pDoc->Add(pClone);

        //POSITION pos = pDoc->GetFirstViewPosition();
        CDrawView* pView = pDoc->m_pActiveMainView;//(CDrawView*)pDoc->GetNextView(pos);
        pView->Select(pClone, TRUE);
    }
    return pClone;
}

void CDrawObj::Select()
{
    m_bSelected = TRUE;
}

void CDrawObj::Deselect()
{
    m_bSelected = FALSE;
}

void CDrawObj::SetZoomFactor(double zoomFactor)
{
    m_ZoomFactor = zoomFactor;
}

#ifdef _DEBUG
void CDrawObj::AssertValid()
{
    ASSERT(m_position.left <= m_position.right);
    ASSERT(m_position.bottom <= m_position.top);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CDrawRect

//IMPLEMENT_SERIAL(CDrawRect, CDrawObj, 0)

//CDrawRect::CDrawRect()
//{
//}
CDrawRect::~CDrawRect()
{
    //TRACE(_T("Delete : CDrawRect\n"));
    if (m_pParent) {
        m_pParent->RemovingChild(this);
        m_pParent = NULL;
    }
}

CDrawRect::CDrawRect(const CRect& position)
    : CDrawObj(position)
{
    ASSERT_VALID(this);

    m_nShape = rectangle;
    m_roundness.x = 16;
    m_roundness.y = 16;

    m_pRoiData->SetRect(position);
}

CDrawRect& CDrawRect::operator=(const CDrawRect& myObj)
{
    if (this == &myObj)
        return *this;
    *(CDrawObj*)this =  *(CDrawObj*)&myObj;
    m_roundness = myObj.m_roundness;

    return *this;
}

void CDrawRect::Draw(CDC* pDC)
{
    ASSERT_VALID(this);
    if ((m_dwType&m_dwFilter) == 0)
        return;

    CBrush brush;
    if (!brush.CreateBrushIndirect(&m_logbrush))
        return;
    CPen pen;

    (m_bSelected == TRUE) ? pen.CreatePen(PS_SOLID, 2, m_selcolor) : pen.CreatePenIndirect(&m_logpen);

    CBrush* pOldBrush;
    CPen* pOldPen;

    if (m_bBrush)
        pOldBrush = pDC->SelectObject(&brush);
    else
        pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    if (m_bPen)
        pOldPen = pDC->SelectObject(&pen);
    else
        pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

    CRect rect = m_position;
    if (m_nShape != line)
        rect.NormalizeRect();
    CPoint roundPt = m_roundness;
    //TRACE("RECT = %d,%d,%d,%d\n", rect.left, rect.top, rect.right, rect.bottom);

    int w = rect.Width();
    int h = rect.Height();
    rect.left = (LONG)((double)rect.left * m_ZoomFactor);
    rect.right = (LONG)((double)rect.right * m_ZoomFactor);
    rect.top = (LONG)((double)rect.top * m_ZoomFactor);
    rect.bottom = (LONG)((double)rect.bottom * m_ZoomFactor);

    if (m_bSizeFix) {
        rect.left -= (LONG)(((double)w/2) * (1.0-m_ZoomFactor));
        rect.top -= (LONG)(((double)h / 2) * (1.0 - m_ZoomFactor));
        rect.right = rect.left + w;
        rect.bottom = rect.top + h;
    }


    roundPt.x *= (LONG)((double)m_ZoomFactor);
    roundPt.y *= (LONG)((double)m_ZoomFactor);

	RECT rc;
	switch (m_nShape)
	{
	case roi:
	case rectangle:
		pDC->Rectangle(rect);
		//TRACE("    =>RECT = %d,%d,%d,%d\n", rect.left, rect.top, rect.right, rect.bottom);
		break;

	case roundRectangle:
		pDC->RoundRect(rect, roundPt);
		break;

	case ellipse:
		pDC->Ellipse(rect);
		break;

	case line:
		if (rect.top > rect.bottom)
		{
			rect.top -= m_logpen.lopnWidth.y / 2;
			rect.bottom += (m_logpen.lopnWidth.y + 1) / 2;
		}
		else
		{
			rect.top += (m_logpen.lopnWidth.y + 1) / 2;
			rect.bottom -= m_logpen.lopnWidth.y / 2;
		}

		if (rect.left > rect.right)
		{
			rect.left -= m_logpen.lopnWidth.x / 2;
			rect.right += (m_logpen.lopnWidth.x + 1) / 2;
		}
		else
		{
			rect.left += (m_logpen.lopnWidth.x + 1) / 2;
			rect.right -= m_logpen.lopnWidth.x / 2;
		}

		pDC->MoveTo(rect.TopLeft());
		pDC->LineTo(rect.BottomRight());
		break;

	case cross:
	case point:

		pDC->MoveTo(rect.left, rect.top + rect.Height() / 2);
		pDC->LineTo(rect.right, rect.top + rect.Height() / 2);

		pDC->MoveTo(rect.left + rect.Width() / 2, rect.top);
		pDC->LineTo(rect.left + rect.Width() / 2, rect.bottom);

		break;

		//case roi:
	case roipat:
	{
		int width = 7;
		int height = 7;

		pDC->MoveTo(rect.left, rect.top + height);
		pDC->LineTo(rect.left, rect.top);
		pDC->LineTo(rect.left + width, rect.top);

		pDC->MoveTo(rect.right - width, rect.top);
		pDC->LineTo(rect.right, rect.top);
		pDC->LineTo(rect.right, rect.top + height);

		pDC->MoveTo(rect.left, rect.bottom - height);
		pDC->LineTo(rect.left, rect.bottom);
		pDC->LineTo(rect.left + width, rect.bottom);

		pDC->MoveTo(rect.right - width, rect.bottom);
		pDC->LineTo(rect.right, rect.bottom);
		pDC->LineTo(rect.right, rect.bottom - height);
	}

	break;

	case text:
		{
		CFont *pFont = new CFont();
		pDC->SetTextColor(m_textColor);
		pDC->SetBkMode(TRANSPARENT);

		LOGFONT m_logfont;

		m_logfont.lfHeight = fontHeight;
		m_logfont.lfWeight = 0;
		m_logfont.lfEscapement = 0; // 기울어진 각도.
		m_logfont.lfOrientation = 0;
		m_logfont.lfWeight = FW_NORMAL; // 굵기.
		m_logfont.lfItalic = FALSE; // 기울임꼴과밑줄.
		m_logfont.lfUnderline = FALSE;
		m_logfont.lfStrikeOut = FALSE;  //취소선과 문자셋트.
		m_logfont.lfCharSet = DEFAULT_CHARSET;
		m_logfont.lfOutPrecision = OUT_CHARACTER_PRECIS;  //출력정확도.
		m_logfont.lfClipPrecision = CLIP_CHARACTER_PRECIS; //클립핑정확도.
		m_logfont.lfQuality = DEFAULT_QUALITY; //출력의질과시간.
		m_logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		_tcscpy(m_logfont.lfFaceName, fontFaceName);
		pFont->CreateFontIndirect(&m_logfont);

		CFont* pOldFont;
		pOldFont = pDC->SelectObject(pFont);

		::SetRect(&rc, rect.left, rect.top, rect.Width(), rect.Height());
		::DrawText(*pDC, m_text, _tcslen(m_text), &rc, DT_LEFT | DT_WORDBREAK);

		pDC->SelectObject(pOldFont);

		pFont->DeleteObject();
		delete pFont;
		}
		break;
    }


    //CRect rect = m_position;
    //rect.NormalizeRect();

    //int w = rect.Width();
    //int h = rect.Height();
    //rect.left = (LONG)((double)rect.left * m_ZoomFactor);
    //rect.right = (LONG)((double)rect.right * m_ZoomFactor);
    //rect.top = (LONG)((double)rect.top * m_ZoomFactor);
    //rect.bottom = (LONG)((double)rect.bottom * m_ZoomFactor);

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);

    CDrawObj::Draw(pDC);
}


int CDrawRect::GetHandleCount()
{
    ASSERT_VALID(this);

    //if (m_bSizeFix)
    //	return 0;
    return m_nShape == line ? 2 :
        CDrawObj::GetHandleCount() + (m_nShape == roundRectangle);
}

// returns center of handle in logical coordinates
CPoint CDrawRect::GetHandle(int nHandle)
{
    ASSERT_VALID(this);

    if (m_nShape == line && nHandle == 2)
        nHandle = 5;
    else if (m_nShape == roundRectangle && nHandle == 9)
    {
        CRect rect = m_position;
        rect.NormalizeRect();
        CPoint point = rect.BottomRight();
        point.x -= m_roundness.x / 2;
        point.y -= m_roundness.y / 2;
        return point;
    }

    return CDrawObj::GetHandle(nHandle);
}

HCURSOR CDrawRect::GetHandleCursor(int nHandle)
{
    ASSERT_VALID(this);

    if (m_nShape == line && nHandle == 2)
        nHandle = 5;
    else if (m_nShape == roundRectangle && nHandle == 9) // nHandle=9는 roundRect의 모서리 각도를 조정하기위한 tracker이다.
        return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
    return CDrawObj::GetHandleCursor(nHandle);
}

// point is in logical coordinates
void CDrawRect::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
    ASSERT_VALID(this);

    if (m_nShape == line && nHandle == 2)
        nHandle = 5;
    else if (m_nShape == roundRectangle && nHandle == 9)
    {
        CRect rect = m_position;
        rect.NormalizeRect();
        if (point.x > rect.right - 1)
            point.x = rect.right - 1;
        else if (point.x < rect.left + rect.Width() / 2)
            point.x = rect.left + rect.Width() / 2;
        if (point.y > rect.bottom - 1)
            point.y = rect.bottom - 1;
        else if (point.y < rect.top + rect.Height() / 2)
            point.y = rect.top + rect.Height() / 2;
        m_roundness.x = 2 * (rect.right - point.x);
        m_roundness.y = 2 * (rect.bottom - point.y);
        if (pView == NULL)
            Invalidate();
        else
            pView->InvalObj(this);
        return;
    }

    CDrawObj::MoveHandleTo(nHandle, point, pView);
}

// rect must be in logical coordinates
BOOL CDrawRect::Intersects(const CRect& rect)
{
    ASSERT_VALID(this);

    CRect rectT = rect;
    rectT.NormalizeRect();

    CRect fixed = m_position;
    fixed.NormalizeRect();

    CRect inter;
    BOOL rst;

    if (m_bSelected) {
        // 선택되어 있는 Object는 사각형내에만 클릭하면됨 - jlyoon 2014.8.30
        rst = inter.IntersectRect(rectT, fixed);
        if (rst)
            return rst;
    }

    if (m_nShape == CDrawObj::point || m_nShape == CDrawObj::cross) { // 포인트는 중심점에 클릭해야함.
        CPoint pt = fixed.CenterPoint();
        fixed = CRect(pt.x-10, pt.y-10, pt.x+10, pt.y+10);
        rst = inter.IntersectRect(rectT, fixed);
    } else { // 사각형은 테두리부근을 클릭해야함.
        rst = inter.IntersectRect(rectT, fixed);
        if (rst && inter == rectT)
            rst = FALSE;
    }
    return rst;

/*
    CRgn rgn;
    switch (m_nShape)
    {
    case rectangle:
        return TRUE;

    case roundRectangle:
        rgn.CreateRoundRectRgn(fixed.left, fixed.top, fixed.right, fixed.bottom,
            m_roundness.x, m_roundness.y);
        break;

    case ellipse:
        rgn.CreateEllipticRgnIndirect(fixed);
        break;

    case line:
        {
            int x = (m_logpen.lopnWidth.x + 5) / 2;
            int y = (m_logpen.lopnWidth.y + 5) / 2;
            POINT points[4];
            points[0].x = fixed.left;
            points[0].y = fixed.top;
            points[1].x = fixed.left;
            points[1].y = fixed.top;
            points[2].x = fixed.right;
            points[2].y = fixed.bottom;
            points[3].x = fixed.right;
            points[3].y = fixed.bottom;

            if (fixed.left < fixed.right)
            {
                points[0].x -= x;
                points[1].x += x;
                points[2].x += x;
                points[3].x -= x;
            }
            else
            {
                points[0].x += x;
                points[1].x -= x;
                points[2].x -= x;
                points[3].x += x;
            }

            if (fixed.top < fixed.bottom)
            {
                points[0].y -= y;
                points[1].y += y;
                points[2].y += y;
                points[3].y -= y;
            }
            else
            {
                points[0].y += y;
                points[1].y -= y;
                points[2].y -= y;
                points[3].y += y;
            }
            rgn.CreatePolygonRgn(points, 4, ALTERNATE);
        }
        break;
    case cross:
        return TRUE;
    case point:
        return TRUE;
    case roi:
        return TRUE;
    case roipat:
        return TRUE;
    }
    return rgn.RectInRegion(fixed);
*/
}

CDrawObj* CDrawRect::Clone(CDrawDoc* pDoc)
{
    ASSERT_VALID(this);

    //if (m_pParent) return NULL; // 상위에서 수행.

    CDrawRect* pClone = new CDrawRect(m_position);
    //*(CDrawObj*)pClone = *(CDrawObj*)this;
    *pClone = *this;
    //pClone->m_pRoiData->nIndex = -1; // index를 -1로 clear함으로써 새로운 index를 부여받을수 있다.

    ASSERT_VALID(pClone);

    if (pDoc != NULL) {
        pDoc->Add(pClone);

        //POSITION pos = pDoc->GetFirstViewPosition();
        CDrawView* pView = pDoc->m_pActiveMainView;;//(CDrawView*)pDoc->GetNextView(pos);
        pView->Select(pClone, TRUE);
    }
    ASSERT_VALID(pClone);
    //TRACE("pObj=%d,%d,%d,%d pClone=%d,%d,%d,%d\n",
    //	m_position.left,m_position.right,m_position.top,m_position.bottom,	pClone->m_position.left,pClone->m_position.right,pClone->m_position.top,m_position.bottom);

    return pClone;
}

void CDrawRect::Remove()
{
    #if 0
    if (this->IsKindOf(RUNTIME_CLASS( CDrawRoiPat ))) { // m_pCrossObj 그리고 m_pPatObj 보다 먼저 지워지면 안되므로 편법동원.
        CDrawRoiPat *p = (CDrawRoiPat *)this;
        if (p->m_pCrossObj != NULL || p->m_pPatObj != NULL) {
            p->m_bZombie = TRUE;
            return;
        }
    }
    #endif

    if (m_pParent) {
        //m_pParent->RemovingChild(this);
        m_pParent = NULL;
        ///delete this;
        return;
    }

    CDrawObj::Remove();
}

////////////////////////////////////////////////////////////////////////////
// CDrawPoly

//IMPLEMENT_SERIAL(CDrawPoly, CDrawObj, 0)

//CDrawPoly::CDrawPoly()
//{
//	m_points = NULL;
//	m_nPoints = 0;
//	m_nAllocPoints = 0;
//}

CDrawPoly::CDrawPoly(const CRect& position) // CDrawPoly에서 position은 의미없는 값이다.
    : CDrawObj(position)
{
    m_points = NULL;
    m_nPoints = 0;
    m_nAllocPoints = 0;
    m_bPen = TRUE;
    m_bBrush = FALSE;
}

CDrawPoly& CDrawPoly::operator=(const CDrawPoly& myObj)
{
    if (this == &myObj)
        return *this;

    *(CDrawObj*)this =  *(CDrawObj*)&myObj;
    m_nPoints = myObj.m_nPoints;
    m_nAllocPoints = myObj.m_nAllocPoints;
    m_points = new CPoint[m_nAllocPoints];
    memcpy(m_points, myObj.m_points, sizeof(CPoint) * m_nPoints);
    m_pDrawObj = myObj.m_pDrawObj;

    return *this;
}

CDrawPoly::~CDrawPoly()
{
    if (m_points != NULL)
        delete[] m_points;
}

void CDrawPoly::Draw(CDC* pDC)
{
    ASSERT_VALID(this);
    if ((m_dwType&m_dwFilter) == 0)
        return;

    CBrush brush;
    if (!brush.CreateBrushIndirect(&m_logbrush))
        return;
    CPen pen;

    (m_bSelected == TRUE) ? pen.CreatePen(PS_SOLID, 2, m_selcolor) : pen.CreatePenIndirect(&m_logpen);

    CBrush* pOldBrush;
    CPen* pOldPen;

    if (m_bBrush)
        pOldBrush = pDC->SelectObject(&brush);
    else
        pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);

    if (m_bPen)
        pOldPen = pDC->SelectObject(&pen);
    else
        pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);


    CPoint *points = new CPoint[m_nAllocPoints];
    memcpy(points, m_points, sizeof(CPoint) * m_nPoints);
    for (int i = 0; i < m_nPoints; i += 1)
    {
        points[i].x = (LONG)(points[i].x * (double)m_ZoomFactor);
        points[i].y = (LONG)(points[i].y * (double)m_ZoomFactor);
    }
    pDC->Polygon(points, m_nPoints);
    delete points;

    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);

    CDrawObj::Draw(pDC);
}

// position must be in logical coordinates
void CDrawPoly::MoveTo(const CRect& position, CDrawView* pView)
{
    ASSERT_VALID(this);
    if (position == m_position)
        return;

    if (pView == NULL)
        Invalidate();
    else
        pView->InvalObj(this);

    for (int i = 0; i < m_nPoints; i += 1)
    {
        m_points[i].x += position.left - m_position.left;
        m_points[i].y += position.top - m_position.top;
    }

    m_position = position;

    if (pView == NULL)
        Invalidate();
    else
        pView->InvalObj(this);
}

int CDrawPoly::GetHandleCount()
{
    return m_nPoints;
}

CPoint CDrawPoly::GetHandle(int nHandle)
{
    ASSERT_VALID(this);

    ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
    return m_points[nHandle - 1];
}

HCURSOR CDrawPoly::GetHandleCursor(int )
{
    return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

// point is in logical coordinates
void CDrawPoly::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
    ASSERT_VALID(this);
    ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
    if (m_points[nHandle - 1] == point)
        return;

    m_points[nHandle - 1] = point;
    RecalcBounds(pView);

    if (pView == NULL)
        Invalidate();
    else
        pView->InvalObj(this);
}

// rect must be in logical coordinates
BOOL CDrawPoly::Intersects(const CRect& rect)
{
    ASSERT_VALID(this);
    CRgn rgn;
    rgn.CreatePolygonRgn(m_points, m_nPoints, ALTERNATE);
    return rgn.RectInRegion(rect);
}

CDrawObj* CDrawPoly::Clone(CDrawDoc* pDoc)
{
    ASSERT_VALID(this);

    CDrawPoly* pClone = new CDrawPoly(m_position);
    //*(CDrawObj*)pClone = *(CDrawObj*)this;
    *pClone = *this;
    pClone->m_points = new CPoint[m_nAllocPoints];
    memcpy(pClone->m_points, m_points, sizeof(CPoint) * m_nPoints);
    pClone->m_nAllocPoints = m_nAllocPoints;
    pClone->m_nPoints = m_nPoints;
    //pClone->m_pRoiData->nIndex = -1; // index를 -1로 clear함으로써 새로운 index를 부여받을수 있다.
    ASSERT_VALID(pClone);

    if (pDoc != NULL) {
        pDoc->Add(pClone);

        //POSITION pos = pDoc->GetFirstViewPosition();
        CDrawView* pView = pDoc->m_pActiveMainView;;//(CDrawView*)pDoc->GetNextView(pos);
        pView->Select(pClone, TRUE);
    }
    ASSERT_VALID(pClone);
    return pClone;
}
// point is in logical coordinates
void CDrawPoly::AddPoint(const CPoint& point, CDrawView* pView)
{
    ASSERT_VALID(this);
    if (m_nPoints == m_nAllocPoints)
    {
        CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
        if (m_points != NULL)
        {
            memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
            delete[] m_points;
        }
        m_points = newPoints;
        m_nAllocPoints += 10;
    }

    if (m_nPoints == 0 || m_points[m_nPoints - 1] != point)
    {
        m_points[m_nPoints++] = point;
        if (!RecalcBounds(pView))
        {
            if (pView == NULL)
                Invalidate();
            else
                pView->InvalObj(this);
        }
    }
}

BOOL CDrawPoly::RecalcBounds(CDrawView* pView)
{
    ASSERT_VALID(this);

    if (m_nPoints == 0)
        return FALSE;

    CRect bounds(m_points[0], CSize(0, 0));
    for (int i = 1; i < m_nPoints; ++i)
    {
        if (m_points[i].x < bounds.left)
            bounds.left = m_points[i].x;
        if (m_points[i].x > bounds.right)
            bounds.right = m_points[i].x;
        if (m_points[i].y < bounds.top)
            bounds.top = m_points[i].y;
        if (m_points[i].y > bounds.bottom)
            bounds.bottom = m_points[i].y;
    }

    if (bounds == m_position)
        return FALSE;

    if (pView == NULL)
        Invalidate();
    else
        pView->InvalObj(this);

    m_position = bounds;

    if (pView == NULL)
        Invalidate();
    else
        pView->InvalObj(this);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
// CDrawRoiPat

//IMPLEMENT_SERIAL(CDrawRoiPat, CDrawObj, 0)

//CDrawRoiPat::CDrawRoiPat() : m_bIsFirst(TRUE), m_bZombie(FALSE)
//{
//	m_pPatObj = NULL;
//	m_pCrossObj = NULL;
//	m_pParent = NULL;
//}

CDrawRoiPat::CDrawRoiPat(const CRect& position)
    : CDrawRect(position)
{
    ASSERT_VALID(this);

    m_bZombie = FALSE;
    m_nShape = CDrawObj::roipat;
    m_pParent = NULL;

    CRect rect = position;
    rect.NormalizeRect();

    m_pPatObj = new CDrawRect(rect);
    m_pPatObj->m_nShape = CDrawRect::rectangle;
    m_pPatObj->m_pParent = this;

    int w = rect.Width() / 2;
    int h = rect.Height() / 2;
    rect.DeflateRect(w/2, h/2);
    m_pPatObj->m_position = rect;

    m_pCrossObj = new CDrawRect(rect);
    m_pCrossObj->m_nShape = CDrawRect::cross;
    m_pCrossObj->m_bSizeFix = TRUE;
    m_pCrossObj->m_pParent = this;

    CPoint c = rect.CenterPoint();
    m_pCrossObj->m_position = CRect(c.x-5, c.y-5, c.x+5, c.y+5);

    m_bIsFirst = TRUE;
}

CDrawRoiPat& CDrawRoiPat::operator=(const CDrawRoiPat& myObj)
{
    if (this == &myObj)
        return *this;

    *(CDrawObj*)this = *(CDrawObj*)&myObj;
    m_bIsFirst = myObj.m_bIsFirst;
    m_bZombie = myObj.m_bZombie;

    *m_pPatObj = *(myObj.m_pPatObj);
    m_pPatObj->m_pParent = this;
    *m_pCrossObj = *(myObj.m_pCrossObj);
    m_pCrossObj->m_pParent = this;

    return *this;
}

CDrawRoiPat::~CDrawRoiPat()
{
    //TRACE(_T("Delete : CDrawRoiPat\n"));

    if (m_pPatObj != NULL)
    {
        delete m_pPatObj;
        m_pPatObj = NULL;
    }

    if (m_pCrossObj != NULL)
    {
        delete m_pCrossObj;
        m_pCrossObj = NULL;
    }
}

void CDrawRoiPat::Draw(CDC* pDC)
{
    ASSERT_VALID(this);

    CDrawRect::Draw(pDC);

    if (m_pPatObj)
    {
        m_pPatObj->Draw(pDC);
        m_pPatObj->SetHidden(FALSE); // "선택ROI제외 숨김"을 선택했을때 RoiPat Child를 보이게..
    }

    if (m_pCrossObj) {
        m_pCrossObj->Draw(pDC);
        m_pCrossObj->SetHidden(FALSE);
    }

    CDrawObj::Draw(pDC);
}

// point is in logical coordinates
void CDrawRoiPat::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
    ASSERT_VALID(this);

    CRect rect;

    if (!m_bIsFirst) { // 처음 생성할때만 세개의 Object크기가 같이 움직인다.
        CDrawObj::MoveHandleTo(nHandle, point, pView);
        return;
    }

    CDrawObj::MoveHandleTo(nHandle, point, pView);

    rect = m_position;
    rect.NormalizeRect();
    int w = rect.Width() / 2;
    int h = rect.Height() / 2;
    rect.DeflateRect(w/2, h/2);
    if(m_pPatObj) m_pPatObj->m_position = rect;

    rect = m_position;
    rect.NormalizeRect();
    CPoint c = rect.CenterPoint();
    if(m_pCrossObj) m_pCrossObj->m_position = CRect(c.x-5, c.y-5, c.x+5, c.y+5);

    pView->Invalidate();
}

CDrawObj* CDrawRoiPat::Clone(CDrawDoc* pDoc)
{
    ASSERT_VALID(this);

    CDrawRoiPat* pClone = new CDrawRoiPat(m_position);
    ASSERT_VALID(pClone);
    //*(CDrawObj*)pClone = *(CDrawObj*)this;
    *pClone = *this;
    pClone->m_bIsFirst = m_bIsFirst;
    pClone->m_bZombie = m_bZombie;
    //pClone->m_pRoiData->nIndex = -1; // index를 -1로 clear함으로써 새로운 index를 부여받을수 있다.

    if (pDoc != NULL) {
        pDoc->Add(pClone->m_pCrossObj);
        pDoc->Add(pClone->m_pPatObj);
        pDoc->Add(pClone);

        //POSITION pos = pDoc->GetFirstViewPosition();
        CDrawView* pView = pDoc->m_pActiveMainView;;//(CDrawView*)pDoc->GetNextView(pos);

        pView->Deselect(this); // Clone만 선택되어 있는 상태가 된다.
        pView->Deselect(m_pCrossObj);
        pView->Deselect(m_pPatObj);

        pView->Select(pClone->m_pCrossObj);
        pView->Select(pClone->m_pPatObj, TRUE);
        pView->Select(pClone, TRUE);
        pView->InvalObj(pClone);
    }

    ASSERT_VALID(pClone);
    return pClone;
}

void CDrawRoiPat::RemovingChild(CDrawObj *pChild)
{
    if (m_pPatObj == pChild) { // Child가 지워진다는 보고이므로 여기서 Child를 지우면 안된다.
        m_pPatObj = NULL;
    }
    if (m_pCrossObj == pChild) {
        m_pCrossObj = NULL;
    }

    if (m_pCrossObj == NULL && m_pPatObj == NULL)
    {
        //if (m_bZombie) // 이 Object가 먼저 화면에서 없어졌고 자식 Object들이 남아있을때 Setting한다.
        //	delete this; // 2015.10.24 - jlyoon 삭제... 간헐적 Program 죽이는 요인이 되는것 같다.. 원인분석필요
    }
}

void CDrawRoiPat::Select()
{
    ASSERT_VALID(this);

    COLORREF cr = m_selcolor;
    m_bSelected = TRUE;

    if (m_pCrossObj) {
        m_pCrossObj->m_bSelected = TRUE;
        m_pCrossObj->SetLineColor(cr);
    }
    if (m_pPatObj) {
        m_pPatObj->m_bSelected = TRUE;
        m_pPatObj->SetLineColor(cr);
    }
}

void CDrawRoiPat::Deselect()
{
    ASSERT_VALID(this);

    COLORREF cr = m_logpen.lopnColor;
    m_bSelected = FALSE;

    if (m_pCrossObj) {
        m_pCrossObj->m_bSelected = FALSE;
        m_pCrossObj->SetLineColor(cr);
    }
    if (m_pPatObj) {
        m_pPatObj->m_bSelected = FALSE;
        m_pPatObj->SetLineColor(cr);
    }
}
CDrawRoi::CDrawRoi(const CRect& position)
    : CDrawRect(position)
{
    ASSERT_VALID(this);

    m_bZombie = FALSE;
    m_nShape = CDrawObj::roi;
    m_pParent = NULL;

    CRect rect = position;
    rect.NormalizeRect();

    m_pCrossObj = new CDrawRect(rect);
    m_pCrossObj->m_nShape = CDrawRect::cross;
    m_pCrossObj->m_bSizeFix = TRUE;
    m_pCrossObj->m_pParent = this;

    CPoint c = rect.CenterPoint();
    m_pCrossObj->m_position = CRect(c.x-5, c.y-5, c.x+5, c.y+5);

    m_bIsFirst = TRUE;

}

CDrawRoi& CDrawRoi::operator=(const CDrawRoi& myObj)
{
    if (this == &myObj)
        return *this;

    *(CDrawObj*)this = *(CDrawObj*)&myObj;
    m_bIsFirst = myObj.m_bIsFirst;
    m_bZombie = myObj.m_bZombie;

    *m_pCrossObj = *(myObj.m_pCrossObj);
    m_pCrossObj->m_pParent = this;

    return *this;
}

CDrawRoi::~CDrawRoi()
{
    //TRACE(_T("Delete : CDrawRoi\n"));
    if (m_pCrossObj != NULL)
    {
        delete m_pCrossObj;
        m_pCrossObj = NULL;
    }
}

void CDrawRoi::Draw(CDC* pDC)
{
    ASSERT_VALID(this);
    if ((m_dwType&m_dwFilter) == 0)
        return;

    CDrawRect::Draw(pDC);
    if (m_pCrossObj) {
        m_pCrossObj->Draw(pDC);
        m_pCrossObj->SetHidden(FALSE);  // "선택ROI제외 숨김"을 선택했을때 RoiPat Child를 보이게..
    }
    CDrawObj::Draw(pDC);
}

// point is in logical coordinates
void CDrawRoi::MoveHandleTo(int nHandle, CPoint point, CDrawView* pView)
{
    ASSERT_VALID(this);

    CRect rect;

    if (!m_bIsFirst) { // 처음 생성할때만 세개의 Object크기가 같이 움직인다.
        CDrawObj::MoveHandleTo(nHandle, point, pView);
        return;
    }

    CDrawObj::MoveHandleTo(nHandle, point, pView);

    rect = m_position;
    rect.NormalizeRect();
    CPoint c = rect.CenterPoint();
    if(m_pCrossObj) m_pCrossObj->m_position = CRect(c.x-5, c.y-5, c.x+5, c.y+5);

    pView->Invalidate();
}

CDrawObj* CDrawRoi::Clone(CDrawDoc* pDoc)
{
    ASSERT_VALID(this);

    CDrawRoi* pClone = new CDrawRoi(m_position);
    ASSERT_VALID(pClone);
    *pClone = *this;
    pClone->m_bIsFirst = m_bIsFirst;
    pClone->m_bZombie = m_bZombie;
    //pClone->m_pRoiData->nIndex = -1; // index를 -1로 clear함으로써 새로운 index를 부여받을수 있다.

    if (pDoc != NULL) {
        pDoc->Add(pClone->m_pCrossObj);
        pDoc->Add(pClone);

        //POSITION pos = pDoc->GetFirstViewPosition();
        CDrawView* pView = pDoc->m_pActiveMainView;;//(CDrawView*)pDoc->GetNextView(pos);

        pView->Deselect(this); // Clone만 선택되어 있는 상태가 된다.
        pView->Deselect(m_pCrossObj);

        pView->Select(pClone->m_pCrossObj);
        pView->Select(pClone, TRUE);
        pView->InvalObj(pClone);
    }

    ASSERT_VALID(pClone);
    return pClone;
}

void CDrawRoi::RemovingChild(CDrawObj *pChild)
{
    if (m_pCrossObj == pChild) { // Child가 지워진다는 보고이므로 여기서 Child를 지우면 안된다.
        m_pCrossObj = NULL;
    }

    if (m_pCrossObj == NULL)
    {
        //if (m_bZombie) // 이 Object가 먼저 화면에서 없어졌고 자식 Object들이 남아있을때 Setting한다.
        //	delete this;  // 2015.10.24 - jlyoon 삭제... 간헐적 Program 죽이는 요인이 되는것 같다.. 원인분석필요
    }
}

void CDrawRoi::Select()
{
    ASSERT_VALID(this);

    COLORREF cr = m_selcolor;
    m_bSelected = TRUE;

    if (m_pCrossObj) {
        m_pCrossObj->m_bSelected = TRUE;
        m_pCrossObj->SetLineColor(cr);
    }
}

void CDrawRoi::Deselect()
{
    ASSERT_VALID(this);

    COLORREF cr = m_logpen.lopnColor;
    m_bSelected = FALSE;

    if (m_pCrossObj) {
        m_pCrossObj->m_bSelected = FALSE;
        m_pCrossObj->SetLineColor(cr);
    }
}
