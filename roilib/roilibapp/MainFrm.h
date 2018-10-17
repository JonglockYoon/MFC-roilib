#pragma once

//
// Copyright (C) 2018
// All rights reserved by jerry1455@gmail.com
//

// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once
#include "RoilibApp.h"
#include "LogWnd.h"
#include "PropertiesWnd.h"
#include "CustomRibbonBar.h"
#include "RoilibAppView.h"
#include "IniUtil.h"
#include "RecipeView.h"

class CMainFrame : public CSDIFrameWnd//CFrameWndEx
{

protected: // serialization에서만 만들어집니다.
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 재정의입니다.
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnApplicationLook(UINT id);
    afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
    afx_msg void OnFilePrint();
    afx_msg void OnFilePrintPreview();
    afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    afx_msg LRESULT Msg_Log_Display(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGrabImageUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChangeRibbonBarCategory(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChangeRibbonBarSize(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMoveViewPosition(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPropertyAdd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPropertyUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPropertyDelete(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPropertySelect(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnPropertyEdit(WPARAM wParam, LPARAM lParam);

    afx_msg void OnViewAllbar();
    afx_msg void OnClose();
    afx_msg void OnInspectionStart();
    afx_msg void OnSettingConfig();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    DECLARE_MESSAGE_MAP()

    BOOL CreateDockingWindows();
    void SetDockingWindowIcons(BOOL bHiColorIcons);

protected:  // 컨트롤 모음이 포함된 멤버입니다.
    CMFCToolBarImages m_PanelImages;
    CMFCRibbonStatusBar  m_wndStatusBar;
    CLogBarWnd        m_wndLogBar;
    int os_ver;

public:
    CCustomRibbonBar     m_wndRibbonBar;
    CPropertiesWnd    m_wndProperties;
    CRecipeView*	m_pRecipeView;			//레시피 작성을위한 View
    CRoilibAppView* m_pMainView[MAX_CH_NUM];	// 2개의 이미지 그랩View를 가짐. OnTimer에의해 연속적으로 Update된다.

    vector<CAMINFO> vecCamList;
    BOOL m_bMultiNetConnected;
    ULONGLONG m_oTime;
    BOOL m_bGrabON;

public:
    inline CMFCRibbonStatusBar&	  GetStatusBar() { return m_wndStatusBar; }
    void ToggleViewAllbar(BOOL bVisible);

    void UpdateRoiDataInfo(CDrawObj *pObj, CRoiData* pRoiData);
    void DeleteReceipe(CRoiData* pData);
    void UpdateReceipe(CRoiData* pData);

    BOOL RecipeLoad();
    HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum);
    void DisplayDeviceInformation(IEnumMoniker *pEnum);
    void WebcamDeviceList();

#define LEFT 0
#define RIGHT 1
    char m_TmpBuffer[256];

    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};


