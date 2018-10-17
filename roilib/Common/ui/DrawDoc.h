// drawdoc.h : interface of the CDrawDoc class
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

#define MAX_SEL_POINTS 8000

#include "DrawObj.h"


struct DocType
{
public:
    int nID;
    BOOL bRead;
    BOOL bWrite;
    const TCHAR* description;
    const TCHAR* ext;
};

class CDrawView;
class CDrawDoc : public CDocument
{
protected: // create from serialization only
    CDrawDoc();
    DECLARE_DYNCREATE(CDrawDoc)

// Attributes
public:
    CDrawObjList* GetObjects() { return &m_objects; }
    const CSize& GetSize() const { return m_size; }
    void ComputePageSize();
    int GetMapMode() const { return m_nMapMode; }
    COLORREF GetPaperColor() const { return m_paperColor; }

public:
    //friend class CDrawView;
    //friend class CRectTool;
    //friend class CPolyTool;
    //friend class CRoiPatTool;
    //friend class CRoiTool;
    inline double GetOldZoomFactor() { return m_OldZF; }
    virtual double GetZoomFactor() { return m_ZoomFactor; }
    virtual inline void SetZoomFactor(double zf) {
        m_OldZF = m_ZoomFactor;
        m_ZoomFactor = zf;

        POSITION pos = m_objects.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = m_objects.GetNext(pos);
            pObj->SetZoomFactor(m_ZoomFactor);

        }
    }

// Operations
public:
    virtual inline void SetShowFilter(DWORD dwViewFilter) {
        POSITION pos = m_objects.GetHeadPosition();
        while (pos != NULL)
        {
            CDrawObj* pObj = m_objects.GetNext(pos);
            pObj->SetViewFilter(dwViewFilter);

        }
    }

    void Stopwatch(int start0stop1);

    //int GetLastNewIndex();
    CDrawObj* ObjectAt(const CPoint& point);
    virtual void Draw(CDC* pDC, CDrawView* pView);
    virtual void DrawLine(CDC* pDC, CDrawView* pView, int nCh);
    void Add(CDrawObj* pObj);
    void AddBefore(CDrawObj* pObj, CDrawObj* pNewObj);
    void RemoveAll();
    void Remove(CDrawObj* pObj);
    virtual void UpdateProperty() {};

public:
    int ComputePixel(double x, double y, double &x1, double &y1);

    BOOL PromptForFileName(CString& fileName, UINT nIDSTitle,
    DWORD dwFlags, BOOL bOpenFileDialog, int* pType=NULL);
    int GetIndexFromType(int nDocType, BOOL bOpenFileDialog);
    int GetTypeFromIndex(int nIndex, BOOL bOpenFileDialog);
    CString GetExtFromType(int nDocType);
    CString GetDescFromType(int nDocType);
    CString GetFileTypes(BOOL bOpenFileDialog);
    BOOL GetWritableType(int nDocType);

// Implementation
public:
    virtual ~CDrawDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual BOOL OnNewDocument();

    LARGE_INTEGER m_swFreq, m_swStart, m_swStop; //stopwatch
    float m_etime; //elapsed time

    CDrawObjList m_objects;
    CSize m_size;
    int m_nMapMode;
    COLORREF m_paperColor;

    double m_OldZF;
    double m_ZoomFactor;
    int m_nLastROIIndex;


public:
    int m_nSelectCh; //채널

    CDrawView *m_pActiveMainView; // 여러개의 View중 ROI편집기능을 가진 View Pointer를 저장
    POINT m_Sel[MAX_SEL_POINTS]; //Selection
    long m_NumSel;

    void SetChannel(int nCh) { m_nSelectCh = nCh; };
    //int GetChannel() { return m_nSelectCh; };

// Generated message map functions
public:
    //{{AFX_MSG(CDrawDoc)
    afx_msg void OnEditCopy();
    afx_msg void OnEditClear();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
