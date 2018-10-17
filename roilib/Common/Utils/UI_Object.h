#pragma once

class CUI_Object
{
public:
	//CUI_Object(void);
	CUI_Object();
	~CUI_Object(void);

	//================== font / static =================//
	CFont m_user_font[32];
	LOGFONT m_user_font_info[32];
	long m_user_font_count;

	CFont font_70;
	CFont font_80;
	CFont font_90;
	CFont font_100;
	CFont font_120;
	CFont font_150;
	CFont font_200;
	CFont font_250;
	CFont font_300;
	CFont font_350;
	CFont font_400;
	CFont font_600;
	CFont font_1000;

	CFont *font_basic;
	bool first_disp;
	CBrush brush_orange;
	CBrush brush_pink;
	CBrush brush_white;
	CBrush brush_black;
	CBrush brush_yellow;
	CBrush brush_skygreen;
	CBrush brush_gray;
	CBrush brush_lightgray;
	CBrush brush_darkgray;
	CBrush brush_default;
	CBrush brush_g_skygreen;

	CBrush brush_red;
	CBrush brush_green;
	CBrush brush_darkgreen;

	CBrush brush_pastel_b1;
	CBrush brush_pastel_b2;
	CBrush brush_pastel_b3;

	CPen lightgray_pen;
	CPen white_pen;
	CPen green_pen;
	CPen blue_pen;
	CPen red_pen;
	CPen black_pen;


	void object_Init();
	void object_delete();
	void Menu_Font_Setting(long id, CFont * font);

	BOOL AllocUserFont(int nBarWidth, int nBarHeight, CString sData, CFont **pFont);
	BOOL FreeUserFont(CFont **pFont);

	//=================================================//

	int Get_User_Font(int f_height, int f_width, int f_weight, bool f_italic, CString font_type);
};
extern CUI_Object *g_pUiObject;