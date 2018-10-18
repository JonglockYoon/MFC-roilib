// RecipeView.cpp : CRecipeView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "RoilibApp.h"
#endif

#include "MainFrm.h"
#include "RoilibAppDoc.h"
#include "RecipeView.h"
#include "UI/memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRecipeView

IMPLEMENT_DYNCREATE(CRecipeView, CDrawView)

BEGIN_MESSAGE_MAP(CRecipeView, CDrawView)
    // 표준 인쇄 명령입니다.
    ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRecipeView::OnFilePrintPreview)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_ERASEBKGND()
    ON_WM_SETFOCUS()
    ON_WM_CONTEXTMENU()
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_WM_CHAR()
    ON_WM_SETCURSOR()
    ON_MESSAGE(WM_USER_NEWIMAGE, OnNewImage)
    ON_MESSAGE(WM_USER_PROGRESS, OnNewProgress)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
    ON_WM_KEYDOWN()
    ON_MESSAGE(WM_USER_LOAD_ROIDATA, OnReloadRoiData)
    ON_COMMAND(ID_INSPECTION_CHANNEL, &CRecipeView::OnInspectionChannel)
    ON_UPDATE_COMMAND_UI(ID_INSPECTION_CHANNEL, &CRecipeView::OnUpdateInspectionChannel)

    ON_COMMAND(ID_VIEW_TOOLS_MOVE, OnViewToolsMove)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_MOVE, OnUpdateViewToolsMove)
    ON_COMMAND(ID_VIEW_TOOLS_SELECT, OnViewToolsSelect)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_SELECT, OnUpdateViewToolsSelect)
    ON_COMMAND(ID_VIEW_TOOLS_TRACKER, OnViewToolsTracker)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_TRACKER, OnUpdateViewToolsTracker)
    ON_COMMAND(ID_VIEW_TOOLS_MEASURE, &OnViewToolsMeasure)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_MEASURE, &OnUpdateViewToolsMeasure)
    ON_COMMAND(ID_VIEW_TOOLS_DRAWRECT, &OnViewToolsDrawRect)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_DRAWRECT, &OnUpdateViewToolsDrawRect)
    ON_COMMAND(ID_VIEW_TOOL_DRAWROIPAT, &OnViewToolsDrawRoiPat)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOL_DRAWROIPAT, &OnUpdateViewToolsDrawRoiPat)

    ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomin)
    ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
    ON_COMMAND(ID_VIEW_NORMALVIEWING11, OnViewNormalviewing11)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NORMALVIEWING11, OnUpdateViewNormalviewing11)
    ON_COMMAND(ID_RECIPE_GRAB_START, &OnRecipeGrabStart)

    ON_COMMAND(ID_RECIPE_IMAGESAVE, &OnRecipeImagesave)
    ON_COMMAND(ID_RECIPE_IMAGELOAD, &CRecipeView::OnRecipeImageload)
    ON_COMMAND(ID_RECIPE_INSPECTION1, &CRecipeView::OnRecipeInspection)
    ON_COMMAND(ID_BUTTON_PREVIEW, &CRecipeView::OnButtonPreview)
    ON_COMMAND(ID_BUTTON_STOP, &CRecipeView::OnButtonStop)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_PREVIEW, &CRecipeView::OnUpdateButtonPreview)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_STOP, &CRecipeView::OnUpdateButtonStop)
    ON_COMMAND(ID_VIEW_TOOLS_DRAWLINE, &CRecipeView::OnViewToolsDrawline)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_DRAWLINE, &CRecipeView::OnUpdateViewToolsDrawline)
END_MESSAGE_MAP()

// CRecipeView 생성/소멸

CRecipeView::CRecipeView()
{
    SetScrollSizes(MM_TEXT, CSize(0, 0));

    VERIFY(m_brHatch.CreateHatchBrush(HS_DIAGCROSS, RGB(191, 191, 191)));

    //m_bDrawGride = FALSE;
    //theApp.m_nSelectCh = 0;
    m_OldZF = m_ZoomFactor = 1;

    for (int i = 0; i < MAX_CH_NUM; i++)
        m_ximage[i] = NULL;
    cimg = NULL;
}

CRecipeView::~CRecipeView()
{
    if (cimg)
        cvReleaseImage(&cimg);
    cimg = NULL;
    OtherObjectsClear();
}

void CRecipeView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
    CScrollView::OnPrepareDC(pDC, pInfo);
}

BOOL CRecipeView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: CREATESTRUCT cs를 수정하여 여기에서
    //  Window 클래스 또는 스타일을 수정합니다.

    return CScrollView::PreCreateWindow(cs);
}


// CRecipeView 그리기.

void CRecipeView::OnDraw(CDC* pDC)
{
    CRoilibAppDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);

    CxMemDC* pMemDC = NULL;
    pDC = pMemDC = new CxMemDC(pDC);

    if (m_brHatch.m_hObject){
        CRect rect;
        GetClientRect(&rect);
        rect.right  = max(rect.right , m_totalDev.cx);
        rect.bottom = max(rect.bottom, m_totalDev.cy);
        m_brHatch.UnrealizeObject();
        CPoint pt(0, 0);
        pDC->LPtoDP(&pt);
        pt = pDC->SetBrushOrg(pt.x % 8, pt.y % 8);
        CBrush* old = pDC->SelectObject(&m_brHatch);
        pDC->FillRect(&rect, &m_brHatch);
        pDC->SelectObject(old);
    }

    CxImage* ima = m_ximage[theApp.m_nSelectCh];// pDoc->GetImage();
    if (ima) {

            double zoom=GetZoomFactor();
            if (zoom==1)
                ima->Draw(pDC->GetSafeHdc());
            else
                ima->Draw(pDC->GetSafeHdc(),
                    CRect(0,0,(int)(ima->GetWidth()*zoom),(int)(ima->GetHeight()*zoom)),
                    0,0);


        pDoc->SetZoomFactor(zoom);
        pDoc->Draw(pDC, this);

        DrawGride(pDC);

        SetZoomFactor(zoom);
        Draw(pDC, this);
    }
    CDrawView::OnDraw(pDC);


    delete pMemDC;
}

void CRecipeView::OnInitialUpdate()
{
    CRoilibAppDoc* pDoc = GetDocument();
    CString str;
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    CMFCRibbonStatusBar& statusBar = pMainFrame->GetStatusBar();
    //int n = statusBar.GetCount();

    CString sFileName;
    for (int i = 0; i < MAX_CH_NUM; i++)
    {
        sFileName.Format(_T("%s\\Recipe\\%s\\Pat%02d\\BaseImg.bmp"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, i);
        if (FileExists(sFileName) == true)
        {
            m_ximage[i] = new CxImage(sFileName, CXIMAGE_FORMAT_BMP);		// 화면에 나타내는 이미지는 RGB Color를 이용해서 추가적인 정보를 Color로 나타낼수 있도록 하자.
            CSize sizeTotal;
            sizeTotal.cx = m_ximage[i]->GetWidth();
            sizeTotal.cy = m_ximage[i]->GetHeight();
            SetScrollSizes(MM_TEXT, sizeTotal);

            CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[i];
            if (FileExists(sFileName) == true) {
                CT2A ascii(sFileName);
                if (pProcessingClass->iplImage != NULL)
                    cvReleaseImage(&pProcessingClass->iplImage);
                pProcessingClass->iplImage = cvLoadImage(ascii, 0);

                if (cimg != NULL)
                    cvReleaseImage(&cimg);
                cimg = cvLoadImage(ascii, 1);
            }
        }
        else {
            m_ximage[i] = new CxImage(DEFAULT_CAM_WIDTH, DEFAULT_CMA_HEIGHT, 24, CXIMAGE_FORMAT_BMP);		// 화면에 나타내는 이미지는 RGB Color를 이용해서 추가적인 정보를 Color로 나타낼수 있도록 하자.
        }
    }

    pMainFrame->m_pRecipeView = this;
    pMainFrame->RecipeLoad();

    //CRoilibAppDoc* pDoc = GetDocument();
    //pDoc->m_pRecipeView = this;

    LPCTSTR p = statusBar.GetElement(0)->GetText();
    if (p != NULL) {
        if (p[0] == '_')
        {
            CString s = _T(" ");
            statusBar.GetElement(0)->SetText(s);
            statusBar.GetExElement(0)->SetText(s);
            statusBar.GetExElement(1)->SetText(s);
            statusBar.GetExElement(2)->SetText(s);
            statusBar.Invalidate();
        }
    }

    CString strChs[8] = {
        _T("Fist"), _T("Second"), _T("Third"), _T("Fourth"), _T("Fifth"), _T("Sixth"), _T("Seventh"), _T("Eighth"),
    };

    CMFCRibbonComboBox* pCbChannel = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pMainFrame->m_wndRibbonBar.FindByID(ID_INSPECTION_CHANNEL));
    for (int i = 0; i<MAX_CH_NUM; i++) {
        str.Format(_T("%d-%s"), i, strChs[i]);
        pCbChannel->AddItem(str);
    }
    pCbChannel->SelectItem(theApp.m_nSelectCh);

    pDoc->m_pActiveMainView->m_ximage = m_ximage[theApp.m_nSelectCh];

    theApp.m_ImgProcEngine.InitEngine(theApp.m_nSelectCh);

    SetTimer(0, 50, 0); // Camera preview 용.
}

void CRecipeView::DrawGride(CDC* pDC)
{
    CRoilibAppDoc* pDoc = GetDocument();
    CxImage*  ima = m_ximage[theApp.m_nSelectCh]; //pDoc->GetImage();
    if (!ima)	return;

    //if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) // LBUTTON down일때는 그리지 않는다.
    //	return;

    CPen pen(PS_SOLID, 1, RGB(176, 196, 222));
    CPen* pOldPen = pDC->SelectObject (&pen);

    CRect rect;
    GetClientRect(&rect);
    //CPoint pt = GetScrollPosition();
    //rect += pt;
    //TRACE(_T("ScrollPos = %d,%d\n"), pt.x, pt.y);

    int nOffsetx = ima->GetWidth() / 2;
    int nOffsety = ima->GetHeight() / 2;
    float z = GetZoomFactor();
    //if (z > 1.0)
    {
        nOffsetx *= z;
        nOffsety *= z;

        rect.bottom = (float)ima->GetHeight() * z;
        rect.right = (float)ima->GetWidth() * z;
    }
    for (int x = rect.left; x<rect.right; x += nOffsetx) {
        pDC->MoveTo(x, rect.top);
        pDC->LineTo(x, rect.bottom);
    }
    for (int y = rect.top; y<rect.bottom; y += nOffsety) {
        pDC->MoveTo(rect.left, y);
        pDC->LineTo(rect.right, y);
    }

    pen.DeleteObject();
    pDC->SelectObject (pOldPen);
}

// CRecipeView 진단.

#ifdef _DEBUG
void CRecipeView::AssertValid() const
{
    CScrollView::AssertValid();
}

void CRecipeView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}

CRoilibAppDoc* CRecipeView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRoilibAppDoc)));
    return (CRoilibAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CRecipeView 메시지 처리기.

void CRecipeView::SetZoomCal(double zf) {
    SetZoomFactor(zf);
    OnUpdate(NULL, WM_USER_NEWIMAGE, NULL);
}

void CRecipeView::OnViewZoomin()
{
    if (m_ZoomFactor >= 32) return;

    m_OldZF = m_ZoomFactor;

    if (m_ZoomFactor == 0.50f) m_ZoomFactor = 0.75f;
    else if (m_ZoomFactor == 0.75f) m_ZoomFactor = 1.00f;
    else if (m_ZoomFactor == 1.00f)	m_ZoomFactor = 1.50f;
    else if (m_ZoomFactor == 1.50f)	m_ZoomFactor = 2.00f;
    else if (m_ZoomFactor > 1.0f)	m_ZoomFactor += 1;
    else m_ZoomFactor *= 2;

    OnUpdate(NULL, WM_USER_NEWIMAGE, NULL);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewZoomout()
{
    if (m_ZoomFactor<0.01) return;

    m_OldZF = m_ZoomFactor;

    if (m_ZoomFactor == 2.00f) m_ZoomFactor = 1.50f;
    else if (m_ZoomFactor == 1.50f) m_ZoomFactor = 1.00f;
    else if (m_ZoomFactor == 1.00f)	m_ZoomFactor = 0.75f;
    else if (m_ZoomFactor == 0.75f)	m_ZoomFactor = 0.50f;
    else if (m_ZoomFactor > 1.0) m_ZoomFactor -= 1;
    else m_ZoomFactor /= 2.0;

    OnUpdate(NULL, WM_USER_NEWIMAGE, NULL);
}

//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnUpdateViewZoomin(CCmdUI* pCmdUI)
{
    if (m_ZoomFactor >= 32) pCmdUI->Enable(0);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnUpdateViewZoomout(CCmdUI* pCmdUI)
{
    if (m_ZoomFactor<0.01) pCmdUI->Enable(0);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewNormalviewing11()
{
    m_OldZF = m_ZoomFactor;
    m_ZoomFactor = 1;
    OnUpdate(NULL, WM_USER_NEWIMAGE, NULL);

}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnUpdateViewNormalviewing11(CCmdUI* pCmdUI)
{
}

//////////////////////////////////////////////////////////////////////////////
void CRecipeView::GetImageXY(CRoilibAppDoc *pDoc, CxImage *ima, long *x, long *y)
{
    if (!ima || !pDoc) return;

    CPoint point=GetScrollPosition();
    double fx =(double)(*x + point.x);
    double fy =(double)(*y + point.y);

    fx/=GetZoomFactor();
    fy/=GetZoomFactor();

    *x = (long)fx;
    *y = (long)fy;
}
void CRecipeView::GetImageXY(CRoilibAppDoc *pDoc, CxImage *ima, long *x, long *y, double zf)
{
    if (!ima || !pDoc) return;

    CPoint point=GetScrollPosition();
    double fx =(double)(*x + point.x);
    double fy =(double)(*y + point.y);

    fx/=zf;
    fy/=zf;

    *x = (long)fx;
    *y = (long)fy;
}
void CRecipeView::GetScreenXY(CRoilibAppDoc *pDoc, CxImage *ima, long *x, long *y)
{
    if (!ima || !pDoc) return;

    CPoint point=GetScrollPosition();

    *x = (double)*x * GetZoomFactor();
    *y = (double)*y * GetZoomFactor();

    double fx =(double)(*x - point.x);
    double fy =(double)(*y - point.y);

    *x = (long)fx;
    *y = (long)fy;
}
//////////////////////////////////////////////////////////////////////////////
// CRecipeView message handlers
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnMouseMove(UINT nFlags, CPoint point)
{
    CRoilibAppDoc* pDoc = GetDocument();

    TCHAR s[256];
    long x = point.x;
    long y = point.y;

    CxImage*  ima = m_ximage[theApp.m_nSelectCh]; //pDoc->GetImage();
    if (!ima)	{
        CDrawView::OnMouseMove(nFlags, point);
        return;
    }

    // We'll get the RGB values at the point the user selects
    GetImageXY(pDoc, ima, &x,&y);

    if (ima->IsInside(x,y))
    {
        //if (CDrawTool::c_drawShape == moving)
        //	SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HANDOPEN)));  //SetCursor(LoadCursor(0,IDC_HANDOPEN));

        long yflip = ima->GetHeight() - y - 1;
        _stprintf(s,_T("x: %d y: %d  idx: %d"), x, y, ima->GetPixelIndex(x,yflip));
        RGBQUAD rgb=ima->GetPixelColor(x,yflip);
        if (ima->AlphaIsValid()) rgb.rgbReserved=ima->AlphaGet(x,yflip);
        else rgb.rgbReserved=ima->GetPaletteColor(ima->GetPixelIndex(x,yflip)).rgbReserved;
        _stprintf(&s[_tcslen(s)],_T("  RGBA: (%d, %d, %d, %d)"), rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue, rgb.rgbReserved);

        if (CDrawTool::c_drawShape == tracker)
        {
            _stprintf(&s[_tcslen(s)],_T("  Width/Height : %.0f/%.0f"),
                m_tracker.m_rect.Width()*(1/GetZoomFactor()),m_tracker.m_rect.Height()*(1/GetZoomFactor()));
        }
    } else {
        _tcscpy(s,_T(" "));
    }

    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    if (pMainFrame != NULL) {
        CMFCRibbonStatusBar& statusBar = pMainFrame->GetStatusBar();
        statusBar.GetElement(0)->SetText(s);
        statusBar.Invalidate();
    }
    CDrawView::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
void CRecipeView::MoveViewImage(CPoint delta, double r, BOOL bAbs)
{
    CRoilibAppDoc* pDoc = GetDocument();
    CxImage* ima = m_ximage[theApp.m_nSelectCh]; //pDoc->GetImage();
    long x = (long)((1.0 / r) * delta.x);
    long y = (long)((1.0 / r) * delta.y);

    if (bAbs) {
        SetScrollPos(SB_HORZ,0);
        SetScrollPos(SB_VERT,0);
    }

    m_RefScroll = GetScrollPosition();

    CSize sz(GetTotalSize());
    CRect rect;
    GetClientRect(&rect);
    if (sz.cx>rect.right) SetScrollPos(SB_HORZ,m_RefScroll.x + x); else SetScrollPos(SB_HORZ,0);
    if (sz.cy>rect.bottom) SetScrollPos(SB_VERT,m_RefScroll.y + y); else SetScrollPos(SB_VERT,0);

    Invalidate();
}

//////////////////////////////////////////////////////////////////////////////
void CRecipeView::MoveCenterPosition(CPoint pt)
{
    //CRoilibAppDoc* pDoc = GetDocument();

    CSize sz(GetTotalSize());
    //CWnd* pMainFrame=GetParentFrame();
    CRect rect;
    GetClientRect(&rect);
    //pMainFrame->GetClientRect(rect);
    pt.x *= GetZoomFactor();
    pt.y *= GetZoomFactor();

    pt.x -= rect.Width()/2;
    pt.y -= rect.Height()/2;

    if (sz.cx>pt.x)
        SetScrollPos(SB_HORZ,pt.x);
    else SetScrollPos(SB_HORZ,0);

    if (sz.cy>pt.y)
        SetScrollPos(SB_VERT,pt.y);
    else SetScrollPos(SB_VERT,0);

    Invalidate();
}

//////////////////////////////////////////////////////////////////////////////
void CRecipeView::MoveBottomLeftPosition(CPoint pt)
{
    //CRoilibAppDoc* pDoc = GetDocument();

    CSize sz(GetTotalSize());
    CRect rect;
    GetClientRect(&rect);
    pt.x *= GetZoomFactor();
    pt.y *= GetZoomFactor();

    pt.x -= 20;
    pt.y -= (rect.Height()-20);

    if (sz.cx>pt.x) SetScrollPos(SB_HORZ,pt.x); else SetScrollPos(SB_HORZ,0);
    if (sz.cy>pt.y) SetScrollPos(SB_VERT,pt.y); else SetScrollPos(SB_VERT,0);

    Invalidate();
}
//////////////////////////////////////////////////////////////////////////////
BOOL CRecipeView::OnEraseBkgnd(CDC* /*pDC*/)
{
    return 1;
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnSetFocus(CWnd* pOldWnd)
{
    CScrollView::OnSetFocus(pOldWnd);

    CRoilibAppDoc* pDoc = GetDocument();
    if (pDoc) pDoc->UpdateStatusBar();
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnLButtonDown(UINT nFlags, CPoint point)
{
    //CRoilibAppDoc* pDoc = GetDocument();

    //if (pDoc) {
    //	CxImage* ima = pDoc->GetImage();
    //	if (!ima) return;
    //}

    CDrawView::OnLButtonDown(nFlags, point);
}
//////////////////////////////////////////////////////////////////////////////

void CRecipeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if (theApp.m_nSelectCh < 0)
        return;
    //CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);

    CRoilibAppDoc* pDoc = GetDocument();
    switch (lHint)
    {
    case WM_USER_NEWIMAGE:
        {
            //ClearTracker();
            //CRoilibAppDoc* pDoc = GetDocument();
            CxImage*  ima = m_ximage[theApp.m_nSelectCh]; //pDoc->GetImage();
            if (ima) {
                int px=GetScrollPos(SB_HORZ);
                int py=GetScrollPos(SB_VERT);
                CSize sz(GetTotalSize()); // 현재 Display 되어 있는 전체 Size
                int x=(int)(ima->GetWidth()*GetZoomFactor());
                int y=(int)(ima->GetHeight()*GetZoomFactor());
                SetScrollSizes(MM_TEXT,	CSize(x,y));
                CSize sz2(GetTotalSize()); // ZoomFactor 적용후 전체 Size

                //CWnd* pFrame=GetParentFrame();
                CRect rClient;
                GetClientRect(&rClient);

                // Zoom이 변할때 한번만 계산하면 된다.
                long x1 = 0; long y1 = 0;	GetImageXY(pDoc, ima, &x1, &y1);
                long x2 = (rClient.right - rClient.left); long y2 = (rClient.bottom - rClient.top);
                GetImageXY(pDoc, ima, &x2, &y2); // 실제 이미지가 표시되는 영역을 얻어옴.

                double zf = GetOldZoomFactor(); SetZoomFactor(GetZoomFactor());
                long x3 = 0; long y3 = 0;	GetImageXY(pDoc, ima, &x3, &y3, zf);
                long x4 = (rClient.right - rClient.left); long y4 = (rClient.bottom - rClient.top);
                GetImageXY(pDoc, ima, &x4, &y4, zf); // Zoom 이전에 이미지가 표시되는 영역을 얻어옴.

                int vh = (((x4-x3) - (x2-x1)) / 2) * GetZoomFactor() ; // 변화된 화면 Scroll 범위의 반.
                int vv = (((y4-y3) - (y2-y1)) / 2) * GetZoomFactor() ;
                //

                if (sz.cx!=0 && sz.cy!=0){
                    int h = (int)( (double)sz2.cx * ((double)px / sz.cx) + vh);
                    int v = (int)( (double)sz2.cy * ((double)py / sz.cy) + vv);

                    if (x>rClient.right) SetScrollPos(SB_HORZ, h);
                    else SetScrollPos(SB_HORZ,0);

                    if (y>rClient.bottom) SetScrollPos(SB_VERT, v);
                    else SetScrollPos(SB_VERT,0);
                    //TRACE("ScrPos = %d,%d\n", h, v);
                }
                if (!ima->SelectionIsValid()) KillTimer(1);

                Invalidate();
            }
        }

        break;

    default:
        {
        CRoilibAppDoc* pDoc = GetDocument();
        if (pDoc){
            CxImage*  ima = m_ximage[theApp.m_nSelectCh]; //pDoc->GetImage();
            if (ima){
                 SetScrollSizes(MM_TEXT,CSize((int)(ima->GetWidth()*GetZoomFactor()),
                                                    (int)(ima->GetHeight()*GetZoomFactor())));
            }
        }
        }
    }

    CDrawView::OnUpdate(pSender, lHint, pHint);



}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
/*
    //CRoilibAppDoc* pDoc = GetDocument();
    static UINT toolbars[]={
        IDR_MODELFRAME,
        IDR_MODELTOOLBAR1,
    };

    CDrawTool::c_last = point;
    BCMenu menu;
    menu.LoadMenu(IDM_CONTEXTMENU);
    menu.LoadToolbars(toolbars,2);
    CMenu *pContextMenu=menu.GetSubMenu(0);

    CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,point.x,point.y,pMainFrame);//AfxGetMainWnd());
*/
}
//////////////////////////////////////////////////////////////////////////////
LRESULT CRecipeView::OnNewImage(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    CRoilibAppDoc* pDoc = GetDocument();
    pDoc->UpdateAllViews(0,WM_USER_NEWIMAGE);
    pDoc->UpdateStatusBar();
    return 0;
}
//////////////////////////////////////////////////////////////////////////////
LRESULT CRecipeView::OnNewProgress(WPARAM wParam, LPARAM /*lParam*/)
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    CMFCRibbonStatusBar& statusBar = pMainFrame->GetStatusBar();

    CString s;
    s.Format(_T("%d %%"),(int)wParam);
    statusBar.GetExElement(0)->SetText(s);

    return 0;
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnDestroy()
{
    KillTimer(0);
    CScrollView::OnDestroy();
//	((CMainFrame*)AfxGetMainWnd())->m_HistoBar.Invalidate();
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnTimer(UINT_PTR nIDEvent)
{
    CDrawView::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_ESCAPE){
        //CMainFrame *pMainFrame = (CMainFrame*) AfxGetMainWnd();
        //if (pMainFrame->m_fullscreen){
        //	pMainFrame->PostMessage(WM_COMMAND, ID_VIEW_FULLSCREEN);
        //}
    }

    CDrawView::OnChar(nChar, nRepCnt, nFlags);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
    if (pActivateView)
    {
        CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetDocument();
        pDoc->m_pActiveMainView = (CRoilibAppView*)this;
    }

    CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);

    // invalidate selections when active status changes
    if (m_bActive != bActivate)
    {
        if( bActivate )  // if becoming active update as if active
        {
            m_bActive = bActivate;
            CRoilibAppDoc* pDoc = GetDocument();
            //SyncToolbarObjButtons();
        }
        if (!m_selection.IsEmpty()) {
            OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
            Invalidate();
        }
        m_bActive = bActivate;
    }
}
//////////////////////////////////////////////////////////////////////////////
BOOL CRecipeView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    return CDrawView::OnSetCursor(pWnd, nHitTest, message);
}
//////////////////////////////////////////////////////////////////////////////
bool CRecipeView::SetImageRectSelection(CDrawDoc *pDrawDoc,CRect *rect)
{
    if (pDrawDoc==0)
        return false;
    CRoilibAppDoc *pDoc = (CRoilibAppDoc*)pDrawDoc;

    CxImage* ima = m_ximage[theApp.m_nSelectCh]; //pDoc->GetImage();

    if (ima==0)
        return false;

    long x,y;
    RECT rect_img;
    x = rect_img.left = rect->left;
    y = rect_img.top = rect->top;
    GetImageXY(pDoc, ima, &x,&y);
    rect_img.top = ima->GetHeight() - 1 - y;
    rect_img.left = x;

    x = rect_img.right = rect->right;
    y = rect_img.bottom = rect->bottom;
    GetImageXY(pDoc, ima, &x,&y);
    rect_img.bottom = ima->GetHeight() - 1 - y;
    rect_img.right = x;

    ima->SelectionClear();
    ima->SelectionAddRect(rect_img);

    return true;
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CRoilibAppDoc* pDoc = GetDocument();
    if(nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
    {
        // User is dragging the scroll bar.
        // nPos is a legacy 16 bit coordinate.
        // Replace it with 32-bit position (ref: Q166473)
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_TRACKPOS;
        GetScrollInfo(SB_VERT, &si);
        nPos = si.nTrackPos;
    }
    //Invalidate();
    CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CRecipeView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CRoilibAppDoc* pDoc = GetDocument();
    if(nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
    {
        // User is dragging the scroll bar.
        // nPos is a legacy 16 bit coordinate.
        // Replace it with 32-bit position (ref: Q166473)
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_TRACKPOS;
        GetScrollInfo(SB_HORZ, &si);
        nPos = si.nTrackPos;
    }
    //Invalidate();
    CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CRecipeView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    CRoilibAppDoc* pDoc = GetDocument();
    // Ctrl+Wheel
    if ((nFlags & MK_CONTROL) != 0) {
        //TRACE("Wheel : zDelta=%d, pt=%d,%d\n", zDelta, pt.x, pt.y);
        if (zDelta > 0)
            OnViewZoomin();
        else
            OnViewZoomout();
    }

    BOOL r = CDrawView::OnMouseWheel(nFlags, zDelta, pt);
    //Invalidate();
    return r;
}


void CRecipeView::OnSize(UINT nType, int cx, int cy)
{
    CRoilibAppDoc* pDoc = GetDocument();
    CScrollView::OnSize(nType, cx, cy);
}

LRESULT CRecipeView::OnReloadRoiData(WPARAM wParam, LPARAM lParam)
{

    return TRUE;
}

#define IsSHIFTPressed() ( 0x8000 ==(GetKeyState(VK_SHIFT) & 0x8000   ))
#define IsCTRLPressed()  ( 0x8000 ==(GetKeyState(VK_CONTROL) & 0x8000 ))

void CRecipeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    int nOffset = 20;
    if (IsSHIFTPressed()) nOffset = 1;

    if(nChar == VK_DOWN)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_VERT, pt.y+nOffset);

            m_tracker.m_rect.top -= nOffset;
            m_tracker.m_rect.bottom -= nOffset;

    } else if(nChar == VK_UP)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_VERT, pt.y-nOffset);
        if (pt.y > 0) {
            m_tracker.m_rect.top += nOffset;
            m_tracker.m_rect.bottom += nOffset;
        }
    } else if(nChar == VK_RIGHT)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_HORZ,pt.x+nOffset);

            m_tracker.m_rect.left -= nOffset;
            m_tracker.m_rect.right -= nOffset;

    } else if(nChar == VK_LEFT)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_HORZ,pt.x-nOffset);
        if (pt.x > 0) {
            m_tracker.m_rect.left += nOffset;
            m_tracker.m_rect.right += nOffset;
        }
    }

    CDrawView::OnKeyDown(nChar, nRepCnt, nFlags);
    Invalidate();
}

BOOL CRecipeView::PreTranslateMessage(MSG* pMsg)
{
    CRoilibAppDoc* pDoc = GetDocument();
    if(pMsg->message == WM_KEYDOWN){
        switch(pMsg->wParam){
            case VK_ESCAPE:
                CDrawTool::c_drawShape = selection;
                return FALSE;
            case VK_DELETE:
                OnEditClear();
                return FALSE;
        }
    }
    return CDrawView::PreTranslateMessage(pMsg);
}

void CRecipeView::OnInspectionChannel(int index)
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);

    BeginWaitCursor();

    OtherObjectsClear();

    //int nOldCh = theApp.m_nSelectCh;
    theApp.m_nSelectCh = index;
    int nCam = theApp.m_nSelectCh;

    //if (m_ximage[theApp.m_nSelectCh] == NULL)
    {
        CString sFileName;
        sFileName.Format(_T("%s\\Recipe\\%s\\Pat%02d\\BaseImg.bmp"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, theApp.m_nSelectCh);
        if (FileExists(sFileName) == true) {
            m_ximage[theApp.m_nSelectCh] = new CxImage(sFileName, CXIMAGE_FORMAT_BMP);		// 화면에 나타내는 이미지는 RGB Color를 이용해서 추가적인 정보를 Color로 나타낼수 있도록 하자.

            CSize sizeTotal;
            sizeTotal.cx = m_ximage[theApp.m_nSelectCh]->GetWidth();
            sizeTotal.cy = m_ximage[theApp.m_nSelectCh]->GetHeight();
            SetScrollSizes(MM_TEXT, sizeTotal);
            CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[theApp.m_nSelectCh];

            if (pProcessingClass->iplImage)
                cvReleaseImage(&pProcessingClass->iplImage);
            if (FileExists(sFileName) == true) {
                CT2A ascii(sFileName);
                pProcessingClass->iplImage = cvLoadImage(ascii, 0);
            }
        }
    }

    pMainFrame->RecipeLoad();
    pMainFrame->m_wndProperties.InitPropList();

    EndWaitCursor();

    CRoilibAppDoc* pDoc = GetDocument();
    pDoc->SetChannel(theApp.m_nSelectCh);
}

void CRecipeView::OnInspectionChannel()
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    CMFCRibbonComboBox* pCbChannel = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pMainFrame->m_wndRibbonBar.FindByID(ID_INSPECTION_CHANNEL));
    int index = pCbChannel->GetCurSel();

    OnInspectionChannel(index);
}


void CRecipeView::OnUpdateInspectionChannel(CCmdUI *pCmdUI)
{
}


//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewToolsMove()
{
    CDrawTool::c_drawShape = moving;
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnUpdateViewToolsMove(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == moving);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewToolsSelect()
{
    CDrawTool::c_drawShape = selection;
}
void CRecipeView::OnUpdateViewToolsSelect(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == selection);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewToolsTracker()
{
    CDrawTool::c_drawShape = tracker;
}
void CRecipeView::OnUpdateViewToolsTracker(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == DrawShape::tracker);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewToolsMeasure()
{
    //CDrawTool::c_drawShape = line;
}
void CRecipeView::OnUpdateViewToolsMeasure(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == DrawShape::line);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewToolsDrawRect()
{
    CDrawTool::c_drawShape = roi;
}
void CRecipeView::OnUpdateViewToolsDrawRect(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == roi);
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnViewToolsDrawRoiPat()
{
    CDrawTool::c_drawShape = roipat;
}
void CRecipeView::OnUpdateViewToolsDrawRoiPat(CCmdUI *pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == roipat);
}


void CRecipeView::OnViewToolsDrawline()
{
    CDrawTool::c_drawShape = DrawShape::line;
}

void CRecipeView::OnUpdateViewToolsDrawline(CCmdUI *pCmdUI)
{
    pCmdUI->SetRadio(CDrawTool::c_drawShape == DrawShape::line);
}

void CRecipeView::OnRecipeImagesave()
{
    int nRet;

    if (m_ximage[theApp.m_nSelectCh] == NULL)
        return;

    nRet = AfxMessageBox(_S(IDS_SETTING_BASE_IMAGE), MB_OKCANCEL);

    if (nRet == IDOK)
    {
        BeginWaitCursor();
        //		BaseImageSet(theApp.m_nSelectCh);

        CString sFileName, str;
        CRect Roi;
        __int64 nSTick, nETick;
        int width = 0;
        int height = 0;
        // 해당 채널이 존재하지 않을 경우 새로 생성//
        sFileName.Format(_T("%s\\Recipe\\%s\\Pat%02d"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, theApp.m_nSelectCh);
        if (g_pDirectory->Search_FileName(sFileName) == FALSE){
            g_pDirectory->Create_Directory(sFileName);
        }

        sFileName.Format(_T("%s\\Recipe\\%s\\Pat%02d\\BaseImg.bmp"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, theApp.m_nSelectCh);
        //MbufExport((LPCTSTR)sFileName, M_BMP, pProcessingClass->MilImage);
        //CT2A ascii(sFileName); cvSaveImage(ascii, m_ximage[theApp.m_nSelectCh]);
        m_ximage[theApp.m_nSelectCh]->Save(sFileName, CXIMAGE_FORMAT_BMP);

        EndWaitCursor();
    }
}


void CRecipeView::OnUpdateEditClear(CCmdUI *pCmdUI)
{
}
//////////////////////////////////////////////////////////////////////////////
void CRecipeView::OnEditClear()
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    //POSITION pos = GetFirstViewPosition();
    CDrawView* pView = pMainFrame->m_pRecipeView;//(CDrawView*)GetNextView(pos);

    CRoilibAppDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);


    // update all the views before the selection goes away
    pDoc->UpdateAllViews(NULL, HINT_DELETE_SELECTION, &pView->m_selection);
    OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

    BeginWaitCursor();
    POSITION pos = pView->m_selection.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = pView->m_selection.GetNext(pos);
        // 패턴Object인경우 pat 와 cross를 자식으로 가지고 있다.
        // 이런경우 pat 와 cross는 Parent를 가지고 있다.
        if (pObj->m_pParent == NULL) {
            pMainFrame->m_wndProperties.DeletePropertyItem(pObj);
            pView->RegisterUndoList(pObj);

            //g_cRecipeData->m_vecRoiData 내에 roiData를 삭제한다.
            pMainFrame->DeleteReceipe(pObj->m_pRoiData);

        }
    }

    // doc 와 view에 있는 object를 삭제한다.
    pDoc->OnEditClear();
    EndWaitCursor();

    pView->m_selection.RemoveAll();
    Invalidate();

}

void CRecipeView::OnRecipeImageload()
{
    CRoilibAppDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CString filename;// (newName);
    //static CString newName;
    if (!pDoc->PromptForFileName(filename, AFX_IDS_OPENFILE,
        OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, TRUE, 0))
        return; // open cancelled

    CString ext(pDoc->FindExtension(filename));
    ext.MakeLower();
    if (ext == _T("")) return;

    int type =pDoc-> FindType(ext);

    if (m_ximage[theApp.m_nSelectCh] != NULL)
        delete m_ximage[theApp.m_nSelectCh];
    m_ximage[theApp.m_nSelectCh] = new CxImage(filename, type);

    CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[theApp.m_nSelectCh];

    CSize sizeTotal;
    sizeTotal.cx = m_ximage[theApp.m_nSelectCh]->GetWidth();
    sizeTotal.cy = m_ximage[theApp.m_nSelectCh]->GetHeight();
    SetScrollSizes(MM_TEXT, sizeTotal);

    if (pProcessingClass->iplImage)
        cvReleaseImage(&pProcessingClass->iplImage);
    if (FileExists(filename) == true) {
        CT2A ascii(filename);
        pProcessingClass->iplImage = cvLoadImage(ascii, 0);
	}


    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    pMainFrame->RecipeLoad();
    pMainFrame->m_wndProperties.InitPropList();

    pDoc->m_pActiveMainView = this;
    pDoc->m_pActiveMainView->m_ximage = m_ximage[theApp.m_nSelectCh];
    pDoc->UpdateStatusBar();
    pDoc->UpdateAllViews(NULL);

    CString sFileName;
    sFileName.Format(_T("%s\\Recipe\\%s\\Pat%02d"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, theApp.m_nSelectCh);
    if (g_pDirectory->Search_FileName(sFileName) == FALSE){
        g_pDirectory->Create_Directory(sFileName);
    }

}

void CRecipeView::OnRecipeGrabStart()
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    int nCh = theApp.m_nSelectCh;

    if (theApp.m_vecProcessingClass.size() >= nCh)
    {
        CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[nCh];
        pProcessingClass->AreaCamImageGrab(TRUE);	// iplImage에 저장.
        if (pProcessingClass->cimg == NULL)
            return;

        int nCh = theApp.m_nSelectCh;
        int sx = pProcessingClass->cimg->width;
        int sy = pProcessingClass->cimg->height;

        if (m_ximage[nCh] == NULL)
            m_ximage[nCh] = new CxImage(sx, sy, 24, CXIMAGE_FORMAT_BMP);

        int tx = m_ximage[nCh]->GetWidth();
        int ty = m_ximage[nCh]->GetHeight();

        if (sx != tx || sy != ty) {
            delete m_ximage[nCh];
            m_ximage[nCh] = new CxImage(sx, sy, 24, CXIMAGE_FORMAT_BMP); // m_ximage(CxImage) Color로 화면에 Display하는 이미지.
        }

        theApp.Opencv2ximage(pProcessingClass->iplImage, m_ximage[nCh]);	// 그랩한 이미지를 화면에 실시간 Display한다.
        Invalidate(FALSE);

        SYSTEMTIME SystemTime;
        GetLocalTime(&SystemTime);
        CString st;
        CTime t = CTime::GetCurrentTime();
        st.Format(_T("%02d%02d%02d%03d"), t.GetHour(), t.GetMinute(), t.GetSecond(), SystemTime.wMilliseconds);
        pProcessingClass->GrabImageSave(st);
    }
}

void CRecipeView::OnRecipeInspection()
{
    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    BOOL oldPreviewMode = theApp.m_bPreviewMode;
    theApp.m_bPreviewMode = FALSE;

    CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetDocument();
    bool bResult = FALSE;

    OtherObjectsClear();

    int nCh = theApp.m_nSelectCh;

    theApp.m_ImgProcEngine.InitEngine(nCh);
    CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[nCh];
    if (pProcessingClass->iplImage) {
        for (int i = 0; i < (int)g_cRecipeData->m_vecWorkRoiData[nCh].size(); i++)
        {
            CRoiData *pData = g_cRecipeData->m_vecWorkRoiData[nCh][i];
            if (pData->m_nCh != nCh)
                continue;

            pData->m_vecDetectResult.clear();
            theApp.m_ImgProcEngine.InspectOneItem(nCh, pProcessingClass->iplImage, pData);


			int size = pData->m_vecDetectResult.size();
			for (int i = 0; i < size; i++) {
				DetectResult *prst = &pData->m_vecDetectResult[i];

				if (pData->m_nInspectType == _Inspect_BarCode)
				{
					CDrawObj* pObj = new CDrawRect(CRect(0, 0, 600, 30));
					_tcscpy(pObj->m_text, prst->strResult);
					pObj->m_dwType = DRAWOBJ_TYPE_RESULT;
					pObj->m_pDocument = pDoc;
					pObj->m_nShape = CDrawObj::text;
					pObj->m_ZoomFactor = GetZoomFactor();
					_stprintf(pObj->m_pRoiData->m_sName, _T(""));
					m_otherObjects.AddTail(pObj);
				}
				else {
					CDrawObj* pObj = new CDrawRect(CRect(prst->pt.x, prst->pt.y, prst->pt1.x, prst->pt1.y));
					pObj->m_dwType = DRAWOBJ_TYPE_RESULT;
					pObj->SetLineColor(RGB(255, 0, 0));
					pObj->m_pDocument = pDoc;
					pObj->m_nShape = CDrawObj::ellipse;
					pObj->m_ZoomFactor = GetZoomFactor();
					pObj->SetLineWidth(2);
					_stprintf(pObj->m_pRoiData->m_sName, _T("%.2f %.2f"), prst->pt.x, prst->pt.y);
					m_otherObjects.AddTail(pObj);
				}
			}

        }
    }
    pMainFrame->Invalidate();

    theApp.m_bPreviewMode = oldPreviewMode;
}


void CRecipeView::OtherObjectsClear(DWORD dwFilter)
{
    POSITION pos = m_otherObjects.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_otherObjects.GetNext(pos);
        if ((pObj->m_dwType&dwFilter) != 0)
            delete pObj;
    }
    m_otherObjects.RemoveAll();
}

void CRecipeView::Draw(CDC* pDC, CDrawView* pView)
{
    POSITION pos = m_otherObjects.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_otherObjects.GetNext(pos);
        pObj->Draw(pDC);
    }
}

void CRecipeView::OnButtonPreview()
{
    theApp.m_bPreviewMode = !theApp.m_bPreviewMode;
}
void CRecipeView::OnUpdateButtonPreview(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!theApp.m_bPreviewMode);
}


void CRecipeView::OnButtonStop()
{
    theApp.m_bPreviewMode = !theApp.m_bPreviewMode;
}
void CRecipeView::OnUpdateButtonStop(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(theApp.m_bPreviewMode);
}
