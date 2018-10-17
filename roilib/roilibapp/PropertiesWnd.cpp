#include "stdafx.h"
#include "Resource.h"

#include "PropertiesWnd.h"
#include "RoilibApp.h"
#include "MainFrm.h"
#include "ui/DrawObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_wndPropList.Create(IDD_FORM_ROI, this);
    m_wndPropList.ShowWindow(SW_SHOW);

    InitPropList();
    return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if(m_wndPropList.GetSafeHwnd())
        m_wndPropList.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);
}

void CPropertiesWnd::InitPropList()
{
    SetPropListFont();

    if (g_cRecipeData == NULL)
        return;

    CXListCtrl * pListCtrl = &m_wndPropList.lstRoi;
    pListCtrl->DeleteAllItems();

    for (int nItem = 0; nItem < g_cRecipeData->m_vecRoiData.size(); nItem++) {
        CRoiData* pData = g_cRecipeData->m_vecRoiData[nItem];
        if (pData->m_nCh != theApp.m_nSelectCh)
            continue;

        m_wndPropList.AddListItem(pData, true);
    }

    int	nContentType;
    int nObjIndex;

    int size = g_cRecipeData->m_vecWorkRoiData[theApp.m_nSelectCh].size();
    for (int i = 0; i<size; i++){
        CString name = g_cRecipeData->m_vecWorkRoiData[theApp.m_nSelectCh][i]->m_sName;
        if(!name.IsEmpty()) {
            m_wndPropList.SetWorkListItem(name);
        }
    }

    m_wndPropList.RedrawWindow();
}

void CPropertiesWnd::AddPropertyItem(CDrawObj *pObj)
{
    CRoiData *pData = pObj->m_pRoiData;
    m_wndPropList.m_pSelObj = pObj;

    m_wndPropList.AddListItem(pData, false);

}
void CPropertiesWnd::DeletePropertyItem(CDrawObj *pObj)
{
    CRoiData* pData = pObj->m_pRoiData;
    //TRACE("DeletePropertyItem=%d\n", pObj->m_roiData.nIndex);
    m_wndPropList.m_pSelObj = NULL;

    m_wndPropList.DeleteListItem(pData);
}
void CPropertiesWnd::UpdatePropertyItem(CDrawObj *pObj)
{
    if (pObj->m_nShape != CDrawObj::line)
        pObj->m_position.NormalizeRect();
    CRoiData* pData = pObj->m_pRoiData;
    //TRACE("UpdatePropertyItem=%d\n", pObj->m_roiData.nIndex);
    m_wndPropList.m_pSelObj = pObj;

    m_wndPropList.UpdateListItem(pData);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);
    m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CDockablePane::OnSettingChange(uFlags, lpszSection);
    SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
    ::DeleteObject(m_fntPropList.Detach());

    LOGFONT lf;
    afxGlobalData.fontRegular.GetLogFont(&lf);

    NONCLIENTMETRICS info;
    info.cbSize = sizeof(info);

    afxGlobalData.GetNonClientMetrics(info);

    lf.lfHeight = info.lfMenuFont.lfHeight;
    lf.lfWeight = info.lfMenuFont.lfWeight;
    lf.lfItalic = info.lfMenuFont.lfItalic;

    m_fntPropList.CreateFontIndirect(&lf);

    m_wndPropList.SetFont(&m_fntPropList);
}


//
// pView에서 CDrawObj한개가 선택되면 여기 PropertyList에도 선택되었다는것을 표시한다.
//

void CPropertiesWnd::SetSelectProperty(CDrawObj* pObj)
{
    m_wndPropList.m_pSelObj = pObj;
    m_wndPropList.SelectProperty(pObj->m_pRoiData);
}
