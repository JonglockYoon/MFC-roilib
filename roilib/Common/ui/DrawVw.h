// drawvw.h : interface of the CDrawView class
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

#pragma once

#include "drawobj.h"
#include "scrollviewex.h"

// Hints for UpdateAllViews/OnUpdate
#define HINT_UPDATE_WINDOW					0
#define HINT_UPDATE_DRAWOBJ					1
#define HINT_UPDATE_SELECTION				2
#define HINT_DELETE_SELECTION				3
//#define HINT_UPDATE_OLE_ITEMS				4
#define HINT_UPDATE_TOOLBAR_COLOR_BUTTONS	5

#ifndef ID_DRAW_RECT
    #define ID_DRAW_RECT	40000
#endif

#ifndef ID_DRAW_POLYGON
    #define ID_DRAW_POLYGON 40001
#endif

#ifndef ID_SETTING_REGION
    #define ID_SETTING_REGION 40002
#endif

class CDrawObj;
class CDrawView;

class CDrawObjListEx : public CDrawObjList
{
public:
    CDrawObjListEx(CDrawView* pView)
    {
        m_pView = pView;
    }
    void RemoveAt(POSITION pos)
    {
        CDrawObjList::RemoveAt(pos);
//		((CMainFrame*)AfxGetMainWnd())->UpdatePropertyGridContent(m_pView);
    }
    void AddTail(CDrawObj* pObj)
    {
        pObj->m_bSelected = TRUE;
        CDrawObjList::AddTail(pObj);
//		((CMainFrame*)AfxGetMainWnd())->UpdatePropertyGridContent(m_pView);
    }
    void RemoveAll()
    {
        POSITION pos = GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = GetNext(pos);
            pObj->m_bSelected = FALSE;

        }

        CDrawObjList::RemoveAll();
//		((CMainFrame*)AfxGetMainWnd())->UpdatePropertyGridContent(m_pView);
    }

    CDrawView* m_pView;
};

class CDrawDoc;
class CDrawView : public CScrollViewEx
{
public: // create from serialization only
    CDrawView();
    DECLARE_DYNCREATE(CDrawView)

// Attributes
public:
    CDrawDoc* GetDocument()
        { return (CDrawDoc*)m_pDocument; }
    void SetPageSize(CSize size);
    CRect GetInitialPosition();

// Operations
public:
    void DocToClient(CRect& rect);
    void DocToClient(CPoint& point);
    void ClientToDoc(CPoint& point);
    void ClientToDoc(CRect& rect);
    void SelectAddBefore(CDrawObj* pObj, CDrawObj* pNewObj);
    void Select(CDrawObj* pObj, BOOL bAdd = FALSE);
    void SelectWithinRect(CRect rect, BOOL bAdd = FALSE);
    void Deselect(CDrawObj* pObj);
    void CloneSelection();
    void UpdateActiveItem();
    void InvalObj(CDrawObj* pObj);
    void Remove(CDrawObj* pObj);
    void RemoveAll();
    void ClearTracker();
    virtual bool SetImageRectSelection(CDrawDoc *pDoc,CRect *rect) { return false; };
    void RegisterUndoList(CDrawObj* pRegObj);

// Implementation
protected:

public:
    virtual ~CDrawView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnActivateView(BOOL bActivate, CView* pActiveView, CView* pDeactiveView);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
    //virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    CDrawObjListEx m_selection;
    CDrawObjListEx m_clone;
    BOOL m_bActive;					// is the view active?
    BOOL m_bEditEnable;

    long m_SelCount; // LineDDAProc()을 위한 변수.
    long m_SelShift;
    void DrawSelection();

    CPoint m_RefPoint; // 마우스를 클릭했을때화면상의 x,y좌표.
    CPoint m_RefScroll; // 마우스를 클릭했을때 현재의 Scroll Position - 이미지Size에대한 화면 좌상단 위치를 나타낸다.

    CRectTracker m_tracker;
    CDrawObjList m_undoList;

    CxImage* m_ximage;
    inline double GetOldZoomFactor() { return m_OldZF; }
    double GetZoomFactor() { return m_ZoomFactor; }
    inline void SetZoomFactor(double zf) { m_OldZF = m_ZoomFactor; m_ZoomFactor = zf; }
    void SetShowFilter(DWORD dwFilter);

protected:
    double m_OldZF;
    double m_ZoomFactor;

protected:
    virtual void OnInitialUpdate(); // called first time after construct

    // OLE Container support
public:
    virtual BOOL IsSelected(const CObject* pDocItem) const;

// Generated message map functions
protected:
    //{{AFX_MSG(CDrawView)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

};

/////////////////////////////////////////////////////////////////////////////
