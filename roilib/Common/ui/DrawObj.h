// DrawObj.h - interface for CDrawObj and derivatives
//

#ifndef __DRAWOBJ_H__
#define __DRAWOBJ_H__

#define DRAWOBJ_TYPE_RECIPE			0x00000001
#define DRAWOBJ_TYPE_RESULT			0x00000002
//#define DRAWOBJ_TYPE_CALIBRATION	0x00000004
#define DRAWOBJ_TYPE_ALL			0xFFFFFFFF

#define MIN_SIZE	2
#define BUTTON_PRESS	0x0001	// mouse button press
#define BUTTON_RELEASE	0x0010	// Mouse button release, vecRoiData에 추가된다.

#ifndef WM_USER_PROPERTY_UPDATE
    #define WM_USER_PROPERTY_ADD		WM_USER+1004
    #define WM_USER_PROPERTY_UPDATE		WM_USER+1005
    #define WM_USER_PROPERTY_DELETE		WM_USER+1006
    #define WM_USER_PROPERTY_SELECT		WM_USER+1007
    #define WM_USER_PROPERTY_EDIT		WM_USER+1008
    #define WM_USER_GET_ROISET			WM_USER+1009
#endif

#include "Data/RecipeData.h"

class CDrawView;
class CDrawDoc;

/////////////////////////////////////////////////////////////////////////////
// CDrawObj - base class for all 'drawable objects'

//
// 변수추가할때는 CDrawObj& operator= 함수에도 넣어주세요
//
class CDrawObj : public CObject
{
protected:
    //DECLARE_SERIAL(CDrawObj);
    //CDrawObj();

// Constructors
public:
    CDrawObj(const CRect& position);
    CDrawObj& operator=(const CDrawObj& myObj);

// Attributes
    CRect m_position;
    CDrawDoc* m_pDocument;
    CPoint m_textOffset;

    CRect GetRect();
    virtual int GetHandleCount();
    virtual CPoint GetHandle(int nHandle);
    CRect GetHandleRect(int nHandleID, CDrawView* pView);
    virtual HCURSOR GetHandleCursor(int nHandle);

    virtual void SetLineWidth(int w);
    virtual void SetLineColor(COLORREF color);
    virtual void SetTextOffset(int x, int y);
    virtual void SetTextColor(COLORREF color);
    virtual void SetTextPosition(DWORD position);
    virtual void SetTextFontSize(int size);
    virtual void SetTextFontFaceName(TCHAR *pszFaceName);
    virtual void SetFillColor(COLORREF color);
    virtual COLORREF GetLineColor();
    virtual COLORREF GetTextColor();
    virtual COLORREF GetFillColor();
    virtual BOOL IsNoFill();
    virtual BOOL IsNoOutline();
    virtual BOOL SetNoFill( BOOL bSet = TRUE );
    virtual BOOL SetNoOutline( BOOL bSet = TRUE );
    virtual void Select();
    virtual void Deselect();
    virtual void SetZoomFactor(double zoomFactor);

// Operations
    virtual void Draw(CDC* pDC);
    enum TrackerState { normal, selected, active };
    virtual void DrawTracker(CDC* pDC, TrackerState state);
    virtual void MoveTo(const CRect& positon, CDrawView* pView = NULL);
    virtual int HitTest(CPoint point, CDrawView* pView, BOOL bSelected);
    BOOL circleLineIntersect(float x1, float y1, float x2, float y2, float cx, float cy, float cr);
    virtual BOOL Intersects(float cx, float cy, float cr);
    virtual BOOL Intersects(const CRect& rect);
    virtual void MoveHandleTo(int nHandle, CPoint point, CDrawView* pView = NULL);
    virtual void OnOpen(CDrawView* pView);
    virtual void OnEditProperties(CDrawView* pView);
    virtual CDrawObj* Clone(CDrawDoc* pDoc = NULL);
    virtual void Remove();
    virtual void RemovingChild(CDrawObj *pChild);
    void Invalidate();
    void SetHidden(BOOL flag = TRUE) { m_bHidden = flag; }
    BOOL IsHidden() { return m_bHidden; }

// Implementation
public:
    virtual ~CDrawObj();
#ifdef _DEBUG
    void AssertValid();
#endif

    // implementation data
public:
    //CPoint m_scrpt; // MIL일때만 사용하는 임시변수이며, GetScrollPosition()의 값을 가지고 있다
    CRoiData* m_pRoiData; // CRoiBase를 상속받는 객체중 가장 메모리공간을 크게 사용하는 객체로 Alloc해서 사용하자..
public:
    CDrawObj *m_pParent;
    enum Shape { none, point, rectangle, roundRectangle, ellipse, line, cross, roi, roipat, polygon };
    Shape m_nShape;
    BOOL m_bSizeFix; // 패턴ROI의 cross포인터에서 사용
    double m_ZoomFactor;

    int m_nSeq; // CDrawObj가 가지는 유일한값 - Undo에서 사용할려고 추가
    enum Undo { unone, umove, uadd, udelete };
    int m_nUndoOper; // 0-none, 1-move, 2-add, 3-delete (0이면 m_objects의 CDrawObj 대체, 1이면 삭제, 2이면추가)
protected:
    BOOL m_bPen;
    LOGPEN m_logpen;
    COLORREF m_selcolor;
    BOOL m_bBrush;
    LOGBRUSH m_logbrush;
    COLORREF m_textColor;
    DWORD m_textPosition;
    BOOL m_bHidden;
    int fontHeight;
    TCHAR fontFaceName[128];

public:
    BOOL m_bSelected;
    DWORD	m_dwType;
    DWORD	m_dwFilter; // dwType이 dwFilter에 포함된 항목들만 Display한다.

    void SetViewFilter(DWORD dwFilter) {
        m_dwFilter = dwFilter;
    }
};

// special 'list' class for this application (requires afxtempl.h)
class CDrawObjList : public CTypedPtrList<CObList, CDrawObj*>
{

};

////////////////////////////////////////////////////////////////////////
// rectangle, roundRectangle, ellipse, line, cross, roi 등이 여기에 해당되는 Object들이다.
//
class CDrawRect : public CDrawObj
{
protected:
    //DECLARE_SERIAL(CDrawRect);
    //CDrawRect();

public:
    CDrawRect(const CRect& position);
    CDrawRect& operator=(const CDrawRect& myObj);

// Implementation
public:
    virtual ~CDrawRect();
    virtual void Draw(CDC* pDC);
    virtual int GetHandleCount();
    virtual CPoint GetHandle(int nHandle);
    virtual HCURSOR GetHandleCursor(int nHandle);
    virtual void MoveHandleTo(int nHandle, CPoint point, CDrawView* pView = NULL);
    virtual BOOL Intersects(const CRect& rect);
    virtual CDrawObj* Clone(CDrawDoc* pDoc);
    virtual void Remove();

protected:
    CPoint m_roundness; // for roundRect corners -> roundRectangle일때

    friend class CRectTool;
    friend class CDrawRoiPat;
};

/////////////////////////////////////////////////////////////////////////////
// 십자표시 Object
class CDrawPoint : public CDrawObj
{
protected:
    //DECLARE_SERIAL(CDrawPoint);
    //CDrawPoint();

public:
    CDrawPoint(const CRect& position);
    CDrawPoint& operator=(const CDrawPoint& myObj);

// Implementation
public:
    virtual ~CDrawPoint();
    virtual void Draw(CDC* pDC);
    virtual int GetHandleCount();
    virtual CPoint GetHandle(int nHandle);
    virtual HCURSOR GetHandleCursor(int nHandle);
    virtual void MoveHandleTo(int nHandle, CPoint point, CDrawView* pView = NULL);
    virtual BOOL Intersects(const CRect& rect);
    virtual CDrawObj* Clone(CDrawDoc* pDoc);
    virtual void Remove();

protected:

    friend class CRectTool;
};

/////////////////////////////////////////////////////////////////////////////
// 이 Object는 적용을 해보지 않아서 적용을 할때는 전체적인 검토를 해서 적용이 필요하다.
class CDrawPoly : public CDrawObj
{
protected:
    //DECLARE_SERIAL(CDrawPoly);
    //CDrawPoly();

public:
    CDrawPoly(const CRect& position);
    CDrawPoly& operator=(const CDrawPoly& myObj);

// Operations
    void AddPoint(const CPoint& point, CDrawView* pView = NULL);
    BOOL RecalcBounds(CDrawView* pView = NULL);

// Implementation
public:
    virtual ~CDrawPoly();
    virtual void Draw(CDC* pDC);
    virtual void MoveTo(const CRect& position, CDrawView* pView = NULL);
    virtual int GetHandleCount();
    virtual CPoint GetHandle(int nHandle);
    virtual HCURSOR GetHandleCursor(int nHandle);
    virtual void MoveHandleTo(int nHandle, CPoint point, CDrawView* pView = NULL);
    virtual BOOL Intersects(const CRect& rect);
    virtual CDrawObj* Clone(CDrawDoc* pDoc);

protected:
    int m_nPoints;
    int m_nAllocPoints;
    CPoint* m_points;
    CDrawPoly* m_pDrawObj;

    friend class CPolyTool;
};

/////////////////////////////////////////////////////////////////////////////

class CDrawRoiPat : public CDrawRect
{
protected:
    //DECLARE_SERIAL(CDrawRoiPat);
    //CDrawRoiPat();

public:
    CDrawRoiPat(const CRect& position);
    CDrawRoiPat& operator=(const CDrawRoiPat& myObj);

// Implementation
public:
    virtual ~CDrawRoiPat();
    virtual void Draw(CDC* pDC);
    virtual void MoveHandleTo(int nHandle, CPoint point, CDrawView* pView = NULL);
    virtual void RemovingChild(CDrawObj *pChild);
    virtual CDrawObj* Clone(CDrawDoc* pDoc);
    virtual void Select();
    virtual void Deselect();

public:
    BOOL m_bZombie;	//m_pPatObj 또는 m_pCrossObj가 NULL이 될때 ...
    BOOL m_bIsFirst; // 처음 생성할때만 TRUE로 된다.
    CDrawRect* m_pPatObj;
    CDrawRect* m_pCrossObj;

    friend class CRoiPatTool;
};

class CDrawRoi : public CDrawRect
{
protected:
    //DECLARE_SERIAL(CDrawRoi);
    //CDrawRoi();

public:
    CDrawRoi(const CRect& position);
    CDrawRoi& operator=(const CDrawRoi& myObj);

    // Implementation
public:
    virtual ~CDrawRoi();
    virtual void Draw(CDC* pDC);
    virtual void MoveHandleTo(int nHandle, CPoint point, CDrawView* pView = NULL);
    virtual void RemovingChild(CDrawObj *pChild);
    virtual CDrawObj* Clone(CDrawDoc* pDoc);
    virtual void Select();
    virtual void Deselect();

public:
    BOOL m_bZombie;	//m_pPatObj 또는 m_pCrossObj가 NULL이 될때 ...
    BOOL m_bIsFirst; // 처음 생성할때만 TRUE로 된다.
    CDrawRect* m_pCrossObj;

    friend class CRoiTool;
};

#endif // __DRAWOBJ_H__
