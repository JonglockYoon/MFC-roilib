// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

// RoilibAppDoc.h : CRoilibAppDoc 클래스의 인터페이스
//
/////////////////////////////////////////////////////////////////////////////
#define MAX_UNDO_LEVELS 8

#define	WM_USER_NEWIMAGE WM_USER+1
#define WM_USER_PROGRESS WM_USER+2

#pragma once

#include "ui/DrawDoc.h"
#include "cximage/include/ximajpg.h"

//struct DlgDataExtInfo
//{
//	float	fYmin;
//	float	fYmax;
//	float	fXmin;
//	float	fXmax;
//	BYTE	nThresh;
//	COLORREF cColor;
//	bool	bMinmax;
//	bool	bAvg;
//	bool	bDetect;
//	bool	bLogXaxis;
//	bool	bLogYaxis;
//};

class CxImage;
class CRoilibAppDoc : public CDrawDoc
{
protected: // serialization에서만 만들어집니다.
    CRoilibAppDoc();
    DECLARE_DYNCREATE(CRoilibAppDoc)

// 특성입니다.
public:

// 작업입니다.
    //CxImage *image;				//main image
    //CxImage::CXTEXTINFO  m_text; // CxImage모듈
    //BOOL m_bInspResultView;

public:
    //CRoilibAppView *m_pActiveMainView; // 여러개의 View중 ROI편집기능을 가진 View Pointer를 저장

// 재정의입니다.
    void SubmitUndo();
    //inline CxImage *GetImage() { return image; }
    //inline BOOL GetStretchMode() { return stretchMode; }
    //inline BOOL GetSmoothMode() { return m_bSmoothDisplay; }
    CString FindExtension(const CString& name);
    CString RemoveExtension(const CString& name);
    int FindType(const CString& ext);
    void RegionRotateLeft();
    void RegionRotateRight();

    void RoiHidden(BOOL flag);
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
    virtual void InitializeSearchContent();
    virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);



// 구현입니다.
public:
    void UpdateStatusBar();
    virtual ~CRoilibAppDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    //CxImageJPG m_exif;

    //BOOL stretchMode;
    //BOOL m_bSmoothDisplay;

public:

// 생성된 메시지 맵 함수
protected:
    DECLARE_MESSAGE_MAP()
public:

#ifdef SHARED_HANDLERS
    // 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
    void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

    afx_msg void OnRecipeLoad();
};
