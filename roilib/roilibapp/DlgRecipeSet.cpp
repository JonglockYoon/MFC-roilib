// RecipeSetDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgRecipeSet.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "RoilibAppView.h"

// CDlgRecipeSet 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRecipeSet, CDialogEx)

CDlgRecipeSet::CDlgRecipeSet(CWnd* pParent /*=NULL*/)
    : CDialogEx(CDlgRecipeSet::IDD, pParent)
{

}

CDlgRecipeSet::~CDlgRecipeSet()
{
}

void CDlgRecipeSet::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_RECIPE_MAN, m_ListRecipeList);
    DDX_Control(pDX, IDC_COMBO_COPY_SRCLIST, m_ComboSrcRecipe);
    DDX_Control(pDX, IDC_COMBO_COPY_DSTLIST, m_ComboDstRecipe);
}


BEGIN_MESSAGE_MAP(CDlgRecipeSet, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_COPY_RCP, &CDlgRecipeSet::OnBnClickedBtnCopyRcp)
    ON_BN_CLICKED(IDC_BTN_LOAD_RECIPE, &CDlgRecipeSet::OnBnClickedBtnLoadRecipe)
    ON_BN_CLICKED(IDC_BTN_DEL_RECIPE, &CDlgRecipeSet::OnBnClickedBtnDelRecipe)
    ON_BN_CLICKED(IDC_BTN_RCPUP, &CDlgRecipeSet::OnBnClickedBtnRcpup)
    ON_BN_CLICKED(IDC_BTN_RCPDOWN, &CDlgRecipeSet::OnBnClickedBtnRcpdown)
    ON_BN_CLICKED(IDC_BTN_NEW_RECIPE, &CDlgRecipeSet::OnBnClickedBtnNewRecipe)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_RECIPE_MAN, &CDlgRecipeSet::OnNMCustomdrawListRecipeMan)
    ON_NOTIFY(NM_CLICK, IDC_LIST_RECIPE_MAN, &CDlgRecipeSet::OnNMClickListRecipeMan)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgRecipeSet 메시지 처리기입니다.


void CDlgRecipeSet::OnBnClickedBtnCopyRcp()
{
    COPY_RECIPE_INFO * ptCopyRcpInfo;
    ptCopyRcpInfo = new COPY_RECIPE_INFO();

    CString str;
    int nRet;
    int nSrcRcpIndex;
    int nDstRcpIndex;
    nSrcRcpIndex = nDstRcpIndex = -1;

    UpdateData(TRUE);

    nSrcRcpIndex = m_ComboSrcRecipe.GetCurSel();
    nDstRcpIndex = m_ComboDstRecipe.GetCurSel();

    if(nSrcRcpIndex == -1){
        AfxMessageBox(_S(IDS_NO_SELECT_COPYED_RECIPE));
        delete ptCopyRcpInfo;
        return;
    }
    else if(nDstRcpIndex == -1){
        AfxMessageBox(_S(IDS_NO_SELECT_TARGET_RECIPE));
        delete ptCopyRcpInfo;
        return;
    }
    else if(nSrcRcpIndex == nDstRcpIndex){
        AfxMessageBox(_S(IDS_NO_SAME_COPYRECIPE_TARGETRECIPE));
        delete ptCopyRcpInfo;
        return;
    }

    str.Format(_S(IDS_EXCHANGE_RECIPES, theApp.m_vecRecipeInfo[nDstRcpIndex].sRecipeName, theApp.m_vecRecipeInfo[nSrcRcpIndex].sRecipeName));
    nRet = AfxMessageBox(str, MB_OKCANCEL);
    if(nRet == IDCANCEL){
        delete ptCopyRcpInfo;
        return;
    }

    CString sSrcFilePath;
    CString sDstFilePath;
    ptCopyRcpInfo->sSrcRcpName.Format(_T("%s"), theApp.m_vecRecipeInfo[nSrcRcpIndex].sRecipeName);
    ptCopyRcpInfo->sDstRcpName.Format(_T("%s"), theApp.m_vecRecipeInfo[nDstRcpIndex].sRecipeName);
    sSrcFilePath.Format(_T("%s\\Recipe\\%s\\%s.ini"), theApp.g_sRootPath, ptCopyRcpInfo->sSrcRcpName, ptCopyRcpInfo->sSrcRcpName);
    sDstFilePath.Format(_T("%s\\Recipe\\%s\\%s.ini"), theApp.g_sRootPath, ptCopyRcpInfo->sDstRcpName, ptCopyRcpInfo->sDstRcpName);
    CopyFile(sSrcFilePath, sDstFilePath, FALSE);

    DefineRecipeList_Update(&m_ListRecipeList, gCfg.m_sLastRecipeName);
    delete ptCopyRcpInfo;
}


BOOL CDlgRecipeSet::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CString str;
    RecipeList_AddColum(&m_ListRecipeList);
    DefineRecipeList_Update(&m_ListRecipeList, gCfg.m_sLastRecipeName);
    ComboCopyList_Update();
    str.Format(_S(IDS_CURRENT_RECIPE2, gCfg.m_sLastRecipeName));
    SetDlgItemText(IDC_STATIC_CURRCP, str);

    GetDlgItem(IDC_STATIC_CURRCP)->SetFont(&g_pUiObject->font_100);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgRecipeSet::RecipeList_AddColum(CListCtrl* pListCtl)
{
    CRect rect;
    pListCtl->GetClientRect(&rect);
    CString strTemp;

    CString col_name[4]={_S(IDS_INDEX), _S(IDS_RECIPE_NAME), _S(IDS_LONG_FACE), _S(IDS_SHORT_FACE)};
    LV_COLUMN lvcolumn[4];

    for(int i=0; i<2; i++){
        lvcolumn[i].mask= LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
        lvcolumn[i].fmt = LVCFMT_CENTER;
        lvcolumn[i].pszText=(LPTSTR)(LPCTSTR)col_name[i];
        lvcolumn[i].iSubItem=i;

        switch(i){
        case 0:
            lvcolumn[i].cx=rect.Width()*0.15;
            break;
        case 1:
            lvcolumn[i].cx = rect.Width()*0.8;// 0.40;
            break;
        default:
            lvcolumn[i].cx=rect.Width()*0.25;
            break;
        }
        pListCtl->InsertColumn(i,&lvcolumn[i]);
    }
    pListCtl->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE | LVS_EX_HEADERDRAGDROP);
}


void CDlgRecipeSet::DefineRecipeList_Update(CListCtrl* pListCtl, CString sCurName)
{
    CString str;
    pListCtl->DeleteAllItems();
    for(int i=0; i<(int)theApp.m_vecRecipeInfo.size(); i++){

        str.Format(_T("%02d"), i+1);
        pListCtl->InsertItem(i,str);
        pListCtl->SetItemText(i,1,theApp.m_vecRecipeInfo[i].sRecipeName);

        if(theApp.m_vecRecipeInfo[i].sRecipeName == sCurName){
            m_nSelectRecipeIndex = i;
        }
    }
}


void CDlgRecipeSet::ComboCopyList_Update(void)
{
    m_ComboSrcRecipe.ResetContent();
    m_ComboDstRecipe.ResetContent();

    for(int i=0; i<(int)theApp.m_vecRecipeInfo.size(); i++){
        m_ComboSrcRecipe.InsertString(i, theApp.m_vecRecipeInfo[i].sRecipeName);
        m_ComboDstRecipe.InsertString(i, theApp.m_vecRecipeInfo[i].sRecipeName);
    }
}

void CDlgRecipeSet::OnBnClickedBtnLoadRecipe()
{
    if (m_nSelectRecipeIndex < 0)
        return;

    CString sName, str;
    sName.Format(_T("%s"), m_ListRecipeList.GetItemText(m_nSelectRecipeIndex, 1));

    if (sName == gCfg.m_sLastRecipeName)
        return;

    theApp.m_nSelectCh = 0;
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    for (int i = 0; i < MAX_CH_NUM; i++)
        if (pMainFrame->m_pRecipeView->m_ximage[i]) {
            delete pMainFrame->m_pRecipeView->m_ximage[i];
            pMainFrame->m_pRecipeView->m_ximage[i] = NULL;
        }

    g_cRecipeData->ClearImageBuff();
    g_cRecipeData->m_vecRoiData.clear();

    //레시피 경로 변경후 읽어 들이기 - 레시피 Ini 파일이 존재하지 않을 경우 기본값을 적용하여 불러들인다.//
    gCfg.m_sLastRecipeName.Format(_T("%s"), sName);//레시피 이름을 변경하여 준다.
    g_cRecipeData->m_sFileName.Format(_T("%s\\Recipe\\%s\\%s.ini"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, gCfg.m_sLastRecipeName);

    CRect rect;
    m_ListRecipeList.GetClientRect(rect);
    m_ListRecipeList.RedrawWindow(rect);

    str.Format(_S(IDS_CURRENT_RECIPE2, gCfg.m_sLastRecipeName));
    SetDlgItemText(IDC_STATIC_CURRCP, str);

    pMainFrame->RecipeLoad();
    pMainFrame->m_pRecipeView->OnInspectionChannel();
    pMainFrame->m_pRecipeView->Invalidate();

    theApp.m_ImgProcEngine.InitEngine(theApp.m_nSelectCh);
}


void CDlgRecipeSet::OnBnClickedBtnDelRecipe()
{
    if(m_nSelectRecipeIndex == -1) return;

    CString sPath, strDelPath;
    CString str = m_ListRecipeList.GetItemText(m_nSelectRecipeIndex, 1);

    if(str == gCfg.m_sLastRecipeName){
        AfxMessageBox(_S(IDS_NO_REMOVE_RECIPE_USING_PROGRAM));
        return;
    }

    g_cRecipeData->ClearImageBuff();
    g_cRecipeData->m_vecRoiData.clear();

    for (int i = 0; i<(int)theApp.m_vecRecipeInfo.size(); i++){
        if(str == theApp.m_vecRecipeInfo[i].sRecipeName){
            theApp.m_vecRecipeInfo.erase(theApp.m_vecRecipeInfo.begin() + i);
            break;
        }
    }

    DefineRecipeList_Update(&m_ListRecipeList, gCfg.m_sLastRecipeName);
    ComboCopyList_Update();
    g_cRecipeData->WriteRecipeList();

    theApp.m_ImgProcEngine.InitEngine(theApp.m_nSelectCh);

}


void CDlgRecipeSet::OnBnClickedBtnRcpup()
{
    if(m_nSelectRecipeIndex == -1) return;
    if(m_nSelectRecipeIndex > (int)theApp.m_vecRecipeInfo.size()) return;
    if(m_nSelectRecipeIndex == 0) return;

    vector<RecipeListInformation> tTempData;

    tTempData.resize((int)theApp.m_vecRecipeInfo.size());

    for(int i=0; i<(int)theApp.m_vecRecipeInfo.size(); i++){
        if(i == m_nSelectRecipeIndex){
            tTempData[i-1] = theApp.m_vecRecipeInfo[i];
            tTempData[i] = theApp.m_vecRecipeInfo[i-1];
        }
        else{
            tTempData[i] = theApp.m_vecRecipeInfo[i];
        }
    }
    theApp.m_vecRecipeInfo.clear();
    theApp.m_vecRecipeInfo.resize((int)tTempData.size());
    copy(tTempData.begin(), tTempData.end(), theApp.m_vecRecipeInfo.begin());

    m_nSelectRecipeIndex -= 1;

    DefineRecipeList_Update(&m_ListRecipeList);
    ComboCopyList_Update();
    g_cRecipeData->WriteRecipeList();
}


void CDlgRecipeSet::OnBnClickedBtnRcpdown()
{
    if(m_nSelectRecipeIndex == -1) return;
    if(m_nSelectRecipeIndex > (int)theApp.m_vecRecipeInfo.size()) return;
    if(m_nSelectRecipeIndex == (int)theApp.m_vecRecipeInfo.size()-1) return;

    vector<RecipeListInformation> tTempData;

    tTempData.resize((int)theApp.m_vecRecipeInfo.size());
    for(int i=0; i<(int)theApp.m_vecRecipeInfo.size(); i++){
        if(i == m_nSelectRecipeIndex){
            tTempData[i] = theApp.m_vecRecipeInfo[i+1];
            tTempData[i+1] = theApp.m_vecRecipeInfo[i];
            i++;
        }
        else{
            tTempData[i] = theApp.m_vecRecipeInfo[i];
        }
    }

    theApp.m_vecRecipeInfo.clear();
    theApp.m_vecRecipeInfo.resize((int)tTempData.size());
    copy(tTempData.begin(), tTempData.end(), theApp.m_vecRecipeInfo.begin());

    m_nSelectRecipeIndex += 1;

    DefineRecipeList_Update(&m_ListRecipeList);
    ComboCopyList_Update();
    g_cRecipeData->WriteRecipeList();
}

void CDlgRecipeSet::OnBnClickedBtnNewRecipe()
{

    CString sName, str, sPath;
    CString sSrcFilePath, sDstFilePath;

    CString strNewModel, strCurModel;
    RecipeListInformation tNewRcpData;

    GetDlgItemText(IDC_EDIT_NEWRCP_NAME, str);
    if(str.GetLength() == 0){
        AfxMessageBox(_S(IDS_INPUT_RECIPE_NAME));
        return;
    }
    tNewRcpData.sRecipeName.Format(_T("%s"), str);

    tNewRcpData.sRecipeName.TrimLeft();	//문자의 좌측 공백을 없애준다.
    tNewRcpData.sRecipeName.TrimRight();	//문자의 우측 공백을 없애준다.

    if(tNewRcpData.sRecipeName.GetLength() <= 0){//문자의 길이를 비교한다.
        AfxMessageBox(_S(IDS_INPUT_RECIPE_NAME));
        return;
    }

    for(int i=0; i<m_ListRecipeList.GetItemCount(); i++){//레시피 목록과 현재 등록할려는 레시피에 대한 이름을 비교한다.
        if(tNewRcpData.sRecipeName == m_ListRecipeList.GetItemText(i, 1)){
            AfxMessageBox(_S(IDS_DUPLICATE_RECIPE));
            return;
        }
    }

    theApp.m_nSelectCh = 0;
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
    for (int i = 0; i < MAX_CH_NUM; i++)
        if (pMainFrame->m_pRecipeView->m_ximage[i]) {
            delete pMainFrame->m_pRecipeView->m_ximage[i];
            pMainFrame->m_pRecipeView->m_ximage[i] = NULL;
        }

    g_cRecipeData->ClearImageBuff();
    g_cRecipeData->m_vecRoiData.clear();

    //레시피 폴더 생성
    sPath.Format(_T("%s\\Recipe\\%s"), theApp.g_sRootPath, tNewRcpData.sRecipeName);
    g_pDirectory->Create_Directory(sPath);

    gCfg.m_sLastRecipeName.Format(_T("%s"), tNewRcpData.sRecipeName);//레시피 이름을 변경하여 준다.
    g_cRecipeData->m_sFileName.Format(_T("%s\\Recipe\\%s\\%s.ini"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, gCfg.m_sLastRecipeName);

    g_cRecipeData->SaveRecipeData();
    g_cRecipeData->LoadRecipeData();

    theApp.m_vecRecipeInfo.push_back(tNewRcpData);
    DefineRecipeList_Update(&m_ListRecipeList, gCfg.m_sLastRecipeName);
    ComboCopyList_Update();
    g_cRecipeData->WriteRecipeList();

    CString *pStr;
    pStr = new CString();
    pStr->Format(_T("%s"), gCfg.m_sLastRecipeName);

    str.Format(_S(IDS_CURRENT_RECIPE2, gCfg.m_sLastRecipeName));
    SetDlgItemText(IDC_STATIC_CURRCP, str);

    str.Format(_T(""));
    SetDlgItemText(IDC_EDIT_NEWRCP_NAME, str);
    SetDlgItemText(IDC_EDIT_NEWRCP_HEIGHT, str);
    SetDlgItemText(IDC_EDIT_NEWRCP_WIDTH, str);

    pMainFrame->RecipeLoad();
    pMainFrame->m_pRecipeView->OnInspectionChannel();
    pMainFrame->m_pRecipeView->Invalidate();

    theApp.m_ImgProcEngine.InitEngine(theApp.m_nSelectCh);
}


void CDlgRecipeSet::OnNMCustomdrawListRecipeMan(NMHDR *pNMHDR, LRESULT *pResult)
{
    //LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    NMLVCUSTOMDRAW * pLVCD = (NMLVCUSTOMDRAW *)pNMHDR;
    *pResult = 0;

    switch(pLVCD->nmcd.dwDrawStage){
    case CDDS_PREPAINT:
        {
            *pResult = CDRF_NOTIFYITEMDRAW;
            break;
        }
    case CDDS_ITEMPREPAINT://Item
        {
            if(m_ListRecipeList.GetItemText(pLVCD->nmcd.dwItemSpec, 1) == gCfg.m_sLastRecipeName){
                pLVCD->clrTextBk = RGB(250, 50, 125);
            }
            else{
                pLVCD->clrTextBk = RGB(255, 255, 255);
            }
            *pResult = CDRF_DODEFAULT;
            break;
        }
    }
}


void CDlgRecipeSet::RecipeInfoListGet(void)
{
}


void CDlgRecipeSet::OnNMClickListRecipeMan(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    if(pNMItemActivate->iItem < 0)
        return;

    m_nSelectRecipeIndex = pNMItemActivate->iItem;

    *pResult = 0;
}


void CDlgRecipeSet::OnTimer(UINT_PTR nIDEvent)
{

    CDialogEx::OnTimer(nIDEvent);
}
