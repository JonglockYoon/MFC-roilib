// author : jerry1455@gmail.com
// drawtool.cpp - implementation for drawing tools
//

#include "stdafx.h"
#include "DrawDoc.h"
#include "DrawVw.h"
#include "DrawObj.h"
#include "DrawTool.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawTool implementation

CPtrList CDrawTool::c_tools;

// c_tools.AddTail 작동을위한 변수들이므로 반드시 있어야한다.
static CMovingTool movingTool;
static CSelectTool selectTool;
static CRectTool lineTool(line);
static CRectTool crossTool(cross);
static CRectTool rectTool(rect);
static CRectTool point1Tool(point1);
//static CRectTool roiTool(roi);
static CRectTool roundRectTool(roundRect);
static CRectTool ellipseTool(ellipse);
static CPolyTool polyTool;
static CRoiPatTool roipatTool;
static CRoiTool roiTool;

////////////////////////////////////////////////////////////////////////////
// CDrawTool

CPoint CDrawTool::c_down;
UINT CDrawTool::c_nDownFlags;
CPoint CDrawTool::c_last;
DrawShape CDrawTool::c_drawShape = moving;

CDrawTool::CDrawTool(DrawShape drawShape)
{
    m_drawShape = drawShape;
    c_tools.AddTail(this);
}

CDrawTool* CDrawTool::FindTool(DrawShape drawShape)
{
    POSITION pos = c_tools.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawTool* pTool = (CDrawTool*)c_tools.GetNext(pos);
        if (pTool->m_drawShape == drawShape)
            return pTool;
    }

    return NULL;
}

void CDrawTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    pView->SetCapture();
    c_nDownFlags = nFlags;
    c_down = point;
    c_last = point;
}

void CDrawTool::OnLButtonDblClk(CDrawView* /*pView*/, UINT /*nFlags*/, const CPoint& /*point*/)
{
}

void CDrawTool::OnLButtonUp(CDrawView* /*pView*/, UINT /*nFlags*/, const CPoint& point)
{
    ReleaseCapture();

    if (point == c_down)
        c_drawShape = selection;
}

void CDrawTool::OnMouseMove(CDrawView* /*pView*/, UINT /*nFlags*/, const CPoint& point)
{
    c_last = point;
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CDrawTool::OnEditProperties(CDrawView* /*pView*/)
{
}

void CDrawTool::OnCancel()
{
    c_drawShape = selection;
}

static SelectMode selectMode = none;
static int nDragHandle;
static CPoint lastPoint = ( 0,0 );

////////////////////////////////////////////////////////////////////////////
// CMovingTool

CMovingTool::CMovingTool()
    : CDrawTool(moving)
{
}

void CMovingTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    pView->SetCapture();

    pView->m_RefScroll = pView->GetScrollPosition();
    pView->m_RefPoint.x = point.x;
    pView->m_RefPoint.y = point.y;

    CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CMovingTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    ReleaseCapture();

    CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CMovingTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    //CDrawDoc* pDoc = pView->GetDocument();
    CPoint local = point;

    if (nFlags & MK_LBUTTON)
    {
        //TRACE("CMovingTool MouseMove \n");

        SetCursor(LoadCursor(0,IDC_SIZEALL));
        CSize sz(pView->GetTotalSize());
        CWnd* pFrame = pView;// ->GetParentFrame();
        RECT rClient;
        pFrame->GetClientRect(&rClient);
        if (sz.cx>rClient.right)
            pView->SetScrollPos(SB_HORZ,pView->m_RefScroll.x - point.x + pView->m_RefPoint.x);
        else
            pView->SetScrollPos(SB_HORZ,0);
        if (sz.cy>rClient.bottom)
            pView->SetScrollPos(SB_VERT,pView->m_RefScroll.y - point.y + pView->m_RefPoint.y);
        else
            pView->SetScrollPos(SB_VERT,0);
        pView->Invalidate(0);
    }
    CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CTrackerTool

CTrackerTool::CTrackerTool()
    : CDrawTool(tracker)
{
}

void CTrackerTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{

    CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CTrackerTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{

    CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CTrackerTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{

    CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CSelectTool

CSelectTool::CSelectTool()
    : CDrawTool(selection)
{
    m_lastSelectionRect = ( 0,0,0,0 );
}

void CSelectTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CPoint local = point;
    pView->ClientToDoc(local);

    CDrawObj* pObj;
    selectMode = none;

    // Check for resizing (only allowed on single selections)
    if (pView->m_selection.GetCount() == 1)
    {
        pObj = pView->m_selection.GetHead();
        nDragHandle = pObj->HitTest(local, pView, TRUE);
        if (nDragHandle != 0)
            selectMode = size1;
        else {
            // SelectTool로 선택되어 있는 Object내부에 다른 Object를 선택했을때.
            if ((pObj->m_nShape == CDrawObj::roipat) ||
                (pObj->m_pParent && pObj->m_pParent->m_nShape == CDrawObj::roipat) )
            {
                CDrawRoiPat *pdrp = (CDrawRoiPat *)pObj;
                if (pObj->m_pParent)
                    pdrp = (CDrawRoiPat *)pObj->m_pParent;
                if (pdrp->m_pCrossObj && pdrp->m_pCrossObj->HitTest(local, pView, FALSE))
                {
                    pView->Remove(pObj);
                    pView->Select(pdrp->m_pCrossObj);
                    selectMode = move1;
                }
                else if (pdrp->m_pPatObj && pdrp->m_pPatObj->HitTest(local, pView, FALSE))
                {
                    pView->Remove(pObj);
                    pView->Select(pdrp->m_pPatObj);
                    nDragHandle = pdrp->m_pPatObj->HitTest(local, pView, TRUE);
                    if (nDragHandle != 0)
                        selectMode = size1;
                    selectMode = move1;
                }
            }
            if ((pObj->m_nShape == CDrawObj::roi) ||
                (pObj->m_pParent && pObj->m_pParent->m_nShape == CDrawObj::roi) )
            {
                CDrawRoi *pdrp = (CDrawRoi *)pObj;
                if (pObj->m_pParent)
                    pdrp = (CDrawRoi *)pObj->m_pParent;
                if (pdrp->m_pCrossObj && pdrp->m_pCrossObj->HitTest(local, pView, FALSE))
                {
                    pView->Remove(pObj);
                    pView->Select(pdrp->m_pCrossObj);
                    selectMode = move1;
                }
            }
        }
    }

    // See if the click was on an object, select and start move if so
    if (selectMode == none)
    {
        pObj = pView->GetDocument()->ObjectAt(local); // 마우스로 클릭한 선택된 Object가 리턴된다.
        if (pObj && pObj->m_pParent)
            pObj = pObj->m_pParent;

        if (pObj != NULL)
        {
            selectMode = move1;

            if (!pView->IsSelected(pObj))
                pView->Select(pObj, (nFlags & MK_SHIFT) != 0);

            // Ctrl+Click clones the selection...
            //우선 지원하지 않음
            //if ((nFlags & MK_CONTROL) != 0)
            //	pView->CloneSelection();
            if (pView->m_selection.GetCount() == 1)
            {
                AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_SELECT, 0, (LPARAM)pObj);
                pView->GetDocument()->UpdateAllViews(NULL);
            }
        }
    }

    // Click on background, start a net-selection
    if (selectMode == none)
    {
        if ((nFlags & MK_SHIFT) == 0)
            pView->Select(NULL);

        selectMode = netSelect;

        CClientDC dc(pView);
        CRect rect(point.x, point.y, point.x, point.y);
        rect.NormalizeRect();
        dc.DrawFocusRect(rect);

        pView->GetDocument()->UpdateAllViews(NULL);
        //pView->OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
    }

    if (selectMode != none) 
	{
        POSITION pos = pView->m_selection.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = pView->m_selection.GetNext(pos);
            pView->RegisterUndoList(pObj);
        }
    }

    lastPoint = local;
    CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CSelectTool::OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    if ((nFlags & MK_SHIFT) != 0)
    {
        // Shift+DblClk deselects object...
        CPoint local = point;
        pView->ClientToDoc(local);
        CDrawObj* pObj = pView->GetDocument()->ObjectAt(local);
        if (pObj != NULL)
            pView->Deselect(pObj);
    }
    else
    {
        // "Normal" DblClk opens properties...
        if (pView->m_selection.GetCount() == 1) 
		{
            CDrawObj* pObj = pView->m_selection.GetHead();
            if (pObj->m_pParent != NULL)
                pObj->m_pParent->OnOpen(pView);
            else pObj->OnOpen(pView);
        } else {
            if (pView->m_selection.GetCount() == 3) { // Pattern Object에대한 처리.
                POSITION pos = pView->m_selection.GetHeadPosition();
                while (pos != NULL)
                {
                    CDrawObj* pObj = pView->m_selection.GetNext(pos);
                    if (pObj->m_pParent != NULL) continue;
                    pObj->OnOpen(pView);
                    break;
                }
            }
        }
    }

    CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CSelectTool::OnEditProperties(CDrawView* pView)
{
    if (pView->m_selection.GetCount() == 1)
        pView->m_selection.GetHead()->OnEditProperties(pView);
}

void CSelectTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    m_lastSelectionRect = ( 0,0,0,0 );
    if (pView->GetCapture() == pView)
    {
        if (selectMode == netSelect)
        {
            CClientDC dc(pView);
            CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
            rect.NormalizeRect();
            dc.DrawFocusRect(rect);

            rect.InflateRect(1, 1); // CDrawView::OnDraw(CDC* pDC) 에서 CMemDC()사용 이미지를 그리니까 DrawFocusRect() XOR기능이 동작을 하지않아 편법동원.
            pView->InvalidateRect(rect, FALSE);

            pView->SelectWithinRect(rect, TRUE);
        }
        else if (selectMode != none)
        {
            pView->GetDocument()->UpdateAllViews(pView);

            CRect rect(point, c_down);

            if (point != c_down)
            {
                POSITION pos = pView->m_selection.GetHeadPosition();
                while (pos != NULL)
                {
                    CDrawObj* pObj = pView->m_selection.GetNext(pos);
                    AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_UPDATE, 0, (LPARAM)pObj);
                }
            }
        }
    }

    CDrawTool::OnLButtonUp(pView, nFlags, point);
}

void CSelectTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    if (pView->GetCapture() != pView)
    {
        if (c_drawShape == selection && pView->m_selection.GetCount() == 1)
        {
            CDrawObj* pObj = pView->m_selection.GetHead();
            CPoint local = point;
            pView->ClientToDoc(local);
            int nHandle = pObj->HitTest(local, pView, TRUE);
            if (nHandle != 0)
            {
                SetCursor(pObj->GetHandleCursor(nHandle));
                return; // bypass CDrawTool
            }
        }
        if (c_drawShape == selection)
            CDrawTool::OnMouseMove(pView, nFlags, point);
        return;
    }

    if (selectMode == netSelect)
    {
        CClientDC dc(pView);
        CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
        rect.NormalizeRect();
        dc.DrawFocusRect(rect);
        rect.SetRect(c_down.x, c_down.y, point.x, point.y);
        rect.NormalizeRect();
        dc.DrawFocusRect(rect);
        m_lastSelectionRect = rect;

        CDrawTool::OnMouseMove(pView, nFlags, point);
        return;
    }

    CPoint local = point;
    pView->ClientToDoc(local);
    CPoint delta = (CPoint)(local - lastPoint);

    POSITION pos = pView->m_selection.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = pView->m_selection.GetNext(pos);
        CRect position = pObj->m_position;
        //position.NormalizeRect();

        if (selectMode == move1 || pObj->m_bSizeFix)
        {
            position += delta;
            pObj->MoveTo(position, pView);
        }
        else if (nDragHandle != 0)
        {
            pObj->MoveHandleTo(nDragHandle, local, pView);
        }
    }

    lastPoint = local;

    if (selectMode == size1 && c_drawShape == selection)
    {
        c_last = point;
        SetCursor(pView->m_selection.GetHead()->GetHandleCursor(nDragHandle));
        return; // bypass CDrawTool
    }

    c_last = point;

    if (c_drawShape == selection)
        CDrawTool::OnMouseMove(pView, nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
// CRectTool (does rectangles, round-rectangles, and ellipses)

CRectTool::CRectTool(DrawShape drawShape)
    : CDrawTool(drawShape)
{
}

void CRectTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CDrawTool::OnLButtonDown(pView, nFlags, point);

    CPoint local = point;
    pView->ClientToDoc(local);
    TRACE(_T("CRectTool::OnLButtonDown = %d,%d\n"), local.x, local.y);

    CDrawRect* pObj = new CDrawRect(CRect(local, CSize(2, 2)));
    pObj->m_pDocument = pView->GetDocument();
    pObj->m_ZoomFactor = pView->GetDocument()->GetZoomFactor();
    //pObj->m_roiData.nType = _ObjType_Roi;
    switch (m_drawShape)
    {
    default:
        ASSERT(FALSE); // unsuported shape!

    case rect:
        pObj->m_nShape = CDrawRect::rectangle;
        break;

    case roundRect:
        pObj->m_nShape = CDrawRect::roundRectangle;
        break;

    case ellipse:
        pObj->m_nShape = CDrawRect::ellipse;
        break;

    case line:
        pObj->m_nShape = CDrawRect::line;
        break;

    case cross:
        pObj->m_nShape = CDrawRect::cross;
        break;

    case point1:
        pObj->m_nShape = CDrawRect::point;
        break;

    //case roi:
    //	pObj->m_nShape = CDrawRect::roi;
    //	break;

    }

    pView->GetDocument()->Add(pObj);
    pView->Select(pObj);

    AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_ADD, BUTTON_PRESS, (LPARAM)pObj);

    selectMode = size1;
    nDragHandle = 1;
    lastPoint = local;
}

void CRectTool::OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CDrawTool::OnLButtonDblClk(pView, nFlags, point);
}

void CRectTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CRect rect(point, c_down);
    rect.NormalizeRect();
    CDrawObj *pObj = NULL;
    if (pView->m_selection.GetCount() > 0)
        pObj = pView->m_selection.GetTail();
    if (pObj == NULL) {
        selectTool.OnLButtonUp(pView, nFlags, point);
        return; //방어코드.
    }
    if (rect.Width() < 5 && rect.Height() < 5)
    {
        // Don't create empty objects...
        pView->GetDocument()->Remove(pObj);

        AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_DELETE, 0, (LPARAM)pObj);

        pObj->Remove();
        selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
        pView->Invalidate();
    } else {
        AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_ADD, BUTTON_RELEASE, (LPARAM)pObj);
    }

    selectTool.OnLButtonUp(pView, nFlags, point);
}

void CRectTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
    selectTool.OnMouseMove(pView, nFlags, point);
}


////////////////////////////////////////////////////////////////////////////
// CPolyTool

CPolyTool::CPolyTool()
    : CDrawTool(poly)
{
    m_pDrawObj = NULL;
}

void CPolyTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CDrawTool::OnLButtonDown(pView, nFlags, point);

    CPoint local = point;
    pView->ClientToDoc(local);

    if (m_pDrawObj == NULL)
    {
        pView->SetCapture();

        m_pDrawObj = new CDrawPoly(CRect(local, CSize(2, 2)));
        m_pDrawObj->m_pDocument = pView->GetDocument();
        m_pDrawObj->m_ZoomFactor = pView->GetDocument()->GetZoomFactor();
        //m_pDrawObj->m_roiData.nType = _ObjType_Roi; // 재정의가 필요한 타입.. 나중에 필요할때 정의하자 2014.6.14

        pView->GetDocument()->Add(m_pDrawObj);
        pView->Select(m_pDrawObj);
        m_pDrawObj->AddPoint(local, pView);
    }
    else if (local == m_pDrawObj->m_points[0])
    {
        // Stop when the first point is repeated...
        ReleaseCapture();
        m_pDrawObj->m_nPoints -= 1;
        if (m_pDrawObj->m_nPoints < 2)
        {
            m_pDrawObj->Remove();
        }
        else
        {
            pView->InvalObj(m_pDrawObj);
        }
        m_pDrawObj = NULL;
        c_drawShape = selection;
        return;
    }

    local.x += 1; // adjacent points can't be the same!
    m_pDrawObj->AddPoint(local, pView);

    selectMode = size1;
    nDragHandle = m_pDrawObj->GetHandleCount();
    lastPoint = local;
}

void CPolyTool::OnLButtonUp(CDrawView* /*pView*/, UINT /*nFlags*/, const CPoint& /*point*/)
{
    // Don't release capture yet!
}

void CPolyTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    if (m_pDrawObj != NULL && (nFlags & MK_LBUTTON) != 0)
    {
        CPoint local = point;
        pView->ClientToDoc(local);
        m_pDrawObj->AddPoint(local);
        nDragHandle = m_pDrawObj->GetHandleCount();
        lastPoint = local;
        c_last = point;
        SetCursor(AfxGetApp()->LoadCursor(IDC_ARROW));
    }
    else
    {
        SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
        selectTool.OnMouseMove(pView, nFlags, point);
    }
}

void CPolyTool::OnLButtonDblClk(CDrawView* pView, UINT , const CPoint& )
{
    ReleaseCapture();

    int nPoints = m_pDrawObj->m_nPoints;
    if (nPoints > 2 &&
        (m_pDrawObj->m_points[nPoints - 1] == m_pDrawObj->m_points[nPoints - 2] ||
        m_pDrawObj->m_points[nPoints - 1].x - 1 == m_pDrawObj->m_points[nPoints - 2].x &&
        m_pDrawObj->m_points[nPoints - 1].y == m_pDrawObj->m_points[nPoints - 2].y))

    {
        // Nuke the last point if it's the same as the next to last...
        m_pDrawObj->m_nPoints -= 1;
        pView->InvalObj(m_pDrawObj);
    }

    m_pDrawObj = NULL;
    c_drawShape = selection;
}

void CPolyTool::OnCancel()
{
    CDrawTool::OnCancel();

    m_pDrawObj = NULL;
}

////////////////////////////////////////////////////////////////////////////
// CRoiTool

CRoiPatTool::CRoiPatTool()
    : CDrawTool(roipat)
{
    m_pRoiObj = NULL;
}

void CRoiPatTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CDrawDoc *pDrawDoc = pView->GetDocument();
    if (pDrawDoc == NULL)
        return;
    CPoint local = point;
    pView->ClientToDoc(local);

    m_pRoiObj = new CDrawRoiPat(CRect(local, CSize(2, 2)));
    if (m_pRoiObj == NULL)
        return;
    m_pRoiObj->m_pDocument = pDrawDoc;
    m_pRoiObj->m_ZoomFactor = pDrawDoc->GetZoomFactor();
    if (m_pRoiObj->m_pPatObj)
        m_pRoiObj->m_pPatObj->m_ZoomFactor = m_pRoiObj->m_ZoomFactor;
    if (m_pRoiObj->m_pCrossObj)
        m_pRoiObj->m_pCrossObj->m_ZoomFactor = m_pRoiObj->m_ZoomFactor;
    m_pRoiObj->m_nShape = CDrawObj::roipat;
    //m_pRoiObj->m_roiData.nType = _ObjType_Patt;

    pDrawDoc->Add(m_pRoiObj);
    pView->Select(m_pRoiObj);

    AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_ADD, BUTTON_PRESS, (LPARAM)m_pRoiObj);

    selectMode = size1;
    nDragHandle = 1;
    lastPoint = local;

    CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CRoiPatTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CRect rect(point, c_down);
    rect.NormalizeRect();
    CDrawObj *pObj = NULL;
    if (pView->m_selection.GetCount() > 0)
        pObj = pView->m_selection.GetTail();
    if (pObj == NULL) {
        selectTool.OnLButtonUp(pView, nFlags, point);
        return; //방어코드
    }
    if (rect.Width() < 10 || rect.Height() < 10)
    {
        pView->GetDocument()->Remove(pObj);

        AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_DELETE, 0, (LPARAM)pObj);

        pObj->Remove();
        m_pRoiObj = NULL;
        selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
        pView->Invalidate();
    } else {
        AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_ADD, BUTTON_RELEASE, (LPARAM)pObj);
    }

    selectTool.OnLButtonUp(pView, nFlags, point);

    if (m_pRoiObj != NULL)
    {
        pView->GetDocument()->AddBefore(m_pRoiObj, m_pRoiObj->m_pCrossObj);  // m_pDrawObj가 등록되고난후Object들을 등록한다.
        pView->GetDocument()->AddBefore(m_pRoiObj, m_pRoiObj->m_pPatObj);
        m_pRoiObj->m_bIsFirst = FALSE;

        pView->SelectAddBefore(m_pRoiObj, m_pRoiObj->m_pCrossObj);
        pView->SelectAddBefore(m_pRoiObj, m_pRoiObj->m_pPatObj);

        m_pRoiObj->m_bIsFirst = FALSE;
    }

}

void CRoiPatTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));

    selectTool.OnMouseMove(pView, nFlags, point);
}

CRoiTool::CRoiTool()
    : CDrawTool(roi)
{
    m_pRoiObj = NULL;
}

void CRoiTool::OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CPoint local = point;
    pView->ClientToDoc(local);

    m_pRoiObj = new CDrawRoi(CRect(local, CSize(2, 2)));
    m_pRoiObj->m_pDocument = pView->GetDocument();
    m_pRoiObj->m_ZoomFactor = pView->GetDocument()->GetZoomFactor();
    m_pRoiObj->m_pCrossObj->m_ZoomFactor = m_pRoiObj->m_ZoomFactor;
    m_pRoiObj->m_nShape = CDrawObj::roi;

    pView->GetDocument()->Add(m_pRoiObj);
    pView->Select(m_pRoiObj);

    AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_ADD, BUTTON_PRESS, (LPARAM)m_pRoiObj);

    selectMode = size1;
    nDragHandle = 1;
    lastPoint = local;

    CDrawTool::OnLButtonDown(pView, nFlags, point);
}

void CRoiTool::OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    CRect rect(point, c_down);
    rect.NormalizeRect();
    CDrawObj *pObj = NULL;
    if (pView->m_selection.GetCount() > 0)
        pObj = pView->m_selection.GetTail();

    if (pObj == NULL) {
        selectTool.OnLButtonUp(pView, nFlags, point);
        return; //방어코드.
    }

    if (rect.Width() < 10 || rect.Height() < 10)
    {
        // Don't create empty objects...
        if (pView->m_selection.GetCount() > 0)
        {
            pView->GetDocument()->Remove(pObj);

            AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_DELETE, 0, (LPARAM)pObj);

            pObj->Remove();
            m_pRoiObj = NULL;
            selectTool.OnLButtonDown(pView, nFlags, point); // try a select!
            pView->Invalidate();
        }
    } else {
        AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_PROPERTY_ADD, BUTTON_RELEASE, (LPARAM)pObj);
    }

    selectTool.OnLButtonUp(pView, nFlags, point);

    if (m_pRoiObj != NULL)
    {
        pView->GetDocument()->AddBefore(m_pRoiObj, m_pRoiObj->m_pCrossObj);  // m_pDrawObj가 등록되고난후Object들을 등록한다.
        m_pRoiObj->m_bIsFirst = FALSE;

        pView->SelectAddBefore(m_pRoiObj, m_pRoiObj->m_pCrossObj);

        m_pRoiObj->m_bIsFirst = FALSE;
    }

}

void CRoiTool::OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point)
{
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));

    selectTool.OnMouseMove(pView, nFlags, point);

}
