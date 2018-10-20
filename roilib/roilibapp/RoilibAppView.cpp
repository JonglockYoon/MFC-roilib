//
// Copyright (C) 2018
// All rights reserved by jerry1455@gmail.com
//
// RoilibAppView.cpp : implementation of the CRoilibAppView class
//

#include "stdafx.h"
#include "RoilibApp.h"

#include "MainFrm.h"

#include "RoilibAppDoc.h"
#include "RoilibAppView.h"
#include "UI/memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRoilibAppView

IMPLEMENT_DYNCREATE(CRoilibAppView, CDrawView)

BEGIN_MESSAGE_MAP(CRoilibAppView, CDrawView)
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CRoilibAppView::OnFilePrintPreview)

    ON_COMMAND(ID_VIEW_TOOLS_MOVE, OnViewToolsMove)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_MOVE, OnUpdateViewToolsMove)
    ON_COMMAND(ID_VIEW_TOOLS_SELECT, OnViewToolsSelect)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_SELECT, OnUpdateViewToolsSelect)
    ON_COMMAND(ID_VIEW_TOOLS_TRACKER, OnViewToolsTracker)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_TRACKER, OnUpdateViewToolsTracker)
    ON_COMMAND(ID_VIEW_TOOLS_MEASURE, &OnViewToolsMeasure)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLS_MEASURE, &OnUpdateViewToolsMeasure)

    ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomin)
    ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
    ON_COMMAND(ID_VIEW_NORMALVIEWING11, OnViewNormalviewing11)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NORMALVIEWING11, OnUpdateViewNormalviewing11)

    ON_WM_KEYDOWN()
    ON_WM_MOUSEWHEEL()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_MOUSEMOVE()
    ON_WM_SHOWWINDOW()
    ON_COMMAND(ID_BUTTON_PREVIEW, &CRoilibAppView::OnButtonPreview)
    ON_COMMAND(ID_BUTTON_STOP, &CRoilibAppView::OnButtonStop)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_PREVIEW, &CRoilibAppView::OnUpdateButtonPreview)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_STOP, &CRoilibAppView::OnUpdateButtonStop)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CRoilibAppView construction/destruction

CRoilibAppView::CRoilibAppView()
{
    // TODO: add construction code here
    m_nMapMode = MM_TEXT;
    VERIFY(m_brHatch.CreateHatchBrush(HS_DIAGCROSS, RGB(191, 191, 191)));

    m_OldZF = m_ZoomFactor = 1;
    m_ximage = NULL;
    m_nIndex = -1;
    m_bEditEnable = FALSE;
}

CRoilibAppView::~CRoilibAppView()
{
    OtherObjectsClear();
    if (m_ximage != NULL)
        delete m_ximage;
}

BOOL CRoilibAppView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CScrollView::PreCreateWindow(cs);
}

// CRoilibAppView drawing

void CRoilibAppView::OnDraw(CDC* pDC)
{
    CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetDocument();
    ASSERT_VALID(pDoc);
    //if (!pDoc)
    //	return;

    CxMemDC* pMemDC = NULL;
    pDC = pMemDC = new CxMemDC(pDC);

    if (m_brHatch.m_hObject){
        CRect rect;
        GetClientRect(&rect);
        rect.right  = MAX(rect.right , m_totalDev.cx);
        rect.bottom = MAX(rect.bottom, m_totalDev.cy);
        m_brHatch.UnrealizeObject();
        CPoint pt(0, 0);
        pDC->LPtoDP(&pt);
        pt = pDC->SetBrushOrg(pt.x % 8, pt.y % 8);
        CBrush* old = pDC->SelectObject(&m_brHatch);
        pDC->FillRect(&rect, &m_brHatch);
        pDC->SelectObject(old);
    }

    // CRoilibAppView에서는 4개의 Grab이미지를 합친 이미지를 관리할것이므로
    // 여기서는 RoilibAppView마다 별도의 Grab이미지를 관리하도록 CxImage instance를 한다.
    //CxImage* ima = pDoc->GetImage();
    if (m_ximage) {

        double zoom=GetZoomFactor();
        if (zoom==1)
            m_ximage->Draw(pDC->GetSafeHdc());
        else
            m_ximage->Draw(pDC->GetSafeHdc(),
                CRect(0,0,(int)(m_ximage->GetWidth()*zoom),(int)(m_ximage->GetHeight()*zoom)),
                0, theApp.m_bSmoothMode); // 마지막 TRUE는 Smooth Mode

        DrawGride(pDC);

        SetZoomFactor(zoom);
        Draw(pDC, this);
        pDoc->SetZoomFactor(zoom);
        pDoc->DrawLine(pDC, this, m_nIndex);

        m_rulerType = RT_HORIZONTAL; // 12.4
        x1 = -(gCfg.m_dRulerMaxX / 2); // -6.2;
        x2 = (gCfg.m_dRulerMaxX / 2); //6.2;
        RulerDraw(pDC);
        m_rulerType = RT_VERTICAL; // 9.3
        y1 = -(gCfg.m_dRulerMaxY / 2);// -4.6;
        y2 = (gCfg.m_dRulerMaxY / 2);// 4.6;
        RulerDraw(pDC);

        // Draw Good Circle
        CPen pen;
        CPen* pOldPen;
        COLORREF m_color = RGB(0, 255, 0);
        pen.CreatePen(PS_SOLID, 2, m_color);
        pOldPen = pDC->SelectObject(&pen);
        //int oldBkMode = pDC->SetBkMode(TRANSPARENT);
        CBrush *pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);


        CRect rect = CRect(0, 0, (int)(m_ximage->GetWidth()*zoom), (int)(m_ximage->GetHeight()*zoom));
        int x = rect.left + rect.Width() / 2;
        int y = rect.top + rect.Height() / 2;

        double dX = gCfg.m_dRulerMaxX / rect.right; // 1개 dot당 거리.
        double dY = gCfg.m_dRulerMaxY / rect.bottom;

        pDC->SelectObject(pOldPen);
        pDC->SelectObject(pOldBrush);


    }

    CDrawView::OnDraw(pDC);

    DrawAlphaBlend(*pDC);

    delete pMemDC;
}

void CRoilibAppView::RulerDraw(CDC* pDC)
{
    // set the map mode right
    int oldMapMode = pDC->SetMapMode(MM_TEXT);

    CFont vFont;
    CFont hFont;
    vFont.CreateFont(20, 0, 900, 900, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, _T("Times New Roman"));
    hFont.CreateFont(22, 0, 000, 000, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, _T("Times New Roman"));

    CFont* pOldFont = pDC->SelectObject((m_rulerType == RT_HORIZONTAL) ? &hFont : &vFont);
    int oldTextAlign = pDC->SetTextAlign((m_rulerType == RT_HORIZONTAL) ? (TA_RIGHT | TA_TOP) : (TA_LEFT | TA_TOP));
    int oldBkMode = pDC->SetBkMode(TRANSPARENT);

    CRect rulerRect;
    double zoom = GetZoomFactor();
    rulerRect = CRect(0, 0, (int)(m_ximage->GetWidth()*zoom), (int)(m_ximage->GetHeight()*zoom));

    if (m_rulerType == RT_HORIZONTAL)
    {
        int c = rulerRect.top + rulerRect.Height() / 2;
        rulerRect.top = c - 30;
        rulerRect.bottom = c;
    }
    else //(m_rulerType==RT_VERTICAL)
    {
        int c = rulerRect.left + rulerRect.Width() / 2;
        rulerRect.left = c - 20;
        rulerRect.right = c - 8;
    }

    CBrush* pOldBrush;

    COLORREF m_color = RGB(255, 0, 0);
    CBrush backBrush(m_color);
    pOldBrush = pDC->SelectObject(&backBrush);
    pDC->SetTextColor(m_color);

    DrawTicker(pDC, rulerRect);

    pDC->SelectObject(pOldBrush);

    // restore DC settings
    pDC->SetMapMode(oldMapMode);
    pDC->SelectObject(pOldFont);
    pDC->SetTextAlign(oldTextAlign);
    pDC->SetBkMode(oldBkMode);
}


void CRoilibAppView::DrawTicker(CDC* pDC, CRect rulerRect, BOOL bShowPos)
{
    if (m_rulerType == RT_HORIZONTAL)
    {
        gtk_hruler_draw_ticks(pDC, rulerRect, bShowPos);
        return;
    }
    else {
        gtk_vruler_draw_ticks(pDC, rulerRect, bShowPos);
        return;
    }
}

static const GtkRulerMetric ruler_metrics = { "Pixels", "Pi", 1.0, { 1, 2, 5, 10, 25, 50, 100, 250, 500, 1000 }, { 1, 5, 10, 50, 100 } };

void CRoilibAppView::gtk_hruler_draw_ticks(CDC* pDC, CRect rulerRect, BOOL bShowPos)
{
    int i;
    int width, height;
    int xthickness;
    int ythickness;
    int length, ideal_length;
    float lower, upper;		/* Upper and lower limits, in ruler units */
    float increment;		/* Number of pixels per unit */
    int scale;			/* Number of units per major unit */
    float subd_incr;
    float start, end, cur;
    char unit_str[32];
    int digit_height;
    int text_width;
    int pos;

    GtkRuler *ruler = &rruler;
    ruler->lower = x1;
    ruler->upper = x2;
    ruler->position = 0;
    ruler->max_size = x2 - x1;// xEnd - xStart;
    ruler->metric = (GtkRulerMetric *)&ruler_metrics;

    xthickness = 0;// (int)(nFactor*m_fZoomFactor);// widget->style->klass->xthickness;
    ythickness = 0;// (int)(nFactor*m_fZoomFactor);// widget->style->klass->ythickness;
    digit_height = 12;// font->ascent; /* assume descent == 0 ? */

    //if (m_DocSize.cx == 0 || m_DocSize.cy == 0)
    //	return;
    width = rulerRect.Width();// widget->allocation.width;
    height = rulerRect.Height();// widget->allocation.height - ythickness * 2;
    if (width == 0 || height == 0)
        return;

    i = 0;
    //pDC->PatBlt(xthickness + 1, rulerRect.top + (ythickness + 2), 1, height + ythickness, BLACKNESS);

    upper = ruler->upper / ruler->metric->pixels_per_unit;
    lower = ruler->lower / ruler->metric->pixels_per_unit;

    if ((upper - lower) == 0)
        return;
    increment = (float)width / (upper - lower);

    /* determine the scale
    *  We calculate the text size as for the vruler instead of using
    *  text_width = gdk_string_width(font, unit_str), so that the result
    *  for the scale looks consistent with an accompanying vruler
    */
    scale = ceil(ruler->max_size / ruler->metric->pixels_per_unit);
    sprintf(unit_str, "%d", scale);
    text_width = strlen(unit_str) * digit_height + 1;

    for (scale = 0; scale < MAXIMUM_SCALES; scale++)
        if (ruler->metric->ruler_scale[scale] * fabs(increment) > 2 * text_width)
            break;

    if (scale == MAXIMUM_SCALES)
        scale = MAXIMUM_SCALES - 1;

    /* drawing starts here */
    length = 0;
    for (i = MAXIMUM_SUBDIVIDE - 1; i >= 0; i--)
    {
        subd_incr = (float)ruler->metric->ruler_scale[scale] /
            (float)ruler->metric->subdivide[i];
        if (subd_incr * fabs(increment) <= MINIMUM_INCR)
            continue;

        /* Calculate the length of the tickmarks. Make sure that
        * this length increases for each set of ticks
        */
        ideal_length = height / (i + 1) - 1;
        if (ideal_length > ++length)
            length = ideal_length;

        if (lower < upper)
        {
            start = floor(lower / subd_incr) * subd_incr;
            end = ceil(upper / subd_incr) * subd_incr;
        }
        else
        {
            start = floor(upper / subd_incr) * subd_incr;
            end = ceil(lower / subd_incr) * subd_incr;
        }


        for (cur = start; cur <= end; cur += subd_incr)
        {
            pos = ROUND((cur - lower) * increment);

            pDC->PatBlt(pos, rulerRect.top + (height - length + ythickness), 1, length + ythickness, PATCOPY);// BLACKNESS);

            /* draw label */
            if (i == 0)
            {
                sprintf(unit_str, "%d", (int)cur);

                if (bShowPos)
                    pDC->TextOut(pos + strlen(unit_str) * 5 + 8, rulerRect.top + (ythickness - 1), CString(unit_str));
            }
        }
    }
}

void CRoilibAppView::gtk_vruler_draw_ticks(CDC* pDC, CRect rulerRect, BOOL bShowPos)
{
    int i, j;
    int width, height;
    int xthickness;
    int ythickness;
    int length, ideal_length;
    float lower, upper;		/* Upper and lower limits, in ruler units */
    float increment;		/* Number of pixels per unit */
    int scale;			/* Number of units per major unit */
    float subd_incr;
    float start, end, cur;
    char unit_str[32];
    char digit_str[2] = { '\0', '\0' };
    int digit_height;
    int text_height;
    int pos;

    GtkRuler *ruler = &rruler;
    ruler->lower = y2;
    ruler->upper = y1;
    ruler->position = 0;
    ruler->max_size = y2 - y1;// yEnd - yStart;
    ruler->metric = (GtkRulerMetric *)&ruler_metrics;

    xthickness = 0;// widget->style->klass->xthickness;
    ythickness = 0;// widget->style->klass->ythickness;
    digit_height = 12;// font->ascent; /* assume descent == 0 ? */

    //if (m_DocSize.cx == 0 || m_DocSize.cy == 0)
    //	return;
    width = rulerRect.Height();// widget->allocation.height;
    height = rulerRect.Width();// widget->allocation.width - ythickness * 2;
    if (width == 0 || height == 0)
        return;

    i = 0;
    //pDC->PatBlt(rulerRect.left + (xthickness + 2), ythickness, 1, height + ythickness, BLACKNESS);

    upper = ruler->upper / ruler->metric->pixels_per_unit;
    lower = ruler->lower / ruler->metric->pixels_per_unit;

    if ((upper - lower) == 0)
        return;
    increment = (float)width / (upper - lower);

    /* determine the scale
    *   use the maximum extents of the ruler to determine the largest
    *   possible number to be displayed.  Calculate the height in pixels
    *   of this displayed text. Use this height to find a scale which
    *   leaves sufficient room for drawing the ruler.
    */
    scale = ceil(ruler->max_size / ruler->metric->pixels_per_unit);
    sprintf(unit_str, "%d", scale);
    text_height = strlen(unit_str) * digit_height + 1;

    for (scale = 0; scale < MAXIMUM_SCALES; scale++)
        if (ruler->metric->ruler_scale[scale] * fabs(increment) > 2 * text_height)
            break;

    if (scale == MAXIMUM_SCALES)
        scale = MAXIMUM_SCALES - 1;

    /* drawing starts here */
    length = 0;
    for (i = MAXIMUM_SUBDIVIDE - 1; i >= 0; i--)
    {
        subd_incr = (float)ruler->metric->ruler_scale[scale] /
            (float)ruler->metric->subdivide[i];
        if (subd_incr * fabs(increment) <= MINIMUM_INCR)
            continue;

        /* Calculate the length of the tickmarks. Make sure that
        * this length increases for each set of ticks
        */
        ideal_length = height / (i + 1) - 1;
        if (ideal_length > (length+=5))
            length = ideal_length;

        if (lower < upper)
        {
            start = floor(lower / subd_incr) * subd_incr;
            end = ceil(upper / subd_incr) * subd_incr;
        }
        else
        {
            start = floor(upper / subd_incr) * subd_incr;
            end = ceil(lower / subd_incr) * subd_incr;
        }

        for (cur = start; cur <= end; cur += subd_incr)
        {
            pos = ROUND((cur - lower) * increment);

            pDC->PatBlt(rulerRect.left + (height + xthickness - length), pos, height + xthickness, 1, PATCOPY);// BLACKNESS);

            memset(unit_str, 0, sizeof(unit_str));
            /* draw label */
            if (i == 0)
            {
                sprintf(unit_str, "%d", (int)cur);
                pDC->TextOut(rulerRect.left + (xthickness - 1) - 10, pos - 4, CString(unit_str));
            }
        }
    }
}

void CRoilibAppView::DrawAlphaBlend(HDC hdcwnd)
{
    HDC hdc;               // handle of the DC we will create
    BLENDFUNCTION bf;      // structure for alpha blending
    HBITMAP hbitmap;       // bitmap handle
    BITMAPINFO bmi;        // bitmap header
    VOID *pvBits;          // pointer to DIB section
    ULONG   ulBitmapWidth, ulBitmapHeight;      // bitmap width/height
    RECT    rt;            // used for getting window dimensions
    UINT32   x, y;          // stepping variables
    static BOOL bOn = TRUE;

    // get window dimensions
    GetClientRect(&rt);

    // create a DC for our bitmap -- the source DC for AlphaBlend
    hdc = CreateCompatibleDC(hdcwnd);

    // zero the memory for the bitmap info
    ZeroMemory(&bmi, sizeof(BITMAPINFO));

    // setup bitmap info
    // set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas.
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = ulBitmapWidth = 60;
    bmi.bmiHeader.biHeight = ulBitmapHeight = 20;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = ulBitmapWidth * ulBitmapHeight * 4;

    // create our DIB section and select the bitmap into the dc
    hbitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
    SelectObject(hdc, hbitmap);

    // constant alpha = 20%, but no source alpha the color format for each pixel is 0xaarrggbb
    // set all pixels to yellow and set source alpha to zero
    for (y = 0; y < ulBitmapHeight; y++)
        for (x = 0; x < ulBitmapWidth; x++)
            ((UINT32 *)pvBits)[x + y * ulBitmapWidth] = 0x00ffff00;

    //CBrush brush1(RGB(255, 0, 0));
    //RECT rect = CRect(2,2,18,18);
    if (bOn)
    {
        int x = 0;
        HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(hdc, hRedBrush);
        Ellipse(hdc, x+6, 2, x+18, 18);
        DeleteObject(hRedBrush);
    }
    bOn = !bOn;

    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.SourceConstantAlpha = 255.0 * 0.2;// transparency
    bf.AlphaFormat = 0;             // ignore source alpha channel

    CPoint pt = GetScrollPosition();
    pt.x += (rt.right - rt.left) / 2 - 30;
    pt.y += rt.bottom - 30;
    AlphaBlend(hdcwnd, pt.x, pt.y,
        ulBitmapWidth, ulBitmapHeight,
        hdc, 0, 0, ulBitmapWidth, ulBitmapHeight, bf);

    // do cleanup
    DeleteObject(hbitmap);
    DeleteDC(hdc);

}

void CRoilibAppView::OnInitialUpdate()
{
    CScrollView::OnInitialUpdate();

    CSize sizeTotal;

    CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
    for (int i = 0; i<MAX_CH_NUM; i++) // [0][0],[0][1] 순으로 Window포인터가 저장된다.
    {
        if (pFrame->m_pMainView[i] == NULL)
        {
            m_nIndex = i;
            pFrame->m_pMainView[i] = this;
            break;
        }
    }

    CString sFileName;
    sFileName.Format(_T("%s\\Recipe\\%s\\Pat%02d\\BaseImg.bmp"), theApp.g_sRootPath, gCfg.m_sLastRecipeName, m_nIndex);
    if (FileExists(sFileName) == true) {
        CProcessingClass *pProcessingClass = theApp.m_vecProcessingClass[m_nIndex];
        m_ximage = new CxImage(sFileName, CXIMAGE_FORMAT_BMP);
    }
    else m_ximage = new CxImage(DEFAULT_CAM_WIDTH, DEFAULT_CMA_HEIGHT, 24, CXIMAGE_FORMAT_BMP);

    sizeTotal.cx = m_ximage->GetWidth();
    sizeTotal.cy = m_ximage->GetHeight();
    SetScrollSizes(MM_TEXT, sizeTotal);

    CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetDocument();
    if (pDoc->m_pActiveMainView == NULL)
    {
        pDoc->m_pActiveMainView = (CRoilibAppView*)this;
        //m_bActive = TRUE;
    }
}


// CRoilibAppView printing


void CRoilibAppView::OnFilePrintPreview()
{
    AFXPrintPreview(this);
}

BOOL CRoilibAppView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CRoilibAppView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CRoilibAppView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}

void CRoilibAppView::OnRButtonUp(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void CRoilibAppView::OnContextMenu(CWnd* pWnd, CPoint point)
{
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


void CRoilibAppView::DrawGride(CDC* pDC)
{
    CRoilibAppDoc* pDoc = GetDocument();
    CxImage*  ima = m_ximage; //pDoc->GetImage();
    if (!ima)	return;

    //if ((GetKeyState(VK_LBUTTON) & 0x80) != 0) // LBUTTON down일때는 그리지 않는다.
    //	return;

    CPen pen(PS_SOLID, 1, RGB(176, 196, 222));
    CPen* pOldPen = pDC->SelectObject(&pen);

    CRect rect;
    GetClientRect(&rect);

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
    pDC->SelectObject(pOldPen);
}


// CRoilibAppView diagnostics

#ifdef _DEBUG
void CRoilibAppView::AssertValid() const
{
    CScrollView::AssertValid();
}

void CRoilibAppView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}

CRoilibAppDoc* CRoilibAppView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRoilibAppDoc)));
    return (CRoilibAppDoc*)m_pDocument;
}
#endif //_DEBUG


// CRoilibAppView message handlers

void CRoilibAppView::SetZoomCal(double zf) {
    SetZoomFactor(zf);
    OnUpdate(NULL, WM_USER_NEWIMAGE, NULL);
}

void CRoilibAppView::OnViewZoomin()
{
    if (m_ZoomFactor>=32) return;

    m_OldZF = m_ZoomFactor;

    if		(m_ZoomFactor == 0.50f) m_ZoomFactor = 0.75f;
    else if (m_ZoomFactor == 0.75f) m_ZoomFactor = 1.00f;
    else if (m_ZoomFactor == 1.00f)	m_ZoomFactor = 1.50f;
    else if (m_ZoomFactor == 1.50f)	m_ZoomFactor = 2.00f;
    else if (m_ZoomFactor > 1.0f)	m_ZoomFactor+=1;
    else m_ZoomFactor*=2;

    OnUpdate(NULL,WM_USER_NEWIMAGE,NULL);
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnViewZoomout()
{
    if (m_ZoomFactor<0.01) return;

    m_OldZF = m_ZoomFactor;

    if		(m_ZoomFactor == 2.00f) m_ZoomFactor = 1.50f;
    else if (m_ZoomFactor == 1.50f) m_ZoomFactor = 1.00f;
    else if (m_ZoomFactor == 1.00f)	m_ZoomFactor = 0.75f;
    else if (m_ZoomFactor == 0.75f)	m_ZoomFactor = 0.50f;
    else if (m_ZoomFactor > 1.0) m_ZoomFactor-=1;
    else m_ZoomFactor/=2.0;

    OnUpdate(NULL,WM_USER_NEWIMAGE,NULL);
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnUpdateViewZoomin(CCmdUI* pCmdUI)
{
    if (m_ZoomFactor>=32) pCmdUI->Enable(0);
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnUpdateViewZoomout(CCmdUI* pCmdUI)
{
    if (m_ZoomFactor<0.01) pCmdUI->Enable(0);
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnViewNormalviewing11()
{
    m_OldZF = m_ZoomFactor;
    m_ZoomFactor=1;
    OnUpdate(NULL,WM_USER_NEWIMAGE,NULL);

}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnUpdateViewNormalviewing11(CCmdUI* pCmdUI)
{
}

#define IsSHIFTPressed() ( 0x8000 ==(GetKeyState(VK_SHIFT) & 0x8000   ))
#define IsCTRLPressed()  ( 0x8000 ==(GetKeyState(VK_CONTROL) & 0x8000 ))

void CRoilibAppView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    int nOffset = 20;
    if (IsSHIFTPressed()) nOffset = 1;

    if(nChar == VK_DOWN)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_VERT, pt.y+nOffset);

    } else if(nChar == VK_UP)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_VERT, pt.y-nOffset);
    } else if(nChar == VK_RIGHT)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_HORZ,pt.x+nOffset);

    } else if(nChar == VK_LEFT)
    {
        CPoint pt=GetScrollPosition();
        SetScrollPos(SB_HORZ,pt.x-nOffset);
    }

    CDrawView::OnKeyDown(nChar, nRepCnt, nFlags);

    OnUpdate(NULL,NULL,NULL);
}


BOOL CRoilibAppView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // Ctrl+Wheel
    if ((nFlags & MK_CONTROL) != 0) {
        //TRACE("Wheel : zDelta=%d, pt=%d,%d\n", zDelta, pt.x, pt.y);
        if (zDelta > 0)
            OnViewZoomin();
        else
            OnViewZoomout();
        return TRUE;
    }

    if (zDelta > 0)
        OnKeyDown(VK_UP, 0, 0);
    else if (zDelta < 0)
        OnKeyDown(VK_DOWN, 0, 0);
    //BOOL r = CDrawView::OnMouseWheel(nFlags, zDelta, pt);
    //Invalidate();

    return 1;
}

void CRoilibAppView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_ximage == NULL)
        return;

    // We'll get the RGB values at the point the user selects
    TCHAR s[256];
    long x = point.x;
    long y = point.y;
    GetImageXY(m_ximage, &x,&y);

    if (m_ximage->IsInside(x,y))
    {
        //if (CDrawTool::c_drawShape == moving)
        //	SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HANDOPEN)));  //SetCursor(LoadCursor(0,IDC_HANDOPEN));

        long yflip = m_ximage->GetHeight() - y - 1;
        _stprintf(s,_T("x: %d y: %d  idx: %d"), x, y, m_ximage->GetPixelIndex(x,yflip));
        RGBQUAD rgb=m_ximage->GetPixelColor(x,yflip);
        if (m_ximage->AlphaIsValid()) rgb.rgbReserved=m_ximage->AlphaGet(x,yflip);
        else rgb.rgbReserved=m_ximage->GetPaletteColor(m_ximage->GetPixelIndex(x,yflip)).rgbReserved;
        _stprintf(&s[_tcslen(s)],_T("  RGBA: (%d, %d, %d, %d)"), rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue, rgb.rgbReserved);

        if (CDrawTool::c_drawShape == tracker)
        {
            _stprintf(&s[_tcslen(s)], _T("  Width/Height : %.0f/%.0f"),
                m_tracker.m_rect.Width()*(1 / GetZoomFactor()), m_tracker.m_rect.Height()*(1 / GetZoomFactor()));
        }
    } else {
        _tcscpy(s,_T(" "));
            //SetCursor(LoadCursor(0,IDC_ARROW));
    }

    CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    if (pMainFrame != NULL) {
        CMFCRibbonStatusBar& statusBar = pMainFrame->GetStatusBar();
        statusBar.GetElement(0)->SetText(s);
        statusBar.Invalidate();
    }

    //CDrawTool::c_drawShape = moving; // RoilibAppView는 이미지 이동만 지원.
    CDrawView::OnMouseMove(nFlags, point);
}

void CRoilibAppView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
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


void CRoilibAppView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
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

//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::GetImageXY(CxImage *ima, long *x, long *y)
{
    if (!ima) return;

    CPoint point=GetScrollPosition();
    double fx =(double)(*x + point.x);
    double fy =(double)(*y + point.y);

    fx/=GetZoomFactor();
    fy/=GetZoomFactor();

    *x = (long)fx;
    *y = (long)fy;
}
void CRoilibAppView::GetImageXY(CxImage *ima, long *x, long *y, double zf)
{
    if (!ima) return;

    CPoint point=GetScrollPosition();
    double fx =(double)(*x + point.x);
    double fy =(double)(*y + point.y);

    fx/=zf;
    fy/=zf;

    *x = (long)fx;
    *y = (long)fy;
}

void CRoilibAppView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if (theApp.m_nSelectCh < 0)
        return;
    //CMainFrame *pMainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);

    switch (lHint)
    {
    case WM_USER_NEWIMAGE:
        {

            CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetDocument();
            if (pDoc)	pDoc->UpdateStatusBar();

            //CxImage*  ima  = pDoc->GetImage();
            if (m_ximage) {
                int px=GetScrollPos(SB_HORZ);
                int py=GetScrollPos(SB_VERT);
                CSize sz(GetTotalSize()); // 현재 Display 되어 있는 전체 Size
                int x=(int)(m_ximage->GetWidth()*GetZoomFactor());
                int y=(int)(m_ximage->GetHeight()*GetZoomFactor());
                SetScrollSizes(MM_TEXT,	CSize(x,y));
                CSize sz2(GetTotalSize()); // ZoomFactor 적용후 전체 Size

                //CWnd* pFrame=GetParentFrame();
                CRect rClient;
                GetClientRect(&rClient);

                // Zoom이 변할때 한번만 계산하면 된다.
                long x1 = 0; long y1 = 0;	GetImageXY(m_ximage, &x1,&y1);
                long x2 = (rClient.right - rClient.left); long y2 = (rClient.bottom - rClient.top);
                GetImageXY(m_ximage, &x2,&y2); // 실제 이미지가 표시되는 영역을 얻어옴.

                double zf = GetOldZoomFactor(); SetZoomFactor(GetZoomFactor());
                long x3 = 0; long y3 = 0;	GetImageXY(m_ximage, &x3,&y3, zf);
                long x4 = (rClient.right - rClient.left); long y4 = (rClient.bottom - rClient.top);
                GetImageXY(m_ximage, &x4,&y4, zf); // Zoom 이전에 이미지가 표시되는 영역을 얻어옴.

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
                if (!m_ximage->SelectionIsValid()) KillTimer(1);

                Invalidate();
            }
        }

        break;

    default:
        if (m_ximage){
            SetScrollSizes(MM_TEXT,CSize((int)(m_ximage->GetWidth()*GetZoomFactor()),
                                        (int)(m_ximage->GetHeight()*GetZoomFactor())));
        }
    }

    CDrawView::OnUpdate(pSender, lHint, pHint);

}

void CRoilibAppView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
    if (pActivateView)
    {
        CRoilibAppDoc* pDoc = (CRoilibAppDoc*)GetDocument();
        pDoc->m_pActiveMainView = (CRoilibAppView*)this;
        pDoc->UpdateStatusBar();
    }

    CDrawView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::MoveViewImage(CPoint delta, double r, BOOL bAbs)
{
    CxImage* ima = m_ximage;// pDoc->GetImage();
    long x = (long)((1.0 / r) * delta.x);
    long y = (long)((1.0 / r) * delta.y);
    //GetImageXY(pDoc, ima, &x,&y);

    if (bAbs) {
        SetScrollPos(SB_HORZ, 0);
        SetScrollPos(SB_VERT, 0);
    }

    m_RefScroll = GetScrollPosition();

    CSize sz(GetTotalSize());
    //CWnd* pMainFrame=GetParentFrame();
    //RECT rClient;
    //pMainFrame->GetClientRect(&rClient);
    CRect rect;
    GetClientRect(&rect);
    if (sz.cx>rect.right) SetScrollPos(SB_HORZ, m_RefScroll.x + x); else SetScrollPos(SB_HORZ, 0);
    if (sz.cy>rect.bottom) SetScrollPos(SB_VERT, m_RefScroll.y + y); else SetScrollPos(SB_VERT, 0);

    Invalidate();
}

//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::MoveCenterPosition(CPoint pt)
{
    CRoilibAppDoc* pDoc = GetDocument();

    CSize sz(GetTotalSize());
    //CWnd* pMainFrame=GetParentFrame();
    CRect rect;
    GetClientRect(&rect);
    //pMainFrame->GetClientRect(rect);
    pt.x *= GetZoomFactor();
    pt.y *= GetZoomFactor();

    pt.x -= rect.Width() / 2;
    pt.y -= rect.Height() / 2;

    if (sz.cx>pt.x)
        SetScrollPos(SB_HORZ, pt.x);
    else SetScrollPos(SB_HORZ, 0);

    if (sz.cy>pt.y)
        SetScrollPos(SB_VERT, pt.y);
    else SetScrollPos(SB_VERT, 0);

    Invalidate();
}

//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::MoveBottomLeftPosition(CPoint pt)
{
    CRoilibAppDoc* pDoc = GetDocument();

    CSize sz(GetTotalSize());
    CRect rect;
    GetClientRect(&rect);
    pt.x *= GetZoomFactor();
    pt.y *= GetZoomFactor();

    pt.x -= 20;
    pt.y -= (rect.Height() - 20);

    if (sz.cx>pt.x) SetScrollPos(SB_HORZ, pt.x); else SetScrollPos(SB_HORZ, 0);
    if (sz.cy>pt.y) SetScrollPos(SB_VERT, pt.y); else SetScrollPos(SB_VERT, 0);

    Invalidate();
}


//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnViewToolsMove()
{
    CDrawTool::c_drawShape = moving;
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnUpdateViewToolsMove(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == moving);
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnViewToolsSelect()
{
    CDrawTool::c_drawShape = selection;
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnUpdateViewToolsSelect(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == selection);
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnViewToolsTracker()
{
    CDrawTool::c_drawShape = tracker;
}
//////////////////////////////////////////////////////////////////////////////
void CRoilibAppView::OnUpdateViewToolsTracker(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == tracker);
}
void CRoilibAppView::OnViewToolsMeasure()
{
    CDrawTool::c_drawShape = DrawShape::line;
}
void CRoilibAppView::OnUpdateViewToolsMeasure(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(CDrawTool::c_drawShape == DrawShape::line);
}


void CRoilibAppView::OtherObjectsClear(DWORD dwFilter)
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

void CRoilibAppView::Draw(CDC* pDC, CDrawView* pView)
{
    //CDrawView::Draw(pDC, pView);
    POSITION pos = m_otherObjects.GetHeadPosition();
    while (pos != NULL)
    {
        CDrawObj* pObj = m_otherObjects.GetNext(pos);
        pObj->Draw(pDC);
    }
}


void CRoilibAppView::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDrawView::OnShowWindow(bShow, nStatus);

    if (bShow) {
        OnViewToolsMove();

        theApp.m_bPreviewMode = TRUE;
    }
}


void CRoilibAppView::OnButtonPreview()
{
    theApp.m_bPreviewMode = TRUE;// !theApp.m_bPreviewMode;
}


void CRoilibAppView::OnButtonStop()
{
    theApp.m_bPreviewMode = FALSE;// !theApp.m_bPreviewMode;
}


void CRoilibAppView::OnUpdateButtonPreview(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(!theApp.m_bPreviewMode);
}


void CRoilibAppView::OnUpdateButtonStop(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(theApp.m_bPreviewMode);
}


void CRoilibAppView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CDrawView::OnLButtonDown(nFlags, point);
}


void CRoilibAppView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CDrawView::OnLButtonUp(nFlags, point);
}

