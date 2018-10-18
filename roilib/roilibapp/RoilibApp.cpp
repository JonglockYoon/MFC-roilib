//
// Copyright (C) 2018
// All rights reserved by jerry1455@gmail.com
//

// RoilibApp.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "RoilibApp.h"
#include "MainFrm.h"

#include "RoilibAppDoc.h"
#include "RoilibAppView.h"
#include "RecipeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment (lib, "Psapi.lib")
#pragma comment (lib, "Dbghelp.lib")

#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"Setupapi.lib")
#pragma comment (lib, "./pthread/pthreadVC.lib")
#pragma comment (lib, "../../lib/ZXingCore.lib")

#ifdef _DEBUG
#pragma comment (lib, "c:/opencv/build/x64/vc14/lib/opencv_world340d.lib")
#pragma comment (lib, "../../lib/libdcr.lib")
#pragma comment (lib, "../../lib/png.lib")
#pragma comment (lib, "../../lib/jpeg.lib")
#pragma comment (lib, "../../lib/zlib.lib")
#pragma comment (lib, "../../lib/tiff.lib")
#pragma comment (lib, "../../lib/jbig.lib")
#pragma comment (lib, "../../lib/jasper.lib")
#pragma comment (lib, "../../lib/mng.lib")
#pragma comment (lib, "../../lib/cximage.lib")
#else
#pragma comment (lib, "c:/opencv/build/x64/vc14/lib/opencv_world340.lib")
#pragma comment (lib, "../../lib/libdcr.lib")
#pragma comment (lib, "../../lib/png.lib")
#pragma comment (lib, "../../lib/jpeg.lib")
#pragma comment (lib, "../../lib/zlib.lib")
#pragma comment (lib, "../../lib/tiff.lib")
#pragma comment (lib, "../../lib/jbig.lib")
#pragma comment (lib, "../../lib/jasper.lib")
#pragma comment (lib, "../../lib/mng.lib")
#pragma comment (lib, "../../lib/cximage.lib")
#endif


// CRoilibAppApp

BEGIN_MESSAGE_MAP(CRoilibAppApp, CSDIWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CRoilibAppApp::OnAppAbout)
    // 표준 파일을 기초로 하는 문서 명령입니다.
    ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
    // 표준 인쇄 설정 명령입니다.
    ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CRoilibAppApp 생성

CRoilibAppApp::CRoilibAppApp()
{
    m_bHiColorIcons = TRUE;

    // 다시 시작 관리자 지원
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
    // 응용 프로그램을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
    //     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
    //     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
    System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

    // TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
    // 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
    SetAppID(_T("RoilibApp.1.0"));

    // TODO: 여기에 생성 코드를 추가합니다.
    // InitInstance에 모든 중요한 초기화 작업을 배치합니다.

    m_bStopProcess = FALSE;
    m_nSelectCh = 0;
    //m_bAutoSelectCh = TRUE;
    m_bPreviewMode = TRUE;
    m_bSmoothMode = FALSE;

}

// 유일한 CRoilibAppApp 개체입니다.

CRoilibAppApp theApp;


// CRoilibAppApp 초기화.

BOOL CRoilibAppApp::InitInstance()
{
    // 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
    // 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
    // InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
    // 이 항목을 설정하십시오.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CSDIWinApp::InitInstance();


    // OLE 라이브러리를 초기화합니다.
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    if (!AfxSocketInit())
    {
        AfxMessageBox(_T("Socket init failed"));
        return FALSE;
    }

    AfxEnableControlContainer();

    EnableTaskbarInteraction(FALSE);


    // RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.
    // AfxInitRichEdit2();

    // 표준 초기화
    // 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
    // 아래에서 필요 없는 특정 초기화
    // 루틴을 제거해야 합니다.
    // 해당 설정이 저장된 레지스트리 키를 변경하십시오.
    // TODO: 이 문자열을 회사 또는 조직의 이름과 같은
    // 적절한 내용으로 수정해야 합니다.
    SetRegistryKey(_T("MFC-Roilib"));
    TCHAR szCurPath[512];

    GetCurrentDirectory(sizeof(szCurPath), szCurPath); // 현재 Directory Path를 가져온다. Config위치를 고정할려면 여기에 절대Path를 사용하면 된다.
    gCfg.m_sFileName.Format(_T("%s\\Config\\CONFIG.ini"), szCurPath);
    theApp.g_sRootPath = szCurPath;

    gCfg.ReadConfig();
    if (!gCfg.m_Root.IsEmpty())
        theApp.g_sRootPath = gCfg.m_Root;
    if (!DirectoryExists(theApp.g_sRootPath))
        theApp.g_sRootPath = szCurPath;

    CString str;
    str.Format(_T("%s\\EngineImage\\"), theApp.g_sRootPath);
    str.TrimRight(_T("\\"));
    checkPath(str, true);
    m_ImgProcEngine.m_sDebugPath = str;

    LoadStdProfileSettings(0);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.


    InitContextMenuManager();

    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
    //  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
    CSDIDocTemplate* pDocTemplate; //CSingleDocTemplate

    int col = 0;
    int row = 0;
    for (col = 0; col<2; col++)
    {
        pDocTemplate = new CSDIDocTemplate(
            IDR_GRABVIEW,
            RUNTIME_CLASS(CRoilibAppDoc),
            RUNTIME_CLASS(CMainFrame),		// main SDI frame window
            RUNTIME_CLASS(CRoilibAppView), 0, row, col);	// splitter, row, col
            //RUNTIME_CLASS(CRoilibAppView));	// splitter, row, col

        if (!pDocTemplate)
            return FALSE;
        AddDocTemplate(pDocTemplate);
    }

    pDocTemplate = new CSDIDocTemplate( //CSingleDocTemplate
        IDR_MAINFRAME,
        RUNTIME_CLASS(CRoilibAppDoc),
        RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
        RUNTIME_CLASS(CRecipeView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);


    // 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);



    // 명령줄에 지정된 명령을 디스패치합니다.
    // 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    // 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    // 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
    //  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.

    return TRUE;
}

int CRoilibAppApp::ExitInstance()
{
    //TODO: 추가한 추가 리소스를 처리합니다.
    AfxOleTerm(FALSE);

    return CSDIWinApp::ExitInstance();
}

// CRoilibAppApp 메시지 처리기.


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

// 대화 상자 데이터입니다.
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CRoilibAppApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CRoilibAppApp 사용자 지정 로드/저장 메서드

void CRoilibAppApp::PreLoadState()
{
    BOOL bNameValid;
    CString strName;
    bNameValid = strName.LoadString(IDS_EDIT_MENU);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
    bNameValid = strName.LoadString(IDS_EXPLORER);
    ASSERT(bNameValid);
    GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CRoilibAppApp::LoadCustomState()
{
}

void CRoilibAppApp::SaveCustomState()
{
}

// CRoilibAppApp 메시지 처리기.

CString _fnLoadString(UINT id, ...) {
    CString sID;

    sID.LoadString(id);

    va_list vl;

    va_start( vl, id );

    CString sMsg;

    sMsg.FormatV( sID, vl );

    va_end( vl );

    return sMsg;
}


// 디버깅을 위한 함수.
int CRoilibAppApp::AddLog(CString str)
{
    int ret = 1;

    CString nStr, nDateStr, nTimeStr, s;
    int *pLogType;

        std::FILE *pFile = NULL;
        SYSTEMTIME	lsystime;


        GetLocalTime(&lsystime);

        nDateStr.Format(_T("%04d/%02d/%02d"), lsystime.wYear, lsystime.wMonth, lsystime.wDay);
        nTimeStr.Format(_T("%02d:%02d:%02d.%03d"), lsystime.wHour, lsystime.wMinute, lsystime.wSecond, lsystime.wMilliseconds);
        nStr.Format(_T("[%s %s] %s\n"), nDateStr, nTimeStr, str	);

        if((pFile = _tfopen((LPCTSTR)_T(".\\DebuggingLog.txt"), _T("ab+"))) == NULL){
            ret = 2;
        }
        else{
#ifdef _UNICODE
            fseek( pFile, 0, SEEK_END);
            long lSize = ftell(pFile);
            if (lSize == 0) {
                WORD mark = 0xFEFF;
                fwrite(&mark, sizeof(TCHAR), 1, pFile);
                fseek( pFile, 0, SEEK_END);
            }
#endif
            fwrite(nStr, sizeof(TCHAR), nStr.GetLength(), pFile);
            fclose(pFile);

            ret = 0;
        }

    return ret;
}


BOOL CRoilibAppApp::PreTranslateMessage(MSG* pMsg)
{

    return CSDIWinApp::PreTranslateMessage(pMsg);
}


int CRoilibAppApp::Opencv2ximage(IplImage *iplImage, CxImage *ximg)
{
    if (iplImage == NULL)
        return -1;
    if (ximg == NULL)
        return -1;
    int cx = ximg->GetWidth();
    int cy = iplImage->height;

    HANDLE hImage = ximg->GetDIB();
    BITMAPINFOHEADER ds;
    memcpy(&ds, hImage, sizeof(ds));

    int nCh = iplImage->nChannels;

    if (ds.biBitCount == 8 || ds.biBitCount == 24)
    {
        int effwdt = ((((ds.biBitCount * ds.biWidth) + 31) / 32) * 4);
        int	nPad = effwdt - (((ds.biWidth *	ds.biBitCount) + 7) / 8);
        BYTE* pbBits = (BYTE*)hImage + *(DWORD*)hImage + ds.biClrUsed * sizeof(RGBQUAD);

        int nBytePerPixel = ds.biBitCount / 8;
        int wsize = cx*iplImage->nChannels;
        char *pImgData = iplImage->imageData;

        pbBits += (cy * cx * nBytePerPixel);
        pbBits += cy*nPad;

        for (int i = 0; i < cy; i++) {
            pbBits -= (cx * nBytePerPixel);
            pbBits -= nPad;

            if (nCh == 1) { // OPENCV gray to ximage color
                for (int i = 0; i < wsize; i++) {
                    pbBits[i * 3] = pImgData[i];
                    pbBits[i * 3 + 1] = pImgData[i];
                    pbBits[i * 3 + 2] = pImgData[i];
                }
            }
            else memcpy(pbBits, pImgData, wsize);

            pImgData += wsize;
            pImgData += nPad;
        }
        return 0;
    }
    return -1;
}
