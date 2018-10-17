// drawvw.cpp : implementation of the CDrawView class
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2008 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
// modify : jerry1455@gmail.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxpriv.h>

#include "resource.h"

#include "DrawDoc.h"
#include "DrawVw.h"
#include "DrawObj.h"
#include "DrawTool.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawView

IMPLEMENT_DYNCREATE(CDrawView, CScrollViewEx)

BEGIN_MESSAGE_MAP(CDrawView, CScrollViewEx)
    //{{AFX_MSG_MAP(CDrawView)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_WM_SETCURSOR()

    //}}AFX_MSG_MAP
    ON_WM_KEYDOWN()

    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawView construction/destruction

#pragma warning(disable: 4355) // OK here

CDrawView::CDrawView()
    : m_selection(this), m_clone(this)
{
    m_bActive = FALSE;
    m_bEditEnable = FALSE;
    m_nMapMode = MM_TEXT;

    m_tracker.m_rect = CRect(0,0,0,0);
    m_tracker.m_nStyle = 0;
    m_tracker.m_nStyle |= CRectTracker::dottedLine;
    m_tracker.m_nStyle |= CRectTracker::resizeOutside;

    m_bEditEnable = TRUE;
}

CDrawView::~CDrawView()
{
}

BOOL CDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
    ASSERT(cs.style & WS_CHILD);
    if (cs.lpszClass == NULL)
        cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS);
    return TRUE;
}

void CDrawView::OnActivateView(BOOL bActivate, CView* pActiveView,
    CView* pDeactiveView)
{
    CView::OnActivateView(bActivate, pActiveView, pDeactiveView);

    // invalidate selections when active status changes
    //if (m_bActive != bActivate)
    {
        if (bActivate)  // if becoming active update as if active
            m_bActive = bActivate;
        if (!m_selection.IsEmpty()) {
            OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
        }
        m_bActive = bActivate;

    }
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView drawing

//////////////////////////////////////////////////////////////////////////////

BOOL CDrawView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_tracker.SetCursor( pWnd, nHitTest )) return TRUE;
    return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

void CDrawView::InvalObj(CDrawObj* pObj)
{
    //CRect rect = pObj->m_position;
    //InvalidateRect(rect, FALSE);

    CRect rect = pObj->m_position;

    DocToClient(rect);

    if (m_bActive && IsSelected(pObj))
    {
        rect.left -= 4;
        rect.top -= 5;
        rect.right += 5;
        rect.bottom += (4 + 16); // +16은 Text가 있을때를 고려한것이다.
    }
    rect.InflateRect(1, 1); // handles CDrawOleObj objects

    InvalidateRect(rect, FALSE);
}

void CDrawView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
    CScrollView::OnPrepareDC(pDC, pInfo);
}

void CDrawView::ClientToDoc(CPoint& point)
{
    CDrawDoc* pDoc = GetDocument();
    double zoom=pDoc->GetZoomFactor();
    CPoint pt=GetScrollPosition();
    point.x = (LONG)((point.x + pt.x) / zoom);
    point.y = (LONG)((point.y + pt.y) / zoom);
}

void CDrawView::ClientToDoc(CRect& rect)
{
    CDrawDoc* pDoc = GetDocument();
    double zoom=pDoc->GetZoomFactor();
    CPoint pt=GetScrollPosition();
    rect.NormalizeRect();
    rect.left = (LONG)((rect.left + pt.x) / zoom);
    rect.top = (LONG)((rect.top + pt.y) / zoom);
    rect.right = (LONG)((rect.right + pt.x) / zoom);
    rect.bottom = (LONG)((rect.bottom + pt.y) / zoom);
}

void CDrawView::DocToClient(CPoint& point)

{
    CDrawDoc* pDoc = GetDocument();
    double zoom=pDoc->GetZoomFactor();
    CPoint pt=GetScrollPosition();
    point.x = (LONG)(point.x * zoom);
    point.y = (LONG)(point.y * zoom);
    point.x = (point.x - pt.x);
    point.y = (point.y - pt.y);
    //TRACE("DocToClient = %d,%d\n", point.x, point.y);
}

void CDrawView::DocToClient(CRect& rect)
{
    CDrawDoc* pDoc = GetDocument();
    double zoom=pDoc->GetZoomFactor();
    CPoint pt=GetScrollPosition();
    rect.NormalizeRect();
    rect.left = (LONG)((rect.left) * zoom);
    rect.top = (LONG)((rect.top) * zoom);
    rect.right = (LONG)((rect.right) * zoom);
    rect.bottom = (LONG)((rect.bottom) * zoom);
    rect.left = (rect.left - pt.x);
    rect.top = (rect.top - pt.y);
    rect.right = (rect.right - pt.x);
    rect.bottom = (rect.bottom - pt.y);
    //TRACE("DocToClient = %d,%d => %d,%d,%d,%d  (zoom=%f)\n", pt.x, pt.y, rect.left, rect.top, rect.right, rect.bottom, zoom);
}

void CDrawView::SelectAddBefore(CDrawObj* pObj, CDrawObj* pNewObj)
{
    POSITION pos = m_selection.Find(pObj);
    m_selection.InsertBefore(pos, pNewObj);
}

void CDrawView::Select(CDrawObj* pObj, BOOL bAdd)
{
    if (pObj && pObj->IsHidden())
        return;
    if (!bAdd)
    {
        if (pObj) pObj->Deselect();
        OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

        m_selection.RemoveAll();
    }

    if (pObj == NULL || IsSelected(pObj))
        return;

    pObj->Select();
    m_selection.AddTail(pObj);
    InvalObj(pObj);

}

// rect is in device coordinates
void CDrawView::SelectWithinRect(CRect rect, BOOL bAdd)
{
    //CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    CDrawDoc* pDoc = GetDocument();

    if (!bAdd)
        Select(NULL);

    ClientToDoc(rect);

    CDrawObjList* pObList = GetDocument()->GetObjects();
    POSITION posObj = pObList->GetHeadPosition();
    while (posObj != NULL)
    {
        CDrawObj* pObj = pObList->GetNext(posObj);

        if (pDoc->m_nSelectCh != pObj->m_pRoiData->m_nCh)
            continue; // 현재 master image가 아니면..

        if (pObj->Intersects(rect))
        {
            Select(pObj, TRUE);
        }
    }
}

void CDrawView::Deselect(CDrawObj* pObj)
{
    POSITION pos = m_selection.Find(pObj);
    if (pos != NULL)
    {
        pObj->Deselect();
        InvalObj(pObj);
        m_selection.RemoveAt(pos);
    }
}

void CDrawView::CloneSelection()
{
    if (m_selection.GetCount() != 1) // Ctrl+ 는 한개 Object만 지원한다.
        return;

    POSITION pos = m_selection.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_selection.GetNext(pos);
        CDrawObj* pNewCloneObj;
        if (pObj->m_nShape == CDrawObj::roipat) {
            pNewCloneObj = (CDrawRoiPat *)pObj->Clone(pObj->m_pDocument);
        } else if(pObj->m_nShape == CDrawObj::roi){
            pNewCloneObj = (CDrawRoi *)pObj->Clone(pObj->m_pDocument);
        } else if (pObj->m_nShape == CDrawObj::rectangle || /*pObj->m_nShape == CDrawObj::roi ||*/ pObj->m_nShape == CDrawObj::point) {
            pNewCloneObj = (CDrawRect *)pObj->Clone(pObj->m_pDocument);
        } else {
            pNewCloneObj = pObj->Clone(pObj->m_pDocument);
        }

        if (pNewCloneObj == NULL)
            continue;

        // pObj가 가지고 있는 CRoiSet Data를 pNewCloneObj->m_pRoiData으로 가져온다.
        pNewCloneObj->m_pRoiData->sUniqueID = pObj->m_pRoiData->sUniqueID;
        AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_GET_ROISET, 0, (LPARAM)pNewCloneObj->m_pRoiData);

        // copies object and adds it to the document
        CString str;
        str = pNewCloneObj->m_pRoiData->m_sName;
        str += _T(" Clone");
        _tcscpy(pNewCloneObj->m_pRoiData->m_sName, str);
        //pNewCloneObj->m_pRoiData->nIndex = -1;//g_cRecipeData->GetLastNewIndex();
        Deselect(pObj); // Clone만 선택되어 있는 상태가 된다.
    }

    pos = m_selection.GetHeadPosition(); // Pattern Obj는 3개가 선택되어 있을것이므로..
    while (pos != NULL)
    {
        CDrawObj* pObj = m_selection.GetNext(pos);
        if (pObj->m_pParent != NULL) continue; // Parent가 명시되어 있으면 Clone동작은 Parent에서 할수 있도록 한다.

        AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_ADD, BUTTON_PRESS|BUTTON_RELEASE, (LPARAM)pObj);
        break;
    }

}

void CDrawView::Remove(CDrawObj* pObj)
{
    POSITION pos = m_selection.Find(pObj);
    if (pos != NULL)
    {
        if (pObj) pObj->Deselect();
        m_selection.RemoveAt(pos);
    }
}

BOOL CDrawView::IsSelected(const CObject* pDocItem) const
{
    CDrawObj* pDrawObj = (CDrawObj*)pDocItem;
    //if (pDocItem->IsKindOf(RUNTIME_CLASS(CDrawItem)))
    //	pDrawObj = ((CDrawItem*)pDocItem)->m_pDrawObj;
    return m_selection.Find(pDrawObj) != NULL;
}

void CDrawView::OnDraw(CDC* pDC)
{
    if (CDrawTool::c_drawShape == tracker) {
        if ( m_tracker.m_rect.Width()>0 && m_tracker.m_rect.Height()>0 )
            m_tracker.Draw(pDC);
    } else if (CDrawTool::c_drawShape == selection) {
        CSelectTool* pTool = (CSelectTool*)CDrawTool::FindTool(CDrawTool::c_drawShape);

        CRect rect = pTool->m_lastSelectionRect;
        rect += GetScrollPosition();

        pDC->DrawFocusRect(rect);
    }
}

void CDrawView::OnInitialUpdate()
{
    //CSize size = GetDocument()->GetSize();
    //CClientDC dc(NULL);
    //size.cx = MulDiv(size.cx, dc.GetDeviceCaps(LOGPIXELSX), 100);
    //size.cy = MulDiv(size.cy, dc.GetDeviceCaps(LOGPIXELSY), 100);
    //SetScrollSizes(MM_TEXT, size);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView message handlers


void CDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_bActive)
    {
        CScrollViewEx::OnMouseMove(nFlags, point);
        return;
    }

    TCHAR s[256] = { 0, };
    CDrawDoc* pDoc = GetDocument();
    CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);

    // 2011.6.13 SHIFT+LButton으로 대체 <- CTRL+에서 대체 .. Clone동작과 중복되어 불편하네.. 2014.9.16
    //if (nFlags & MK_RBUTTON) { // 마우스 오른쪽 버턴을 누르고 이동하면 moving동작강제실행
    //	if (CDrawTool::c_drawShape == tracker)
    //		m_tracker.m_rect = CRect(0,0,0,0);;//CDrawTool::c_drawShape = moving;
    //	CDrawTool::FindTool(moving)->OnMouseMove(this, nFlags, point);
    //} else
    if ((nFlags & MK_SHIFT) != 0)// && m_selection.GetCount() == 0)
    {
        //if (CDrawTool::c_drawShape == tracker)
        //	m_tracker.m_rect = CRect(0,0,0,0);;//CDrawTool::c_drawShape = moving;
        CDrawTool::FindTool(moving)->OnMouseMove(this, nFlags, point);
    }
    if (!m_bEditEnable)
    {
        CScrollViewEx::OnMouseMove(nFlags, point);
        return;
    }
    if (pTool != NULL) {
        pTool->OnMouseMove(this, nFlags, point);
    }

    if (CDrawTool::c_drawShape == tracker)
    {
        //CSelectTool* pTool = (CSelectTool*)CDrawTool::FindTool(CDrawTool::c_drawShape);
        //int w = pTool->m_lastSelectionRect.Width();
        //int h = pTool->m_lastSelectionRect.Height();
        //w *= (1.0/pDoc->GetZoomFactor());
        //h *= (1.0/pDoc->GetZoomFactor());

        _stprintf(&s[_tcslen(s)],_T(" [Tracker W/H : %.0f/%.0f]"),
            m_tracker.m_rect.Width()*(1/pDoc->GetZoomFactor()),m_tracker.m_rect.Height()*(1/pDoc->GetZoomFactor()));
    }

    CScrollViewEx::OnMouseMove(nFlags, point);
}

void CDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_bActive)
        return;
    CDrawDoc* pDoc = GetDocument();

    CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
    if (pTool != NULL)
        pTool->OnLButtonDown(this, nFlags, point);
    if ((nFlags & MK_SHIFT) != 0)
        CDrawTool::FindTool(moving)->OnLButtonDown(this, nFlags, point);

    // Select Tool일때 CTRL+Object를 선택하면 Object를 Copy하는데 Object를 선택하지 않았을때는 Moving을 할수 있도록..
    if ((nFlags & MK_CONTROL) != 0)
    {
        if (m_selection.GetCount() == 0)
        {
            CDrawTool* pMoveingTool = CDrawTool::FindTool(moving);
            if (pMoveingTool != NULL)
                pMoveingTool->OnLButtonDown(this, nFlags, point);
            return;
        }
    }

    if (CDrawTool::c_drawShape == tracker)
    {
        if (m_tracker.HitTest(point) < 0)
        {
            CRectTracker track;
            if (track.TrackRubberBand(this, point, true)) {
                track.m_rect.NormalizeRect();
                m_tracker.m_rect = track.m_rect;
                SetImageRectSelection(pDoc,&(m_tracker.m_rect));
            } else {
                m_tracker.m_rect = CRect(0,0,0,0);
            }
        } else {
            if (m_tracker.Track(this, point, true)){
                m_tracker.m_rect.NormalizeRect();
                SetImageRectSelection(pDoc,&(m_tracker.m_rect));
            } else {
                m_tracker.m_rect = CRect(0,0,0,0);
            }
        }
        OnUpdate(0,0,0);
    }
}

void CDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (!m_bActive)
        return;
    CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
    if (pTool != NULL)
        pTool->OnLButtonUp(this, nFlags, point);
}

void CDrawView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (!m_bActive || !m_bEditEnable)
        return;
    CDrawTool* pTool = CDrawTool::FindTool(CDrawTool::c_drawShape);
    if (pTool != NULL) {
        if (m_selection.GetCount() == 0) {
            pTool->OnLButtonDown(this, 0, point);
        }
        pTool->OnLButtonDblClk(this, nFlags, point);
    }
}

//////////////////////////////////////////////////////////////////////////////
void CDrawView::ClearTracker()
{
    //CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    // invalidate current selection since it will be deselected
    OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
    m_selection.RemoveAll();

    m_tracker.m_rect = CRect(0,0,0,0);
    CWnd* pFrame=GetParentFrame();
    CRect rClient;
    GetClientRect(&rClient);
    InvalidateRect(rClient);

    // invalidate new pasted stuff
    GetDocument()->UpdateAllViews(NULL);//, HINT_UPDATE_SELECTION, &m_selection);
}

void CDrawView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
    switch (lHint)
    {
    case HINT_UPDATE_WINDOW:    // redraw entire window
        Invalidate(FALSE);
        break;

    case HINT_UPDATE_DRAWOBJ:   // a single object has changed
        {
        InvalObj((CDrawObj*)pHint);
        }
        break;

    case HINT_UPDATE_SELECTION: // an entire selection has changed
        {
            CDrawObjList* pList = pHint != NULL ?
                (CDrawObjList*)pHint : &m_selection;
            POSITION pos = pList->GetHeadPosition();
            while (pos != NULL) {
                CDrawObj* pObj = pList->GetNext(pos);
                if (pObj) {
                    pObj->Deselect();
                    InvalObj(pObj);
                }
            }
        }
        break;

    case HINT_DELETE_SELECTION: // an entire selection has been removed
        if (pHint != &m_selection)
        {
            CDrawObjList* pList = (CDrawObjList*)pHint;
            POSITION pos = pList->GetHeadPosition();
            while (pos != NULL)
            {
                CDrawObj* pObj = pList->GetNext(pos);
                if (pObj) {
                    pObj->Deselect();
                    InvalObj(pObj);
                    Remove(pObj);   // remove it from this view's selection
                }
            }
        }
        break;

    case HINT_UPDATE_TOOLBAR_COLOR_BUTTONS:
        {
            CWnd * pWndFocus = GetFocus();
        }
        break;

    default:
        //ASSERT(FALSE);
        break;
    }
}

void CALLBACK LineDDAProc(int x, int y, CDrawView *pview)
{
    CDC* pDC = pview->GetDC();
    if (pDC) {

        pview->m_SelCount++;
        if (pview->m_SelCount <= 4)	pDC->SetPixelV(x,y,RGB(255,255,255));
        else pDC->SetPixelV(x,y,RGB(0,0,0));
        if (pview->m_SelCount == 8) pview->m_SelCount=0;

/*
        // x, y는 선을 이룰 점이 그려질 좌표
        // 아래는 10도트 마다 원을 그려 작은 원으로 된 사선을 형성하게 만듭니다.
        if ((x % 10) == 0)
            Ellipse(*pDC, x - 5, y - 5, x + 5, y + 5);
*/
    }
    pview->ReleaseDC(pDC); //<jh>
}
//////////////////////////////////////////////////////////////////////////////
void CDrawView::DrawSelection()
{
    CDrawDoc* pDoc = GetDocument();
    if (pDoc->m_NumSel>2){
        CPoint pos(GetScrollPosition());
        double zoom=pDoc->GetZoomFactor();
        m_SelCount=m_SelShift;
//		pDC->MoveTo(zoom*pDoc->m_Sel[0].x - pos.x,zoom*pDoc->m_Sel[0].y - pos.y);
        for(int i=1;i<pDoc->m_NumSel;i++){
//			pDC->LineTo(zoom*pDoc->m_Sel[i].x - pos.x,zoom*pDoc->m_Sel[i].y - pos.y);
            LineDDA((int)(zoom*pDoc->m_Sel[i-1].x - pos.x),
                    (int)(zoom*pDoc->m_Sel[i-1].y - pos.y),
                    (int)(zoom*pDoc->m_Sel[i].x - pos.x),
                    (int)(zoom*pDoc->m_Sel[i].y - pos.y),
                    (LINEDDAPROC)LineDDAProc,(LPARAM)this);
        }
    }
}

void CDrawView::OnDestroy()
{
    CScrollView::OnDestroy();
    OnCancelMode();
}

void CDrawView::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        m_SelShift++;
        if (m_SelShift==8) m_SelShift=0;
        DrawSelection();
    }
    CScrollView::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawView diagnostics

#ifdef _DEBUG
void CDrawView::AssertValid() const
{
    CScrollView::AssertValid();
}

void CDrawView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}
#endif //_DEBUG

void CDrawView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CDrawDoc* pDoc = GetDocument();
    int nCount = 0;
    if (!m_bActive || !m_bEditEnable)
        return;
    switch (nChar)
    {
        case VK_DELETE:
/*
            nCount = m_selection.GetCount();
            if (nCount <= 0) return;
            if (AfxMessageBox(_T("Do you want delete?"), MB_YESNO|MB_ICONWARNING) == IDYES)
            {
                OnEditClear();
                m_bEditEnable = TRUE;
                CDrawTool::c_drawShape = selection;
                Invalidate();
            }
            AfxGetMainWnd()->Invalidate();
*/
        break;
        case VK_ESCAPE:
            Select(NULL);
        break;

        default:
            break;
    }
    CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//
// CDrawObj가 삭제되거나 선택(클릭)되었을때 호출되며 m_undoList로 백업CDrawObj를 저장한다.
//
void CDrawView::RegisterUndoList(CDrawObj* pRegObj)
{
    // CDrawObj를 새로 만들어서 UndoList에 추가한다. m_orgObj는 선택된Obj의 원본정보를 담고있다;
    CDrawObj *pUndoObj, *pObj;
    pObj = pRegObj;
    if (pObj->m_pParent)
        pObj = pObj->m_pParent;

    if (pObj->m_nShape == CDrawObj::roipat) {
        pUndoObj = new CDrawRoiPat(pObj->m_position);
        *pUndoObj = *(CDrawRoiPat *)pObj;
    } else if (pObj->m_nShape == CDrawObj::roi){
        pUndoObj = new CDrawRoi(pObj->m_position);
        *pUndoObj = *(CDrawRoi *)pObj;
    } else if (pObj->m_nShape == CDrawObj::rectangle || /*pObj->m_nShape == CDrawObj::roi ||*/ pObj->m_nShape == CDrawObj::point) {
        pUndoObj = new CDrawRect(pObj->m_position);
        *pUndoObj = *(CDrawRect *)pObj;
    } else {
        pUndoObj = new CDrawObj(pObj->m_position);
        *pUndoObj = *pObj;
    }
    pUndoObj->m_nUndoOper = CDrawObj::umove;
    m_undoList.AddTail(pUndoObj);

}

void CDrawView::SetShowFilter(DWORD dwFilter)
{
    CDrawObjList* pObList = GetDocument()->GetObjects();
    POSITION posObj = pObList->GetHeadPosition();
    while (posObj != NULL)
    {
        CDrawObj* pObj = pObList->GetNext(posObj);
        pObj->m_dwFilter = dwFilter;
    }
};
