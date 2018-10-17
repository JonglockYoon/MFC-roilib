// CustomRibbonBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RoilibApp.h"
#include "CustomRibbonBar.h"


// CCustomRibbonBar

IMPLEMENT_DYNAMIC(CCustomRibbonBar, CMFCRibbonBar)

CCustomRibbonBar::CCustomRibbonBar()
{

}

CCustomRibbonBar::~CCustomRibbonBar()
{
}


BEGIN_MESSAGE_MAP(CCustomRibbonBar, CMFCRibbonBar)
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CCustomRibbonBar 메시지 처리기입니다.

BOOL CCustomRibbonBar::PreTranslateMessage(MSG* pMsg)
{
//If command was finishing a click
    if(pMsg->message == WM_LBUTTONUP && pMsg->wParam == 0)
    {

        //Allow ribbon to handle itself first
         BOOL result = CMFCRibbonBar::PreTranslateMessage(pMsg);

         //Get new active tab
         int activeTab = GetCategoryIndex(GetActiveCategory());

         //If tab has changed, forward message
         if(activeTab != m_LastActiveCategory)
         {
              //forward message to parent
            ::SendMessage(GetParentFrame()->GetSafeHwnd(), WM_USER_ACTIVE_RIBBON_TAB_CHANGED, activeTab, 0);
            m_LastActiveCategory = activeTab;
         }

         return result;
    }

    //Otherwise handle like normal
    return CMFCRibbonBar::PreTranslateMessage(pMsg);
}


void CCustomRibbonBar::OnSize(UINT nType, int cx, int cy)
{
    CMFCRibbonBar::OnSize(nType, cx, cy);

    BOOL bState = (GetHideFlags() & AFX_RIBBONBAR_HIDE_ELEMENTS);
//	if (!(GetHideFlags() & AFX_RIBBONBAR_HIDE_ELEMENTS))
//		ToggleMimimizeState();

    ::SendMessage(GetParentFrame()->GetSafeHwnd(), WM_USER_ACTIVE_RIBBON_SIZE, bState, 0);
}
