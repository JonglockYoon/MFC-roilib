#pragma once



class MyCMFCRibbonCategory : public CMFCRibbonCategory
{
public:
	void force_ShowElements(BOOL todo)
	{
		ShowElements(todo);
	}
};

// CCustomRibbonBar

class CCustomRibbonBar : public CMFCRibbonBar
{
	DECLARE_DYNAMIC(CCustomRibbonBar)

public:
	CCustomRibbonBar();
	virtual ~CCustomRibbonBar();

public:
	BOOL is_minimized()
	{
		return m_dwHideFlags == AFX_RIBBONBAR_HIDE_ELEMENTS;
	}

	void minimize_me(BOOL show_minimized)
	{
		MyCMFCRibbonCategory* cc = (MyCMFCRibbonCategory*)GetActiveCategory();
		if (cc != NULL)
		{
			cc->force_ShowElements(!show_minimized);
			RedrawWindow();
		}
	}


protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

    int m_LastActiveCategory;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


