// FrmProperty.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RoilibApp.h"
#include "FrmProperty.h"
#include "RoilibAppDoc.h"
#include "RecipeView.h"
#include "MainFrm.h"
#include "ui/DrawObj.h"
#include <stack>


// CFrmProperty 대화 상자입니다.

IMPLEMENT_DYNAMIC(CFrmProperty, CDialog)

CFrmProperty::CFrmProperty(CWnd* pParent /*=NULL*/)
    : CDialog(CFrmProperty::IDD, pParent)
{
    m_bFromTspInspView = FALSE;
    m_pSelObj = NULL;
}

CFrmProperty::~CFrmProperty()
{
}

void CFrmProperty::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ROI, lstRoi);
}


BEGIN_MESSAGE_MAP(CFrmProperty, CDialog)
    ON_WM_SIZE()
    ON_WM_RBUTTONUP()
    //ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_ROI, &CFrmProperty::OnLvnKeydownListResult)
    ON_NOTIFY(NM_CLICK, IDC_LIST_ROI, OnClick)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_ROI, OnDblClick)
    ON_REGISTERED_MESSAGE(WM_XLISTCTRL_CHECKBOX_CLICKED, OnCheckbox)
    ON_REGISTERED_MESSAGE(WM_XLISTCTRL_EDIT_END, OnEditEnd)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_ROI, OnColumnClick)
END_MESSAGE_MAP()


// CFrmProperty 메시지 처리기입니다.

void CFrmProperty::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    CListCtrl* pitem = (CListCtrl*)GetDlgItem(IDC_LIST_ROI);
    if(pitem->GetSafeHwnd())
        pitem->SetWindowPos(NULL, 2, 2, cx-4, cy-4, SWP_NOZORDER);
}

BOOL CFrmProperty::OnInitDialog()
{
    CDialog::OnInitDialog();

    CXListCtrl* pListCtl = (CXListCtrl*)GetDlgItem(IDC_LIST_ROI);
    CRect rect;
    pListCtl->GetClientRect(&rect);

    CString col_name[3]={_T("Enable"), _T("Name"), _T("Inspection Type")};
    LV_COLUMN lvcolumn[3];

    for(int i=0; i<3; i++){
        lvcolumn[i].mask= LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
        lvcolumn[i].fmt = LVCFMT_CENTER;//LVCFMT_LEFT;
        lvcolumn[i].pszText=(LPTSTR)(LPCTSTR)col_name[i];
        lvcolumn[i].iSubItem=i;

        if(i == 0)
            lvcolumn[i].cx= rect.Width()*0.15;
        else lvcolumn[i].cx= rect.Width()*0.4;

        pListCtl->InsertColumn(i,&lvcolumn[i]);
    }

    // iterate through header items and attach the image list
    HDITEM hditem;

    for (int i = 0; i < pListCtl->m_HeaderCtrl.GetItemCount(); i++)
    {
        hditem.mask = HDI_IMAGE | HDI_FORMAT;
        pListCtl->m_HeaderCtrl.GetItem(i, &hditem);
        hditem.fmt |=  HDF_IMAGE;
        if (i == 0)
            hditem.iImage = XHEADERCTRL_UNCHECKED_IMAGE;
        else
            hditem.iImage = XHEADERCTRL_NO_IMAGE;

        pListCtl->m_HeaderCtrl.SetItem(i, &hditem);
    }

    pListCtl->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE | LVS_EX_HEADERDRAGDROP);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFrmProperty::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    //CDialog::OnRButtonUp(nFlags, point);
}

void CFrmProperty::AddListItem(CRoiData *pData, bool save)
{
    CXListCtrl * pListCtrl = &lstRoi;
    int cnt = pListCtrl->GetItemCount();
    CString str;

    str.Format(_T(" ")); // WorkList Enable Check Box
    pListCtrl->InsertItem(cnt, str);
    pListCtrl->SetCheckbox(cnt, 0, FALSE);

    str.Format(_T("%s"), pData->m_sName); // 이름.
    pListCtrl->SetItemText(cnt, 1, str);
    pListCtrl->SetEdit(cnt, 1);
    str = g_cRecipeData->getInspName(pData->m_nInspectType);
    pListCtrl->SetItemText(cnt, 2, str);

    DWORD_PTR pPtr = reinterpret_cast<DWORD_PTR>(pData);
    pListCtrl->SetItemData(cnt, pPtr);
}


void CFrmProperty::DeleteListItem(CRoiData* pData)
{
    //CXListCtrl * pListCtrl = &lstRoi;
    int cnt = lstRoi.GetItemCount();
    for (int nItem=0; nItem<cnt; nItem++) {
        CRoiData *pListData = (CRoiData *)lstRoi.GetItemData(nItem);
        if (pListData->sUniqueID == pData->sUniqueID) {
            lstRoi.DeleteItem(nItem);
            break;
        }
    }
}

void CFrmProperty::UpdateListItem(CRoiData* pData)
{
    //CXListCtrl * pListCtrl = &lstRoi;
    CString str;
    int cnt = lstRoi.GetItemCount();

    for (int nItem=0; nItem<cnt; nItem++) {
        DWORD_PTR pPtr = lstRoi.GetItemData(nItem);;
        CRoiData *pListData = reinterpret_cast<CRoiData*>(pPtr);
        if (pListData->sUniqueID == pData->sUniqueID) {
            lstRoi.SetItemText(nItem, 1, pData->m_sName);
            lstRoi.SetEdit(nItem, 1);
            str = g_cRecipeData->getInspName(pData->m_nInspectType);
            lstRoi.SetItemText(nItem, 2, str);
            break;
        }
    }
}

void CFrmProperty::SelectProperty(CRoiData* pData)
{
    //CXListCtrl * pListCtrl = &lstRoi;
    int cnt = lstRoi.GetItemCount();

    for (int nItem=0; nItem<cnt; nItem++) {
        DWORD_PTR pPtr = lstRoi.GetItemData(nItem);;
        CRoiData *pListData = reinterpret_cast<CRoiData*>(pPtr);
        if (pListData->sUniqueID == pData->sUniqueID) {
            lstRoi.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        } else {
            lstRoi.SetItemState(nItem, 0, LVIS_SELECTED | LVIS_FOCUSED);
        }
    }
    // 포커스를 LIST에 두니까 ROI선택을 해서 Delete 키가 먹질않는다.
    //pListCtrl->SetFocus();
}

void CFrmProperty::SetWorkListItem(CString sName)
{
    //CXListCtrl * pListCtrl = &lstRoi;
    int cnt = lstRoi.GetItemCount();

    for (int nItem=0; nItem<cnt; nItem++) {
        CString str = lstRoi.GetItemText(nItem, 1); // sName
        if (sName == str) {
            lstRoi.SetCheckbox(nItem, 0, 1);
            break;
        }
    }
    //pListCtrl->SetFocus();
}

CString CFrmProperty::GetRoiSet(CString sName)
{
    CString str;
    for(int i=0; i<g_cRecipeData->m_vecRoiData.size(); i++)
    {
        CRoiData* pdata = g_cRecipeData->m_vecRoiData[i];
        if (pdata->m_sName == sName) {
            return pdata->m_sName;
        }
    }

    return _T("");
}

///////////////////////////////////////////////////////////////////////////////
// OnCheckbox
//
// This method shows how to handle WM_XLISTCTRL_CHECKBOX_CLICKED messages
// from XListCtrl
//
LRESULT CFrmProperty::OnCheckbox(WPARAM nItem, LPARAM nSubItem)
{
    if (nItem >= 0 && nSubItem >= 0)
    {
        int state = lstRoi.GetItemCheckedState(nItem, 0);
        CString strText = lstRoi.GetItemText(nItem, 1);

        if (state == 0) {
            DelWorkList(strText);
        } else {
            DWORD_PTR pPtr = lstRoi.GetItemData(nItem);;
            CRoiData *pListData = reinterpret_cast<CRoiData*>(pPtr);
            NewWorkList(pListData);
        }
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// OnEditEnd
//
// This method shows how to handle WM_XLISTCTRL_EDIT_END messages
// from XListCtrl
//
LRESULT CFrmProperty::OnEditEnd(WPARAM nItem, LPARAM nSubItem)
{
    if (m_pSelObj == NULL)
        return FALSE;
    if (nItem >= 0 && nSubItem >= 0)
    {
        DWORD_PTR pPtr = lstRoi.GetItemData(nItem);;
        CRoiData *pListData = reinterpret_cast<CRoiData*>(pPtr);
        CString strUnique = pListData->sUniqueID;// lstRoi.GetItemText(nItem, 2);
        CString strText = lstRoi.GetItemText(nItem, nSubItem);
        //Log(_T("OnEditEnd at (%d,%d): '%s'"), nItem, nSubItem, strText);

        CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
        //CRoilibAppDoc* pDoc = (CRoilibAppDoc*)pMainFrame->GetActiveDocument();
        //POSITION pos = pDoc->GetFirstViewPosition();
        CRecipeView* pView = pMainFrame->m_pRecipeView;//(CRecipeView*)pDoc->GetNextView(pos);
        if (pView) {

            _tcscpy(m_pSelObj->m_pRoiData->m_sName, strText);
            for(int i=0; i<g_cRecipeData->m_vecRoiData.size(); i++){
                CRoiData *pListData1 = (CRoiData *)lstRoi.GetItemData(nItem);
                if (pListData1->sUniqueID == strUnique)
                {
                    _tcscpy(pListData1->m_sName, strText);
                    int nSeq = g_cRecipeData->getSeq((CRoiData *)pListData1);
                    g_cRecipeData->UpdateOneRecipeData((CRoiData *)pListData1, nSeq);
                    break;
                }
            }

            pView->Invalidate();
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnClick
//
// This method shows how to handle NM_CLICK messages from XListCtrl
//
void CFrmProperty::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int nItem = -1;
    int nSubItem = -1;
    if (pNMIA)
    {
        nItem = pNMIA->iItem;
        nSubItem = pNMIA->iSubItem;
    }

    if (nItem >= 0 && nSubItem >= 0)
    {
        CRoiData *pData = (CRoiData *)lstRoi.GetItemData(nItem);
        CString strUnique = pData->sUniqueID;// lstRoi.GetItemText(nItem, 2);
        //Log(_T("OnClick at (%d,%d):  '%s'"), nItem, nSubItem, strText);

        CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
        CRoilibAppDoc* pDoc = (CRoilibAppDoc*)pMainFrame->GetActiveDocument();
        POSITION pos;// = pDoc->GetFirstViewPosition();
        CRecipeView* pView = pMainFrame->m_pRecipeView;//(CRecipeView*)pDoc->GetNextView(pos);
        if (pView) {
            CDrawObjList *pObjects = pDoc->GetObjects();
            pos = pObjects->GetHeadPosition();
            while (pos != NULL)
            {
                CDrawObj* pObj = pObjects->GetNext(pos);
                if (pObj->m_pRoiData->sUniqueID == strUnique && pObj->m_pRoiData->m_nCh == theApp.m_nSelectCh) {
                    if (pObj->m_pParent)
                        pObj = pObj->m_pParent;
                    pView->Select(pObj);
                    if (m_bFromTspInspView == FALSE) {
                        CPoint pt = CPoint(pObj->m_position.left,pObj->m_position.bottom);
                        pView->MoveBottomLeftPosition(pt); // 선택된 ROI의 왼쪽하단부를 화면 왼쪽하단에 오도록 한다.
                    }
                    m_bFromTspInspView = FALSE;
                    m_pSelObj = pObj;
                    break;
                }
            }
        }

    }

    *pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnDblClick
//
// This method shows how to handle NM_DBLCLK messages from XListCtrl
//
void CFrmProperty::OnDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int nItem = -1;
    int nSubItem = -1;
    if (pNMIA)
    {
        nItem = pNMIA->iItem;
        nSubItem = pNMIA->iSubItem;
    }

    if (nItem >= 0 && nSubItem >= 0)
    {
        CString strText = lstRoi.GetItemText(nItem, nSubItem);
        //Log(_T("OnDblClick at (%d,%d):  '%s'"), nItem, nSubItem, strText);

        // Property Edit로 생각중..
    }

    *pResult = 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnColumnClick
//
// This method shows how to handle LVN_COLUMNCLICK messages from XListCtrl
//
void CFrmProperty::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    int nItem = -1;
    int nSubItem = -1;
    if (pNMLV)
    {
        nItem = pNMLV->iItem;
        nSubItem = pNMLV->iSubItem;

        if (nItem < 0 && nSubItem == 0) {
            DelAllWorkList();
            int nState = lstRoi.GetHeaderCheckedState(0);
            if (nState == 2) {
                NewAllWorkList();
            }

        }
    }

    *pResult = 0;
}

void CFrmProperty::DelWorkList(CString name)
{
	std::vector<CRoiData*> *pVecRoiData = &g_cRecipeData->m_vecWorkRoiData[theApp.m_nSelectCh];
    int size = pVecRoiData->size();
    for (int i = size-1; i >= 0; i--){
        CRoiData* pData = (*pVecRoiData)[i];
        if (pData->m_sName == name)
        {
            pVecRoiData->erase(pVecRoiData->begin() + i);
            break;
        }
    }
    g_cRecipeData->SaveRecipeData();
}

void CFrmProperty::NewWorkList(CRoiData* pData)
{
	std::vector<CRoiData*>* pVecRoiData = &g_cRecipeData->m_vecWorkRoiData[theApp.m_nSelectCh];
    int size = pVecRoiData->size();
    BOOL bExist = FALSE; //수정부분.
    for (int i = 0; i < size; i++){
        CRoiData* pWorkData = (*pVecRoiData)[i];
        if (pData->m_sName == pWorkData->m_sName)
        {
            (*pVecRoiData)[i] = (CRoiData*)pData;
            bExist = TRUE;
        }
    }
    if (bExist == FALSE)
        pVecRoiData->push_back((CRoiData*)pData);
    g_cRecipeData->SaveRecipeData();
}

void CFrmProperty::DelAllWorkList()
{
	std::vector<CRoiData*>* pVecRoiData = &g_cRecipeData->m_vecWorkRoiData[theApp.m_nSelectCh];
    pVecRoiData->clear();
    g_cRecipeData->SaveRecipeData();
}

void CFrmProperty::NewAllWorkList()
{
    //CString str;
	std::vector<CRoiData*>* pVecRoiData = &g_cRecipeData->m_vecWorkRoiData[theApp.m_nSelectCh];
    pVecRoiData->clear();
    for (int i = 0; i<g_cRecipeData->m_vecRoiData.size(); i++){

        if(g_cRecipeData->m_vecRoiData[i]->m_nCh != theApp.m_nSelectCh) continue;//채널이 다른 roiData일 경우 등록할 필요가 없음.
        CRoiData* pdata = g_cRecipeData->m_vecRoiData[i];
        pVecRoiData->push_back(pdata);
    }
    g_cRecipeData->SaveRecipeData();
}
