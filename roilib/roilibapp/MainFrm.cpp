//
// Copyright (C) 2018
// All rights reserved by jerry1455@gmail.com
//

// MainFrm.cpp : CMainFrame 클래스의 구현.
//

#include "stdafx.h"
#include "MainFrm.h"
#include "RoilibAppDoc.h"
#include "RoilibAppView.h"
#include "Utils/Utils.h"
#include "ProcessingClass.h"
#include "IniUtil.h"
#include "DlgConfig.h"
#include "DlgObject.h"
#include <algorithm>
#include <dshow.h>
#include "Utils/String.h"

#pragma comment(lib, "strmiids")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CSDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CSDIFrameWnd)
    ON_WM_CREATE()
    ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
    ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
    ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
    ON_WM_SETTINGCHANGE()
    ON_COMMAND(ID_VIEW_ALLBAR, &CMainFrame::OnViewAllbar)
    ON_WM_CLOSE()
    ON_COMMAND(ID_INSPECTION_START, &CMainFrame::OnInspectionStart)
    ON_MESSAGE(WM_LOG_MSG, &CMainFrame::Msg_Log_Display)
    ON_MESSAGE(WM_USER_ACTIVE_RIBBON_TAB_CHANGED, OnChangeRibbonBarCategory)
    ON_MESSAGE(WM_USER_ACTIVE_RIBBON_SIZE, OnChangeRibbonBarSize)
    ON_MESSAGE(WM_GRAB_UPDATE, OnGrabImageUpdate)
    ON_COMMAND(ID_SETTING_CONFIG, &CMainFrame::OnSettingConfig)
    ON_WM_TIMER()
    ON_MESSAGE(WM_USER_PROPERTY_ADD, OnPropertyAdd)
    ON_MESSAGE(WM_USER_PROPERTY_UPDATE, OnPropertyUpdate)
    ON_MESSAGE(WM_USER_PROPERTY_DELETE, OnPropertyDelete)
    ON_MESSAGE(WM_USER_PROPERTY_SELECT, OnPropertySelect)
    ON_MESSAGE(WM_USER_PROPERTY_EDIT, OnPropertyEdit)
    ON_MESSAGE(WM_USER_MOVEVIEWPOSITION, OnMoveViewPosition)
    ON_WM_SIZE()
    //ON_COMMAND(ID_BUTTON_DIO_TEST1, &CMainFrame::OnButtonDioTest1)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
    theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
    theApp.m_bViewMeasureResult = FALSE;

    for (int i = 0; i<MAX_CH_NUM; i++)
        m_pMainView[i] = NULL;

    /*
        Windows 2000               => 5.0
        Windows XP                 => 5.1
        Windows Server 2003        => 5.2
        Windows Server 2003 R2     => 5.2
        Windows Vista              => 6.0
        Windows Server 2008        => 6.0
        Windows Server 2008 R2     => 6.1
        Windows 7                  => 6.1
    */
    os_ver = 1;
    DWORD dwVersion = ::GetVersion();
    DWORD dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    DWORD dwWindowsMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
    if (dwWindowsMajorVersion > 5)  os_ver = 0;

    m_bMultiNetConnected = FALSE;
    m_bGrabON = FALSE;

	// ZXing init
	bool tryHarder = false;
	bool tryRotate = false;
	ZXing::DecodeHints hints;
	hints.setShouldTryHarder(tryHarder);
	hints.setShouldTryRotate(tryRotate);
	_bcreader = std::make_shared<ZXing::MultiFormatReader>(hints);

}

CMainFrame::~CMainFrame()
{
    //CloseHandle(m_hWorkingEvent);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CSDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    g_cRecipeData = new CRecipeData();

    BOOL bNameValid;
    // 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
    OnApplicationLook(theApp.m_nAppLook);
    m_wndRibbonBar.Create(this);
    m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

    if (!m_wndStatusBar.Create(this))
    {
        TRACE0(_S(IDS_FAILED_CREATE_STATUS_BAR));
        return -1;      // 만들지 못했습니다.
    }

    CString strTitlePane1, strTitlePane2, strTitlePane3, strTitlePane4, strTitlePane5, strTitlePane6, strTitlePane7;
    bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
    ASSERT(bNameValid);
    bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
    bNameValid = strTitlePane3.LoadString(IDS_STATUS_PANE3);
    bNameValid = strTitlePane4.LoadString(IDS_STATUS_PANE4);
    bNameValid = strTitlePane5.LoadString(IDS_STATUS_PANE5);
    m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2); //0
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE3, strTitlePane3, TRUE), strTitlePane3);
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE4, strTitlePane4, TRUE), strTitlePane4);
    m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE5, strTitlePane5, TRUE), strTitlePane5); //3
    // Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
    CDockingManager::SetDockingMode(DT_SMART);
    // Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
    EnableAutoHidePanes(CBRS_ALIGN_ANY);

    // 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
    CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

    // 도킹 창을 만듭니다.
    if (!CreateDockingWindows())
    {
        TRACE0(_S(IDS_FAILED_CREATE_DOCKING_WINDOW));
        return -1;
    }

    m_wndLogBar.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndLogBar);
    m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
    DockPane(&m_wndProperties);
    ////////////////////////////////////////////////////////////////////////////////////////////////
    g_pUiObject = new CUI_Object();
    CString sError;
    CString str;

    WebcamDeviceList();

    for (int i = 0; i < vecCamList.size(); i++) {
        if (theApp.cap[i].open(i) != TRUE) {
            str.Format(_T("Webcam %d not open"), i);
            AfxMessageBox(str);
        }
        else {
            theApp.cap[i].set(cv::CAP_PROP_FRAME_WIDTH, DEFAULT_CAM_WIDTH);// 1280);
            theApp.cap[i].set(cv::CAP_PROP_FRAME_HEIGHT, DEFAULT_CMA_HEIGHT);// 720);
        }
    }
    theApp.m_ImgProcEngine.m_pMainWnd = this;

    for (int i = 0; i<MAX_CH_NUM; i++){
        CProcessingClass *pProcessingClass = new CProcessingClass(i);
        pProcessingClass->Class_Init(this, 20, true, i);
        theApp.m_vecProcessingClass.push_back(pProcessingClass);
    }

    g_pDirectory = new CDirect_Info();
    g_cLog = new CLogClass(this);

    g_cRecipeData->m_sPathRecipeRoot.Format(_T("%s\\Recipe"), theApp.g_sRootPath);
    g_cRecipeData->m_sFileName.Format(_T("%s\\%s\\%s.ini"), g_cRecipeData->m_sPathRecipeRoot, gCfg.m_sLastRecipeName, gCfg.m_sLastRecipeName);
    if (g_pDirectory->Search_FileName(g_cRecipeData->m_sFileName) == FALSE){
        sError.Format(_T("Can not find a recipe.\n %s"), g_cRecipeData->m_sFileName);
        g_cLog->AddLog(sError, _LOG_LIST_SYS);
        AfxMessageBox(sError);
    }

    for (int i = 0; i < MAX_CH_NUM; i++){
        CString sFileName;
        sFileName.Format(_T("%s\\Recipe\\%s\\Pat%02d"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, i);
        if (g_pDirectory->Search_FileName(sFileName) == FALSE){
            g_pDirectory->Create_Directory(sFileName);
        }
    }

    g_cLog->AddLog(_S(IDS_NORMAL_BOOT), _LOG_LIST_SYS);

    CRect rect;
    GetClientRect(&rect);

    SetTimer(0, 1000, 0);


    return 0;
}

HRESULT CMainFrame::EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
    // Create the System Device Enumerator.
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the category.
        hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
        if (hr == S_FALSE)
        {
            hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
        }
        pDevEnum->Release();
    }
    return hr;
}

void CMainFrame::DisplayDeviceInformation(IEnumMoniker *pEnum)
{
    CString str;
    IMoniker *pMoniker = NULL;

    CAMINFO camInfo;
    while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
    {
        camInfo.strDevPath.Empty();

        IPropertyBag *pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr))
        {
            pMoniker->Release();
            continue;
        }

        VARIANT var;
        VariantInit(&var);

        // Get description or friendly name.
        hr = pPropBag->Read(L"Description", &var, 0);
        if (FAILED(hr))
        {
            hr = pPropBag->Read(L"FriendlyName", &var, 0);
        }
        if (SUCCEEDED(hr))
        {
            //str.Format(_T("%s\n"), common::varValue2CString(var));
            camInfo.strFriendlyName = common::varValue2CString(var);
            VariantClear(&var);
        }

        hr = pPropBag->Read(L"DevicePath", &var, 0);
        if (SUCCEEDED(hr))
        {
            // The device path is not intended for display.
            //str.Format(_T("Device path: %s\n"), common::varValue2CString(var));
            camInfo.strDevPath = common::varValue2CString(var);
            VariantClear(&var);

            TCHAR seps[] = _T("&");
            TCHAR *token;
            TCHAR szTok[512];
            _tcscpy(szTok, (LPCTSTR)camInfo.strDevPath.GetBuffer(0));
            int seq = 0;
            token = _tcstok(szTok, seps);
            while (token != NULL)
            {
                if (seq == 3) {
                    camInfo.strFriendlyName += _T(" ");
                    camInfo.strFriendlyName += token;
                    break;
                }
                seq++;
                token = _tcstok(NULL, seps);
            }


        }

        if (camInfo.strDevPath.GetLength() > 0) {
            if (camInfo.strDevPath != _T("Basler GenICam Generic Device Path"))
                vecCamList.push_back(camInfo);
        }

        pPropBag->Release();
        pMoniker->Release();
    }
}

void CMainFrame::WebcamDeviceList()
{
    HRESULT hr;// = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    //if (SUCCEEDED(hr))
    {
        IEnumMoniker *pEnum;

        hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
        if (SUCCEEDED(hr))
        {
            DisplayDeviceInformation(pEnum);
            pEnum->Release();
        }

        //CoUninitialize();
    }
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CSDIFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: CREATESTRUCT cs를 수정하여 여기에서
    //  Window 클래스 또는 스타일을 수정합니다.

    cs.style = WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

    return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
    BOOL bNameValid;
    DWORD dwNoCloseBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE & ~AFX_CBRS_CLOSE;


    // 로그 창을 만듭니다.
    CString strOutputWnd;
    bNameValid = strOutputWnd.LoadString(IDS_LOGBAR_WND);
    ASSERT(bNameValid);
    if (!m_wndLogBar.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
    {
        TRACE0(_S(IDS_FAILED_CREATE_LOG_WINDOW));
        return FALSE; // 만들지 못했습니다.
    }

    // 속성 창을 만듭니다.
    CString strPropertiesWnd;
    bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
    ASSERT(bNameValid);
    if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI/*,AFX_CBRS_REGULAR_TABS, dwNoCloseBarStyle*/))
    {
        TRACE0(_T("Fail create window"));
        return FALSE; // 만들지 못했습니다.
    }

    SetDockingWindowIcons(theApp.m_bHiColorIcons);
    return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
    HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    m_wndLogBar.SetIcon(hOutputBarIcon, FALSE);

    HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
    m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame 진단.

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnApplicationLook(UINT id)
{
    CWaitCursor wait;

    theApp.m_nAppLook = id;

    switch (theApp.m_nAppLook)
    {
    case ID_VIEW_APPLOOK_WIN_2000:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_OFF_XP:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_WIN_XP:
        CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_OFF_2003:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_VS_2005:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_VS_2008:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
        break;

    case ID_VIEW_APPLOOK_WINDOWS_7:
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(TRUE);
        break;

    default:
        switch (theApp.m_nAppLook)
        {
        case ID_VIEW_APPLOOK_OFF_2007_BLUE:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_BLACK:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_SILVER:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
            break;

        case ID_VIEW_APPLOOK_OFF_2007_AQUA:
            CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
            break;
        }

        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
        CDockingManager::SetDockingMode(DT_SMART);
        m_wndRibbonBar.SetWindows7Look(FALSE);
    }

    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

    theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
    pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnFilePrint()
{
    if (IsPrintPreview())
    {
        PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
    }
}

void CMainFrame::OnFilePrintPreview()
{
    if (IsPrintPreview())
    {
        PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 인쇄 미리 보기 모드를 닫습니다.
    }
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(IsPrintPreview());
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    CSDIFrameWnd::OnSettingChange(uFlags, lpszSection);
    m_wndLogBar.UpdateFonts();
}



void CMainFrame::OnViewAllbar()
{
    if (m_wndProperties.GetSafeHwnd())
        m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
    if (m_wndLogBar.GetSafeHwnd())
        m_wndLogBar.ShowPane(SW_SHOW, FALSE, TRUE);
}

//
// GrabView에서는 Dockable창이 보일필요가 없으므로 숨겼다가, RoilibAppView에서 다시 보이도록 한다.
//
void CMainFrame::ToggleViewAllbar(BOOL bVisible)
{
    if (bVisible == 0) {
        m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
        m_wndLogBar.ShowPane(FALSE, FALSE, TRUE);
    } else {
        m_wndProperties.ShowPane(FALSE, FALSE, TRUE);
        m_wndLogBar.ShowPane(TRUE, FALSE, TRUE);
    }
}

void CMainFrame::OnClose()
{
    KillTimer(0);
    KillTimer(3);

    for (int i = 0; i<theApp.m_vecProcessingClass.size(); i++) {
        CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[i];
        if (pProcessingClass) {
            pProcessingClass->m_bInspProcessRun = FALSE;
            ::SetEvent(pProcessingClass->m_hProcessEvent);
        }
    }
    Sleep(100);
    for (int i = 0; i < vecCamList.size(); i++) {
        if (theApp.cap[i].isOpened()) {
            theApp.cap[i].release();
        }
    }

    if (g_cRecipeData){
        delete g_cRecipeData;
    }

    for (int i = 0; i<theApp.m_vecProcessingClass.size(); i++) {
        CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[i];
        if (pProcessingClass) {
            pProcessingClass->m_bInspProcessRun = FALSE;
            pProcessingClass->Class_Destory();
            delete pProcessingClass;
        }
    }

    if (g_pDirectory)
        delete g_pDirectory;

    delete g_pUiObject;

    if (g_cLog)
        delete g_cLog;

    CSDIFrameWnd::OnClose();
}

//
// 메세지 처리 루틴.
//

// CLogBarWnd Window(Pain Win)에 Log를 추가한다.
LRESULT CMainFrame::Msg_Log_Display(WPARAM wParam, LPARAM lParam){
    CString pStr = *((CString *)lParam);
    int		pType = (int)wParam;
    int nLen;

    m_wndLogBar.m_wndLog[pType].AddString(pStr);

    return 1;
}

LRESULT CMainFrame::OnChangeRibbonBarSize(WPARAM wParam, LPARAM lParam)
{
    SetTimer(11, 500, NULL);
    return TRUE;
}

LRESULT CMainFrame::OnChangeRibbonBarCategory(WPARAM wParam, LPARAM lParam)
{
    int nCategory = wParam;
    switch (nCategory+os_ver)
    {
        case 1:
            ActivateView(RUNTIME_CLASS(CRoilibAppView));
            //ToggleViewAllbar(TRUE);
            break;
        case 2:
            ActivateView(RUNTIME_CLASS(CRecipeView));
            //ToggleViewAllbar(FALSE);
            break;
    }
    SetTimer(11, 500, NULL);
    return TRUE;
}

void CMainFrame::OnSettingConfig()
{
    CDlgConfig dlg;
    dlg.DoModal();
}

LRESULT CMainFrame::OnGrabImageUpdate(WPARAM wParam, LPARAM lParam) // wParam = nCam
{
    int nCh = wParam;
    if (nCh >= MAX_CH_NUM)
        return FALSE;

    CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[nCh];
    CRoilibAppView *pView = m_pMainView[nCh];
    if (pView && pProcessingClass->cimg != NULL)
    {
        int sx = pProcessingClass->cimg->width;
        int sy = pProcessingClass->cimg->height;
        if (pView->m_ximage != NULL) {
            int tx = pView->m_ximage->GetWidth();
            int ty = pView->m_ximage->GetHeight();

            if (sx != tx || sy != ty) {
                theApp.cs.Lock();
                delete pView->m_ximage;
                pView->m_ximage = new CxImage(sx, sy, 24, CXIMAGE_FORMAT_BMP); // m_ximage(CxImage) Color로 화면에 Display하는 이미지.
                theApp.cs.Unlock();
            }
            theApp.Opencv2ximage(pProcessingClass->cimg, pView->m_ximage);	// 그랩한 이미지를 화면에 실시간 Display한다.
        }
        pView->Invalidate(FALSE);
    }


    return TRUE;

}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 0)		// 1000ms Timer
    {
        CTime NowTime = CTime::GetCurrentTime();
        CString str;

        str.Format(_T("%s\\LOG\\%04d\\%02d\\%02d.txt"), theApp.g_sRootPath, NowTime.GetYear(), NowTime.GetMonth(), NowTime.GetDay());
        if (g_pDirectory->Search_FileName(str) == FALSE){
            g_cLog->CheckLog();
        }
    }

    CSDIFrameWnd::OnTimer(nIDEvent);
}

BOOL CMainFrame::RecipeLoad()
{
    CString str;
    CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetActiveDocument();
    if (pDoc == NULL)
        return FALSE;

    pDoc->RemoveAll();
    g_cRecipeData->ClearImageBuff();
    g_cRecipeData->m_vecRoiData.clear();

    str.Format(_T("%s\\Recipe\\%s"), theApp.g_sRootPath, gCfg.m_sLastRecipeName);
    g_cRecipeData->LoadRecipeData();

    gCfg.WriteConfig();

    for (int i = 0; i < g_cRecipeData->m_vecRoiData.size(); i++)
    {
        CRoiData *pData = g_cRecipeData->m_vecRoiData[i];

        if (pData->m_nObjectType == _ObjType_Patt)
        {
            CDrawRoiPat *m_pRoiObj = new CDrawRoiPat(CRect(pData->m_RoiArea.left, pData->m_RoiArea.top, pData->m_RoiArea.right, pData->m_RoiArea.bottom));
            m_pRoiObj->m_pDocument = pDoc;
            m_pRoiObj->m_ZoomFactor = m_pRecipeView->GetZoomFactor();
            m_pRoiObj->m_nShape = CDrawObj::roipat;
            m_pRoiObj->m_pParent = NULL;
            m_pRoiObj->m_bIsFirst = FALSE;

            CDrawRect* pPatObj = m_pRoiObj->m_pPatObj;
            pPatObj->m_position = CRect(pData->ptnRoi.left, pData->ptnRoi.top, pData->ptnRoi.right, pData->ptnRoi.bottom);
            pPatObj->m_pParent = m_pRoiObj;
            pPatObj->m_ZoomFactor = m_pRoiObj->m_ZoomFactor;
            pDoc->Add(pPatObj);

            CDrawRect* pCrossObj = m_pRoiObj->m_pCrossObj;
            CPoint c = CPoint(pData->ptnRoi.left + pData->m_dCenterPos.x, pData->ptnRoi.top + pData->m_dCenterPos.y);
            pCrossObj->m_position = CRect(c.x - 5, c.y - 5, c.x + 5, c.y + 5);
            pCrossObj->m_bSizeFix = TRUE;
            pCrossObj->m_pParent = m_pRoiObj;
            pCrossObj->m_ZoomFactor = m_pRoiObj->m_ZoomFactor;
            pDoc->Add(pCrossObj);

            m_pRoiObj->m_pRoiData = pData;
            pCrossObj->m_pRoiData = pData;
            pPatObj->m_pRoiData = pData;
            pDoc->Add(m_pRoiObj);
        }
        if (pData->m_nObjectType == _ObjType_Roi)
        {
            CDrawRoi *m_pRoiObj = new CDrawRoi(CRect(pData->m_RoiArea.left, pData->m_RoiArea.top, pData->m_RoiArea.right, pData->m_RoiArea.bottom));
            m_pRoiObj->m_pDocument = pDoc;
            m_pRoiObj->m_ZoomFactor = m_pRecipeView->GetZoomFactor();
            m_pRoiObj->m_nShape = CDrawObj::roi;
            m_pRoiObj->m_pParent = NULL;
            m_pRoiObj->m_bIsFirst = FALSE;

            CDrawRect* pCrossObj = m_pRoiObj->m_pCrossObj;
            CPoint c = CPoint(pData->m_dCenterPos.x, pData->m_dCenterPos.y);

            pCrossObj->m_position = CRect(c.x - 5, c.y - 5, c.x + 5, c.y + 5);
            pCrossObj->m_bSizeFix = TRUE;
            pCrossObj->m_pParent = m_pRoiObj;
            pCrossObj->m_ZoomFactor = m_pRoiObj->m_ZoomFactor;
            pDoc->Add(pCrossObj);

            m_pRoiObj->m_pRoiData = pData;
            pCrossObj->m_pRoiData = pData;
            pDoc->Add(m_pRoiObj);
        }
        if (pData->m_nObjectType == _ObjType_Point)
        {
            CDrawRect* pObj = new CDrawRect(CRect(pData->m_RoiArea.left, pData->m_RoiArea.top, pData->m_RoiArea.right, pData->m_RoiArea.bottom));
            pObj->m_pDocument = pDoc;
            pObj->m_ZoomFactor = m_pRecipeView->GetZoomFactor();
            pObj->m_nShape = CDrawObj::point;
            CRect rect(pData->m_dCenterPos.x - 100, pData->m_dCenterPos.y - 100, pData->m_dCenterPos.x + 100, pData->m_dCenterPos.y + 100);
            pObj->m_position = rect;

            pObj->m_pRoiData = pData;
            pDoc->Add(pObj);
        }
        if (pData->m_nObjectType == _ObjType_Line)
        {
            CDrawRect* pObj = new CDrawRect(CRect(pData->m_RoiArea.left, pData->m_RoiArea.top, pData->m_RoiArea.right, pData->m_RoiArea.bottom));
            pObj->m_pDocument = pDoc;
            pObj->m_ZoomFactor = m_pRecipeView->GetZoomFactor();
            pObj->m_nShape = CDrawObj::line;

            pObj->m_pRoiData = pData;
            pDoc->Add(pObj);
        }
    }

    m_wndProperties.InitPropList();

    pDoc->UpdateAllViews(NULL);
    return TRUE;
}

//
// 현재 pObj->m_position는 정상적은 좌표를 가지고 있으므로 pObj->m_pRoiData에도 정상적인 좌표정보를 Update한다.
//
void CMainFrame::UpdateRoiDataInfo(CDrawObj *pObj, CRoiData* pRoiData)
{
    CRect rect;
    CString str;
    pRoiData->SetRect(pObj->m_position);

    switch (pObj->m_nShape) {
    case CDrawObj::line:
        pRoiData->m_nObjectType = _ObjType_Line;
        break;
    case CDrawObj::point:
    case CDrawObj::cross:
        pRoiData->m_nObjectType = _ObjType_Point;
        break;
    case CDrawObj::rectangle:
        pRoiData->m_nObjectType = _ObjType_Roi;
        break;
    case CDrawObj::roi:
    {
        CDrawRoi *pdrp = (CDrawRoi *)pObj;
        pRoiData->m_nObjectType = _ObjType_Roi;

        pdrp->m_position.NormalizeRect();
        pRoiData->m_RoiArea.SetRect(pdrp->m_position);
        rect = pdrp->m_pCrossObj->m_position;
        pRoiData->m_dCenterPos.x = rect.CenterPoint().x;
        pRoiData->m_dCenterPos.y = rect.CenterPoint().y;
    }
    break;
    case CDrawObj::roipat:
    {
        CDrawRoiPat *pdrp = (CDrawRoiPat *)pObj;
        pRoiData->m_nObjectType = _ObjType_Patt;
        pdrp->m_position.NormalizeRect();
        pRoiData->m_RoiArea.SetRect(pdrp->m_position);
        pdrp->m_pPatObj->m_position.NormalizeRect();
        pRoiData->ptnRoi.SetRect(pdrp->m_pPatObj->m_position);
        rect = pdrp->m_pCrossObj->m_position;
        rect.NormalizeRect();
        pRoiData->m_dCenterPos.x = rect.CenterPoint().x - pRoiData->ptnRoi.left;
        pRoiData->m_dCenterPos.y = rect.CenterPoint().y - pRoiData->ptnRoi.top;
    }
    break;
    }
}

LRESULT CMainFrame::OnPropertyAdd(WPARAM wParam, LPARAM lParam)
{
    int size;
    CRoiData roiData;
    CDrawObj *pObj = (CDrawObj*)lParam;
    if (pObj->m_pParent)
        pObj = pObj->m_pParent;

    if (wParam&BUTTON_PRESS) { //  마우스버턴을 눌렀을때.
        m_wndProperties.AddPropertyItem(pObj);
    }

    if (wParam&BUTTON_RELEASE) // 마우스버턴을 띄었을때.
    {
        m_pRecipeView->ClearTracker();

        CRect rect(pObj->m_position);
        switch (pObj->m_nShape) {
        case CDrawObj::line:
        case CDrawObj::point:
        case CDrawObj::cross:
        case CDrawObj::rectangle:
        case CDrawObj::roi:
        case CDrawObj::roipat:
            UpdateRoiDataInfo(pObj, pObj->m_pRoiData);
            g_cRecipeData->m_vecRoiData.push_back(pObj->m_pRoiData);
            g_cRecipeData->SaveRecipeData();
            break;
        }
    }

    return TRUE;
}

LRESULT CMainFrame::OnPropertyUpdate(WPARAM wParam, LPARAM lParam)
{
    CDrawObj *pObj = (CDrawObj*)lParam;
    if (pObj->m_pParent)
        pObj = pObj->m_pParent;
    CRoiData *pRoiData = pObj->m_pRoiData;

    switch (pObj->m_nShape) {
    case CDrawObj::point:
    case CDrawObj::cross:
    case CDrawObj::rectangle:
    case CDrawObj::roi:
    case CDrawObj::roipat:
        UpdateRoiDataInfo(pObj, pRoiData);
        break;
    }

    m_wndProperties.UpdatePropertyItem(pObj);

    if (pObj->m_pParent == NULL)
    {
        int nSeq = g_cRecipeData->getSeq(pRoiData);
        if (nSeq >= 0)
            g_cRecipeData->UpdateOneRecipeData(pRoiData, nSeq);
    }
    return TRUE;
}

LRESULT CMainFrame::OnPropertyDelete(WPARAM wParam, LPARAM lParam)
{
    CDrawObj *pObj = (CDrawObj*)lParam;
    if (pObj->m_pParent)
        pObj = pObj->m_pParent;
    CRoiData *pRoiData = pObj->m_pRoiData;

    switch (pObj->m_nShape) {
    case CDrawObj::point:
    case CDrawObj::cross:
    case CDrawObj::rectangle:
    case CDrawObj::roi:
    case CDrawObj::roipat:
        UpdateRoiDataInfo(pObj, pRoiData);
        break;
    }

    m_wndProperties.DeletePropertyItem(pObj);

    return TRUE;
}

LRESULT CMainFrame::OnPropertySelect(WPARAM wParam, LPARAM lParam)
{
    CDrawObj *pObj = (CDrawObj*)lParam;
    if (pObj->m_pParent)
        pObj = pObj->m_pParent;

    m_wndProperties.SetSelectProperty(pObj);
    return TRUE;
}

LRESULT CMainFrame::OnPropertyEdit(WPARAM wParam, LPARAM lParam)
{
    CDrawObj *pObj = (CDrawObj*)lParam;
    if (pObj->m_pParent)
        pObj = pObj->m_pParent;
    CRoiData *pRoiData = pObj->m_pRoiData;

    if (_tcslen(pRoiData->m_sName) == 0) {
        AfxMessageBox(_T("There is no ROI name"));
        return FALSE;
    }

    CDlgObject dlg;
    UpdateRoiDataInfo(pObj, pRoiData);
    dlg.m_roiData = *pRoiData;

    if (dlg.DoModal() == IDOK){
        if (dlg.m_bDeleteFlag){
            m_pRecipeView->RegisterUndoList(pObj);
            DeleteReceipe(pRoiData);
            return TRUE;
        }
        //////////////////////////////////////////////////////////////////

        dlg.ObjectListGet();

        UpdateReceipe(&dlg.m_roiData);

        m_wndProperties.UpdatePropertyItem(pObj);

    }

    return TRUE;
}

void CMainFrame::DeleteReceipe(CRoiData* pDelData)
{
    int nCh = theApp.m_nSelectCh;
    int size = g_cRecipeData->m_vecWorkRoiData[nCh].size();
	std::vector<CRoiData*> *pVecRoiData = &g_cRecipeData->m_vecWorkRoiData[nCh];
    for (int i = 0; i<size; i++){
        CRoiData *pWorkData = (*pVecRoiData)[i];
        if (pWorkData->sUniqueID == pDelData->sUniqueID) {
            pVecRoiData->erase(pVecRoiData->begin() + i);
            break;
        }
    }

    for (int i = 0; i < g_cRecipeData->m_vecRoiData.size(); i++)
    {
        CRoiData *pData = g_cRecipeData->m_vecRoiData[i];
        if (pData->sUniqueID == pDelData->sUniqueID) {
            g_cRecipeData->m_vecRoiData.erase(g_cRecipeData->m_vecRoiData.begin() + i);
            break;
        }
    }

    g_cRecipeData->SaveRecipeData();
}

void CMainFrame::UpdateReceipe(CRoiData* pDlgData)
{
    CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[theApp.m_nSelectCh];
    CString str;
    int size = g_cRecipeData->m_vecRoiData.size();
    for (int i = 0; i < size; i++)
    {
        CRoiData *pData = g_cRecipeData->m_vecRoiData[i];
        if (pDlgData->sUniqueID == pData->sUniqueID)
        {
            *pData = *pDlgData;
            g_cRecipeData->UpdateOneRecipeData(pData, i);
        }
    }

}

LRESULT CMainFrame::OnMoveViewPosition(WPARAM wParam, LPARAM lParam)
{
    WORD nCh = wParam;
    CvPoint pos = *(CvPoint*)lParam;
    nCh = nCh & 0x00FF;
    CPoint pt = CPoint(pos.x, pos.y);

    CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetActiveDocument();
    if (pDoc != NULL) {
        if (pDoc->m_pActiveMainView->IsKindOf(RUNTIME_CLASS(CRecipeView))) {
            m_pRecipeView->MoveCenterPosition(pt);
            return TRUE;
        }
    }

    int nCam = nCh % 4;
    m_pMainView[nCam]->MoveCenterPosition(pt);

    return TRUE;
}

// 전체 검사 시작 - 검사모드는 현재 Display되어 있는 이미지의 검사를 진행한다.
void CMainFrame::OnInspectionStart()
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetActiveDocument();
    BOOL oldPreviewMode = theApp.m_bPreviewMode;
    theApp.m_bPreviewMode = FALSE;

    for (int nCh = 0; nCh < 2; nCh++)
    {
        if (theApp.m_vecProcessingClass.size() <= nCh)
            break;
        CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[nCh];
        if (pProcessingClass->cimg == NULL) // 이 채널에 카메라가 없다.
            break;

        int sx = pProcessingClass->cimg->width;
        int sy = pProcessingClass->cimg->height;
        if (pProcessingClass->iplImage == NULL) {
            pProcessingClass->iplImage = cvCreateImage(cvSize(sx, sy), IPL_DEPTH_8U, 1);
        }
        else {
            if (sx != pProcessingClass->iplImage->width || sy != pProcessingClass->iplImage->height) {
                cvReleaseImage(&pProcessingClass->iplImage);
                pProcessingClass->iplImage = cvCreateImage(cvSize(sx, sy), IPL_DEPTH_8U, 1);
            }
        }
        cvCvtColor(pProcessingClass->cimg, pProcessingClass->iplImage, CV_RGB2GRAY);

        theApp.m_ImgProcEngine.InitEngine(nCh);

        CRoilibAppView *pView = m_pMainView[nCh];// nCam % gCfg.m_nCamNumber];
        pView->OtherObjectsClear();

        int size = g_cRecipeData->m_vecWorkRoiData[nCh].size();
        for (int i = 0; i < size; i++){
            CRoiData* pData = g_cRecipeData->m_vecWorkRoiData[nCh][i];

            pData->m_vecDetectResult.clear();
            theApp.m_ImgProcEngine.InspectOneItem(nCh, pProcessingClass->cimg, pData);


            int size = pData->m_vecDetectResult.size();
            for (int i = 0; i < size; i++) {
                DetectResult *prst = &pData->m_vecDetectResult[i];

				if (pData->m_nInspectType == _Inspect_BarCode || pData->m_nInspectType == _Inspect_Teseract)
				{
					CDrawObj* pObj = new CDrawRect(CRect(0, 0, 600, 30));
					_tcscpy(pObj->m_text, prst->strResult);
					pObj->m_dwType = DRAWOBJ_TYPE_RESULT;
					pObj->m_pDocument = pDoc;
					pObj->m_nShape = CDrawObj::text;
					pObj->m_ZoomFactor = pView->GetZoomFactor();
					_stprintf(pObj->m_pRoiData->m_sName, _T(""));
					pView->m_otherObjects.AddTail(pObj);
				}
				else {
					CDrawObj* pObj = new CDrawRect(CRect(prst->pt.x, prst->pt.y, prst->pt1.x, prst->pt1.y));
					pObj->m_dwType = DRAWOBJ_TYPE_RESULT;
					pObj->SetLineColor(RGB(255, 0, 0));
					pObj->m_pDocument = pDoc;
					pObj->m_nShape = CDrawObj::ellipse;
					pObj->m_ZoomFactor = pView->GetZoomFactor();
					pObj->SetLineWidth(2);
					_stprintf(pObj->m_pRoiData->m_sName, _T("%.2f %.2f"), prst->pt.x, prst->pt.y);
					pView->m_otherObjects.AddTail(pObj);
				}
            }
        }
        pView->Invalidate();
    }
    theApp.m_bPreviewMode = oldPreviewMode;

    theApp.m_bSmoothMode = TRUE;

}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
    CSDIFrameWnd::OnSize(nType, cx, cy);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    return CSDIFrameWnd::PreTranslateMessage(pMsg);
}
