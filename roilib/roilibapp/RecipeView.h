
// RecipeView.h : CRecipeView 클래스의 인터페이스
//

#pragma once

#include "ui/DrawVw.h"

class CRoilibAppDoc;
class CRecipeView : public CDrawView
{
protected: // serialization에서만 만들어집니다.
    CRecipeView();
    DECLARE_DYNCREATE(CRecipeView)

// 특성입니다.
public:
    CRoilibAppDoc* GetDocument() const;

// 재정의입니다.
public:
    virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    protected:
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
    //}}AFX_VIRTUAL
    virtual BOOL PreTranslateMessage(MSG* pMsg);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
    virtual void OnInitialUpdate(); // called first time after construct

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg LRESULT OnNewImage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnNewProgress(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    afx_msg void OnViewToolsTracker();
    afx_msg void OnUpdateViewToolsTracker(CCmdUI* pCmdUI);
    afx_msg void OnViewToolsMove();
    afx_msg void OnUpdateViewToolsMove(CCmdUI* pCmdUI);
    afx_msg void OnViewToolsSelect();
    afx_msg void OnUpdateViewToolsSelect(CCmdUI* pCmdUI);
    afx_msg void OnViewToolsMeasure();
    afx_msg void OnUpdateViewToolsMeasure(CCmdUI *pCmdUI);
    afx_msg void OnViewToolsDrawRect();
    afx_msg void OnUpdateViewToolsDrawRect(CCmdUI *pCmdUI);
    afx_msg void OnViewToolsDrawRoiPat();
    afx_msg void OnUpdateViewToolsDrawRoiPat(CCmdUI *pCmdUI);

    afx_msg void OnViewZoomin();
    afx_msg void OnUpdateViewZoomin(CCmdUI* pCmdUI);
    afx_msg void OnViewZoomout();
    afx_msg void OnUpdateViewZoomout(CCmdUI* pCmdUI);
    afx_msg void OnViewNormalviewing11();
    afx_msg void OnUpdateViewNormalviewing11(CCmdUI* pCmdUI);
    afx_msg void OnRecipeGrabStart();
    afx_msg void OnInspectionChannel();
    void OnInspectionChannel(int index);
    afx_msg void OnUpdateInspectionChannel(CCmdUI *pCmdUI);
    afx_msg void OnRecipeImagesave();
    afx_msg void OnEditClear();
    afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
    afx_msg void OnRecipeImageload();

public:
    CxImage* m_ximage[MAX_CH_NUM]; // theApp.m_nSelectCh 로 구분한다.
    IplImage *cimg;	// Grab을 위한 Temporary 저장소.
    void SetZoomCal(double zf);

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

public:
    LRESULT OnReloadRoiData(WPARAM wParam, LPARAM lParam);
    void DrawGride(CDC* pDC);
    void GetImageXY(CRoilibAppDoc *pDoc, CxImage *ima, long *x, long *y);
    void GetImageXY(CRoilibAppDoc *pDoc, CxImage *ima, long *x, long *y, double zf);
    void GetScreenXY(CRoilibAppDoc *pDoc, CxImage *ima, long *x, long *y);
    virtual bool SetImageRectSelection(CDrawDoc *pDoc, CRect *rect);
    virtual ~CRecipeView();
    void MoveViewImage(CPoint delta, double r, BOOL bAbs = FALSE);
    void MoveCenterPosition(CPoint pt);
    void MoveBottomLeftPosition(CPoint pt);

    afx_msg void OnRecipeInspection();

    void OtherObjectsClear(DWORD dwFilter = DRAWOBJ_TYPE_ALL);
    void Draw(CDC* pDC, CDrawView* pView);

    CDrawObjList m_otherObjects; // 분석결과를 화면에 Display하기위한 정보가 담겨있다.

    virtual inline void SetShowFilter(DWORD dwFilter) {
        CDrawView::SetShowFilter(dwFilter);
        POSITION pos = m_otherObjects.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = m_otherObjects.GetNext(pos);
            pObj->SetViewFilter(dwFilter);

        }
    }

    CPoint GetCenter(int nCh) {
        if (m_ximage[nCh] == NULL)
            return CPoint(0, 0);
        int cx = m_ximage[nCh]->GetWidth();
        int cy = m_ximage[nCh]->GetHeight();
        return CPoint(cx / 2, cy / 2);
    }

    afx_msg void OnButtonPreview();
    afx_msg void OnButtonStop();
    afx_msg void OnUpdateButtonPreview(CCmdUI *pCmdUI);
    afx_msg void OnUpdateButtonStop(CCmdUI *pCmdUI);
    afx_msg void OnViewToolsDrawline();
    afx_msg void OnUpdateViewToolsDrawline(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // RecipeView.cpp의 디버그 버전
inline CRoilibAppDoc* CRecipeView::GetDocument() const
   { return reinterpret_cast<CRoilibAppDoc*>(m_pDocument); }
#endif

