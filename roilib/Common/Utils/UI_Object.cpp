#include "StdAfx.h"
#include "UI_Object.h"

CUI_Object *g_pUiObject = NULL;

CUI_Object::CUI_Object()
{
    object_Init();
}

CUI_Object::~CUI_Object(void)
{
    object_delete();
}


void CUI_Object::object_Init(){

    brush_orange.CreateSolidBrush(RGB(255,128,0));
    brush_pink.CreateSolidBrush(RGB(252,194,232));
    brush_white.CreateSolidBrush(RGB(255,255,255));
    brush_black.CreateSolidBrush(RGB(0,0,0));
    brush_yellow.CreateSolidBrush(RGB(255,255,150));
    brush_skygreen.CreateSolidBrush(RGB(180,250,200));
    brush_gray.CreateSolidBrush(RGB(190,190,190));
    //brush_default.CreateSolidBrush(RGB(236,233,216));
    brush_default.CreateSolidBrush(RGB(212,208,200));
    brush_lightgray.CreateSolidBrush(RGB(40,40,40));
    brush_darkgray.CreateSolidBrush(RGB(20,20,20));

    brush_g_skygreen.CreateSolidBrush(RGB(210,250,220));

    brush_pastel_b1.CreateSolidBrush(RGB(206,206,239));
    brush_pastel_b2.CreateSolidBrush(RGB(235,235,235));
    //brush_pastel_b3.CreateSolidBrush(RGB(130,13,13));
    brush_pastel_b3.CreateSolidBrush(RGB(128,72,72));

    brush_red.CreateSolidBrush(RGB(255,25,25));
    brush_green.CreateSolidBrush(RGB(25,255,25));

    brush_darkgreen.CreateSolidBrush(RGB(0,85,40));


    font_70.CreatePointFont(70, _T("Arial Unicode MS 보통"));
    font_80.CreatePointFont(80, _T("Arial Unicode MS 보통"));
    font_90.CreatePointFont(90, _T("Arial Unicode MS 보통"));
    font_100.CreatePointFont(100, _T("Arial Unicode MS 보통"));
    font_120.CreatePointFont(120, _T("Arial Unicode MS 보통"));
    font_150.CreatePointFont(150, _T("Arial Unicode MS 보통"));
    font_200.CreatePointFont(200, _T("Arial Unicode MS 보통"));
    font_250.CreatePointFont(250, _T("Arial Unicode MS 보통"));
    font_300.CreatePointFont(300, _T("Arial Unicode MS 보통"));
    font_350.CreatePointFont(350, _T("Arial Unicode MS 보통"));
    font_400.CreatePointFont(400, _T("Arial Unicode MS 보통"));
    font_600.CreatePointFont(600, _T("Arial Unicode MS 보통"));
    font_1000.CreatePointFont(1000, _T("Arial Unicode MS 보통"));

    lightgray_pen.CreatePen(PS_SOLID, 1, RGB(40,40, 40));
    white_pen.CreatePen(PS_SOLID, 1, RGB(255,255, 255));
    green_pen.CreatePen(PS_SOLID, 1, RGB(55,255, 55));
    blue_pen.CreatePen(PS_SOLID, 1, RGB(55,55, 255));
    red_pen.CreatePen(PS_SOLID, 1, RGB(255,55, 55));
    black_pen.CreatePen(PS_SOLID, 1, RGB(0,0, 0));

}
void CUI_Object::object_delete(){
    brush_orange.DeleteObject();
    brush_pink.DeleteObject();
    brush_white.DeleteObject();
    brush_black.DeleteObject();
    brush_yellow.DeleteObject();
    brush_skygreen.DeleteObject();
    brush_gray.DeleteObject();
    brush_default.DeleteObject();
    brush_lightgray.DeleteObject();
    brush_darkgray.DeleteObject();
    brush_g_skygreen.DeleteObject();

    brush_pastel_b1.DeleteObject();
    brush_pastel_b2.DeleteObject();
    brush_pastel_b3.DeleteObject();

    brush_red.DeleteObject();
    brush_green.DeleteObject();
    brush_darkgreen.DeleteObject();


    font_70.DeleteObject();
    font_80.DeleteObject();
    font_90.DeleteObject();
    font_100.DeleteObject();
    font_120.DeleteObject();
    font_150.DeleteObject();
    font_200.DeleteObject();
    font_250.DeleteObject();

    font_300.DeleteObject();
    font_350.DeleteObject();
    font_400.DeleteObject();
    font_600.DeleteObject();
    font_1000.DeleteObject();

    lightgray_pen.DeleteObject();
    white_pen.DeleteObject();
    green_pen.DeleteObject();
    blue_pen.DeleteObject();
    red_pen.DeleteObject();
    black_pen.DeleteObject();
}

BOOL CUI_Object::AllocUserFont(int nBarWidth, int nBarHeight, CString sData, CFont **pFont){
    //사이즈에 맞게 글자 크기 맞추기//
    CString s;
    int nLen;
    LONG	nWidth;
    double dRatio;

    nLen = sData.GetLength();

    LOGFONT	lf;
    ZeroMemory(&lf, sizeof(LOGFONT));
    lf.lfWeight	= FW_BOLD;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfQuality	= DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH;
    _tcscpy(lf.lfFaceName, _T("Arial Unicode MS 보통"));
    lf.lfHeight = nBarHeight;
    nWidth = (nBarHeight * nLen);
    dRatio = (double)nBarWidth / nWidth;
    lf.lfWidth = nBarHeight * dRatio;
    //////////////////////////////////

    *pFont = new CFont();
    (*pFont)->CreateFontIndirect(&lf);

    return TRUE;
}
BOOL CUI_Object::FreeUserFont(CFont **pFont){
    (*pFont)->DeleteObject();
    delete *pFont;
    return TRUE;
}
