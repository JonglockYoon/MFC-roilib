//
// Copyright (C) 2018
// All rights reserved by jerry1455@gmail.com
//


// RoilibAppDoc.cpp : CRoilibAppDoc 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#include "RoilibApp.h"
#include "MainFrm.h"
#include "RoilibAppDoc.h"
#include "RoilibAppView.h"
#include <propkey.h>
#include "IniUtil.h"
#include "Utils/Path.h"
#include "DlgRecipeSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRoilibAppDoc

IMPLEMENT_DYNCREATE(CRoilibAppDoc, CDrawDoc)

BEGIN_MESSAGE_MAP(CRoilibAppDoc, CDrawDoc)
    ON_COMMAND(ID_RECIPE_LOAD, &OnRecipeLoad)
END_MESSAGE_MAP()


// CRoilibAppDoc 생성/소멸

CRoilibAppDoc::CRoilibAppDoc()
{
    m_NumSel=0;
    m_pActiveMainView = NULL;
    SetChannel(theApp.m_nSelectCh);
    QueryPerformanceFrequency(&m_swFreq);
}

CRoilibAppDoc::~CRoilibAppDoc()
{
}

CString CRoilibAppDoc::FindExtension(const CString& name)
{
    int len = name.GetLength();
    int i;
    for (i = len-1; i >= 0; i--){
        if (name[i] == '.'){
            return name.Mid(i+1);
        }
    }
    return CString(_T(""));
}
//////////////////////////////////////////////////////////////////////////////
CString CRoilibAppDoc::RemoveExtension(const CString& name)
{
    int len = name.GetLength();
    int i;
    for (i = len-1; i >= 0; i--){
        if (name[i] == '.'){
            return name.Mid(0,i);
        }
    }
    return name;
}
//////////////////////////////////////////////////////////////////////////////
int CRoilibAppDoc::FindType(const CString& ext)
{
    return CxImage::GetTypeIdFromName(ext);
}
//////////////////////////////////////////////////////////////////////////////

BOOL CRoilibAppDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: 여기에 재초기화 코드를 추가합니다.
    // SDI 문서는 이 문서를 다시 사용합니다.
    //SetModifiedFlag();

    return TRUE;
}

BOOL CRoilibAppDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    return FALSE; // TRUE->FALSE 동일파일을 Open하더러도 이함수가 호출될수 있게 변경.
}

BOOL CRoilibAppDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
    CxImage *pximage = NULL;
    if (m_pActiveMainView->IsKindOf(RUNTIME_CLASS(CRecipeView)))
    {
        CRecipeView *pView = (CRecipeView *)m_pActiveMainView;
        pximage = pView->m_ximage[theApp.m_nSelectCh];
    }
    else {
        CRoilibAppView *pView = (CRoilibAppView *)m_pActiveMainView;
        pximage = pView->m_ximage;
    }
    if (pximage == NULL) {
        AfxMessageBox(_T("Did not saved image, there is no image information."));
        return false;
    }
    int cx = pximage->GetWidth();
    int cy = pximage->GetHeight();
    if (cx <= 0 || cy <= 0) {
        AfxMessageBox(_T("Did not saved image, there is no image information."));
        return false;
    }

/*
    static CString newName;
    if (!PromptForFileName(newName, AFX_IDS_SAVEFILE,
      OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, 0))
        return false; // open cancelled
*/
    CString filename = lpszPathName;//newName;
    CString ext = FindExtension(filename);

    int type = FindType(ext);
    if (type == CXIMAGE_FORMAT_UNKNOWN) {
        AfxMessageBox(_T("You must enter the extensions as bmp or png, etc."));
        return false;
    }

    if (type == CXIMAGE_FORMAT_GIF && m_pActiveMainView->m_ximage->GetBpp()>8){
        AfxMessageBox(_T("The image will be saved as a true color GIF!\nThis is ok for CxImage, but not for many other programs.\nFor better compatibility, please use DecreaseBpp to 8 bits or less."),MB_ICONINFORMATION);
    }

    bool retval;
    if (pximage != NULL)
        retval = pximage->Save(filename, type);

    UpdateStatusBar();
    if (retval)
        return true;

    //CString s = CString(m_pActiveMainView->m_ximage->GetLastError());
    //AfxMessageBox(s);
    return false;
}

// CRoilibAppDoc serialization

//////////////////////////////////////////////////////////////////////////////
void CRoilibAppDoc::UpdateStatusBar()
{
    if (m_pActiveMainView == NULL)
        return;
    if (m_pActiveMainView->m_ximage != NULL)
    {
        CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
        if (pMainFrame == NULL)
            return;

        CMFCRibbonStatusBar& statusBar = pMainFrame->GetStatusBar();
        //int n = statusBar.GetCount();

        if (m_pActiveMainView != NULL)
        {
            CxImage *pximage = NULL;

            if (m_pActiveMainView->IsKindOf(RUNTIME_CLASS(CRecipeView)))
            {
                CRecipeView *pView = (CRecipeView *)m_pActiveMainView;
                pximage = pView->m_ximage[theApp.m_nSelectCh];
            }
            else {
                CRoilibAppView *pView = (CRoilibAppView *)m_pActiveMainView;
                pximage = pView->m_ximage;
            }


            CString s, t;
            s.Format(_T("%4.0f %%"), m_pActiveMainView->GetZoomFactor() * 100);
            statusBar.GetExElement(0)->SetText(s);

            if (pximage != NULL)
            {
                t = GetDescFromType(pximage->GetType());
                s.Format(_T("(%dx%dx%d)"), pximage->GetWidth(), pximage->GetHeight(), pximage->GetBpp());
                statusBar.GetExElement(1)->SetText(s);
                statusBar.GetExElement(2)->SetText(t.Mid(0, 3));
            }
            statusBar.Invalidate();
        }
    }
}
void CRoilibAppDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: 여기에 저장 코드를 추가합니다.
    }
    else
    {
        // TODO: 여기에 로딩 코드를 추가합니다.
    }
}

// CRoilibAppDoc 진단

#ifdef _DEBUG
void CRoilibAppDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CRoilibAppDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CRoilibAppDoc 명령

void CRoilibAppDoc::OnRecipeLoad()
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    CDlgRecipeSet dlg;
    g_cRecipeData->ReadRecipeList();
    dlg.DoModal();
}
