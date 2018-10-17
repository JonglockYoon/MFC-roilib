// NaButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NaButton.h"


// CNaButton

IMPLEMENT_DYNAMIC(CNaButton, CMFCButton)

CNaButton::CNaButton()
{

}

CNaButton::~CNaButton()
{
}


BEGIN_MESSAGE_MAP(CNaButton, CMFCButton)
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CNaButton 메시지 처리기입니다.




void CNaButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    GetParent()->PostMessage(WM_COMMAND, GetDlgCtrlID() + COMMAND_MSG_LMOUSE_UP, (LPARAM)m_hWnd);

    CMFCButton::OnLButtonUp(nFlags, point);
}


void CNaButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    GetParent()->PostMessage(WM_COMMAND, GetDlgCtrlID() + COMMAND_MSG_LMOUSE_DOWN, (LPARAM)m_hWnd);

    CMFCButton::OnLButtonDown(nFlags, point);
}
