//
// Copyright (C) 2018
// All rights reserved by jerry1455@gmail.com
//
#include "stdafx.h"

#include "LogWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "RoilibApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

CLogBarWnd::CLogBarWnd()
{
}

CLogBarWnd::~CLogBarWnd()
{
}

BEGIN_MESSAGE_MAP(CLogBarWnd, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

int CLogBarWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // 탭 창을 만듭니다.
    if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
    {
        TRACE0(_S(IDS_FAILED_CREATE_LOG_TAB));
        return -1;      // 만들지 못했습니다.
    }

    // 출력 창을 만듭니다.
    const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

    if (!m_wndLog[0].Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
        !m_wndLog[1].Create(dwStyle, rectDummy, &m_wndTabs, 3))
    {
        TRACE0(_S(IDS_FAILED_CREATE_LOG_WINDOW));
        return -1;      // 만들지 못했습니다.
    }

    UpdateFonts();

    CString strTabName;
    BOOL bNameValid;

    // 탭에 목록 창을 연결합니다.
    bNameValid = strTabName.LoadString(IDS_LOG_SYST_TAB);
    ASSERT(bNameValid);
    m_wndTabs.AddTab(&m_wndLog[0], strTabName, (UINT)0);
    bNameValid = strTabName.LoadString(IDS_LOG_INSP_TAB);
    ASSERT(bNameValid);
    m_wndTabs.AddTab(&m_wndLog[1], strTabName, (UINT)1);

    // 로그 탭을 더미 텍스트로 채웁니다.
    FillLogWindow();

    return 0;
}

void CLogBarWnd::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    // Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
    m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CLogBarWnd::AdjustHorzScroll(CListBox& wndListBox)
{
    CClientDC dc(this);
    CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

    int cxExtentMax = 0;

    for (int i = 0; i < wndListBox.GetCount(); i ++)
    {
        CString strItem;
        wndListBox.GetText(i, strItem);

        cxExtentMax = MAX(cxExtentMax, dc.GetTextExtent(strItem).cx);
    }

    wndListBox.SetHorizontalExtent(cxExtentMax);
    dc.SelectObject(pOldFont);
}

void CLogBarWnd::FillLogWindow()
{
    m_wndLog[0].AddString(_S(IDS_SYSTEM_LOG_PRINTED));
    m_wndLog[1].AddString(_S(IDS_INSPECTION_LOG_PRINTED));
}
void CLogBarWnd::UpdateFonts()
{
    for (int i=0; i<2; i++)
        m_wndLog[i].SetFont(&afxGlobalData.fontRegular);
}

/////////////////////////////////////////////////////////////////////////////
// CLogList

CLogList::CLogList()
{
}

CLogList::~CLogList()
{
}

BEGIN_MESSAGE_MAP(CLogList, CListBox)
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CLogList 메시지 처리기.

void CLogList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    CMenu menu;
    menu.LoadMenu(IDR_OUTPUT_POPUP);

    CMenu* pSumMenu = menu.GetSubMenu(0);

    if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
    {
        CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

        if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
            return;

        ((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
        UpdateDialogControls(this, FALSE);
    }

    SetFocus();
}

void CLogList::OnEditCopy()
{
    MessageBox(_S(IDS_COPY_LOG));
}

void CLogList::OnEditClear()
{
    MessageBox(_S(IDS_CLEAR_LOG));
}

void CLogList::OnViewOutput()
{
    CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
    CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

    if (pMainFrame != NULL && pParentBar != NULL)
    {
        pMainFrame->SetFocus();
        pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
        pMainFrame->RecalcLayout();

    }
}
void CLogList::AddString(CString s)
{
    int nLen = GetCount();
    if(nLen > MAXLOG) DeleteString(0);

    SetCurSel(CListBox::AddString(s));
}
