// RoilibAppView.h : interface of the CRoilibAppView class
//


#pragma once

#include "ui/DrawVw.h"


#define RULER_HEIGHT          14
#define MINIMUM_INCR          5
#define MAXIMUM_SUBDIVIDE     5
#define MAXIMUM_SCALES        10

#define ROUND(x) ((int) ((x) + 0.5))

// ruler types
#define RT_VERTICAL	  0
#define RT_HORIZONTAL 1

#define GSEAL(ident)      ident

struct _GtkRulerMetric
{
    char *metric_name;
    char *abbrev;
    /* This should be points_per_unit. This is the size of the unit
    * in 1/72nd's of an inch and has nothing to do with screen pixels */
    double pixels_per_unit;
    double ruler_scale[10];
    int subdivide[5];        /* five possible modes of subdivision */
};
typedef struct _GtkRulerMetric  GtkRulerMetric;

struct _GtkRuler
{
    GtkRulerMetric *GSEAL(metric);
    int GSEAL(xsrc);
    int GSEAL(ysrc);
    int GSEAL(slider_size);

    /* The upper limit of the ruler (in points) */
    double GSEAL(lower);
    /* The lower limit of the ruler */
    double GSEAL(upper);
    /* The position of the mark on the ruler */
    double GSEAL(position);
    /* The maximum size of the ruler */
    double GSEAL(max_size);
};
typedef struct _GtkRuler        GtkRuler;


class CRoilibAppDoc;
class CRoilibAppView : public CDrawView
{
protected: // create from serialization only
    CRoilibAppView();
    DECLARE_DYNCREATE(CRoilibAppView)

// Attributes
public:
    CRoilibAppDoc* GetDocument() const;
    int m_nIndex;

    void DrawAlphaBlend(HDC hdcwnd);

// Operations
public:

// Overrides
public:
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
    virtual void OnInitialUpdate(); // called first time after construct
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
    virtual ~CRoilibAppView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    void DrawGride(CDC* pDC);
    void GetImageXY(CxImage *ima, long *x, long *y);
    void GetImageXY(CxImage *ima, long *x, long *y, double zf);

    void MoveViewImage(CPoint delta, double r, BOOL bAbs = FALSE);
    void MoveCenterPosition(CPoint pt);
    void MoveBottomLeftPosition(CPoint pt);

    void OtherObjectsClear(DWORD dwFilter = DRAWOBJ_TYPE_ALL);
    void Draw(CDC* pDC, CDrawView* pView);

    double x1, x2;
    double y1, y2;
    GtkRuler rruler;
    UINT m_rulerType;
    //float m_fZoomFactor;
    void RulerDraw(CDC* pDC);
    void DrawTicker(CDC* pDC, CRect rulerRect, BOOL bShowPos = TRUE);
    void DrawCursorPos(CPoint NewPos);
    void gtk_hruler_draw_ticks(CDC* pDC, CRect rulerRect, BOOL bShowPos);
    void gtk_vruler_draw_ticks(CDC* pDC, CRect rulerRect, BOOL bShowPos);


    CDrawObjList m_otherObjects; // 분석결과를 화면에 Display하기위한 정보가 담겨있다.

    void SetZoomCal(double zf);
    CPoint GetCenter() {
        int cx = m_ximage->GetWidth();
        int cy = m_ximage->GetHeight();
        return CPoint(cx / 2, cy / 2);
    }
    virtual inline void SetShowFilter(DWORD dwViewFilter) {
        POSITION pos = m_otherObjects.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = m_otherObjects.GetNext(pos);
            pObj->SetViewFilter(dwViewFilter);

        }
    }
protected:
    CBrush m_brHatch;

    virtual inline void SetZoomFactor(double zf) {
        CDrawView::SetZoomFactor(zf);

        POSITION pos = m_otherObjects.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = m_otherObjects.GetNext(pos);
            pObj->SetZoomFactor(m_ZoomFactor);

        }
    }

// Generated message map functions
protected:
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

    afx_msg void OnViewToolsTracker();
    afx_msg void OnUpdateViewToolsTracker(CCmdUI* pCmdUI);
    afx_msg void OnViewToolsMove();
    afx_msg void OnUpdateViewToolsMove(CCmdUI* pCmdUI);
    afx_msg void OnViewToolsSelect();
    afx_msg void OnUpdateViewToolsSelect(CCmdUI* pCmdUI);
    afx_msg void OnViewToolsMeasure();
    afx_msg void OnUpdateViewToolsMeasure(CCmdUI *pCmdUI);

    afx_msg void OnViewZoomin();
    afx_msg void OnUpdateViewZoomin(CCmdUI* pCmdUI);
    afx_msg void OnViewZoomout();
    afx_msg void OnUpdateViewZoomout(CCmdUI* pCmdUI);
    afx_msg void OnViewNormalviewing11();
    afx_msg void OnUpdateViewNormalviewing11(CCmdUI* pCmdUI);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnButtonPreview();
    afx_msg void OnButtonStop();
    afx_msg void OnUpdateButtonPreview(CCmdUI *pCmdUI);
    afx_msg void OnUpdateButtonStop(CCmdUI *pCmdUI);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in InspectView.cpp
inline CRoilibAppDoc* CRoilibAppView::GetDocument() const
   { return reinterpret_cast<CRoilibAppDoc*>(m_pDocument); }
#endif

