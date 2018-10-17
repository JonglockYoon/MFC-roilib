// DrawTool.h - interface for CDrawTool and derivatives
//


#ifndef __DRAWTOOL_H__
#define __DRAWTOOL_H__

#include "DrawObj.h"

//class CVMS5View;

enum SelectMode
{
    none,
    netSelect,
    move1, // move -> move1, size -> size1 일반적인용어라서 1을 붙임
    size1
};

enum DrawShape // ToolBar와 1:1 매칭되는 값
{
    moving,
    selection,
    tracker,
    point1,
    rect,
    line,
    cross,
    roundRect,
    ellipse,
    poly,
    roi,
    roipat
};

class CDrawTool
{
// Constructors
public:
    CDrawTool(DrawShape nDrawShape);

// Overridables
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnEditProperties(CDrawView* pView);
    virtual void OnCancel();

// Attributes
    DrawShape m_drawShape;

    static CDrawTool* FindTool(DrawShape drawShape);
    static CPtrList c_tools;
    static CPoint c_down;
    static UINT c_nDownFlags;
    static CPoint c_last;
    static DrawShape c_drawShape;
};

class CMovingTool : public CDrawTool
{
// Constructors
public:
    CMovingTool();

// Implementation
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
};

class CTrackerTool : public CDrawTool
{
// Constructors
public:
    CTrackerTool();

// Implementation
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
};

class CSelectTool : public CDrawTool
{
// Constructors
public:
    CSelectTool();

// Implementation
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnEditProperties(CDrawView* pView);

    CRect m_lastSelectionRect;
};

class CRectTool : public CDrawTool
{
// Constructors
public:
    CRectTool(DrawShape drawShape);

// Implementation
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
};

class CPolyTool : public CDrawTool
{
// Constructors
public:
    CPolyTool();

// Implementation
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonDblClk(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnCancel();

    CDrawPoly* m_pDrawObj;
};

////////////////////////////////////////////////////////////////////////////

class CRoiPatTool : public CDrawTool
{
// Constructors
public:
    CRoiPatTool();

// Implementation
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);

    CDrawRoiPat* m_pRoiObj;
};

class CRoiTool : public CDrawTool
{
    // Constructors
public:
    CRoiTool();

    // Implementation
    virtual void OnLButtonDown(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnLButtonUp(CDrawView* pView, UINT nFlags, const CPoint& point);
    virtual void OnMouseMove(CDrawView* pView, UINT nFlags, const CPoint& point);

    CDrawRoi* m_pRoiObj;
};
////////////////////////////////////////////////////////////////////////////

#endif // __DRAWTOOL_H__
